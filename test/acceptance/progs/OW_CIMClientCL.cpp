/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
#include "OW_Format.hpp"
#include "OW_GetPass.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"
#include "OW_XMLPrettyPrint.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_Socket.hpp"
#include "OW_URL.hpp"
#ifdef OW_HAVE_OPENSSL
#include <openssl/ssl.h>
#endif

#include <iostream>
#include <algorithm> // for sort

#define TEST_ASSERT(CON) if(!(CON)) throw OW_AssertionException(__FILE__, __LINE__, #CON)

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using namespace OW_WBEMFlags;

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
createClass(OW_CIMClient& hdl, const OW_String& name)
{
	testStart("createClass");

	try
	{
		OW_String cqtPath("Key");
		OW_CIMQualifierType cqt = hdl.getQualifierType( cqtPath);
		OW_CIMQualifier cimQualifierKey(cqt);

		cimQualifierKey.setValue(OW_CIMValue(OW_Bool(true)));
		OW_CIMClass cimClass;
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
			OW_CIMProperty cimProp;
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

		hdl.createClass( cimClass);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumClassNames(OW_CIMClient& hdl)
{
	testStart("enumClassNames");

	try
	{
		OW_StringEnumeration enu = hdl.enumClassNamesE( "", E_DEEP);
		while (enu.hasMoreElements())
		{
			cout << "CIMClass: " << enu.nextElement() << endl;
		}
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	cout << "Testing non-deep" << endl;

	try
	{
		OW_StringEnumeration enu = hdl.enumClassNamesE( "", E_SHALLOW);
		while (enu.hasMoreElements())
		{
			cout << "CIMClass: " << enu.nextElement() << endl;
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
enumClasses(OW_CIMClient& hdl)
{
	testStart("enumClasses");

	cout << "deep = true, localOnly = false" << endl;
	try
	{
		OW_CIMClassEnumeration enu = hdl.enumClassE( "", E_DEEP, E_NOT_LOCAL_ONLY);
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
		OW_CIMClassEnumeration enu = hdl.enumClassE( "", E_SHALLOW, E_NOT_LOCAL_ONLY);
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
		OW_CIMClassEnumeration enu = hdl.enumClassE( "", E_SHALLOW, E_LOCAL_ONLY);
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
		OW_CIMClassEnumeration enu = hdl.enumClassE( "", E_DEEP, E_LOCAL_ONLY);
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
modifyClass(OW_CIMClient& hdl)
{
	testStart("modifyClass");

	try
	{
		OW_CIMClass cimClass = hdl.getClass(
			"EXP_BionicComputerSystem");
		cout << "CIMClass before: " << cimClass.toMOF() << endl;
		OW_TempFileStream tfs;
		OW_CIMtoXML(cimClass,tfs,OW_CIMtoXMLFlags::notLocalOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		cout << OW_XMLPrettyPrint(tfs);

		OW_CIMProperty cimProp;
		cimProp.setDataType(OW_CIMDataType::STRING);
		cimProp.setName("BrandNewProperty");
		cimClass.addProperty(cimProp);
		hdl.modifyClass( cimClass);

		cimClass = hdl.getClass( "EXP_BionicComputerSystem");
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
getClass(OW_CIMClient& hdl)
{
	testStart("getClass");

	cout << "localOnly = false" << endl;
	try
	{
		OW_CIMClass cimClass = hdl.getClass(
			"EXP_BionicComputerSystem", E_NOT_LOCAL_ONLY);
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
		OW_CIMClass cimClass = hdl.getClass(
			"EXP_BionicComputerSystem", E_LOCAL_ONLY);
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
testDynInstances(OW_CIMClient& hdl)
{
	testStart("testDynInstances");
	try
	{
		OW_CIMClass cc = hdl.getClass( "testinstance");
		OW_CIMInstance ci = cc.newInstance();
		ci.setProperty("name", OW_CIMValue(OW_String("one")));
		OW_StringArray params;
		params.push_back("one");
		params.push_back("two");
		ci.setProperty("params", OW_CIMValue(params));
		hdl.createInstance( ci);
		OW_CIMObjectPath cop1("root/testsuite", ci);
		ci = hdl.getInstance(cop1);
		OW_TempFileStream tfs;
		tfs << "<CIM>";
		OW_CIMtoXML(ci, tfs, cop1, OW_CIMtoXMLFlags::isNotInstanceName,
			OW_CIMtoXMLFlags::localOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		tfs << "</CIM>";
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);

		ci = cc.newInstance();
		ci.setProperty("name", OW_CIMValue(OW_String("two")));
		params.clear();
		params.push_back("A");
		params.push_back("B");
		params.push_back("C");
		ci.setProperty("params", OW_CIMValue(params));
		hdl.createInstance( ci);
		OW_CIMObjectPath cop2("root/testsuite", ci);
		ci = hdl.getInstance(cop2);
		tfs.reset();
		tfs << "<CIM>";
		OW_CIMtoXML(ci, tfs, cop2, OW_CIMtoXMLFlags::isNotInstanceName,
			OW_CIMtoXMLFlags::localOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		tfs << "</CIM>";
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);

		params.clear();
		params.push_back("uno");
		params.push_back("dos");
		ci = cc.newInstance();
		ci.setProperty("name", OW_CIMValue(OW_String("one")));
		ci.setProperty("params", OW_CIMValue(params));
		hdl.modifyInstance( ci);
		ci = hdl.getInstance(cop1);
		tfs.reset();
		tfs << "<CIM>";
		OW_CIMtoXML(ci, tfs, cop1, OW_CIMtoXMLFlags::isNotInstanceName,
			OW_CIMtoXMLFlags::localOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		tfs << "</CIM>";
		tfs.rewind();
		cout << OW_XMLPrettyPrint(tfs);

		OW_CIMInstanceEnumeration enu = hdl.enumInstancesE(
			"testinstance");
		TEST_ASSERT(enu.numberOfElements() == 2);

		hdl.deleteInstance( cop1);
		enu = hdl.enumInstancesE( "testinstance");
		TEST_ASSERT(enu.numberOfElements() == 1);

		hdl.deleteInstance( cop2);
		enu = hdl.enumInstancesE( "testinstance");
		TEST_ASSERT(enu.numberOfElements() == 0);
		
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}
	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
testModifyProviderQualifier(OW_CIMClient& hdl)
{
	testStart("testModifyProviderQualifier");
	try
	{
		OW_CIMClass cc = hdl.getClass( "testinstance");
		OW_CIMInstance ci = cc.newInstance();
		ci.setProperty("name", OW_CIMValue(OW_String("one")));
		OW_StringArray params;
		params.push_back("one");
		params.push_back("two");
		ci.setProperty("params", OW_CIMValue(params));
		hdl.createInstance( ci);
		OW_CIMObjectPath cop1("root/testsuite", ci);
		ci = hdl.getInstance(cop1);

		OW_CIMQualifier provQual = cc.getQualifier("provider");
		TEST_ASSERT(!provQual); // shouldn't be there since the provider registers itself.
		provQual = OW_CIMQualifier("provider");
		provQual.setValue(OW_CIMValue("somejunk")); // the provider qualifier should be ignored.
		cc.addQualifier(provQual);
		hdl.modifyClass(cc);

		ci = hdl.getInstance(cop1);

		cc.removeQualifier(provQual);
		hdl.modifyClass(cc); // change it back

		OW_CIMInstanceEnumeration enu = hdl.enumInstancesE(
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
		provQual.setValue(OW_CIMValue("c++::testinstance"));
		cc.addQualifier(provQual);
		hdl.createClass(cc);
		ci = cc.newInstance();
		ci.setProperty("name", OW_CIMValue(OW_String("one")));
		params.clear();
		params.push_back("one");
		params.push_back("two");
		ci.setProperty("params", OW_CIMValue(params));
		hdl.createInstance( ci);
		cop1 = OW_CIMObjectPath("root/testsuite", ci);
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
		catch (const OW_CIMException& e)
		{
			TEST_ASSERT(e.getErrNo() == OW_CIMException::NOT_FOUND);
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
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}
	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
createInstance(OW_CIMClient& hdl, const OW_String& fromClass, const OW_String& newInstance)
{
	testStart("createInstance");

	try
	{
		OW_CIMClass cimClass = hdl.getClass( fromClass);

		OW_CIMInstance newInst = cimClass.newInstance();

		newInst.setProperty(OW_CIMProperty::NAME_PROPERTY,
								  OW_CIMValue(newInstance));
		newInst.setProperty("CreationClassName",
								  OW_CIMValue(fromClass));

		hdl.createInstance( newInst);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumerateInstanceNames(OW_CIMClient& hdl)
{
	testStart("enumInstanceNames");

	try
	{
		OW_String ofClass = "CIM_ComputerSystem";
		OW_CIMObjectPathEnumeration enu = hdl.enumInstanceNamesE( ofClass);
		while (enu.hasMoreElements())
		{
			OW_CIMObjectPath cop = enu.nextElement();
			cout << cop.toString() << endl;
			OW_TempFileStream tfs;
			OW_CIMInstancePathtoXML(cop, tfs);
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
enumerateInstances(OW_CIMClient& hdl, OW_String ofClass, EDeepFlag deep, ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_String pstr;
	pstr = format("ofClass = %1, deep = %2, localOnly = %3, includeQualifiers = %4, "
			"includeClassOrigin = %5, propertyList? %6",
			ofClass, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList != 0);
		
	testStart("enumInstances", pstr.c_str());

	try
	{
		OW_CIMInstanceEnumeration enu = hdl.enumInstancesE( ofClass, deep, localOnly,
				includeQualifiers, includeClassOrigin, propertyList);

		while (enu.hasMoreElements())
		{
			OW_CIMInstance i = enu.nextElement();
			cout << i.toMOF() << endl;
			OW_TempFileStream tfs;
			OW_CIMtoXML(i,tfs, OW_CIMObjectPath(OW_CIMNULL),
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
getInstance(OW_CIMClient& hdl, const OW_String& theInstance,
		ELocalOnlyFlag localOnly = E_NOT_LOCAL_ONLY,
		EIncludeQualifiersFlag includeQualifiers = E_EXCLUDE_QUALIFIERS,
		OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = OW_WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList = 0)
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

		OW_CIMInstance in = hdl.getInstance( cop, localOnly, includeQualifiers,
				includeClassOrigin, propertyList);
		OW_TempFileStream tfs;
		OW_CIMtoXML(in, tfs, OW_CIMObjectPath(OW_CIMNULL),
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
modifyInstance(OW_CIMClient& hdl, const OW_String& theInstance,
	EIncludeQualifiersFlag includeQualifiers, OW_StringArray* propList,
	bool addProperty, bool addQualifier)
{
	OW_String pstr = format("includeQualifiers=%1, "
			"propertyList? %2, addProperty = %3, addQualifier = %4",
			includeQualifiers, propList != 0, addProperty, addQualifier);
	testStart("modifyInstance", pstr.c_str());

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(theInstance));

		OW_CIMInstance in = hdl.getInstance(cop);

		if (addProperty)
		{
			in.setProperty(OW_CIMProperty("BrandNewProperty",
				OW_CIMValue(OW_String("true"))));
		}
		else
		{
			in.removeProperty("BrandNewProperty");
		}

		if (addQualifier)
		{
			OW_CIMQualifierType borg = hdl.getQualifierType("version");
			in.setQualifier(OW_CIMQualifier(borg));
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
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteInstance(OW_CIMClient& hdl, const OW_String& ofClass, const OW_String& theInstance)
{
	testStart("deleteInstance");

	try
	{
		OW_CIMObjectPath cop(ofClass );
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(theInstance));
		hdl.deleteInstance( cop);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
deleteAssociations(OW_CIMClient& hdl)
{
	testStart("deleteAssociations");

	try
	{
		hdl.execQueryE( "delete from CIM_SystemComponent", "wql1");
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
setQualifier(OW_CIMClient& hdl)
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


		hdl.setQualifierType( qt);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumerateQualifiers(OW_CIMClient& hdl)
{
	testStart("enumerateQualifiers");

	try
	{
		OW_CIMQualifierTypeEnumeration enu = hdl.enumQualifierTypesE();
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
getQualifier(OW_CIMClient& hdl)
{
	testStart("getQualifier");

	try
	{
		OW_CIMQualifierType qt = hdl.getQualifierType( "borgishness");
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

void createAssociation(OW_CIMClient& hdl, const OW_String& assocName,
		const OW_String& propName1, const OW_CIMObjectPath& cop1,
		const OW_String& propName2, const OW_CIMObjectPath& cop2)
{

		OW_CIMClass cc = hdl.getClass( assocName);
		OW_CIMInstance inst = cc.newInstance();
			
		inst.setProperty(propName1, OW_CIMValue(cop1));

		inst.setProperty(propName2, OW_CIMValue(cop2));

		hdl.createInstance( inst);

}
//////////////////////////////////////////////////////////////////////////////
void
setupAssociations(OW_CIMClient& hdl)
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
associatorNames(OW_CIMClient& hdl, const OW_String& assocClass,
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
			cop, assocClass, resultClass, role, resultRole);

		std::vector<OW_CIMObjectPath> v = std::vector<OW_CIMObjectPath>(
			OW_Enumeration_input_iterator<OW_CIMObjectPath>(enu),
			OW_Enumeration_input_iterator<OW_CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			v[x].setHost("localhost");
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
associatorNamesClass(OW_CIMClient& hdl, const OW_String& assocClass,
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
			 cop, assocClass, resultClass, role, resultRole);

		std::vector<OW_CIMObjectPath> v = std::vector<OW_CIMObjectPath>(
			OW_Enumeration_input_iterator<OW_CIMObjectPath>(enu),
			OW_Enumeration_input_iterator<OW_CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			v[x].setHost("localhost");
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
associators(OW_CIMClient& hdl, const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole, EIncludeQualifiersFlag includeQualifiers,
		OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OW_StringArray* propertyList)
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

		OW_CIMInstanceEnumeration enu = hdl.associatorsE( cop,
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
			OW_CIMtoXML(v[x], tfs, OW_CIMObjectPath(OW_CIMNULL),
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
associatorsClasses(OW_CIMClient& hdl, const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole, EIncludeQualifiersFlag includeQualifiers,
		OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OW_StringArray* propertyList)
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
		OW_CIMClassEnumeration enu = hdl.associatorsClassesE( cop,
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
referenceNames(OW_CIMClient& hdl,
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

		OW_CIMObjectPathEnumeration enu = hdl.referenceNamesE( cop,
				resultClass, role);

		std::vector<OW_CIMObjectPath> v = std::vector<OW_CIMObjectPath>(
			OW_Enumeration_input_iterator<OW_CIMObjectPath>(enu),
			OW_Enumeration_input_iterator<OW_CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			v[x].setHost("localhost");
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
referenceNamesClass(OW_CIMClient& hdl,
		const OW_String& resultClass, const OW_String& role)
{
	OW_String pstr;
	pstr = format("resultClass = %1, role = %2", resultClass, role);
	testStart("referenceNamesClass", pstr.c_str());

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);

		OW_CIMObjectPathEnumeration enu = hdl.referenceNamesE( cop,
				resultClass, role);

		std::vector<OW_CIMObjectPath> v = std::vector<OW_CIMObjectPath>(
			OW_Enumeration_input_iterator<OW_CIMObjectPath>(enu),
			OW_Enumeration_input_iterator<OW_CIMObjectPath>());
		std::sort(v.begin(), v.end(), sorter<OW_CIMObjectPath>());
		for (size_t x = 0; x < v.size(); ++x)
		{
			v[x].setHost("localhost");
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
references(OW_CIMClient& hdl,
		const OW_String& resultClass, const OW_String& role,
		EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
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

		OW_CIMInstanceEnumeration enu = hdl.referencesE( cop,
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
			OW_CIMtoXML(v[x], tfs, OW_CIMObjectPath(OW_CIMNULL),
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
referencesClasses(OW_CIMClient& hdl,
		const OW_String& resultClass, const OW_String& role,
		EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
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

		OW_CIMClassEnumeration enu = hdl.referencesClassesE( cop,
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
execQuery(OW_CIMClient& hdl)
{
	testStart("execQuery");

	try
	{
		OW_CIMInstanceEnumeration cie = hdl.execQueryE(
			"select * from EXP_BionicComputerSystem", "wql1");
		while (cie.hasMoreElements())
		{
			OW_CIMInstance i = cie.nextElement();
			cout << "Instance from Query: ";
			OW_TempFileStream tfs;
			OW_CIMtoXML(i, tfs, OW_CIMObjectPath(OW_CIMNULL),
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
deleteQualifier(OW_CIMClient& hdl)
{
	testStart("deleteQualifier");

	try
	{
		hdl.deleteQualifierType( "borgishness");
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}


//////////////////////////////////////////////////////////////////////////////
void
deleteClass(OW_CIMClient& hdl, const OW_String& delClass)
{
	testStart("deleteClass");

	try
	{
		hdl.deleteClass( delClass);
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
	//	string GetState(
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
	in.push_back(OW_CIMParamValue("nullParam", OW_CIMValue(OW_CIMNULL)));
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
invokeMethod(OW_CIMClient& hdl, int num)
{
	testStart("invokeMethod");

	try
	{
		OW_CIMObjectPath cop("EXP_BartComputerSystem");

		OW_String rval;
		OW_CIMParamValueArray in, out;
		OW_CIMValue cv(OW_CIMNULL);
		switch (num)
		{
			case 1:
				cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BartComputerSystem")));
				cop.addKey("Name", OW_CIMValue(OW_String("test")));
				in.push_back(OW_CIMParamValue("newState", OW_CIMValue(OW_String("off"))));
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
				in.push_back(OW_CIMParamValue("newState", OW_CIMValue(OW_String("off"))));
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
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
createNameSpace(OW_CIMClient& hdl)
{
	testStart("createNameSpace");

	try
	{
		hdl.createNameSpace("root/testsuite/Vintela");
		hdl.createNameSpace("root/testsuite/Vintela/test");
		// test case sensitivity.  Namespaces (unlike other CIM names)
		// are *NOT* case sensitive, and so we should be able to create
		// both vintela and Vintela
		hdl.createNameSpace("root/testsuite/vintela");
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
enumNameSpace(OW_CIMClient& hdl)
{
	testStart("enumNamespace");

	try
	{
		cout << "deep = false" << endl;
		OW_StringArray rval = hdl.enumNameSpaceE(E_SHALLOW);
		for (size_t i = 0; i < rval.size(); i++)
		{
			cout << "Namespace: " << rval[i] << endl;
		}
		
		cout << "deep = true" << endl;
		rval = hdl.enumNameSpaceE(E_DEEP);
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
deleteNameSpace(OW_CIMClient& hdl)
{
	testStart("deleteNameSpace");

	try
	{
		hdl.deleteNameSpace("root/testsuite/Vintela/test");
		hdl.deleteNameSpace("root/testsuite/Vintela");
		hdl.deleteNameSpace("root/testsuite/vintela");
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
getProperty(OW_CIMClient& hdl, const OW_String& instName)
{
	testStart("getProperty");

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(OW_String(instName)));

		OW_CIMValue v = hdl.getProperty( cop, "OptionalArg");
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
setProperty(OW_CIMClient& hdl, const OW_String& instName)
{
	testStart("setProperty");

	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass);
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(instName));

		hdl.setProperty( cop, "OptionalArg", OW_CIMValue(OW_Bool(true)));
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
	}

	testDone();
}

//////////////////////////////////////////////////////////////////////////////
void
testSingleton(OW_CIMClient& hdl)
{
	testStart("testSingleton");

	try
	{
		// first create the singletonClass
		OW_CIMClass cimClass("singletonClass");

		OW_Array<OW_String> zargs;

		zargs.push_back("Name");
		zargs.push_back("string");

		zargs.push_back("OptionalArg");
		zargs.push_back("boolean");

		OW_String name;
		OW_String type;

		for (size_t i = 0; i < zargs.size(); i += 2)
		{
			name = zargs[i];
			type = zargs[i + 1];
			OW_CIMProperty cimProp;
			if (type.equals("string"))
			{
				cimProp.setDataType(OW_CIMDataType::STRING);
			}
			else if (type.equals("boolean"))
			{
				cimProp.setDataType(OW_CIMDataType::BOOLEAN);
			}

			cimProp.setName(name);
			cimClass.addProperty(cimProp);
		}

		hdl.createClass(cimClass);

		OW_CIMInstance newInst = cimClass.newInstance();
		newInst.setProperty("Name", OW_CIMValue("singleton"));
		newInst.setProperty("OptionalArg", OW_CIMValue(true));

		hdl.createInstance(newInst);

		OW_CIMInstance got = hdl.getInstance(OW_CIMObjectPath("root/testsuite", newInst));

		TEST_ASSERT(got.getPropertyT("Name").getValueT() == OW_CIMValue("singleton"));
		TEST_ASSERT(got.getPropertyT("OptionalArg").getValueT() == OW_CIMValue(true));

		newInst.setProperty("Name", OW_CIMValue("singleton2"));
		newInst.setProperty("OptionalArg", OW_CIMValue(false));

		hdl.modifyInstance(newInst);
		
		got = hdl.getInstance(OW_CIMObjectPath("root/testsuite", newInst));

		TEST_ASSERT(got.getPropertyT("Name").getValueT() == OW_CIMValue("singleton2"));
		TEST_ASSERT(got.getPropertyT("OptionalArg").getValueT() == OW_CIMValue(false));

		OW_CIMObjectPathEnumeration e(hdl.enumInstanceNamesE("singletonClass"));
		TEST_ASSERT(e.numberOfElements() == 1);
		OW_CIMObjectPath gotpath = e.nextElement();
		TEST_ASSERT(gotpath.getObjectName() == "singletonClass");
		TEST_ASSERT(gotpath.getKeys().size() == 0);

		OW_CIMInstanceEnumeration e2(hdl.enumInstancesE("singletonClass"));
		TEST_ASSERT(e2.numberOfElements() == 1);
		got = e2.nextElement();
		TEST_ASSERT(got.getPropertyT("Name").getValueT() == OW_CIMValue("singleton2"));
		TEST_ASSERT(got.getPropertyT("OptionalArg").getValueT() == OW_CIMValue(false));

		hdl.deleteInstance(gotpath);

		hdl.deleteClass("singletonClass");
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
		 * Create an instance of our authentication callback class.
		 **********************************************************************/
		OW_ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);

		/**********************************************************************
		 * Here we create the OW_CIMClient.  It'll uses the OW_HTTPClient
		 * (capable of handling HTTP/1.1 and HTTPS -- HTTP over SSL).
		 * The OW_CIMClient takes a URL in it's constructor, representing
		 * the CIM Server that it will connect to.  A URL has the form
		 *   http[s]://[USER:PASSWORD@]HOSTNAME[:PORT][/PATH].
		 *
		 * Example:  https://bill:secret@managedhost.example.com/cimom
		 *
		 * If no port is given, the defaults are used: 5988 for HTTP,
		 * and 5989 for HTTPS.  If no username and password are given,
		 * and the CIM Server requires authentication, a callback may
		 * be provided to retrieve authentication credentials.
		 * If the path is /owbinary then the openwbem binary protocol will be
		 * used, otherwise it uses CIM/XML
		 **********************************************************************/

		OW_CIMClient rch(url, "root/testsuite", getLoginInfo);



		/**********************************************************************
		 * Now we have essentially established a "connection" to the CIM
		 * Server.  We can access the methods on the remote CIMOM handle,
		 * and these methods will call into the CIM Server.  The OW_HTTPClient
		 * will take care of the particulars of the HTTP protocol, including
		 * authentication, compression, SSL, chunking, etc.
		 **********************************************************************/

		testSingleton(rch);

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
		enumerateInstances(rch, "CIM_ComputerSystem", E_SHALLOW, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);	
		// deep, non-localOnly, no qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", E_DEEP, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		// deep, localOnly, no qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", E_DEEP, E_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		// deep, non-localOnly, qualifiers, no classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", E_DEEP, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
		// deep, non-localOnly, no qualifiers, classOrigin, all props
		enumerateInstances(rch, "CIM_ComputerSystem", E_DEEP, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
		// deep, non-localOnly, no qualifiers, no classOrigin, no props
		OW_StringArray sa;
		enumerateInstances(rch, "CIM_ComputerSystem", E_DEEP, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
		// deep, non-localOnly, no qualifiers, no classOrigin, one prop
		sa.push_back(OW_String("BrandNewProperty"));
		enumerateInstances(rch, "CIM_ComputerSystem", E_DEEP, E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

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
		sa.push_back(OW_String("BrandNewProperty"));
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
		sa.push_back(OW_String("BrandNewProperty"));
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

			associators(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "CIM_SystemDevice", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associators(rch, "", "CIM_ComputerSystem", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
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
			sa.push_back(OW_String("BrandNewProperty"));
			associators(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

			associatorsClasses(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "CIM_SystemDevice", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			associatorsClasses(rch, "", "CIM_ComputerSystem", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
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
			sa.push_back(OW_String("BrandNewProperty"));
			associatorsClasses(rch, "", "", "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

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

			references(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "cim_systemdevice", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "cim_component", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "", "GroupComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "", "PartComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "", "", E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			references(rch, "", "", E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
			sa.clear();
			references(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
			sa.push_back(OW_String("GroupComponent"));
			references(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

			referencesClasses(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "cim_systemdevice", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "cim_component", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "", "GroupComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "", "PartComponent", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "", "", E_INCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0);
			referencesClasses(rch, "", "", E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
			sa.clear();
			referencesClasses(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);
			sa.push_back(OW_String("GroupComponent"));
			referencesClasses(rch, "", "", E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, &sa);

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
	static bool	 bPrompted = false;

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
