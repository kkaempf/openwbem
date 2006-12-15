/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_OOPProviderInterface.hpp"
#include "OW_OOPInstanceProvider.hpp"
#include "OW_OOPSecondaryInstanceProvider.hpp"
#include "OW_OOPMethodProvider.hpp"
#include "OW_OOPAssociatorProvider.hpp"
#include "OW_OOPIndicationProvider.hpp"
#include "OW_OOPPolledProvider.hpp"
#include "OW_OOPIndicationExportProvider.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_NoSuchPropertyException.hpp"
#include "OW_NULLValueException.hpp"
#include "OW_Logger.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_OpenWBEM_OOPProviderRegistration.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Infinity.hpp"

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");
}

//////////////////////////////////////////////////////////////////////////////
OOPProviderInterface::OOPProviderInterface()
{
}

//////////////////////////////////////////////////////////////////////////////
OOPProviderInterface::~OOPProviderInterface()
{
}

//////////////////////////////////////////////////////////////////////////////
const char*
OOPProviderInterface::getName() const
{
	return "OOP";
}
	
//////////////////////////////////////////////////////////////////////////////
void
OOPProviderInterface::doInit(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& ipia,
		SecondaryInstanceProviderInfoArray& sipia,
		AssociatorProviderInfoArray& apia,
		MethodProviderInfoArray& mpia,
		IndicationProviderInfoArray& indpia)
{
	Logger lgr(COMPONENT_NAME);
	String interopNs = env->getConfigItem(ConfigOpts::INTEROP_SCHEMA_NAMESPACE_opt, OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
	CIMInstanceArray registrations;
	try
	{
		registrations = env->getCIMOMHandle()->enumInstancesA(interopNs, "OpenWBEM_OOPProviderRegistration");
	}
	catch (CIMException& e)
	{
		OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() caught exception (%1) while enumerating instances of "
			"OpenWBEM_OOPProviderRegistration in namespace %2", e, interopNs));
	}
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() found %1 registrations", registrations.size()));
	for (size_t i = 0; i < registrations.size(); ++i)
	{
		OpenWBEM::OOPProviderRegistration curReg(registrations[i]);
		OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() processing registration %1: %2", i, curReg.toString()));
		try
		{
			String instanceID = curReg.getInstanceID();
			ProvRegInfoRef info(new ProvRegInfo);
			info->process = curReg.getProcess();
			info->args = curReg.getArgs();
			info->protocol = curReg.getProtocol();
			if (info->protocol != "owcpp1")
			{
				OW_LOG_ERROR(lgr, Format("Unknown protocol: %1. Skipping registration: %2", info->protocol, curReg.toString()));
				continue;
			}
			
			StringArray namespaceNames;
			if (!curReg.NamespaceNamesIsNULL())
				namespaceNames = curReg.getNamespaceNames();

			CIMDateTime timeout = curReg.getTimeout();
			if (!timeout.isInterval())
			{
				OW_LOG_ERROR(lgr, Format("Timeout property value is not an interval: %1. Skipping registration: %2", timeout, curReg.toString()));
				continue;
			}

			float timeoutSecs = timeout.getSeconds() + 60 * 
				(timeout.getMinutes() + 60 * 
				 (timeout.getHours() + 24 * static_cast<float>(timeout.getDays())));

			OW_LOG_DEBUG(lgr, Format("timeoutSecs = %1", timeoutSecs));

			if (timeoutSecs == INFINITY)
			{
				info->timeout = Timeout::infinite;
			}
			else
			{
				info->timeout = Timeout::relative(timeoutSecs);
			}

			info->userContext = curReg.getUserContext();
			if (info->userContext < 1 || info->userContext > 4)
			{
				OW_LOG_ERROR(lgr, Format("Invalid value for UserContext: %1", info->userContext));
				continue;
			}

			if (!curReg.MonitorPrivilegesFileIsNULL())
			{
				info->monitorPrivilegesFile = curReg.getMonitorPrivilegesFile();
			}

			UInt16Array providerTypes = curReg.getProviderTypes();

			if (providerTypes.empty())
			{
				OW_LOG_ERROR(lgr, "ProviderTypes property value has no entries. Registration will be ignored.");
			}

			String className;
			try
			{
				// This can throw a NullValueException. This is fine if this 
				// is a polled provider or an indication export provider, otherwise
				// the class name must be known.
				className = curReg.getClassName();
			}
			catch (NULLValueException& e)
			{
				for (size_t j = 0; j < providerTypes.size(); ++j)
				{
					if (providerTypes[j] != OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_POLLED
						&& providerTypes[j] != OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_INDICATION_EXPORT)
					{
						throw;
					}
				}
			}


			for (size_t j = 0; j < providerTypes.size(); ++j)
			{
				switch (providerTypes[j])
				{
					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_INSTANCE:
					{
						// keep it for ourselves
						m_instanceProvReg[instanceID] = info;
						// give the info back to the provider manager
						InstanceProviderInfo ipi;
						ipi.setProviderName(instanceID);
						InstanceProviderInfo::ClassInfo classInfo(className, namespaceNames);
						ipi.addInstrumentedClass(classInfo);
						ipia.push_back(ipi);
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_SECONDARY_INSTANCE:
					{
						// keep it for ourselves
						m_secondaryInstanceProvReg[instanceID] = info;
						// give the info back to the provider manager
						SecondaryInstanceProviderInfo sipi;
						sipi.setProviderName(instanceID);
						SecondaryInstanceProviderInfo::ClassInfo classInfo(className, namespaceNames);
						sipi.addInstrumentedClass(classInfo);
						sipia.push_back(sipi);
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_ASSOCIATION:
					{
						// keep it for ourselves
						m_associatorProvReg[instanceID] = info;
						// give the info back to the provider manager
						AssociatorProviderInfo api;
						api.setProviderName(instanceID);
						AssociatorProviderInfo::ClassInfo classInfo(className, namespaceNames);
						api.addInstrumentedClass(classInfo);
						apia.push_back(api);
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_LIFECYCLE_INDICATION:
					{
						// keep it for ourselves
						info->isPersistent = true;
						m_indicationProvReg[instanceID] = info;
						// give the info back to the provider manager
						IndicationProviderInfo ipi;
						ipi.setProviderName(instanceID);

						const char* instanceLifeCycleIndicationClassNames[] =
							{
								"CIM_InstCreation",
								"CIM_InstModification",
								"CIM_InstDeletion",
								"CIM_InstIndication",
								"CIM_Indication",
								0
							};

						for (const char** pIndicationClassName = instanceLifeCycleIndicationClassNames;
							  *pIndicationClassName != 0;
							  ++pIndicationClassName)
						{
							const char* indicationClassName = *pIndicationClassName;
							IndicationProviderInfoEntry e(indicationClassName, namespaceNames, StringArray(1, className));
							ipi.addInstrumentedClass(e);
						}
						indpia.push_back(ipi);
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_ALERT_INDICATION:
					{
						// keep it for ourselves
						info->isPersistent = true;
						m_indicationProvReg[instanceID] = info;
						// give the info back to the provider manager
						IndicationProviderInfo ipi;
						ipi.setProviderName(instanceID);
						IndicationProviderInfo::ClassInfo classInfo(className, namespaceNames);
						ipi.addInstrumentedClass(classInfo);
						indpia.push_back(ipi);
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_METHOD:
					{
						// keep it for ourselves
						m_methodProvReg[instanceID] = info;
						// give the info back to the provider manager
						MethodProviderInfo mpi;
						mpi.setProviderName(instanceID);
						StringArray methods; // leaving this empty means all methods
						MethodProviderInfo::ClassInfo classInfo(className, namespaceNames, methods);
						mpi.addInstrumentedClass(classInfo);
						mpia.push_back(mpi);
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_POLLED:
					{
						// keep it for ourselves
						info->isPersistent = true;
						m_polledProvReg[instanceID] = info;
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_INDICATION_EXPORT:
					{
						// keep it for ourselves
						info->isPersistent = true;
						if (curReg.IndicationExportHandlerClassNamesIsNULL())
						{
							OW_LOG_ERROR(lgr, "IndicationExportHandlerClassNames property value has no entries. Registration will be ignored.");
						}
						else
						{
							info->indicationExportHandlerClassNames = curReg.getIndicationExportHandlerClassNames();
							m_indicationExportProvReg[instanceID] = info;
						}
					}
					break;

					default:
						OW_LOG_ERROR(lgr, Format("Invalid or unsupported value (%1) in ProviderTypes", providerTypes[j]));
						break;
				}
				// this code relies on the constructor and above switch to set info->isPersistent to the default based on the provider type
				if (!curReg.PersistentIsNULL())
				{
					info->isPersistent = curReg.getPersistent();
				}
			}
		}
		catch (NoSuchPropertyException& e)
		{
			OW_LOG_ERROR(lgr, Format("Registration instance: %1 has no property: %2", curReg.toString(), e.getMessage()));
			throw;
		}
		catch (NULLValueException& e)
		{
			OW_LOG_ERROR(lgr, Format("Registration instance: %1 property has null value: %2", curReg.toString(), e.getMessage()));
			throw;
		}
	}

}
	
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
OOPProviderInterface::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doGetInstanceProvider, provIdString = %1", provIdString));
	ProvRegMap_t::const_iterator iter = m_instanceProvReg.find(provIdString);
	if (iter == m_instanceProvReg.end())
	{
		OW_THROW(NoSuchProviderException, provIdString);
	}
	else
	{
		return InstanceProviderIFCRef(new OOPInstanceProvider(*iter->second));
	}
}

//////////////////////////////////////////////////////////////////////////////
SecondaryInstanceProviderIFCRef
OOPProviderInterface::doGetSecondaryInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doGetSecondaryInstanceProvider, provIdString = %1", provIdString));
	ProvRegMap_t::const_iterator iter = m_secondaryInstanceProvReg.find(provIdString);
	if (iter == m_secondaryInstanceProvReg.end())
	{
		OW_THROW(NoSuchProviderException, provIdString);
	}
	else
	{
		return SecondaryInstanceProviderIFCRef();
	}
}

//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
OOPProviderInterface::doGetMethodProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doGetMethodProvider, provIdString = %1", provIdString));
	ProvRegMap_t::const_iterator iter = m_methodProvReg.find(provIdString);
	if (iter == m_methodProvReg.end())
	{
		OW_THROW(NoSuchProviderException, provIdString);
	}
	else
	{
		return MethodProviderIFCRef(new OOPMethodProvider(*iter->second));
	}
}

//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
OOPProviderInterface::doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doGetAssociatorProvider, provIdString = %1", provIdString));
	ProvRegMap_t::const_iterator iter = m_associatorProvReg.find(provIdString);
	if (iter == m_associatorProvReg.end())
	{
		OW_THROW(NoSuchProviderException, provIdString);
	}
	else
	{
		return AssociatorProviderIFCRef(new OOPAssociatorProvider(*iter->second));
	}
}

//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray 
OOPProviderInterface::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef& env)
{
	IndicationExportProviderIFCRefArray rval;
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, "OOPProviderInterface::doGetIndicationexportProviders");
	
	MutexLock lock(m_persistentProvsGuard);
	ProvRegMap_t::const_iterator iter = m_indicationExportProvReg.begin();
	while (iter != m_indicationExportProvReg.end())
	{
		PersistentProvMap_t::iterator proviter = m_persistentProvs.find(iter->first);
		if (proviter != m_persistentProvs.end())
		{
			if (proviter->second.indicationExportProv)
			{
				rval.push_back(proviter->second.indicationExportProv);
			}
			else
			{
				IndicationExportProviderIFCRef p(new OOPIndicationExportProvider(*iter->second, proviter->second.guard, proviter->second.process));
				proviter->second.indicationExportProv = p;
				rval.push_back(p);
			}
		}
		else
		{
			SavedProviders savedProviders;
			IndicationExportProviderIFCRef p(new OOPIndicationExportProvider(*iter->second, savedProviders.guard, savedProviders.process));
			savedProviders.indicationExportProv = p;
			m_persistentProvs.insert(std::make_pair(iter->first, savedProviders));
			rval.push_back(p);
		}
		++iter;
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
PolledProviderIFCRefArray 
OOPProviderInterface::doGetPolledProviders(const ProviderEnvironmentIFCRef& env)
{
	PolledProviderIFCRefArray rval;
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, "OOPProviderInterface::doGetPolledProviders");
	
	MutexLock lock(m_persistentProvsGuard);
	ProvRegMap_t::const_iterator iter = m_polledProvReg.begin();
	while (iter != m_polledProvReg.end())
	{
		PersistentProvMap_t::iterator proviter = m_persistentProvs.find(iter->first);
		if (proviter != m_persistentProvs.end())
		{
			if (proviter->second.polledProv)
			{
				rval.push_back(proviter->second.polledProv);
			}
			else
			{
				PolledProviderIFCRef p(new OOPPolledProvider(*iter->second, proviter->second.guard, proviter->second.process));
				proviter->second.polledProv = p;
				rval.push_back(p);
			}
		}
		else
		{
			SavedProviders savedProviders;
			PolledProviderIFCRef p(new OOPPolledProvider(*iter->second, savedProviders.guard, savedProviders.process));
			savedProviders.polledProv = p;
			m_persistentProvs.insert(std::make_pair(iter->first, savedProviders));
			rval.push_back(p);
		}
		++iter;
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
IndicationProviderIFCRef 
OOPProviderInterface::doGetIndicationProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doGetIndicationProvider, provIdString = %1", provIdString));
	ProvRegMap_t::const_iterator iter = m_indicationProvReg.find(provIdString);
	if (iter == m_indicationProvReg.end())
	{
		OW_THROW(NoSuchProviderException, provIdString);
	}
	else
	{
		MutexLock lock(m_persistentProvsGuard);
		PersistentProvMap_t::iterator proviter = m_persistentProvs.find(provIdString);
		if (proviter != m_persistentProvs.end())
		{
			if (proviter->second.indProv)
			{
				return proviter->second.indProv;
			}
			else
			{
				IndicationProviderIFCRef p(new OOPIndicationProvider(*iter->second, proviter->second.guard, proviter->second.process));
				proviter->second.indProv = p;
				return p;
			}
		}
		else
		{
			SavedProviders savedProviders;
			IndicationProviderIFCRef p(new OOPIndicationProvider(*iter->second, savedProviders.guard, savedProviders.process));
			savedProviders.indProv = p;
			m_persistentProvs.insert(std::make_pair(String(provIdString), savedProviders));
			return p;
		}
	}
	// Not reachable
	return IndicationProviderIFCRef();
}

//////////////////////////////////////////////////////////////////////////////
void 
OOPProviderInterface::doUnloadProviders(const ProviderEnvironmentIFCRef& env)
{
}

//////////////////////////////////////////////////////////////////////////////
void 
OOPProviderInterface::doShuttingDown(const ProviderEnvironmentIFCRef& env)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, "OOPProviderInterface::doShuttingDown");
	Mutex mutexOnStack;
	OOPProviderBase* pprov;
	MutexLock lock(m_persistentProvsGuard);

	for (PersistentProvMap_t::iterator proviter = m_persistentProvs.begin();
		proviter != m_persistentProvs.end(); proviter++)
	{
		pprov = 0;
		if (proviter->second.polledProv)
		{
			IntrusiveReference<OOPPolledProvider> pref = proviter->second.polledProv.cast_to<OOPPolledProvider>();
			if (pref)
			{
				Mutex* mutexToUse = proviter->second.guard ? proviter->second.guard.getPtr() : &mutexOnStack;
				MutexLock pl(*mutexToUse);
				OW_LOG_DEBUG(lgr, "OOPProviderInterface::doShuttingDown terminating polled provider");
				pref->terminate(env);
			}
		}
		else if (proviter->second.indicationExportProv)
		{
			IntrusiveReference<OOPIndicationExportProvider> pref = proviter->second.polledProv.cast_to<OOPIndicationExportProvider>();
			if (pref)
			{
				Mutex* mutexToUse = proviter->second.guard ? proviter->second.guard.getPtr() : &mutexOnStack;
				MutexLock pl(*mutexToUse);
				OW_LOG_DEBUG(lgr, "OOPProviderInterface::doShuttingDown terminating indication export provider");
				pref->terminate(env);
			}
		}
		else if (proviter->second.indProv)
		{
			IntrusiveReference<OOPIndicationProvider> pref = proviter->second.polledProv.cast_to<OOPIndicationProvider>();
			if (pref)
			{
				Mutex* mutexToUse = proviter->second.guard ? proviter->second.guard.getPtr() : &mutexOnStack;
				MutexLock pl(*mutexToUse);
				OW_LOG_DEBUG(lgr, "OOPProviderInterface::doShuttingDown terminating indication provider");
				pref->terminate(env);
			}
		}
	}
}

} // end namespace OW_NAMESPACE

OW_PROVIDERIFCFACTORY(OpenWBEM::OOPProviderInterface, owoopprovifc)

