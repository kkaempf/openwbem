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
	unitAssert(DateTime("20111020051022.333333+000") == CIMDateTime("20111020051022.333333+000").toDateTime());
}

void OW_CIMDateTimeTestCases::testConstructor()
{
	String empty = "00000000000000.000000:000";
	unitAssertNoThrow(CIMDateTime(empty));
	unitAssert(CIMDateTime(empty).toString() == empty);
}

void OW_CIMDateTimeTestCases::testStringConstructor()
{
	String testDateTimeString("20040102030405.678987-420");
	CIMDateTime testDateTime(testDateTimeString);
	unitAssert(testDateTimeString.equals(testDateTime.toString()));
}

void OW_CIMDateTimeTestCases::testGetMethods()
{
	String testDateTimeString("20040102030405.678987-420");
	CIMDateTime testDateTime(testDateTimeString);

	unitAssert(testDateTime.getYear() == 2004);
	unitAssert(testDateTime.getMonth() == 1);
	unitAssert(testDateTime.getDay() == 2);
	unitAssert(testDateTime.getHours() == 3);
	unitAssert(testDateTime.getMinutes() == 4);
	unitAssert(testDateTime.getSeconds() == 5);
	unitAssert(testDateTime.getMicroSeconds() == 678987);
	unitAssert(testDateTime.getUtc() == -420);
	unitAssert(!testDateTime.isInterval());
}

void OW_CIMDateTimeTestCases::testSetMethods()
{
	String testDateTimeString("20040102030405.678987-420");
	CIMDateTime testDateTime(testDateTimeString);

	CIMDateTime testDateTime2 = CIMDateTime();
	testDateTime2.setInterval(false);
	testDateTime2.setYear(2004);
	testDateTime2.setMonth(1);
	testDateTime2.setDay(2);
	testDateTime2.setHours(3);
	testDateTime2.setMinutes(4);
	testDateTime2.setSeconds(5);
	testDateTime2.setMicroSeconds(678987);
	testDateTime2.setUtc(-420);

	unitAssert(testDateTime2.equal(testDateTime));
}

void OW_CIMDateTimeTestCases::testStringConstructorInterval()
{
	String testDateTimeString("87654321010203.678987:000");
	CIMDateTime testDateTime(testDateTimeString);

	unitAssert(testDateTimeString.equals(testDateTime.toString()));
}

void OW_CIMDateTimeTestCases::testGetMethodsInterval()
{
	String testDateTimeString("87654321010203.678987:000");
	CIMDateTime testDateTime(testDateTimeString);

	unitAssert(testDateTime.getDays() == 87654321);
	unitAssert(testDateTime.getHours() == 1);
	unitAssert(testDateTime.getMinutes() == 2);
	unitAssert(testDateTime.getSeconds() == 3);
	unitAssert(testDateTime.getMicroSeconds() == 678987);
	unitAssert(testDateTime.isInterval());
}

void OW_CIMDateTimeTestCases::testSetMethodsInterval()
{
	String testDateTimeString("87654321010203.678987:000");
	CIMDateTime testDateTime(testDateTimeString);

	CIMDateTime testDateTime2 = CIMDateTime();
	testDateTime2.setInterval(true);
	testDateTime2.setDays(87654321);
	testDateTime2.setHours(1);
	testDateTime2.setMinutes(2);
	testDateTime2.setSeconds(3);
	testDateTime2.setMicroSeconds(678987);

	unitAssert(testDateTime2.equal(testDateTime));
}

Test* OW_CIMDateTimeTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_CIMDateTime");

	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testToDateTime);
	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testConstructor);
	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testStringConstructor);
	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testGetMethods);
	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testSetMethods);
	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testStringConstructorInterval);
	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testGetMethodsInterval);
	ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testSetMethodsInterval);
	return testSuite;
}
