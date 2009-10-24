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
 * @author Dan Nuffer
 */


#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_CIMDateTimeTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_CIMDateTimeTestCases,"OW_CIMDateTime");
#include "OW_CIMDateTime.hpp"
#include "blocxx/DateTime.hpp"

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
	d1.set(2008, 2, 12, 21, 5, 3, 0, DateTime::E_LOCAL_TIME);
	CIMDateTime cd1(d1);
	d2 = cd1.toDateTime();
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

void OW_CIMDateTimeTestCases::testLessThan()
{
	// compare all sequential days to make sure we didn't screw up leap years.
	DateTime dt(0); // start out at the epoch
	while (dt.getYear() < 2020)
	{
		DateTime dt2(dt);
		dt2.addDays(1);
		unitAssert(CIMDateTime(dt) < CIMDateTime(dt2));

		dt.addDays(1);
	}

	// test every year boundary -- validates that leap years don't overlap
	for (int i = 0; i <= 9998; ++i)
	{
		CIMDateTime endOfYear1("00001231235959.999999-000");
		endOfYear1.setYear(i);
		CIMDateTime beginOfYear2("00010101000000.000000-000");
		beginOfYear2.setYear(i + 1);
		if (!(endOfYear1 < beginOfYear2))
		{
			std::cout << "\nFailed for year " << i << std::endl;
		}
		unitAssert(endOfYear1 < beginOfYear2);
	}

	// now a few from before the epoch, which unfortunately we can't use DateTime to help out.
	unitAssert(CIMDateTime("00040102030405.678987-420") < CIMDateTime("00040103000000.000000-000"));
	// extremes
	unitAssert(CIMDateTime("00000101000000.000000-999") < CIMDateTime("99991231235959.999999+999"));
	unitAssert(CIMDateTime("00000101000000.000000-000") < CIMDateTime("00000101000001.000000-000"));
	unitAssert(CIMDateTime("00000101000000.000000-000") < CIMDateTime("00000101000000.000001-000"));
	unitAssert(CIMDateTime("99991231235959.999998-000") < CIMDateTime("99991231235959.999999-000"));
	// off by 1 millisecond
	unitAssert(CIMDateTime("19691231235959.999999-000") < CIMDateTime("19700101000000.000000-000"));
	// try a leap day on a leap year
	unitAssert(CIMDateTime("20040228000000.000000-000") < CIMDateTime("20040229000000.000000-000"));
	unitAssert(CIMDateTime("19680228000000.000000-000") < CIMDateTime("19680229000000.000000-000"));
	// test milliseconds
	unitAssert(CIMDateTime("19680228000000.000000-000") < CIMDateTime("19680228000000.000001-000"));
	// test equivalence
	unitAssert(!(CIMDateTime("19680228000000.000000-000") < CIMDateTime("19680228000000.000000-000")));
	// test UTC offset
	unitAssert(CIMDateTime("19691231220000.000000-120") < CIMDateTime("19691231210000.000000-000"));
	// UTC offset equivalence
	unitAssert(!(CIMDateTime("19691231220000.000000-120") < CIMDateTime("19691231200000.000000-000")));
	unitAssert(!(CIMDateTime("19691231200000.000000-000") < CIMDateTime("19691231220000.000000-120")));
	// intervals are always less than datetimes
	unitAssert(CIMDateTime("99999999235959.999999:000") < CIMDateTime("00000101000000.000000-999"));
	unitAssert(CIMDateTime("00000000000000.000000:000") < CIMDateTime("99991231235959.999999+999"));
	unitAssert(!(CIMDateTime("99991231235959.999999+999") < CIMDateTime("00000000000000.000000:000")));
	unitAssert(!(CIMDateTime("00000101000000.000000-999") < CIMDateTime("99999999235959.999999:000")));
	// check intervals
	unitAssert(CIMDateTime("87654321010203.678987:000") < CIMDateTime("87654322010203.678987:000"));
	unitAssert(CIMDateTime("87654321010203.678987:000") < CIMDateTime("87654321020203.678987:000"));
	unitAssert(CIMDateTime("87654321010203.678987:000") < CIMDateTime("87654321010303.678987:000"));
	unitAssert(CIMDateTime("87654321010203.678987:000") < CIMDateTime("87654321010204.678987:000"));
	unitAssert(CIMDateTime("87654321010203.678987:000") < CIMDateTime("87654321010203.678988:000"));
	unitAssert(!(CIMDateTime("87654321010203.678987:000") < CIMDateTime("87654321010203.678987:000")));
	unitAssert(!(CIMDateTime("87654322010203.678987:000") < CIMDateTime("87654321010203.678987:000")));
	unitAssert(!(CIMDateTime("87654321020203.678987:000") < CIMDateTime("87654321010203.678987:000")));
	unitAssert(!(CIMDateTime("87654321010303.678987:000") < CIMDateTime("87654321010203.678987:000")));
	unitAssert(!(CIMDateTime("87654321010204.678987:000") < CIMDateTime("87654321010203.678987:000")));
	unitAssert(!(CIMDateTime("87654321010203.678988:000") < CIMDateTime("87654321010203.678987:000")));
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
    ADD_TEST_TO_SUITE(OW_CIMDateTimeTestCases, testLessThan);
	return testSuite;
}
