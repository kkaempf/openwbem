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
#include "OW_SocketBaseImpl.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_Format.hpp"
#include "OW_GetPass.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMClass.hpp"

#include <iostream>


using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using namespace OpenWBEM;

#define TEST_ASSERT(CON) if(!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)
//////////////////////////////////////////////////////////////////////////////

void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
}

//////////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			usage(argv[0]);
			exit(1);
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

		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
		CIMOMHandleIFCRef chRef = ClientCIMOMHandle::createFromURL(url, getLoginInfo);

		CIMOMHandleIFC& rch = *chRef;

		/**********************************************************************
		 * Now we have essentially established a "connection" to the CIM
		 * Server.  We can access the methods on the remote CIMOM handle,
		 * and these methods will call into the CIM Server.  The HTTPClient
		 * will take care of the particulars of the HTTP protocol, including
		 * authentication, compression, SSL, chunking, etc.
		 **********************************************************************/


		cout << "** Enumerating instances (0 instances)" << endl;
		CIMObjectPathEnumeration copEnu = rch.enumInstanceNamesE("root", "TestInstance");
		TEST_ASSERT(copEnu.numberOfElements() == 0);

		cout << "** Getting class" << endl;
		CIMClass cc = rch.getClass("root", "TestInstance");

		cout << "** Creating instance one" << endl;
		CIMInstance inst = cc.newInstance();
		inst.setProperty("Name", CIMValue(String("One")));
		StringArray sa;
		sa.push_back(String("One"));
		sa.push_back(String("Two"));
#if 1
		inst.setProperty("Params", CIMValue(sa));
#else
		inst.setProperty(CIMProperty(String("Params"), CIMValue(sa)));
#endif
		CIMObjectPath ccop("TestInstance", "root");
		CIMObjectPath icop = ccop;
		icop.setKeyValue("Name", CIMValue(String("One")));
		rch.createInstance("root", inst);

		cout << "** Enumerating instances (1 instance)" << endl;
		copEnu = rch.enumInstanceNamesE("root", "TestInstance");
		TEST_ASSERT(copEnu.numberOfElements() == 1);

		cout << "** Getting Instance" << endl;
		inst = rch.getInstance(icop.getNameSpace(), icop);
		sa.clear();
		cout << "** Checking array property on instance" << endl;
		inst.getProperty("Params").getValue().get(sa);
		TEST_ASSERT(sa.size() == 2);
		TEST_ASSERT(sa[0] == "One");
		TEST_ASSERT(sa[1] == "Two");

		cout << "** Modifying array property on instance" << endl;
		sa.push_back(String("Three"));

#if 1
		inst.setProperty("Params", CIMValue(sa));
#else
		inst.setProperty(CIMProperty(String("Params"), CIMValue(sa)));
#endif

		rch.modifyInstance("root", inst);

		cout << "** Getting new instance" << endl;
		inst = rch.getInstance(icop.getNameSpace(), icop);

		cout << "** Checking array property on new instance" << endl;
		sa.clear();
		inst.getProperty("Params").getValue().get(sa);
		TEST_ASSERT(sa.size() == 3);
		TEST_ASSERT(sa[0] == "One");
		TEST_ASSERT(sa[1] == "Two");
		TEST_ASSERT(sa[2] == "Three");

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



