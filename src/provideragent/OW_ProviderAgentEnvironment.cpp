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
#include "OW_ProviderAgentEnvironment.hpp"
#include "OW_ProviderAgentCIMOMHandle.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_Assertion.hpp"

namespace OpenWBEM
{

ProviderAgentEnvironment::ProviderAgentEnvironment(Map<String,String> configMap,
		Reference<CppProviderBaseIFC> provider, 
		Reference<AuthenticatorIFC> authenticator,
		Array<RequestHandlerIFCRef> requestHandlers, 
		LoggerRef logger,
		Reference<Array<SelectablePair_t> > selectables)
	: m_authenticator(authenticator)
	, m_logger(logger ? logger : LoggerRef(new DummyLogger))
	, m_requestHandlers(requestHandlers)
	, m_selectables(selectables)
	, m_prov(provider)
	, m_configItems(configMap)
{
}

//////////////////////////////////////////////////////////////////////////////
ProviderAgentEnvironment::~ProviderAgentEnvironment() {}


//////////////////////////////////////////////////////////////////////////////
bool 
ProviderAgentEnvironment::authenticate(String &userName,
		const String &info, String &details, OperationContext& context)
{
	return m_authenticator->authenticate(userName, info, details, context);
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentEnvironment::addSelectable(const SelectableIFCRef& obj,
		const SelectableCallbackIFCRef& cb)
{
	m_selectables->push_back(std::make_pair(obj, cb));
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentEnvironment::removeSelectable(const SelectableIFCRef& obj)
{
	m_selectables->erase(std::remove_if(m_selectables->begin(), m_selectables->end(),
		selectableFinder(obj)), m_selectables->end());
}
//////////////////////////////////////////////////////////////////////////////
String 
ProviderAgentEnvironment::getConfigItem(const String &name, const String& defRetVal) const
{
	Map<String, String>::const_iterator i =
		m_configItems.find(name);
	if (i != m_configItems.end())
	{
		return (*i).second;
	}
	else
	{
		return defRetVal;
	}
}
//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentEnvironment::setConfigItem(const String& item, const String& value, EOverwritePreviousFlag overwritePrevious)
{
	if (overwritePrevious == E_OVERWRITE_PREVIOUS || getConfigItem(item) == "")
		m_configItems[item] = value;
}
	
//////////////////////////////////////////////////////////////////////////////
RequestHandlerIFCRef 
ProviderAgentEnvironment::getRequestHandler(const String& ct)
{
	for (Array<RequestHandlerIFCRef>::const_iterator iter = m_requestHandlers.begin(); 
		  iter != m_requestHandlers.end(); ++iter)
	{
		StringArray sa = (*iter)->getSupportedContentTypes(); 
		if (find(sa.begin(), sa.end(), ct) != sa.end())
		{
			return *iter; 
		}
	}
	return RequestHandlerIFCRef(SharedLibraryRef(0), 0);  //TODO need to throw? 
}
//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef 
ProviderAgentEnvironment::getCIMOMHandle(OperationContext&,
		ESendIndicationsFlag /*doIndications*/,
		EBypassProvidersFlag /*bypassProviders*/)
{
	return CIMOMHandleIFCRef(new ProviderAgentCIMOMHandle(m_prov)); 
}
//////////////////////////////////////////////////////////////////////////////
LoggerRef 
ProviderAgentEnvironment::getLogger() const
{
	return m_logger;
}

//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray 
ProviderAgentEnvironment::getInteropInstances(const String& className)const 
{
	OW_ASSERT("not implemented" == 0); 
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentEnvironment::setInteropInstance(const CIMInstance& inst)
{
	OW_ASSERT("not implemented" == 0); 
}

//////////////////////////////////////////////////////////////////////////////

} // end namespace OpenWBEM

