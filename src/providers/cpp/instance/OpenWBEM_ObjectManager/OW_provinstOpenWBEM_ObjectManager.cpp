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
#include "OW_CppReadOnlyInstanceProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_UUID.hpp"
#include "OW_Logger.hpp"
#include "OW_ResultHandlerIFC.hpp"

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.OpenWBEM_ObjectManager");
}

using namespace WBEMFlags;
class OpenWBEM_ObjectManagerInstProv : public CppReadOnlyInstanceProviderIFC
{
private:
	CIMInstance m_inst;
	String m_uuid;
public:
	////////////////////////////////////////////////////////////////////////////
	OpenWBEM_ObjectManagerInstProv()
		: m_inst(CIMNULL)
		, m_uuid(UUID().toString()) // TODO: Fix this to be permanent!!!!
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
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "In OpenWBEM_ObjectManagerInstProv::enumInstanceNames");
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
		
		// TODO: Try to find a CIM_ComputerSystem and use it's keys.
		// This property is a KEY, it must be filled out
		newInst.setProperty("SystemCreationClassName", CIMValue("CIM_System"));
		// This property is a KEY, it must be filled out
		SocketAddress addr = SocketAddress::getAnyLocalHost();
		newInst.setProperty("SystemName", CIMValue(addr.getName()));
		// This property is a KEY, it must be filled out
		newInst.setProperty("CreationClassName", CIMValue("OpenWBEM_ObjectManager"));
		// This property is a KEY, it must be filled out
		String Name = OW_PACKAGE_PREFIX ":";
		if (Name == ":")
		{
			// OW_PACKAGE_PREFIX is empty
			Name = "OpenWBEM:";
		}
		
		Name += m_uuid;

		newInst.setProperty("Name", CIMValue(Name));
		newInst.setProperty("Started", CIMValue(true));
		newInst.setProperty("EnabledState", CIMValue(UInt16(2))); // 2 = Enabled
		newInst.setProperty("Caption", CIMValue("owcimomd"));
		newInst.setProperty("Description", CIMValue("owcimomd"));
		return newInst;
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
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "In OpenWBEM_ObjectManagerInstProv::getInstance");
		if (!m_inst)
		{
			m_inst = createTheInst(cimClass);
		}
		if (instanceName != CIMObjectPath(instanceName.getNameSpace(), m_inst))
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND, instanceName.toString().c_str());
		}
		return m_inst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}
};

} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::OpenWBEM_ObjectManagerInstProv, owprovinstOpenWBEM_ObjectManager)

