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
#include "OW_CppAssociatorProviderIFC.hpp"
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

class CIM_NamespaceInManagerInstProv : public OW_CppInstanceProviderIFC, public OW_CppAssociatorProviderIFC
{
public:

	////////////////////////////////////////////////////////////////////////////
	virtual ~CIM_NamespaceInManagerInstProv()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void getProviderInfo(OW_InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_NamespaceInManager");
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
		env->getLogger()->logDebug("In CIM_NamespaceInManagerInstProv::enumInstanceNames");

		OW_CIMObjectPath newCop(className, ns);

		OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
		OW_CIMObjectPathEnumeration objectManagers = hdl->enumInstanceNamesE(ns, "CIM_ObjectManager");
		OW_CIMObjectPathEnumeration namespaces = hdl->enumInstanceNamesE(ns, "CIM_Namespace");
		if (!objectManagers.hasMoreElements())
		{
			return;
		}
		
		// should only be one ObjectManager
		OW_CIMObjectPath omPath = objectManagers.nextElement();
		while (namespaces.hasMoreElements())
		{
			OW_CIMObjectPath nsPath = namespaces.nextElement();
			newCop.setKeys(OW_CIMPropertyArray());
			newCop.addKey("Antecedent", OW_CIMValue(omPath));
			newCop.addKey("Dependent", OW_CIMValue(nsPath));
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
		(void)className;
		env->getLogger()->logDebug("In CIM_NamespaceInManagerInstProv::enumInstances");

		OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
		OW_CIMObjectPathEnumeration objectManagers = hdl->enumInstanceNamesE(ns, "CIM_ObjectManager");
		OW_CIMObjectPathEnumeration namespaces = hdl->enumInstanceNamesE(ns, "CIM_Namespace");
		if (!objectManagers.hasMoreElements())
		{
			return;
		}
		
		// should only be one ObjectManager
		OW_CIMObjectPath omPath = objectManagers.nextElement();
		while (namespaces.hasMoreElements())
		{
			OW_CIMObjectPath nsPath = namespaces.nextElement();
			OW_CIMInstance newInst = cimClass.newInstance();
			newInst.setProperty("Antecedent", OW_CIMValue(omPath));
			newInst.setProperty("Dependent", OW_CIMValue(nsPath));

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
		env->getLogger()->logDebug("In CIM_NamespaceInManagerInstProv::getInstance");
		OW_CIMInstance inst = cimClass.newInstance();

		try
		{

			// first verify that the object manager exists.
			OW_CIMObjectPath objectManagerPath(instanceName.getKeyT("Antecedent").getValueT().toCIMObjectPath());
			OW_String omNs = objectManagerPath.getNameSpace();
			if (omNs.empty())
			{
				omNs = ns;
			}
	
			// This will throw if it doesn't exist
			env->getCIMOMHandle()->getInstance(omNs, objectManagerPath);
	
			OW_CIMObjectPath nsPath(instanceName.getKeyT("Dependent").getValueT().toCIMObjectPath());
			OW_String nsNs = nsPath.getNameSpace();
			if (nsNs.empty())
			{
				nsNs = ns;
			}
	
			// This will throw if it doesn't exist
			env->getCIMOMHandle()->getInstance(nsNs, nsPath);
	
	
			inst = cimClass.newInstance();
			inst.setProperty("Antecedent", OW_CIMValue(objectManagerPath));
			inst.setProperty("Dependent", OW_CIMValue(nsPath));
	
		}
		catch (const OW_CIMException& e)
		{
			throw;
		}
		catch (const OW_Exception& e)
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, e.getMessage());
		}

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
        // just ignore createInstance.
		return OW_CIMObjectPath(cimInstance);
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
        // just ignore, since there nothing they can modify.
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
        // just ignore deleteInstance.
	}

	// Associator provider functions.
	virtual void getProviderInfo(OW_AssociatorProviderInfo &info) 
	{
		info.addInstrumentedClass("CIM_NamespaceInManager");
	}

	class InstanceToObjectPathHandler : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		InstanceToObjectPathHandler(OW_CIMObjectPathResultHandlerIFC& result_, const OW_String& ns_)
		: result(result_)
		, ns(ns_)
		{}

