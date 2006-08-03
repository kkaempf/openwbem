/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_SPNEGOAuthentication.hpp"
#include "OW_Process.hpp"
#include "OW_Exec.hpp"
#include "OW_MutexLock.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_Exception.hpp"
#include "OW_FileSystem.hpp"
#include "OW_IOException.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(SPNEGOAuthentication)
OW_DEFINE_EXCEPTION(SPNEGOAuthentication)

namespace
{
	String COMPONENT_NAME("ow.SPNEGOAuthentication");
}

SPNEGOAuthentication::SPNEGOAuthentication()
	: m_logger(COMPONENT_NAME)
{

}

SPNEGOAuthentication::~SPNEGOAuthentication() 
{
	try
	{
		if (m_vasHelper)
		{
			m_vasHelper->waitCloseTerm(Timeout::relative(0), Timeout::relative(0.01), Timeout::relative(0.02));
		}
	}
	catch (...)
	{
		// eat it.
	}
}

const String NegotiateSTR("Negotiate ");

EAuthenticateResult 
SPNEGOAuthentication::authenticate(String& userName,
	const String& info, HTTPSvrConnection* htcon)
{
	if (info.indexOf('\n') != String::npos)
	{
		htcon->setErrorDetails("Newline not allowed");
		return E_AUTHENTICATE_FAIL;
	}


	if (!info.startsWith(NegotiateSTR))
	{
		htcon->setErrorDetails("Internal error. !info.startsWith(\"Negotiate: \")");
		OW_LOG_DEBUG(m_logger, Format("Error, expected info to begin with \"Negotiate \", but it is: \"%1\"", info));
		return E_AUTHENTICATE_FAIL;
	}

	String info2 = info.substring(NegotiateSTR.length());

	MutexLock lock(m_guard);
	checkProcess();
	
	try
	{

		IOIFCStreamBuffer inbuf(m_vasHelper->out().getPtr());
		IOIFCStreamBuffer outbuf(m_vasHelper->in().getPtr());
		std::istream istr(&inbuf);
		std::ostream ostr(&outbuf);
		istr.tie(&ostr);
		ostr << info2 << '\n';
		OW_LOG_DEBUG(m_logger, Format("SPNEGOAuthentication got request, sending to helper: %1", info2));
		ostr << htcon->getConnectionId() << std::endl;
		OW_LOG_DEBUG(m_logger, Format("SPNEGOAuthentication sending connection id: %1", htcon->getConnectionId()));
		String result = String::getLine(istr);
		OW_LOG_DEBUG(m_logger, Format("SPNEGOAuthentication got response: %1", result));
		if (result == "S")
		{
			userName = String::getLine(istr);
			String challenge = String::getLine(istr);
			htcon->addHeader("WWW-Authenticate", "Negotiate " + challenge);
			OW_LOG_DEBUG(m_logger, Format("SPNEGOAuthentication got success. username: %1", userName));
			return E_AUTHENTICATE_SUCCESS;
		}
		else if (result == "F")
		{
			String details = String::getLine(istr);
			htcon->setErrorDetails(details);
			return E_AUTHENTICATE_FAIL;
		}
		else if (result == "C")
		{
			String challenge = String::getLine(istr);
			htcon->addHeader("WWW-Authenticate", "Negotiate " + challenge);
			htcon->setErrorDetails("SPNEGO continue");
			return E_AUTHENTICATE_CONTINUE;
		}
		else
		{
			// something has gone horribly wrong. This shouldn't ever happen unless there is a bug.
			OW_LOG_ERROR(m_logger, Format("SPNEGOAuthentication received unknown response (%1) from %2. Terminating.", result, vasHelperPath()));
			m_vasHelper->waitCloseTerm(Timeout::relative(0), Timeout::relative(0.01), Timeout::relative(0.02));
		}
	}
	catch (IOException& e)
	{
		htcon->setErrorDetails(Format("Error communicating with %1: %2", vasHelperPath(), e));
	}
	
	return E_AUTHENTICATE_FAIL;
}

String
SPNEGOAuthentication::getChallenge()
{
	return "Negotiate";
}

void
SPNEGOAuthentication::checkProcess()
{
	// precondition: m_guard is locked.

	if (m_vasHelper && m_vasHelper->processStatus().running())
	{
		return;
	}

	if (m_vasHelper)
	{
		// must have died
		OW_LOG_ERROR(m_logger, Format("SPNEGOAuthentication Detected that %1 is not running. Status: %2", vasHelperPath(), m_vasHelper->processStatus().toString()));
		m_vasHelper->waitCloseTerm(0.00, 0.01, 0.02);
		m_vasHelper = 0;
	}
	
	PrivilegeManager privman = PrivilegeManager::getPrivilegeManager();
	OW_ASSERT(!privman.isNull());
	String helperPath(vasHelperPath());
	if (helperPath.empty())
	{
		const char* msg = "SPNEGOAuthentication unable to locate libvas";
		OW_LOG_ERROR(m_logger, msg);
		OW_THROW(SPNEGOAuthenticationException, msg);
	}

	StringArray helperArgv(1, helperPath);
	helperArgv.push_back("server");
	m_vasHelper = privman.userSpawn(helperPath, helperArgv, Exec::currentEnvironment, "root");

	if (!m_vasHelper->processStatus().running())
	{
		String msg = Format("SPNEGOAuthentication failed to start %1. status = %2, stderr = %3", vasHelperPath(), 
			m_vasHelper->processStatus().toString(), m_vasHelper->err()->readAll());
		OW_LOG_ERROR(m_logger, msg);
		OW_THROW(SPNEGOAuthenticationException, msg.c_str());
	}

	Timeout to(Timeout::relative(5));
	m_vasHelper->in()->setTimeouts(to);
	m_vasHelper->out()->setTimeouts(to);
	m_vasHelper->err()->setTimeouts(to);
}

String 
SPNEGOAuthentication::vasHelperPath()
{
	if (FileSystem::exists("/opt/quest/lib/libvas.so.4"))
	{
		return ConfigOpts::installed_owlibexec_dir + "/owspnegovas4helper";
	}
	else if (FileSystem::exists("/opt/vas/lib/libvas.so.3"))
	{
		return ConfigOpts::installed_owlibexec_dir + "/owspnegovas3helper";
	}
	return "";
}

} // end namespace OW_NAMESPACE


