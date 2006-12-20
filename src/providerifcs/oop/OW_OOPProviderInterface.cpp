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
#include "OW_Assertion.hpp"

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");

	bool userContextIsOperationDependent(const OpenWBEM::OOPProviderRegistration& reg)
	{
		return reg.getUserContext() == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION || reg.getUserContext() == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION_MONITORED;
	}

	bool userContextIsMonitorDependent(const OpenWBEM::OOPProviderRegistration& reg)
	{
		return reg.getUserContext() == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_MONITORED || reg.getUserContext() == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION_MONITORED;
	}
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

			if (userContextIsMonitorDependent(curReg) && info->monitorPrivilegesFile == "")
			{
				OW_LOG_ERROR(lgr, "MonitorPrivilegesFile property cannot be NULL if UserContext is \"Monitored\" or \"OperationMonitored\"");
				continue;
			}

			UInt16Array providerTypes = curReg.getProviderTypes();

			if (providerTypes.empty())
			{
				OW_LOG_ERROR(lgr, "ProviderTypes property value has no entries. Registration will be ignored.");
				continue;
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

			// work on figuring out isPersistent
			for (size_t j = 0; j < providerTypes.size(); ++j)
			{
				switch (providerTypes[j])
				{
					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_LIFECYCLE_INDICATION:
					{
						info->isPersistent = true;
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_ALERT_INDICATION:
					{
						info->isPersistent = true;
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_POLLED:
					{
						info->isPersistent = true;
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_INDICATION_EXPORT:
					{
						info->isPersistent = true;
					}
					break;
				}
				// this code relies on the constructor and above switch to set info->isPersistent to the default based on the provider type
				if (!curReg.PersistentIsNULL())
				{
					info->isPersistent = curReg.getPersistent();
				}

			}
			if (userContextIsOperationDependent(curReg) && info->isPersistent)
			{
				OW_LOG_ERROR(lgr, Format("Invalid OOP provider registration (%1). A persistent provider cannot have a UserContext that depends on the operation user (\"Operation\" or \"OperationMonitored\")", 
					instanceID));
				continue;
			}

			if (info->isPersistent && !curReg.UnloadTimeoutIsNULL())
			{
				OW_LOG_ERROR(lgr, Format("Invalid OOP provider registration (%1). A persistent provider cannot have an UnloadTimeout value", 
					instanceID));
				continue;
			}

			if (!curReg.UnloadTimeoutIsNULL())
			{
				CIMDateTime unloadTimeout = curReg.getUnloadTimeout();
				if (!unloadTimeout.isInterval())
				{
					OW_LOG_ERROR(lgr, Format("UnloadTimeout property value is not an interval: %1. Skipping registration: %2", timeout, curReg.toString()));
					continue;
				}

				float timeoutSecs = unloadTimeout.getSeconds() + 60 * 
					(unloadTimeout.getMinutes() + 60 * 
					 (unloadTimeout.getHours() + 24 * static_cast<float>(unloadTimeout.getDays())));

				OW_LOG_DEBUG(lgr, Format("unload timeoutSecs = %1", timeoutSecs));

				if (timeoutSecs == INFINITY)
				{
					info->unloadTimeout = Timeout::infinite;
				}
				else
				{
					info->unloadTimeout = Timeout::relativeWithReset(timeoutSecs);
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
						m_polledProvReg[instanceID] = info;
					}
					break;

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_INDICATION_EXPORT:
					{
						// keep it for ourselves
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
template <typename T, typename RT, typename DMP>
RT
OOPProviderInterface::getProvider(const char* provIdString, DMP dmp, const OOPProviderInterface::ProvRegInfo& info)
{
	if (info.providerNeedsNewProcessForEveryInvocation())
	{
		return RT(new T(info, Reference<Mutex>(), Reference<ProcessRef>(), Reference<String>()));
	}

	MutexLock lock(m_persistentProvsGuard);
	PersistentProvMap_t::iterator proviter = m_persistentProvs.find(provIdString);
	if (proviter != m_persistentProvs.end())
	{
		if (proviter->second.*dmp)
		{
			return proviter->second.*dmp;
		}
		else
		{
			RT p(new T(info, proviter->second.guard, proviter->second.process, proviter->second.processUserName));
			proviter->second.*dmp = p;
			return p;
		}
	}
	else
	{
		SavedProviders savedProviders;
		RT p(new T(info, savedProviders.guard, savedProviders.process, savedProviders.processUserName));
		savedProviders.*dmp = p;
		m_persistentProvs.insert(std::make_pair(String(provIdString), savedProviders));
		return p;
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
		return getProvider<OOPInstanceProvider, InstanceProviderIFCRef>(provIdString, &SavedProviders::instanceProv, *(iter->second));
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
		return getProvider<OOPMethodProvider, MethodProviderIFCRef>(provIdString, &SavedProviders::methodProv, *(iter->second));
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
		return getProvider<OOPAssociatorProvider, AssociatorProviderIFCRef>(provIdString, &SavedProviders::associatorProv, *(iter->second));
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
				IndicationExportProviderIFCRef p(new OOPIndicationExportProvider(*iter->second, proviter->second.guard, proviter->second.process, proviter->second.processUserName));
				proviter->second.indicationExportProv = p;
				rval.push_back(p);
			}
		}
		else
		{
			SavedProviders savedProviders;
			IndicationExportProviderIFCRef p(new OOPIndicationExportProvider(*iter->second, savedProviders.guard, savedProviders.process, savedProviders.processUserName));
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
				PolledProviderIFCRef p(new OOPPolledProvider(*iter->second, proviter->second.guard, proviter->second.process, proviter->second.processUserName));
				proviter->second.polledProv = p;
				rval.push_back(p);
			}
		}
		else
		{
			SavedProviders savedProviders;
			PolledProviderIFCRef p(new OOPPolledProvider(*iter->second, savedProviders.guard, savedProviders.process, savedProviders.processUserName));
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
		return getProvider<OOPIndicationProvider, IndicationProviderIFCRef>(provIdString, &SavedProviders::indProv, *(iter->second));
	}
	// Not reachable
	return IndicationProviderIFCRef();
}

//////////////////////////////////////////////////////////////////////////////
void 
OOPProviderInterface::doUnloadProviders(const ProviderEnvironmentIFCRef& env)
{
	DateTime dt;
	dt.setToCurrent();
	MutexLock l(m_persistentProvsGuard);
	PersistentProvMap_t::iterator proviter = m_persistentProvs.begin();
	while (proviter != m_persistentProvs.end())
	{
		// If this is not a persistent provider, see if we can unload it.
		if (!proviter->second.getInfo().isPersistent)
		{
			OOPProviderBase* prov = proviter->second.getOOPProviderBase();
            if (prov->unloadTimeoutExpired())
			{
				Logger lgr(COMPONENT_NAME);
				OW_LOG_INFO(lgr, Format("Shutting down and terminating provider %1", proviter->first));
// TODO: implement this				prov->shuttingDown();
				prov->terminate(env, proviter->first);
				proviter = m_persistentProvs.erase(proviter);
				continue;
			}
		}

		++proviter;
	}
}

//////////////////////////////////////////////////////////////////////////////
void 
OOPProviderInterface::doShuttingDown(const ProviderEnvironmentIFCRef& env)
{
	/* TODO: This should not call terminate(). At this point, providers are still expected to work and can be called after shuttingDown. 
	Instead it should send a shuttingDown command, which should cause shuttingDown() to be called on the provider, and the provider should
	not terminate. The providers get terminated in ~OOPProviderInterface.

	Logger lgr(COMPONENT_NAME);
	Mutex mutexOnStack;
	MutexLock lock(m_persistentProvsGuard);
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doShuttingDown, there are %1 persistent providers to shutdown", m_persistentProvs.size()));

	for (PersistentProvMap_t::iterator proviter = m_persistentProvs.begin();
		proviter != m_persistentProvs.end(); proviter++)
	{
		OOPProviderBase* pprov = proviter->second.getOOPProviderBase();
		OW_ASSERT(pprov);
		if (pprov)
		{
			Mutex* mutexToUse = proviter->second.guard ? proviter->second.guard.getPtr() : &mutexOnStack;
			MutexLock pl(*mutexToUse);
			OW_LOG_DEBUG(lgr, "OOPProviderInterface::doShuttingDown terminating provider");
			pprov->terminate(env, proviter->first);
		}
	}
	*/
}

//////////////////////////////////////////////////////////////////////////////
const OOPProviderInterface::ProvRegInfo&
OOPProviderInterface::SavedProviders::getInfo() const
{
	OOPProviderBase* p = getOOPProviderBase();
	return p->getProvInfo();
}

//////////////////////////////////////////////////////////////////////////////
OOPProviderBase* 
OOPProviderInterface::SavedProviders::getOOPProviderBase() const
{
	if (instanceProv)
	{
		return dynamic_cast<OOPProviderBase*>(instanceProv.getPtr());
	}
	else if (secondaryInstanceProv)
	{
		return dynamic_cast<OOPProviderBase*>(secondaryInstanceProv.getPtr());
	}
	else if (associatorProv)
	{
		return dynamic_cast<OOPProviderBase*>(associatorProv.getPtr());
	}
	else if (methodProv)
	{
		return dynamic_cast<OOPProviderBase*>(methodProv.getPtr());
	}
	else if (indProv)
	{
		return dynamic_cast<OOPProviderBase*>(indProv.getPtr());
	}
	else if (polledProv)
	{
		return dynamic_cast<OOPProviderBase*>(polledProv.getPtr());
	}
	else if (indicationExportProv)
	{
		return dynamic_cast<OOPProviderBase*>(indicationExportProv.getPtr());
	}
	else
	{
		OW_ASSERTMSG(0, "Error, no pointer was set!");
		return 0;
	}
}


} // end namespace OW_NAMESPACE

OW_PROVIDERIFCFACTORY(OpenWBEM::OOPProviderInterface, owoopprovifc)

