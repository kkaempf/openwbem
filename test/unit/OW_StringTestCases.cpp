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


#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_config.h"
#include "OW_StringTestCases.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_Types.hpp"

using namespace OpenWBEM;

void OW_StringTestCases::setUp()
{
}

void OW_StringTestCases::tearDown()
{
}

void OW_StringTestCases::testErase()
{
	String s = "abc";
	s.erase();
	unitAssert( s == "" );
	unitAssert( s.length() == 0 );
	unitAssert( String("12345").erase(2) == "12" );
	unitAssert( String("12345").erase(2, 1) == "1245" );
}

void OW_StringTestCases::testEqualsIgnoreCase()
{
	String s = "abc";
	unitAssert(s.equalsIgnoreCase("abc")); 
	s = ""; 
	unitAssert(s.equalsIgnoreCase("")); 
}

void OW_StringTestCases::testSubstring()
{
	String s = "abc";
	unitAssert( s.substring(0, 0) == "" );
	unitAssert( s.substring(0, 0).length() == 0 );
	unitAssert( s.substring(static_cast<UInt32>(-1)) == "" );
	unitAssert( s.substring(static_cast<UInt32>(-1)).length() == 0 );
	unitAssert( s.substring(1) == "bc" );
	unitAssert( s.substring(1, 1) == "b" );
	unitAssert( s.substring(2, 3) == "c" );
}

void OW_StringTestCases::testNumbers()
{
	String s = "-1";
	unitAssert(s.toInt32() == -1);
	String uls("5000000000000");
	UInt64 ul = uls.toUInt64();
	unitAssert(ul == 5000000000000LL);
	String uls2(ul);
	unitAssert(uls.equals(uls2));
	String rs("4.56e+80");
	Real64 r = rs.toReal64();
	String rs2(r);
	unitAssert(rs.equals(rs2));	
}

