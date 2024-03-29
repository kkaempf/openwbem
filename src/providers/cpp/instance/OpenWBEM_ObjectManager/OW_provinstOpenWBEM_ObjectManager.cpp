/*******************************************************************************
* Copyright (C) 2003-2004 Quest Software, Inc. All rights reserved.
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
#include "OW_CppProviderIncludes.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/UUID.hpp"
#include "blocxx/Thread.hpp"

namespace OW_NAMESPACE
{

using namespace blocxx;

namespace
{
	const String COMPONENT_NAME("ow.provider.OpenWBEM_ObjectManager");
	const String CLASS_OpenWBEM_ObjectManager("OpenWBEM_ObjectManager");
	const String CLASS_OpenWBEM_InternalData("OpenWBEM_InternalData");
	const String Class_OpenWBEM_ComputerSystem("OpenWBEM_UnitaryComputerSystem");
	const String dataKey("OpenWBEM_ObjectManager.Name");
	const String CLASS_CIM_InstModification("CIM_InstModification");
	const String CLASS_OpenWBEM_HostedObjectManager("OpenWBEM_HostedObjectManager");
	const CIMName PROP_Antecedent("Antecedent");
	const CIMName PROP_Dependent("Dependent");
	const CIMName PROP_SystemCreationClassName("SystemCreationClassName");
	const CIMName PROP_CreationClassName("CreationClassName");
	const CIMName PROP_Name("Name");
	const CIMName PROP_SystemName("SystemName");
}

using namespace WBEMFlags;
class OpenWBEM_ObjectManagerInstProv
	: public CppReadOnlyInstanceProviderIFC
	, public CppSimpleInstanceProviderIFC
	, public CppIndicationProviderIFC
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	, public CppSimpleAssociatorProviderIFC
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
{
private:
	// Only have one Object Manager
	CIMInstance m_inst;
	bool m_haveSubscriptions;

public:
	////////////////////////////////////////////////////////////////////////////
	OpenWBEM_ObjectManagerInstProv()
		: m_inst(CIMNULL)
		, m_haveSubscriptions(false)
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass(CLASS_OpenWBEM_ObjectManager);
		info.addInstrumentedClass(CLASS_OpenWBEM_HostedObjectManager);
	}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	////////////////////////////////////////////////////////////////////////////
	virtual void getAssociatorProviderInfo(AssociatorProviderInfo& info)
	{
		info.addInstrumentedClass(CLASS_OpenWBEM_HostedObjectManager);
	}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

	////////////////////////////////////////////////////////////////////////////
	virtual void getIndicationProviderInfo(IndicationProviderInfo& info)
	{
		// Add the class(es) we are monitoring for lifecycle indications.
		const char* theMonitoredClasses[] =
			{
				"OpenWBEM_ObjectManager",
				0
			};

		// all the life-cycle indications that may be generated by this provider
		const char* instanceLifeCycleIndicationClassNames[] =
			{
				"CIM_InstModification",
				"CIM_InstIndication",
				"CIM_Indication",
				0
			};

		for (const char** pIndicationClassName = instanceLifeCycleIndicationClassNames;
			  *pIndicationClassName != 0;
			  ++pIndicationClassName)
		{
			const char* indicationClassName = *pIndicationClassName;
			IndicationProviderInfoEntry e(indicationClassName);
			for (const char** pClassName = theMonitoredClasses;
				  *pClassName != 0;
				  ++pClassName)
			{
				const char* className = *pClassName;
				e.classes.push_back(className);
			}
			info.addInstrumentedClass(e);

		}
	}

	void activateFilter(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes,
		bool firstActivation
		)
	{
		m_haveSubscriptions = true;
	}

	void deActivateFilter(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes,
		bool lastActivation
		)
	{
		if (lastActivation)
		{
			m_haveSubscriptions = false;
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void initialize(const ProviderEnvironmentIFCRef &env)
	{
		// retrieve the name from the repository
		String interopNS = env->getConfigItem(ConfigOpts::INTEROP_SCHEMA_NAMESPACE_opt, OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
		CIMOMHandleIFCRef rephdl(env->getRepositoryCIMOMHandle());

		String omName; // will be set to something if we find a previous instance in the repository.
		try
		{
			// get the uuid
			CIMObjectPath omNamePath(CLASS_OpenWBEM_InternalData, interopNS);
			omNamePath.setKeyValue(PROP_Name, CIMValue(dataKey));
			CIMInstance nameInst(rephdl->getInstance(interopNS, omNamePath));
			omName = nameInst.getPropertyValue("Value").toString();
		}
		catch (CIMException& e)
		{
			if (e.getErrNo() == CIMException::NOT_FOUND)
			{
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
				omName = OW_PACKAGE_PREFIX ":";
				if (omName == ":")
				{
					// OW_PACKAGE_PREFIX is empty
					omName = "OpenWBEM:";
				}
				omName += UUID().toString();

				// now save it
				CIMClass OpenWBEM_InternalData(rephdl->getClass(interopNS, CLASS_OpenWBEM_InternalData));
				CIMInstance newData(OpenWBEM_InternalData.newInstance());
				newData.updatePropertyValue(PROP_Name, CIMValue(dataKey));
				newData.updatePropertyValue("Value", CIMValue(omName));
				/// @todo  FIX THIS. The provider could get loaded and initialized
				/// as the result of a read operation, with only the read lock
				/// acquired, but here we are doing a write operation.
				rephdl->createInstance(interopNS, newData);
#else
				throw;
#endif
			}
			else
			{
				throw;
			}
		}

		m_inst = createTheInstance(env, omName);
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void shuttingDown(const ProviderEnvironmentIFCRef &env)
	{
		if (m_haveSubscriptions)
		{
			// build the indication
			String interopNS = env->getConfigItem(ConfigOpts::INTEROP_SCHEMA_NAMESPACE_opt, OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
			CIMOMHandleIFCRef hdl(env->getCIMOMHandle());
			try
			{
				CIMClass CIM_InstModification(hdl->getClass(interopNS, CLASS_CIM_InstModification));
				CIMInstance indicationInst(CIM_InstModification.newInstance());
				indicationInst.updatePropertyValue("PreviousInstance", CIMValue(m_inst));
				// set our instance to Started=false
				m_inst.updatePropertyValue("Started", CIMValue(false));
				indicationInst.updatePropertyValue("SourceInstance", CIMValue(m_inst));
				indicationInst.setProperty("IndicationTime", CIMValue(CIMDateTime(DateTime::getCurrent())));
				hdl->exportIndication(indicationInst, interopNS);
				// Wait a bit to give the indication server time to export it.
				// Yeah this doesn't guarantee the listeners will receive the indication,
				// but shutting down is more important. And if we don't sleep, there's no chance
				// they'll ever get it.
				Thread::sleep(1000);
			}
			catch (Exception& e)
			{
				BLOCXX_LOG_ERROR(Logger(COMPONENT_NAME), Format("OpenWBEM_ObjectManager caught exception while exporting indication in shuttingDown(): %1", e));
			}

		}
	}

	////////////////////////////////////////////////////////////////////////////
	static CIMInstance createTheInstance(const ProviderEnvironmentIFCRef &env, const String& omName)
	{
		String interopNS = env->getConfigItem(ConfigOpts::INTEROP_SCHEMA_NAMESPACE_opt, OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
		CIMOMHandleIFCRef hdl(env->getCIMOMHandle());

		CIMClass OpenWBEM_ObjectManager(hdl->getClass(interopNS, CLASS_OpenWBEM_ObjectManager));

		CIMInstance newInst(OpenWBEM_ObjectManager.newInstance());

		// We are weakly associated to the ComputerSystem, we'll get it and use it's keys.
		CIMObjectPathArray computerSystems(hdl->enumInstanceNamesA(interopNS, Class_OpenWBEM_ComputerSystem));
		if (computerSystems.size() != 1)
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Expected 1 instance of %1, got %2",
				Class_OpenWBEM_ComputerSystem, computerSystems.size()).c_str());
		}

		CIMObjectPath& computerSystem(computerSystems[0]);

		newInst.updatePropertyValue("Version", CIMValue(OW_VERSION));

		// This property is a KEY, it must be filled out
		newInst.updatePropertyValue(PROP_SystemCreationClassName, computerSystem.getKeyValue(PROP_CreationClassName));
		// This property is a KEY, it must be filled out
		newInst.updatePropertyValue(PROP_SystemName, computerSystem.getKeyValue(PROP_Name));
		// This property is a KEY, it must be filled out
		newInst.updatePropertyValue(PROP_CreationClassName, CIMValue(newInst.getClassName()));

		// This property is a KEY, it must be filled out
		newInst.updatePropertyValue(PROP_Name, CIMValue(omName));

		newInst.updatePropertyValue("Started", CIMValue(true));
		newInst.updatePropertyValue("EnabledState", CIMValue(UInt16(2))); // 2 = Enabled
		newInst.updatePropertyValue("Caption", CIMValue("owcimomd"));
		newInst.updatePropertyValue("Description", CIMValue("owcimomd"));

		return newInst;
	}

	////////////////////////////////////////////////////////////////////////////
	void doSimpleEnumInstances(
		const ProviderEnvironmentIFCRef &env,
		const String &ns,
		const CIMClass &cimClass,
		CIMInstanceResultHandlerIFC &result,
		EPropertiesFlag propertiesFlag)
	{
		if (cimClass.getName() == CLASS_OpenWBEM_ObjectManager)
		{
			result.handle(m_inst);
		}
		else if (cimClass.getName() == CLASS_OpenWBEM_HostedObjectManager)
		{
			CIMInstance ci(cimClass.newInstance());
			ci.updatePropertyValue(PROP_Dependent, CIMValue(CIMObjectPath(ns, m_inst)));

			String systemCreationClassName(m_inst.getPropertyValue(PROP_SystemCreationClassName).toString());
			CIMObjectPath csPath(systemCreationClassName, ns);
			csPath.setKeyValue(PROP_CreationClassName, CIMValue(systemCreationClassName));
			csPath.setKeyValue(PROP_Name, m_inst.getPropertyValue(PROP_SystemName));

			ci.updatePropertyValue(PROP_Antecedent, CIMValue(csPath));
			result.handle(ci);
		}
	}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	////////////////////////////////////////////////////////////////////////////
	void doReferences(const ProviderEnvironmentIFCRef &env,
		CIMInstanceResultHandlerIFC &result,
		const String &ns,
		const CIMObjectPath &objectName,
		const CIMClass &assocClass,
		const String &resultClass,
		const String &role,
		const String &resultRole)
	{
	    if (
		(objectName.getClassName().equalsIgnoreCase(Class_OpenWBEM_ComputerSystem)
		 && (resultClass.empty() || resultClass.equalsIgnoreCase(CLASS_OpenWBEM_ObjectManager)))
		||
		(objectName.getClassName().equalsIgnoreCase(CLASS_OpenWBEM_ObjectManager)
		 && (resultClass.empty() || resultClass.equalsIgnoreCase(Class_OpenWBEM_ComputerSystem)))
		)

	    {
		doSimpleEnumInstances(env, ns, assocClass, result, E_ALL_PROPERTIES);
	    }
	}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
};

} // end namespace OW_NAMESPACE

OW_PROVIDERFACTORY(OpenWBEM::OpenWBEM_ObjectManagerInstProv, owprovinstOpenWBEM_ObjectManager)

