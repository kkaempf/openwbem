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
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "XMLParserCoreTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(XMLParserCoreTestCases,"XMLParserCore");
#include "OW_XMLParserCore.hpp"
#include "OW_XMLParseException.hpp"
#include "blocxx/StringStream.hpp"
#include "blocxx/String.hpp"
#include <cstring>

#include <iostream> // DEBUG

using namespace OpenWBEM;
using namespace std;

void XMLParserCoreTestCases::setUp()
{
}

void XMLParserCoreTestCases::tearDown()
{
}

namespace {
	bool messageContains(Exception const & e, String const & s)
	{
		return String(e.getMessage()).indexOf(s) != String::npos;
	}
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

void XMLParserCoreTestCases::testDocTypeTag()
{
	String input(
		"<?xml version=\"1.0\"?>"
		"<!DOCTYPE foo ["
		"<!ELEMENT foo (bar)>"
		"<!ATTLIST foo quux (flurp|blarp) \"flurp\">"
		"<!ELEMENT bar (#PCDATA)>"
		"]>"
		"<foo quux=\"flurp\">"
		"<bar>"
		"baz"
		"</bar>"
		"</foo>"
	);
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	//<?xml>
	bool has_token = parser.next(token);
	unitAssert( has_token );
	unitAssertEquals( XMLToken::XML_DECLARATION, token.type );

	//<!DOCTYPE>
	has_token = parser.next(token);
	unitAssert( has_token );
	unitAssertEquals( XMLToken::DOCTYPE, token.type );

	//<foo>
	has_token = parser.next(token);
	unitAssert( has_token );
	unitAssertEquals( XMLToken::START_TAG, token.type );
	unitAssertEquals( "foo", token.text.toString() );
	// quux="flurp"
	unitAssert( !token.attributes.empty() );

	//<bar>
	has_token = parser.next(token);
	unitAssert( has_token );
	unitAssertEquals( XMLToken::START_TAG, token.type );
	unitAssertEquals( "bar", token.text.toString() );

	//baz
	has_token = parser.next(token);
	unitAssert( has_token );
	unitAssertEquals( XMLToken::CONTENT, token.type );

	//</bar>
	has_token = parser.next(token);
	unitAssert( has_token );
	unitAssertEquals( "bar", token.text.toString() );
	unitAssertEquals( XMLToken::END_TAG, token.type );

	//</foo>
	has_token = parser.next(token);
	unitAssert( has_token );
	unitAssertEquals( "foo", token.text.toString() );
	unitAssertEquals( XMLToken::END_TAG, token.type );

	has_token = parser.next(token);
	unitAssert( !has_token );
}

void XMLParserCoreTestCases::testBadXmlDeclarationName()
{
	String input("<?37 ?>");
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	try {
		parser.next(token);
		unitAssert( false );
	}
	catch (XMLParseException & e) {
		unitAssert( e.getCode() == XMLParseException::BAD_XML_DECLARATION );
		unitAssert( messageContains(e, "Bad XML declaration") );
	}
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

	has_token = parser.next(token); // "</a>"
	unitAssert( has_token );

	has_token = parser.next(token); // end of input
	unitAssert( !has_token );
}

void XMLParserCoreTestCases::testBadStartTagName()
{
	String input("<(></O>");
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	try {
		parser.next(token);
		unitAssert( false );
	}
	catch (XMLParseException & e) {
		unitAssert( e.getCode() == XMLParseException::BAD_START_TAG );
		unitAssert( messageContains(e, "Bad opening element") );
	}
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

void XMLParserCoreTestCases::testBadEndTagName()
{
	String input("<a></37>");
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	bool has_token = parser.next(token);  // "<a>"
	unitAssert( has_token );

	try {
		parser.next(token);       // "</37>"
		unitAssert( false );
	}
	catch (XMLParseException & e) {
		unitAssert( e.getCode() == XMLParseException::BAD_END_TAG );
		unitAssert( messageContains(e, "Bad closing element") );
	}
}

void XMLParserCoreTestCases::testMismatchedStartEndTags()
{
	String input("<a></b>");
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	bool has_token = parser.next(token);  // "<a>"
	unitAssert( has_token );

	try {
		parser.next(token);  // "</b>"
		unitAssert( false );
	}
	catch (XMLParseException & e) {
		unitAssert( e.getCode() == XMLParseException::START_END_MISMATCH );
		char const * s = "Closing element does not match opening element";
		unitAssert( messageContains(e, s) );
	}
}

void XMLParserCoreTestCases::testEmptyDocument()
{
	String input;
	IStringStream is(input);
	XMLParserCore parser(is);
	XMLToken token;

	unitAssert(!parser.next(token));
}

Test* XMLParserCoreTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("XMLParserCore");

	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testEmptyElement);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testXmlDeclaration);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testDocTypeTag);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testBadXmlDeclarationName);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testStartTag);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testBadStartTagName);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testEndTag);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testBadEndTagName);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testMismatchedStartEndTags);
	ADD_TEST_TO_SUITE(XMLParserCoreTestCases, testEmptyDocument);

	return testSuite;
}

