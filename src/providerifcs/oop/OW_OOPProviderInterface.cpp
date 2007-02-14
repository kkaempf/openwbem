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
#include "OW_OOPQueryProvider.hpp"
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
#include "OW_OpenWBEM_OOPAlertIndicationProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPAssociationProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPIndicationExportProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPInstanceProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPLifecycleIndicationProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPMethodProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPPolledProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPProviderProcess.hpp"
#include "OW_OpenWBEM_OOPProviderProcessCapabilities.hpp"
#include "OW_OpenWBEM_OOPQueryProviderCapabilities.hpp"
#include "OW_OpenWBEM_OOPSecondaryInstanceProviderCapabilities.hpp"

#include "OW_UnnamedPipe.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Infinity.hpp"
#include "OW_Assertion.hpp"
#include "OW_LocalOperationContext.hpp"
#include "OW_RepositoryIFC.hpp"

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");
	const String CLASS_OpenWBEM_OOPInstanceProviderCapabilities("OpenWBEM_OOPInstanceProviderCapabilities");
	const String CLASS_OpenWBEM_OOPProviderProcessCapabilities("OpenWBEM_OOPProviderProcessCapabilities");
	const String CLASS_OpenWBEM_OOPProviderProcess("OpenWBEM_OOPProviderProcess");
	const String CLASS_OpenWBEM_OOPSecondaryInstanceProviderCapabilities("OpenWBEM_OOPSecondaryInstanceProviderCapabilities");
	const String CLASS_OpenWBEM_OOPAssociationProviderCapabilities("OpenWBEM_OOPAssociationProviderCapabilities");
	const String CLASS_OpenWBEM_OOPLifecycleIndicationProviderCapabilities("OpenWBEM_OOPLifecycleIndicationProviderCapabilities");
	const String CLASS_OpenWBEM_OOPAlertIndicationProviderCapabilities("OpenWBEM_OOPAlertIndicationProviderCapabilities");
	const String CLASS_OpenWBEM_OOPMethodProviderCapabilities("OpenWBEM_OOPMethodProviderCapabilities");
	const String CLASS_OpenWBEM_OOPPolledProviderCapabilities("OpenWBEM_OOPPolledProviderCapabilities");
	const String CLASS_OpenWBEM_OOPIndicationExportProviderCapabilities("OpenWBEM_OOPIndicationExportProviderCapabilities");
	const String CLASS_OpenWBEM_OOPQueryProviderCapabilities("OpenWBEM_OOPQueryProviderCapabilities");

	bool userContextIsOperationDependent(const OpenWBEM::OOPProviderRegistration& reg)
	{
		return reg.getUserContext() == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION || reg.getUserContext() == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION_MONITORED;
	}

	bool userContextIsOperationDependent(const OpenWBEM::OOPProviderProcess& reg)
	{
		return reg.getUserContext() == OpenWBEM::OOPProviderProcess::E_USERCONTEXT_OPERATION || reg.getUserContext() == OpenWBEM::OOPProviderProcess::E_USERCONTEXT_OPERATION_MONITORED;
	}

	bool userContextIsMonitorDependent(const OpenWBEM::OOPProviderRegistration& reg)
	{
		return reg.getUserContext() == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_MONITORED || reg.getUserContext() == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION_MONITORED;
	}

	bool userContextIsMonitorDependent(const OpenWBEM::OOPProviderProcess& reg)
	{
		return reg.getUserContext() == OpenWBEM::OOPProviderProcess::E_USERCONTEXT_MONITORED || reg.getUserContext() == OpenWBEM::OOPProviderProcess::E_USERCONTEXT_OPERATION_MONITORED;
	}

	class DoNothingProviderEnvironment : public ProviderEnvironmentIFC
	{
	public:
		virtual ProviderEnvironmentIFCRef clone() const
		{
			return ProviderEnvironmentIFCRef();
		}
		virtual RepositoryIFCRef getAuthorizingRepository() const
		{
			return RepositoryIFCRef();
		}
		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return CIMOMHandleIFCRef();
		}
		virtual String getConfigItem(const String& name, const String& defRetVal) const
		{
			return String();
		}
		virtual StringArray getMultiConfigItem(const String& itemName, const StringArray& defRetVal, const char* tokenizeSeparator) const
		{
			return StringArray();
		}
		virtual OperationContext& getOperationContext()
		{
			return m_context;
		}
		virtual RepositoryIFCRef getRepository() const
		{
			return RepositoryIFCRef();
		}
		virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			return CIMOMHandleIFCRef();
		}
		virtual String getUserName() const
		{
			return String();
		}
		LocalOperationContext m_context;
	};
}

