/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_DateTime.hpp"
#include "OW_String.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_Format.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

#include <time.h>
#ifdef OW_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

namespace OpenWBEM
{

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
OW_DEFINE_EXCEPTION(DateTime)

//////////////////////////////////////////////////////////////////////////////
DateTime::DateTime() 
	: m_time(0)
	, m_microseconds(0)

{
}
//////////////////////////////////////////////////////////////////////////////
namespace {
inline void badDateTime(const String& str)
{
	OW_THROW(DateTimeException, Format("Invalid DateTime: %1", str).c_str());
}

inline void validateRanges(Int32 year, Int32 month, Int32 day, Int32 hour, 
	Int32 minute, Int32 second, Int32 microseconds, const String& str)
{
	if (year < 0 || year > 9999 ||
		month < 1 || month > 12 ||
		day < 1 || day > 31 ||
		hour < 0 || hour > 23 ||
		minute < 0 || minute > 59 ||
		second < 0 || second > 60 ||
		microseconds < 0 || microseconds > 999999)
	{
		badDateTime(str);
	}
}

inline bool isDOWValid(const char* str)
{
	// a little FSM to validate the day of the week
	bool good = true;
	if (str[0] == 'S') // Sun, Sat
	{
		if (str[1] == 'u')
		{
			if (str[2] != 'n') // Sun
				good = false;
		}
		else if (str[1] ==  'a')
		{
			if (str[2] != 't') // Sat
				good = false;
		}
		else
				good = false;
	}
	else if (str[0] == 'M') // Mon
	{
		if (str[1] == 'o')
		{
			if (str[2] != 'n')
			{
				good = false;
			}
		}
		else
			good = false;
	}
	else if (str[0] == 'T') // Tue, Thu
	{
		if (str[1] == 'u')
		{
			if (str[2] != 'e') // Tue
				good = false;
		}
		else if (str[1] ==  'h')
		{
			if (str[2] != 'u') // Thu
				good = false;
		}
		else
				good = false;
	}
	else if (str[0] == 'W') // Wed
	{
		if (str[1] == 'e')
		{
			if (str[2] != 'd')
			{
				good = false;
			}
		}
		else
			good = false;
	}
	else if (str[0] == 'F') // Fri
	{
		if (str[1] == 'r')
		{
			if (str[2] != 'i')
			{
				good = false;
			}
		}
		else
			good = false;
	}
	else
		good = false;

	return good;
}

// returns -1 if the month is invalid, 1-12 otherwise
inline int decodeMonth(const char* str)
{
	// a little FSM to calculate the month
	if (str[0] == 'J') // Jan, Jun, Jul
	{
		if (str[1] == 'a')
		{
			if (str[2] == 'n') // Jan
				return 1;
		}
		else if (str[1] ==  'u')
		{
			if (str[2] == 'n') // Jun
				return 6;
			else if (str[2] == 'l') // Jul
				return 7;
		}
	}
	else if (str[0] == 'F') // Feb
	{
		if (str[1] == 'e' && str[2] == 'b')
			return 2;
	}
	else if (str[0] == 'M') // Mar, May
	{
		if (str[1] == 'a')
		{
			if (str[2] == 'r') // Mar
				return 3;
			else if (str[2] == 'y') // May
				return 5;
		}
	}
	else if (str[0] == 'A') // Apr, Aug
	{
		if (str[1] == 'p')
		{
			if (str[2] == 'r') // Apr
				return 4;
		}
		else if (str[1] == 'u')
		{
			if (str[2] == 'g') // Aug
				return 8;
		}
	}
	else if (str[0] == 'S') // Sep
	{
		if (str[1] == 'e' && str[2] == 'p')
			return 9;
	}
	else if (str[0] == 'O') // Oct
	{
		if (str[1] == 'c' && str[2] == 't')
			return 10;
	}
	else if (str[0] == 'N') // Nov
	{
		if (str[1] == 'o' && str[2] == 'v')
			return 11;
	}
	else if (str[0] == 'D') // Dec
	{
		if (str[1] == 'e' && str[2] == 'c')
			return 12;
	}

	return -1;
}

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
DateTime::DateTime(const String& str)
{
	// CIM DateTimes are 25 chars long, and ctime dates are 24 or 25 long
	if (str.length() < 24 || str.length() > 25)
	{
		badDateTime(str);
	}
	if (str[3] == ' ') // it's a ctime date
	{
		// validate required characters
		if (str[7] != ' ' || str[10] != ' ' || str[13] != ':' || str[16] != ':' || str[19] != ' ')
		{
			badDateTime(str);
		}
		if (!isDOWValid(str.c_str()))
		{
			badDateTime(str);
		}
		int month = decodeMonth(str.c_str() + 4);
		if (month == -1)
		{
			badDateTime(str);
		}
		try
		{
			Int32 day = str.substring(8, 2).toInt32();
			Int32 hour = str.substring(11, 2).toInt32();
			Int32 minute = str.substring(14, 2).toInt32();
			Int32 second = str.substring(17, 2).toInt32();
			Int32 year = str.substring(20, 4).toInt32();
			UInt32 microseconds = 0;
			validateRanges(year, month, day, hour, minute, second, microseconds, str);
			set(year, month, day, hour, minute, second, 
				microseconds, E_LOCAL_TIME);
		}
		catch (StringConversionException&)
		{
			badDateTime(str);
		}
	}
	else // it's a CIM DateTime string
	{
		// validate required characters
		if (str[14] != '.' || (str[21] != '+' && str[21] != '-'))
		{
			badDateTime(str);
		}

		try
		{
			// in CIM, "Fields which are not significant must be 
			// replaced with asterisk characters."  We'll convert
			// asterisks to 0s so we can process them.
			String strNoAsterisks(str);
			for (size_t i = 0; i < strNoAsterisks.length(); ++i)
			{
				if (strNoAsterisks[i] == '*')
					strNoAsterisks[i] = '0';
			}
			Int32 year = strNoAsterisks.substring(0, 4).toInt32();
			Int32 month = strNoAsterisks.substring(4, 2).toInt32();
			Int32 day = strNoAsterisks.substring(6, 2).toInt32();
			Int32 hour = strNoAsterisks.substring(8, 2).toInt32();
			Int32 minute = strNoAsterisks.substring(10, 2).toInt32();
			Int32 second = strNoAsterisks.substring(12, 2).toInt32();
			Int32 microseconds = strNoAsterisks.substring(15, 6).toInt32();

			validateRanges(year, month, day, hour, minute, second, microseconds, str);

			Int32 utc = strNoAsterisks.substring(22, 3).toInt32();
			// adjust the time to utc.  According to the CIM spec:
			// "utc is the offset from UTC in minutes"
			if (str[21] == '+')
			{
				utc = 0 - utc;
			}
			minute += utc;

			set(year, month, day, hour, minute, second, 
				microseconds, E_UTC_TIME);
			return;
		}
		catch (StringConversionException&)
		{
			badDateTime(str);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////									
DateTime::DateTime(time_t t, UInt32 microseconds) 
	: m_time(t)
	, m_microseconds(microseconds)
{
}
//////////////////////////////////////////////////////////////////////////////
DateTime::DateTime(int year, int month, int day, int hour, int minute,
	int second, UInt32 microseconds, ETimeOffset timeOffset)
{
	set(year, month, day, hour, minute, second, microseconds, timeOffset);
}
//////////////////////////////////////////////////////////////////////////////									
DateTime::~DateTime()
{
}
//////////////////////////////////////////////////////////////////////////////
inline tm
DateTime::getTm(ETimeOffset timeOffset) const
{
	if (timeOffset == E_LOCAL_TIME)
	{
#ifdef OW_HAVE_LOCALTIME_R
		tm theTime;
		localtime_r(&m_time, &theTime);
		return theTime;
#else
		tm* theTime = localtime(&m_time);
		return *theTime;
#endif
	}
	else // timeOffset == E_UTC_TIME
	{
#ifdef OW_HAVE_GMTIME_R
		tm theTime;
		gmtime_r(&m_time, &theTime);
		return theTime;
#else
		tm* theTime = gmtime(&m_time);
		return *theTime;
#endif
	}
}

//////////////////////////////////////////////////////////////////////////////
inline void
DateTime::setTime(tm& tmarg, ETimeOffset timeOffset)
{
	if (timeOffset == E_LOCAL_TIME)
	{
		m_time = ::mktime(&tmarg);
	}
	else // timeOffset == E_UTC_TIME
	{
		// timezone is a global that is set by mktime() which is "the 
		// difference, in seconds, between Coordinated Universal Time 
		// (UTC) and local standard time."
		m_time = ::mktime(&tmarg) - ::timezone;
	}
}
//////////////////////////////////////////////////////////////////////////////									
int
DateTime::getHour(ETimeOffset timeOffset) const
{
	return getTm(timeOffset).tm_hour;
}
//////////////////////////////////////////////////////////////////////////////									
int
DateTime::getMinute(ETimeOffset timeOffset) const
{
	return getTm(timeOffset).tm_min;
}
//////////////////////////////////////////////////////////////////////////////									
int
DateTime::getSecond(ETimeOffset timeOffset) const
{
	return getTm(timeOffset).tm_sec;
}
//////////////////////////////////////////////////////////////////////////////									
UInt32
DateTime::getMicrosecond() const
{
	return m_microseconds;
}
//////////////////////////////////////////////////////////////////////////////									
int
DateTime::getDay(ETimeOffset timeOffset) const
{
	return getTm(timeOffset).tm_mday;
}
//////////////////////////////////////////////////////////////////////////////
int
DateTime::getDow(ETimeOffset timeOffset) const
{
	return getTm(timeOffset).tm_wday;
}
//////////////////////////////////////////////////////////////////////////////									
int
DateTime::getMonth(ETimeOffset timeOffset) const
{
	return getTm(timeOffset).tm_mon+1;
}
//////////////////////////////////////////////////////////////////////////////									
int
DateTime::getYear(ETimeOffset timeOffset) const
{
	return (getTm(timeOffset).tm_year + 1900);
}
//////////////////////////////////////////////////////////////////////////////
time_t
DateTime::get() const
{
	return m_time;
}
//////////////////////////////////////////////////////////////////////////////									
void
DateTime::setHour(int hour, ETimeOffset timeOffset)
{
	tm theTime = getTm(timeOffset);
	theTime.tm_hour = hour;
	setTime(theTime, timeOffset);
}
//////////////////////////////////////////////////////////////////////////////									
void
DateTime::setMinute(int minute, ETimeOffset timeOffset)
{
	tm theTime = getTm(timeOffset);
	theTime.tm_min = minute;
	setTime(theTime, timeOffset);
}
//////////////////////////////////////////////////////////////////////////////									
void
DateTime::setSecond(int second, ETimeOffset timeOffset)
{
	tm theTime = getTm(timeOffset);
	theTime.tm_sec = second;
	setTime(theTime, timeOffset);
}
//////////////////////////////////////////////////////////////////////////////									
void
DateTime::setMicrosecond(UInt32 microseconds)
{
	m_microseconds = microseconds;
}
//////////////////////////////////////////////////////////////////////////////
void
DateTime::setTime(int hour, int minute, int second, ETimeOffset timeOffset)
{
	tm theTime = getTm(timeOffset);
	theTime.tm_hour = hour;
	theTime.tm_min = minute;
	theTime.tm_sec = second;
	setTime(theTime, timeOffset);
}
//////////////////////////////////////////////////////////////////////////////									
void
DateTime::setDay(int day, ETimeOffset timeOffset)
{
	tm theTime = getTm(timeOffset);
	theTime.tm_mday = day;
	setTime(theTime, timeOffset);
}
//////////////////////////////////////////////////////////////////////////////									
void
DateTime::setMonth(int month, ETimeOffset timeOffset)
{
	tm theTime = getTm(timeOffset);
	theTime.tm_mon = month-1;
	setTime(theTime, timeOffset);
}
//////////////////////////////////////////////////////////////////////////////									
void
DateTime::setYear(int year, ETimeOffset timeOffset)
{
	tm theTime = getTm(timeOffset);
	theTime.tm_year = year - 1900;
	setTime(theTime, timeOffset);
}
//////////////////////////////////////////////////////////////////////////////
void
DateTime::set(int year, int month, int day, int hour, int minute, int second,
	UInt32 microseconds, ETimeOffset timeOffset)
{
	tm tmarg;
	tmarg.tm_year = (year >= 1900) ? year - 1900 : year;
	tmarg.tm_mon = month-1;
	tmarg.tm_mday = day;
	tmarg.tm_hour = hour;
	tmarg.tm_min = minute;
	tmarg.tm_sec = second;
	if (timeOffset == E_UTC_TIME)
	{
		tmarg.tm_isdst = 0; // don't want dst applied to utc time!
	}
	else
	{
		tmarg.tm_isdst = -1; // don't know about daylight savings time
	}
	setTime(tmarg, timeOffset);
	m_microseconds = microseconds;
}
//////////////////////////////////////////////////////////////////////////////
void
DateTime::setToCurrent()
{
#ifdef OW_HAVE_GETTIMEOFDAY
	timeval tv;
	gettimeofday(&tv, NULL);
	m_time = tv.tv_sec;
	m_microseconds = tv.tv_usec;
#else
	m_time = time(NULL);
	m_microseconds = 0;
#endif
}
//////////////////////////////////////////////////////////////////////////////
void
DateTime::addDays(int days)
{
	tm theTime = getTm(E_UTC_TIME);
	theTime.tm_mday += days;
	setTime(theTime, E_UTC_TIME);
}
//////////////////////////////////////////////////////////////////////////////
void
DateTime::addYears(int years)
{
	tm theTime = getTm(E_UTC_TIME);
	theTime.tm_year += years;
	setTime(theTime, E_UTC_TIME);
}
//////////////////////////////////////////////////////////////////////////////
void
DateTime::addMonths(int months)
{
	tm theTime = getTm(E_UTC_TIME);
	theTime.tm_mon += months;
	setTime(theTime, E_UTC_TIME);
}
//////////////////////////////////////////////////////////////////////////////									
String
DateTime::toString(ETimeOffset timeOffset) const
{
	tm theTime = getTm(timeOffset);
#ifdef OW_HAVE_ASCTIME_R
	char buff[30];
	asctime_r(&theTime, buff);
	String s(buff);
	return s;
#else
	// if the c library isn't thread-safe, we'll need a mutex here.
	return asctime(&theTime);
#endif
}
//////////////////////////////////////////////////////////////////////////////
String
DateTime::toStringGMT() const 
{ 
	return toString(E_UTC_TIME); 
}

} // end namespace OpenWBEM

