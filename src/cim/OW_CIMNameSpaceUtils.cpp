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
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_CIMException.hpp"
#include "OW_String.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifier.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
namespace CIMNameSpaceUtils
{
namespace
{
	CIMClass the__NamespaceClass(CIMNULL);
	class StringArrayBuilder : public StringResultHandlerIFC
	{
	public:
		StringArrayBuilder(StringArray& a) : m_a(a)
		{
		}
	protected:
		virtual void doHandle(const String &s)
		{
			m_a.push_back(s);
		}
	private:
		StringArray& m_a;
	};
	class NamespaceObjectPathToStringHandler : public CIMObjectPathResultHandlerIFC
	{
	public:
		NamespaceObjectPathToStringHandler(StringResultHandlerIFC& result_)
			: result(result_)
		{}
		void doHandle(const CIMObjectPath& op)
		{
			result.handle(op.getKeyT("Name").getValueT().toString());
		}
	private:
		StringResultHandlerIFC& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
String
prepareNamespace(String ns)
{
	// translate \\ to /
	for (size_t i = 0; i < ns.length(); ++i )
	{
		if (ns[i] == '\\')
		{
			ns[i] = '/';
		}
	}
	while (!ns.empty() && ns[0] == '/')
	{
		ns = ns.substring(1);
	}
	return ns;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
/////////////////////////////////////////////////////////////////////////////
void
createCIM_Namespace(CIMOMHandleIFC& hdl, const String& ns_, UInt16 classInfo, const String& descriptionOfClassInfo, const String& interopNs)
{
	String ns(prepareNamespace(ns_));
	CIMClass theCIM_NamespaceClass = hdl.getClass(interopNs, "CIM_Namespace");
	CIMClass theAssocCls = hdl.getClass(interopNs, "CIM_NamespaceInManager");
	CIMObjectPathEnumeration e = hdl.enumInstanceNamesE(interopNs, "CIM_ObjectManager");
	if (e.numberOfElements() != 1)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Failed to get one instance of "
			"CIM_ObjectManager.  Unable to create an instance of CIM_Namespace");
	}
	CIMObjectPath theObjectManager = e.nextElement();
	CIMInstance cimInstance = theCIM_NamespaceClass.newInstance();
	cimInstance.setProperty(theObjectManager.getKeyT("SystemCreationClassName"));
	cimInstance.setProperty(theObjectManager.getKeyT("SystemName"));
	cimInstance.setProperty("ObjectManagerCreationClassName", theObjectManager.getKeyT("CreationClassName").getValue());
	cimInstance.setProperty("ObjectManagerName", theObjectManager.getKeyT("Name").getValue());
	cimInstance.setProperty("CreationClassName", CIMValue("CIM_Namespace"));
	cimInstance.setProperty("Name", CIMValue(ns));
	cimInstance.setProperty("ClassInfo", CIMValue(classInfo));
	if (!descriptionOfClassInfo.empty())
	{
		cimInstance.setProperty("DescriptionOfClassInfo", CIMValue(descriptionOfClassInfo));
	}
	CIMObjectPath theNewNsPath = hdl.createInstance(interopNs, cimInstance);
	CIMInstance theAssoc = theAssocCls.newInstance();
	theAssoc.setProperty("Antecedent", CIMValue(theObjectManager));
	theAssoc.setProperty("Dependent", CIMValue(theNewNsPath));
	hdl.createInstance(interopNs, theAssoc);
}
/////////////////////////////////////////////////////////////////////////////
void
deleteCIM_Namespace(CIMOMHandleIFC& hdl, const String& ns_, const String& interopNs)
{
	String ns(prepareNamespace(ns_));
	CIMObjectPathEnumeration e = hdl.enumInstanceNamesE(interopNs, "CIM_ObjectManager");
	if (e.numberOfElements() != 1)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Failed to get one instance of "
			"CIM_ObjectManager.  Unable to create an instance of CIM_Namespace");
	}
	CIMObjectPath theObjectManager = e.nextElement();
	CIMObjectPath nsPath("CIM_Namespace", interopNs);
	CIMPropertyArray keys;
	keys.push_back(theObjectManager.getKeyT("SystemCreationClassName"));
	keys.push_back(theObjectManager.getKeyT("SystemName"));
	keys.push_back(CIMProperty("ObjectManagerCreationClassName", theObjectManager.getKeyT("CreationClassName").getValue()));
	keys.push_back(CIMProperty("ObjectManagerName", theObjectManager.getKeyT("Name").getValue()));
	keys.push_back(CIMProperty("CreationClassName", CIMValue("CIM_Namespace")));
	keys.push_back(CIMProperty("Name", CIMValue(ns)));
	nsPath.setKeys(keys);

