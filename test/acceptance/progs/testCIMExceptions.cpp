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
		hdl->getClass(OW_CIMObjectPath("foo", "badNamespace"));
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
		hdl->getClass(OW_CIMObjectPath("fooXXX", "root"));
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
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.addKey("fooKey", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->getInstance(cop);
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
		OW_CIMObjectPath cop("fooXXX", "root");
		cop.addKey("fooKey", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->getInstance(cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		OW_CIMObjectPath cop("CIM_PhysicalElement", "root");
		cop.addKey("CreationClassName", OW_CIMValue(OW_String("fooKeyValue")));
		cop.addKey("Tag", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->getInstance(cop);
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
		OW_CIMObjectPath cop("foo", "badNamespace");
		hdl->deleteClass(cop);
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
		hdl->deleteClass(OW_CIMObjectPath("fooXXX", "root"));
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
		OW_CIMObjectPath cop("foo", "badNamespace");
		cop.addKey("fooKey", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->deleteInstance(cop);
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
		OW_CIMObjectPath cop("fooXXX", "root");
		cop.addKey("fooKey", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->deleteInstance(cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_CLASS);
	}

	// CIM_ERR_NOT_FOUND
	try
	{
		OW_CIMObjectPath cop("CIM_PhysicalElement", "root");
		cop.addKey("CreationClassName", OW_CIMValue(OW_String("fooKeyValue")));
		cop.addKey("Tag", OW_CIMValue(OW_String("fooKeyValue")));
		hdl->deleteInstance(cop);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::NOT_FOUND);
	}


	// CreateClass

	OW_CIMClass cc("footest");
	// CIM_ERR_INVALID_NAMESPACE
	try
	{
		OW_CIMObjectPath cop("foo", "badNamespace");
		hdl->createClass(cop, cc);
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
	OW_CIMQualifierType assocQualType = hdl->getQualifierType(OW_CIMObjectPath(OW_CIMQualifier::CIM_QUAL_ASSOCIATION, "root"));
	OW_CIMQualifier assocQual(assocQualType);
	assocQual.setValue(OW_CIMValue(true));
	baseClass.addQualifier(assocQual);
	try
	{
		hdl->createClass(OW_CIMObjectPath(baseClass.getName(), "root"), baseClass);
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
		OW_CIMClass cc2("invalidTestSub");
		cc2.setSuperClass("invalidTestBase");
		OW_CIMQualifier assocQual2(assocQual);
		assocQual2.setValue(OW_CIMValue(false));
		cc2.addQualifier(assocQual2);
		OW_CIMObjectPath cop(cc2.getName(), "root");
		hdl->createClass(cop, cc2);
		assert(0);
	}
	catch (const OW_CIMException& e)
	{
		assert(e.getErrNo() == OW_CIMException::INVALID_PARAMETER);
	}

	hdl->deleteClass(OW_CIMObjectPath(baseClass.getName(), "root"));

	// CIM_ERR_ALREADY_EXISTS
	// CIM_ERR_INVALID_SUPERCLASS

}