//////////////////////////////////////////////////////////////////////////////
OOPProviderInterface::OOPProviderInterface()
{
}

//////////////////////////////////////////////////////////////////////////////
OOPProviderInterface::~OOPProviderInterface()
{
	Logger lgr(COMPONENT_NAME);
	for (PersistentProvMap_t::iterator proviter = m_persistentProvs.begin(); proviter != m_persistentProvs.end(); ++proviter)
	{
		try
		{
			if (proviter->second.processState.m_persistentProcessRef && *proviter->second.processState.m_persistentProcessRef && 
				(*proviter->second.processState.m_persistentProcessRef)->processStatus().running())
			{
				OOPProviderBase* prov = proviter->second.getOOPProviderBase();
				OW_LOG_INFO(lgr, Format("terminating provider %1", proviter->first));
				ProviderEnvironmentIFCRef env(new DoNothingProviderEnvironment);
				prov->terminate(env, proviter->first);
			}
		}
		catch (Exception& e)
		{
			OW_LOG_ERROR(lgr, Format("Caught exception in OOPProviderInterface::~OOPProviderInterface(): %1", e));
		}
		catch (...)
		{
			OW_LOG_ERROR(lgr, "Caught unknown exception in OOPProviderInterface::~OOPProviderInterface()");
		}
	}
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
		IndicationProviderInfoArray& indpia,
		QueryProviderInfoArray& qpia)
{
	processOOPProviderRegistrationInstances(env, ipia, sipia, apia, mpia, indpia, qpia);
	processOOPProviderProcessCapabilitiesInstances(env, ipia, sipia, apia, mpia, indpia, qpia);
}

