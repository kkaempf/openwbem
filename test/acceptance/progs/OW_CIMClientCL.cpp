/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_Assertion.hpp"
#include "OW_CIMClient.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMDataType.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/GetPass.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"
#include "OW_XMLPrettyPrint.hpp"
#include "OW_CIMParamValue.hpp"
#include "blocxx/Socket.hpp"
#include "OW_URL.hpp"
#ifdef OW_HAVE_OPENSSL
#include <openssl/ssl.h>
#endif

#include <iostream>
#include <algorithm> // for sort

#define TEST_ASSERT(CON) if (!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using namespace OpenWBEM;
using namespace WBEMFlags;

//////////////////////////////////////////////////////////////////////////////
void
testStart(const char* funcName, const char* parm=NULL)
{
	cout << "*** Doing " << funcName << '(';
	if (parm)
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
createClass(CIMClient& hdl, const String& name)
{
	testStart("createClass");

	try
	{
		String cqtPath("Key");
		CIMQualifierType cqt = hdl.getQualifierType( cqtPath);
		CIMQualifier cimQualifierKey(cqt);

		cimQualifierKey.setValue(CIMValue(Bool(true)));
		CIMClass cimClass;
		cimClass.setName(name);
		cimClass.setSuperClass("CIM_System");

		Array<String> zargs;
		zargs.push_back("CreationClassName");
		zargs.push_back("string");
		zargs.push_back("true");
		zargs.push_back("Name");
		zargs.push_back("string");
		zargs.push_back("true");
		zargs.push_back("OptionalArg");
		zargs.push_back("boolean");
		zargs.push_back("false");

		String name;
		String type;
		String isKey;

		for (size_t i = 0; i < zargs.size(); i += 3)
		{
			name = zargs[i];
			type = zargs[i + 1];
			isKey = zargs[i + 2];
			CIMProperty cimProp;
			if (type.equals("string"))
			{
				cimProp.setDataType(CIMDataType::STRING);
			}
			else if (type.equals("uint16"))
			{
				cimProp.setDataType(CIMDataType::UINT16);
			}
			else if (type.equals("boolean"))
			{
				cimProp.setDataType(CIMDataType::BOOLEAN);
			}
			else if (type.equals("datatime"))
			{
				cimProp.setDataType(CIMDataType::DATETIME);
			}
			else cimProp.setDataType(CIMDataType::STRING);

			cimProp.setName(name);
			if (isKey.equals("true"))
			{
				cimProp.addQualifier(cimQualifierKey);
			}
			cimClass.addProperty(cimProp);
		}

		hdl.createClass( cimClass);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumClassNames(CIMClient& hdl)
{
	testStart("enumClassNames");

	try
	{
		StringEnumeration enu = hdl.enumClassNamesE( "", E_DEEP);
		while (enu.hasMoreElements())
		{
			cout << "CIMClass: " << enu.nextElement() << endl;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "Testing non-deep" << endl;

	try
	{
		StringEnumeration enu = hdl.enumClassNamesE( "", E_SHALLOW);
		while (enu.hasMoreElements())
		{
			cout << "CIMClass: " << enu.nextElement() << endl;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumClasses(CIMClient& hdl)
{
	testStart("enumClasses");

	cout << "deep = true, localOnly = false" << endl;
	try
	{
		CIMClassEnumeration enu = hdl.enumClassE( "", E_DEEP, E_NOT_LOCAL_ONLY);
		while (enu.hasMoreElements())
		{
			CIMClass c = enu.nextElement();
			cout << "CIMClass: " << c.toMOF() << endl;
			TempFileStream tfs;
			CIMtoXML(c,tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "deep = false, localOnly = false" << endl;
	try
	{
		CIMClassEnumeration enu = hdl.enumClassE( "", E_SHALLOW, E_NOT_LOCAL_ONLY);
		while (enu.hasMoreElements())
		{
			CIMClass c = enu.nextElement();
			cout << "CIMClass: " << c.toMOF() << endl;
			TempFileStream tfs;
			CIMtoXML(c,tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "deep = false, localOnly = true" << endl;
	try
	{
		CIMClassEnumeration enu = hdl.enumClassE( "", E_SHALLOW, E_LOCAL_ONLY);
		while (enu.hasMoreElements())
		{
			CIMClass c = enu.nextElement();
			cout << "CIMClass: " << c.toMOF() << endl;
			TempFileStream tfs;
			CIMtoXML(c,tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "deep = true, localOnly = true" << endl;
	try
	{
		CIMClassEnumeration enu = hdl.enumClassE( "", E_DEEP, E_LOCAL_ONLY);
		while (enu.hasMoreElements())
		{
			CIMClass c = enu.nextElement();
			cout << "CIMClass: " << c.toMOF() << endl;
			TempFileStream tfs;
			CIMtoXML(c,tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
modifyClass(CIMClient& hdl)
{
	testStart("modifyClass");

	try
	{
		CIMClass cimClass = hdl.getClass(
			"EXP_BionicComputerSystem");
		cout << "CIMClass before: " << cimClass.toMOF() << endl;
		TempFileStream tfs;
		CIMtoXML(cimClass,tfs);
		tfs << '\n';
		cout << XMLPrettyPrint(tfs);

		CIMProperty cimProp;
		cimProp.setDataType(CIMDataType::STRING);
		cimProp.setName("BrandNewProperty");
		cimClass.addProperty(cimProp);
		hdl.modifyClass( cimClass);

		cimClass = hdl.getClass( "EXP_BionicComputerSystem");
		cout << "CIMClass after: " << cimClass.toMOF() << endl;
		tfs.reset();
		CIMtoXML(cimClass,tfs);
		tfs << '\n';
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);

	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getClass(CIMClient& hdl)
{
	testStart("getClass");

	cout << "localOnly = false" << endl;
	try
	{
		CIMClass cimClass = hdl.getClass(
			"EXP_BionicComputerSystem", E_NOT_LOCAL_ONLY);
		cout << "CIMClass: " << cimClass.toMOF() << endl;
		TempFileStream tfs;
		CIMtoXML(cimClass,tfs);
		tfs << '\n';
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "localOnly = true" << endl;
	try
	{
		CIMClass cimClass = hdl.getClass(
			"EXP_BionicComputerSystem", E_LOCAL_ONLY);
		cout << "CIMClass: " << cimClass.toMOF() << endl;
		TempFileStream tfs;
		CIMtoXML(cimClass,tfs);
		tfs << '\n';
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
testDynInstances(CIMClient& hdl)
{
	testStart("testDynInstances");
	try
	{
		CIMClass cc = hdl.getClass( "testinstance");
		CIMInstance ci = cc.newInstance();
		ci.setProperty("name", CIMValue(String("one")));
		StringArray params;
		params.push_back("one");
		params.push_back("two");
		ci.setProperty("params", CIMValue(params));
		CIMObjectPath cop1 = hdl.createInstance( ci);
		ci = hdl.getInstance(cop1);

		// Check Content-Language
		String cl;
		TEST_ASSERT(hdl.getHTTPResponseHeader("Content-Language", cl));
		TEST_ASSERT(cl == "x-testinst");
		TEST_ASSERT(ci.getLanguage() == "x-owtest");
		
		TempFileStream tfs;
		tfs << "<CIM>";
		CIMInstancePathAndInstancetoXML(ci, tfs, cop1);
		tfs << '\n';
		tfs << "</CIM>";
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);

		ci = cc.newInstance();
		ci.setProperty("name", CIMValue(String("two")));
		params.clear();
		params.push_back("A");
		params.push_back("B");
		params.push_back("C");
		ci.setProperty("params", CIMValue(params));
		CIMObjectPath cop2 = hdl.createInstance( ci);
		ci = hdl.getInstance(cop2);
		tfs.reset();
		tfs << "<CIM>";
		CIMInstancePathAndInstancetoXML(ci, tfs, cop2);
		tfs << '\n';
		tfs << "</CIM>";
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);

		params.clear();
		params.push_back("uno");
		params.push_back("dos");
		ci = cc.newInstance();
		ci.setProperty("name", CIMValue(String("one")));
		ci.setProperty("params", CIMValue(params));
		hdl.modifyInstance( ci);
		ci = hdl.getInstance(cop1);
		tfs.reset();
		tfs << "<CIM>";
		CIMInstancePathAndInstancetoXML(ci, tfs, cop1);
		tfs << '\n';
		tfs << "</CIM>";
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);

		CIMInstanceEnumeration enu = hdl.enumInstancesE(
			"testinstance");
		TEST_ASSERT(enu.numberOfElements() == 2);
		TEST_ASSERT(hdl.getHTTPResponseHeader("Content-Language", cl));
		TEST_ASSERT(cl == "x-testinst");

		hdl.deleteInstance( cop1);
		enu = hdl.enumInstancesE( "testinstance");
		TEST_ASSERT(enu.numberOfElements() == 1);

		hdl.deleteInstance( cop2);
		enu = hdl.enumInstancesE( "testinstance");
		TEST_ASSERT(enu.numberOfElements() == 0);
		
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}
	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
testModifyProviderQualifier(CIMClient& hdl)
{
	testStart("testModifyProviderQualifier");
	try
	{
		CIMClass cc = hdl.getClass( "testinstance");
		CIMInstance ci = cc.newInstance();
		ci.setProperty("name", CIMValue(String("one")));
		StringArray params;
		params.push_back("one");
		params.push_back("two");
		ci.setProperty("params", CIMValue(params));
		CIMObjectPath cop1 = hdl.createInstance( ci);
		ci = hdl.getInstance(cop1);

		CIMQualifier provQual = cc.getQualifier("provider");
		TEST_ASSERT(!provQual); // shouldn't be there since the provider registers itself.
		provQual = CIMQualifier("provider");
		provQual.setValue(CIMValue("somejunk")); // the provider qualifier should be ignored.
		cc.addQualifier(provQual);
		hdl.modifyClass(cc);

		ci = hdl.getInstance(cop1);

		cc.removeQualifier(provQual);
		hdl.modifyClass(cc); // change it back

		CIMInstanceEnumeration enu = hdl.enumInstancesE(
			"testinstance");
		TEST_ASSERT(enu.numberOfElements() == 1);

		hdl.deleteInstance( cop1);
		enu = hdl.enumInstancesE( "testinstance");
		TEST_ASSERT(enu.numberOfElements() == 0);
		

		// now test the old behavior.  Create a new class that the provider
		// didn't register for, but we'll have the qualifier point to the
		// provider.
		cc = hdl.getClass( "testinstance");
		cc.setName("testinstance2");
		provQual.setValue(CIMValue("c++::testinstance"));
		cc.addQualifier(provQual);
		hdl.createClass(cc);
		ci = cc.newInstance();
		ci.setProperty("name", CIMValue(String("one")));
		params.clear();
		params.push_back("one");
		params.push_back("two");
		ci.setProperty("params", CIMValue(params));
		cop1 = hdl.createInstance( ci);
		ci = hdl.getInstance(cop1);

		provQual = cc.getQualifier("provider");
		cc.removeQualifier(provQual);
		hdl.modifyClass(cc);

		try
		{
			// should throw since we removed the qualifier and the provider
			// won't be called
			ci = hdl.getInstance(cop1);
			TEST_ASSERT(0);
		}
		catch (const CIMException& e)
		{
			TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
		}

		cc.addQualifier(provQual);
		hdl.modifyClass(cc); // change it back

		enu = hdl.enumInstancesE("testinstance");
		TEST_ASSERT(enu.numberOfElements() == 1);

		hdl.deleteInstance( cop1);
		enu = hdl.enumInstancesE( "testinstance");
		TEST_ASSERT(enu.numberOfElements() == 0);
		hdl.deleteClass("testinstance2");
		
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}
	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
createInstance(CIMClient& hdl, const String& fromClass, const String& newInstance)
{
	testStart("createInstance");

	try
	{
		CIMClass cimClass = hdl.getClass( fromClass);

		CIMInstance newInst = cimClass.newInstance();

		newInst.setProperty(CIMProperty::NAME_PROPERTY,
								  CIMValue(newInstance));
		newInst.setProperty("CreationClassName",
								  CIMValue(fromClass));

		CIMObjectPath cop = hdl.createInstance( newInst);
		cout << "cop = " << cop << endl;
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumerateInstanceNames(CIMClient& hdl)
{
	testStart("enumInstanceNames");

	try
	{
		String ofClass = "CIM_System";
		CIMObjectPathEnumeration enu = hdl.enumInstanceNamesE( ofClass);
		while (enu.hasMoreElements())
		{
			CIMObjectPath cop = enu.nextElement();
			cop.setHost("localhost");
			cout << cop.toString() << endl;
			TempFileStream tfs;
			CIMInstancePathtoXML(cop, tfs);
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumerateInstances(CIMClient& hdl, String ofClass, EDeepFlag deep, ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	String pstr;
	pstr = Format("ofClass = %1, deep = %2, localOnly = %3, includeQualifiers = %4, "
			"includeClassOrigin = %5, propertyList? %6",
			ofClass, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList != 0);
		
	testStart("enumInstances", pstr.c_str());

	try
	{
		CIMInstanceEnumeration enu = hdl.enumInstancesE( ofClass, deep, localOnly,
				includeQualifiers, includeClassOrigin, propertyList);

		while (enu.hasMoreElements())
		{
			CIMInstance i = enu.nextElement();
			cout << i.toMOF() << endl;
			TempFileStream tfs;
			CIMInstancetoXML(i,tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getInstance(CIMClient& hdl, const String& theInstance,
		ELocalOnlyFlag localOnly = E_NOT_LOCAL_ONLY,
		EIncludeQualifiersFlag includeQualifiers = E_EXCLUDE_QUALIFIERS,
		EIncludeClassOriginFlag includeClassOrigin = E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0)
{
	String pstr;
	pstr = Format("localOnly = %1, includeQualifiers=%2, "
			"includeClassOrigin = %3, propertyList? %4",
			localOnly, includeQualifiers, includeClassOrigin, propertyList != 0);
	testStart("getInstance", pstr.c_str());

	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass);
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(theInstance));

		CIMInstance in = hdl.getInstance( cop, localOnly, includeQualifiers,
				includeClassOrigin, propertyList);
		TempFileStream tfs;
		CIMInstancetoXML(in, tfs);
		tfs << '\n';
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
modifyInstance(CIMClient& hdl, const String& theInstance,
	EIncludeQualifiersFlag includeQualifiers, StringArray* propList,
	bool addProperty, bool addQualifier)
{
	String pstr = Format("includeQualifiers=%1, "
			"propertyList? %2, addProperty = %3, addQualifier = %4",
			includeQualifiers, propList != 0, addProperty, addQualifier);
	testStart("modifyInstance", pstr.c_str());

	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass);
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(theInstance));

		CIMInstance in = hdl.getInstance(cop);

		if (addProperty)
		{
			in.setProperty(CIMProperty("BrandNewProperty",
				CIMValue(String("true"))));
		}
		else
		{
			in.removeProperty("BrandNewProperty");
		}

		if (addQualifier)
		{
			CIMQualifierType borg = hdl.getQualifierType("version");
			in.setQualifier(CIMQualifier(borg));
		}
		else
		{
			in.removeQualifier("version");
		}

		// getInstance with includeQualifiers = false doesn't have any keys, so
		// we'll have to set them so modifyInstance will work.
		in.setKeys(cop.getKeys());

		hdl.modifyInstance(in, includeQualifiers, propList);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteInstance(CIMClient& hdl, const String& ofClass, const String& theInstance)
{
	testStart("deleteInstance");

	try
	{
		CIMObjectPath cop(ofClass );
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(theInstance));
		hdl.deleteInstance( cop);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteAssociations(CIMClient& hdl)
{
	testStart("deleteAssociations");

	try
	{
		hdl.execQueryE( "delete from CIM_SystemComponent", "wql1");
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
setQualifier(CIMClient& hdl)
{
	testStart("setQualifier");

	try
	{
		String qualName = "borgishness";

		CIMQualifierType qt(qualName);

		qt.setDataType(CIMDataType::STRING);
		qt.setDefaultValue(CIMValue(String()));

		qt.addScope(CIMScope::CLASS);
		qt.addScope(CIMScope::PROPERTY);

		qt.addFlavor(CIMFlavor::DISABLEOVERRIDE);


		hdl.setQualifierType( qt);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumerateQualifiers(CIMClient& hdl)
{
	testStart("enumerateQualifiers");

	try
	{
		CIMQualifierTypeEnumeration enu = hdl.enumQualifierTypesE();
		while (enu.hasMoreElements())
		{
			CIMQualifierType cqt = enu.nextElement();
			cout << "Found Qualifier Definition: " << cqt.getName() << endl;
			cout << cqt.toMOF() << endl;
			TempFileStream tfs;
			CIMtoXML(cqt, tfs);
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getQualifier(CIMClient& hdl)
{
	testStart("getQualifier");

	try
	{
		CIMQualifierType qt = hdl.getQualifierType( "borgishness");
		cout << "Got Qualifier: " << qt.getName() << endl;
		cout << qt.toMOF() << endl;
		TempFileStream tfs;
		CIMtoXML(qt, tfs);
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

void createAssociation(CIMClient& hdl, const String& assocName,
		const String& propName1, const CIMObjectPath& cop1,
		const String& propName2, const CIMObjectPath& cop2)
{

		CIMClass cc = hdl.getClass( assocName);
		CIMInstance inst = cc.newInstance();
			
		inst.setProperty(propName1, CIMValue(cop1));

		inst.setProperty(propName2, CIMValue(cop2));

		hdl.createInstance( inst);

}
//////////////////////////////////////////////////////////////////////////////
void
setupAssociations(CIMClient& hdl)
{
	testStart("setupAssociations");

	try
	{
		CIMObjectPath cop1("EXP_BionicComputerSystem", "root/testsuite");
		cop1.setKeyValue("CreationClassName", CIMValue(String("EXP_BionicComputerSystem")));
		cop1.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMObjectPath cop2("EXP_BionicComputerSystem2", "root/testsuite");
		cop2.setKeyValue("CreationClassName", CIMValue(String("EXP_BionicComputerSystem2")));
		cop2.setKeyValue("Name", CIMValue(String("SixMillion")));

		CIMObjectPath cop3("EXP_BionicComputerSystem2", "root/testsuite");
		cop3.setKeyValue("CreationClassName", CIMValue(String("EXP_BionicComputerSystem2")));
		cop3.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMObjectPath cop4("EXP_BionicComputerSystem", "root/testsuite");
		cop4.setKeyValue("CreationClassName", CIMValue(String("EXP_BionicComputerSystem")));
		cop4.setKeyValue("Name", CIMValue(String("SixMillion")));

		createAssociation(hdl, "CIM_SystemComponent", "GroupComponent", cop1,
				"PartComponent", cop2);
		createAssociation(hdl, "CIM_SystemDevice", "GroupComponent", cop2,
				"PartComponent", cop1);
		createAssociation(hdl, "CIM_SystemDevice", "GroupComponent", cop2,
				"PartComponent", cop3);
		createAssociation(hdl, "CIM_SystemDevice", "GroupComponent", cop1,
				"PartComponent", cop4);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
associatorNames(CIMClient& hdl, const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole)
{
	String pstr;
	pstr = Format("assocClass = %1, resultClass = %2, role = %3, resultRole = %4",
			assocClass, resultClass, role, resultRole);
	testStart("associatorNames", pstr.c_str());

	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMObjectPathEnumeration enu = hdl.associatorNamesE(
			cop, assocClass, resultClass, role, resultRole);

		std::vector<CIMObjectPath> v = std::vector<CIMObjectPath>(
			Enumeration_input_iterator<CIMObjectPath>(enu),
			Enumeration_input_iterator<CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			v[x].setHost("localhost");
			cout << "Associated path: " << v[x].toString() << endl;
		}

	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
associatorNamesClass(CIMClient& hdl, const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole)
{
	String pstr;
	pstr = Format("assocClass = %1, resultClass = %2, role = %3, resultRole = %4",
			assocClass, resultClass, role, resultRole);
	testStart("associatorNamesClass", pstr.c_str());

	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem");
		
		CIMObjectPathEnumeration enu = hdl.associatorNamesE(
			 cop, assocClass, resultClass, role, resultRole);

		std::vector<CIMObjectPath> v = std::vector<CIMObjectPath>(
			Enumeration_input_iterator<CIMObjectPath>(enu),
			Enumeration_input_iterator<CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			v[x].setHost("localhost");
			cout << "Associated path: " << v[x].toString() << endl;
		}

	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
associators(CIMClient& hdl, const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList)
{
	String pstr;
	pstr = Format("assocClass = %1, resultClass = %2, role = %3, resultRole = "
			"%4, includeQualifiers = %5, includeClassOrigin = %6, propertyList? "
			"%7",
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, propertyList != 0);
	testStart("associators", pstr.c_str());

	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));

		cop.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMInstanceEnumeration enu = hdl.associatorsE( cop,
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, propertyList);

		std::vector<CIMInstance> v = std::vector<CIMInstance>(
			Enumeration_input_iterator<CIMInstance>(enu),
			Enumeration_input_iterator<CIMInstance>());
		std::sort(v.begin(), v.end(), sorter<CIMInstance>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Association Instance: ";
			TempFileStream tfs;
			CIMInstancetoXML(v[x], tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
			cout << endl;
			cout << "In MOF: " << v[x].toMOF() << endl;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
associatorsClasses(CIMClient& hdl, const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList)
{
	String pstr;
	pstr = Format("assocClass = %1, resultClass = %2, role = %3, resultRole = "
			"%4, includeQualifiers = %5, includeClassOrigin = %6, propertyList? "
			"%7",
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, propertyList != 0);
	testStart("associatorsClasses", pstr.c_str());

	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem");
		CIMClassEnumeration enu = hdl.associatorsClassesE( cop,
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, propertyList);

		std::vector<CIMClass> v = std::vector<CIMClass>(
			Enumeration_input_iterator<CIMClass>(enu),
			Enumeration_input_iterator<CIMClass>());
		std::sort(v.begin(), v.end(), sorter<CIMClass>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Association Class: ";
			TempFileStream tfs;
			CIMtoXML(v[x], tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
			cout << endl;
			cout << "In MOF: " << v[x].toMOF() << endl;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
referenceNames(CIMClient& hdl,
		const String& resultClass, const String& role)
{
	String pstr;
	pstr = Format("resultClass = %1, role = %2", resultClass, role);
	testStart("referenceNames", pstr.c_str());

	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass);
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMObjectPathEnumeration enu = hdl.referenceNamesE( cop,
				resultClass, role);

		std::vector<CIMObjectPath> v = std::vector<CIMObjectPath>(
			Enumeration_input_iterator<CIMObjectPath>(enu),
			Enumeration_input_iterator<CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			v[x].setHost("localhost");
			cout << "Associated path: " << v[x].toString() << endl;
		}

	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
referenceNamesClass(CIMClient& hdl,
		const String& resultClass, const String& role)
{
	String pstr;
	pstr = Format("resultClass = %1, role = %2", resultClass, role);
	testStart("referenceNamesClass", pstr.c_str());

	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass);

		CIMObjectPathEnumeration enu = hdl.referenceNamesE( cop,
				resultClass, role);

		std::vector<CIMObjectPath> v = std::vector<CIMObjectPath>(
			Enumeration_input_iterator<CIMObjectPath>(enu),
			Enumeration_input_iterator<CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			v[x].setHost("localhost");
			cout << "Associated path: " << v[x].toString() << endl;
		}

	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
references(CIMClient& hdl,
		const String& resultClass, const String& role,
		EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	String pstr;
	pstr = Format("resultClass = %1, role = %2, includeQualifiers = %3, "
			"includeClassOrigin = %4, propertyList? %5",
			resultClass, role, includeQualifiers, includeClassOrigin,
			propertyList != 0);
	testStart("references", pstr.c_str());

	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMInstanceEnumeration enu = hdl.referencesE( cop,
				resultClass, role, includeQualifiers, includeClassOrigin,
				propertyList);

		std::vector<CIMInstance> v = std::vector<CIMInstance>(
			Enumeration_input_iterator<CIMInstance>(enu),
			Enumeration_input_iterator<CIMInstance>());
		std::sort(v.begin(), v.end(), sorter<CIMInstance>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Association Instance: ";
			TempFileStream tfs;
			CIMInstancetoXML(v[x], tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
			cout << endl;
			cout << "In MOF: " << v[x].toMOF() << endl;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
referencesClasses(CIMClient& hdl,
		const String& resultClass, const String& role,
		EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	String pstr;
	pstr = Format("resultClass = %1, role = %2, includeQualifiers = %3, "
			"includeClassOrigin = %4, propertyList? %5",
			resultClass, role, includeQualifiers, includeClassOrigin,
			propertyList != 0);
	testStart("referencesClasses", pstr.c_str());

	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem");

		CIMClassEnumeration enu = hdl.referencesClassesE( cop,
				resultClass, role, includeQualifiers, includeClassOrigin,
				propertyList);

		std::vector<CIMClass> v = std::vector<CIMClass>(
			Enumeration_input_iterator<CIMClass>(enu),
			Enumeration_input_iterator<CIMClass>());
		std::sort(v.begin(), v.end(), sorter<CIMClass>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			cout << "Referencing Class: ";
			TempFileStream tfs;
			CIMtoXML(v[x], tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
			cout << endl;
			CIMClass cc = v[x];
			cout << "In MOF: " << cc.toMOF() << endl;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
execQuery(CIMClient& hdl)
{
	testStart("execQuery");

	try
	{
		CIMInstanceEnumeration cie = hdl.execQueryE(
			"select * from EXP_BionicComputerSystem", "wql1");
		while (cie.hasMoreElements())
		{
			CIMInstance i = cie.nextElement();
			cout << "Instance from Query: ";
			TempFileStream tfs;
			CIMInstancetoXML(i, tfs);
			tfs << '\n';
			tfs.rewind();
			cout << XMLPrettyPrint(tfs);
			cout << "In MOF: " << i.toMOF() << endl;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteQualifier(CIMClient& hdl)
{
	testStart("deleteQualifier");

	try
	{
		hdl.deleteQualifierType( "borgishness");
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}


//////////////////////////////////////////////////////////////////////////////
void
deleteClass(CIMClient& hdl, const String& delClass)
{
	testStart("deleteClass");

	try
	{
		hdl.deleteClass( delClass);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

void
prepareGetStateParams(CIMParamValueArray& in, const CIMObjectPath& cop)
{
	// MOF of this method:
	//	string GetState(
	//		[in] string s,
	//		[in] uint8 uint8array[],
	//		[out] boolean b,
	//		[out] real64 r64,
	//		[in, out] sint16 io16,
	//		[out] string msg,
	//		[in, out] CIM_System REF paths[],
	//		[in, out] datetime nullParam);

	// These are out of order on purpose, to test that the CIMOM will order them correctly.
	in.push_back(CIMParamValue("io16", CIMValue(Int16(16))));
	in.push_back(CIMParamValue("nullParam", CIMValue(CIMNULL)));
	in.push_back(CIMParamValue("s", CIMValue(String("input string"))));
	UInt8Array uint8array;
	uint8array.push_back(1);
	uint8array.push_back(255);
	uint8array.push_back(0);
	uint8array.push_back(128);
	uint8array.push_back(0);
	in.push_back(CIMParamValue("uint8array", CIMValue(uint8array)));
	CIMObjectPathArray paths;
	CIMObjectPath copWithNS(cop);
	copWithNS.setNameSpace("root/testsuite");
	paths.push_back(copWithNS);
	CIMObjectPath cop2(copWithNS);
	cop2.setKeyValue("name", CIMValue("foo"));
	paths.push_back(cop2);
	in.push_back(CIMParamValue("paths", CIMValue(paths)));

}

//////////////////////////////////////////////////////////////////////////////
void
invokeMethod(CIMClient& hdl, int num)
{
	testStart("invokeMethod");

	try
	{
		CIMObjectPath cop("EXP_BartComputerSystem");

		String rval;
		CIMParamValueArray in, out;
		CIMValue cv(CIMNULL);
		switch (num)
		{
			case 1:
				cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BartComputerSystem")));
				cop.setKeyValue("Name", CIMValue(String("test")));
				in.push_back(CIMParamValue("newState", CIMValue(String("off"))));
				hdl.invokeMethod( cop, "setstate", in, out);
				cout << "invokeMethod: setstate(\"off\")" << endl;
				break;
			case 2:
			{
				prepareGetStateParams(in,cop);
				cv = hdl.invokeMethod( cop, "getstate", in, out);
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
				hdl.invokeMethod( cop, "togglestate", in, out);
				cout << "invokeMethod: togglestate()" << endl;
				break;
			case 4:
				prepareGetStateParams(in,cop);
				cv = hdl.invokeMethod( cop, "getstate", in, out);
				cv.get(rval);
				cout << "invokeMethod: getstate(): " << rval << endl;
				break;
			case 5:
				in.push_back(CIMParamValue("newState", CIMValue(String("off"))));
				hdl.invokeMethod( cop, "setstate", in, out);
				cout << "invokeMethod: setstate(\"off\")" << endl;
				break;
			case 6:
				prepareGetStateParams(in,cop);
				cv = hdl.invokeMethod( cop, "getstate", in, out);
				cv.get(rval);
				cout << "invokeMethod: getstate(): " << rval << endl;
				break;
			default:
				break;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
createNameSpace(CIMClient& hdl)
{
	testStart("createNameSpace");

	try
	{
		hdl.createNameSpace("root/testsuite/sub1");
		hdl.createNameSpace("root/testsuite/sub1/test");
		// test case sensitivity.  Namespaces (unlike other CIM names)
		// are *NOT* case sensitive, and so we should be able to create
		// both sub1 and Sub1
		hdl.createNameSpace("root/testsuite/Sub1");
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumNameSpace(CIMClient& hdl)
{
	testStart("enumNamespace");

	try
	{
		StringArray rval = hdl.enumCIM_NamespaceE();
		for (size_t i = 0; i < rval.size(); i++)
		{
			cout << "Namespace: " << rval[i] << endl;
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteNameSpace(CIMClient& hdl)
{
	testStart("deleteNameSpace");

	try
	{
		hdl.deleteNameSpace("root/testsuite/sub1/test");
		hdl.deleteNameSpace("root/testsuite/sub1");
		hdl.deleteNameSpace("root/testsuite/Sub1");
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getProperty(CIMClient& hdl, const String& instName)
{
	testStart("getProperty");

	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass);
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(String(instName)));

		CIMValue v = hdl.getProperty( cop, "OptionalArg");
		// with xml, this is a string.  we want a bool.
		v = CIMValueCast::castValueToDataType(v, CIMDataType::BOOLEAN);
		cout << "** getProperty returned. CIMValue: " << v.toMOF() << endl;
		TempFileStream tfs;
		CIMtoXML(v, tfs);
		tfs.rewind();
		cout << XMLPrettyPrint(tfs);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
setProperty(CIMClient& hdl, const String& instName)
{
	testStart("setProperty");

	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass);
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(instName));

		hdl.setProperty( cop, "OptionalArg", CIMValue(Bool(true)));
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
testSingleton(CIMClient& hdl)
{
	testStart("testSingleton");

	try
	{
		// first create the singletonClass
		CIMClass cimClass("singletonClass");

		Array<String> zargs;

		zargs.push_back("Name");
		zargs.push_back("string");

		zargs.push_back("OptionalArg");
		zargs.push_back("boolean");

		String name;
		String type;

		for (size_t i = 0; i < zargs.size(); i += 2)
		{
			name = zargs[i];
			type = zargs[i + 1];
			CIMProperty cimProp;
			if (type.equals("string"))
			{
				cimProp.setDataType(CIMDataType::STRING);
			}
			else if (type.equals("boolean"))
			{
				cimProp.setDataType(CIMDataType::BOOLEAN);
			}

			cimProp.setName(name);
			cimClass.addProperty(cimProp);
		}

		hdl.createClass(cimClass);

		CIMInstance newInst = cimClass.newInstance();
		newInst.setProperty("Name", CIMValue("singleton"));
		newInst.setProperty("OptionalArg", CIMValue(true));

		CIMObjectPath cop = hdl.createInstance(newInst);

		CIMInstance got = hdl.getInstance(cop);

		TEST_ASSERT(got.getPropertyT("Name").getValueT() == CIMValue("singleton"));
		TEST_ASSERT(got.getPropertyT("OptionalArg").getValueT() == CIMValue(true));

		newInst.setProperty("Name", CIMValue("singleton2"));
		newInst.setProperty("OptionalArg", CIMValue(false));

		hdl.modifyInstance(newInst);
		
		got = hdl.getInstance(cop);

		TEST_ASSERT(got.getPropertyT("Name").getValueT() == CIMValue("singleton2"));
		TEST_ASSERT(got.getPropertyT("OptionalArg").getValueT() == CIMValue(false));

		CIMObjectPathEnumeration e(hdl.enumInstanceNamesE("singletonClass"));
		TEST_ASSERT(e.numberOfElements() == 1);
		CIMObjectPath gotpath = e.nextElement();
		TEST_ASSERT(gotpath.getClassName() == "singletonClass");
		TEST_ASSERT(gotpath.getKeys().size() == 0);

		CIMInstanceEnumeration e2(hdl.enumInstancesE("singletonClass"));
		TEST_ASSERT(e2.numberOfElements() == 1);
		got = e2.nextElement();
		TEST_ASSERT(got.getPropertyT("Name").getValueT() == CIMValue("singleton2"));
		TEST_ASSERT(got.getPropertyT("OptionalArg").getValueT() == CIMValue(false));

		hdl.deleteInstance(gotpath);

		hdl.deleteClass("singletonClass");
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}


/****************************************************************************
 * This is the class that will be used to obtain authentication credentials
 * if none are provided in the URL used by the HTTP Client.
 ****************************************************************************/
class GetLoginInfo : public ClientAuthCBIFC
{
	public:
		bool getCredentials(const String& realm, String& name,
				String& passwd, const String& details)
		{
			cout << "Authentication required for " << realm << endl;
			cout << "Enter the user name: ";
			name = String::getLine(cin);
			passwd = GetPass::getPass("Enter the password for " +
				name + ": ");
			return true;
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
			String sockDumpOut = argv[2];
			String sockDumpIn = argv[2];
			sockDumpOut += "SockDumpOut";
			sockDumpIn += "SockDumpIn";
			SocketBaseImpl::setDumpFiles(sockDumpIn.c_str(),
				sockDumpOut.c_str());
		}
		else
		{
			SocketBaseImpl::setDumpFiles("","");
		}

		String url(argv[1]);
		URL owurl(url);

		/**********************************************************************
		 * We assign our SSL certificate callback into the SSLCtxMgr.
		 * If we don't do this, we'll accept any server certificate without
		 * any verification.  We leave this commented out here, so our
		 * acceptance test will run without user interaction.
		 **********************************************************************/

#ifdef OW_HAVE_OPENSSL
		//SSLCtxMgr::setCertVerifyCallback(ssl_verifycert_callback);
#endif
		
		
		/**********************************************************************
		 * Create an instance of our authentication callback class.
		 **********************************************************************/
		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);

		/**********************************************************************
		 * Here we create the CIMClient.  It'll uses the HTTPClient
		 * (capable of handling HTTP/1.1 and HTTPS -- HTTP over SSL).
		 * The CIMClient takes a URL in it's constructor, representing
		 * the CIM Server that it will connect to.  A URL has the form
		 *   http[s]://[USER:PASSWORD@]HOSTNAME[:PORT][/PATH].
		 *
		 * Example:  https://bill:secret@managedhost.example.com/cimom
		 *
		 * If no port is given, the defaults are used: 5988 for HTTP,
		 * and 5989 for HTTPS.  If no username and password are given,
		 * and the CIM Server requires authentication, a callback may
		 * be provided to retrieve authentication credentials.
		 * If the scheme starts with owbinary then the openwbem binary protocol will be
		 * used, otherwise it uses CIM/XML
		 **********************************************************************/

		CIMClient rch(url, "root/testsuite", getLoginInfo);

		TEST_ASSERT(rch.setHTTPRequestHeader("Accept-Language", "x-owtest"));

		/**********************************************************************
		 * Now we have essentially established a "connection" to the CIM
		 * Server.  We can access the methods on the remote CIMOM handle,
		 * and these methods will call into the CIM Server.  The HTTPClient
		 * will take care of the particulars of the HTTP protocol, including
		 * authentication, compression, SSL, chunking, etc.
		 **********************************************************************/

		testSingleton(rch);

		createNameSpace(rch);
		enumNameSpace(rch);
		deleteNameSpace(rch);
		createClass(rch, "EXP_BionicComputerSystem");
		createClass(rch, "EXP_BionicComputerSystem2");
		enumClassNames(rch);

		if (getenv("OWLONGTEST"))
		{
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
		enumerateInstances(rch, "CIM_System", E_SHALLOW, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);	
		// deep, non-localOnly, no qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_System", E_DEEP, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		// deep, localOnly, no qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_System", E_DEEP, E_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		// deep, non-localOnly, qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_System", E_DEEP, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		// deep, non-localOnly, no qualifiers, classOrigin, all props
		enumerateInstances(rch, "CIM_System", E_DEEP, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
		// deep, non-localOnly, no qualifiers, no classOrigin, no props
		StringArray sa;
		enumerateInstances(rch, "CIM_System", E_DEEP, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
		// deep, non-localOnly, no qualifiers, no classOrigin, one prop
		sa.push_back(String("BrandNewProperty"));
		enumerateInstances(rch, "CIM_System", E_DEEP, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

		enumerateInstances(rch, "Example_C1", E_DEEP,E_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C1", E_DEEP,E_NOT_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C1", E_SHALLOW,E_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C1", E_SHALLOW,E_NOT_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C2", E_DEEP,E_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C2", E_DEEP,E_NOT_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C2", E_SHALLOW,E_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C2", E_SHALLOW,E_NOT_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C3", E_DEEP,E_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C3", E_DEEP,E_NOT_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C3", E_SHALLOW,E_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
		enumerateInstances(rch, "Example_C3", E_SHALLOW,E_NOT_LOCAL_ONLY,E_EXCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);

		getInstance(rch, "SixMillion");
		getInstance(rch, "SevenMillion");
		// localOnly = true
		getInstance(rch, "SevenMillion", E_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		// includeQualifiers = true
		getInstance(rch, "SevenMillion", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		// includeClassOrigin = true
		getInstance(rch, "SevenMillion", E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
		// propertyList = non-null
		sa.clear();
		getInstance(rch, "SevenMillion", E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
		// propertyList = non-null, with element
		sa.push_back(String("BrandNewProperty"));
		getInstance(rch, "SevenMillion", E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

		// add a property and a qualifier
		modifyInstance(rch, "SixMillion", E_INCLUDE_QUALIFIERS, 0, true, true);
		getInstance(rch, "SixMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);
		// add a property, remove a qualifier
		modifyInstance(rch, "SixMillion", E_INCLUDE_QUALIFIERS, 0, true, false);
		getInstance(rch, "SixMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);
		// remove a property, remove a qualifier
		modifyInstance(rch, "SixMillion", E_INCLUDE_QUALIFIERS, 0, false, false);
		getInstance(rch, "SixMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);
		// remove a property, add a qualifier
		modifyInstance(rch, "SixMillion", E_INCLUDE_QUALIFIERS, 0, false, true);
		getInstance(rch, "SixMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);

		// includeQualifier = false.  don't add qualifier, add property, qual should still be there.
		modifyInstance(rch, "SixMillion", E_EXCLUDE_QUALIFIERS, 0, true, false);
		getInstance(rch, "SixMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);

		// includeQualifier = true, don't add qualifier or property, qual and prop should be gone.
		modifyInstance(rch, "SixMillion", E_INCLUDE_QUALIFIERS, 0, false, false);
		getInstance(rch, "SixMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);
		
		// add the property, but it shouldn't appear, because the prop list is empty.
		sa.clear();
		modifyInstance(rch, "SixMillion", E_INCLUDE_QUALIFIERS, &sa, true, false);
		getInstance(rch, "SixMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);

		// add the property, now it should appear.
		sa.push_back(String("BrandNewProperty"));
		modifyInstance(rch, "SixMillion", E_EXCLUDE_QUALIFIERS, &sa, true, false);
		getInstance(rch, "SixMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);

		// try another instance
		modifyInstance(rch, "SevenMillion", E_EXCLUDE_QUALIFIERS, &sa, true, false);
		getInstance(rch, "SevenMillion", E_LOCAL_ONLY, E_INCLUDE_QUALIFIERS);

		setProperty(rch, "SixMillion");
		getProperty(rch, "SixMillion");
		getInstance(rch, "SixMillion");
		setQualifier(rch);
		enumerateQualifiers(rch);
		getQualifier(rch);

		setupAssociations(rch);

		associatorNames(rch, "", "", "", "");
		associatorNamesClass(rch, "", "", "", "");
		associators(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		associatorsClasses(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		referenceNames(rch, "", "");
		referenceNamesClass(rch, "", "");
		references(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		referencesClasses(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);

		if (getenv("OWLONGTEST"))
		{
			// try all the variations
			associatorNames(rch, "CIM_SystemDevice", "", "", "");
			associatorNames(rch, "", "CIM_System", "", "");
			associatorNames(rch, "", "EXP_BionicComputerSystem", "", "");
			associatorNames(rch, "", "", "GroupComponent", "");
			associatorNames(rch, "", "", "PartComponent", "");
			associatorNames(rch, "", "", "PartComponent", "PartComponent");
			associatorNames(rch, "", "", "", "PartComponent");
			associatorNames(rch, "", "", "", "GroupComponent");
			associatorNamesClass(rch, "CIM_SystemDevice", "", "", "");
			associatorNamesClass(rch, "", "CIM_System", "", "");
			associatorNamesClass(rch, "", "EXP_BionicComputerSystem", "", "");
			associatorNamesClass(rch, "", "", "GroupComponent", "");
			associatorNamesClass(rch, "", "", "PartComponent", "");
			associatorNamesClass(rch, "", "", "PartComponent", "PartComponent");
			associatorNamesClass(rch, "", "", "", "PartComponent");
			associatorNamesClass(rch, "", "", "", "GroupComponent");

			associators(rch, "CIM_SystemDevice", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "CIM_System", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "EXP_BionicComputerSystem", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "", "GroupComponent", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "", "PartComponent", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "", "PartComponent", "PartComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "", "", "PartComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "", "", "GroupComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "", "", "", E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
			sa.clear();
			associators(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
			sa.push_back(String("BrandNewProperty"));
			associators(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

			associatorsClasses(rch, "CIM_SystemDevice", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "CIM_System", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "EXP_BionicComputerSystem", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "", "GroupComponent", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "", "PartComponent", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "", "PartComponent", "PartComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "", "", "PartComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "", "", "GroupComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "", "", "", E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
			sa.clear();
			associatorsClasses(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
			sa.push_back(String("BrandNewProperty"));
			associatorsClasses(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

			referenceNames(rch, "cim_systemdevice", "");
			referenceNames(rch, "cim_component", "");
			referenceNames(rch, "", "GroupComponent");
			referenceNames(rch, "", "PartComponent");

			referenceNamesClass(rch, "cim_systemdevice", "");
			referenceNamesClass(rch, "cim_component", "");
			referenceNamesClass(rch, "", "GroupComponent");
			referenceNamesClass(rch, "", "PartComponent");

			references(rch, "cim_systemdevice", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "cim_component", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "", "GroupComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "", "PartComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "", "", E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "", "", E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
			sa.clear();
			references(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
			sa.push_back(String("GroupComponent"));
			references(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

			referencesClasses(rch, "cim_systemdevice", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "cim_component", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "", "GroupComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "", "PartComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "", "", E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "", "", E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
			sa.clear();
			referencesClasses(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
			sa.push_back(String("GroupComponent"));
			referencesClasses(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

		}

		execQuery(rch);

		deleteAssociations(rch);

		deleteInstance(rch, "EXP_BionicComputerSystem", "SixMillion");
		deleteInstance(rch, "EXP_BionicComputerSystem", "SevenMillion");
		deleteInstance(rch, "EXP_BionicComputerSystem2", "SixMillion");
		deleteInstance(rch, "EXP_BionicComputerSystem2", "SevenMillion");
		deleteClass(rch, "EXP_BionicComputerSystem");
		deleteClass(rch, "EXP_BionicComputerSystem2");
		deleteQualifier(rch);

		testDynInstances(rch);
		testModifyProviderQualifier(rch);

		invokeMethod(rch, 1);
		invokeMethod(rch, 2);
		invokeMethod(rch, 3);
		invokeMethod(rch, 4);
		invokeMethod(rch, 5);
		invokeMethod(rch, 6);

		return 0;

	}
	catch (AssertionException& a)
	{
		cerr << "Caught Assertion: " << a << endl;
	}
	catch (Exception& e)
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
	X509_NAME*	  name;
	int			 unsigned i = 16;
	unsigned char   digest[16];

	cout << endl;

	/* print the issuer */
	printf("   issuer:\n");
	name = X509_get_issuer_name(cert);
	display_name("	  ",name);

	/* print the subject */
	name = X509_get_subject_name(cert);
	printf("   subject:\n");
	display_name("	  ",name);

	/* print the fingerprint */
	X509_digest(cert,EVP_md5(),digest,&i);
	printf("   fingerprint:\n");
	printf("	  ");
	for (i=0;i<16;i++)
	{
		printf("%02X",digest[i]);
		if (i != 15)
		{
			printf(":");
		}
	}
	printf("\n");
}

//////////////////////////////////////////////////////////////////////////////
int ssl_verifycert_callback(X509* cert)
{
	static bool	 bPrompted = false;

	if (!bPrompted)
	{
		X509   *pX509Cert = cert;

		bPrompted = true;

		cout << "The SSL connection received the following certificate:" << endl;

		display_cert(pX509Cert);

		cout << "\nDo you want to accept this Certificate (Y/N)? ";
		String response = String::getLine(cin);
		if (response.compareToIgnoreCase("Y") != 0)
			return 0;
	}
	return 1;
}

#endif
