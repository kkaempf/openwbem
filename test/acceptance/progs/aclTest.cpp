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

#define TEST_ASSERT(CON) if(!(CON)) throw OW_AssertionException(__FILE__, __LINE__, #CON)

using std::cerr;
using std::endl;
using std::cout;

void usage(const char* name)
{
	cerr << "Usage: " << name << " <url> <mode = 0,r,w,rw> [dump file extension]" << endl;
}

OW_String mode;
OW_CIMClass bionicClass(OW_CIMNULL);
OW_CIMInstance bionicInstance(OW_CIMNULL);

void createClass(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing createClass() *******\n" << endl;
	try
	{
		OW_CIMObjectPath cqtPath("Key", "/root/acltest");
		OW_CIMQualifierType cqt("Key");
		cqt.setDataType(OW_CIMDataType::BOOLEAN);
		cqt.setDefaultValue(OW_CIMValue(OW_Bool(false)));
		OW_CIMQualifier cimQualifierKey(cqt);

		cimQualifierKey.setValue(OW_CIMValue(OW_Bool(true)));

		OW_CIMClass cimClass;
		cimClass.setName("EXP_BionicComputerSystem2");
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
		bionicClass = cimClass;

		hdl.createClass("/root/acltest", cimClass);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void enumClassNames(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumClassNames() *******\n" << endl;
	try
	{
		OW_CIMObjectPathEnumeration enu = hdl.enumClassNamesE("/root/acltest", "", true);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void enumClasses(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumClasses() *******\n" << endl;
	try
	{
		OW_CIMClassEnumeration enu = hdl.enumClassE("root/acltest", "", true, false);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void modifyClass(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing modifyClass() *******\n" << endl;
	try
	{
		OW_CIMClass cimClass = bionicClass;
		OW_CIMProperty cimProp;
		cimProp.setDataType(OW_CIMDataType::STRING);
		cimProp.setName("BrandNewProperty");
		cimClass.addProperty(cimProp);
		bionicClass = cimClass;
		hdl.modifyClass("/root/acltest", cimClass);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void getClass(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getClass() *******\n" << endl;
	try
	{
		OW_CIMClass cimClass = hdl.getClass("/root/acltest",
			"EXP_BionicComputerSystem", false);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void createInstance(OW_CIMOMHandleIFC& hdl, const OW_String& newInstance)
{
	cout << "\n\n******* Doing createInstance() *******\n" << endl;
	try
	{
		OW_String fromClass = "EXP_BionicComputerSystem2";

		OW_CIMObjectPath cop(fromClass, "/root/acltest");
		OW_CIMClass cimClass = bionicClass;

		OW_CIMInstance newInst = cimClass.newInstance();

		newInst.setName(newInstance);
		newInst.setProperty(OW_CIMProperty::NAME_PROPERTY,
								  OW_CIMValue(newInstance));
		newInst.setProperty("CreationClassName",
								  OW_CIMValue(fromClass));

		bionicInstance = newInst;
		hdl.createInstance("/root/acltest", newInst);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void enumerateInstanceNames(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumInstanceNames() *******\n" << endl;
	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPathEnumeration enu = hdl.enumInstanceNamesE("/root/acltest", ofClass);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void enumerateInstances(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumInstances() *******\n" << endl;
	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMInstanceEnumeration enu = hdl.enumInstancesE("/root/acltest", ofClass, true);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void getInstance(OW_CIMOMHandleIFC& hdl, const OW_String& theInstance)
{
	cout << "\n\n******* Doing getInstance() *******\n" << endl;
	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass, "/root/acltest");
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(theInstance));

		OW_CIMInstance in = hdl.getInstance("/root/acltest", cop, false);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void modifyInstance(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing modifyInstance() *******\n" << endl;
	try
	{
		OW_CIMInstance in = bionicInstance;
		in.setProperty(OW_CIMProperty("BrandNewProperty",
			OW_CIMValue(OW_Bool(true))));

		hdl.modifyInstance("/root/acltest", in);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void deleteInstance(OW_CIMOMHandleIFC& hdl, const OW_String& theInstance)
{
	cout << "\n\n******* Doing deleteInstance() *******\n" << endl;
	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem2";
		OW_CIMObjectPath cop(ofClass, "/root/acltest");
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(theInstance));
		hdl.deleteInstance("/root/acltest", cop);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void setQualifier(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing setQualifier() *******\n" << endl;
	try
	{
		OW_String qualName = "borgishness";

		OW_CIMQualifierType qt(qualName);

		qt.setDataType(OW_CIMDataType::STRING);
		qt.setDefaultValue(OW_CIMValue(OW_String()));

		qt.addScope(OW_CIMScope::CLASS);
		qt.addScope(OW_CIMScope::PROPERTY);

		qt.addFlavor(OW_CIMFlavor::DISABLEOVERRIDE);


		hdl.setQualifierType("/root/acltest", qt);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void enumerateQualifiers(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumerateQualifier() *******\n" << endl;
	try
	{
		OW_CIMQualifierTypeEnumeration enu = hdl.enumQualifierTypesE("/root/acltest");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void getQualifier(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getQualifier() *******\n" << endl;
	try
	{
		OW_CIMQualifierType qt = hdl.getQualifierType("/root/acltest", "description");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void associatorNames(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing associatorNames() ****** " << endl;
	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem", "/root/acltest");
		cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BionicComputerSystem")));
		cop.addKey("Name", OW_CIMValue(OW_String("SevenMillion")));

		OW_CIMObjectPathEnumeration enu = hdl.associatorNamesE(
			"/root/acltest", cop, "CIM_Component", "", "", "");

		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void associators(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing associators() ****** " << endl;
	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem", "/root/acltest");
		cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BionicComputerSystem")));

		cop.addKey("Name", OW_CIMValue(OW_String("SixMillion")));

		OW_CIMInstanceEnumeration enu = hdl.associatorsE("/root/acltest", cop,
									"CIM_Component", "", "", "", true, true, NULL);

		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void referenceNames(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing referenceNames() ****** " << endl;
	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass, "/root/acltest");
		cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BionicComputerSystem")));
		cop.addKey("Name", OW_CIMValue(OW_String("SixMillion")));

		OW_CIMObjectPathEnumeration enu = hdl.referenceNamesE("/root/acltest", cop,
			"CIM_Component", "");

		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void references(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing references() ****** " << endl;
	try
	{
		OW_CIMObjectPath cop("EXP_BionicComputerSystem", "/root/acltest");
		cop.addKey("CreationClassName",
					  OW_CIMValue(OW_String("EXP_BionicComputerSystem")));
		cop.addKey("Name", OW_CIMValue(OW_String("SevenMillion")));

		OW_CIMInstanceEnumeration enu = hdl.referencesE("/root/acltest", cop,
									"CIM_Component", "", true, true, NULL);

		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

void execReadQuery(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing execQuery() (read) ****** " << endl;
	try
	{
		hdl.execQueryE("/root/acltest",
			"select * from EXP_BionicComputerSystem", "wql1");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void execWriteQuery(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing execQuery() (write) ****** " << endl;
	try
	{
		hdl.execQueryE("/root/acltest",
			"UPDATE EXP_BionicComputerSystem2 SET OptionalArg=false", "wql2");
		if (mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		//TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "rw")
			throw;
	}
}

void deleteQualifier(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteQualifier() *******\n" << endl;
	try
	{
		hdl.deleteQualifierType("/root/acltest", "borgishness");
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void deleteClass(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteClass() *******\n" << endl;
	try
	{
		OW_String delClass = "EXP_BionicComputerSystem2";
		hdl.deleteClass("/root/acltest", delClass);
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void invokeMethod(OW_CIMOMHandleIFC& hdl, int num)
{
	cout << "\n\n******* Doing invokeMethod() *******\n" << endl;
	try
	{
		OW_CIMObjectPath cop("EXP_BartComputerSystem", "/root/acltest");

		OW_String rval;
		OW_CIMParamValueArray in, out;
		OW_CIMValue cv(OW_CIMNULL);
		switch (num)
		{
			case 1:
				in.push_back(OW_CIMParamValue("newState", OW_CIMValue(OW_String("off"))));
				hdl.invokeMethod("/root/acltest", cop, "setstate", in, out);
				break;
			default:
				break;
		}
		if (mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "rw")
			throw;
	}
}

void createNameSpace(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing createNameSpace() *******\n" << endl;
	try
	{
		OW_CIMNameSpaceUtils::create__Namespace(OW_CIMOMHandleIFCRef(&hdl, true), "/root/acltest/Caldera");
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void enumNameSpace(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumNameSpace() *******\n" << endl;
	try
	{
		OW_StringArray rval = OW_CIMNameSpaceUtils::enum__Namespace(OW_CIMOMHandleIFCRef(&hdl, true), "/root/acltest", true);
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void deleteNameSpace(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteNameSpace() *******\n" << endl;
	try
	{
		OW_CIMNameSpaceUtils::delete__Namespace(OW_CIMOMHandleIFCRef(&hdl, true), "/root/acltest/Caldera");
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "w" || mode == "rw")
			throw;
	}
}

void getProperty(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getProperty() *******\n" << endl;
	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem";
		OW_CIMObjectPath cop(ofClass, "/root/acltest");
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(OW_String("SixMillion")));

		OW_CIMValue v = hdl.getProperty("/root/acltest", cop, "OptionalArg");
		if (mode != "r" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
		if (mode == "r" || mode == "rw")
			throw;
	}
}

void setProperty(OW_CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing setProperty() *******\n" << endl;
	try
	{
		OW_String ofClass = "EXP_BionicComputerSystem2";
		OW_CIMObjectPath cop(ofClass, "/root/acltest");
		cop.addKey("CreationClassName", OW_CIMValue(ofClass));
		cop.addKey("Name", OW_CIMValue(OW_String("SixMillion")));

		hdl.setProperty("/root/acltest", cop, "OptionalArg", OW_CIMValue(OW_Bool(true)));
		if (mode != "w" && mode != "rw")
			TEST_ASSERT(0);
	}
	catch (OW_CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == OW_CIMException::ACCESS_DENIED);
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
			OW_String sockDumpOut = "/tmp/owACLDumpOut";
			OW_String sockDumpIn = "/tmp/owACLDumpIn";
			sockDumpOut += argv[3];
			sockDumpIn += argv[3];
			OW_SocketBaseImpl::setDumpFiles(sockDumpIn.c_str(),
				sockDumpOut.c_str());
		}
		else
		{
			OW_SocketBaseImpl::setDumpFiles("","");
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

		OW_String url(argv[1]);

		OW_URL owurl(url);
		OW_CIMOMHandleIFCRef rchRef;

		OW_CIMProtocolIFCRef client;
		client = new OW_HTTPClient(url);

		if (owurl.path.equalsIgnoreCase("/owbinary"))
		{
			rchRef = new OW_BinaryCIMOMHandle(client);
		}
		else
		{
			rchRef = new OW_CIMXMLCIMOMHandle(client);
		}

		OW_CIMOMHandleIFC& rch = *rchRef;


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

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		associatorNames(rch);
		associators(rch);
		referenceNames(rch);
		references(rch);
#endif
		execReadQuery(rch);
		execWriteQuery(rch);

		deleteInstance(rch, "SixMillion");
		deleteClass(rch);
		deleteQualifier(rch);

		invokeMethod(rch, 1);

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

