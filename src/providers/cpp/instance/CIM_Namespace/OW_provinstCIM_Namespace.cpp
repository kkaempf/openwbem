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
#include "OW_CppSimpleInstanceProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_UserInfo.hpp"

namespace OpenWBEM
{

	namespace
	{

		class NSHandlerInst : public OW_StringResultHandlerIFC
		{
		public:
			NSHandlerInst(OW_CIMInstanceResultHandlerIFC& result_,
				const OW_CIMClass& cls,
				const OW_String& sccn, const OW_String& sn,
				const OW_String& omccn, const OW_String& omn)
			: result(result_)
			, inst(cls.newInstance())
			{
				inst.setProperty("SystemCreationClassName", OW_CIMValue(sccn));
				inst.setProperty("SystemName", OW_CIMValue(sn));
				inst.setProperty("ObjectManagerCreationClassName", OW_CIMValue(omccn));
				inst.setProperty("ObjectManagerName", OW_CIMValue(omn));
				inst.setProperty("CreationClassName", OW_CIMValue("CIM_Namespace"));
			}
	
			void doHandle(const OW_String& s)
			{
				inst.setProperty("Name", OW_CIMValue(s));
				// This property is Required
				inst.setProperty("ClassInfo", OW_CIMValue(0));
				//newInst.setProperty("Caption", OW_CIMValue(/* TODO: Put the value here */));
				//newInst.setProperty("Description", OW_CIMValue(/* TODO: Put the value here */));
				//newInst.setProperty("ElementName", OW_CIMValue(/* TODO: Put the value here */));

				result.handle(inst);
			}
		private:
			OW_CIMInstanceResultHandlerIFC& result;
			OW_CIMInstance inst;
		};

		class DeleteHandler : public OW_StringResultHandlerIFC
		{
		public:
			void doHandle(const OW_String&)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED, "Cannot delete namespace because it is not empty");
			}
		};

	}

class CIM_NamespaceInstProv : public OW_CppSimpleInstanceProviderIFC
{
public:

	////////////////////////////////////////////////////////////////////////////
	virtual ~CIM_NamespaceInstProv()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(OW_InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_Namespace");
	}

	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	virtual void doSimpleEnumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMClass& cimClass,
		OW_CIMInstanceResultHandlerIFC& result,
		EPropertiesFlag propertiesFlag)
	{
		(void)propertiesFlag;

		env->getLogger()->logDebug("In CIM_NamespaceInstProv::enumInstances");

		OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
		OW_CIMObjectPathEnumeration e = hdl->enumInstanceNamesE(ns, "OpenWBEM_ObjectManager");
		if (e.numberOfElements() < 1)
		{
			return;
		}
		// assume there'll only be one OpenWBEM_ObjectManager.
		OW_CIMObjectPath objectManager = e.nextElement();
		OW_String sccn = objectManager.getKeyT("SystemCreationClassName").getValueT().toString();
		OW_String sn = objectManager.getKeyT("SystemName").getValueT().toString();
		OW_String omccn = objectManager.getKeyT("CreationClassName").getValueT().toString();
		OW_String omn = objectManager.getKeyT("Name").getValueT().toString();
		
		NSHandlerInst nshandler(result, cimClass, sccn, sn, omccn, omn);
		OW_RepositoryIFCRef rep = env->getRepository();
		rep->enumNameSpace(nshandler, OW_UserInfo(env->getUserName()));
	}


#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMObjectPath createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& cimInstance )
	{
		env->getLogger()->logDebug("In CIM_NamespaceInstProv::createInstance");
		try
		{
			OW_String name = cimInstance.getPropertyT("Name").getValueT().toString();
			OW_RepositoryIFCRef rep = env->getRepository();
			rep->createNameSpace(name, OW_UserInfo(env->getUserName()));
		}
		catch (const OW_CIMException& e)
		{
			throw;
		}
		catch (const OW_Exception& e)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, e.getMessage());
		}
		return OW_CIMObjectPath(ns, cimInstance);
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
		// This is just a no-op since we don't really care about any properties
		// other than the keys (which can't change.)
        //OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support modifyInstance");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
	{
		(void)ns;
		env->getLogger()->logDebug("In CIM_NamespaceInstProv::createInstance");
		try
		{
			OW_String name = cop.getKeyT("Name").getValueT().toString();
			OW_RepositoryIFCRef rep = env->getRepository();

			// The client can't delete a non-empty namespace.  If we find any class names, we'll throw an exception
			DeleteHandler handler;
			rep->enumClassNames(name,"", handler, false, OW_UserInfo(env->getUserName()));

			rep->deleteNameSpace(name, OW_UserInfo(env->getUserName()));
		}
		catch (const OW_CIMException& e)
		{
			throw;
		}
		catch (const OW_Exception& e)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, e.getMessage());
		}
	}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION


};


}


OW_PROVIDERFACTORY(OpenWBEM::CIM_NamespaceInstProv, owprovinstCIM_Namespace)



