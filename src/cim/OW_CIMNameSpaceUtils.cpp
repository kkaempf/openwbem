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
//#include "OW_Format.hpp"

namespace OW_CIMNameSpaceUtils
{

namespace
{
OW_CIMClass the__NamespaceClass(OW_CIMNULL);

class StringArrayBuilder : public OW_StringResultHandlerIFC
{
public:
    StringArrayBuilder(OW_StringArray& a) : m_a(a) {}
protected:
    virtual void doHandle(const OW_String &s)
    {
        m_a.push_back(s);
    }
private:
    OW_StringArray& m_a;
};

}

//////////////////////////////////////////////////////////////////////////////
OW_String
prepareNamespace(OW_String ns)
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

/////////////////////////////////////////////////////////////////////////////
void
createCIM_Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns_, OW_UInt16 classInfo, const OW_String& descriptionOfClassInfo, const OW_String& interopNs)
{
    OW_String ns(prepareNamespace(ns_));

	OW_CIMClass theCIM_NamespaceClass = hdl->getClass(interopNs, "CIM_Namespace");
    OW_CIMClass theAssocCls = hdl->getClass(interopNs, "CIM_NamespaceInManager");

    OW_CIMObjectPathEnumeration e = hdl->enumInstanceNamesE(interopNs, "CIM_ObjectManager");
    if (e.numberOfElements() != 1)
    {
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Failed to get one instance of "
            "CIM_ObjectManager.  Unable to create an instance of CIM_Namespace");
    }
    OW_CIMObjectPath theObjectManager = e.nextElement();

	OW_CIMInstance cimInstance = theCIM_NamespaceClass.newInstance();
    cimInstance.setProperty(theObjectManager.getKeyT("SystemCreationClassName"));
    cimInstance.setProperty(theObjectManager.getKeyT("SystemName"));
    cimInstance.setProperty("ObjectManagerCreationClassName", theObjectManager.getKeyT("CreationClassName").getValue());
    cimInstance.setProperty("ObjectManagerName", theObjectManager.getKeyT("Name").getValue());
	cimInstance.setProperty("CreationClassName", OW_CIMValue("CIM_Namespace"));
	cimInstance.setProperty("Name", OW_CIMValue(ns));
    cimInstance.setProperty("ClassInfo", OW_CIMValue(classInfo));
    if (!descriptionOfClassInfo.empty())
    {
        cimInstance.setProperty("DescriptionOfClassInfo", OW_CIMValue(descriptionOfClassInfo));
    }

	OW_CIMObjectPath theNewNsPath = hdl->createInstance(interopNs, cimInstance);

    OW_CIMInstance theAssoc = theAssocCls.newInstance();
    theAssoc.setProperty("Antecedent", OW_CIMValue(theObjectManager));
    theAssoc.setProperty("Dependent", OW_CIMValue(theNewNsPath));

	hdl->createInstance(interopNs, theAssoc);
}

/////////////////////////////////////////////////////////////////////////////
void
deleteCIM_Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns_, const OW_String& interopNs)
{
    OW_String ns(prepareNamespace(ns_));

    OW_CIMObjectPathEnumeration e = hdl->enumInstanceNamesE(interopNs, "CIM_ObjectManager");
    if (e.numberOfElements() != 1)
    {
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Failed to get one instance of "
            "CIM_ObjectManager.  Unable to create an instance of CIM_Namespace");
    }
    OW_CIMObjectPath theObjectManager = e.nextElement();

	OW_CIMObjectPath nsPath("CIM_Namespace", interopNs);
    nsPath.addKey(theObjectManager.getKeyT("SystemCreationClassName"));
    nsPath.addKey(theObjectManager.getKeyT("SystemName"));
    nsPath.addKey("ObjectManagerCreationClassName", theObjectManager.getKeyT("CreationClassName").getValue());
    nsPath.addKey("ObjectManagerName", theObjectManager.getKeyT("Name").getValue());
	nsPath.addKey("CreationClassName", OW_CIMValue("CIM_Namespace"));
	nsPath.addKey("Name", OW_CIMValue(ns));


    OW_CIMObjectPath theAssoc("CIM_NamespaceInManager", interopNs);
    theAssoc.addKey("Antecedent", OW_CIMValue(theObjectManager));
    theAssoc.addKey("Dependent", OW_CIMValue(nsPath));

	hdl->deleteInstance(interopNs, theAssoc);
	hdl->deleteInstance(interopNs, nsPath);
}

