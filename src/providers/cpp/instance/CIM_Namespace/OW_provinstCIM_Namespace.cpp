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
#include "OW_OperationContext.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
	namespace
	{
		class NSHandlerInst : public StringResultHandlerIFC
		{
		public:
			NSHandlerInst(CIMInstanceResultHandlerIFC& result_,
				const CIMClass& cls,
				const String& sccn, const String& sn,
				const String& omccn, const String& omn)
			: result(result_)
			, inst(cls.newInstance())
			{
				inst.setProperty("SystemCreationClassName", CIMValue(sccn));
				inst.setProperty("SystemName", CIMValue(sn));
				inst.setProperty("ObjectManagerCreationClassName", CIMValue(omccn));
				inst.setProperty("ObjectManagerName", CIMValue(omn));
				inst.setProperty("CreationClassName", CIMValue("CIM_Namespace"));
			}
	
			void doHandle(const String& s)
			{
				inst.setProperty("Name", CIMValue(s));
				// This property is Required
				inst.setProperty("ClassInfo", CIMValue(0));
				//newInst.setProperty("Caption", CIMValue(/* TODO: Put the value here */));
				//newInst.setProperty("Description", CIMValue(/* TODO: Put the value here */));
				//newInst.setProperty("ElementName", CIMValue(/* TODO: Put the value here */));
				result.handle(inst);
			}
		private:
			CIMInstanceResultHandlerIFC& result;
			CIMInstance inst;
		};
		class DeleteHandler : public StringResultHandlerIFC
		{
		public:
			void doHandle(const String&)
			{
				OW_THROWCIMMSG(CIMException::FAILED, "Cannot delete namespace because it is not empty");
			}
		};
	}
class CIM_NamespaceInstProv : public CppSimpleInstanceProviderIFC
{
public:
	////////////////////////////////////////////////////////////////////////////
	virtual ~CIM_NamespaceInstProv()
	{
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_Namespace");
	}
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	virtual void doSimpleEnumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMClass& cimClass,
		CIMInstanceResultHandlerIFC& result,
		EPropertiesFlag propertiesFlag)
	{
		(void)propertiesFlag;
		env->getLogger()->logDebug("In CIM_NamespaceInstProv::enumInstances");
		CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
		CIMObjectPathEnumeration e = hdl->enumInstanceNamesE(ns, "OpenWBEM_ObjectManager");
		if (e.numberOfElements() < 1)
		{
			return;
		}
		// assume there'll only be one OpenWBEM_ObjectManager.
		CIMObjectPath objectManager = e.nextElement();
		String sccn = objectManager.getKeyT("SystemCreationClassName").getValueT().toString();
		String sn = objectManager.getKeyT("SystemName").getValueT().toString();
		String omccn = objectManager.getKeyT("CreationClassName").getValueT().toString();
		String omn = objectManager.getKeyT("Name").getValueT().toString();
		
		NSHandlerInst nshandler(result, cimClass, sccn, sn, omccn, omn);
		RepositoryIFCRef rep = env->getRepository();
		rep->enumNameSpace(nshandler, env->getOperationContext());
	}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	////////////////////////////////////////////////////////////////////////////
	virtual CIMObjectPath createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{
		env->getLogger()->logDebug("In CIM_NamespaceInstProv::createInstance");
		try
		{
			String name = cimInstance.getPropertyT("Name").getValueT().toString();
			RepositoryIFCRef rep = env->getRepository();
			rep->createNameSpace(name, env->getOperationContext());
		}
		catch (const CIMException& e)
		{
			throw;
		}
		catch (const Exception& e)
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, e.getMessage());
		}
		return CIMObjectPath(ns, cimInstance);
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
		// This is just a no-op since we don't really care about any properties
		// other than the keys (which can't change.)
		//OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		(void)ns;
		env->getLogger()->logDebug("In CIM_NamespaceInstProv::createInstance");
		try
		{
			String name = cop.getKeyT("Name").getValueT().toString();
			RepositoryIFCRef rep = env->getRepository();
			// The client can't delete a non-empty namespace.  If we find any class names, we'll throw an exception
			DeleteHandler handler;
			rep->enumClassNames(name,"", handler, E_SHALLOW, env->getOperationContext());
			rep->deleteNameSpace(name, env->getOperationContext());
		}
		catch (const CIMException& e)
		{
			throw;
		}
		catch (const Exception& e)
		{
			OW_THROWCIMMSG(CIMException::FAILED, e.getMessage());
		}
	}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
};

} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::CIM_NamespaceInstProv, owprovinstCIM_Namespace)

