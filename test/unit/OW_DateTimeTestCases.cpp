/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_DateTimeTestCases.hpp"
#include "OW_DateTime.hpp"
#include "OW_String.hpp"

#include <time.h>

using namespace OpenWBEM;

void OW_DateTimeTestCases::setUp()
{
}

void OW_DateTimeTestCases::tearDown()
{
}

namespace {
time_t calcTimeT(int year, int month, int day, int hour, int minute, int second)
{
	struct tm t;
	t.tm_year = year - 1900;
	t.tm_mon = month - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = minute;
	t.tm_sec = second;
	t.tm_isdst = 0;
	// since our input was in ust, adjust for the timezone, since mktime assumes it's input is in local time.
	time_t rv = mktime(&t) - timezone;
	return rv;
}

time_t calcTimeTLocal(int year, int month, int day, int hour, int minute, int second)
{
	struct tm t;
	t.tm_year = year - 1900;
	t.tm_mon = month - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = minute;
	t.tm_sec = second;
	t.tm_isdst = -1; // let the system adjust for dst
	time_t rv = mktime(&t);
	return rv;
}
} // end unnamed namespace

void OW_DateTimeTestCases::testCreation()
{
	// first test that DateTimeException is thrown for an obviously bad input
	unitAssertThrows(DateTime d(String("some bad date")));
	// now try some that look almost right
	unitAssertThrows(DateTime d(String("19980525X33015.000000-300")));
	unitAssertThrows(DateTime d(String("19980525133015.000X00-300")));
	unitAssertThrows(DateTime d(String("19980525133015.000000-30X")));
	unitAssertThrows(DateTime d(String("19980525133015X000000-300")));
	unitAssertThrows(DateTime d(String("19980525133015.000000X300")));
	unitAssertThrows(DateTime d(String("00000000000000.000000-000")));
	unitAssertThrows(DateTime d(String("99999999999999.999999-999")));
	unitAssertThrows(DateTime d(String("19980525X33015.000000-300")));
	unitAssertThrows(DateTime d(String("-9980525133015.000000-300")));
	// boundary conditions
	// negative year
	unitAssertThrows(DateTime d(String("-9980525133015.000000-300")));
	// 0 month
	unitAssertThrows(DateTime d(String("19980025133015.000000-300")));
	// 13 month
	unitAssertThrows(DateTime d(String("19981325133015.000000-300")));
	// 0 day
	unitAssertThrows(DateTime d(String("19980500133015.000000-300")));
	// 32 day
	unitAssertThrows(DateTime d(String("19980532133015.000000-300")));
	// -1 hour
	unitAssertThrows(DateTime d(String("19980525-13015.000000-300")));
	// 24 hour
	unitAssertThrows(DateTime d(String("19980525243015.000000-300")));
	// -1 minute
	unitAssertThrows(DateTime d(String("1998052513-115.000000-300")));
	// 60 minute
	unitAssertThrows(DateTime d(String("19980525136015.000000-300")));
	// -1 second
	unitAssertThrows(DateTime d(String("199805251330-1.000000-300")));
	// 61 second
	unitAssertThrows(DateTime d(String("19980525133061.000000-300")));
	// some ctime style dates that are just a little wrong
	unitAssertThrows(DateTime d(String("WXd Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("Wed JXn 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 00 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 99:49:08 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:49:08X1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:49:08 -993")));
	unitAssertThrows(DateTime d(String("Wed-Jun-30-21:49:08-1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 99 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("SuX Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("SaX Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("SXt Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("MoX Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("MXn Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("TuX Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("ThX Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("TXe Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("WeX Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("WXd Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("FrX Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("FXi Jun 30 21:49:08 1993")));
	unitAssertThrows(DateTime d(String("Xri Jun 30 21:49:08 1993")));

	// some more incorrect time patterns
	unitAssertThrows(DateTime d(String("WXd Jun 30 21:49:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed JXn 30 21:49:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun X0 21:49:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 X1:49:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:X9:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:49:X8 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:49:08 FOO X993")));
	unitAssertThrows(DateTime d(String("Wed-Jun-30-21:49:08-FOO-1993")));
	unitAssertThrows(DateTime d(String("Wed Jun-30-21:49:08-FOO-1993")));
	unitAssertThrows(DateTime d(String("Wed Jun-30-21:49:08-FOO-1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30-21:49:08-FOO-1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:49:08-FOO-1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:49:08 FOO-1993")));
	
	unitAssertThrows(DateTime d(String("Wed Jun 30 21-49-08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:49-08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21-49:08 FOO 1993")));
	
	unitAssertThrows(DateTime d(String("Wed Jun 3 211:49:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 300 2:49:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 3 2:49:08 VERYLONGERROR 1993")));	

	unitAssertThrows(DateTime d(String("Wed Jun 30 21:99:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 3 211:49:08 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 3 2:49:99 FOO 1993")));
	unitAssertThrows(DateTime d(String("Wed Jun 30 21:49:08 FOO 14-3")));	
	
	
	// now test a valid CIM DateTime
	{
		DateTime d("19980525133015.012345-300");
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1998);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 5);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 25);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 18);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 30);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 15);
		unitAssert(d.getMicrosecond() == 12345);
		unitAssert(d.get() == calcTimeT(1998, 5, 25, 18, 30, 15));
	}
	{
		DateTime d("19991231235959.012345-300");
		// year, month, day will wrap because of timezone adjustment
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 2000);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 1);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 1);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 4);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 59);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 59);
		unitAssert(d.getMicrosecond() == 12345);
		unitAssert(d.get() == calcTimeT(2000, 1, 1, 4, 59, 59));
	}
	{
		DateTime d("20000101045959.012345+300");
		// year, month, day will wrap backward because of timezone adjustment
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1999);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 31);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 23);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 59);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 59);
		unitAssert(d.getMicrosecond() == 12345);
		unitAssert(d.get() == calcTimeT(1999, 12, 31, 23, 59, 59));
	}
	// try some with asterisks
	{
		DateTime d("20000101045959.******+000");
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 2000);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 1);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 1);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 4);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 59);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 59);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeT(2000, 1, 1, 4, 59, 59));
	}
	// try some ctime style dates - one for each month
	{
		DateTime d(String("Wed Jan 16 13:50:09 1994"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1994);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 1);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 50);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 9);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1994, 1, 16, 13, 50, 9));
	}
	{
		DateTime d(String("Wed Feb 17 14:51:10 1995"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1995);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 2);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 51);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 10);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1995, 2, 17, 14, 51, 10));
	}
	{
		DateTime d(String("Wed Mar 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 3);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 3, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Apr 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 4);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 4, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed May 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 5);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 5, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Jun 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 6);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 6, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Jul 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 7);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 7, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Aug 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 8);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 8, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Sep 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 9);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 9, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Oct 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 10);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 10, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Nov 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 11);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 11, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Dec 15 12:49:08 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 12, 15, 12, 49, 8));
	}

	// Dates of a different form.
	{
		DateTime d(String("Wed Dec 2 12:49:08 UTC 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 2);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		//unitAssert(d.get() == calcTimeTLocal(1993, 12, 2, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Dec 15 7:49:08 UTC 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 7);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		// unitAssert(d.get() == calcTimeTLocal(1993, 12, 15, 7, 49, 8));
	}
	{
		DateTime d(String("Wed Dec 2 7:49:08 UTC 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 2);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 7);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		//unitAssert(d.get() == calcTimeTLocal(1993, 12, 2, 7, 49, 8));
	}
	{
		DateTime d(String("Wed Dec 20 7:49:08 GMT 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 20);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 7);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		//unitAssert(d.get() == calcTimeTLocal(1993, 12, 20, 7, 49, 8));
	}
	{
		DateTime d(String("Wed Dec 2 7:49:08 GMT 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getDay(DateTime::E_UTC_TIME) == 2);
		unitAssert(d.getHour(DateTime::E_UTC_TIME) == 7);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		//unitAssert(d.get() == calcTimeTLocal(1993, 12, 15, 12, 49, 8));
	}
	// Local timezones.
	{
		DateTime d(String("Wed Dec 15 12:49:08 MST 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 12, 15, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Dec 7 12:49:08 PST 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 12, 7, 12, 49, 8));
	}
	{
		DateTime d(String("Wed Dec 15 5:49:08 EST 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 12, 15, 5, 49, 8));
	}
	{
		DateTime d(String("Wed Dec 3 5:49:08 CST 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 12, 3, 5, 49, 8));
	}
	// FIXME! If things get corrected to detect proper timezones, then this case should fail.
	{
		DateTime d(String("Wed Dec 15 12:49:08 XXX 1993"));
		unitAssert(d.getYear(DateTime::E_UTC_TIME) == 1993);
		unitAssert(d.getMonth(DateTime::E_UTC_TIME) == 12);
		// unless we do some nice calculations to account for our local
		// timezone, we can't check the day or hour.
		//unitAssert(d.getDay(DateTime::E_UTC_TIME) == 15);
		//unitAssert(d.getHour(DateTime::E_UTC_TIME) == 12);
		unitAssert(d.getMinute(DateTime::E_UTC_TIME) == 49);
		unitAssert(d.getSecond(DateTime::E_UTC_TIME) == 8);
		unitAssert(d.getMicrosecond() == 0);
		unitAssert(d.get() == calcTimeTLocal(1993, 12, 15, 12, 49, 8));
	}	
}

void OW_DateTimeTestCases::testSetToCurrent()
{
	DateTime d;
	d.setToCurrent();
	unitAssert(d.get() != -1); // can't really test this too much...
	unitAssert(d.getMicrosecond() <= 999999);
}

Test* OW_DateTimeTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_DateTime");

	ADD_TEST_TO_SUITE(OW_DateTimeTestCases, testCreation);
	ADD_TEST_TO_SUITE(OW_DateTimeTestCases, testSetToCurrent);

	return testSuite;
}

