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
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_InetAddressTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_InetAddressTestCases,"OW_SocketAddress");
#include "OW_SocketAddress.hpp"

using namespace OpenWBEM;

void OW_InetAddressTestCases::setUp()
{
}

void OW_InetAddressTestCases::tearDown()
{
}

void OW_InetAddressTestCases::testSomething()
{
	SocketAddress addr = SocketAddress::getAnyLocalHost();
	String addrName = addr.getName();
	unitAssert(addrName.length());
	//unitAssert(addrName.indexOf('.') != String::npos);
}

void OW_InetAddressTestCases::testIPv4_localaddress()
{
	unitAssertNoThrow(SocketAddress::getByName("127.0.0.1"));
	unitAssertThrows(SocketAddress::getByName("127.0.0.1.3")); // error: too many entries
}

#ifdef OW_HAVE_IPV6
void OW_InetAddressTestCases::testIPv6_localaddress()
{
	// These are local IPv6 addresses (all mapping to 127.0.0.1) and
	// should not cause any host resolution errors.
	unitAssertNoThrow(SocketAddress::getByName("::1"));
	unitAssertNoThrow(SocketAddress::getByName("0::1"));
	unitAssertNoThrow(SocketAddress::getByName("0:0:0:0:0:0:0:1"));
	unitAssertNoThrow(SocketAddress::getByName("::ffff:127.0.0.1"));
	unitAssertNoThrow(SocketAddress::getByName("0:0:0:0:0:ffff:127.0.0.1"));
	unitAssertNoThrow(SocketAddress::getByName("0:0:0:0:0:ffff:7F00:0001"));
	// Prove that the above is actually doing something that really does throw.
	unitAssertThrows(SocketAddress::getByName("::0::1")); // error: only one :: is allowed.
	unitAssertThrows(SocketAddress::getByName("0:0:0:0:0:0:0:0:0")); // error: too many entries
}
#endif

Test* OW_InetAddressTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_SocketAddress");

	ADD_TEST_TO_SUITE(OW_InetAddressTestCases, testSomething);
	ADD_TEST_TO_SUITE(OW_InetAddressTestCases, testIPv4_localaddress);
#ifdef OW_HAVE_IPV6
	ADD_TEST_TO_SUITE(OW_InetAddressTestCases, testIPv6_localaddress);
#endif

	return testSuite;
}