void OW_StringTestCases::testTokenize()
{
	String teststring1 = "0.1.2.3.4";
	StringArray tokenized1 = teststring1.tokenize( "." );
	unitAssert( tokenized1.size() == 5 );
	unitAssert( tokenized1[0] == "0" );
	unitAssert( tokenized1[1] == "1" );
	unitAssert( tokenized1[2] == "2" );
	unitAssert( tokenized1[3] == "3" );
	unitAssert( tokenized1[4] == "4" );

	tokenized1 = teststring1.tokenize( ".", String::E_RETURN_TOKENS );
	unitAssert( tokenized1.size() == 9 );
	unitAssert( tokenized1[0] == "0" );
	unitAssert( tokenized1[1] == "." );
	unitAssert( tokenized1[2] == "1" );
	unitAssert( tokenized1[3] == "." );
	unitAssert( tokenized1[4] == "2" );
	unitAssert( tokenized1[5] == "." );
	unitAssert( tokenized1[6] == "3" );
	unitAssert( tokenized1[7] == "." );
	unitAssert( tokenized1[8] == "4" );

	tokenized1 = teststring1.tokenize( ".", String::E_DISCARD_TOKENS, String::E_RETURN_EMPTY_TOKENS );
	unitAssert( tokenized1.size() == 5 );
	unitAssert( tokenized1[0] == "0" );
	unitAssert( tokenized1[4] == "4" );

	
	String teststring2 = "0..1.2.3..4";
	StringArray tokenized2 = teststring2.tokenize( "." );
	unitAssert( tokenized2.size() == 5 );
	unitAssert( tokenized2[0] == "0" );
	unitAssert( tokenized2[1] == "1" );
	unitAssert( tokenized2[2] == "2" );
	unitAssert( tokenized2[3] == "3" );
	unitAssert( tokenized2[4] == "4" );

	tokenized2 = teststring2.tokenize( ".", String::E_RETURN_TOKENS );
	unitAssert( tokenized2.size() == 11 );
	unitAssert( tokenized2[0] == "0" );
	unitAssert( tokenized2[1] == "." );
	unitAssert( tokenized2[2] == "." );	
	unitAssert( tokenized2[3] == "1" );
	unitAssert( tokenized2[4] == "." );
	unitAssert( tokenized2[5] == "2" );
	unitAssert( tokenized2[6] == "." );
	unitAssert( tokenized2[7] == "3" );
	unitAssert( tokenized2[8] == "." );
	unitAssert( tokenized2[9] == "." );
	unitAssert( tokenized2[10] == "4" );

	tokenized2 = teststring2.tokenize( ".", String::E_DISCARD_TOKENS, String::E_RETURN_EMPTY_TOKENS );
	unitAssert( tokenized2.size() == 7 );
	unitAssert( tokenized2[0] == "0" );
	unitAssert( tokenized2[1] == "" );
	unitAssert( tokenized2[2] == "1" );
	unitAssert( tokenized2[3] == "2" );
	unitAssert( tokenized2[4] == "3" );
	unitAssert( tokenized2[5] == "" );
	unitAssert( tokenized2[6] == "4" );

	tokenized2 = teststring2.tokenize( ".", String::E_RETURN_TOKENS, String::E_RETURN_EMPTY_TOKENS );
	unitAssert( tokenized2.size() == 13 );
	unitAssert( tokenized2[0] == "0" );
	unitAssert( tokenized2[1] == "." );
	unitAssert( tokenized2[2] == "" );	
	unitAssert( tokenized2[3] == "." );	
	unitAssert( tokenized2[4] == "1" );
	unitAssert( tokenized2[5] == "." );
	unitAssert( tokenized2[6] == "2" );
	unitAssert( tokenized2[7] == "." );
	unitAssert( tokenized2[8] == "3" );
	unitAssert( tokenized2[9] == "." );
	unitAssert( tokenized2[10] == "" );
	unitAssert( tokenized2[11] == "." );
	unitAssert( tokenized2[12] == "4" );	


	String teststring3 = "a b c\nd e f\n\r\tg";
	StringArray tokenized3 = teststring3.tokenize();
	unitAssert( tokenized3.size() == 7 );
	unitAssert( tokenized3[0] == "a" );
	unitAssert( tokenized3[1] == "b" );
	unitAssert( tokenized3[2] == "c" );
	unitAssert( tokenized3[3] == "d" );
	unitAssert( tokenized3[4] == "e" );
	unitAssert( tokenized3[5] == "f" );
	unitAssert( tokenized3[6] == "g" );

	tokenized3 = teststring3.tokenize(" \n\r\t",  String::E_RETURN_TOKENS);
	unitAssert( tokenized3.size() == 15 );
	unitAssert( tokenized3[0] == "a" );
	unitAssert( tokenized3[1] == " " );
	unitAssert( tokenized3[2] == "b" );
	unitAssert( tokenized3[3] == " " );
	unitAssert( tokenized3[4] == "c" );
	unitAssert( tokenized3[5] == "\n");
	unitAssert( tokenized3[6] == "d" );
	unitAssert( tokenized3[7] == " " );
	unitAssert( tokenized3[8] == "e" );
	unitAssert( tokenized3[9] == " " );
	unitAssert( tokenized3[10] == "f" );
	unitAssert( tokenized3[11] == "\n" );
	unitAssert( tokenized3[12] == "\r" );
	unitAssert( tokenized3[13] == "\t" );
	unitAssert( tokenized3[14] == "g" );

	tokenized3 = teststring3.tokenize(" \n\r\t",  String::E_DISCARD_TOKENS, String::E_RETURN_EMPTY_TOKENS);
	unitAssert( tokenized3.size() == 9 );
	unitAssert( tokenized3[0] == "a" );
	unitAssert( tokenized3[1] == "b" );
	unitAssert( tokenized3[2] == "c" );
	unitAssert( tokenized3[3] == "d" );
	unitAssert( tokenized3[4] == "e" );
	unitAssert( tokenized3[5] == "f" );
	unitAssert( tokenized3[6] == "" );
	unitAssert( tokenized3[7] == "" );
	unitAssert( tokenized3[8] == "g" );	


	String teststring4 = "foo bar        baz  quux  flarp   snoz  blarf                    zzyzx   veryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryvery_looooooooooonnnnnnnnggggggg_word";
	StringArray tokenized4 = teststring4.tokenize();
	unitAssert( tokenized4.size() == 9 );
	unitAssert( tokenized4[0] == "foo" );
	unitAssert( tokenized4[1] == "bar" );
	unitAssert( tokenized4[2] == "baz" ); 
	unitAssert( tokenized4[3] == "quux" );
	unitAssert( tokenized4[4] == "flarp" );
	unitAssert( tokenized4[5] == "snoz" );
	unitAssert( tokenized4[6] == "blarf" );
	unitAssert( tokenized4[7] == "zzyzx" );
	unitAssert( tokenized4[8].indexOf("veryvery") == 0 );
}

Test* OW_StringTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_String");

	testSuite->addTest (new TestCaller <OW_StringTestCases> 
			("testErase", 
			&OW_StringTestCases::testErase));
	testSuite->addTest (new TestCaller <OW_StringTestCases> 
			("testSubstring", 
			&OW_StringTestCases::testSubstring));
	testSuite->addTest (new TestCaller <OW_StringTestCases> 
			("testNumbers", 
			&OW_StringTestCases::testNumbers));
	testSuite->addTest (new TestCaller <OW_StringTestCases> 
			("testEqualsIgnoreCase", 
			&OW_StringTestCases::testEqualsIgnoreCase));
	testSuite->addTest (new TestCaller <OW_StringTestCases> 
			("testTokenize", 
			&OW_StringTestCases::testTokenize));	

	return testSuite;
}

