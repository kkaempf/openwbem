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
#include "OW_XMLEscapeTestCases.hpp"
#include "OW_XMLEscape.hpp"

using namespace OpenWBEM;

void OW_XMLEscapeTestCases::setUp()
{
}

void OW_XMLEscapeTestCases::tearDown()
{
}

void OW_XMLEscapeTestCases::test()
{
	unitAssert( XMLEscape("&\"<>\x9'\n\r") == "&amp;&quot;&lt;&gt;&#9;&apos;&#10;&#13;" );
	unitAssert( XMLEscape("Minix (<=1.4a) | Linux | Microsoft") == "Minix (&lt;=1.4a) | Linux | Microsoft");
}

Test* OW_XMLEscapeTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_XMLEscape");

	testSuite->addTest (new TestCaller <OW_XMLEscapeTestCases>
			("test",
			&OW_XMLEscapeTestCases::test));

	return testSuite;
}