//////////////////////////////////////////////////////////////////////////////
void
OOPProviderInterface::processOOPProviderRegistrationInstances(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& ipia,
		SecondaryInstanceProviderInfoArray& sipia,
		AssociatorProviderInfoArray& apia,
		MethodProviderInfoArray& mpia,
		IndicationProviderInfoArray& indpia,
		QueryProviderInfoArray& qpia)
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
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() found %1 instances of OpenWBEM_OOPProviderRegistration", registrations.size()));
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

					case OpenWBEM::OOPProviderRegistration::E_PROVIDERTYPES_QUERY:
					{
						// keep it for ourselves
						m_queryProvReg[instanceID] = info;
						// give the info back to the provider manager
						QueryProviderInfo qpi;
						qpi.setProviderName(instanceID);
						QueryProviderInfo::ClassInfo classInfo(className, namespaceNames);
						qpi.addInstrumentedClass(classInfo);
						qpia.push_back(qpi);
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
void
OOPProviderInterface::processOOPProviderProcessCapabilitiesInstances(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& ipia,
		SecondaryInstanceProviderInfoArray& sipia,
		AssociatorProviderInfoArray& apia,
		MethodProviderInfoArray& mpia,
		IndicationProviderInfoArray& indpia,
		QueryProviderInfoArray& qpia)
{
	Logger lgr(COMPONENT_NAME);
	String interopNs = env->getConfigItem(ConfigOpts::INTEROP_SCHEMA_NAMESPACE_opt, OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
	CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
	CIMInstanceArray providerProcesses;
	try
	{
		providerProcesses = hdl->enumInstancesA(interopNs, CLASS_OpenWBEM_OOPProviderProcess);
	}
	catch (CIMException& e)
	{
		OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() caught exception (%1) while enumerating instances of "
			"OpenWBEM_OOPProviderProcess in namespace %2", e, interopNs));
	}
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() found %1 instances of OpenWBEM_OOPProviderProcess", providerProcesses.size()));
	for (size_t i = 0; i < providerProcesses.size(); ++i)
	{
		OpenWBEM::OOPProviderProcess curProvProc(providerProcesses[i]);
		OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() processing OpenWBEM_OOPProviderProcess %1: %2", i, curProvProc.toString()));
		try
		{
			String instanceID = curProvProc.getInstanceID();
			ProvRegInfoRef info(new ProvRegInfo);
			info->process = curProvProc.getProcess();
			info->args = curProvProc.getArgs();
			info->protocol = curProvProc.getProtocol();
			if (info->protocol != "owcpp1")
			{
				OW_LOG_ERROR(lgr, Format("Unknown protocol: %1. Skipping registration: %2", info->protocol, curProvProc.toString()));
				continue;
			}
			
			CIMDateTime timeout = curProvProc.getTimeout();
			if (!timeout.isInterval())
			{
				OW_LOG_ERROR(lgr, Format("Timeout property value is not an interval: %1. Skipping registration: %2", timeout, curProvProc.toString()));
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

			info->userContext = curProvProc.getUserContext();
			if (info->userContext < 1 || info->userContext > 4)
			{
				OW_LOG_ERROR(lgr, Format("Invalid value for UserContext: %1", info->userContext));
				continue;
			}

			if (!curProvProc.MonitorPrivilegesFileIsNULL())
			{
				info->monitorPrivilegesFile = curProvProc.getMonitorPrivilegesFile();
			}

			if (userContextIsMonitorDependent(curProvProc) && info->monitorPrivilegesFile == "")
			{
				OW_LOG_ERROR(lgr, "MonitorPrivilegesFile property cannot be NULL if UserContext is \"Monitored\" or \"OperationMonitored\"");
				continue;
			}

			// this code relies on the constructor to set info->isPersistent to the default based on the provider type
			if (!curProvProc.PersistentIsNULL())
			{
				info->isPersistent = curProvProc.getPersistent();
			}

			if (userContextIsOperationDependent(curProvProc) && info->isPersistent)
			{
				OW_LOG_ERROR(lgr, Format("Invalid OOP provider registration (%1). A persistent provider cannot have a UserContext that depends on the operation user (\"Operation\" or \"OperationMonitored\")", 
					instanceID));
				continue;
			}

			if (info->isPersistent && !curProvProc.UnloadTimeoutIsNULL())
			{
				OW_LOG_ERROR(lgr, Format("Invalid OOP provider registration (%1). A persistent provider cannot have an UnloadTimeout value", 
					instanceID));
				continue;
			}

			if (!curProvProc.UnloadTimeoutIsNULL())
			{
				CIMDateTime unloadTimeout = curProvProc.getUnloadTimeout();
				if (!unloadTimeout.isInterval())
				{
					OW_LOG_ERROR(lgr, Format("UnloadTimeout property value is not an interval: %1. Skipping registration: %2", timeout, curProvProc.toString()));
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


			CIMInstanceArray providerCapabilities;
			try
			{
				providerCapabilities = hdl->associatorsA(interopNs, CIMObjectPath(interopNs, curProvProc.instance()), CLASS_OpenWBEM_OOPProviderProcessCapabilities);
			}
			catch (CIMException& e)
			{
				OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() caught exception (%1) while getting associations of "
					"OpenWBEM_OOPProviderCapabilities in namespace %2", e, interopNs));
			}
			OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doInit() found %1 instances of OpenWBEM_OOPProviderCapabilities", providerCapabilities.size()));

			for (size_t j = 0; j < providerCapabilities.size(); ++j)
			{
				const CIMInstance& curCapabilities(providerCapabilities[j]);
				const CIMName clsName = curCapabilities.getClassName();
				if (clsName == CLASS_OpenWBEM_OOPInstanceProviderCapabilities)
				{
					// keep it for ourselves
					m_instanceProvReg[instanceID] = info;
					// give the info back to the provider manager
					InstanceProviderInfo ipi;
					ipi.setProviderName(instanceID);
					OpenWBEM::OOPInstanceProviderCapabilities instCap(curCapabilities);
					StringArray namespaceNames;
					if (!instCap.NamespaceNamesIsNULL())
						namespaceNames = instCap.getNamespaceNames();

					StringArray classNames = instCap.getClassNames();
					for (size_t k = 0; k < classNames.size(); ++k)
					{
						InstanceProviderInfo::ClassInfo classInfo(classNames[k], namespaceNames);
						ipi.addInstrumentedClass(classInfo);
						ipia.push_back(ipi);
					}
				}
				else if (clsName == CLASS_OpenWBEM_OOPSecondaryInstanceProviderCapabilities)
				{
					// keep it for ourselves
					m_secondaryInstanceProvReg[instanceID] = info;
					// give the info back to the provider manager
					SecondaryInstanceProviderInfo sipi;
					sipi.setProviderName(instanceID);
					OpenWBEM::OOPSecondaryInstanceProviderCapabilities secInstCap(curCapabilities);
					StringArray namespaceNames;
					if (!secInstCap.NamespaceNamesIsNULL())
						namespaceNames = secInstCap.getNamespaceNames();

					StringArray classNames = secInstCap.getClassNames();
					for (size_t k = 0; k < classNames.size(); ++k)
					{
						SecondaryInstanceProviderInfo::ClassInfo classInfo(classNames[k], namespaceNames);
						sipi.addInstrumentedClass(classInfo);
						sipia.push_back(sipi);
					}
				}
				else if (clsName == CLASS_OpenWBEM_OOPAssociationProviderCapabilities)
				{
					// keep it for ourselves
					m_associatorProvReg[instanceID] = info;
					// give the info back to the provider manager
					AssociatorProviderInfo api;
					api.setProviderName(instanceID);
					OpenWBEM::OOPAssociationProviderCapabilities assocCap(curCapabilities);
					StringArray namespaceNames;
					if (!assocCap.NamespaceNamesIsNULL())
						namespaceNames = assocCap.getNamespaceNames();

					StringArray classNames = assocCap.getClassNames();
					for (size_t k = 0; k < classNames.size(); ++k)
					{
						AssociatorProviderInfo::ClassInfo classInfo(classNames[k], namespaceNames);
						api.addInstrumentedClass(classInfo);
						apia.push_back(api);
					}
				}
				else if (clsName == CLASS_OpenWBEM_OOPLifecycleIndicationProviderCapabilities)
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

					OpenWBEM::OOPLifecycleIndicationProviderCapabilities liCap(curCapabilities);
					StringArray namespaceNames;
					if (!liCap.NamespaceNamesIsNULL())
						namespaceNames = liCap.getNamespaceNames();

					StringArray classNames = liCap.getClassNames();
					for (size_t k = 0; k < classNames.size(); ++k)
					{
						for (const char** pIndicationClassName = instanceLifeCycleIndicationClassNames;
							  *pIndicationClassName != 0;
							  ++pIndicationClassName)
						{
							const char* indicationClassName = *pIndicationClassName;
							IndicationProviderInfoEntry e(indicationClassName, namespaceNames, StringArray(1, classNames[k]));
							ipi.addInstrumentedClass(e);
						}
						indpia.push_back(ipi);
					}
				}
				else if (clsName == CLASS_OpenWBEM_OOPAlertIndicationProviderCapabilities)
				{
					// keep it for ourselves
					m_indicationProvReg[instanceID] = info;
					// give the info back to the provider manager
					IndicationProviderInfo ipi;
					ipi.setProviderName(instanceID);
					OpenWBEM::OOPAlertIndicationProviderCapabilities aiCap(curCapabilities);
					StringArray namespaceNames;
					if (!aiCap.NamespaceNamesIsNULL())
						namespaceNames = aiCap.getNamespaceNames();

					StringArray classNames = aiCap.getClassNames();
					for (size_t k = 0; k < classNames.size(); ++k)
					{
						IndicationProviderInfo::ClassInfo classInfo(classNames[k], namespaceNames);
						ipi.addInstrumentedClass(classInfo);
						indpia.push_back(ipi);
					}
				}
				else if (clsName == CLASS_OpenWBEM_OOPMethodProviderCapabilities)
				{
					// keep it for ourselves
					m_methodProvReg[instanceID] = info;
					// give the info back to the provider manager
					MethodProviderInfo mpi;
					mpi.setProviderName(instanceID);
					OpenWBEM::OOPMethodProviderCapabilities methCap(curCapabilities);
					StringArray namespaceNames;
					if (!methCap.NamespaceNamesIsNULL())
						namespaceNames = methCap.getNamespaceNames();

					StringArray methods; // leaving this empty means all methods
					if (!methCap.MethodNamesIsNULL())
					{
						methods = methCap.getMethodNames();
					}
					StringArray classNames = methCap.getClassNames();
					for (size_t k = 0; k < classNames.size(); ++k)
					{
						MethodProviderInfo::ClassInfo classInfo(classNames[k], namespaceNames, methods);
						mpi.addInstrumentedClass(classInfo);
						mpia.push_back(mpi);
					}
				}
				else if (clsName == CLASS_OpenWBEM_OOPPolledProviderCapabilities)
				{
					// keep it for ourselves
					m_polledProvReg[instanceID] = info;
				}
				else if (clsName == CLASS_OpenWBEM_OOPIndicationExportProviderCapabilities)
				{
					OpenWBEM::OOPIndicationExportProviderCapabilities ieCap(curCapabilities);
					// keep it for ourselves
					info->indicationExportHandlerClassNames = ieCap.getClassNames();
					m_indicationExportProvReg[instanceID] = info;
				}
				else if (clsName == CLASS_OpenWBEM_OOPQueryProviderCapabilities)
				{
					// keep it for ourselves
					m_queryProvReg[instanceID] = info;
					// give the info back to the provider manager
					QueryProviderInfo qpi;
					qpi.setProviderName(instanceID);
					OpenWBEM::OOPQueryProviderCapabilities queryCap(curCapabilities);
					StringArray namespaceNames;
					if (!queryCap.NamespaceNamesIsNULL())
						namespaceNames = queryCap.getNamespaceNames();

					StringArray classNames = queryCap.getClassNames();
					for (size_t k = 0; k < classNames.size(); ++k)
					{
						QueryProviderInfo::ClassInfo classInfo(classNames[k], namespaceNames);
						qpi.addInstrumentedClass(classInfo);
						qpia.push_back(qpi);
					}
				}
				else
				{
					OW_LOG_ERROR(lgr, Format("Invalid or unsupported registration instance: %1", curCapabilities.toString()));
				}
			}
		}
		catch (NoSuchPropertyException& e)
		{
			OW_LOG_ERROR(lgr, Format("Registration instance: %1 has no property: %2", curProvProc.toString(), e.getMessage()));
			throw;
		}
		catch (NULLValueException& e)
		{
			OW_LOG_ERROR(lgr, Format("Registration instance: %1 property has null value: %2", curProvProc.toString(), e.getMessage()));
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
		return RT(new T(info, OOPProcessState(OOPProcessState::E_PSNULL)));
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
			RT p(new T(info, proviter->second.processState));
			proviter->second.*dmp = p;
			return p;
		}
	}
	else
	{
		SavedProviders savedProviders;
		RT p(new T(info, savedProviders.processState));
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
				IndicationExportProviderIFCRef p(new OOPIndicationExportProvider(*iter->second, proviter->second.processState));
				proviter->second.indicationExportProv = p;
				rval.push_back(p);
			}
		}
		else
		{
			SavedProviders savedProviders;
			IndicationExportProviderIFCRef p(new OOPIndicationExportProvider(*iter->second, savedProviders.processState));
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
				PolledProviderIFCRef p(new OOPPolledProvider(*iter->second, proviter->second.processState));
				proviter->second.polledProv = p;
				rval.push_back(p);
			}
		}
		else
		{
			SavedProviders savedProviders;
			PolledProviderIFCRef p(new OOPPolledProvider(*iter->second, savedProviders.processState));
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
QueryProviderIFCRef
OOPProviderInterface::doGetQueryProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doGetQueryProvider, provIdString = %1", provIdString));
	ProvRegMap_t::const_iterator iter = m_queryProvReg.find(provIdString);
	if (iter == m_queryProvReg.end())
	{
		OW_THROW(NoSuchProviderException, provIdString);
	}
	else
	{
		return getProvider<OOPQueryProvider, QueryProviderIFCRef>(provIdString, &SavedProviders::queryProv, *(iter->second));
	}
}