	CIMObjectPath theAssoc("CIM_NamespaceInManager", interopNs);
	keys.clear();
	keys.push_back(CIMProperty("Antecedent", CIMValue(theObjectManager)));
	keys.push_back(CIMProperty("Dependent", CIMValue(nsPath)));
	theAssoc.setKeys(keys);

	hdl.deleteInstance(interopNs, theAssoc);
	hdl.deleteInstance(interopNs, nsPath);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
/////////////////////////////////////////////////////////////////////////////
StringArray
enumCIM_Namespace(CIMOMHandleIFC& hdl, const String& interopNs)
{
	StringArray rval;
	StringArrayBuilder handler(rval);
	enumCIM_Namespace(hdl, handler, interopNs);
	return rval;
}
/////////////////////////////////////////////////////////////////////////////
void
enumCIM_Namespace(CIMOMHandleIFC& hdl,
	StringResultHandlerIFC& result, const String& interopNs)
{
	NamespaceObjectPathToStringHandler handler(result);
	hdl.enumInstanceNames(interopNs, "CIM_Namespace", handler);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
/////////////////////////////////////////////////////////////////////////////
void
create__Namespace(CIMOMHandleIFC& hdl, const String& ns_)
{
	String ns(prepareNamespace(ns_));
	size_t index = ns.lastIndexOf('/');
	String parentPath = ns.substring(0, index);
	String newNameSpace = ns.substring(index + 1);
	if (!the__NamespaceClass)
	{
		the__NamespaceClass = CIMClass("__Namespace");
		CIMProperty cimProp(CIMProperty::NAME_PROPERTY);
		cimProp.setDataType(CIMDataType::STRING);
		cimProp.addQualifier(CIMQualifier::createKeyQualifier());
		the__NamespaceClass.addProperty(cimProp);
	}
	CIMInstance cimInstance = the__NamespaceClass.newInstance();
	CIMValue cv(newNameSpace);
	cimInstance.setProperty("Name", cv);
	hdl.createInstance(parentPath, cimInstance);
}
/////////////////////////////////////////////////////////////////////////////
void
delete__Namespace(CIMOMHandleIFC& hdl, const String& ns_)
{
	String ns(prepareNamespace(ns_));
	size_t index = ns.lastIndexOf('/');
	String parentPath = ns.substring(0,index);
	String newNameSpace = ns.substring(index + 1);
	CIMPropertyArray v;
	CIMValue cv(newNameSpace);
	CIMProperty cp("Name", cv);
	cp.setDataType(CIMDataType::STRING);
	v.push_back(cp);
	CIMObjectPath path(CIMClass::NAMESPACECLASS, v);
	hdl.deleteInstance(parentPath, path);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
/////////////////////////////////////////////////////////////////////////////
StringArray
enum__Namespace(CIMOMHandleIFC& hdl, const String& ns, EDeepFlag deep)
{
	StringArray rval;
	StringArrayBuilder handler(rval);
	enum__Namespace(hdl, ns, handler, deep);
	return rval;
}
namespace
{
//////////////////////////////////////////////////////////////////////////////
	void
	enumNameSpaceAux(CIMOMHandleIFC& hdl,
		const String& ns,
		StringResultHandlerIFC& result, EDeepFlag deep)
	{
		// can't use the callback version of enumInstances, because the recursion
		// throws a wrench in the works.  Each CIM Method call has to finish
		// before another one can begin.
		CIMInstanceEnumeration en = hdl.enumInstancesE(ns,
			String(CIMClass::NAMESPACECLASS), E_SHALLOW, E_LOCAL_ONLY);
		while (en.hasMoreElements())
		{
			CIMInstance i = en.nextElement();
			CIMProperty nameProp;
			CIMPropertyArray keys = i.getKeyValuePairs();
			if (keys.size() == 1)
			{
				nameProp = keys[0];
			}
			else
			{
				for (size_t i = 0; i < keys.size(); i++)
				{
					if (keys[i].getName().equalsIgnoreCase("Name"))
					{
						nameProp = keys[i];
						break;
					}
				}
				OW_THROWCIMMSG(CIMException::FAILED,
					"Name of namespace not found");
			}
			String tmp;
			nameProp.getValue().get(tmp);
			result.handle(ns + "/" + tmp);
			if (deep)
			{
				enumNameSpaceAux(hdl, ns + "/" + tmp, result, deep);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void
enum__Namespace(CIMOMHandleIFC& hdl, const String& ns_,
	StringResultHandlerIFC& result, EDeepFlag deep)
{
	String ns(prepareNamespace(ns_));
	result.handle(ns);
	enumNameSpaceAux(hdl, ns, result, deep);
}
} // end namespace OW_CIMNameSpaceUtils

} // end namespace OpenWBEM

