/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_Format.hpp"
#include "OW_GetPass.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"
#include "OW_XMLPrettyPrint.hpp"
#include "OW_CIMParamValue.hpp"

#include <iostream>
#include <algorithm> // for sort


using std::cerr;
using std::cin;
using std::cout;
using std::endl;

//////////////////////////////////////////////////////////////////////////////
void
testStart(const char* funcName, const char* parm=NULL)
{
	cout << "*** Doing " << funcName << '(';
	if(parm)
	{
		cout << parm;
	}
	cout << ") ***" << endl;

}

//////////////////////////////////////////////////////////////////////////////
inline void
testDone()
{
	//cout << "done" << endl;
}

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
}

//////////////////////////////////////////////////////////////////////////////
template <typename T>
struct sorter
{
	bool operator()(const T& x, const T& y)
	{
		return x.toString() < y.toString();
	}
};

//////////////////////////////////////////////////////////////////////////////
void
createClass(OW_CIMOMHandleIFC& hdl, const OW_String& name)
{
	testStart("createClass");

	try
	{
		OW_String cqtPath("Key");
		OW_CIMQualifierType cqt = hdl.getQualifierType("root/testsuite", cqtPath);
		OW_CIMQualifier cimQualifierKey(cqt);

		cimQualifierKey.setValue(OW_CIMValue(OW_Bool(true)));
		OW_CIMClass cimClass(OW_Bool(true));
		cimClass.setName(name);
		cimClass.setSuperClass("CIM_ComputerSystem");

		OW_Array<OW_String> zargs;
		zargs.push_back("CreationClassName");
		zargs.push_back("string");
		zargs.push_back("true");
		zargs.push_back("Name");
		zargs.push_back("string");
		zargs.push_back("true");
		zargs.push_back("OptionalArg");
		zargs.push_back("boolean");
		zargs.push_back("false");

		OW_String name;
		OW_String type;
		OW_String isKey;

		for (size_t i = 0; i < zargs.size(); i += 3)
		{
			name = zargs[i];
			type = zargs[i + 1];
			isKey = zargs[i + 2];
			OW_CIMProperty cimProp(OW_Bool(true));
			if (type.equals("string"))
			{
				cimProp.setDataType(OW_CIMDataType::STRING);
			}
			else if (type.equals("uint16"))
			{
				cimProp.setDataType(OW_CIMDataType::UINT16);
			}
			else if (type.equals("boolean"))
			{
				cimProp.setDataType(OW_CIMDataType::BOOLEAN);
			}
			else if (type.equals("datatime"))
			{
				cimProp.setDataType(OW_CIMDataType::DATETIME);
			}
			else cimProp.setDataType(OW_CIMDataType::STRING);

			cimProp.setName(name);
			if (isKey.equals("true"))
			{
				cimProp.addQualifier(cimQualifierKey);
			}
			cimClass.addProperty(cimProp);
		}

		hdl.createClass("root/testsuite", cimClass);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumClassNames(OW_CIMOMHandleIFC& hdl)
{
	testStart("enumClassNames");

	try
	{
		OW_CIMObjectPathEnumeration enu = hdl.enumClassNamesE("root/testsuite", "", true);
		while (enu.hasMoreElements())
		{
			cout << "CIMClass: " << enu.nextElement().getObjectName() << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "Testing non-deep" << endl;

	try
	{
		OW_CIMObjectPathEnumeration enu = hdl.enumClassNamesE("root/testsuite", "", false);
		while (enu.hasMoreElements())
		{
			cout << "CIMClass: " << enu.nextElement().getObjectName() << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumClasses(OW_CIMOMHandleIFC& hdl)
{
	testStart("enumClasses");

	cout << "deep = true, localOnly = false" << endl;
	try
	{
		OW_CIMClassEnumeration enu = hdl.enumClassE("root/testsuite", "", true, false);
		while (enu.hasMoreElements())
		{
			OW_CIMClass c = enu.nextElement();
			cout << "CIMClass: " << c.toMOF() << endl;
			OW_TempFileStream tfs;
			OW_CIMtoXML(c,tfs,OW_CIMtoXMLFlags::notLocalOnly,
				OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,
				OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "deep = false, localOnly = false" << endl;
	try
	{
		OW_CIMClassEnumeration enu = hdl.enumClassE("root/testsuite", "", false, false);
		while (enu.hasMoreElements())
		{
			OW_CIMClass c = enu.nextElement();
			cout << "CIMClass: " << c.toMOF() << endl;
			OW_TempFileStream tfs;
			OW_CIMtoXML(c,tfs,OW_CIMtoXMLFlags::notLocalOnly,
				OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,
				OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "deep = false, localOnly = true" << endl;
	try
	{
		OW_CIMClassEnumeration enu = hdl.enumClassE("root/testsuite", "", false, true);
		while (enu.hasMoreElements())
		{
			OW_CIMClass c = enu.nextElement();
			cout << "CIMClass: " << c.toMOF() << endl;
			OW_TempFileStream tfs;
			OW_CIMtoXML(c,tfs,OW_CIMtoXMLFlags::localOnly,
				OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,
				OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "deep = true, localOnly = true" << endl;
	try
	{
		OW_CIMClassEnumeration enu = hdl.enumClassE("root/testsuite", "", true, true);
		while (enu.hasMoreElements())
		{
			OW_CIMClass c = enu.nextElement();
			cout << "CIMClass: " << c.toMOF() << endl;
			OW_TempFileStream tfs;
			OW_CIMtoXML(c,tfs,OW_CIMtoXMLFlags::localOnly,
				OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,
				OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
modifyClass(OW_CIMOMHandleIFC& hdl)
{
	testStart("modifyClass");

	try
	{
		OW_CIMClass cimClass = hdl.getClass("root/testsuite",
			"EXP_BionicComputerSystem", false);
		cout << "CIMClass before: " << cimClass.toMOF() << endl;
		OW_TempFileStream tfs;
		OW_CIMtoXML(cimClass,tfs,OW_CIMtoXMLFlags::notLocalOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		cout << OW_XMLPrettyPrint(tfs);

		OW_CIMProperty cimProp(OW_Bool(true));
		cimProp.setDataType(OW_CIMDataType::STRING);
		cimProp.setName("BrandNewProperty");
		cimClass.addProperty(cimProp);
		hdl.modifyClass("root/testsuite", cimClass);

		cimClass = hdl.getClass("root/testsuite", "EXP_BionicComputerSystem", false);
		cout << "CIMClass after: " << cimClass.toMOF() << endl;
		tfs.reset();
		OW_CIMtoXML(cimClass,tfs,OW_CIMtoXMLFlags::notLocalOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);

	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getClass(OW_CIMOMHandleIFC& hdl)
{
	testStart("getClass");

	cout << "localOnly = false" << endl;
	try
	{
		OW_CIMClass cimClass = hdl.getClass("root/testsuite",
			"EXP_BionicComputerSystem", false);
		cout << "CIMClass: " << cimClass.toMOF() << endl;
		OW_TempFileStream tfs;
		OW_CIMtoXML(cimClass,tfs,OW_CIMtoXMLFlags::notLocalOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "localOnly = true" << endl;
	try
	{
		OW_CIMClass cimClass = hdl.getClass("root/testsuite",
			"EXP_BionicComputerSystem", true);
		cout << "CIMClass: " << cimClass.toMOF() << endl;
		OW_TempFileStream tfs;
		OW_CIMtoXML(cimClass,tfs,OW_CIMtoXMLFlags::localOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
createInstance(OW_CIMOMHandleIFC& hdl, const OW_String& fromClass, const OW_String& newInstance)
{
	testStart("createInstance");

	try
	{
		OW_CIMClass cimClass = hdl.getClass("root/testsuite", fromClass, false);

		OW_CIMInstance newInst = cimClass.newInstance();

		newInst.setName(newInstance);
		newInst.setProperty(OW_CIMProperty::NAME_PROPERTY,
								  OW_CIMValue(newInstance));
		newInst.setProperty("CreationClassName",
								  OW_CIMValue(fromClass));

		hdl.createInstance("root/testsuite", newInst);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumerateInstanceNames(OW_CIMOMHandleIFC& hdl)
{
	testStart("enumInstanceNames");

	try
	{
		OW_String ofClass = "CIM_ComputerSystem";
		OW_CIMObjectPathEnumeration enu = hdl.enumInstanceNamesE("root/testsuite", ofClass);
		while (enu.hasMoreElements())
		{
			OW_CIMObjectPath cop = enu.nextElement();
			cout << cop.toString() << endl;
			OW_TempFileStream tfs;
			OW_CIMtoXML(cop, tfs, OW_CIMtoXMLFlags::isNotInstanceName);
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumerateInstances(OW_CIMOMHandleIFC& hdl, OW_String ofClass, OW_Bool deep, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_String pstr;
	pstr = format("ofClass = %1, deep = %2, localOnly = %3, includeQualifiers = %4, "
			"includeClassOrigin = %5, propertyList? %6",
			ofClass, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList != 0);
		
	testStart("enumInstances", pstr.c_str());

	try
	{
		OW_CIMInstanceEnumeration enu = hdl.enumInstancesE("root/testsuite", ofClass, deep, localOnly,
				includeQualifiers, includeClassOrigin, propertyList);

		while (enu.hasMoreElements())
		{
			OW_CIMInstance i = enu.nextElement();
			cout << i.toMOF() << endl;
			OW_TempFileStream tfs;
			OW_CIMtoXML(i,tfs, OW_CIMObjectPath(),
				OW_CIMtoXMLFlags::isNotInstanceName,
				OW_CIMtoXMLFlags::notLocalOnly,OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getInstance(OW_CIMOMHandleIFC& hdl, const OW_String& theInstance,
		OW_Bool localOnly=false,
		OW_Bool includeQualifiers=false,
		OW_Bool includeClassOrigin=false,
		const OW_StringArray* propertyList=0)
{
	OW_String pstr;
	pstr = format("localOnly = %1, includeQualifiers=%2, "
			"includeClassOrigin = %3, propertyList? %4",
			localOnly, includeQualifiers, includeClassOrigin, propertyList != 0);
	testStart("getInstance", pstr.c_str());

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(theInstance));

		OW_CIMInstance in = hdl.getInstance("root/testsuite", cop, localOnly, includeQualifiers,
				includeClassOrigin, propertyList);
		OW_TempFileStream tfs;
		OW_CIMtoXML(in, tfs, OW_CIMObjectPath(),
			OW_CIMtoXMLFlags::isNotInstanceName,
			OW_CIMtoXMLFlags::notLocalOnly,OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,OW_StringArray());
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
modifyInstance(OW_CIMOMHandleIFC& hdl, const OW_String& theInstance)
{
	testStart("modifyInstance");

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(theInstance));

		OW_CIMInstance in = hdl.getInstance("root/testsuite", cop, false);

		in.setProperty(OW_CIMProperty("BrandNewProperty",
			OW_CIMValue(OW_String("true"))));

		// getInstance with includeQualifiers = false doesn't have any keys, so
		// we'll have to set them so modifyInstance will work.
		in.setKeys(cop.getKeys());

		hdl.modifyInstance("root/testsuite", in);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteInstance(OW_CIMOMHandleIFC& hdl, const OW_String& ofClass, const OW_String& theInstance)
{
	testStart("deleteInstance");

	try
	{
		OW_CIMObjectPath cop(ofClass, "root/testsuite");
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(theInstance));
		hdl.deleteInstance("root/testsuite", cop);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteAssociations(OW_CIMOMHandleIFC& hdl)
{
	testStart("deleteAssociations");

	try
	{
		hdl.execQueryE("root/testsuite",
			"delete from CIM_SystemComponent", "wql1");
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
setQualifier(OW_CIMOMHandleIFC& hdl)
{
	testStart("setQualifier");

	try
	{
		OW_String qualName = "borgishness";

		OW_CIMQualifierType qt(qualName);

		qt.setDataType(OW_CIMDataType::STRING);
		qt.setDefaultValue(OW_CIMValue(OW_String()));

		qt.addScope(OW_CIMScope::CLASS);
		qt.addScope(OW_CIMScope::PROPERTY);

		qt.addFlavor(OW_CIMFlavor::DISABLEOVERRIDE);


		hdl.setQualifierType("root/testsuite", qt);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumerateQualifiers(OW_CIMOMHandleIFC& hdl)
{
	testStart("enumerateQualifiers");

	try
	{
		OW_CIMQualifierTypeEnumeration enu = hdl.enumQualifierTypesE("root/testsuite");
		while (enu.hasMoreElements())
		{
			OW_CIMQualifierType cqt = enu.nextElement();
			cout << "Found Qualifier Definition: " << cqt.getName() << endl;
			cout << cqt.toMOF() << endl;
			OW_TempFileStream tfs;
			OW_CIMtoXML(cqt, tfs);
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getQualifier(OW_CIMOMHandleIFC& hdl)
{
	testStart("getQualifier");

	try
	{
		OW_CIMQualifierType qt = hdl.getQualifierType("root/testsuite", "borgishness");
		cout << "Got Qualifier: " << qt.getName() << endl;
		cout << qt.toMOF() << endl;
		OW_TempFileStream tfs;
		OW_CIMtoXML(qt, tfs);
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

void createAssociation(OW_CIMOMHandleIFC& hdl, const OW_String& assocName,
		const OW_String& propName1, const OW_CIMObjectPath& cop1,
		const OW_String& propName2, const OW_CIMObjectPath& cop2)
{

		OW_CIMClass cc = hdl.getClass("root/testsuite", assocName);
		OW_CIMInstance inst = cc.newInstance();
			
		inst.setProperty(propName1, OW_CIMValue(cop1));

		inst.setProperty(propName2, OW_CIMValue(cop2));

		hdl.createInstance("root/testsuite", inst);

}
//////////////////////////////////////////////////////////////////////////////
void
setupAssociations(OW_CIMOMHandleIFC& hdl)
{
	testStart("setupAssociations");

	try
	{
		OW_CIMObjectPath cop1("EXP_BionicComputerSystem", "root/testsuite");
		cop1.addKey("CreationClassName", OW_CIMValue(OW_String("EXP_BionicComputerSystem")));
		cop1.addKey("Name", OW_CIMValue(OW_String("SevenMillion")));

		OW_CIMObjectPath cop2("EXP_BionicComputerSystem2", "root/testsuite");
		cop2.addKey("CreationClassName", OW_CIMValue(OW_String("EXP_BionicComputerSystem2")));
		cop2.addKey("Name", OW_CIMValue(OW_String("SixMillion")));

		OW_CIMObjectPath cop3("EXP_BionicComputerSystem2", "root/testsuite");
		cop3.addKey("CreationClassName", OW_CIMValue(OW_String("EXP_BionicComputerSystem2")));
		cop3.addKey("Name", OW_CIMValue(OW_String("SevenMillion")));

		OW_CIMObjectPath cop4("EXP_BionicComputerSystem", "root/testsuite");
		cop4.addKey("CreationClassName", OW_CIMValue(OW_String("EXP_BionicComputerSystem")));
		cop4.addKey("Name", OW_CIMValue(OW_String("SixMillion")));

		createAssociation(hdl, "CIM_SystemComponent", "GroupComponent", cop1,
				"PartComponent", cop2);
		createAssociation(hdl, "CIM_SystemDevice", "GroupComponent", cop2,
				"PartComponent", cop1);
		createAssociation(hdl, "CIM_SystemDevice", "GroupComponent", cop2,
				"PartComponent", cop3);
		createAssociation(hdl, "CIM_SystemDevice", "GroupComponent", cop1,
				"PartComponent", cop4);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
associatorNames(OW_CIMOMHandleIFC& hdl, const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole)
{
	OW_String pstr;
	pstr = format("assocClass = %1, resultClass = %2, role = %3, resultRole = %4",
			assocClass, resultClass, role, resultRole);
	testStart("associatorNames", pstr.c_str());

	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem");
		cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BionicComputerSystem")));
		cop.addKey("Name", OW_CIMValue(OW_String("SevenMillion")));

		OW_CIMObjectPathEnumeration enu = hdl.associatorNamesE(
			"root/testsuite", cop, assocClass, resultClass, role, resultRole);

		std::vector<OW_CIMObjectPath> v = std::vector<OW_CIMObjectPath>(
			OW_Enumeration_input_iterator<OW_CIMObjectPath>(enu),
			OW_Enumeration_input_iterator<OW_CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Associated path: " << v[x].toString() << endl;
		}

	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
associatorNamesClass(OW_CIMOMHandleIFC& hdl, const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole)
{
	OW_String pstr;
	pstr = format("assocClass = %1, resultClass = %2, role = %3, resultRole = %4",
			assocClass, resultClass, role, resultRole);
	testStart("associatorNamesClass", pstr.c_str());

	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem");
		
		OW_CIMObjectPathEnumeration enu = hdl.associatorNamesE(
			"root/testsuite", cop, assocClass, resultClass, role, resultRole);

		std::vector<OW_CIMObjectPath> v = std::vector<OW_CIMObjectPath>(
			OW_Enumeration_input_iterator<OW_CIMObjectPath>(enu),
			OW_Enumeration_input_iterator<OW_CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Associated path: " << v[x].toString() << endl;
		}

	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
associators(OW_CIMOMHandleIFC& hdl, const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList)
{
	OW_String pstr;
	pstr = format("assocClass = %1, resultClass = %2, role = %3, resultRole = "
			"%4, includeQualifiers = %5, includeClassOrigin = %6, propertyList? "
			"%7",
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, propertyList != 0);
	testStart("associators", pstr.c_str());

	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem");
		cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BionicComputerSystem")));

		cop.addKey("Name", OW_CIMValue(OW_String("SevenMillion")));

		OW_CIMInstanceEnumeration enu = hdl.associatorsE("root/testsuite", cop,
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, propertyList);

		std::vector<OW_CIMInstance> v = std::vector<OW_CIMInstance>(
			OW_Enumeration_input_iterator<OW_CIMInstance>(enu),
			OW_Enumeration_input_iterator<OW_CIMInstance>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMInstance>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Association Instance: ";
			OW_TempFileStream tfs;
			OW_CIMtoXML(v[x], tfs, OW_CIMObjectPath(),
				OW_CIMtoXMLFlags::isNotInstanceName,
				OW_CIMtoXMLFlags::notLocalOnly,OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
			cout << endl;
			cout << "In MOF: " << v[x].toMOF() << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
associatorsClasses(OW_CIMOMHandleIFC& hdl, const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList)
{
	OW_String pstr;
	pstr = format("assocClass = %1, resultClass = %2, role = %3, resultRole = "
			"%4, includeQualifiers = %5, includeClassOrigin = %6, propertyList? "
			"%7",
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, propertyList != 0);
	testStart("associatorsClasses", pstr.c_str());

	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem");
		OW_CIMClassEnumeration enu = hdl.associatorsClassesE("root/testsuite", cop,
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, propertyList);

		std::vector<OW_CIMClass> v = std::vector<OW_CIMClass>(
			OW_Enumeration_input_iterator<OW_CIMClass>(enu),
			OW_Enumeration_input_iterator<OW_CIMClass>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMClass>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Association Class: ";
			OW_TempFileStream tfs;
			OW_CIMtoXML(v[x], tfs,
				OW_CIMtoXMLFlags::notLocalOnly,OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
			cout << endl;
			cout << "In MOF: " << v[x].toMOF() << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
referenceNames(OW_CIMOMHandleIFC& hdl,
		const OW_String& resultClass, const OW_String& role)
{
	OW_String pstr;
	pstr = format("resultClass = %1, role = %2", resultClass, role);
	testStart("referenceNames", pstr.c_str());

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);
		cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BionicComputerSystem")));
		cop.addKey("Name", OW_CIMValue(OW_String("SevenMillion")));

		OW_CIMObjectPathEnumeration enu = hdl.referenceNamesE("root/testsuite", cop,
				resultClass, role);

		std::vector<OW_CIMObjectPath> v = std::vector<OW_CIMObjectPath>(
			OW_Enumeration_input_iterator<OW_CIMObjectPath>(enu),
			OW_Enumeration_input_iterator<OW_CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Associated path: " << v[x].toString() << endl;
		}

	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
referenceNamesClass(OW_CIMOMHandleIFC& hdl,
		const OW_String& resultClass, const OW_String& role)
{
	OW_String pstr;
	pstr = format("resultClass = %1, role = %2", resultClass, role);
	testStart("referenceNamesClass", pstr.c_str());

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);

		OW_CIMObjectPathEnumeration enu = hdl.referenceNamesE("root/testsuite", cop,
				resultClass, role);

		std::vector<OW_CIMObjectPath> v = std::vector<OW_CIMObjectPath>(
			OW_Enumeration_input_iterator<OW_CIMObjectPath>(enu),
			OW_Enumeration_input_iterator<OW_CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Associated path: " << v[x].toString() << endl;
		}

	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
references(OW_CIMOMHandleIFC& hdl,
		const OW_String& resultClass, const OW_String& role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_String pstr;
	pstr = format("resultClass = %1, role = %2, includeQualifiers = %3, "
			"includeClassOrigin = %4, propertyList? %5",
			resultClass, role, includeQualifiers, includeClassOrigin,
			propertyList != 0);
	testStart("references", pstr.c_str());

	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem");
		cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BionicComputerSystem")));
		cop.addKey("Name", OW_CIMValue(OW_String("SevenMillion")));

		OW_CIMInstanceEnumeration enu = hdl.referencesE("root/testsuite", cop,
				resultClass, role, includeQualifiers, includeClassOrigin,
				propertyList);

		std::vector<OW_CIMInstance> v = std::vector<OW_CIMInstance>(
			OW_Enumeration_input_iterator<OW_CIMInstance>(enu),
			OW_Enumeration_input_iterator<OW_CIMInstance>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMInstance>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Association Instance: ";
			OW_TempFileStream tfs;
			OW_CIMtoXML(v[x], tfs, OW_CIMObjectPath(),
				OW_CIMtoXMLFlags::isNotInstanceName,
				OW_CIMtoXMLFlags::notLocalOnly,OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
			cout << endl;
			cout << "In MOF: " << v[x].toMOF() << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
referencesClasses(OW_CIMOMHandleIFC& hdl,
		const OW_String& resultClass, const OW_String& role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_String pstr;
	pstr = format("resultClass = %1, role = %2, includeQualifiers = %3, "
			"includeClassOrigin = %4, propertyList? %5",
			resultClass, role, includeQualifiers, includeClassOrigin,
			propertyList != 0);
	testStart("referencesClasses", pstr.c_str());

	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem");

		OW_CIMClassEnumeration enu = hdl.referencesClassesE("root/testsuite", cop,
				resultClass, role, includeQualifiers, includeClassOrigin,
				propertyList);

		std::vector<OW_CIMClass> v = std::vector<OW_CIMClass>(
			OW_Enumeration_input_iterator<OW_CIMClass>(enu),
			OW_Enumeration_input_iterator<OW_CIMClass>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMClass>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Referencing Class: ";
			OW_TempFileStream tfs;
			OW_CIMtoXML(v[x], tfs,
				OW_CIMtoXMLFlags::notLocalOnly,OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
			cout << endl;
			OW_CIMClass cc = v[x];
			cout << "In MOF: " << cc.toMOF() << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
execQuery(OW_CIMOMHandleIFC& hdl)
{
	testStart("execQuery");

	try
	{
		OW_CIMInstanceEnumeration cie = hdl.execQueryE("root/testsuite",
			"select * from EXP_BionicComputerSystem", "wql1");
		while (cie.hasMoreElements())
		{
			OW_CIMInstance i = cie.nextElement();
			cout << "Instance from Query: ";
			OW_TempFileStream tfs;
			OW_CIMtoXML(i, tfs, OW_CIMObjectPath(),
				OW_CIMtoXMLFlags::isNotInstanceName,
				OW_CIMtoXMLFlags::notLocalOnly,OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,OW_StringArray());
			tfs.rewind();
			cout << OW_XMLPrettyPrint(tfs);
			cout << "In MOF: " << i.toMOF() << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteQualifier(OW_CIMOMHandleIFC& hdl)
{
	testStart("deleteQualifier");

	try
	{
		hdl.deleteQualifierType("root/testsuite", "borgishness");
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteClass(OW_CIMOMHandleIFC& hdl, const OW_String& delClass)
{
	testStart("deleteClass");

	try
	{
		hdl.deleteClass("root/testsuite", delClass);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

void
prepareGetStateParams(OW_CIMParamValueArray& in, const OW_CIMObjectPath& cop)
{
	// MOF of this method:
	//    string GetState(
	//		[in] string s,
	//		[in] uint8 uint8array[],
	//		[out] boolean b,
	//		[out] real64 r64,
	//		[in, out] sint16 io16,
	//		[out] string msg,
	//		[in, out] CIM_ComputerSystem REF paths[],
	//		[in, out] datetime nullParam);

	// These are out of order on purpose, to test that the CIMOM will order them correctly.
	in.push_back(OW_CIMParamValue("io16", OW_CIMValue(OW_Int16(16))));
	in.push_back(OW_CIMParamValue("nullParam", OW_CIMValue()));
	in.push_back(OW_CIMParamValue("s", OW_CIMValue(OW_String("input string"))));
	OW_UInt8Array uint8array;
	uint8array.push_back(1);
	uint8array.push_back(255);
	uint8array.push_back(0);
	uint8array.push_back(128);
	uint8array.push_back(0);
	in.push_back(OW_CIMParamValue("uint8array", OW_CIMValue(uint8array)));
	OW_CIMObjectPathArray paths;
	OW_CIMObjectPath copWithNS(cop);
	copWithNS.setNameSpace("root/testsuite");
	paths.push_back(copWithNS);
	OW_CIMObjectPath cop2(copWithNS);
	cop2.addKey("name", OW_CIMValue("foo"));
	paths.push_back(cop2);
	in.push_back(OW_CIMParamValue("paths", OW_CIMValue(paths)));

}

//////////////////////////////////////////////////////////////////////////////
void
invokeMethod(OW_CIMOMHandleIFC& hdl, int num)
{
	testStart("invokeMethod");

	try
	{
		OW_CIMObjectPath cop("EXP_BartComputerSystem");

		OW_String rval;
		OW_CIMParamValueArray in, out;
		OW_CIMValue cv;
		switch (num)
		{
			case 1:
                cop.addKey("CreationClassName",
                      OW_CIMValue(OW_String("EXP_BartComputerSystem")));
                cop.addKey("Name", OW_CIMValue(OW_String("test")));
				in.push_back(OW_CIMParamValue("newState", OW_CIMValue(OW_String("off"))));
				hdl.invokeMethod("root/testsuite", cop, "setstate", in, out);
				cout << "invokeMethod: setstate(\"off\")" << endl;
				break;
			case 2:
			{
				prepareGetStateParams(in,cop);
				cv = hdl.invokeMethod("root/testsuite", cop, "getstate", in, out);
				cv.get(rval);

				cout << "invokeMethod: getstate(): " << rval << endl;
				cout << out.size() << " out params:\n";
				for (size_t i = 0; i < out.size(); ++i)
				{
					cout << "param " << i << ": " << out[i].toString() << '\n';
				}
				break;
			}
			case 3:
				hdl.invokeMethod("root/testsuite", cop, "togglestate", in, out);
				cout << "invokeMethod: togglestate()" << endl;
				break;
			case 4:
				prepareGetStateParams(in,cop);
				cv = hdl.invokeMethod("root/testsuite", cop, "getstate", in, out);
				cv.get(rval);
				cout << "invokeMethod: getstate(): " << rval << endl;
				break;
			case 5:
				in.push_back(OW_CIMParamValue("newState", OW_CIMValue(OW_String("off"))));
				hdl.invokeMethod("root/testsuite", cop, "setstate", in, out);
				cout << "invokeMethod: setstate(\"off\")" << endl;
				break;
			case 6:
				prepareGetStateParams(in,cop);
				cv = hdl.invokeMethod("root/testsuite", cop, "getstate", in, out);
				cv.get(rval);
				cout << "invokeMethod: getstate(): " << rval << endl;
				break;
			default:
				break;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
createNameSpace(OW_CIMOMHandleIFC& hdl)
{
	testStart("createNameSpace");

	try
	{
		hdl.createNameSpace("root/testsuite/Caldera");
		hdl.createNameSpace("root/testsuite/Caldera/test");
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumNameSpace(OW_CIMOMHandleIFC& hdl)
{
	testStart("enumNamespace");

	try
	{
		cout << "deep = false" << endl;
		OW_StringArray rval = hdl.enumNameSpaceE("root/testsuite", OW_Bool(false));
		for (size_t i = 0; i < rval.size(); i++)
		{
			cout << "Namespace: " << rval[i] << endl;
		}
		
		cout << "deep = true" << endl;
		rval = hdl.enumNameSpaceE("root/testsuite", OW_Bool(true));
		for (size_t i = 0; i < rval.size(); i++)
		{
			cout << "Namespace: " << rval[i] << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteNameSpace(OW_CIMOMHandleIFC& hdl)
{
	testStart("deleteNameSpace");

	try
	{
		hdl.deleteNameSpace("root/testsuite/Caldera");
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getProperty(OW_CIMOMHandleIFC& hdl, const OW_String& instName)
{
	testStart("getProperty");

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(OW_String(instName)));

		OW_CIMValue v = hdl.getProperty("root/testsuite", cop, "OptionalArg");
		// with xml, this is a string.  we want a bool.
		v = OW_CIMValueCast::castValueToDataType(v, OW_CIMDataType::BOOLEAN);
		cout << "** getProperty returned. CIMValue: " << v.toMOF() << endl;
		OW_TempFileStream tfs;
		OW_CIMtoXML(v, tfs);
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
setProperty(OW_CIMOMHandleIFC& hdl, const OW_String& instName)
{
	testStart("setProperty");

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(instName));

		hdl.setProperty("root/testsuite", cop, "OptionalArg", OW_CIMValue(OW_Bool(true)));
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}



/****************************************************************************
 * This is the class that will be used to obtain authentication credentials
 * if none are provided in the URL used by the HTTP Client.
 ****************************************************************************/
class GetLoginInfo : public OW_ClientAuthCBIFC
{
	public:
		OW_Bool getCredentials(const OW_String& realm, OW_String& name,
				OW_String& passwd, const OW_String& details)
		{
			(void)details;
			cout << "Authentication required for " << realm << endl;
			cout << "Enter the user name: ";
			name = OW_String::getLine(cin);
			passwd = OW_GetPass::getPass("Enter the password for " +
				name + ": ");
			return OW_Bool(true);
		}
};

/****************************************************************************
 * This is the prototype for our SSL certificate verification function.
 * This function is called during the client connect of the SSL handshake.
 * It should return 1 if the certificate is to be accepted, and 0 if it
 * is to rejected (and the connection should not be established).
 * This function could check the certificate against a list of accepted
 * Certificate Authorities or something.  Ours will simply display
 * the certificate and ask the user if he/she wishes to accept it.
 ****************************************************************************/
#ifdef OW_HAVE_OPENSSL
int ssl_verifycert_callback(X509* cert);
#endif

//////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			usage(argv[0]);
			return 1;
		}

		if (argc == 3)
		{
			OW_String sockDumpOut = argv[2];
			OW_String sockDumpIn = argv[2];
			sockDumpOut += "SockDumpOut";
			sockDumpIn += "SockDumpIn";
			OW_SocketBaseImpl::setDumpFiles(sockDumpIn.c_str(),
				sockDumpOut.c_str());
		}
		else
		{
			OW_SocketBaseImpl::setDumpFiles("","");
		}

		OW_String url(argv[1]);
		OW_URL owurl(url);

		/**********************************************************************
		 * We assign our SSL certificate callback into the OW_SSLCtxMgr.
		 * If we don't do this, we'll accept any server certificate without
		 * any verification.  We leave this commented out here, so our
		 * acceptance test will run without user interaction.
		 **********************************************************************/

#ifdef OW_HAVE_OPENSSL
		//OW_SSLCtxMgr::setCertVerifyCallback(ssl_verifycert_callback);
#endif
		
		
		/**********************************************************************
		 * Here we create the concrete OW_CIMProtocol that we want
		 * our OW_CIMXMLCIMOMHandle to use.  We'll use the OW_HTTPClient
		 * (capable of handling HTTP/1.1 and HTTPS -- HTTP over SSL).
		 * The OW_HTTPClient takes a URL in it's constructor, representing
		 * the CIM Server that it will connect to.  A URL has the form
		 *   http[s]://[USER:PASSWORD@]HOSTNAME[:PORT][/PATH].
		 *
		 * Example:  https://bill:secret@managedhost.example.com/cimom
		 *
		 * If no port is given, the defaults are used: 5988 for HTTP,
		 * and 5989 for HTTPS.  If no username and password are given,
		 * and the CIM Server requires authentication, a callback may
		 * be provided to retrieve authentication credentials.
		 **********************************************************************/

		OW_CIMProtocolIFCRef client(new OW_HTTPClient(url));


		/**********************************************************************
		 * Create an instance of our authentication callback class.
		 **********************************************************************/
		
		OW_ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);

		/**********************************************************************
		 * Assign our callback to the HTTP Client.
		 **********************************************************************/

		client->setLoginCallBack(getLoginInfo);

		/**********************************************************************
		 * Here we create a OW_CIMXMLCIMOMHandle and have it use the
		 * OW_HTTPClient we've created.  OW_CIMXMLCIMOMHandle takes
		 * a OW_Reference<OW_CIMProtocol> it it's constructor, so
		 * we have to make a OW_Reference out of our HTTP Client first.
		 * By doing this, we don't have to worry about deleting our
		 * OW_HTTPClient.  OW_Reference will delete it for us when the
		 * last copy goes out of scope (reference count goes to zero).
		 **********************************************************************/

		OW_CIMOMHandleIFCRef chRef;
		if (owurl.path.equalsIgnoreCase("/owbinary"))
		//cout << "owurl.path = " << owurl.path << endl;
		//if (owurl.path.equalsIgnoreCase("/binary"))
		{
			chRef = new OW_BinaryCIMOMHandle(client);
		}
		else
		{
			chRef = new OW_CIMXMLCIMOMHandle(client);
		}

		OW_CIMOMHandleIFC& rch = *chRef;

		/**********************************************************************
		 * Now we have essentially established a "connection" to the CIM
		 * Server.  We can access the methods on the remote CIMOM handle,
		 * and these methods will call into the CIM Server.  The OW_HTTPClient
		 * will take care of the particulars of the HTTP protocol, including
		 * authentication, compression, SSL, chunking, etc.
		 **********************************************************************/

		//foobar(rch);

		createNameSpace(rch);
		enumNameSpace(rch);
		deleteNameSpace(rch);
		createClass(rch, "EXP_BionicComputerSystem");
		createClass(rch, "EXP_BionicComputerSystem2");

		if (getenv("OWLONGTEST"))
		{
			enumClassNames(rch);
			enumClasses(rch);
		}

		modifyClass(rch);
		getClass(rch);
		createInstance(rch, "EXP_BionicComputerSystem", "SixMillion");
		createInstance(rch, "EXP_BionicComputerSystem", "SevenMillion");
		createInstance(rch, "EXP_BionicComputerSystem2", "SixMillion");
		createInstance(rch, "EXP_BionicComputerSystem2", "SevenMillion");
		enumerateInstanceNames(rch);
		// non-deep, non-localOnly, no qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", false, false, false, false, 0);	
		// deep, non-localOnly, no qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", true, false, false, false, 0);
		// deep, localOnly, no qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", true, true, false, false, 0);
		// deep, non-localOnly, qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", true, false, true, false, 0);
		// deep, non-localOnly, no qualifiers, classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", true, false, false, true, 0);
		// deep, non-localOnly, no qualifiers, no classOrigin, no props
		OW_StringArray sa;
		enumerateInstances(rch, "CIM_ComputerSystem", true, false, false, false, &sa);
		// deep, non-localOnly, no qualifiers, no classOrigin, one prop
		sa.push_back(OW_String("BrandNewProperty"));
		enumerateInstances(rch, "CIM_ComputerSystem", true, false, false, false, &sa);

		enumerateInstances(rch, "Example_C1", true,true,false,true,0);
		enumerateInstances(rch, "Example_C1", true,false,false,true,0);
		enumerateInstances(rch, "Example_C1", false,true,false,true,0);
		enumerateInstances(rch, "Example_C1", false,false,false,true,0);
		enumerateInstances(rch, "Example_C2", true,true,false,true,0);
		enumerateInstances(rch, "Example_C2", true,false,false,true,0);
		enumerateInstances(rch, "Example_C2", false,true,false,true,0);
		enumerateInstances(rch, "Example_C2", false,false,false,true,0);
		enumerateInstances(rch, "Example_C3", true,true,false,true,0);
		enumerateInstances(rch, "Example_C3", true,false,false,true,0);
		enumerateInstances(rch, "Example_C3", false,true,false,true,0);
		enumerateInstances(rch, "Example_C3", false,false,false,true,0);

		getInstance(rch, "SixMillion");
		getInstance(rch, "SevenMillion");
		// localOnly = true
		getInstance(rch, "SevenMillion", true, false, false, 0);
		// includeQualifiers = true
		getInstance(rch, "SevenMillion", false, true, false, 0);
		// includeClassOrigin = true
		getInstance(rch, "SevenMillion", false, false, true, 0);
		// propertyList = non-null
		sa.clear();
		getInstance(rch, "SevenMillion", false, false, false, &sa);
		// propertyList = non-null, with element
		sa.push_back(OW_String("BrandNewProperty"));
		getInstance(rch, "SevenMillion", false, false, false, &sa);

		modifyInstance(rch, "SixMillion");
		getInstance(rch, "SixMillion");
		modifyInstance(rch, "SevenMillion");
		getInstance(rch, "SevenMillion");
		setProperty(rch, "SixMillion");
		getProperty(rch, "SixMillion");
		getInstance(rch, "SixMillion");
		setQualifier(rch);
		enumerateQualifiers(rch);
		getQualifier(rch);

		if (getenv("OWLONGTEST"))
		{
			setupAssociations(rch);

            associatorNames(rch, "", "", "", "");
            associatorNames(rch, "CIM_SystemDevice", "", "", "");
            associatorNames(rch, "", "CIM_ComputerSystem", "", "");
            associatorNames(rch, "", "EXP_BionicComputerSystem", "", "");
            associatorNames(rch, "", "", "GroupComponent", "");
            associatorNames(rch, "", "", "PartComponent", "");
            associatorNames(rch, "", "", "PartComponent", "PartComponent");
            associatorNames(rch, "", "", "", "PartComponent");
            associatorNames(rch, "", "", "", "GroupComponent");
            associatorNamesClass(rch, "", "", "", "");
            associatorNamesClass(rch, "CIM_SystemDevice", "", "", "");
            associatorNamesClass(rch, "", "CIM_ComputerSystem", "", "");
            associatorNamesClass(rch, "", "EXP_BionicComputerSystem", "", "");
            associatorNamesClass(rch, "", "", "GroupComponent", "");
            associatorNamesClass(rch, "", "", "PartComponent", "");
            associatorNamesClass(rch, "", "", "PartComponent", "PartComponent");
            associatorNamesClass(rch, "", "", "", "PartComponent");
            associatorNamesClass(rch, "", "", "", "GroupComponent");

            associators(rch, "", "", "", "", false, false, 0);
            associators(rch, "CIM_SystemDevice", "", "", "", false, false, 0);
            associators(rch, "", "CIM_ComputerSystem", "", "", false, false, 0);
            associators(rch, "", "EXP_BionicComputerSystem", "", "", false, false, 0);
            associators(rch, "", "", "GroupComponent", "", false, false, 0);
            associators(rch, "", "", "PartComponent", "", false, false, 0);
            associators(rch, "", "", "PartComponent", "PartComponent", false, false, 0);
            associators(rch, "", "", "", "PartComponent", false, false, 0);
            associators(rch, "", "", "", "GroupComponent", false, false, 0);
            associators(rch, "", "", "", "", true, false, 0);
            associators(rch, "", "", "", "", false, true, 0);
            sa.clear();
            associators(rch, "", "", "", "", false, false, &sa);
            sa.push_back(OW_String("BrandNewProperty"));
            associators(rch, "", "", "", "", false, false, &sa);

            associatorsClasses(rch, "", "", "", "", false, false, 0);
            associatorsClasses(rch, "CIM_SystemDevice", "", "", "", false, false, 0);
            associatorsClasses(rch, "", "CIM_ComputerSystem", "", "", false, false, 0);
            associatorsClasses(rch, "", "EXP_BionicComputerSystem", "", "", false, false, 0);
            associatorsClasses(rch, "", "", "GroupComponent", "", false, false, 0);
            associatorsClasses(rch, "", "", "PartComponent", "", false, false, 0);
            associatorsClasses(rch, "", "", "PartComponent", "PartComponent", false, false, 0);
            associatorsClasses(rch, "", "", "", "PartComponent", false, false, 0);
            associatorsClasses(rch, "", "", "", "GroupComponent", false, false, 0);
            associatorsClasses(rch, "", "", "", "", true, false, 0);
            associatorsClasses(rch, "", "", "", "", false, true, 0);
            sa.clear();
            associatorsClasses(rch, "", "", "", "", false, false, &sa);
            sa.push_back(OW_String("BrandNewProperty"));
            associatorsClasses(rch, "", "", "", "", false, false, &sa);

            referenceNames(rch, "", "");
            referenceNames(rch, "cim_systemdevice", "");
            referenceNames(rch, "cim_component", "");
            referenceNames(rch, "", "GroupComponent");
            referenceNames(rch, "", "PartComponent");

            referenceNamesClass(rch, "", "");
            referenceNamesClass(rch, "cim_systemdevice", "");
            referenceNamesClass(rch, "cim_component", "");
            referenceNamesClass(rch, "", "GroupComponent");
            referenceNamesClass(rch, "", "PartComponent");

			references(rch, "", "", false, false, 0);
			references(rch, "cim_systemdevice", "", false, false, 0);
			references(rch, "cim_component", "", false, false, 0);
			references(rch, "", "GroupComponent", false, false, 0);
			references(rch, "", "PartComponent", false, false, 0);
			references(rch, "", "", true, false, 0);
			references(rch, "", "", false, true, 0);
			sa.clear();
			references(rch, "", "", false, false, &sa);
			sa.push_back(OW_String("GroupComponent"));
			references(rch, "", "", false, false, &sa);

			referencesClasses(rch, "", "", false, false, 0);
			referencesClasses(rch, "cim_systemdevice", "", false, false, 0);
			referencesClasses(rch, "cim_component", "", false, false, 0);
			referencesClasses(rch, "", "GroupComponent", false, false, 0);
			referencesClasses(rch, "", "PartComponent", false, false, 0);
			referencesClasses(rch, "", "", true, false, 0);
			referencesClasses(rch, "", "", false, true, 0);
			sa.clear();
			referencesClasses(rch, "", "", false, false, &sa);
			sa.push_back(OW_String("GroupComponent"));
			referencesClasses(rch, "", "", false, false, &sa);

			execQuery(rch);
			deleteAssociations(rch);
		}

		deleteInstance(rch, "EXP_BionicComputerSystem", "SixMillion");
		deleteInstance(rch, "EXP_BionicComputerSystem", "SevenMillion");
		deleteInstance(rch, "EXP_BionicComputerSystem2", "SixMillion");
		deleteInstance(rch, "EXP_BionicComputerSystem2", "SevenMillion");
		deleteClass(rch, "EXP_BionicComputerSystem");
		deleteClass(rch, "EXP_BionicComputerSystem2");
		deleteQualifier(rch);

		invokeMethod(rch, 1);
		invokeMethod(rch, 2);
		invokeMethod(rch, 3);
		invokeMethod(rch, 4);
		invokeMethod(rch, 5);
		invokeMethod(rch, 6);

		return 0;

	}
	catch (OW_Assertion& a)
	{
		cerr << "Caught Assertion: " << a << endl;
	}
	catch (OW_Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
#ifdef OW_HAVE_OPENSSL
void display_name(const char* prefix, X509_NAME* name)
{
    char buf[256];

    X509_NAME_get_text_by_NID(name,
                              NID_organizationName,
                              buf,
                              256);
    printf("%s%s\n",prefix,buf);
    X509_NAME_get_text_by_NID(name,
                              NID_organizationalUnitName,
                              buf,
                              256);
    printf("%s%s\n",prefix,buf);
    X509_NAME_get_text_by_NID(name,
                              NID_commonName,
                              buf,
                              256);
    printf("%s%s\n",prefix,buf);
    X509_NAME_get_text_by_NID(name,
                              NID_pkcs9_emailAddress,
                              buf,
                              256);
    printf("%s%s\n",prefix,buf);
}

/////////////////////////////////////////////////////////////////////////////
void display_cert(X509* cert)
{
    X509_NAME*      name;
    int             unsigned i = 16;
    unsigned char   digest[16];

    cout << endl;

    /* print the issuer */
    printf("   issuer:\n");
    name = X509_get_issuer_name(cert);
    display_name("      ",name);

    /* print the subject */
    name = X509_get_subject_name(cert);
    printf("   subject:\n");
    display_name("      ",name);

    /* print the fingerprint */
    X509_digest(cert,EVP_md5(),digest,&i);
    printf("   fingerprint:\n");
    printf("      ");
    for(i=0;i<16;i++)
    {
        printf("%02X",digest[i]);
        if(i != 15)
        {
            printf(":");
        }
    }
    printf("\n");
}

//////////////////////////////////////////////////////////////////////////////
int ssl_verifycert_callback(X509* cert)
{
	static bool     bPrompted = false;

	if(!bPrompted)
	{
		X509   *pX509Cert = cert;

		bPrompted = true;

		cout << "The SSL connection received the following certificate:" << endl;

		display_cert(pX509Cert);

		cout << "\nDo you want to accept this Certificate (Y/N)? ";
		OW_String response = OW_String::getLine(cin);
		if(response.compareToIgnoreCase("Y") != 0)
			return 0;
	}
	return 1;
}

#endif
