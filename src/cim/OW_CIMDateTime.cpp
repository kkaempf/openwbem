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
#include "OW_CIMDateTime.hpp"
#include "OW_DateTime.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_MutexLock.hpp"
#include "OW_String.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include "OW_ThreadImpl.hpp"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>
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
static void fillDateTimeData(CIMDateTime::DateTimeData& data, const char* str);
static Int16 getGMTOffset();
//////////////////////////////////////////////////////////////////////////////
int
CIMDateTime::DateTimeData::compare(const CIMDateTime::DateTimeData& arg)
{
	return ::memcmp(this, &arg, sizeof(*this));
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMDateTime::DateTimeData& x, const CIMDateTime::DateTimeData& y)
{
	return StrictWeakOrdering(
		x.m_year, y.m_year,
		x.m_month, y.m_month,
		x.m_days, y.m_days,
		x.m_hours, y.m_hours,
		x.m_minutes, y.m_minutes,
		x.m_seconds, y.m_seconds,
		x.m_utc, y.m_utc,
		x.m_isInterval, y.m_isInterval);
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime()
	: m_dptr(new DateTimeData)
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
	m_dptr->m_days = arg.getDay();
	m_dptr->m_year = arg.getYear();
	m_dptr->m_month = arg.getMonth();
	m_dptr->m_hours = arg.getHour();
	m_dptr->m_minutes = arg.getMinute();
	m_dptr->m_seconds = arg.getSecond();
	m_dptr->m_isInterval = 0;
	m_dptr->m_microSeconds = arg.getMicrosecond();
	m_dptr->m_utc = getGMTOffset() * 60;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(UInt64 microSeconds) :
	m_dptr(new DateTimeData)
{
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
	if(!m_dptr.isNull())
	{
		return (m_dptr->m_days != 0
			|| m_dptr->m_year != 0
			|| m_dptr->m_month != 0
			|| m_dptr->m_hours != 0
			|| m_dptr->m_minutes != 0
			|| m_dptr->m_seconds != 0
			|| m_dptr->m_microSeconds != 0) ?
			&dummy::nonnull : 0;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::safe_bool
CIMDateTime::operator !() const
{
	if(!m_dptr.isNull())
	{
		return (m_dptr->m_days != 0
			|| m_dptr->m_year != 0
			|| m_dptr->m_month != 0
			|| m_dptr->m_hours != 0
			|| m_dptr->m_minutes != 0
			|| m_dptr->m_seconds != 0
			|| m_dptr->m_microSeconds != 0) ?
			0 : &dummy::nonnull;
	}
	return &dummy::nonnull;
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
	return (::memcmp(arg.m_dptr.getPtr(), m_dptr.getPtr(),
		sizeof(DateTimeData)) == 0);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDateTime::readObject(istream &istrm)
{
	DateTimeData dtdata;
	BinarySerialization::read(istrm, &dtdata, sizeof(dtdata));
	dtdata.m_year = ntoh16(dtdata.m_year);
	dtdata.m_days = ntoh32(dtdata.m_days);
	dtdata.m_microSeconds = ntoh32(dtdata.m_microSeconds);
	dtdata.m_utc = ntoh16(dtdata.m_utc);
	if(m_dptr.isNull())
	{
		m_dptr = new DateTimeData;
	}
	*m_dptr = dtdata;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDateTime::writeObject(ostream &ostrm) const
{
	DateTimeData dtdata;
	memmove(&dtdata, m_dptr.getPtr(), sizeof(dtdata));
	dtdata.m_year = hton16(dtdata.m_year);
	dtdata.m_days = hton32(dtdata.m_days);
	dtdata.m_microSeconds = hton32(dtdata.m_microSeconds);
	dtdata.m_utc = hton16(dtdata.m_utc);
	BinarySerialization::write(ostrm, &dtdata, sizeof(dtdata));
}
//////////////////////////////////////////////////////////////////////////////
String
CIMDateTime::toString() const
{
	char bfr[30];
	if(isInterval())
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
	::memset(&data, 0, sizeof(CIMDateTime::DateTimeData));
	if(str == NULL || *str == '\0')
		return;
	char bfr[35];
	::strncpy(bfr, str, sizeof(bfr));
	bfr[34] = '\0';
	if(bfr[21] == ':')	// Is this an interval
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
	else if(bfr[21] == '+' || bfr[21] == '-')
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
		OW_THROW(CIMDateTimeException, "Invalid format for date time");
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
namespace {
Int16 gmtOffset = 0;
bool offsetComputed = false;
Mutex tzmutex;
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
static Int16
getGMTOffset()
{
	ThreadImpl::memoryBarrier();
	if(!offsetComputed)
	{
		// double-checked locking
		MutexLock ml(tzmutex);
		if (!offsetComputed)
		{
			time_t tm = time(NULL);
			time_t gmt = mktime(gmtime(&tm));
			time_t lctm = mktime(localtime(&tm));
			gmtOffset = ((lctm - gmt) / 60) / 60;
			offsetComputed = true;
		}
	}
	return gmtOffset;
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMDateTime& x, const CIMDateTime& y)
{
	return *x.m_dptr < *y.m_dptr;
}

} // end namespace OpenWBEM