//////////////////////////////////////////////////////////////////////////////
void 
OOPProviderInterface::doUnloadProviders(const ProviderEnvironmentIFCRef& env)
{
	MutexLock l(m_persistentProvsGuard);
	PersistentProvMap_t persistentProvsCopy(m_persistentProvs);
	l.release();

	PersistentProvMap_t::iterator proviter = persistentProvsCopy.begin();
	while (proviter != persistentProvsCopy.end())
	{
		// If this is not a persistent provider, see if we can unload it.
		if (!proviter->second.getInfo().isPersistent && proviter->second.processState.m_persistentProcessRef && *proviter->second.processState.m_persistentProcessRef && 
			(*proviter->second.processState.m_persistentProcessRef)->processStatus().running())
		{
			OOPProviderBase* prov = proviter->second.getOOPProviderBase();
            if (prov->unloadTimeoutExpired())
			{
				Logger lgr(COMPONENT_NAME);
				OW_LOG_INFO(lgr, Format("Shutting down and terminating provider %1", proviter->first));
				dynamic_cast<ProviderBaseIFC&>(*prov).shuttingDown(env);
				prov->terminate(env, proviter->first);
				proviter = persistentProvsCopy.erase(proviter);
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
	// This should not call terminate(). At this point, providers are still expected to work and can be called after shuttingDown. 
	// The providers get terminated in ~OOPProviderInterface.

	Logger lgr(COMPONENT_NAME);

	// this mutex cannot stay locked while calling shuttingDown() as callbacks can happen on another thread and cause a deadlock.
	MutexLock lock(m_persistentProvsGuard);
	PersistentProvMap_t provsCopy(m_persistentProvs);
	lock.release();

	OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doShuttingDown, there are %1 persistent providers to shutdown", provsCopy.size()));

	RWLocker mutexOnStack;
	for (PersistentProvMap_t::iterator proviter = provsCopy.begin();
		proviter != provsCopy.end(); proviter++)
	{
		if (!proviter->second.getInfo().isPersistent && proviter->second.processState.m_persistentProcessRef && *proviter->second.processState.m_persistentProcessRef && 
			(*proviter->second.processState.m_persistentProcessRef)->processStatus().running())
		{
			ProviderBaseIFC* pprov = dynamic_cast<ProviderBaseIFC*>(proviter->second.getOOPProviderBase());
			OW_ASSERT(pprov);
			if (pprov)
			{
				RWLocker* mutexToUse = proviter->second.processState.m_guardRef ? proviter->second.processState.m_guardRef.getPtr() : &mutexOnStack;
				WriteLock pl(*mutexToUse, proviter->second.getInfo().timeout);
				OW_LOG_DEBUG(lgr, Format("OOPProviderInterface::doShuttingDown terminating provider %1", proviter->first));
				try
				{
					pprov->shuttingDown(env);
				}
				catch (Exception& e)
				{
					OW_LOG_ERROR(lgr, Format("OOPProviderInterface::doShuttingDown caught Exception: %1", e));
				}
			}
		}
	}
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
	else if (queryProv)
	{
		return dynamic_cast<OOPProviderBase*>(queryProv.getPtr());
	}
	else
	{
		OW_ASSERTMSG(0, "Error, no pointer was set!");
		return 0;
	}
}


} // end namespace OW_NAMESPACE

OW_PROVIDERIFCFACTORY(OpenWBEM::OOPProviderInterface, owoopprovifc)

