/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_SocketException.hpp"

namespace OpenWBEM
{

///////////////////////////////////////////////////////////////////////////////
RequestHandlerIFC::RequestHandlerIFC()
	: m_cimError()
	, m_hasError(false)
	, m_errorCode(0)
	, m_errorDescription()
	, m_env(0)
{
}
///////////////////////////////////////////////////////////////////////////////
RequestHandlerIFC::~RequestHandlerIFC()
{
}
///////////////////////////////////////////////////////////////////////////////
void RequestHandlerIFC::process(std::istream* istr, std::ostream* ostrEntity,
	std::ostream* ostrError, const SortedVectorMap<String, String>& handlerVars)
{
	doProcess(istr, ostrEntity, ostrError, handlerVars);
}
///////////////////////////////////////////////////////////////////////////////
bool
RequestHandlerIFC::hasError(Int32& errCode, String& errDescr)
{
	if (m_hasError)
	{
		errCode = m_errorCode;
		errDescr = m_errorDescription;
	}
	return m_hasError;
}
///////////////////////////////////////////////////////////////////////////////
ServiceEnvironmentIFCRef
RequestHandlerIFC::getEnvironment() const
{
	return m_env;
}
///////////////////////////////////////////////////////////////////////////////
void
RequestHandlerIFC::setEnvironment(ServiceEnvironmentIFCRef env)
{
	m_env = env;
}
//////////////////////////////////////////////////////////////////////////////
String
RequestHandlerIFC::getHost()
{
	if (!m_cachedHost.empty())
	{
		return m_cachedHost;
	}
	try
	{
		// first try doing a DNS lookup for our full hostname
		m_cachedHost = SocketUtils::getFullyQualifiedHostName();
	}
	catch (const SocketException&)
	{
	}
	
	if (m_cachedHost.empty())
	{
		// TODO:
		// now try setting it to the ip on the incoming socket, if we got a tcp connection.
		// if the connection came in on the domain socket, just set it to the first ip we can get.
	}
	if (m_cachedHost.empty())
	{
		m_cachedHost = "localhost";
	}
	return m_cachedHost;
}

} // end namespace OpenWBEM

