/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_SocketAddress.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
class OpenWBEM_ObjectManagerInstProv : public CppInstanceProviderIFC
{
private:
	CIMInstance m_inst;
public:
	////////////////////////////////////////////////////////////////////////////
	OpenWBEM_ObjectManagerInstProv()
		: m_inst(CIMNULL)
	{
	}
	////////////////////////////////////////////////////////////////////////////
	virtual ~OpenWBEM_ObjectManagerInstProv()
	{
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("OpenWBEM_ObjectManager");
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		(void)className;
		env->getLogger()->logDebug("In OpenWBEM_ObjectManagerInstProv::enumInstanceNames");
		if (!m_inst)
		{
			m_inst = createTheInst(cimClass);
		}
		CIMObjectPath newCop(ns, m_inst);
		result.handle(newCop);
	}
	CIMInstance createTheInst(const CIMClass& cimClass)
	{
		// Only have one Object Manager
		CIMInstance newInst = cimClass.newInstance();
		newInst.setProperty("Version", CIMValue(OW_VERSION));
		//newInst.setProperty("GatherStatisticalData", CIMValue(/* TODO: Put the value here */));
		// This property is a KEY, it must be filled out
		newInst.setProperty("SystemCreationClassName", CIMValue("CIM_System"));
		// This property is a KEY, it must be filled out
		SocketAddress addr = SocketAddress::getAnyLocalHost();
		newInst.setProperty("SystemName", CIMValue(addr.getName()));
		// This property is a KEY, it must be filled out
		newInst.setProperty("CreationClassName", CIMValue("OpenWBEM_ObjectManager"));
		// This property is a KEY, it must be filled out
		newInst.setProperty("Name", CIMValue("owcimomd"));
		newInst.setProperty("Started", CIMValue(true));
		newInst.setProperty("EnabledStatus", CIMValue(UInt16(2))); // 2 = Enabled
		newInst.setProperty("Caption", CIMValue("owcimomd"));
		newInst.setProperty("Description", CIMValue("owcimomd"));
		return newInst;
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
		(void)className;
		env->getLogger()->logDebug("In OpenWBEM_ObjectManagerInstProv::enumInstances");
		if (!m_inst)
		{
			m_inst = createTheInst(cimClass);
		}
		result.handle(m_inst.clone(localOnly,deep,includeQualifiers,
			includeClassOrigin,propertyList,requestedClass,cimClass));
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
		(void)instanceName;
		env->getLogger()->logDebug("In OpenWBEM_ObjectManagerInstProv::getInstance");
		
		if (!m_inst)
		{
			m_inst = createTheInst(cimClass);
		}
		return m_inst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
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
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
};

} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::OpenWBEM_ObjectManagerInstProv, owprovinstOpenWBEM_ObjectManager)

