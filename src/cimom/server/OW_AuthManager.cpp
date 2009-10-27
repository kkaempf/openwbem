/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_AuthManager.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/Logger.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_SafeLibCreate.hpp"
#include "blocxx/ThreadCancelledException.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include "OW_ServiceIFCNames.hpp"
// dumb aCC requires these to build
#include "blocxx/Array.hpp"

namespace OW_NAMESPACE
{

using namespace blocxx;

OW_DEFINE_EXCEPTION_WITH_ID(AuthManager)

namespace
{
	const String COMPONENT_NAME("ow.owcimomd.AuthManager");
}

///////////////////////////////////////////////////////////////////////////////
AuthManager::AuthManager()
	: m_authenticator()
{
}
///////////////////////////////////////////////////////////////////////////////
AuthManager::~AuthManager()
{
}
///////////////////////////////////////////////////////////////////////////////
String
AuthManager::getName() const
{
	return ServiceIFCNames::AuthManager;
}
///////////////////////////////////////////////////////////////////////////////
void
AuthManager::init(const ServiceEnvironmentIFCRef& env)
{
	Logger logger(COMPONENT_NAME);
	String authLib = env->getConfigItem(ConfigOpts::AUTHENTICATION_MODULE_opt);
	if (authLib.empty())
	{
		BLOCXX_LOG_INFO(logger, "Authentication Manager: No Basic Authentication module configured.");
		return;
	}

	BLOCXX_LOG_INFO(logger, Format("Authentication Manager: Loading"
		" authentication module %1", authLib));
	m_authenticator =
		SafeLibCreate<AuthenticatorIFC>::loadAndCreateObject(authLib, "createAuthenticator", OW_VERSION);
	if (m_authenticator)
	{
		try
		{
			m_authenticator->init(env);
			BLOCXX_LOG_INFO(logger, Format("Authentication module %1"
				" is now being used for authentication to the CIMOM",
				authLib));
		}
		catch(Exception& e)
		{
			BLOCXX_LOG_FATAL_ERROR(logger, Format("Basic Authentication Module %1 failed"
				" to initialize: %2 - %3"
				" [No Basic Authentication Mechanism Available!]", authLib, e.type(),
				e.getMessage()));
			OW_THROW(AuthManagerException, "No Basic Authentication Mechanism Available");
		}
		catch (ThreadCancelledException&)
		{
			throw;
		}
		catch(...)
		{
			BLOCXX_LOG_FATAL_ERROR(logger, Format("Basic Authentication Module %1 failed"
				" to initialize: Unknown Exception Caught"
				" [No Basic Authentication Mechanism Available!]", authLib));
			OW_THROW(AuthManagerException, "No Basic Authentication Mechanism Available");
		}
	}
	else
	{
		BLOCXX_LOG_FATAL_ERROR(logger, Format("Basic Authentication Module %1 failed"
			" to produce authentication module"
			" [No Authentication Mechanism Available!]", authLib));
		OW_THROW(AuthManagerException, "No Basic Authentication Mechanism Available");
	}
}
///////////////////////////////////////////////////////////////////////////////
bool
AuthManager::authenticate(String& userName,
	const String& info, String& details, OperationContext& context)
{
	if (m_authenticator)
	{
		return m_authenticator->authenticate(userName, info, details, context);
	}
	details = "CIMOM has no available authentication mechanism";
	return false;
}

///////////////////////////////////////////////////////////////////////////////
void
AuthManager::shutdown()
{
	m_authenticator.setNull();
}

} // end namespace OW_NAMESPACE

