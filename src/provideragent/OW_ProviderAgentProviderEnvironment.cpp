/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ProviderAgentProviderEnvironment.hpp"
#include "OW_Assertion.hpp"
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_Logger.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_ProviderEnvironmentException.hpp"

namespace OW_NAMESPACE
{

//////////////////////////////////////////////////////////////////////////////
ProviderAgentProviderEnvironment::ProviderAgentProviderEnvironment(
	const ConfigFile::ConfigMap& configMap,
	OperationContext& operationContext,
	const String& callbackURL,
	ClientCIMOMHandleConnectionPool& pool,
	ProviderAgentEnvironment::EConnectionCredentialsUsageFlag useConnectionCredentials)
	: m_configMap(configMap)
	, m_operationContext(operationContext)
	, m_callbackURL(callbackURL)
	, m_connectionPool(pool)
	, m_CIMOMHandleRA()
	, m_useConnectionCredentials(useConnectionCredentials)
{
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgentProviderEnvironment::~ProviderAgentProviderEnvironment()
{
	for (Array<ClientCIMOMHandleRef>::const_iterator iter = m_CIMOMHandleRA.begin();
		  iter < m_CIMOMHandleRA.end(); ++iter)
	{
		m_connectionPool.addConnectionToPool(*iter, m_callbackURL);
	}
}

//////////////////////////////////////////////////////////////////////////////
// This function returns a regular cimom handle that does access checking and may call providers.
CIMOMHandleIFCRef
ProviderAgentProviderEnvironment::getCIMOMHandle() const
{
	if (m_callbackURL.empty())
	{
		return CIMOMHandleIFCRef(0);
	}

	String callbackURL(m_callbackURL);
	if (m_useConnectionCredentials)
	{
		URL url(m_callbackURL);
		try
		{
			url.principal = m_operationContext.getStringData(OperationContext::USER_NAME);
			url.credential = m_operationContext.getStringData(OperationContext::USER_PASSWD);
		}
		catch (ContextDataNotFoundException& e)
		{
		}
		callbackURL = url.toString();
	}

	ClientCIMOMHandleRef client = m_connectionPool.getConnection(callbackURL);

	m_CIMOMHandleRA.push_back(client);
	return client;
}
//////////////////////////////////////////////////////////////////////////////
String
ProviderAgentProviderEnvironment::getConfigItem(const String &name, const String &defRetVal) const
{
	return ConfigFile::getConfigItem(m_configMap, name, defRetVal);
}

//////////////////////////////////////////////////////////////////////////////
StringArray
ProviderAgentProviderEnvironment::getMultiConfigItem(const String &itemName, 
	const StringArray& defRetVal, const char* tokenizeSeparator) const
{
	return ConfigFile::getMultiConfigItem(m_configMap, itemName, defRetVal, tokenizeSeparator);
}

//////////////////////////////////////////////////////////////////////////////
// This function returns a cimom handle that directly accesses the repository (CIMServer is bypassed).
// no providers will be called.  This function should only be called if getCIMOMHandle()
// is insufficent.
CIMOMHandleIFCRef
ProviderAgentProviderEnvironment::getRepositoryCIMOMHandle() const
{
	OW_ASSERTMSG(0, "not implemented");
	return CIMOMHandleIFCRef();
}
//////////////////////////////////////////////////////////////////////////////
// This function returns a reference to the repository.  This function should only
// be called if getCIMOMHandle() and getRepositoryCIMOMHandle() are insufficient.
RepositoryIFCRef
ProviderAgentProviderEnvironment::getRepository() const
{
	OW_ASSERTMSG(0, "not implemented");
	return RepositoryIFCRef();
}
RepositoryIFCRef
ProviderAgentProviderEnvironment::getAuthorizingRepository() const
{
	OW_ASSERTMSG(0, "not implemented");
	return RepositoryIFCRef();
}
//////////////////////////////////////////////////////////////////////////////
String
ProviderAgentProviderEnvironment::getUserName() const
{
	OW_ASSERTMSG(0, "not implemented");
	return String();
}
//////////////////////////////////////////////////////////////////////////////
OperationContext&
ProviderAgentProviderEnvironment::getOperationContext()
{
	return m_operationContext;
}

//////////////////////////////////////////////////////////////////////////////
ProviderEnvironmentIFCRef
ProviderAgentProviderEnvironment::clone() const
{
	OW_ASSERTMSG(0, "not implemented");
	return ProviderEnvironmentIFCRef();
}

} // end namespace OW_NAMESPACE

