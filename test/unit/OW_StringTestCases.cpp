/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
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
#include "OW_config.h"
#include "OW_StringTestCases.hpp"
#include "OW_String.hpp"
#include "OW_Types.h"

void OW_StringTestCases::setUp()
{
}

void OW_StringTestCases::tearDown()
{
}

void OW_StringTestCases::testErase()
{
	OW_String s = "abc";
	s.erase();
	unitAssert( s == "" );
	unitAssert( s.length() == 0 );
}

void OW_StringTestCases::testSubstring()
{
	OW_String s = "abc";
	unitAssert( s.substring(0, 0) == "" );
	unitAssert( s.substring(0, 0).length() == 0 );
	unitAssert( s.substring((OW_UInt32)-1) == "" );
	unitAssert( s.substring((OW_UInt32)-1).length() == 0 );
}

void OW_StringTestCases::testNumbers()
{
	OW_String s = "-1";
	unitAssert(s.toInt32() == -1);
	OW_String uls("5000000000000");
	OW_UInt64 ul = uls.toUInt64();
	unitAssert(ul == 5000000000000LL);
	OW_String uls2(ul);
	unitAssert(uls.equals(uls2));
	OW_String rs("4.56e+80");
	OW_Real64 r = rs.toReal64();
	OW_String rs2(r);
	unitAssert(rs.equals(rs2));	
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

	return testSuite;
}

