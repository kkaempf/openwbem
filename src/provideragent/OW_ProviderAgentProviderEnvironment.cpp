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

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
ProviderAgentProviderEnvironment::ProviderAgentProviderEnvironment(LoggerRef logger, 
								 ConfigFile::ConfigMap configMap,
								 OperationContext& operationContext, 
								 const String& callbackURL)
	: m_logger(logger)
	, m_configMap(configMap)
	, m_operationContext(operationContext)
	, m_callbackURL(callbackURL)
{
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
	CIMOMHandleIFCRef rval = ClientCIMOMHandle::createFromURL(m_callbackURL); 
	return rval; 
}
//////////////////////////////////////////////////////////////////////////////
String 
ProviderAgentProviderEnvironment::getConfigItem(const String &name, const String &defRetVal) const
{
	Map<String, String>::const_iterator i =
		m_configMap.find(name);
	if (i != m_configMap.end())
	{
		return (*i).second;
	}
	else
	{
		return defRetVal;
	}
}

//////////////////////////////////////////////////////////////////////////////
// This function returns a cimom handle that directly accesses the repository (CIMServer is bypassed).
// no providers will be called.  This function should only be called if getCIMOMHandle()
// is insufficent.
CIMOMHandleIFCRef 
ProviderAgentProviderEnvironment::getRepositoryCIMOMHandle() const
{
	OW_ASSERT("not implemented" == 0); 
}
//////////////////////////////////////////////////////////////////////////////
// This function returns a reference to the repository.  This function should only
// be called if getCIMOMHandle() and getRepositoryCIMOMHandle() are insufficient.
RepositoryIFCRef 
ProviderAgentProviderEnvironment::getRepository() const
{
	OW_ASSERT("not implemented" == 0); 
}
//////////////////////////////////////////////////////////////////////////////
LoggerRef 
ProviderAgentProviderEnvironment::getLogger() const
{
	return m_logger; 
}
//////////////////////////////////////////////////////////////////////////////
String 
ProviderAgentProviderEnvironment::getUserName() const
{
	OW_ASSERT("not implemented" == 0); 
}
//////////////////////////////////////////////////////////////////////////////
OperationContext& 
ProviderAgentProviderEnvironment::getOperationContext()
{
	return m_operationContext; 
}

//////////////////////////////////////////////////////////////////////////////

} // end namespace OpenWBEM

