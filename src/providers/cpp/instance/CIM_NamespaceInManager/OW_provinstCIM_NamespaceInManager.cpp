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
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_CppAssociatorProviderIFC.hpp"
#endif
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_Assertion.hpp"

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.CIM_NamespaceInManager");
}

using namespace WBEMFlags;

class CIM_NamespaceInManagerInstProv :
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
public CppAssociatorProviderIFC
#else
public CppInstanceProviderIFC
#endif
{
public:
	////////////////////////////////////////////////////////////////////////////
	virtual ~CIM_NamespaceInManagerInstProv()
	{
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_NamespaceInManager");
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		env->getLogger(COMPONENT_NAME)->logDebug("In CIM_NamespaceInManagerInstProv::enumInstanceNames");
		CIMObjectPath newCop(className, ns);
		CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
		CIMObjectPathEnumeration objectManagers = hdl->enumInstanceNamesE(ns, "CIM_ObjectManager");
		CIMObjectPathEnumeration namespaces = hdl->enumInstanceNamesE(ns, "CIM_Namespace");
		if (!objectManagers.hasMoreElements())
		{
			return;
		}
		
		// should only be one ObjectManager
		CIMObjectPath omPath = objectManagers.nextElement();
		while (namespaces.hasMoreElements())
		{
			CIMObjectPath nsPath = namespaces.nextElement();
			newCop.setKeyValue("Antecedent", CIMValue(omPath));
			newCop.setKeyValue("Dependent", CIMValue(nsPath));
			result.handle(newCop);
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
		env->getLogger(COMPONENT_NAME)->logDebug("In CIM_NamespaceInManagerInstProv::enumInstances");
		CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
		CIMObjectPathEnumeration objectManagers = hdl->enumInstanceNamesE(ns, "CIM_ObjectManager");
		CIMObjectPathEnumeration namespaces = hdl->enumInstanceNamesE(ns, "CIM_Namespace");
		if (!objectManagers.hasMoreElements())
		{
			return;
		}
		
		// should only be one ObjectManager
		CIMObjectPath omPath = objectManagers.nextElement();
		while (namespaces.hasMoreElements())
		{
			CIMObjectPath nsPath = namespaces.nextElement();
			CIMInstance newInst = cimClass.newInstance();
			newInst.setProperty("Antecedent", CIMValue(omPath));
			newInst.setProperty("Dependent", CIMValue(nsPath));
			result.handle(newInst.clone(localOnly,deep,includeQualifiers,
				includeClassOrigin,propertyList,requestedClass,cimClass));
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
		env->getLogger(COMPONENT_NAME)->logDebug("In CIM_NamespaceInManagerInstProv::getInstance");
		CIMInstance inst = cimClass.newInstance();
		try
		{
			// first verify that the object manager exists.
			CIMObjectPath objectManagerPath(instanceName.getKeyT("Antecedent").getValueT().toCIMObjectPath());
			String omNs = objectManagerPath.getNameSpace();
			if (omNs.empty())
			{
				omNs = ns;
			}
	
			// This will throw if it doesn't exist
			env->getCIMOMHandle()->getInstance(omNs, objectManagerPath);
	
			CIMObjectPath nsPath(instanceName.getKeyT("Dependent").getValueT().toCIMObjectPath());
			String nsNs = nsPath.getNameSpace();
			if (nsNs.empty())
			{
				nsNs = ns;
			}
	
			// This will throw if it doesn't exist
			env->getCIMOMHandle()->getInstance(nsNs, nsPath);
	
	
			inst = cimClass.newInstance();
			inst.setProperty("Antecedent", CIMValue(objectManagerPath));
			inst.setProperty("Dependent", CIMValue(nsPath));
	
		}
		catch (const CIMException& e)
		{
			throw;
		}
		catch (const Exception& e)
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND, e.getMessage());
		}
		return inst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	////////////////////////////////////////////////////////////////////////////
	virtual CIMObjectPath createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{
		// just ignore createInstance.
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
		// just ignore, since there nothing they can modify.
	}
	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		// just ignore deleteInstance.
	}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	// Associator provider functions.
	virtual void getAssociatorProviderInfo(AssociatorProviderInfo &info)
	{
		info.addInstrumentedClass("CIM_NamespaceInManager");
	}
	class InstanceToObjectPathHandler : public CIMInstanceResultHandlerIFC
	{
	public:
		InstanceToObjectPathHandler(CIMObjectPathResultHandlerIFC& result_, const String& ns_)
		: result(result_)
		, ns(ns_)
		{}
		void doHandle(const CIMInstance& inst)
		{
			result.handle(CIMObjectPath(ns, inst));
		}
	private:
		CIMObjectPathResultHandlerIFC& result;
		String ns;
	};
	virtual void associatorNames(
		const ProviderEnvironmentIFCRef &env,
		CIMObjectPathResultHandlerIFC &result,
		const String &ns,
		const CIMObjectPath &objectName,
		const String &assocClass,
		const String &resultClass,
		const String &role,
		const String &resultRole)
	{
		env->getLogger(COMPONENT_NAME)->logDebug("In CIM_NamespaceInManagerInstProv::associatorNames");
		// This assert should only fail if someone created a subclass of
		// CIM_NamespaceInManager and didn't create a provider for it.
		OW_ASSERT(assocClass.equalsIgnoreCase("CIM_NamespaceInManager"));
		InstanceToObjectPathHandler handler(result, ns);
		associators(env, handler, ns, objectName, assocClass, resultClass, role, resultRole, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
	}
	virtual void referenceNames(
		const ProviderEnvironmentIFCRef &env,
		CIMObjectPathResultHandlerIFC &result,
		const String &ns,
		const CIMObjectPath &objectName,
		const String &resultClass,
		const String &role)
	{
		env->getLogger(COMPONENT_NAME)->logDebug("In CIM_NamespaceInManagerInstProv::referenceNames");
		
		// This assert should only fail if someone created a subclass of
		// CIM_NamespaceInManager and didn't create a provider for it.
		OW_ASSERT(resultClass.equalsIgnoreCase("CIM_NamespaceInManager"));
		InstanceToObjectPathHandler handler(result, ns);
		references(env, handler, ns, objectName, resultClass, role, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
	}
	class AssociatorFilter : public CIMInstanceResultHandlerIFC
	{
	public:
		AssociatorFilter(const CIMObjectPath& objectName_, CIMInstanceResultHandlerIFC& result_, 		
			CIMOMHandleIFCRef hdl_,
			const String& ns_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propertyList_)
		: objectName(objectName_)
		, result(result_)
		, hdl(hdl_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}
		void doHandle(const CIMInstance& i)
		{
			CIMObjectPath op = i.getPropertyT("Antecedent").getValueT().toCIMObjectPath();
			if (op == objectName)
			{
				CIMObjectPath toGet = i.getPropertyT("Dependent").getValueT().toCIMObjectPath();
				CIMInstance assocInst = hdl->getInstance(ns, toGet, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
				assocInst.clone(E_NOT_LOCAL_ONLY, includeQualifiers, includeClassOrigin, propertyList);
				//CIMInstance assocInst = hdl->getInstance(ns, toGet, false, includeQualifiers, includeClassOrigin, propertyList);
				//assocInst.setKeys(toGet.getKeys());
				result.handle(assocInst);
				return;
			}
			op = i.getPropertyT("Dependent").getValueT().toCIMObjectPath();
			if (op == objectName)
			{
				CIMObjectPath toGet = i.getPropertyT("Antecedent").getValueT().toCIMObjectPath();
				CIMInstance assocInst = hdl->getInstance(ns, toGet, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
				assocInst.clone(E_NOT_LOCAL_ONLY, includeQualifiers, includeClassOrigin, propertyList);
				//CIMInstance assocInst = hdl->getInstance(ns, toGet, false, includeQualifiers, includeClassOrigin, propertyList);
				//assocInst.setKeys(toGet.getKeys());
				result.handle(assocInst);
			}
		}
	private:
		const CIMObjectPath& objectName;
		CIMInstanceResultHandlerIFC& result;
		CIMOMHandleIFCRef hdl;
		String ns;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propertyList;
	};
	virtual void associators(
		const ProviderEnvironmentIFCRef &env,
		CIMInstanceResultHandlerIFC &result,
		const String &ns,
		const CIMObjectPath &objectName,
		const String &assocClass,
		const String &resultClass,
		const String &role,
		const String &resultRole,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
	{
		env->getLogger(COMPONENT_NAME)->logDebug("In CIM_NamespaceInManagerInstProv::associators");
		// This assert should only fail if someone created a subclass of
		// CIM_NamespaceInManager and didn't create a provider for it.
		OW_ASSERT(assocClass.equalsIgnoreCase("CIM_NamespaceInManager"));
		if (objectName.getClassName().equalsIgnoreCase("OpenWBEM_ObjectManager")
			|| objectName.getClassName().equalsIgnoreCase("CIM_ObjectManager"))
		{
			if (!role.empty() && !role.equalsIgnoreCase("Antecedent"))
			{
				return;
			}
			if (!resultRole.empty() && !resultRole.equalsIgnoreCase("Dependent"))
			{
				return;
			}
		}
		else if (objectName.getClassName().equalsIgnoreCase("CIM_Namespace"))
		{
			if (!role.empty() && !role.equalsIgnoreCase("Dependent"))
			{
				return;
			}
			if (!resultRole.empty() && !resultRole.equalsIgnoreCase("Antecedent"))
			{
				return;
			}
		}
		CIMClass theClass = env->getCIMOMHandle()->getClass(ns, "CIM_NamespaceInManager");
		AssociatorFilter handler(objectName, result, env->getCIMOMHandle(), ns, includeQualifiers, includeClassOrigin, propertyList);
		enumInstances(env, ns, "CIM_NamespaceInManager", handler, E_NOT_LOCAL_ONLY, E_DEEP, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, theClass, theClass);
	}
	class ReferencesFilter : public CIMInstanceResultHandlerIFC
	{
	public:
		ReferencesFilter(const CIMObjectPath& objectName_, CIMInstanceResultHandlerIFC& result_)
		: objectName(objectName_)
		, result(result_)
		{}
		void doHandle(const CIMInstance& i)
		{
			CIMObjectPath op = i.getPropertyT("Antecedent").getValueT().toCIMObjectPath();
			if (op == objectName)
			{
				result.handle(i);
				return;
			}
			op = i.getPropertyT("Dependent").getValueT().toCIMObjectPath();
			if (op == objectName)
			{
				result.handle(i);
			}
		}
	private:
		const CIMObjectPath& objectName;
		CIMInstanceResultHandlerIFC& result;
	};
	virtual void references(
		const ProviderEnvironmentIFCRef &env,
		CIMInstanceResultHandlerIFC &result,
		const String &ns,
		const CIMObjectPath &objectName,
		const String &resultClass,
		const String &role,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
	{
		env->getLogger(COMPONENT_NAME)->logDebug("In CIM_NamespaceInManagerInstProv::references");
		// This assert should only fail if someone created a subclass of
		// CIM_NamespaceInManager and didn't create a provider for it.
		OW_ASSERT(resultClass.equalsIgnoreCase("CIM_NamespaceInManager"));
		if (objectName.getClassName().equalsIgnoreCase("OpenWBEM_ObjectManager")
			|| objectName.getClassName().equalsIgnoreCase("CIM_ObjectManager"))
		{
			if (!role.empty() && !role.equalsIgnoreCase("Antecedent"))
			{
				return;
			}
		}
		else if (objectName.getClassName().equalsIgnoreCase("CIM_Namespace"))
		{
			if (!role.empty() && !role.equalsIgnoreCase("Dependent"))
			{
				return;
			}
		}
		CIMClass theClass = env->getCIMOMHandle()->getClass(ns, "CIM_NamespaceInManager");
		ReferencesFilter handler(objectName, result);
		enumInstances(env, ns, "CIM_NamespaceInManager", handler, E_NOT_LOCAL_ONLY, E_DEEP, includeQualifiers, includeClassOrigin, propertyList, theClass, theClass);
	}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
};
} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::CIM_NamespaceInManagerInstProv, owprovinstCIM_NamespaceInManager)

