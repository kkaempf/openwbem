/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_URLTestCases.hpp"
#include "OW_URL.hpp"

using namespace OpenWBEM;

void OW_URLTestCases::setUp()
{
}

void OW_URLTestCases::tearDown()
{
}

void OW_URLTestCases::testValidIPv4()
{
	unitAssertNoThrow( URL("localhost") );
	unitAssertNoThrow( URL("localhost:80") );
	unitAssertNoThrow( URL("http://localhost") );
	unitAssertNoThrow( URL("http://localhost:631") );
	unitAssertNoThrow( URL("https://localhost") );
	unitAssertNoThrow( URL("https://localhost:8080") );
	unitAssertNoThrow( URL("https://nobody:nothing@localhost:8080/foo/:ick") );
	unitAssertNoThrow( URL("https://nobody:nothing@127.0.0.1:8080/foo/:ick") );
}

#ifdef OW_HAVE_IPV6
void OW_URLTestCases::testValidIPv6()
{
	unitAssertNoThrow( URL("[::1]") );
	unitAssertNoThrow( URL("http://[::1]:80") );
	unitAssertNoThrow( URL("http://[::FFFF:127.0.0.1]:1234") );
}
#endif

void OW_URLTestCases::testInvalidIPv4()
{
	unitAssertThrows( URL("://:@:/:") );
	unitAssertThrows( URL("/") );
	unitAssertThrows( URL("/bob/") );
	//	unitAssertThrows( URL("://localhost") ); // Should this throw?
}

#ifdef OW_HAVE_IPV6
void OW_URLTestCases::testInvalidIPv6()
{
	// must be in '[' and ']' -- This turns out to be too many empty fields.
	// Not quite the reason we want it to fail, but good enough.
	unitAssertThrows( URL("::1") );
	unitAssertThrows( URL("[::1") );
	unitAssertThrows( URL("[]") );
}
#endif

void OW_URLTestCases::testAddressEquality()
{
	String testAddress1 = "http://localhost:80";
	unitAssertNoThrow(
		URL foo(testAddress1);
		unitAssertEquals("localhost", foo.host);
		unitAssertEquals("80", foo.port);
		unitAssert(!foo.ipv6Address);
		unitAssertEquals(testAddress1, foo.toString());
	);
#ifdef OW_HAVE_IPV6
	String testAddress2 = "http://[::1]:631";
	unitAssertNoThrow(
		URL foo(testAddress2);
		unitAssertEquals("::1", foo.host);
		unitAssertEquals("631", foo.port);
		unitAssert(foo.ipv6Address);
		unitAssertEquals(testAddress2, foo.toString());
	);
#endif
}

Test* OW_URLTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_URL");

	ADD_TEST_TO_SUITE(OW_URLTestCases,  testValidIPv4);
	ADD_TEST_TO_SUITE(OW_URLTestCases,  testInvalidIPv4);

#ifdef OW_HAVE_IPV6
	ADD_TEST_TO_SUITE(OW_URLTestCases,  testValidIPv6);
	ADD_TEST_TO_SUITE(OW_URLTestCases,  testInvalidIPv6);
#endif

	ADD_TEST_TO_SUITE(OW_URLTestCases,  testAddressEquality);

	return testSuite;
}

