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

#include "OW_config.h"
#include "OW_CIMDateTime.hpp"
#include "OW_DateTime.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_MutexLock.hpp"
#include "OW_String.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_MutexLock.hpp"
#include "OW_BinIfcIO.hpp"

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

using std::ostream;
using std::istream;

static void fillDateTimeData(OW_DateTimeData& data, const char* str);
static OW_Int16 getGMTOffset();

//////////////////////////////////////////////////////////////////////////////
int
OW_DateTimeData::compare(const OW_DateTimeData& arg)
{
	return ::memcmp(this, &arg, sizeof(*this));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime::OW_CIMDateTime(OW_Bool notNull)
	: m_dptr(notNull ? new OW_DateTimeData : 0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime::OW_CIMDateTime(const OW_CIMDateTime& arg)
	: m_dptr(arg.m_dptr)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime::OW_CIMDateTime(const OW_String& arg) :
	m_dptr(new OW_DateTimeData)
{
	fillDateTimeData(*m_dptr, arg.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime::OW_CIMDateTime(const OW_DateTime& arg) :
	m_dptr(new OW_DateTimeData)
{
	m_dptr->m_days = arg.getDay();
	m_dptr->m_year = arg.getYear();
	m_dptr->m_month = arg.getMonth();
	m_dptr->m_hours = arg.getHour();
	m_dptr->m_minutes = arg.getMinute();
	m_dptr->m_seconds = arg.getSecond();
	m_dptr->m_isInterval = arg.isInterval();
	m_dptr->m_microSeconds = 0;
	m_dptr->m_utc = getGMTOffset();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime::OW_CIMDateTime(OW_UInt64 microSeconds) :
	m_dptr(new OW_DateTimeData)
{
	OW_UInt32 secs = microSeconds / 1000000ULL;
	microSeconds -= secs * 1000000;
	OW_UInt32 minutes = secs / 60;
	secs -= minutes * 60;
	OW_UInt32 hours = minutes / 60;
	minutes -= hours * 60;
	OW_UInt32 days = hours / 24;
	hours -= days * 24;

	m_dptr->m_days = days;
	m_dptr->m_hours = hours;
	m_dptr->m_minutes = minutes;
	m_dptr->m_seconds = secs;
	m_dptr->m_microSeconds = microSeconds;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime&
OW_CIMDateTime::operator= (const OW_CIMDateTime& arg)
{
	m_dptr = arg.m_dptr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime::operator void*() const
{
	if(!m_dptr.isNull())
	{
		return (void*)(m_dptr->m_days != 0
			|| m_dptr->m_year != 0
			|| m_dptr->m_month != 0
			|| m_dptr->m_hours != 0
			|| m_dptr->m_minutes != 0
			|| m_dptr->m_seconds != 0
			|| m_dptr->m_microSeconds != 0);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setYear(OW_UInt16 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_year = arg;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setMonth(OW_UInt8 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_month = arg;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setDays(OW_UInt32 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_days = arg;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setDay(OW_UInt32 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_days = arg;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setHours(OW_UInt8 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_hours = arg;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setMinutes(OW_UInt8 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_minutes = arg;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setSeconds(OW_UInt8 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_seconds = arg;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setMicroSeconds(OW_UInt32 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_microSeconds = arg;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::setUtc(OW_Int16 arg)
{
	OW_MutexLock l = m_dptr.getWriteLock();
	m_dptr->m_utc = arg;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMDateTime::equal(const OW_CIMDateTime& arg) const
{
	return (::memcmp(arg.m_dptr.getPtr(), m_dptr.getPtr(),
		sizeof(OW_DateTimeData)) == 0);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::readObject(istream &istrm)
{
	OW_DateTimeData dtdata;
	OW_BinIfcIO::read(istrm, &dtdata, sizeof(dtdata));

	dtdata.m_year = OW_ntoh16(dtdata.m_year);
	dtdata.m_days = OW_ntoh32(dtdata.m_days);
	dtdata.m_microSeconds = OW_ntoh32(dtdata.m_microSeconds);
	dtdata.m_utc = OW_ntoh16(dtdata.m_utc);

	if(m_dptr.isNull())
	{
		m_dptr = new OW_DateTimeData;
	}

	OW_MutexLock l = m_dptr.getWriteLock();
	*m_dptr = dtdata;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDateTime::writeObject(ostream &ostrm) const
{
	OW_DateTimeData dtdata;

	memmove(&dtdata, m_dptr.getPtr(), sizeof(dtdata));
	dtdata.m_year = OW_hton16(dtdata.m_year);
	dtdata.m_days = OW_hton32(dtdata.m_days);
	dtdata.m_microSeconds = OW_hton32(dtdata.m_microSeconds);
	dtdata.m_utc = OW_hton16(dtdata.m_utc);

	OW_BinIfcIO::write(ostrm, &dtdata, sizeof(dtdata));
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMDateTime::toString() const
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

	return OW_String(bfr);
}

//////////////////////////////////////////////////////////////////////////////
static void
fillDateTimeData(OW_DateTimeData& data, const char* str)
{
	::memset(&data, 0, sizeof(OW_DateTimeData));
	if(str == NULL || *str == (char)0)
		return;

	char bfr[35];
	::strncpy(bfr, str, sizeof(bfr));
	bfr[34] = (char)0;

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
		OW_THROW(OW_CIMDateTimeException, "Invalid format for date time");
	}
}

//////////////////////////////////////////////////////////////////////////////
ostream&
operator<< (ostream& ostr, const OW_CIMDateTime& arg)
{
	ostr << arg.toString();
	return ostr;
}

//////////////////////////////////////////////////////////////////////////////
static OW_Int16
getGMTOffset()
{
	static OW_Int16 gmtOffset = 0;
	static bool offsetComputed = false;
	static OW_Mutex tzmutex;

	OW_MutexLock ml(tzmutex);
	if(!offsetComputed)
	{
		time_t tm = time(NULL);
		time_t gmt = mktime(gmtime(&tm));
		time_t lctm = mktime(localtime(&tm));

		gmtOffset = ((lctm - gmt) / 60) / 60;
		offsetComputed = true;
	}

	return gmtOffset;
}