		void doHandle(const OW_CIMInstance& inst)
		{
			result.handle(OW_CIMObjectPath(ns, inst));
		}
	private:
		OW_CIMObjectPathResultHandlerIFC& result;
		OW_String ns;
	};

	virtual void associatorNames(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_String &ns, 
		const OW_CIMObjectPath &assocName, 
		const OW_CIMObjectPath &objectName, 
		OW_CIMObjectPathResultHandlerIFC &result, 
		const OW_String &resultClass, 
		const OW_String &role, 
		const OW_String &resultRole) 
	{
		env->getLogger()->logDebug("In CIM_NamespaceInManagerInstProv::associatorNames");

		// This assert should only fail if someone created a subclass of
		// CIM_NamespaceInManager and didn't create a provider for it.
		OW_ASSERT(assocName.getObjectName().equalsIgnoreCase("CIM_NamespaceInManager"));

		InstanceToObjectPathHandler handler(result, ns);
		associators(env, ns, assocName, objectName, handler, resultClass, role, resultRole, false, false, 0);
	}

	virtual void referenceNames(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_String &ns, 
		const OW_CIMObjectPath &assocName, 
		const OW_CIMObjectPath &objectName, 
		OW_CIMObjectPathResultHandlerIFC &result, 
		const OW_String &role) 
	{
		env->getLogger()->logDebug("In CIM_NamespaceInManagerInstProv::referenceNames");
		
		// This assert should only fail if someone created a subclass of
		// CIM_NamespaceInManager and didn't create a provider for it.
		OW_ASSERT(assocName.getObjectName().equalsIgnoreCase("CIM_NamespaceInManager"));

		InstanceToObjectPathHandler handler(result, ns);
		references(env, ns, assocName, objectName, handler, role, false, false, 0);
	}

	class AssociatorFilter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		AssociatorFilter(const OW_CIMObjectPath& objectName_, OW_CIMInstanceResultHandlerIFC& result_, 		
			OW_CIMOMHandleIFCRef hdl_,
			const OW_String& ns_,
			bool includeQualifiers_,
			bool includeClassOrigin_,
			const OW_StringArray* propertyList_)
		: objectName(objectName_)
		, result(result_)
		, hdl(hdl_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}

		void doHandle(const OW_CIMInstance& i)
		{
			OW_CIMObjectPath op = i.getPropertyT("Antecedent").getValueT().toCIMObjectPath();
			if (op == objectName)
			{
				OW_CIMObjectPath toGet = i.getPropertyT("Dependent").getValueT().toCIMObjectPath();
				OW_CIMInstance assocInst = hdl->getInstance(ns, toGet, false, includeQualifiers, includeClassOrigin, propertyList);
				assocInst.setKeys(toGet.getKeys());
				result.handle(assocInst);
				return;
			}

			op = i.getPropertyT("Dependent").getValueT().toCIMObjectPath();
			if (op == objectName)
			{
				OW_CIMObjectPath toGet = i.getPropertyT("Antecedent").getValueT().toCIMObjectPath();
				OW_CIMInstance assocInst = hdl->getInstance(ns, toGet, false, includeQualifiers, includeClassOrigin, propertyList);
				assocInst.setKeys(toGet.getKeys());
				result.handle(assocInst);
			}
		}
	private:
		const OW_CIMObjectPath& objectName;
		OW_CIMInstanceResultHandlerIFC& result;
		OW_CIMOMHandleIFCRef hdl;
		OW_String ns;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propertyList;
	};

	virtual void associators(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_String &ns, 
		const OW_CIMObjectPath &assocName, 
		const OW_CIMObjectPath &objectName, 
		OW_CIMInstanceResultHandlerIFC &result, 
		const OW_String &resultClass, 
		const OW_String &role, 
		const OW_String &resultRole, 
		const OW_Bool &includeQualifiers, 
		const OW_Bool &includeClassOrigin, 
		const OW_StringArray *propertyList) 
	{
		(void)resultClass;
		env->getLogger()->logDebug("In CIM_NamespaceInManagerInstProv::associators");

		// This assert should only fail if someone created a subclass of
		// CIM_NamespaceInManager and didn't create a provider for it.
		OW_ASSERT(assocName.getObjectName().equalsIgnoreCase("CIM_NamespaceInManager"));

		if (objectName.getObjectName().equalsIgnoreCase("OpenWBEM_ObjectManager"))
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
		else if (objectName.getObjectName().equalsIgnoreCase("CIM_Namespace"))
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
		OW_CIMClass theClass = env->getCIMOMHandle()->getClass(ns, "CIM_NamespaceInManager");
		AssociatorFilter handler(objectName, result, env->getCIMOMHandle(), ns, includeQualifiers, includeClassOrigin, propertyList);
		enumInstances(env, ns, "CIM_NamespaceInManager", handler, false, true, true, true, 0, theClass, theClass);
	}

	class ReferencesFilter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		ReferencesFilter(const OW_CIMObjectPath& objectName_, OW_CIMInstanceResultHandlerIFC& result_)
		: objectName(objectName_)
		, result(result_)
		{}

		void doHandle(const OW_CIMInstance& i)
		{
			OW_CIMObjectPath op = i.getPropertyT("Antecedent").getValueT().toCIMObjectPath();
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
		const OW_CIMObjectPath& objectName;
		OW_CIMInstanceResultHandlerIFC& result;
	};

	virtual void references(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_String &ns, 
		const OW_CIMObjectPath &assocName, 
		const OW_CIMObjectPath &objectName, 
		OW_CIMInstanceResultHandlerIFC &result, 
		const OW_String &role, 
		const OW_Bool &includeQualifiers, 
		const OW_Bool &includeClassOrigin, 
		const OW_StringArray *propertyList) 
	{
		env->getLogger()->logDebug("In CIM_NamespaceInManagerInstProv::references");
		// This assert should only fail if someone created a subclass of
		// CIM_NamespaceInManager and didn't create a provider for it.
		OW_ASSERT(assocName.getObjectName().equalsIgnoreCase("CIM_NamespaceInManager"));

		if (objectName.getObjectName().equalsIgnoreCase("OpenWBEM_ObjectManager"))
		{
			if (!role.empty() && !role.equalsIgnoreCase("Antecedent"))
			{
				return;
			}
		}
		else if (objectName.getObjectName().equalsIgnoreCase("CIM_Namespace"))
		{
			if (!role.empty() && !role.equalsIgnoreCase("Dependent"))
			{
				return;
			}
		}
		OW_CIMClass theClass = env->getCIMOMHandle()->getClass(ns, "CIM_NamespaceInManager");
		ReferencesFilter handler(objectName, result);
		enumInstances(env, ns, "CIM_NamespaceInManager", handler, false, true, includeQualifiers, includeClassOrigin, propertyList, theClass, theClass);
	}

};


}


OW_PROVIDERFACTORY(OpenWBEM::CIM_NamespaceInManagerInstProv, owprovinstCIM_NamespaceInManager)



