/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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


#include "OW_config.h"
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_SocketBaseImpl.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMParamValue.hpp"

#include <iostream>

#define TEST_ASSERT(CON) if(!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

using std::cerr;
using std::endl;
using std::cout;
using namespace OpenWBEM;
using namespace WBEMFlags;

void usage(const char* name)
{
	cerr << "Usage: " << name << " <url> <mode = 0,r,w,rw> [dump file extension]" << endl;
}

String mode;
CIMClass bionicClass(CIMNULL);
CIMInstance bionicInstance(CIMNULL);

void createClass(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing createClass() *******\n" << endl;
	try
	{
		CIMObjectPath cqtPath("Key", "/root/acltest");
		CIMQualifierType cqt("Key");
		cqt.setDataType(CIMDataType::BOOLEAN);
		cqt.setDefaultValue(CIMValue(Bool(false)));
		CIMQualifier cimQualifierKey(cqt);

		cimQualifierKey.setValue(CIMValue(Bool(true)));

		CIMClass cimClass;
		cimClass.setName("EXP_BionicComputerSystem2");
		cimClass.setSuperClass("CIM_ComputerSystem");

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
		bionicClass = cimClass;

		hdl.createClass("/root/acltest", cimClass);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void enumClassNames(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumClassNames() *******\n" << endl;
	try
	{
		StringEnumeration enu = hdl.enumClassNamesE("/root/acltest", "", E_DEEP);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void enumClasses(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumClasses() *******\n" << endl;
	try
	{
		CIMClassEnumeration enu = hdl.enumClassE("root/acltest", "", E_DEEP);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void modifyClass(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing modifyClass() *******\n" << endl;
	try
	{
		CIMClass cimClass = bionicClass;
		CIMProperty cimProp;
		cimProp.setDataType(CIMDataType::STRING);
		cimProp.setName("BrandNewProperty");
		cimClass.addProperty(cimProp);
		bionicClass = cimClass;
		hdl.modifyClass("/root/acltest", cimClass);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}


void getClass(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getClass() *******\n" << endl;
	try
	{
		CIMClass cimClass = hdl.getClass("/root/acltest",
			"EXP_BionicComputerSystem");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void createInstance(CIMOMHandleIFC& hdl, const String& newInstance)
{
	cout << "\n\n******* Doing createInstance() *******\n" << endl;
	try
	{
		String fromClass = "EXP_BionicComputerSystem2";

		CIMObjectPath cop(fromClass, "/root/acltest");
		CIMClass cimClass = bionicClass;

		CIMInstance newInst = cimClass.newInstance();

		newInst.setProperty(CIMProperty::NAME_PROPERTY,
								  CIMValue(newInstance));
		newInst.setProperty("CreationClassName",
								  CIMValue(fromClass));

		bionicInstance = newInst;
		hdl.createInstance("/root/acltest", newInst);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void enumerateInstanceNames(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumInstanceNames() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPathEnumeration enu = hdl.enumInstanceNamesE("/root/acltest", ofClass);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void enumerateInstances(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumInstances() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMInstanceEnumeration enu = hdl.enumInstancesE("/root/acltest", ofClass, E_DEEP);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void getInstance(CIMOMHandleIFC& hdl, const String& theInstance)
{
	cout << "\n\n******* Doing getInstance() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass, "/root/acltest");
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(theInstance));

		CIMInstance in = hdl.getInstance("/root/acltest", cop);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void modifyInstance(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing modifyInstance() *******\n" << endl;
	try
	{
		CIMInstance in = bionicInstance;
		in.setProperty(CIMProperty("BrandNewProperty",
			CIMValue(Bool(true))));

		hdl.modifyInstance("/root/acltest", in);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void deleteInstance(CIMOMHandleIFC& hdl, const String& theInstance)
{
	cout << "\n\n******* Doing deleteInstance() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem2";
		CIMObjectPath cop(ofClass, "/root/acltest");
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(theInstance));
		hdl.deleteInstance("/root/acltest", cop);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void setQualifier(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing setQualifier() *******\n" << endl;
	try
	{
		String qualName = "borgishness";

		CIMQualifierType qt(qualName);

		qt.setDataType(CIMDataType::STRING);
		qt.setDefaultValue(CIMValue(String()));

		qt.addScope(CIMScope::CLASS);
		qt.addScope(CIMScope::PROPERTY);

		qt.addFlavor(CIMFlavor::DISABLEOVERRIDE);


		hdl.setQualifierType("/root/acltest", qt);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void enumerateQualifiers(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumerateQualifier() *******\n" << endl;
	try
	{
		CIMQualifierTypeEnumeration enu = hdl.enumQualifierTypesE("/root/acltest");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void getQualifier(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getQualifier() *******\n" << endl;
	try
	{
		CIMQualifierType qt = hdl.getQualifierType("/root/acltest", "description");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void associatorNames(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing associatorNames() ****** " << endl;
	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem", "/root/acltest");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMObjectPathEnumeration enu = hdl.associatorNamesE(
			"/root/acltest", cop, "CIM_Component", "", "", "");

		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void associators(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing associators() ****** " << endl;
	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem", "/root/acltest");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));

		cop.setKeyValue("Name", CIMValue(String("SixMillion")));

		CIMInstanceEnumeration enu = hdl.associatorsE("/root/acltest", cop,
									"CIM_Component", "", "", "", E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL);

		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void referenceNames(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing referenceNames() ****** " << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass, "/root/acltest");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SixMillion")));

		CIMObjectPathEnumeration enu = hdl.referenceNamesE("/root/acltest", cop,
			"CIM_Component", "");

		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void references(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing references() ****** " << endl;
	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem", "/root/acltest");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMInstanceEnumeration enu = hdl.referencesE("/root/acltest", cop,
									"CIM_Component", "", E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL);

		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void execReadQuery(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing execQuery() (read) ****** " << endl;
	try
	{
		hdl.execQueryE("/root/acltest",
			"select * from EXP_BionicComputerSystem", "wql1");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void execWriteQuery(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing execQuery() (write) ****** " << endl;
	try
	{
		hdl.execQueryE("/root/acltest",
			"UPDATE EXP_BionicComputerSystem2 SET OptionalArg=false", "wql2");
		if (mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		//TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "rw")
			throw;
	}
}

void deleteQualifier(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteQualifier() *******\n" << endl;
	try
	{
		hdl.deleteQualifierType("/root/acltest", "borgishness");
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}


void deleteClass(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteClass() *******\n" << endl;
	try
	{
		String delClass = "EXP_BionicComputerSystem2";
		hdl.deleteClass("/root/acltest", delClass);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void invokeMethod(CIMOMHandleIFC& hdl, int num)
{
	cout << "\n\n******* Doing invokeMethod() *******\n" << endl;
	try
	{
		CIMObjectPath cop("EXP_BartComputerSystem", "/root/acltest");

		String rval;
		CIMParamValueArray in, out;
		CIMValue cv(CIMNULL);
		switch (num)
		{
			case 1:
				in.push_back(CIMParamValue("newState", CIMValue(String("off"))));
				hdl.invokeMethod("/root/acltest", cop, "setstate", in, out);
				break;
			default:
				break;
		}
		if (mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "rw")
			throw;
	}
}

void createNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing createNameSpace() *******\n" << endl;
	try
	{
		CIMNameSpaceUtils::create__Namespace(hdl, "/root/acltest/sub1");
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void enumNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumNameSpace() *******\n" << endl;
	try
	{
		StringArray rval = CIMNameSpaceUtils::enum__Namespace(hdl, "/root/acltest", E_DEEP);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void deleteNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteNameSpace() *******\n" << endl;
	try
	{
		CIMNameSpaceUtils::delete__Namespace(hdl, "/root/acltest/sub1");
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void getProperty(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getProperty() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass, "/root/acltest");
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(String("SixMillion")));

		CIMValue v = hdl.getProperty("/root/acltest", cop, "OptionalArg");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void setProperty(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing setProperty() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem2";
		CIMObjectPath cop(ofClass, "/root/acltest");
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(String("SixMillion")));

		hdl.setProperty("/root/acltest", cop, "OptionalArg", CIMValue(Bool(true)));
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 3)
		{
			usage(argv[0]);
			return 1;
		}

		if (argc == 4)
		{
			String sockDumpOut = "/tmp/owACLDumpOut";
			String sockDumpIn = "/tmp/owACLDumpIn";
			sockDumpOut += argv[3];
			sockDumpIn += argv[3];
			SocketBaseImpl::setDumpFiles(sockDumpIn.c_str(),
				sockDumpOut.c_str());
		}
		else
		{
			SocketBaseImpl::setDumpFiles("","");
		}

		mode = argv[2];
		if (mode != "r" && mode != "w" && mode != "rw" && mode != "0")
		{
			usage(argv[0]);
			return 1;
		}
		if (mode == "0")
		{
			mode = "";
		}

		String url(argv[1]);

		URL owurl(url);
		CIMOMHandleIFCRef rchRef;

		CIMProtocolIFCRef client;
		client = new HTTPClient(url);

		if (owurl.path.equalsIgnoreCase("/owbinary"))
		{
			rchRef = new BinaryCIMOMHandle(client);
		}
		else
		{
			rchRef = new CIMXMLCIMOMHandle(client);
		}

		CIMOMHandleIFC& rch = *rchRef;


		createNameSpace(rch);
		enumNameSpace(rch);
		deleteNameSpace(rch);
		createClass(rch);
		enumClassNames(rch);
		enumClasses(rch);
		modifyClass(rch);
		getClass(rch);
		createInstance(rch, "SixMillion");
		enumerateInstanceNames(rch);
		enumerateInstances(rch);
		getInstance(rch, "SixMillion");
		modifyInstance(rch);
		setProperty(rch);
		getProperty(rch);
		setQualifier(rch);
		enumerateQualifiers(rch);
		getQualifier(rch);

		associatorNames(rch);
		associators(rch);
		referenceNames(rch);
		references(rch);
		execReadQuery(rch);
		execWriteQuery(rch);

		deleteInstance(rch, "SixMillion");
		deleteClass(rch);
		deleteQualifier(rch);

		invokeMethod(rch, 1);

		return 0;

	}
	catch (Assertion& a)
	{
		cerr << "Caught Assertion: " << a << endl;
	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}

