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
#include "OW_ProviderAgentProviderEnvironment.hpp"
#include "OW_ProviderAgentCIMOMHandle.hpp"
#include "OW_ProviderAgent.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppSecondaryInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_MethodProviderInfo.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_ConfigException.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMInstance.hpp"

#include <algorithm> // for std::find

namespace OpenWBEM
{
namespace 
{
class DummyLogger : public Logger
{
protected:
	virtual void doLogMessage(const String &, const ELogLevel) const
	{
		return;
	}
};
}


ProviderAgentEnvironment::ProviderAgentEnvironment(const ConfigFile::ConfigMap& configMap,
		const Array<CppProviderBaseIFCRef>& providers, 
		const Array<CIMClass>& cimClasses, 
		const AuthenticatorIFCRef& authenticator,
		const Array<RequestHandlerIFCRef>& requestHandlers, 
		const LoggerRef& logger,
		const String& callbackURL, 
		const Reference<Array<SelectablePair_t> >& selectables)
	: m_configItems(configMap)
	, m_authenticator(authenticator)
	, m_logger(logger ? logger : LoggerRef(new DummyLogger))
	, m_callbackURL(callbackURL)
	, m_requestHandlers(requestHandlers)
	, m_selectables(selectables)
	, m_assocProvs()
	, m_instProvs()
	, m_secondaryInstProvs()
	, m_methodProvs()
	, m_cimClasses()
	, m_lockingType(NONE)
	, m_lockingTimeout(300)
	, m_classRetrieval(DONT_RETRIEVE_CLASSES)
	, m_connectionPool(5)
{
	m_cimClasses.setMaxCacheSize(cimClasses.size() + 100); // 100 extra just in case.
	for (Array<CIMClass>::const_iterator iter = cimClasses.begin(); 
		  iter < cimClasses.end(); ++iter)
	{
		String key = iter->getName(); 
		key.toLowerCase(); 
		m_cimClasses.addToCache(*iter, key); 
	}

	for (Array<CppProviderBaseIFCRef>::const_iterator iter = providers.begin(); 
		  iter < providers.end(); ++iter)
	{
		CppProviderBaseIFCRef prov = *iter; 

		OperationContext oc; 
		ProviderEnvironmentIFCRef pe(new ProviderAgentProviderEnvironment(m_logger, 
																		  m_configItems, 
																		  oc, 
																		  m_callbackURL, 
																		  m_connectionPool)); 
		prov->initialize(pe); 
		CppMethodProviderIFC* methodProv = prov->getMethodProvider(); 
		if (methodProv)
		{
			MethodProviderInfo info; 
			methodProv->getMethodProviderInfo(info); 
			MethodProviderInfo::ClassInfoArray cia = info.getClassInfo(); 
			for (MethodProviderInfo::ClassInfoArray::const_iterator citer  = cia.begin(); 
				  citer < cia.end(); ++citer)
			{
				const MethodProviderInfo::ClassInfo& ci = *citer; 
				String className = ci.className; 
				StringArray namespaces = ci.namespaces; 
				StringArray methods = ci.methods; 
				if (namespaces.size() == 0)
				{
					namespaces.push_back(""); 
				}
				for (StringArray::const_iterator nsiter = namespaces.begin(); 
					  nsiter < namespaces.end(); ++nsiter)
				{
					for (StringArray::const_iterator miter = methods.begin(); 
						  miter < methods.end(); ++miter)
					{
						String key = *nsiter + ":" + className + ":" + *miter; 
						key.toLowerCase(); 
						m_methodProvs[key] = *iter; 
					}
				}
			}
			m_methodProvs["*"] = *iter;
		}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		CppAssociatorProviderIFC* assocProv = prov->getAssociatorProvider(); 
		if (assocProv)
		{
			AssociatorProviderInfo api;  
			assocProv->getAssociatorProviderInfo(api); 
			AssociatorProviderInfo::ClassInfoArray cia = api.getClassInfo(); 
			for (AssociatorProviderInfo::ClassInfoArray::const_iterator citer = cia.begin(); 
				  citer < cia.end(); ++citer)
			{
				const AssociatorProviderInfo::ClassInfo& ci = *citer; 
				String className = ci.className; 
				StringArray namespaces = ci.namespaces; 
				if (namespaces.size() == 0)
				{
					namespaces.push_back(String("")); 
				}
				for (StringArray::const_iterator nsiter = namespaces.begin(); 
					  nsiter < namespaces.end(); ++nsiter)
				{
					String key = *nsiter + ":" + className; 
					key.toLowerCase(); 
					m_assocProvs[key] = *iter; 
				}
			}
			m_assocProvs["*"] = *iter;
		}
#endif
		CppInstanceProviderIFC* instProv = prov->getInstanceProvider(); 
		if (instProv)
		{
			InstanceProviderInfo ipi;  
			instProv->getInstanceProviderInfo(ipi); 
			InstanceProviderInfo::ClassInfoArray cia = ipi.getClassInfo(); 
			for (InstanceProviderInfo::ClassInfoArray::const_iterator citer = cia.begin(); 
				  citer < cia.end(); ++citer)
			{
				const InstanceProviderInfo::ClassInfo& ci = *citer; 
				String className = ci.className; 
				StringArray namespaces = ci.namespaces; 
				if (namespaces.size() == 0)
				{
					namespaces.push_back(String("")); 
				}
				for (StringArray::const_iterator nsiter = namespaces.begin(); 
					  nsiter < namespaces.end(); ++nsiter)
				{
					String key = *nsiter + ":" + className; 
					key.toLowerCase(); 
					m_instProvs[key] = *iter; 
				}
			}
			m_instProvs["*"] = *iter;
		}
		CppSecondaryInstanceProviderIFC* secInstProv = prov->getSecondaryInstanceProvider(); 
		if (secInstProv)
		{
			SecondaryInstanceProviderInfo ipi;  
			secInstProv->getSecondaryInstanceProviderInfo(ipi); 
			SecondaryInstanceProviderInfo::ClassInfoArray cia = ipi.getClassInfo(); 
			for (SecondaryInstanceProviderInfo::ClassInfoArray::const_iterator citer = cia.begin(); 
				  citer < cia.end(); ++citer)
			{
				const SecondaryInstanceProviderInfo::ClassInfo& ci = *citer; 
				String className = ci.className; 
				StringArray namespaces = ci.namespaces; 
				if (namespaces.size() == 0)
				{
					namespaces.push_back(String("")); 
				}
				for (StringArray::const_iterator nsiter = namespaces.begin(); 
					  nsiter < namespaces.end(); ++nsiter)
				{
					String key = *nsiter + ":" + className; 
					key.toLowerCase(); 
					m_secondaryInstProvs[key] = *iter; 
				}
			}
			m_secondaryInstProvs["*"] = *iter;
		}
	}

	String confItem = getConfigItem(ProviderAgent::LockingType_opt, "none"); 
	confItem.toLowerCase(); 
	if (confItem == "none")
	{
		m_lockingType = NONE; 
	}
	else if (confItem == "swmr")
	{
		m_lockingType = SWMR; 
	}
	else if (confItem == "single_threaded")
	{
		m_lockingType = SINGLE_THREADED;  
	}
	else
	{
		OW_THROW(ConfigException, "unknown locking type"); 
	}
	confItem = getConfigItem(ProviderAgent::LockingTimeout_opt, "300"); 
	try
	{
		m_lockingTimeout = confItem.toUInt32(); 
	}
	catch (StringConversionException&)
	{
		OW_THROW(ConfigException, "invalid locking timeout"); 
	}
	confItem = getConfigItem(ProviderAgent::DynamicClassRetrieval_opt, "false"); 
	confItem.toLowerCase(); 
	if (confItem == "true")
	{
		m_classRetrieval = RETRIEVE_CLASSES; 
	}
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
	m_selectables->erase(std::remove_if (m_selectables->begin(), m_selectables->end(),
		selectableFinder(obj)), m_selectables->end());
}
//////////////////////////////////////////////////////////////////////////////
String 
ProviderAgentEnvironment::getConfigItem(const String &name, const String& defRetVal) const
{
	ConfigFile::ConfigMap::const_iterator i =
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
ProviderAgentEnvironment::setConfigItem(const String& item, const String& value, 
										EOverwritePreviousFlag overwritePrevious)
{
	if (overwritePrevious == E_OVERWRITE_PREVIOUS || getConfigItem(item) == "")
	{
		m_configItems[item] = value;
	}
}
	
//////////////////////////////////////////////////////////////////////////////
RequestHandlerIFCRef 
ProviderAgentEnvironment::getRequestHandler(const String& ct)
{
	for (Array<RequestHandlerIFCRef>::const_iterator iter = m_requestHandlers.begin(); 
		  iter != m_requestHandlers.end(); ++iter)
	{
		StringArray sa = (*iter)->getSupportedContentTypes(); 
		if (std::find(sa.begin(), sa.end(), ct) != sa.end())
		{
			return *iter; 
		}
	}
	return RequestHandlerIFCRef(SharedLibraryRef(0), 0);  //TODO need to throw? 
}
//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef 
ProviderAgentEnvironment::getCIMOMHandle(OperationContext& context,
		ESendIndicationsFlag /*doIndications*/,
		EBypassProvidersFlag /*bypassProviders*/)
{
	ProviderEnvironmentIFCRef pe(new ProviderAgentProviderEnvironment(m_logger, 
																	  m_configItems, 
																	  context, 
																	  m_callbackURL, 
																	  m_connectionPool)); 
	return CIMOMHandleIFCRef(new ProviderAgentCIMOMHandle(m_assocProvs, 
														  m_instProvs, 
														  m_secondaryInstProvs, 
														  m_methodProvs, 
														  m_cimClasses, 
														  pe, 
														  m_lockingType, 
														  m_classRetrieval, 
														  m_lockingTimeout)); 
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
	return CIMInstanceArray();
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentEnvironment::setInteropInstance(const CIMInstance& inst)
{
	OW_ASSERT("not implemented" == 0); 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

} // end namespace OpenWBEM

