Could not find property file, CIMOM uses default configuration settings.

// Copyright 2003 Center 7, Inc.

#include "OW_config.h"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"

namespace OpenWBEM
{

class OpenWBEM_ObjectManagerInstProv : public OW_CppInstanceProviderIFC
{
public:

	////////////////////////////////////////////////////////////////////////////
	virtual ~OpenWBEM_ObjectManagerInstProv()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void getProviderInfo(OW_InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("OpenWBEM_ObjectManager");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_CIMClass& cimClass )
	{
		(void)cimClass;
		env->getLogger()->logDebug("In OpenWBEM_ObjectManagerInstProv::enumInstanceNames");

		OW_CIMObjectPath newCop(className, ns);

		// TODO: Figure out the instance names
		while (/* There are more instance name to process */)
		{
			newCop.addKey("SystemCreationClassName", OW_CIMValue(/* TODO: Put the key value here */));
			newCop.addKey("SystemName", OW_CIMValue(/* TODO: Put the key value here */));
			newCop.addKey("CreationClassName", OW_CIMValue(/* TODO: Put the key value here */));
			newCop.addKey("Name", OW_CIMValue(/* TODO: Put the key value here */));
			result.handle(newCop);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool localOnly, 
		OW_Bool deep, 
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList,
		const OW_CIMClass& requestedClass,
		const OW_CIMClass& cimClass )
	{
		(void)ns;
		env->getLogger()->logDebug("In OpenWBEM_ObjectManagerInstProv::enumInstances");

		// TODO: Get the instance information
		while (/* There are more instances to process */)
		{
			OW_CIMInstance newInst = cimClass.newInstance();
			newInst.setProperty("Version", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("GatherStatisticalData", OW_CIMValue(/* TODO: Put the value here */));
			// This property is a KEY, it must be filled out
			newInst.setProperty("SystemCreationClassName", OW_CIMValue(/* TODO: Put the value here */));
			// This property is a KEY, it must be filled out
			newInst.setProperty("SystemName", OW_CIMValue(/* TODO: Put the value here */));
			// This property is a KEY, it must be filled out
			newInst.setProperty("CreationClassName", OW_CIMValue(/* TODO: Put the value here */));
			// This property is a KEY, it must be filled out
			newInst.setProperty("Name", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("PrimaryOwnerName", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("PrimaryOwnerContact", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("StartMode", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("Started", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("EnabledStatus", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("OtherEnabledStatus", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("RequestedStatus", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("EnabledDefault", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("InstallDate", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("OperationalStatus", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("OtherStatusDescriptions", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("Status", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("Caption", OW_CIMValue(/* TODO: Put the value here */));
			newInst.setProperty("Description", OW_CIMValue(/* TODO: Put the value here */));

			result.handle(newInst.clone(localOnly,deep,includeQualifiers,
				includeClassOrigin,propertyList,requestedClass,cimClass));
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMInstance getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly,
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, 
		const OW_CIMClass& cimClass )
	{
		(void)ns;
		env->getLogger()->logDebug("In OpenWBEM_ObjectManagerInstProv::getInstance");
		OW_CIMInstance inst = cimClass.newInstance();
		inst.setProperties(instanceName.getKeys());

		newInst.setProperty("Version", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("GatherStatisticalData", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("PrimaryOwnerName", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("PrimaryOwnerContact", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("StartMode", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("Started", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("EnabledStatus", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("OtherEnabledStatus", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("RequestedStatus", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("EnabledDefault", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("InstallDate", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("OperationalStatus", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("OtherStatusDescriptions", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("Status", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("Caption", OW_CIMValue(/* TODO: Put the value here */));
		newInst.setProperty("Description", OW_CIMValue(/* TODO: Put the value here */));

		return inst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}

	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMObjectPath createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& cimInstance )
	{
		(void)env;
		(void)ns;
		(void)cimInstance;
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support createInstance");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		const OW_CIMInstance& previousInstance,
		OW_Bool includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_CIMClass& theClass)
	{
		(void)env;
		(void)ns;
		(void)modifiedInstance;
		(void)previousInstance;
		(void)includeQualifiers;
		(void)propertyList;
		(void)theClass;
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support modifyInstance");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
	{
		(void)env;
		(void)ns;
		(void)cop;
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support deleteInstance");
	}


};


}


OW_PROVIDERFACTORY(OpenWBEM_ObjectManagerInstProv, owprovinstOpenWBEM_ObjectManager)



