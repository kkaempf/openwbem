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
#include "OW_CIMValueCast.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_Format.hpp"
#include "OW_GetPass.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMParamValue.hpp"

#include <iostream>

#define TEST_ASSERT(CON) if(!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using namespace OpenWBEM;

//////////////////////////////////////////////////////////////////////////////
// This program tries to get the CIMOM to produce each type of CIM error.
// We don't try for CIM_ERR_ACCESS_DENIED because the aclTest does that.
// We also don't try for CIM_ERR_FAILED, because that usually only happens when
// the CIMOM has a really bad internal error (such as a full filesystem).
/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
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
			(void)details;
			cout << "Authentication required for " << realm << endl;
			cout << "Enter the user name: ";
			name = String::getLine(cin);
			passwd = GetPass::getPass("Enter the password for " +
				name + ": ");
			return true;
		}
};

void runTests(const CIMOMHandleIFCRef& hdl);

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
		CIMProtocolIFCRef client;
		client = new HTTPClient(url);
		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
		client->setLoginCallBack(getLoginInfo);
		CIMOMHandleIFCRef chRef;
		if (owurl.path.equalsIgnoreCase("/owbinary"))
		{
			chRef = new BinaryCIMOMHandle(client);
		}
		else
		{
			chRef = new CIMXMLCIMOMHandle(client);
		}

		runTests(chRef);

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

