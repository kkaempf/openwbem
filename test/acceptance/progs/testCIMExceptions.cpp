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
#include "OW_SocketBaseImpl.hpp"
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
#include "OW_CIMMethod.hpp"
#include "OW_Format.hpp"
#include "OW_GetPass.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMParamValue.hpp"

#include <iostream>


using std::cerr;
using std::cin;
using std::cout;
using std::endl;

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

void runTests(const OW_CIMOMHandleIFCRef& hdl);

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
		OW_CIMProtocolIFCRef client;
		client = new OW_HTTPClient(url);
		OW_ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
		client->setLoginCallBack(getLoginInfo);
		OW_CIMOMHandleIFCRef chRef;
		if (owurl.path.equalsIgnoreCase("/owbinary"))
		{
			chRef = new OW_BinaryCIMOMHandle(client);
		}
		else
		{
			chRef = new OW_CIMXMLCIMOMHandle(client);
		}

        runTests(chRef);

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

void runTests(const OW_CIMOMHandleIFCRef& hdl)
{
    // GetClass
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->getClass("badNamespace", "foo");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - Can't do without doing straight XML.  Handle it in a separate test.

	// CIM_ERR_NOT_FOUND
	try
	{
		hdl->getClass("root/testsuite", "fooXXX");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}


	// GetInstance

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo");
		cop.addKey("fooKey", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->getInstance("badNamespace", cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - Can't do without doing straight XML.  Handle it in a separate test.

	// CIM_ERR_INVALID_CLASS
	try
	{
		OW_CIMObjectPath cop("fooXXX");
		cop.addKey("fooKey", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->getInstance("root/testsuite", cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		OW_CIMObjectPath cop("CIM_PhysicalElement");
		cop.addKey("CreationClassName", OW_CIMValue(OW_String("fooKeyValue")));
		cop.addKey("Tag", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->getInstance("root/testsuite", cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}


	// DeleteClass
	
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->deleteClass("badNamespace", "foo");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}


	// CIM_ERR_INVALID_PARAMETER - Can only be done with doctored XML

	// CIM_ERR_NOT_FOUND
	try
	{
		hdl->deleteClass("root/testsuite", "fooXXX");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}


	// CIM_ERR_CLASS_HAS_CHILDREN - Impossible to produce with OpenWBEM

	// CIM_ERR_CLASS_HAS_INSTANCES - Impossible to produce with OpenWBEM


	// DeleteInstance

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo");
		cop.addKey("fooKey", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->deleteInstance("badNamespace", cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - Can only be done with doctored XML

	// CIM_ERR_INVALID_CLASS
	try
	{
		OW_CIMObjectPath cop("fooXXX");
		cop.addKey("fooKey", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->deleteInstance("root/testsuite", cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		OW_CIMObjectPath cop("CIM_PhysicalElement");
		cop.addKey("CreationClassName", OW_CIMValue(OW_String("fooKeyValue")));
		cop.addKey("Tag", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->deleteInstance("root/testsuite", cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}


	// CreateClass

	OW_CIMClass cc("footest");
	OW_CIMQualifierType keyQualType = hdl->getQualifierType("root/testsuite",
		OW_CIMQualifier::CIM_QUAL_KEY);
	OW_CIMQualifier keyQual(keyQualType);
	keyQual.setValue(OW_CIMValue(true));
	OW_CIMProperty theKeyProp("theKeyProp", OW_CIMDataType(OW_CIMDataType::BOOLEAN));
	theKeyProp.addQualifier(keyQual);
	theKeyProp.setValue(OW_CIMValue(true));

	cc.addProperty(theKeyProp);

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->createClass("badNamespace", cc);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER - thrown from OW_MetaRepository::_throwIfBadClass() and OW_MetaRepository::adjustClass()

	// There are different ways to get this error.  Let's try all of them.
	// 1. A subclass overrides a qualifier that has the DISABLEOVERRIDE flavor
	// on the base class
	
	// create a base class that has the associator qualifier, which can't be overridden
	OW_CIMClass baseClass("invalidTestBase");
	OW_CIMQualifierType assocQualType = hdl->getQualifierType("root/testsuite",
		OW_CIMQualifier::CIM_QUAL_ASSOCIATION);
	OW_CIMQualifier assocQual(assocQualType);
	assocQual.setValue(OW_CIMValue(true));
	baseClass.addProperty(theKeyProp);
	baseClass.addQualifier(assocQual);

	try
	{
		try
		{
			hdl->deleteClass("root/testsuite", baseClass.getName());
		}
		catch (const OW_CIMException&)
		{
		}
		hdl->createClass("root/testsuite", baseClass);
	}
	catch (const OW_CIMException& e)
	{
		if (e.getErrNo() != OW_CIMException::ALREADY_EXISTS)
		{
			throw e;
		}
	}

	try
	{
		OW_CIMInstance ci = baseClass.newInstance();
		ci.setProperty(theKeyProp);
		OW_CIMObjectPath cop("foo", "root/testsuite");
		cop.setKeys(ci);
		hdl->createInstance(cop, ci);
	}
	catch (const OW_CIMException& e)
	{
		assert(0);
	}



	// setup's done, now do the tests
	try
	{
		// test overriding an DISABLEOVERRIDE qualifier
		OW_CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		OW_CIMQualifier assocQual2(assocQual);
		assocQual2.setValue(OW_CIMValue(false));
		cc2.addQualifier(assocQual2);
		hdl->createClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
	}

	try
	{
		// test adding an key to a subclass when the parent already has keys.
		OW_CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		OW_CIMProperty theKeyProp2("theKeyProp2", OW_CIMDataType(OW_CIMDataType::BOOLEAN));
		theKeyProp2.addQualifier(keyQual);
		cc2.addProperty(theKeyProp2);
		hdl->createClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
	}

	try
	{
		// test adding a class with no keys
		OW_CIMClass cc2("invalidTestSub");
		hdl->createClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
	}

	// CIM_ERR_ALREADY_EXISTS
	try
	{
		// test adding a class with no keys
		hdl->createClass("root/testsuite", baseClass);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::ALREADY_EXISTS);
	}

	// CIM_ERR_INVALID_SUPERCLASS
	try
	{
		OW_CIMClass cc2(baseClass);
		cc2.setSuperClass("invalid");
		hdl->createClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_SUPERCLASS);
	}


	// CreateInstance

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMInstance ci = baseClass.newInstance();
		ci.setProperty(theKeyProp);
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.setKeys(ci);
		hdl->createInstance(cop, ci);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER

	// CIM_ERR_INVALID_CLASS
	try
	{
		OW_CIMInstance ci = baseClass.newInstance();
		ci.setClassName("nonexistentClass");
		ci.setProperty(theKeyProp);
		OW_CIMObjectPath cop("foo", "root/testsuite");
		cop.setKeys(ci);
		hdl->createInstance(cop, ci);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	// CIM_ERR_ALREADY_EXISTS
	try
	{
		OW_CIMInstance ci = baseClass.newInstance();
		ci.setProperty(theKeyProp);
		OW_CIMObjectPath cop("foo", "root/testsuite");
		cop.setKeys(ci);
		hdl->createInstance(cop, ci);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::ALREADY_EXISTS);
	}


	// ModifyClass

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->modifyClass("badNamespace", cc);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// first create a class to modify
	try
	{
		OW_CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		OW_CIMObjectPath cop(cc2.getName(), "root/testsuite");
		hdl->createClass("root/testsuite", cc2);
	}
	catch (const OW_CIMException& e)
	{
		assert(0);
	}
	
	try
	{
		// test overriding an DISABLEOVERRIDE qualifier
		OW_CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		OW_CIMQualifier assocQual2(assocQual);
		assocQual2.setValue(OW_CIMValue(false));
		cc2.addQualifier(assocQual2);
		hdl->modifyClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
	}

	try
	{
		// test adding an key to a subclass when the parent already has keys.
		OW_CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		OW_CIMProperty theKeyProp2("theKeyProp2", OW_CIMDataType(OW_CIMDataType::BOOLEAN));
		theKeyProp2.addQualifier(keyQual);
		cc2.addProperty(theKeyProp2);
		hdl->modifyClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
	}

	try
	{
		// test adding a class with no keys
		OW_CIMClass cc2("invalidTestSub");
		hdl->modifyClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		OW_CIMClass cc2("invalidTestSub2");
		hdl->modifyClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}

	// CIM_ERR_INVALID_SUPERCLASS
	try
	{
		OW_CIMClass cc2(baseClass);
		cc2.setSuperClass("invalid");
		hdl->modifyClass("root/testsuite", cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_SUPERCLASS);
	}

	// CIM_ERR_CLASS_HAS_CHILDREN - Can't get OpenWBEM to produce this as of July 30, 2002
	// CIM_ERR_CLASS_HAS_INSTANCES - Can't get OpenWBEM to produce this as of July 30, 2002


	// ModifyInstance
	OW_CIMInstance ci = baseClass.newInstance();
	ci.setProperty(theKeyProp);
	try
	{
		OW_CIMObjectPath cop(baseClass.getName(), "root/testsuite");
		cop.setKeys(ci);
		try
		{
			hdl->deleteInstance("root/testsuite", cop);
		}
		catch (const OW_CIMException& e)
		{
		}
		hdl->createInstance(cop, ci);
	}
	catch (const OW_CIMException& e)
	{
		assert(0);
	}


	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->modifyInstance("badNamespace", ci);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		OW_CIMInstance ci2(ci);
		ci2.setClassName("fooBad");
		hdl->modifyInstance("root/testsuite", ci2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		OW_CIMInstance ci2(ci);
		ci2.setProperty("theKeyProp", OW_CIMValue(false));
		hdl->modifyInstance("root/testsuite", ci2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}



	// EnumerateClasses
	
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumClassE("badNamespace", "foo");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		hdl->enumClassE("root/testsuite", "badClass");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}



	// EnumerateClassNames

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumClassNamesE("badNamespace", "foo");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		hdl->enumClassNamesE("root/testsuite", "badClass");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}



	// EnumerateInstances

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumInstancesE("badNamespace", "foo");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		hdl->enumInstancesE("root/testsuite", "badClass");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}


	// EnumerateInstanceNames

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumInstanceNamesE("badNamespace", "foo");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		hdl->enumInstanceNamesE("root/testsuite", "badClass");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	

	// ExecQuery

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->execQueryE(OW_CIMNameSpace("badNameSpace"), "select * from junk", "wql1");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED
	try
	{
		hdl->execQueryE(OW_CIMNameSpace("root/testsuite"), "select * from junk", "badql");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::QUERY_LANGUAGE_NOT_SUPPORTED);
	}

	// CIM_ERR_INVALID_QUERY
	try
	{
		hdl->execQueryE(OW_CIMNameSpace("root/testsuite"), "xxx", "wql1");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_QUERY);
	}

	try
	{
		hdl->execQueryE(OW_CIMNameSpace("root/testsuite"), "select * from junk", "wql1");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_QUERY);
	}


	// Associators

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->associatorsE(cop,"","","","");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}

	// CIM_ERR_INVALID_PARAMETER
	
	
	// AssociatorNames

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->associatorNamesE(cop, "", "", "", "");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER
	

	// References
	
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->referencesE(cop,"","");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER

	
	// ReferenceNames
	
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->referenceNamesE(cop,"","");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER


	// GetProperty

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->getProperty(cop, "theKeyProp");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	
	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		OW_CIMObjectPath cop("badClass", "root/testsuite");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->getProperty(cop, "theKeyProp");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		OW_CIMObjectPath cop(baseClass.getName(), "root/testsuite");
		cop.addKey("theKeyProp", OW_CIMValue(false));
		hdl->getProperty(cop, "theKeyProp");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}
	
	// CIM_ERR_NO_SUCH_PROPERTY
	try
	{
		OW_CIMObjectPath cop(baseClass.getName(), "root/testsuite");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->getProperty(cop, "badProp");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NO_SUCH_PROPERTY);
	}
	


	// SetProperty

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->setProperty(cop, "theKeyProp", OW_CIMValue(true));
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	
	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_INVALID_CLASS
	try
	{
		OW_CIMObjectPath cop("badClass", "root/testsuite");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->setProperty(cop, "theKeyProp", OW_CIMValue(true));
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		OW_CIMObjectPath cop(baseClass.getName(), "root/testsuite");
		cop.addKey("theKeyProp", OW_CIMValue(false));
		hdl->setProperty(cop, "theKeyProp", OW_CIMValue(false));
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}
	
	// CIM_ERR_NO_SUCH_PROPERTY
	try
	{
		OW_CIMObjectPath cop(baseClass.getName(), "root/testsuite");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->setProperty(cop, "badProp", OW_CIMValue(true));
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NO_SUCH_PROPERTY);
	}
	
	// CIM_ERR_TYPE_MISMATCH
	try
	{
		OW_CIMObjectPath cop(baseClass.getName(), "root/testsuite");
		cop.addKey("theKeyProp", OW_CIMValue(true));
		hdl->setProperty(cop, "theKeyProp", OW_CIMValue(OW_String("x")));
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::TYPE_MISMATCH);
	}
	


	// GetQualifier

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->getQualifierType("badNamespace", OW_CIMQualifier::CIM_QUAL_ABSTRACT);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_NOT_FOUND
	try
	{
		hdl->getQualifierType("root/testsuite", "badQualifierType");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}


	// SetQualifier

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMQualifierType cqt("fooqt");
		cqt.setDataType(OW_CIMDataType::BOOLEAN);
		cqt.addScope(OW_CIMScope::ANY);
		hdl->setQualifierType("badNamespace", cqt);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER


	// DeleteQualifier

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->deleteQualifierType("badNamespace", OW_CIMQualifier::CIM_QUAL_ABSTRACT);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER
	// CIM_ERR_NOT_FOUND
	try
	{
		hdl->deleteQualifierType("root/testsuite", "badQualifierType");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}


	// EnumerateQualifiers

	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		hdl->enumQualifierTypesE("badNamespace");
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
	}
	// CIM_ERR_INVALID_PARAMETER


	// CreateNamespace
	// DeleteNamespace
	// EnumNameSpaces


    // invokeMethod

    // CIM_ERR_INVALID_NAMESPACE
    try
    {
        OW_CIMParamValueArray in, out;
        hdl->invokeMethod("badNamespace", OW_CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		assert(0);
    }
    catch (const OW_CIMException& e)
    {
        assert(e.getErrNo() == OW_CIMException::INVALID_NAMESPACE);
    }

    // CIM_ERR_INVALID_PARAMETER - wrong number of params
    try
    {
        OW_CIMParamValueArray in, out;
        hdl->invokeMethod("root/testsuite", OW_CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		assert(0);
    }
    catch (const OW_CIMException& e)
    {
        assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
    }

    // CIM_ERR_INVALID_PARAMETER - wrong name of some parameter
    try
    {
        OW_CIMParamValueArray in, out;
        in.push_back(OW_CIMParamValue("io16", OW_CIMValue(OW_Int16(16))));
        in.push_back(OW_CIMParamValue("nullParam", OW_CIMValue()));
        in.push_back(OW_CIMParamValue("s", OW_CIMValue(OW_String("input string"))));
        OW_UInt8Array uint8array;
        in.push_back(OW_CIMParamValue("uint8array", OW_CIMValue(uint8array)));
        OW_CIMObjectPathArray paths;
        in.push_back(OW_CIMParamValue("pathsBAD", OW_CIMValue(paths)));

        hdl->invokeMethod("root/testsuite", OW_CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		assert(0);
    }
    catch (const OW_CIMException& e)
    {
        assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
    }

    // CIM_ERR_INVALID_PARAMETER - wrong type of some parameter
    try
    {
        OW_CIMParamValueArray in, out;
        in.push_back(OW_CIMParamValue("io16", OW_CIMValue(OW_Int16(16))));
        in.push_back(OW_CIMParamValue("nullParam", OW_CIMValue()));
        in.push_back(OW_CIMParamValue("s", OW_CIMValue(OW_String("input string"))));
        OW_StringArray sarray;
        sarray.push_back("x");
        in.push_back(OW_CIMParamValue("uint8array", OW_CIMValue(sarray)));
        OW_CIMObjectPathArray paths;
        in.push_back(OW_CIMParamValue("paths", OW_CIMValue(paths)));

        hdl->invokeMethod("root/testsuite", OW_CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		assert(0);
    }
    catch (const OW_CIMException& e)
    {
        assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
    }

    // CIM_ERR_INVALID_PARAMETER - duplicate parameter
    try
    {
        OW_CIMParamValueArray in, out;
        in.push_back(OW_CIMParamValue("io16", OW_CIMValue(OW_Int16(16))));
        in.push_back(OW_CIMParamValue("nullParam", OW_CIMValue()));
        in.push_back(OW_CIMParamValue("s", OW_CIMValue(OW_String("input string"))));
        OW_UInt8Array uint8array;
        in.push_back(OW_CIMParamValue("uint8array", OW_CIMValue(uint8array)));
        OW_CIMObjectPathArray paths;
        in.push_back(OW_CIMParamValue("uint8array", OW_CIMValue(paths)));

        hdl->invokeMethod("root/testsuite", OW_CIMObjectPath("EXP_BartComputerSystem"), "getstate", in, out);
		assert(0);
    }
    catch (const OW_CIMException& e)
    {
        assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
    }

    // CIM_ERR_NOT_FOUND - no class
    try
    {
        OW_CIMParamValueArray in, out;
        hdl->invokeMethod("root/testsuite", OW_CIMObjectPath("BADEXP_BartComputerSystem"), "getstate", in, out);
		assert(0);
    }
    catch (const OW_CIMException& e)
    {
        assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
    }

    // CIM_ERR_NOT_FOUND - no instance
    try
    {
        OW_CIMParamValueArray in, out;
        OW_CIMObjectPath cop("EXP_BartComputerSystem");
        cop.addKey("Name", OW_CIMValue("badKey"));
        cop.addKey("CreationClassName", OW_CIMValue("badKey"));
        hdl->invokeMethod("root/testsuite", cop, "getstate", in, out);
		assert(0);
    }
    catch (const OW_CIMException& e)
    {
        assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
    }

    // CIM_ERR_METHOD_NOT_FOUND
    try
    {
        OW_CIMParamValueArray in, out;
        OW_CIMObjectPath cop("EXP_BartComputerSystem");
        hdl->invokeMethod("root/testsuite", cop, "BADgetstate", in, out);
		assert(0);
    }
    catch (const OW_CIMException& e)
    {
        assert(e.getErrNo() == OW_CIMException::METHOD_NOT_FOUND);
    }


	// cleanup

	hdl->deleteClass("root/testsuite", baseClass.getName());


}

