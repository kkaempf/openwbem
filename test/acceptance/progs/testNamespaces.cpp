/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_Assertion.hpp"

#include <iostream>
#include <algorithm> // for std::find

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::find;
using namespace OpenWBEM;
using namespace WBEMFlags;


// create our own TEST_ASSERT, because assert and ASSERT will be turned off
// in non-debug mode.
#define TEST_ASSERT(CON) if(!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
}


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

		CIMProtocolIFCRef client(new HTTPClient(url));

		CIMOMHandleIFCRef chRef;
		if (owurl.scheme.startsWith(URL::OWBINARY))
		{
			chRef = new BinaryCIMOMHandle(client);
		}
		else
		{
			chRef = new CIMXMLCIMOMHandle(client);
		}

		StringArray namespaces;

		// first check for the existence of root
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_SHALLOW);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root")) != namespaces.end());
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_DEEP);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root")) != namespaces.end());

		CIMNameSpaceUtils::create__Namespace(*chRef,"root/testnamespace");
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_SHALLOW);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace")) != namespaces.end());
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_DEEP);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace")) != namespaces.end());
		
		CIMNameSpaceUtils::create__Namespace(*chRef,"root/testnamespace/deep");
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_SHALLOW);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_DEEP);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep")) != namespaces.end());
		
		CIMNameSpaceUtils::create__Namespace(*chRef,"root/testnamespace/deep/deep2");
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_SHALLOW);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep/deep2")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_DEEP);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep/deep2")) != namespaces.end());

		// __Namespace automatically deletes sub-namespaces, make sure deep and deep2 are gone
		CIMNameSpaceUtils::delete__Namespace(*chRef,"root/testnamespace/deep");
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_SHALLOW);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_DEEP);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_SHALLOW);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep/deep2")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_DEEP);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep/deep2")) == namespaces.end());
		
		CIMNameSpaceUtils::delete__Namespace(*chRef,"root/testnamespace");
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_SHALLOW);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enum__Namespace(*chRef, "root", E_DEEP);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace")) == namespaces.end());

		// Now test CIM_Namespace
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root")) != namespaces.end());

		CIMNameSpaceUtils::createCIM_Namespace(*chRef,"root/testnamespace");
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace")) != namespaces.end());
		
		CIMNameSpaceUtils::createCIM_Namespace(*chRef,"root/testnamespace/deep");
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep")) != namespaces.end());
		
		CIMNameSpaceUtils::createCIM_Namespace(*chRef,"root/testnamespace/deep/deep2");
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep/deep2")) != namespaces.end());

		// CIM_Namespace doesn't delete sub-namespaces, make sure deep is gone and deep2 is still there.
		CIMNameSpaceUtils::deleteCIM_Namespace(*chRef,"root/testnamespace/deep");
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep/deep2")) != namespaces.end());
		
		CIMNameSpaceUtils::deleteCIM_Namespace(*chRef,"root/testnamespace");
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep/deep2")) != namespaces.end());

		CIMNameSpaceUtils::deleteCIM_Namespace(*chRef,"root/testnamespace/deep/deep2");
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace")) == namespaces.end());
		namespaces = CIMNameSpaceUtils::enumCIM_Namespace(*chRef);
		TEST_ASSERT(find(namespaces.begin(), namespaces.end(), String("root/testnamespace/deep/deep2")) == namespaces.end());


		return 0;

	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}