void runTests(const CIMOMHandleIFCRef& hdl)
{
	// GetClass
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->getClass("badNamespace", "foo");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - Can't do without doing straight XML.  Handle it in a separate test.

	// CIM_ERR_NOT_FOUND
	try
	{
		hdl->getClass("root/testsuite", "fooXXX");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}


	// GetInstance

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMObjectPath cop("foo");
		cop.setKeyValue("fooKey", CIMValue(String("fooKeyValue")));
		hdl->getInstance("badNamespace", cop);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - Can't do without doing straight XML.  Handle it in a separate test.

	// CIM_ERR_INVALID_CLASS
	try
	{
		CIMObjectPath cop("fooXXX");
		cop.setKeyValue("fooKey", CIMValue(String("fooKeyValue")));
		hdl->getInstance("root/testsuite", cop);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		CIMObjectPath cop("CIM_PhysicalElement");
		cop.setKeyValue("CreationClassName", CIMValue(String("fooKeyValue")));
		cop.setKeyValue("Tag", CIMValue(String("fooKeyValue")));
		hdl->getInstance("root/testsuite", cop);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}


	// DeleteClass
	
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->deleteClass("badNamespace", "foo");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}


	// CIM_ERR_INVALID_PARAMETER - Can only be done with doctored XML

	// CIM_ERR_NOT_FOUND
	try
	{
		hdl->deleteClass("root/testsuite", "fooXXX");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}


	// CIM_ERR_CLASS_HAS_CHILDREN - Impossible to produce with OpenWBEM

	// CIM_ERR_CLASS_HAS_INSTANCES - Impossible to produce with OpenWBEM

	// DeleteInstance

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMObjectPath cop("foo");
		cop.setKeyValue("fooKey", CIMValue(String("fooKeyValue")));
		hdl->deleteInstance("badNamespace", cop);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - Can only be done with doctored XML

	// CIM_ERR_INVALID_CLASS
	try
	{
		CIMObjectPath cop("fooXXX");
		cop.setKeyValue("fooKey", CIMValue(String("fooKeyValue")));
		hdl->deleteInstance("root/testsuite", cop);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		CIMObjectPath cop("CIM_PhysicalElement");
		cop.setKeyValue("CreationClassName", CIMValue(String("fooKeyValue")));
		cop.setKeyValue("Tag", CIMValue(String("fooKeyValue")));
		hdl->deleteInstance("root/testsuite", cop);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}


	// CreateClass

	CIMClass cc("footest");
	CIMQualifierType keyQualType = hdl->getQualifierType("root/testsuite",
		CIMQualifier::CIM_QUAL_KEY);
	CIMQualifier keyQual(keyQualType);
	keyQual.setValue(CIMValue(true));
	CIMProperty theKeyProp("theKeyProp", CIMDataType::BOOLEAN);
	theKeyProp.addQualifier(keyQual);
	theKeyProp.setValue(CIMValue(true));

	cc.addProperty(theKeyProp);

	// create a base class that has the associator qualifier, which can't be overridden
	CIMClass baseClass("invalidTestBase");
	CIMQualifierType assocQualType = hdl->getQualifierType("root/testsuite",
		CIMQualifier::CIM_QUAL_ASSOCIATION);
	CIMQualifier assocQual(assocQualType);
	assocQual.setValue(CIMValue(true));
	baseClass.addProperty(theKeyProp);
	baseClass.addQualifier(assocQual);

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->createClass("badNamespace", cc);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - thrown from MetaRepository::_throwIfBadClass() and MetaRepository::adjustClass()

	// There are different ways to get this error.  Let's try all of them.
	// 1. A subclass overrides a qualifier that has the DISABLEOVERRIDE flavor
	// on the base class

	try
	{
		try
		{
			hdl->deleteClass("root/testsuite", baseClass.getName());
		}
		catch (const CIMException&)
		{
		}
		hdl->createClass("root/testsuite", baseClass);
	}
	catch (const CIMException& e)
	{
		if (e.getErrNo() != CIMException::ALREADY_EXISTS)
		{
			throw e;
		}
	}

	try
	{
		CIMInstance ci = baseClass.newInstance();
		ci.setProperty(theKeyProp);
		hdl->createInstance("root/testsuite", ci);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(0);
	}



	// setup's done, now do the tests
	try
	{
		// test overriding an DISABLEOVERRIDE qualifier
		CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		CIMQualifier assocQual2(assocQual);
		assocQual2.setValue(CIMValue(false));
		cc2.addQualifier(assocQual2);
		hdl->createClass("root/testsuite", cc2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_PARAMETER);
	}

	try
	{
		// test adding an key to a subclass when the parent already has keys.
		CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		CIMProperty theKeyProp2("theKeyProp2", CIMDataType::BOOLEAN);
		theKeyProp2.addQualifier(keyQual);
		cc2.addProperty(theKeyProp2);
		hdl->createClass("root/testsuite", cc2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_PARAMETER);
	}

	// CIM_ERR_ALREADY_EXISTS
	try
	{
		// test adding a class with no keys
		hdl->createClass("root/testsuite", baseClass);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::ALREADY_EXISTS);
	}

	// CIM_ERR_INVALID_SUPERCLASS
	try
	{
		CIMClass cc2(baseClass);
		cc2.setSuperClass("invalid");
		hdl->createClass("root/testsuite", cc2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_SUPERCLASS);
	}


	// CreateInstance

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMInstance ci = baseClass.newInstance();
		ci.setProperty(theKeyProp);
		hdl->createInstance("badNamespace", ci);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER

	// CIM_ERR_INVALID_CLASS
	try
	{
		CIMInstance ci = baseClass.newInstance();
		ci.setClassName("nonexistentClass");
		ci.setProperty(theKeyProp);
		hdl->createInstance("root/testsuite", ci);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}

	// CIM_ERR_ALREADY_EXISTS
	try
	{
		CIMInstance ci = baseClass.newInstance();
		ci.setProperty(theKeyProp);
		hdl->createInstance("root/testsuite", ci);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::ALREADY_EXISTS);
	}


	// ModifyClass

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->modifyClass("badNamespace", cc);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// first create a class to modify
	try
	{
		CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		CIMObjectPath cop(cc2.getName(), "root/testsuite");
		hdl->createClass("root/testsuite", cc2);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(0);
	}
	
	try
	{
		// test overriding an DISABLEOVERRIDE qualifier
		CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		CIMQualifier assocQual2(assocQual);
		assocQual2.setValue(CIMValue(false));
		cc2.addQualifier(assocQual2);
		hdl->modifyClass("root/testsuite", cc2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_PARAMETER);
	}

	try
	{
		// test adding an key to a subclass when the parent already has keys.
		CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		CIMProperty theKeyProp2("theKeyProp2", CIMDataType::BOOLEAN);
		theKeyProp2.addQualifier(keyQual);
		cc2.addProperty(theKeyProp2);
		hdl->modifyClass("root/testsuite", cc2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_PARAMETER);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		CIMClass cc2("invalidTestSub2");
		hdl->modifyClass("root/testsuite", cc2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}

	// CIM_ERR_INVALID_SUPERCLASS
	try
	{
		CIMClass cc2(baseClass);
		cc2.setSuperClass("invalid");
		hdl->modifyClass("root/testsuite", cc2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_SUPERCLASS);
	}

	// CIM_ERR_CLASS_HAS_CHILDREN - Can't get OpenWBEM to produce this as of July 30, 2002
	// CIM_ERR_CLASS_HAS_INSTANCES - Can't get OpenWBEM to produce this as of July 30, 2002

	// ModifyInstance
	CIMInstance ci = baseClass.newInstance();
	ci.setProperty(theKeyProp);
	try
	{
		try
		{
			CIMObjectPath cop("root/testsuite", ci);
			hdl->deleteInstance("root/testsuite", cop);
		}
		catch (const CIMException& e)
		{
		}
		hdl->createInstance("root/testsuite", ci);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(0);
	}


	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->modifyInstance("badNamespace", ci);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		CIMInstance ci2(ci);
		ci2.setClassName("fooBad");
		hdl->modifyInstance("root/testsuite", ci2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		CIMInstance ci2(ci);
		ci2.setProperty("theKeyProp", CIMValue(false));
		hdl->modifyInstance("root/testsuite", ci2);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}



	// EnumerateClasses
	
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumClassE("badNamespace", "foo");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		hdl->enumClassE("root/testsuite", "badClass");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}



	// EnumerateClassNames

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumClassNamesE("badNamespace", "foo");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		hdl->enumClassNamesE("root/testsuite", "badClass");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}



	// EnumerateInstances

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumInstancesE("badNamespace", "foo");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		hdl->enumInstancesE("root/testsuite", "badClass");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}


	// EnumerateInstanceNames

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumInstanceNamesE("badNamespace", "foo");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		hdl->enumInstanceNamesE("root/testsuite", "badClass");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}

	

	// ExecQuery

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->execQueryE("badNameSpace", "select * from junk", "wql1");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED
	try
	{
		hdl->execQueryE("root/testsuite", "select * from junk", "badql");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::QUERY_LANGUAGE_NOT_SUPPORTED);
	}

	// CIM_ERR_INVALID_QUERY
	try
	{
		hdl->execQueryE("root/testsuite", "xxx", "wql1");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_QUERY);
	}

	try
	{
		hdl->execQueryE("root/testsuite", "select * from junk", "wql1");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_QUERY);
	}


	// Associators

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMObjectPath cop("foo");
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->associatorsE("badNamespace", cop,"","","","");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	
	
	// AssociatorNames

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMObjectPath cop("foo");
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->associatorNamesE("badNamespace", cop, "", "", "", "");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER
	

	// References
	
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMObjectPath cop("foo");
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->referencesE("badNamespace", cop,"","");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER

	
	// ReferenceNames
	
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMObjectPath cop("foo");
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->referenceNamesE("badNamespace", cop,"","");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER

	// GetProperty

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMObjectPath cop("foo");
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->getProperty("badNamespace", cop, "theKeyProp");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	
	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		CIMObjectPath cop("badClass");
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->getProperty("root/testsuite", cop, "theKeyProp");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		CIMObjectPath cop(baseClass.getName());
		cop.setKeyValue("theKeyProp", CIMValue(false));
		hdl->getProperty("root/testsuite", cop, "theKeyProp");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}
	
	// CIM_ERR_NO_SUCH_PROPERTY
	try
	{
		CIMObjectPath cop(baseClass.getName());
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->getProperty("root/testsuite", cop, "badProp");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NO_SUCH_PROPERTY);
	}
	


	// SetProperty

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMObjectPath cop("foo");
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->setProperty("badNamespace", cop, "theKeyProp", CIMValue(true));
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	
	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		CIMObjectPath cop("badClass");
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->setProperty("root/testsuite", cop, "theKeyProp", CIMValue(true));
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		CIMObjectPath cop(baseClass.getName());
		cop.setKeyValue("theKeyProp", CIMValue(false));
		hdl->setProperty("root/testsuite", cop, "theKeyProp", CIMValue(false));
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}
	
	// CIM_ERR_NO_SUCH_PROPERTY
	try
	{
		CIMObjectPath cop(baseClass.getName());
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->setProperty("root/testsuite", cop, "badProp", CIMValue(true));
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NO_SUCH_PROPERTY);
	}
	
	// CIM_ERR_TYPE_MISMATCH
	try
	{
		CIMObjectPath cop(baseClass.getName());
		cop.setKeyValue("theKeyProp", CIMValue(true));
		hdl->setProperty("root/testsuite", cop, "theKeyProp", CIMValue(String("x")));
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::TYPE_MISMATCH);
	}
	


	// GetQualifier

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->getQualifierType("badNamespace", CIMQualifier::CIM_QUAL_ABSTRACT);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_NOT_FOUND
	try
	{
		hdl->getQualifierType("root/testsuite", "badQualifierType");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}


	// SetQualifier

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMQualifierType cqt("fooqt");
		cqt.setDataType(CIMDataType::BOOLEAN);
		cqt.addScope(CIMScope::ANY);
		hdl->setQualifierType("badNamespace", cqt);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER


	// DeleteQualifier

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->deleteQualifierType("badNamespace", CIMQualifier::CIM_QUAL_ABSTRACT);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_NOT_FOUND
	try
	{
		hdl->deleteQualifierType("root/testsuite", "badQualifierType");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}


	// EnumerateQualifiers

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumQualifierTypesE("badNamespace");
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER


	// CreateNamespace
	// DeleteNamespace
	// EnumNameSpaces


	// invokeMethod

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		CIMParamValueArray in, out;
		hdl->invokeMethod("badNamespace", CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - wrong number of params
	try
	{
		CIMParamValueArray in, out;
		hdl->invokeMethod("root/testsuite", CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_PARAMETER);
	}

	// CIM_ERR_INVALID_PARAMETER - wrong name of some parameter
	try
	{
		CIMParamValueArray in, out;
		in.push_back(CIMParamValue("io16", CIMValue(Int16(16))));
		in.push_back(CIMParamValue("nullParam", CIMValue(CIMNULL)));
		in.push_back(CIMParamValue("s", CIMValue(String("input string"))));
		UInt8Array uint8array;
		in.push_back(CIMParamValue("uint8array", CIMValue(uint8array)));
		CIMObjectPathArray paths;
		in.push_back(CIMParamValue("pathsBAD", CIMValue(paths)));

		hdl->invokeMethod("root/testsuite", CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_PARAMETER);
	}

	// CIM_ERR_INVALID_PARAMETER - wrong type of some parameter
	try
	{
		CIMParamValueArray in, out;
		in.push_back(CIMParamValue("io16", CIMValue(Int16(16))));
		in.push_back(CIMParamValue("nullParam", CIMValue(CIMNULL)));
		in.push_back(CIMParamValue("s", CIMValue(String("input string"))));
		StringArray sarray;
		sarray.push_back("x");
		in.push_back(CIMParamValue("uint8array", CIMValue(sarray)));
		CIMObjectPathArray paths;
		in.push_back(CIMParamValue("paths", CIMValue(paths)));

		hdl->invokeMethod("root/testsuite", CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_PARAMETER);
	}

	// CIM_ERR_INVALID_PARAMETER - duplicate parameter
	try
	{
		CIMParamValueArray in, out;
		in.push_back(CIMParamValue("io16", CIMValue(Int16(16))));
		in.push_back(CIMParamValue("nullParam", CIMValue(CIMNULL)));
		in.push_back(CIMParamValue("s", CIMValue(String("input string"))));
		UInt8Array uint8array;
		in.push_back(CIMParamValue("uint8array", CIMValue(uint8array)));
		CIMObjectPathArray paths;
		in.push_back(CIMParamValue("uint8array", CIMValue(paths)));

		hdl->invokeMethod("root/testsuite", CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::INVALID_PARAMETER);
	}

	// CIM_ERR_NOT_FOUND - no class
	try
	{
		CIMParamValueArray in, out;
		hdl->invokeMethod("root/testsuite", CIMObjectPath("BADEXP_BartComputerSystem"), "getstate", in, out);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}

	// CIM_ERR_NOT_FOUND - no instance
	try
	{
		CIMParamValueArray in, out;
		CIMObjectPath cop("EXP_BartComputerSystem");
		cop.setKeyValue("Name", CIMValue("badKey"));
		cop.setKeyValue("CreationClassName", CIMValue("badKey"));
		hdl->invokeMethod("root/testsuite", cop, "getstate", in, out);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::NOT_FOUND);
	}

	// CIM_ERR_METHOD_NOT_FOUND
	try
	{
		CIMParamValueArray in, out;
		CIMObjectPath cop("EXP_BartComputerSystem");
		hdl->invokeMethod("root/testsuite", cop, "BADgetstate", in, out);
		TEST_ASSERT(0);
	}
	catch (const CIMException& e)
	{
		TEST_ASSERT(e.getErrNo() == CIMException::METHOD_NOT_FOUND);
	}


	// cleanup

	hdl->deleteClass("root/testsuite", baseClass.getName());


}

