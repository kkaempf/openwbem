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
#include "OW_BinIfcIO.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif


using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////									
OW_DateTime::OW_DateTime() :
	m_time(0), m_isInterval(true)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_DateTime::OW_DateTime(const OW_String& str) :
	m_time(0), m_isInterval(false)
{
	int ndx = str.indexOf(':');	// Check for interval
	if(ndx != -1)
	{
		m_isInterval = true;
	}
}

//////////////////////////////////////////////////////////////////////////////									
OW_DateTime::OW_DateTime(time_t t) :
	m_time(t), m_isInterval(false)
{
}

//////////////////////////////////////////////////////////////////////////////									
OW_DateTime::OW_DateTime(const OW_DateTime& t) :
	m_time(t.m_time), m_isInterval(false)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_DateTime::OW_DateTime(int year, int month, int day, int hour, int minute,
	int second) :
	m_time(0), m_isInterval(false)
{
	set(year, month, day, hour, minute, second);
}

//////////////////////////////////////////////////////////////////////////////									
OW_DateTime::~OW_DateTime()
{
}

//////////////////////////////////////////////////////////////////////////////									
int
OW_DateTime::getHour() const
{
	return getTm().tm_hour;
}

//////////////////////////////////////////////////////////////////////////////									
int
OW_DateTime::getMinute() const
{
	return getTm().tm_min;
}

//////////////////////////////////////////////////////////////////////////////									
int
OW_DateTime::getSecond() const
{
	return getTm().tm_sec;
}

//////////////////////////////////////////////////////////////////////////////									
int
OW_DateTime::getDay() const
{
	return getTm().tm_mday;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_DateTime::getDow() const
{
	return getTm().tm_wday;
}

//////////////////////////////////////////////////////////////////////////////									
int
OW_DateTime::getMonth() const
{
	return getTm().tm_mon+1;
}

//////////////////////////////////////////////////////////////////////////////									
int
OW_DateTime::getYear() const
{
	return (getTm().tm_year + 1900);
}

//////////////////////////////////////////////////////////////////////////////
time_t
OW_DateTime::get() const
{
	return m_time;
}

//////////////////////////////////////////////////////////////////////////////									
void
OW_DateTime::setHour(int hour)
{
	tm theTime = getTm();
	theTime.tm_hour = hour;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////									
void
OW_DateTime::setMinute(int minute)
{
	tm theTime = getTm();
	theTime.tm_min = minute;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////									
void
OW_DateTime::setSecond(int second)
{
	tm theTime = getTm();
	theTime.tm_sec = second;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DateTime::setTime(int hour, int minute, int second)
{
	tm theTime = getTm();
	theTime.tm_hour = hour;
	theTime.tm_min = minute;
	theTime.tm_sec = second;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////									
void
OW_DateTime::setDay(int day)
{
	tm theTime = getTm();
	theTime.tm_mday = day;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////									
void
OW_DateTime::setMonth(int month)
{
	tm theTime = getTm();
	theTime.tm_mon = month-1;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////									
void
OW_DateTime::setYear(int year)
{
	tm theTime = getTm();
	theTime.tm_year = year - 1900;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DateTime::set(int year, int month, int day, int hour, int minute, int second)
{
	tm tmarg;
	tmarg.tm_year = (year >= 1900) ? year - 1900 : year;
	tmarg.tm_mon = month-1;
	tmarg.tm_mday = day;
	tmarg.tm_hour = hour;
	tmarg.tm_min = minute;
	tmarg.tm_sec = second;
	m_time = mktime(&tmarg);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DateTime::setToCurrent()
{
	m_time = time(NULL);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DateTime::addDays(int days)
{
	tm theTime = getTm();
	theTime.tm_mday += days;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DateTime::addYears(int years)
{
	tm theTime = getTm();
	theTime.tm_year += years;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DateTime::addMonths(int months)
{
	tm theTime = getTm();
	theTime.tm_mon += months;
	m_time = mktime(&theTime);
}

//////////////////////////////////////////////////////////////////////////////									
OW_String
OW_DateTime::toString() const
{
	tm theTime = getTm();
	char buff[30];
	asctime_r(&theTime, buff);
	OW_String s(buff);
	return s;
}

//////////////////////////////////////////////////////////////////////////////

OW_String
OW_DateTime::toStringGMT() const
{
	tm theTime = getTm();
	gmtime_r(&m_time, &theTime);
	char buff[30];
	asctime_r(&theTime, buff);
	OW_String s(buff);
	return s;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DateTime::writeObject(ostream &ostrm) const /*throw (OW_IOException)*/
{
	OW_Int32 v = OW_hton32(m_time);
	OW_BinIfcIO::write(ostrm, &v, sizeof(v));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DateTime::readObject(istream &istrm) /*throw (OW_IOException)*/
{
	OW_BinIfcIO::read(istrm, &m_time, sizeof(m_time));
	m_time = OW_ntoh32(m_time);
}

//////////////////////////////////////////////////////////////////////////////
tm
OW_DateTime::getTm() const
{
	tm theTime;
	localtime_r(&m_time, &theTime);
	return theTime;
}
