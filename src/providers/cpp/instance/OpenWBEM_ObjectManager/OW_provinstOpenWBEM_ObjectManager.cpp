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
#include "OW_CppProviderIncludes.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_UUID.hpp"

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.OpenWBEM_ObjectManager");
	const String CLASS_OpenWBEM_ObjectManager("OpenWBEM_ObjectManager");
	const String CLASS_OpenWBEM_InternalData("OpenWBEM_InternalData");
	const String dataKey("OpenWBEM_ObjectManager.Name");
}

using namespace WBEMFlags;
class OpenWBEM_ObjectManagerInstProv : public CppReadOnlyInstanceProviderIFC, public CppSimpleInstanceProviderIFC
{
private:
	// Only have one Object Manager
	CIMInstance m_inst;

public:
	////////////////////////////////////////////////////////////////////////////
	OpenWBEM_ObjectManagerInstProv()
		: m_inst(CIMNULL)
	{
	}
	
	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass(CLASS_OpenWBEM_ObjectManager);
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
			omNamePath.setKeyValue("Name", CIMValue(dataKey));
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
				newData.updatePropertyValue("Name", CIMValue(dataKey));
				newData.updatePropertyValue("Value", CIMValue(omName));
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
	static CIMInstance createTheInstance(const ProviderEnvironmentIFCRef &env, const String& omName)
	{
		String interopNS = env->getConfigItem(ConfigOpts::INTEROP_SCHEMA_NAMESPACE_opt, OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
		CIMOMHandleIFCRef hdl(env->getCIMOMHandle());
		
		CIMClass OpenWBEM_ObjectManager(hdl->getClass(interopNS, CLASS_OpenWBEM_ObjectManager));

		CIMInstance newInst(OpenWBEM_ObjectManager.newInstance());

		// We are weakly associated to the ComputerSystem, we'll get it and use it's keys.
		CIMObjectPathArray computerSystems(hdl->enumInstanceNamesA(interopNS, "CIM_ComputerSystem"));
		if (computerSystems.size() != 1)
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Expected 1 instance of CIM_ComputerSystem, got %1", computerSystems.size()).c_str());
		}

		CIMObjectPath& computerSystem(computerSystems[0]);

		newInst.setProperty("Version", CIMValue(OW_VERSION));
		
		// This property is a KEY, it must be filled out
		newInst.setProperty("SystemCreationClassName", computerSystem.getKeyValue("CreationClassName"));
		// This property is a KEY, it must be filled out
		newInst.setProperty("SystemName", computerSystem.getKeyValue("Name"));
		// This property is a KEY, it must be filled out
		newInst.setProperty("CreationClassName", CIMValue(newInst.getClassName()));

		// This property is a KEY, it must be filled out
		newInst.setProperty("Name", CIMValue(omName));

		newInst.setProperty("Started", CIMValue(true));
		newInst.setProperty("EnabledState", CIMValue(UInt16(2))); // 2 = Enabled
		newInst.setProperty("Caption", CIMValue("owcimomd"));
		newInst.setProperty("Description", CIMValue("owcimomd"));

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
		result.handle(m_inst);
	}
};

} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::OpenWBEM_ObjectManagerInstProv, owprovinstOpenWBEM_ObjectManager)

