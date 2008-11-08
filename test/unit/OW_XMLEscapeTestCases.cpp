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
 * @author Dan Nuffer
 */


#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_XMLEscapeTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_XMLEscapeTestCases,"OW_XMLEscape");
#include "OW_XMLEscape.hpp"
#include "blocxx/UTF8Utils.hpp"

#include <climits>

using namespace OpenWBEM;

void OW_XMLEscapeTestCases::setUp()
{
}

void OW_XMLEscapeTestCases::tearDown()
{
}

void OW_XMLEscapeTestCases::test()
{
	unitAssertEquals( "&amp;&quot;&lt;&gt;&#9;&apos;&#10;&#13;", XMLEscape("&\"<>\x9'\n\r") );
	unitAssertEquals( "Minix (&lt;=1.4a) | Linux | Microsoft", XMLEscape("Minix (<=1.4a) | Linux | Microsoft") );
	unitAssertEquals( "abcdefg", XMLEscape("abcdefg") );
}

Test* OW_XMLEscapeTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_XMLEscape");

	testSuite->addTest (new TestCaller <OW_XMLEscapeTestCases>
			("test",
			&OW_XMLEscapeTestCases::test));

	return testSuite;
}

AUTO_UNIT_TEST(XMLOutOfRange)
{
	const String replacement = "\xef\xbf\xbd";

	const String bad1 = UTF8Utils::UCS2toUTF8(0xffff);
	const String bad2 = UTF8Utils::UCS2toUTF8(0xd800);
	const String bad3 = UTF8Utils::UCS2toUTF8(0xdfff);
	const String bad4 = UTF8Utils::UCS2toUTF8(0xdabc);
	const String bad5 = "ab\x1f""cd\x08""ef\x0b";

	unitAssertEquals( replacement, replacement );
	unitAssertEquals( replacement, XMLEscape(replacement) );
	unitAssertEquals( replacement, XMLEscape(bad1) );
	unitAssertEquals( replacement, XMLEscape(bad2) );
	unitAssertEquals( replacement, XMLEscape(bad3) );
	unitAssertEquals( replacement, XMLEscape(bad4) );
	unitAssertEquals( "ab" + replacement + "cd" + replacement + "ef" + replacement, XMLEscape(bad5) );
	unitAssertEquals( "ab" + replacement + "cd" + replacement + "ef" + replacement + "gh", XMLEscape(bad5 + "gh") );
	unitAssertEquals( replacement + replacement + replacement + replacement, XMLEscape(bad1 + bad2 + bad3 + bad4) );

	// Check that the full range of invalid characters is replaced...
	{
		// 0000-0008 is invalid (0x0000 is interpreted as an empty string)
		for( char i = 0x0001; i <= 0x0008; ++i )
		{
			unitAssertEquals(replacement, XMLEscape(String(i)));
		}
		// 000b is invalid
		unitAssertEquals(replacement, XMLEscape(String(char(0x0b))));
		// 000e-001f is invalid
		for( char i = 0x000e; i <= 0x001f; ++i )
		{
			unitAssertEquals(replacement, XMLEscape(String(i)));
		}
		// d800-dfff is invalid
		for( int i = 0xd800; i <= 0xdfff; ++i )
		{
			String utf8 = UTF8Utils::UCS2toUTF8(i);
			unitAssertEquals(replacement, XMLEscape(utf8));
		}
		// fffe-ffff is invalid
		for( int i = 0xfffe; i <= 0xffff; ++i )
		{
			String utf8 = UTF8Utils::UCS2toUTF8(i);
			unitAssertEquals(replacement, XMLEscape(utf8));
		}
	}

	// Check that the full range of valid characters is not replaced.
	{
		// 0000-0008 are invalid
		// 0009-000a are valid, but they are '\t' and '\n' are replaced.
		// 000b is invalid
		// 000c is valid
		unitAssertEquals(String(char(0x0c)), XMLEscape(String(char(0x0c))));
		// 000d == '\r' and is replaced
		// 000e-001f are invalid
		// 0020-d7ff are valid, with some characters that are substituted for
		// some non-invalid replacement sequence.
		for( char i = 0x0020; i < CHAR_MAX; ++i )
		{
			if( (i == '\"') || (i == '\'') || (i == '<') || (i == '>') || (i == '&') )
			{
				continue;
			}
			unitAssertEquals(String(i), XMLEscape(String(i)));
		}
		for( int i = CHAR_MAX; i < 0xd800; ++i )
		{
			String utf8 = UTF8Utils::UCS2toUTF8(i);
			unitAssertEquals(utf8, XMLEscape(utf8));
		}
		// d800-dfff are invalid
		// e000-fffd are valid
		for( int i = 0xe000; i < 0xfffe; ++i )
		{
			String utf8 = UTF8Utils::UCS2toUTF8(i);
			unitAssertEquals(utf8, XMLEscape(utf8));
		}
		// fffe-ffff are invalid
	}
}
