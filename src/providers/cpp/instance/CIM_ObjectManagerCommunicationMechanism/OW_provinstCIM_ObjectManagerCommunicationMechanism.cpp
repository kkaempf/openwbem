#include "OW_config.h"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_SocketAddress.hpp"

using namespace OW_WBEMFlags;

namespace OpenWBEM
{

class CIM_ObjectManagerCommunicationMechanismInstProv : public OW_CppInstanceProviderIFC
{
public:

	////////////////////////////////////////////////////////////////////////////
	virtual ~CIM_ObjectManagerCommunicationMechanismInstProv()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(OW_InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_ObjectManagerCommunicationMechanism");
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
		env->getLogger()->logDebug("In CIM_ObjectManagerCommunicationMechanismInstProv::enumInstanceNames");

		OW_CIMObjectPath newCop(className, ns);

		OW_CIMInstanceArray insts = OW_CIMOMEnvironment::g_cimomEnvironment->getCommunicationMechanisms();
		for (size_t i = 0; i < insts.size(); ++i)
		{
			if (insts[i].getClassName().equalsIgnoreCase(className))
			{
				newCop.addKey("SystemCreationClassName", OW_CIMValue("OpenWBEM_ObjectManager"));
				OW_SocketAddress addr = OW_SocketAddress::getAnyLocalHost();
				newCop.addKey("SystemName", OW_CIMValue(addr.getName()));
				newCop.addKey("CreationClassName", OW_CIMValue(className));
				newCop.addKey("Name", OW_CIMValue(insts[i].getPropertyT("Name").getValue()));
				result.handle(newCop);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		ELocalOnlyFlag localOnly, 
		EDeepFlag deep, 
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList,
		const OW_CIMClass& requestedClass,
		const OW_CIMClass& cimClass )
	{
		(void)ns;
		env->getLogger()->logDebug("In CIM_ObjectManagerCommunicationMechanismInstProv::enumInstances");

		OW_CIMInstanceArray insts = OW_CIMOMEnvironment::g_cimomEnvironment->getCommunicationMechanisms();
		for (size_t i = 0; i < insts.size(); ++i)
		{
			if (insts[i].getClassName().equalsIgnoreCase(className))
			{
				OW_CIMInstance& newInst(insts[i]);
				newInst.setProperty("SystemCreationClassName", OW_CIMValue("OpenWBEM_ObjectManager"));
				OW_SocketAddress addr = OW_SocketAddress::getAnyLocalHost();
				newInst.setProperty("SystemName", OW_CIMValue(addr.getName()));
				newInst.setProperty("CreationClassName", OW_CIMValue(className));
	
				result.handle(newInst.clone(localOnly,deep,includeQualifiers,
					includeClassOrigin,propertyList,requestedClass,cimClass));
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMInstance getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList, 
		const OW_CIMClass& cimClass )
	{
		(void)ns;
		env->getLogger()->logDebug("In CIM_ObjectManagerCommunicationMechanismInstProv::getInstance");
		OW_CIMInstance inst = cimClass.newInstance();
		inst.setProperties(instanceName.getKeys());

		OW_String className = cimClass.getName();
		OW_SocketAddress addr = OW_SocketAddress::getAnyLocalHost();
		if (!instanceName.getKeyT("SystemCreationClassName").getValueT().toString().equalsIgnoreCase("OpenWBEM_ObjectManager") ||
			!instanceName.getKeyT("SystemName").getValueT().toString().equalsIgnoreCase(addr.getName()) ||
			!instanceName.getKeyT("CreationClassName").getValueT().toString().equalsIgnoreCase(className))
		{
			OW_THROWCIM(OW_CIMException::NOT_FOUND);
		}

		OW_CIMInstanceArray insts = OW_CIMOMEnvironment::g_cimomEnvironment->getCommunicationMechanisms();
		for (size_t i = 0; i < insts.size(); ++i)
		{
			if (insts[i].getClassName().equalsIgnoreCase(className))
			{
				OW_CIMInstance& newInst(insts[i]);
				newInst.setProperty("SystemCreationClassName", OW_CIMValue("OpenWBEM_ObjectManager"));
				newInst.setProperty("SystemName", OW_CIMValue(addr.getName()));
				newInst.setProperty("CreationClassName", OW_CIMValue(className));
	
				return newInst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
			}
		}

		OW_THROWCIM(OW_CIMException::NOT_FOUND);
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
		EIncludeQualifiersFlag includeQualifiers,
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


OW_PROVIDERFACTORY(OpenWBEM::CIM_ObjectManagerCommunicationMechanismInstProv, owprovinstCIM_ObjectManagerCommunicationMechanism)



