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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMDateTime.hpp"
#include "OW_DateTime.hpp"
#include "OW_String.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include "OW_COWIntrusiveCountableBase.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_Assertion.hpp"

#include <cstdio>
#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

using std::ostream;
using std::istream;

OW_DEFINE_EXCEPTION_WITH_ID(CIMDateTime);

//////////////////////////////////////////////////////////////////////////////
struct CIMDateTime::DateTimeData : public COWIntrusiveCountableBase
{
	DateTimeData() :
		m_year(0), m_month(0), m_days(0), m_hours(0),
		m_minutes(0), m_seconds(0), m_microSeconds(0), m_utc(0),
		m_isInterval(1) {}

	UInt16 m_year;
	UInt8 m_month;
	UInt32 m_days;
	UInt8 m_hours;
	UInt8 m_minutes;
	UInt8 m_seconds;
	UInt32 m_microSeconds;
	Int16 m_utc;
	UInt8 m_isInterval;
	DateTimeData* clone() const { return new DateTimeData(*this); }
};


static void fillDateTimeData(CIMDateTime::DateTimeData& data, const char* str);
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime()
	: m_dptr(new DateTimeData)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::~CIMDateTime()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(CIMNULL_t)
	: m_dptr(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(const CIMDateTime& arg)
	: m_dptr(arg.m_dptr)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(const String& arg) :
	m_dptr(new DateTimeData)
{
	fillDateTimeData(*m_dptr, arg.c_str());
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(const DateTime& arg) :
	m_dptr(new DateTimeData)
{
	struct tm t_loc;
	m_dptr->m_isInterval = 0;
	m_dptr->m_utc = arg.toLocal(t_loc);
	m_dptr->m_year = t_loc.tm_year + 1900;
	m_dptr->m_month = t_loc.tm_mon + 1;
	m_dptr->m_days = t_loc.tm_mday;
	m_dptr->m_hours = t_loc.tm_hour;
	m_dptr->m_minutes = t_loc.tm_min;
	m_dptr->m_seconds = t_loc.tm_sec;
	m_dptr->m_microSeconds = arg.getMicrosecond();
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(UInt64 microSeconds) :
	m_dptr(new DateTimeData)
{
	m_dptr->m_isInterval = 1;
	UInt32 secs = microSeconds / 1000000ULL;
	microSeconds -= secs * 1000000;
	UInt32 minutes = secs / 60;
	secs -= minutes * 60;
	UInt32 hours = minutes / 60;
	minutes -= hours * 60;
	UInt32 days = hours / 24;
	hours -= days * 24;
	m_dptr->m_days = days;
	m_dptr->m_hours = hours;
	m_dptr->m_minutes = minutes;
	m_dptr->m_seconds = secs;
	m_dptr->m_microSeconds = microSeconds;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::operator= (const CIMDateTime& arg)
{
	m_dptr = arg.m_dptr;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::operator CIMDateTime::safe_bool() const
{
	if (m_dptr)
	{
		return (m_dptr->m_days != 0
			|| m_dptr->m_year != 0
			|| m_dptr->m_month != 0
			|| m_dptr->m_hours != 0
			|| m_dptr->m_minutes != 0
			|| m_dptr->m_seconds != 0
			|| m_dptr->m_microSeconds != 0) ?
			&CIMDateTime::m_dptr : 0;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDateTime::operator !() const
{
	if (m_dptr)
	{
		return (m_dptr->m_days == 0
			&& m_dptr->m_year == 0
			&& m_dptr->m_month == 0
			&& m_dptr->m_hours == 0
			&& m_dptr->m_minutes == 0
			&& m_dptr->m_seconds == 0
			&& m_dptr->m_microSeconds == 0);
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setYear(UInt16 arg)
{
	m_dptr->m_year = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setMonth(UInt8 arg)
{
	m_dptr->m_month = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setDays(UInt32 arg)
{
	m_dptr->m_days = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setDay(UInt32 arg)
{
	m_dptr->m_days = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setHours(UInt8 arg)
{
	m_dptr->m_hours = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setMinutes(UInt8 arg)
{
	m_dptr->m_minutes = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setSeconds(UInt8 arg)
{
	m_dptr->m_seconds = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setMicroSeconds(UInt32 arg)
{
	m_dptr->m_microSeconds = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setUtc(Int16 arg)
{
	m_dptr->m_utc = arg;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDateTime::equal(const CIMDateTime& arg) const
{
	return (
		m_dptr->m_year == arg.m_dptr->m_year &&
		m_dptr->m_month == arg.m_dptr->m_month &&
		m_dptr->m_days == arg.m_dptr->m_days &&
		m_dptr->m_hours == arg.m_dptr->m_hours &&
		m_dptr->m_minutes == arg.m_dptr->m_minutes &&
		m_dptr->m_seconds == arg.m_dptr->m_seconds &&
		m_dptr->m_microSeconds == arg.m_dptr->m_microSeconds &&
		m_dptr->m_utc == arg.m_dptr->m_utc &&
		m_dptr->m_isInterval == arg.m_dptr->m_isInterval);
	
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDateTime::readObject(istream &istrm)
{
	DateTimeData dtdata;

	BinarySerialization::read(istrm, dtdata.m_year);
	BinarySerialization::read(istrm, dtdata.m_month);
	BinarySerialization::read(istrm, dtdata.m_days);
	BinarySerialization::read(istrm, dtdata.m_hours);
	BinarySerialization::read(istrm, dtdata.m_minutes);
	BinarySerialization::read(istrm, dtdata.m_seconds);
	BinarySerialization::read(istrm, dtdata.m_microSeconds);
	BinarySerialization::read(istrm, dtdata.m_utc);
	BinarySerialization::read(istrm, dtdata.m_isInterval);

	if (!m_dptr)
	{
		m_dptr = new DateTimeData;
	}
	*m_dptr = dtdata;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDateTime::writeObject(ostream &ostrm) const
{
	BinarySerialization::write(ostrm, m_dptr->m_year);
	BinarySerialization::write(ostrm, m_dptr->m_month);
	BinarySerialization::write(ostrm, m_dptr->m_days);
	BinarySerialization::write(ostrm, m_dptr->m_hours);
	BinarySerialization::write(ostrm, m_dptr->m_minutes);
	BinarySerialization::write(ostrm, m_dptr->m_seconds);
	BinarySerialization::write(ostrm, m_dptr->m_microSeconds);
	BinarySerialization::write(ostrm, m_dptr->m_utc);
	BinarySerialization::write(ostrm, m_dptr->m_isInterval);

}
//////////////////////////////////////////////////////////////////////////////
String
CIMDateTime::toString() const
{
	char bfr[30];
	if (isInterval())
	{
		// Interval format
		::sprintf(bfr, "%08u%02u%02u%02u.%06u:000", m_dptr->m_days,
			m_dptr->m_hours, m_dptr->m_minutes, m_dptr->m_seconds,
			m_dptr->m_microSeconds);
	}
	else
	{
		// Date/Time format
		::sprintf(bfr, "%04u%02u%02u%02u%02u%02u.%06u%+04d", m_dptr->m_year,
			m_dptr->m_month, m_dptr->m_days, m_dptr->m_hours, m_dptr->m_minutes,
			m_dptr->m_seconds, m_dptr->m_microSeconds, m_dptr->m_utc);
	}
	return String(bfr);
}
//////////////////////////////////////////////////////////////////////////////
DateTime
CIMDateTime::toDateTime() const
{
	return DateTime(toString());
}
//////////////////////////////////////////////////////////////////////////////
static void
fillDateTimeData(CIMDateTime::DateTimeData& data, const char* str)
{
	if (str == NULL || *str == '\0')
	{
		return;
	}
	char bfr[35];
	::strncpy(bfr, str, sizeof(bfr));
	bfr[34] = '\0';
	if (bfr[21] == ':')	// Is this an interval
	{
		// ddddddddhhmmss.mmmmmm:000
		data.m_isInterval = 1;
		bfr[21] = 0;
		data.m_microSeconds = atoi(&bfr[15]);
		bfr[14] = 0;
		data.m_seconds = atoi(&bfr[12]);
		bfr[12] = 0;
		data.m_minutes = atoi(&bfr[10]);
		bfr[10] = 0;
		data.m_hours = atoi(&bfr[8]);
		bfr[8] = 0;
		data.m_days = atoi(bfr);
	}
	else if (bfr[21] == '+' || bfr[21] == '-')
	{
		// yyyymmddhhmmss.mmmmmmsutc
		data.m_isInterval = 0;
		data.m_utc = atoi(&bfr[21]);
		bfr[21] = 0;
		data.m_microSeconds = atoi(&bfr[15]);
		bfr[14] = 0;
		data.m_seconds = atoi(&bfr[12]);
		bfr[12] = 0;
		data.m_minutes = atoi(&bfr[10]);
		bfr[10] = 0;
		data.m_hours = atoi(&bfr[8]);
		bfr[8] = 0;
		data.m_days = atoi(&bfr[6]);
		bfr[6] = 0;
		data.m_month = atoi(&bfr[4]);
		bfr[4] = 0;
		data.m_year = atoi(bfr);
	}
	else
	{
		OW_THROW_ERR(CIMDateTimeException, "Invalid format for date time", CIMDateTime::E_INVALID_DATE_TIME_FORMAT);
	}
}
//////////////////////////////////////////////////////////////////////////////
ostream&
operator<< (ostream& ostr, const CIMDateTime& arg)
{
	ostr << arg.toString();
	return ostr;
}

//////////////////////////////////////////////////////////////////////////////
UInt16
CIMDateTime::getYear() const {  return m_dptr->m_year; }
//////////////////////////////////////////////////////////////////////////////
UInt8
CIMDateTime::getMonth() const {  return m_dptr->m_month; }
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMDateTime::getDays() const {  return m_dptr->m_days; }
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMDateTime::getDay() const {  return m_dptr->m_days; }
//////////////////////////////////////////////////////////////////////////////
UInt8
CIMDateTime::getHours() const {  return m_dptr->m_hours; }
//////////////////////////////////////////////////////////////////////////////
UInt8
CIMDateTime::getMinutes() const {  return m_dptr->m_minutes; }
//////////////////////////////////////////////////////////////////////////////
UInt8
CIMDateTime::getSeconds() const {  return m_dptr->m_seconds; }
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMDateTime::getMicroSeconds() const {  return m_dptr->m_microSeconds; }
//////////////////////////////////////////////////////////////////////////////
Int16
CIMDateTime::getUtc() const {  return m_dptr->m_utc; }
//////////////////////////////////////////////////////////////////////////////
bool
CIMDateTime::isInterval() const {  return m_dptr->m_isInterval != 0;}
//////////////////////////////////////////////////////////////////////////////
void
CIMDateTime::setInterval(bool val) { m_dptr->m_isInterval = val; }

//////////////////////////////////////////////////////////////////////////////
bool operator==(const CIMDateTime& x, const CIMDateTime& y)
{
	return x.equal(y);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{

	// Nonzero if YEAR is a leap year (every 4 years, except every 100th isn't, and every 400th is).
	inline bool isLeap(UInt16 year)
	{
		return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
	}

	// How many days come before each month (0-12).
	const unsigned short int monthYearDay[2][13] =
	{
		// Normal years.
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
		// Leap years.
		{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
	};

	// This doesn't return the exact number of seconds, since it assumes all previous years are 366 days long,
	// In essense we're introducing some empty space in the range of this function, but that's good enough to
	// use in comparing dates.
	Int64 getMagnitude(const CIMDateTime& dt)
	{
		OW_ASSERT(!dt.isInterval());

		// only check the month, we don't check the rest since it won't cause a crash...
		if (dt.getMonth() > 12)
		{
			return 0; // invalid month would cause an access violation.
		}

		const int EPOCH_YEAR = 1970;

		int dayOfYear = monthYearDay[isLeap(dt.getYear())][dt.getMonth() - 1] + dt.getDay() - 1;
		int days = 366 * (dt.getYear() - EPOCH_YEAR) + dayOfYear; // leap years are accounted for by 366 instead of 365.
		return dt.getSeconds()
			+ 60 * (dt.getMinutes() + dt.getUtc())
			+ 3600 * static_cast<Int64>(dt.getHours() + 24 * days);
	}
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMDateTime& x, const CIMDateTime& y)
{
	// see if they both the same type (intervals or date/times) or are different types.
	if (x.isInterval() ^ y.isInterval())
	{
		// they're different.  We define an interval to be < a date/time
		return x.isInterval();
	}
	else
	{
		if (x.isInterval())
		{
			// both intervals
			return StrictWeakOrdering(
				x.getDays(), y.getDays(),
				x.getHours(), y.getHours(),
				x.getMinutes(), y.getMinutes(),
				x.getSeconds(), y.getSeconds(),
				x.getMicroSeconds(), y.getMicroSeconds());
		}
		else
		{
			// they're both date/times
			return StrictWeakOrdering(
				getMagnitude(x), getMagnitude(y),
				x.getMicroSeconds(), y.getMicroSeconds());
		}
	}

}

//////////////////////////////////////////////////////////////////////////////
bool operator!=(const CIMDateTime& x, const CIMDateTime& y)
{
	return !(x == y);
}

//////////////////////////////////////////////////////////////////////////////
bool operator>(const CIMDateTime& x, const CIMDateTime& y)
{
	return y < x;
}

//////////////////////////////////////////////////////////////////////////////
bool operator<=(const CIMDateTime& x, const CIMDateTime& y)
{
	return !(y < x);
}

//////////////////////////////////////////////////////////////////////////////
bool operator>=(const CIMDateTime& x, const CIMDateTime& y)
{
	return !(x < y);
}

} // end namespace OpenWBEM

