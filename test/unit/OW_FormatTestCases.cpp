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

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_FormatTestCases.hpp"
#include "OW_Format.hpp"

using namespace OpenWBEM;

void OW_FormatTestCases::setUp()
{
}

void OW_FormatTestCases::tearDown()
{
}

void OW_FormatTestCases::testFormat()
{
	unitAssert( Format("%1 != %2", 1, 2) == "1 != 2" );
	unitAssert( Format("%1 != %2", 1, 2).toString() == "1 != 2" );
	unitAssert( Format("%1,%2,%3,%4,%5,%6,%7,%8,%9", 
				1,2,3,4,5,6,7,8,9).toString() == "1,2,3,4,5,6,7,8,9" );
	unitAssert( String(Format("%1 != %2", 1, 2).c_str()) == "1 != 2" );
}

void OW_FormatTestCases::testPercent()
{
	unitAssert( Format("%1%%%2", "abc", "def").toString() == "abc%def" );
	unitAssert( Format("%2%%%1%3%2", "one", "two", '%').toString() == 
			"two%one%two" );
}

void OW_FormatTestCases::testError()
{
	unitAssert( Format("%1,%2xx", 1).toString() == "1,\n*** Parameter specifier too large.\n*** Error in format string at \"2xx\".\n" );
	unitAssert( Format("%xABC", 'e').toString() == "\n*** Error in format string at \"xABC\".\n" );
	unitAssert( Format("%0ABC", 'e').toString() == "\n*** Error in format string at \"0ABC\".\n" );
}

Test* OW_FormatTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Format");

	testSuite->addTest (new TestCaller <OW_FormatTestCases> 
			("testFormat", 
			&OW_FormatTestCases::testFormat));
	testSuite->addTest (new TestCaller <OW_FormatTestCases> 
			("testPercent", 
			&OW_FormatTestCases::testPercent));
	testSuite->addTest (new TestCaller <OW_FormatTestCases> 
			("testError", 
			&OW_FormatTestCases::testError));

	return testSuite;
}


