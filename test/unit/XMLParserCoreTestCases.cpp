/*******************************************************************************
* Copyright (C) 2007 Quest Software, Inc. All rights reserved.
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

#include "OW_config.h"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "XMLParserCoreTestCases.hpp"
#include "OW_XMLParserCore.hpp"
#include "OW_StringStream.hpp"
#include <cstring>

using namespace OpenWBEM;
using namespace std;

void XMLParserCoreTestCases::setUp()
{
}

void XMLParserCoreTestCases::tearDown()
{
}

void XMLParserCoreTestCases::testEmptyElement()
{
	String input("<foo/>");
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	bool has_token = parser.next(token);
	unitAssert( has_token );
	unitAssert( token.type == XMLToken::START_TAG );
	unitAssert( token.text.toString() == "foo" );
	unitAssert( token.attributes.empty() );

	has_token = parser.next(token);
	unitAssert( has_token );
	unitAssert( token.type == XMLToken::END_TAG );

	has_token = parser.next(token);
	unitAssert( !has_token );
}

void XMLParserCoreTestCases::testXmlDeclaration()
{
	String input("<?xml version=\"1.0\" encoding=\"LATIN-1\"?>");
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	bool has_token = parser.next(token);
	unitAssert( has_token );
	unitAssert( token.type == XMLToken::XML_DECLARATION );
	unitAssert( token.attributes.size() == 2U );
	unitAssert( strcmp(token.attributes[0].name.c_str(), "version") == 0);
	unitAssert( strcmp(token.attributes[0].value.c_str(), "1.0") == 0);
	unitAssert( strcmp(token.attributes[1].name.c_str(), "encoding") == 0);
	unitAssert( strcmp(token.attributes[1].value.c_str(), "LATIN-1") == 0);

	has_token = parser.next(token);
	unitAssert( !has_token );
}

void XMLParserCoreTestCases::testStartTag()
{
	String input("<a foo=\"up\" bar=\"down\"></a>");
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	bool has_token = parser.next(token);
	unitAssert( has_token );
	unitAssert( token.type == XMLToken::START_TAG );
	unitAssert( strcmp(token.text.c_str(), "a") == 0 );
	unitAssert( token.attributes.size() == 2U );
	unitAssert( strcmp(token.attributes[0].name.c_str(), "foo") == 0);
	unitAssert( strcmp(token.attributes[0].value.c_str(), "up") == 0);
	unitAssert( strcmp(token.attributes[1].name.c_str(), "bar") == 0);
	unitAssert( strcmp(token.attributes[1].value.c_str(), "down") == 0);

	has_token = parser.next(token);
	unitAssert( !has_token );
}

void XMLParserCoreTestCases::testEndTag()
{
	String input("<a></a>");
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	bool has_token = parser.next(token);
	unitAssert( has_token );

	has_token = parser.next(token);
	unitAssert( has_token );
	unitAssert( token.type == XMLToken::END_TAG );

	has_token = parser.next(token);
	unitAssert( !has_token );
}


Test* XMLParserCoreTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("XMLParserCore");

	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testEmptyElement);

	return testSuite;
}

