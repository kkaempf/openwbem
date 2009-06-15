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
#include "OW_ClientCIMOMHandle.hpp"
#include "blocxx/SocketBaseImpl.hpp"
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
#include "blocxx/Bool.hpp"

#include <iostream>
#include <cstring>

#define TEST_ASSERT(CON) if (!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

using std::cerr;
using std::endl;
using std::cout;
using namespace OpenWBEM;
using namespace WBEMFlags;

void usage(char const * const * argv)
{
	cerr << "Usage: " << argv[0] << " <url> <mode ~ [rwRWN]*> [dump file extension]" << endl;
	cerr << "(Your call: ";
	while (*argv)
	{
		cerr << (**argv ? *argv : "''") << " ";
		++argv;
	}
	cerr << ")" << endl;
}

bool valid_mode(String const & mode)
{
	return std::strspn(mode.c_str(), "rwRWN") == mode.length();
}

String mode;
CIMClass bionicClass(CIMNULL);
CIMInstance simpleInstance(CIMNULL);

inline bool havePerm(char c)
{
	return mode.indexOf(c) != String::npos;
}

CIMClass makeCIMClass(
	char const * class_name, char const * super_class_name,
	char const * const * properties, size_t nelts
)
{
	CIMQualifierType cqt("Key");
	cqt.setDataType(CIMDataType::BOOLEAN);
	cqt.setDefaultValue(CIMValue(Bool(false)));
	CIMQualifier cimQualifierKey(cqt);
	cimQualifierKey.setValue(CIMValue(Bool(true)));

	CIMClass cimClass;
	cimClass.setName(class_name);
	cimClass.setSuperClass(super_class_name);

	for (size_t i = 0; i < nelts; i += 3)
	{
		String name = properties[i];
		String type = properties[i + 1];
		String isKey = properties[i + 2];
		CIMProperty cimProp;
		if (type == "string")
		{
			cimProp.setDataType(CIMDataType::STRING);
		}
		else if (type == "uint32")
		{
			cimProp.setDataType(CIMDataType::UINT32);
		}
		else if (type == "boolean")
		{
			cimProp.setDataType(CIMDataType::BOOLEAN);
		}
		else if (type == "datetime")
		{
			cimProp.setDataType(CIMDataType::DATETIME);
		}
		else
		{
			cimProp.setDataType(CIMDataType::STRING);
		}

		cimProp.setName(name);
		if (isKey == "true")
		{
			cimProp.addQualifier(cimQualifierKey);
		}
		cimClass.addProperty(cimProp);
	}
	return cimClass;
}

template <size_t nelts>
CIMClass makeCIMClass(
	char const * class_name, char const * super_class_name,
	char const * const (&properties)[nelts]
)
{
	return makeCIMClass(class_name, super_class_name, properties, nelts);
}

void createClass(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing createClass() *******\n" << endl;
	try
	{
		char const * const zargs[] =
		{
			"CreationClassName", "string",  "true",
			"Name",              "string",  "true",
			"OptionalArg",       "boolean", "false"
		};
		CIMClass cimClass = makeCIMClass("EXP_BionicComputerSystem2", "", zargs);
			
		bionicClass = cimClass;

		hdl.createClass("root/acltest", cimClass);
		if (!havePerm('W'))
		{
			cerr << "create class should have failed." << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('W'))
		{
			cerr << "create class should not have failed." << endl;
			throw;
		}
	}
}

void enumClassNames(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumClassNames() *******\n" << endl;
	try
	{
		StringEnumeration enu = hdl.enumClassNamesE("root/acltest", "", E_DEEP);
		if (!havePerm('R'))
		{
			cerr << "enumClassNamesE should have failed." << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('R'))
		{
			cerr << "enumClassNamesE should NOT have failed." << endl;
			throw;
		}
	}
}

