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
#include "OW_CIMDateTimeTestCases.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_DateTime.hpp"

using namespace OpenWBEM;

void OW_CIMDateTimeTestCases::setUp()
{
}

void OW_CIMDateTimeTestCases::tearDown()
{
}

void OW_CIMDateTimeTestCases::testToDateTime()
{
	DateTime d1;
	d1.setToCurrent();
	CIMDateTime cd(d1);
	DateTime d2 = cd.toDateTime();
	unitAssert(d1 == d2);
	unitAssert(DateTime("21111020051022.333333+000") == CIMDateTime("21111020051022.333333+000").toDateTime());
}

Test* OW_CIMDateTimeTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_CIMDateTime");

	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testToDateTime);

	return testSuite;
}

