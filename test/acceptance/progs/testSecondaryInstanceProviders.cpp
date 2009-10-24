/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Assertion.hpp"
#include "blocxx/SocketBaseImpl.hpp"

#include <iostream>
#include <algorithm>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::find;
using namespace OpenWBEM;
using namespace WBEMFlags;


// create our own TEST_ASSERT, because assert and ASSERT will be turned off
// in non-debug mode.
#define TEST_ASSERT(CON) if (!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
}

const char* const testNS = "root/secondaryinstanceprovidertest";
const char* const SecondaryInstanceTest = "SecondaryInstanceTest";

void checkEnum(const CIMOMHandleIFCRef& chRef, size_t size)
{
	CIMInstanceArray insts = chRef->enumInstancesA(testNS, SecondaryInstanceTest);
	TEST_ASSERT(insts.size() == size);
	for (size_t i = 0; i < insts.size(); ++i)
	{
		TEST_ASSERT(insts[i].getPropertyValue("prop3").toString() == "from secondary prov 1");
		TEST_ASSERT(insts[i].getPropertyValue("prop4").toString() == "from secondary prov 2");
	}
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

		CIMOMHandleIFCRef chRef = ClientCIMOMHandle::createFromURL(url);

		checkEnum(chRef, 2);

		CIMObjectPath path(SecondaryInstanceTest, testNS);
		path.setKeyValue("key", CIMValue("1"));
		CIMInstance tempInst = chRef->getInstance(testNS, path, E_NOT_LOCAL_ONLY,
			E_INCLUDE_QUALIFIERS);
		tempInst.setProperty("prop2", CIMValue("temp"));
		chRef->modifyInstance(testNS, tempInst);

		checkEnum(chRef, 2);

		chRef->deleteInstance(testNS, CIMObjectPath(testNS, tempInst));

		checkEnum(chRef, 1);

		return 0;

	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}