void enumClasses(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumClasses() *******\n" << endl;
	try
	{
		CIMClassEnumeration enu = hdl.enumClassE("root/acltest", "", E_DEEP);
		if (!havePerm('R'))
		{
			cerr << "enumClassE should have failed" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('R'))
		{
			cerr << "enumClassE should NOT have failed" << endl;
			throw;
		}
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
		hdl.modifyClass("root/acltest", cimClass);
		if (!havePerm('W'))
		{
			cerr << "modifyClass should have failed here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('W'))
		{
			cerr << "modifyClass should NOT have failed here" << endl;
			throw;
		}
	}
}


void getClass(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getClass() *******\n" << endl;
	try
	{
		CIMClass cimClass = hdl.getClass("root/acltest",
			"EXP_BionicComputerSystem");
		if (!havePerm('R'))
		{
			cerr << "getClass should have failed here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('R'))
		{
			cerr << "getClass should NOT have failed here" << endl;
			throw;
		}
	}
}

// Creates the EXP_Simple class, defined in ACLTest.mof, without reading
// it from the repository.
//
CIMClass makeSimpleCIMClass()
{
	char const * const props[] = {
		"the_key",   "string", "true",
		"the_value", "uint64",  "false"
	};
	return makeCIMClass("EXP_Simple", "", props);
}

void createInstance(CIMOMHandleIFC& hdl, const String& newInstance)
{
	cout << "\n\n******* Doing createInstance() *******\n" << endl;
	try
	{
		CIMClass cimClass = makeSimpleCIMClass();

		CIMInstance newInst = cimClass.newInstance();
		newInst.setProperty("the_key", CIMValue(newInstance));
		newInst.setProperty(
			"the_value", CIMValue(static_cast<UInt32>(38479287)));
		simpleInstance = newInst;
		hdl.createInstance("root/acltest", newInst);
		if (!havePerm('w'))
		{
			cerr << "createInstance should have failed here. mode = " << mode << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		cerr << "Current Mode = " << mode << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('w'))
		{
			cerr << "createInstance should NOT have failed here. mode = " << mode << endl;
			throw;
		}
	}
}

void enumerateInstanceNames(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumInstanceNames() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPathEnumeration enu = hdl.enumInstanceNamesE("root/acltest", ofClass);
		if (!havePerm('r'))
		{
			cerr << "enumInstanceNamesE should have failed here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
		{
			cerr << "enumInstanceNamesE should NOT have failed here" << endl;
			throw;
		}
	}
}

void enumerateInstances(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumInstances() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMInstanceEnumeration enu = hdl.enumInstancesE("root/acltest", ofClass, E_DEEP);
		if (!havePerm('r'))
		{
			cerr << "enumInstancesE should have failed here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
		{
			cerr << "enumInstancesE should NOT have failed here" << endl;
			throw;
		}
	}
}

void getInstance(CIMOMHandleIFC& hdl, const String& theInstance)
{
	cout << "\n\n******* Doing getInstance() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass, "root/acltest");
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(theInstance));

		CIMInstance in = hdl.getInstance("root/acltest", cop);
		if (!havePerm('r'))
		{
			cerr << "getInstance should have failed here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
		{
			cerr << "getInstance should NOT have failed here" << endl;
			throw;
		}
	}
}

void modifyInstance(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing modifyInstance() *******\n" << endl;
	try
	{
		CIMInstance in = simpleInstance;
		in.updatePropertyValue(
			"the_value", CIMValue(static_cast<UInt32>(1000000)));

		hdl.modifyInstance("root/acltest", in);
		if (!havePerm('w'))
		{
			cerr << "modifyInstance should have failed here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('w'))
		{
			cerr << "modifyInstance should NOT have failed here" << endl;
			throw;
		}
	}
}

void deleteInstance(CIMOMHandleIFC& hdl, const String& theInstance) {
	cout << "\n\n******* Doing deleteInstance() *******\n" << endl;
	try
	{
		String ofClass = "EXP_Simple";
		CIMObjectPath cop(ofClass, "root/acltest");
		cop.setKeyValue("the_key", CIMValue(theInstance));
		hdl.deleteInstance("root/acltest", cop);
		if (!havePerm('w'))
		{
			cerr << "deleteInstance should have failed here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('w'))
		{
			cerr << "deleteInstance should NOT have failed here" << endl;
			throw;
		}
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


		hdl.setQualifierType("root/acltest", qt);
		if (!havePerm('W'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('W'))
			throw;
	}
}

void enumerateQualifiers(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumerateQualifier() *******\n" << endl;
	try
	{
		CIMQualifierTypeEnumeration enu = hdl.enumQualifierTypesE("root/acltest");
		if (!havePerm('R'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('R'))
			throw;
	}
}

void getQualifier(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getQualifier() *******\n" << endl;
	try
	{
		CIMQualifierType qt = hdl.getQualifierType("root/acltest", "description");
		if (!havePerm('R'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('R'))
			throw;
	}
}

void associatorNames(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing associatorNames() ****** " << endl;
	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem", "root/acltest");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMObjectPathEnumeration enu = hdl.associatorNamesE(
			"root/acltest", cop, "CIM_Component", "", "", "");

		if (!havePerm('r'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
			throw;
	}
}

void associators(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing associators() ****** " << endl;
	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem", "root/acltest");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));

		cop.setKeyValue("Name", CIMValue(String("SixMillion")));

		CIMInstanceEnumeration enu = hdl.associatorsE("root/acltest", cop,
									"CIM_Component", "", "", "", E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL);

		if (!havePerm('r'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
			throw;
	}
}

void referenceNames(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing referenceNames() ****** " << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass, "root/acltest");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SixMillion")));

		CIMObjectPathEnumeration enu = hdl.referenceNamesE("root/acltest", cop,
			"CIM_Component", "");

		if (!havePerm('r'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
			throw;
	}
}

void references(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing references() ****** " << endl;
	try
	{
		CIMObjectPath cop("EXP_BionicComputerSystem", "root/acltest");
		cop.setKeyValue("CreationClassName",
					  CIMValue(String("EXP_BionicComputerSystem")));
		cop.setKeyValue("Name", CIMValue(String("SevenMillion")));

		CIMInstanceEnumeration enu = hdl.referencesE("root/acltest", cop,
									"CIM_Component", "", E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL);

		if (!havePerm('r'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
			throw;
	}
}

void execReadQuery(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing execQuery() (read) ****** " << endl;
	try
	{
		hdl.execQueryE("root/acltest",
			"select * from EXP_BionicComputerSystem", "wql1");
		if (!havePerm('r'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
			throw;
	}
}

void execWriteQuery(CIMOMHandleIFC& hdl)
{
	cout << "\n ***** Doing execQuery() (write) ****** " << endl;
	try
	{
		hdl.execQueryE(
			"root/acltest",	"UPDATE EXP_Simple SET the_value=1234567", "wql2");
		if (!havePerm('r') || !havePerm('w'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r') && havePerm('w'))
			throw;
	}
}

void deleteQualifier(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteQualifier() *******\n" << endl;
	try
	{
		hdl.deleteQualifierType("root/acltest", "borgishness");
		if (!havePerm('W'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('W'))
			throw;
	}
}


void deleteClass(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteClass() *******\n" << endl;
	try
	{
		String delClass = "EXP_BionicComputerSystem2";
		hdl.deleteClass("root/acltest", delClass);
		if (!havePerm('W'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('W'))
			throw;
	}
}

void invokeMethod(CIMOMHandleIFC& hdl, int num)
{
	cout << "\n\n******* Doing invokeMethod() *******\n" << endl;
	try
	{
		CIMObjectPath cop("EXP_BartComputerSystem", "root/acltest");

		String rval;
		CIMParamValueArray in, out;
		CIMValue cv(CIMNULL);
		switch (num)
		{
			case 1:
				in.push_back(CIMParamValue("newState", CIMValue(String("off"))));
				hdl.invokeMethod("root/acltest", cop, "setstate", in, out);
				break;
			default:
				break;
		}
		if (!havePerm('r') || !havePerm('w'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r') && havePerm('w'))
			throw;
	}
}

bool deleteNameSpacePerm()
{
	return havePerm('N') && havePerm('r') && havePerm('w');
}

bool createNameSpacePerm()
{
	return havePerm('N') && havePerm('w');
}

void createNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing createNameSpace() *******\n" << endl;
	bool perm = createNameSpacePerm();
	if (perm != deleteNameSpacePerm())
	{
		cout << "Skipped." << endl;
		return;
	}
	try
	{
		CIMNameSpaceUtils::create__Namespace(hdl, "root/acltest/sub1");
		if (!perm)
		{
			cerr << "create__Namespace should have failed. mode = " << mode << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (perm)
		{
			cerr << "create__Namespace should NOT failed. mode = " << mode << endl;
			throw;
		}
	}
}

bool createCIMNameSpacePerm()
{
	return havePerm('R') && havePerm('r') && havePerm('w') && havePerm('N');
}

bool deleteCIMNameSpacePerm()
{
	return havePerm('r') && havePerm('w') && havePerm('N');
}

void createCIMNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing createCIMNameSpace() *******\n" << endl;
	bool perm = createCIMNameSpacePerm();
	if (perm != deleteCIMNameSpacePerm())
	{
		cout << "Skipped." << endl;
		return;
	}
	try
	{
		CIMNameSpaceUtils::createCIM_Namespace(hdl, "root/acltest/sub1", 0, "", "root/acltest");
		if (!perm)
		{
			cerr << "createCIM_Namespace should have failed. mode = " << mode << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (perm)
		{
			cerr << "createCIM_Namespace should NOT failed. mode = " << mode << endl;
			throw;
		}
	}
}

void enumNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumNameSpace() *******\n" << endl;
	try
	{
		StringArray rval = CIMNameSpaceUtils::enum__Namespace(hdl, "root/acltest", E_SHALLOW);
		for (size_t i = 0; i < rval.size(); ++i)
		{
			cout << "  " << rval[i] << endl;
		}
		if (!havePerm('r'))
		{
			cerr << "enum__Namespace should have failed, mode = " << mode << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
		{
			cerr << "enum__Namespace should NOT have failed, mode = " << mode << endl;
			throw;
		}
	}
}

void enumCIMNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing enumCIMNameSpace() *******\n" << endl;
	bool perm = havePerm('r');
	try
	{
		StringArray rval = CIMNameSpaceUtils::enumCIM_Namespace(hdl, "root/acltest");
		for (size_t i = 0; i < rval.size(); ++i)
		{
			cout << "  " << rval[i] << endl;
		}
		if (!perm)
		{
			cerr << "enumCIM_Namespace should have failed, mode = " << mode << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (perm)
		{
			cerr << "enumCIM_Namespace should NOT have failed, mode = " << mode << endl;
			throw;
		}
	}
}

void deleteNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteNameSpace() *******\n" << endl;
	bool perm = deleteNameSpacePerm();
	if (perm != createNameSpacePerm())
	{
		cout << "Skipped." << endl;
		return;
	}
	try
	{
		char const * ns = perm ? "root/acltest/sub1" : "root/acltest/sub0";
		CIMNameSpaceUtils::delete__Namespace(hdl, ns);
		if (!perm)
		{
			cerr << "delete__Namespace should NOT have worked here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (perm)
		{
			cerr << "delete__Namespace should have worked here. mode = "
				<< mode << endl;
			throw;
		}
	}
}

void deleteCIMNameSpace(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing deleteCIMNameSpace() *******\n" << endl;
	bool perm = deleteCIMNameSpacePerm();
	if (perm != createCIMNameSpacePerm())
	{
		cout << "Skipped." << endl;
		return;
	}
	try
	{
		char const * ns = perm ? "root/acltest/sub1" : "root/acltest/sub0";
		CIMNameSpaceUtils::deleteCIM_Namespace(hdl, ns, "root/acltest");
		if (!perm)
		{
			cerr << "deleteCIM_Namespace should NOT have worked here" << endl;
			TEST_ASSERT(0);
		}
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (perm)
		{
			cerr << "deleteCIM_Namespace should have worked here. mode = "
				<< mode << endl;
			throw;
		}
	}
}

void getProperty(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing getProperty() *******\n" << endl;
	try
	{
		String ofClass = "EXP_BionicComputerSystem";
		CIMObjectPath cop(ofClass, "root/acltest");
		cop.setKeyValue("CreationClassName", CIMValue(ofClass));
		cop.setKeyValue("Name", CIMValue(String("SixMillion")));

		CIMValue v = hdl.getProperty("root/acltest", cop, "OptionalArg");
		if (!havePerm('r'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('r'))
			throw;
	}
}

void setProperty(CIMOMHandleIFC& hdl)
{
	cout << "\n\n******* Doing setProperty() *******\n" << endl;
	try
	{
		String ofClass = "EXP_Simple";
		CIMObjectPath cop(ofClass, "root/acltest");
		cop.setKeyValue("the_key", simpleInstance.getPropertyValue("the_key"));

		hdl.setProperty(
			"root/acltest", cop, "the_value",
			CIMValue(static_cast<UInt32>(2000000)));
		if (!havePerm('w'))
			TEST_ASSERT(0);
	}
	catch (CIMException& e)
	{
		cerr << e << endl;
		TEST_ASSERT(e.getErrNo() == CIMException::ACCESS_DENIED);
		if (havePerm('w'))
			throw;
	}
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 3)
		{
			usage(argv);
			return 1;
		}

		if (argc == 4)
		{
			String sockDumpOut = "/tmp/owACLBasicDumpOut";
			String sockDumpIn = "/tmp/owACLBasicDumpIn";
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
		if (!valid_mode(mode))
		{
			usage(argv);
			return 1;
		}

		String url(argv[1]);

		CIMOMHandleIFCRef chRef = ClientCIMOMHandle::createFromURL(url);

		CIMOMHandleIFC& rch = *chRef;


		createNameSpace(rch);
		enumNameSpace(rch);
		deleteNameSpace(rch);

		createCIMNameSpace(rch);
		enumCIMNameSpace(rch);
		deleteCIMNameSpace(rch);

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

