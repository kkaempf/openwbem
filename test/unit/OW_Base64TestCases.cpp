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

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_Base64TestCases.hpp"
#include "OW_HTTPUtils.hpp"

using namespace OpenWBEM;

void OW_Base64TestCases::setUp()
{
}

void OW_Base64TestCases::tearDown()
{
}

void OW_Base64TestCases::testSomething()
{
	String info("Aladdin:open sesame");
	String encoded = HTTPUtils::base64Encode(info);
	unitAssert(encoded.equals("QWxhZGRpbjpvcGVuIHNlc2FtZQ=="));
	unitAssert(HTTPUtils::base64Decode(encoded).equals(info));
	encoded = "Og==";
	info = HTTPUtils::base64Decode(encoded);
	unitAssert(info.equals(":"));
	unitAssert(HTTPUtils::base64Encode(info).equals(encoded));
}

Test* OW_Base64TestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Base64");

	testSuite->addTest (new TestCaller <OW_Base64TestCases> 
			("testSomething", 
			&OW_Base64TestCases::testSomething));

	return testSuite;
}

