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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_CIMUrlTestCases.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_String.hpp"
#include "OW_URL.hpp"

void OW_CIMUrlTestCases::setUp()
{
}

void OW_CIMUrlTestCases::tearDown()
{
}

void OW_CIMUrlTestCases::testToString()
{
	/* TODO: Finish this
	OW_CIMUrl url(OW_String("http://test2:pass2@localhost:30926"));
	unitAssert( url.getFile() == "cimom" );
	unitAssert( url.getHost() == "localhost" );
	unitAssert( url.getPort() == 30926 );
	unitAssert( url.getProtocol() == "http" );
	unitAssert( url.getRef() == "" );
	unitAssert( url.getSpec() == "" );
	unitAssert( url.toString() == "http://test2:pass2@localhost:30926" );
	*/
	OW_String surl = "proto://user:pass@hostname.foo.com:1234/my/path";
	OW_URL url(surl);
	unitAssert(url.host.equals("hostname.foo.com"));
	unitAssert(url.password.equals("pass"));
	unitAssert(url.path.equals("/my/path"));
	unitAssert(url.port == 1234);
	unitAssert(url.protocol.equals("proto"));
	unitAssert(url.username.equals("user"));
	unitAssert(url.toString().equals(surl));
}

Test* OW_CIMUrlTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_CIMUrl");

	testSuite->addTest (new TestCaller <OW_CIMUrlTestCases>
			("testToString",
			&OW_CIMUrlTestCases::testToString));

	return testSuite;
}

