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

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_HTTPUtilsTestCases.hpp"
#include "OW_HTTPUtils.hpp"

using namespace OpenWBEM;

void OW_HTTPUtilsTestCases::setUp()
{
}

void OW_HTTPUtilsTestCases::tearDown()
{
}

void OW_HTTPUtilsTestCases::testescapeCharForURL()
{
	unitAssert( HTTPUtils::escapeCharForURL('A') == "%41" );
	unitAssert( HTTPUtils::escapeCharForURL('\x22') == "%22" );
	unitAssert( HTTPUtils::escapeCharForURL('\xFF') == "%FF" );
	unitAssert( HTTPUtils::escapeCharForURL('\xAA') == "%AA" );
}

void OW_HTTPUtilsTestCases::testescapeForURL()
{
	unitAssert( HTTPUtils::escapeForURL("Hello") == "Hello" );
	unitAssert( HTTPUtils::escapeForURL("$-_.+!*\'(),;/?:@&=") == "$-_.+!*\'(),;/?:@&=" );
	unitAssert( HTTPUtils::escapeForURL("\x1\x4\x10\x20\xFF%") == "%01%04%10%20%FF%25" );
}
#include <iostream>
void OW_HTTPUtilsTestCases::testbase64Encode()
{
	// These values taken from /usr/lib/python2.2/test/test_base64.py
	unitAssert( HTTPUtils::base64Encode("www.python.org") == "d3d3LnB5dGhvbi5vcmc=");
	unitAssert( HTTPUtils::base64Encode("a") == "YQ==");
	unitAssert( HTTPUtils::base64Encode("ab") == "YWI=");
	unitAssert( HTTPUtils::base64Encode("abc") == "YWJj");
	unitAssert( HTTPUtils::base64Encode("") == "");
	unitAssert( HTTPUtils::base64Encode("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#0^&*();:<>,. []{}") ==
		"YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjAxMjM0NTY3ODkhQCMwXiYqKCk7Ojw+LC4gW117fQ==");

	// test version with length
	unitAssert( HTTPUtils::base64Encode(reinterpret_cast<const UInt8*>("a"), 1) == "YQ==");
	unitAssert( HTTPUtils::base64Encode(reinterpret_cast<const UInt8*>("ab"), 2) == "YWI=");
	unitAssert( HTTPUtils::base64Encode(reinterpret_cast<const UInt8*>("abc"), 2) == "YWI=");
	unitAssert( HTTPUtils::base64Encode(reinterpret_cast<const UInt8*>("a\0"), 2) == "YQA=");
	unitAssert( HTTPUtils::base64Encode(reinterpret_cast<const UInt8*>("a\0\0"), 3) == "YQAA");
}

void OW_HTTPUtilsTestCases::testbase64Decode()
{
	unitAssert( HTTPUtils::base64Decode(String("d3d3LnB5dGhvbi5vcmc=")) == "www.python.org");
	unitAssert( HTTPUtils::base64Decode(String("YQ==")) == "a");
	unitAssert( HTTPUtils::base64Decode(String("YWI=")) == "ab");
	unitAssert( HTTPUtils::base64Decode(String("YWJj")) == "abc");
	unitAssert( HTTPUtils::base64Decode(
		String("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjAxMjM0NTY3ODkhQCMwXiYqKCk7Ojw+LC4gW117fQ==")) ==
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#0^&*();:<>,. []{}");

}

Test* OW_HTTPUtilsTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_HTTPUtils");

	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testescapeCharForURL);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testescapeForURL);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testbase64Encode);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testbase64Decode);

	return testSuite;
}

