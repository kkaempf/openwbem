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

namespace OpenWBEM
{

using namespace WBEMFlags;

class CIM_ObjectManagerCommunicationMechanismInstProv : public CppInstanceProviderIFC
{
public:
	////////////////////////////////////////////////////////////////////////////
	virtual ~CIM_ObjectManagerCommunicationMechanismInstProv()
	{
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_ObjectManagerCommunicationMechanism");
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		(void)cimClass;
		env->getLogger()->logDebug("In CIM_ObjectManagerCommunicationMechanismInstProv::enumInstanceNames");
		CIMObjectPath newCop(className, ns);
		CIMInstanceArray insts = CIMOMEnvironment::g_cimomEnvironment->getCommunicationMechanisms();
		for (size_t i = 0; i < insts.size(); ++i)
		{
			if (insts[i].getClassName().equalsIgnoreCase(className))
			{
				newCop.addKey("SystemCreationClassName", CIMValue("OpenWBEM_ObjectManager"));
				SocketAddress addr = SocketAddress::getAnyLocalHost();
				newCop.addKey("SystemName", CIMValue(addr.getName()));
				newCop.addKey("CreationClassName", CIMValue(className));
				newCop.addKey("Name", CIMValue(insts[i].getPropertyT("Name").getValue()));
				result.handle(newCop);
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		ELocalOnlyFlag localOnly, 
		EDeepFlag deep, 
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass )
	{
		(void)ns;
		env->getLogger()->logDebug("In CIM_ObjectManagerCommunicationMechanismInstProv::enumInstances");
		CIMInstanceArray insts = CIMOMEnvironment::g_cimomEnvironment->getCommunicationMechanisms();
		for (size_t i = 0; i < insts.size(); ++i)
		{
			if (insts[i].getClassName().equalsIgnoreCase(className))
			{
				CIMInstance& newInst(insts[i]);
				newInst.setProperty("SystemCreationClassName", CIMValue("OpenWBEM_ObjectManager"));
				SocketAddress addr = SocketAddress::getAnyLocalHost();
				newInst.setProperty("SystemName", CIMValue(addr.getName()));
				newInst.setProperty("CreationClassName", CIMValue(className));
	
				result.handle(newInst.clone(localOnly,deep,includeQualifiers,
					includeClassOrigin,propertyList,requestedClass,cimClass));
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////
	virtual CIMInstance getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, 
		const CIMClass& cimClass )
	{
		(void)ns;
		env->getLogger()->logDebug("In CIM_ObjectManagerCommunicationMechanismInstProv::getInstance");
		CIMInstance inst = cimClass.newInstance();
		inst.updatePropertyValues(instanceName.getKeys());
		String className = cimClass.getName();
		SocketAddress addr = SocketAddress::getAnyLocalHost();
		if (!instanceName.getKeyT("SystemCreationClassName").getValueT().toString().equalsIgnoreCase("OpenWBEM_ObjectManager") ||
			!instanceName.getKeyT("SystemName").getValueT().toString().equalsIgnoreCase(addr.getName()) ||
			!instanceName.getKeyT("CreationClassName").getValueT().toString().equalsIgnoreCase(className))
		{
			OW_THROWCIM(CIMException::NOT_FOUND);
		}
		CIMInstanceArray insts = CIMOMEnvironment::g_cimomEnvironment->getCommunicationMechanisms();
		for (size_t i = 0; i < insts.size(); ++i)
		{
			if (insts[i].getClassName().equalsIgnoreCase(className))
			{
				CIMInstance& newInst(insts[i]);
				newInst.setProperty("SystemCreationClassName", CIMValue("OpenWBEM_ObjectManager"));
				newInst.setProperty("SystemName", CIMValue(addr.getName()));
				newInst.setProperty("CreationClassName", CIMValue(className));
	
				return newInst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
			}
		}
		OW_THROWCIM(CIMException::NOT_FOUND);
	}
	////////////////////////////////////////////////////////////////////////////
	virtual CIMObjectPath createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{
		(void)env;
		(void)ns;
		(void)cimInstance;
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support createInstance");
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		(void)env;
		(void)ns;
		(void)modifiedInstance;
		(void)previousInstance;
		(void)includeQualifiers;
		(void)propertyList;
		(void)theClass;
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		(void)env;
		(void)ns;
		(void)cop;
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support deleteInstance");
	}
};
} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::CIM_ObjectManagerCommunicationMechanismInstProv, owprovinstCIM_ObjectManagerCommunicationMechanism)

