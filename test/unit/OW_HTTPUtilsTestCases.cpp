/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */


#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_HTTPUtilsTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_HTTPUtilsTestCases,"OW_HTTPUtils");
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

void OW_HTTPUtilsTestCases::testunescapeCharForURL()
{
	unitAssert( HTTPUtils::unescapeCharForURL("%41") == 'A' );
	unitAssert( HTTPUtils::unescapeCharForURL("%22") == '\x22' );
	unitAssert( HTTPUtils::unescapeCharForURL("%FF") == '\xFF' );
	unitAssert( HTTPUtils::unescapeCharForURL("%AA") == '\xAA' );
	unitAssert( HTTPUtils::unescapeCharForURL("%bc") == '\xbc' );
	unitAssert( HTTPUtils::unescapeCharForURL("%10E") == '\x10' );
	unitAssertThrows( HTTPUtils::unescapeCharForURL("") );
	unitAssertThrows( HTTPUtils::unescapeCharForURL("abc") );
	unitAssertThrows( HTTPUtils::unescapeCharForURL("%") );
	unitAssertThrows( HTTPUtils::unescapeCharForURL("%1") );
	unitAssertThrows( HTTPUtils::unescapeCharForURL("%1Z") );
}

void OW_HTTPUtilsTestCases::testescapeForURL()
{
	unitAssert( HTTPUtils::escapeForURL("Hello") == "Hello" );
	unitAssert( HTTPUtils::escapeForURL("-_.!*\'()") == "-_.!*\'()");
	unitAssert( HTTPUtils::escapeForURL("\x1\x4\x10\x20\xFF%") == "%01%04%10%20%FF%25" );
}

void OW_HTTPUtilsTestCases::testunescapeForURL()
{
	unitAssert( HTTPUtils::unescapeForURL("Hello") == "Hello" );
	unitAssert( HTTPUtils::unescapeForURL("-_.!*\'()") == "-_.!*\'()");
	unitAssert( HTTPUtils::unescapeForURL("%01%04%10%20%FF%25") == "\x1\x4\x10\x20\xFF%");
}

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

	// Empty source on encode
	// String version
	unitAssert(0 == HTTPUtils::base64Encode(String("")).length());
	// Length version
	unitAssert(0 == HTTPUtils::base64Encode(reinterpret_cast<const UInt8*>(""), 0).length());
}

void OW_HTTPUtilsTestCases::testbase64Decode()
{
	unitAssert( HTTPUtils::base64Decode(String("d3d3LnB5dGhvbi5vcmc=")) == "www.python.org");
	unitAssert( HTTPUtils::base64Decode(String("YQ==")) == "a");
	unitAssert( HTTPUtils::base64Decode(String("YWI=")) == "ab");
	unitAssert( HTTPUtils::base64Decode(String("YWJj")) == "abc");
	unitAssert( HTTPUtils::base64Decode(String("YWJjYQ==")) == "abca");
	unitAssert( HTTPUtils::base64Decode(String("YWJjYWI=")) == "abcab");
	unitAssert( HTTPUtils::base64Decode(String("YWJjYWJj")) == "abcabc");
	unitAssert( HTTPUtils::base64Decode(
		String("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjAxMjM0NTY3ODkhQCMwXiYqKCk7Ojw+LC4gW117fQ==")) ==
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#0^&*();:<>,. []{}");

	// test array version
	String www_python_org("www.python.org");
	String abc("abcabc");
	String lotsOfChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#0^&*();:<>,. []{}");
	unitAssert( HTTPUtils::base64Decode("d3d3LnB5dGhvbi5vcmc=") ==
		Array<char>(www_python_org.c_str(), www_python_org.c_str() + www_python_org.length()));
	unitAssert( HTTPUtils::base64Decode("YQ==") == Array<char>(abc.c_str(), abc.c_str() + 1));
	unitAssert( HTTPUtils::base64Decode("YWI=") == Array<char>(abc.c_str(), abc.c_str() + 2));
	unitAssert( HTTPUtils::base64Decode("YWJj") == Array<char>(abc.c_str(), abc.c_str() + 3));
	unitAssert( HTTPUtils::base64Decode("YWJjYQ==") == Array<char>(abc.c_str(), abc.c_str() + 4));
	unitAssert( HTTPUtils::base64Decode("YWJjYWI=") == Array<char>(abc.c_str(), abc.c_str() + 5));
	unitAssert( HTTPUtils::base64Decode("YWJjYWJj") == Array<char>(abc.c_str(), abc.c_str() + 6));
	unitAssert( HTTPUtils::base64Decode(
		"YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjAxMjM0NTY3ODkhQCMwXiYqKCk7Ojw+LC4gW117fQ==") ==
		Array<char>(lotsOfChars.c_str(), lotsOfChars.c_str() + lotsOfChars.length()));

	// test binary string w/ nulls
	char binstring[] = "a\0\0";
	unitAssert( HTTPUtils::base64Decode("YQA=") == Array<char>(binstring, binstring + 2));
	unitAssert( HTTPUtils::base64Decode("YQAA") == Array<char>(binstring, binstring + 3));


	// Empty source on decode
	// String version
	unitAssert(0 == HTTPUtils::base64Decode(String("")).length());
	// Array version
	unitAssert(0 == HTTPUtils::base64Decode("").size());
}

void OW_HTTPUtilsTestCases::testbase64EncodeDecode()
{
	// Tests moved from the old OW_Base64TestCases file.  HTTP Credentials and the like
	String info("Aladdin:open sesame");
	String encoded = HTTPUtils::base64Encode(info);
	unitAssert(encoded.equals("QWxhZGRpbjpvcGVuIHNlc2FtZQ=="));
	unitAssert(HTTPUtils::base64Decode(encoded).equals(info));
	encoded = "Og==";
	info = HTTPUtils::base64Decode(encoded);
	unitAssert(info.equals(":"));
	unitAssert(HTTPUtils::base64Encode(info).equals(encoded));

	// Test the other flavors of encode and decode
	info = "Aladdin:open sesame";
	encoded = HTTPUtils::base64Encode(reinterpret_cast<const UInt8*>(info.c_str()), info.length());
	unitAssert(encoded.equals("QWxhZGRpbjpvcGVuIHNlc2FtZQ=="));
	unitAssert(HTTPUtils::base64Decode(encoded.c_str()) == Array<char>(info.c_str(), info.c_str() + info.length()));
	encoded = "Og==";
	Array<char> decoded = HTTPUtils::base64Decode(encoded.c_str());
	unitAssert(decoded == Array<char>(1, ':'));
	unitAssert(HTTPUtils::base64Encode(reinterpret_cast<const UInt8*>(&decoded[0]), decoded.size()).equals(encoded.c_str()));
}

Test* OW_HTTPUtilsTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_HTTPUtils");

	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testescapeCharForURL);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testunescapeCharForURL);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testescapeForURL);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testunescapeForURL);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testbase64Encode);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testbase64Decode);
	ADD_TEST_TO_SUITE(OW_HTTPUtilsTestCases, testbase64EncodeDecode);

	return testSuite;
}