/////////////////////////////////////////////////////////////////////////////
OW_StringArray
enumCIM_Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& interopNs)
{
	OW_StringArray rval;
	StringArrayBuilder handler(rval);
	enumCIM_Namespace(hdl, handler, interopNs);
	return rval;
	// TODO: Write me!
	OW_THROWCIMMSG(OW_CIMException::FAILED, "OW_CIMNameSpaceUtils::enumCIM_Namespace not yet implemented.");
}

/////////////////////////////////////////////////////////////////////////////
void
enumCIM_Namespace(const OW_CIMOMHandleIFCRef& hdl,
    OW_StringResultHandlerIFC& result, const OW_String& interopNs)
{
    (void)hdl;
    (void)result;
    (void)interopNs;
	// TODO: Write me!
	OW_THROWCIMMSG(OW_CIMException::FAILED, "OW_CIMNameSpaceUtils::enumCIM_Namespace not yet implemented.");
}

/////////////////////////////////////////////////////////////////////////////
void
create__Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns_)
{
    OW_String ns(prepareNamespace(ns_));

	int index = ns.lastIndexOf('/');

	OW_String parentPath = ns.substring(0, index);
	OW_String newNameSpace = ns.substring(index + 1);

	if (!the__NamespaceClass)
	{
		the__NamespaceClass = OW_CIMClass("__Namespace");

		OW_CIMProperty cimProp(OW_CIMProperty::NAME_PROPERTY);
		cimProp.setDataType(OW_CIMDataType::STRING);
		cimProp.addQualifier(OW_CIMQualifier::createKeyQualifier());
		the__NamespaceClass.addProperty(cimProp);
	}

	OW_CIMInstance cimInstance = the__NamespaceClass.newInstance();
	OW_CIMValue cv(newNameSpace);
	cimInstance.setProperty("Name", cv);

	hdl->createInstance(parentPath, cimInstance);
}

/////////////////////////////////////////////////////////////////////////////
void
delete__Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns_)
{
    OW_String ns(prepareNamespace(ns_));

	int index = ns.lastIndexOf('/');

	OW_String parentPath = ns.substring(0,index);
	OW_String newNameSpace = ns.substring(index + 1);

	OW_CIMPropertyArray v;
	OW_CIMValue cv(newNameSpace);
	OW_CIMProperty cp("Name", cv);
	cp.setDataType(OW_CIMDataType::STRING);
	v.push_back(cp);

	OW_CIMObjectPath path(OW_CIMClass::NAMESPACECLASS, v);
	hdl->deleteInstance(parentPath, path);
}

/////////////////////////////////////////////////////////////////////////////
OW_StringArray
enum__Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns, OW_Bool deep)
{
	OW_StringArray rval;
	StringArrayBuilder handler(rval);
	enum__Namespace(hdl, ns, handler, deep);
	return rval;
}

namespace
{
//////////////////////////////////////////////////////////////////////////////
void
enumNameSpaceAux(const OW_CIMOMHandleIFCRef& hdl,
	const OW_String& ns,
	OW_StringResultHandlerIFC& result, OW_Bool deep)
{
	// can't use the callback version of enumInstances, because the recursion
	// throws a wrench in the works.  Each CIM Method call has to finish
	// before another one can begin.
	OW_CIMInstanceEnumeration en = hdl->enumInstancesE(ns,
		OW_String(OW_CIMClass::NAMESPACECLASS), false, true);
	while (en.hasMoreElements())
	{
		OW_CIMInstance i = en.nextElement();
		OW_CIMProperty nameProp;

		OW_CIMPropertyArray keys = i.getKeyValuePairs();
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

			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Name of namespace not found");
		}

		OW_String tmp;
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
enum__Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns_,
    OW_StringResultHandlerIFC& result, OW_Bool deep)
{
    OW_String ns(prepareNamespace(ns_));

	result.handle(ns);
	enumNameSpaceAux(hdl, ns, result, deep);
}

} // end namespace OW_CIMNameSpaceUtils




