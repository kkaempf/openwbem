/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_Logger.hpp"
#include "OW_ResultHandlerIFC.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;

namespace
{
	const String COMPONENT_NAME("ow.provider.CIM_ObjectManagerCommunicationMechanism");
}

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
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "In CIM_ObjectManagerCommunicationMechanismInstProv::enumInstanceNames");
		CIMObjectPath newCop(className, ns);
		CIMInstanceArray insts = CIMOMEnvironment::instance()->getInteropInstances("CIM_ObjectManagerCommunicationMechanism");
		for (size_t i = 0; i < insts.size(); ++i)
		{
			if (insts[i].getClassName().equalsIgnoreCase(className))
			{
				newCop.setKeyValue("SystemCreationClassName", CIMValue("OpenWBEM_ObjectManager"));
				SocketAddress addr = SocketAddress::getAnyLocalHost();
				newCop.setKeyValue("SystemName", CIMValue(addr.getName()));
				newCop.setKeyValue("CreationClassName", CIMValue(className));
				newCop.setKeyValue("Name", CIMValue(insts[i].getPropertyT("Name").getValue()));
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
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "In CIM_ObjectManagerCommunicationMechanismInstProv::enumInstances");
		CIMInstanceArray insts = CIMOMEnvironment::instance()->getInteropInstances("CIM_ObjectManagerCommunicationMechanism");
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
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "In CIM_ObjectManagerCommunicationMechanismInstProv::getInstance");
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
		CIMInstanceArray insts = CIMOMEnvironment::instance()->getInteropInstances("CIM_ObjectManagerCommunicationMechanism");
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
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support deleteInstance");
	}
};
} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::CIM_ObjectManagerCommunicationMechanismInstProv, owprovinstCIM_ObjectManagerCommunicationMechanism)

