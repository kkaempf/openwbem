/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_CIMException.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"

#include <iostream>
#include <algorithm> // for std::find

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::find;
using namespace OW_WBEMFlags;

// create our own TEST_ASSERT, because assert and OW_ASSERT will be turned off
// in non-debug mode.
#define TEST_ASSERT(CON) if(!(CON)) throw OW_AssertionException(__FILE__, __LINE__, #CON)

static const OW_String ns("root/testsuite");

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
}

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
// The point of this test is to verify that we can set a qualifier value on
// an instance that is specific to the instance, or different than what is
// specified on the class.
void testInstanceLevelQualifier(OW_CIMOMHandleIFCRef& chRef)
{
	const OW_String cName = "testC1";
	OW_CIMQualifier max = 
		OW_CIMQualifier(
			chRef->getQualifierType(ns, "MaxValue")
		).setValue(
			OW_CIMValue(OW_UInt64(100))
		);

	OW_CIMClass c = 
		OW_CIMClass(cName).addProperty(
			OW_CIMProperty("p", OW_CIMValue(OW_Int32(10))).addQualifier(
				max
			)
		);
	chRef->createClass(ns, c);
	c = chRef->getClass(ns, cName);
	OW_CIMInstance i = c.newInstance();
	OW_CIMProperty p = i.getProperty("p");
	max.setValue(OW_CIMValue(OW_UInt64(50)));
	p.setQualifier(max);
	i.setProperty(p);
	chRef->createInstance(ns,i);
	i = chRef->getInstance(ns, OW_CIMObjectPath(ns, i), 
		E_NOT_LOCAL_ONLY,
		E_INCLUDE_QUALIFIERS);

	p = i.getPropertyT("p");
	OW_CIMQualifier q = p.getQualifier("MaxValue");
	TEST_ASSERT(q);
	TEST_ASSERT(q.getValue());
	TEST_ASSERT(q.getValue().getType() == OW_CIMDataType::UINT64);
	TEST_ASSERT(q.getValue().toUInt64() == 50);


}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

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

		OW_CIMProtocolIFCRef client(new OW_HTTPClient(url));

		OW_CIMOMHandleIFCRef chRef;
		if (owurl.path.equalsIgnoreCase("/owbinary"))
		{
			chRef = new OW_BinaryCIMOMHandle(client);
		}
		else
		{
			chRef = new OW_CIMXMLCIMOMHandle(client);
		}

		// This doesn't work yet. testInstanceLevelQualifier(chRef);

		return 0;

	}
	catch (OW_Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}

