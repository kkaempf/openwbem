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

#ifndef OW_DATETIME_HPP_INCLUDE_GUARD_
#define OW_DATETIME_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_Types.hpp"

extern "C"
{
#include <time.h>
}

namespace OpenWBEM
{

class String;

OW_DECLARE_APIEXCEPTION(DateTime, OW_COMMON_API)

/**
 * The DateTime class is an abstraction for date time data.
 * It allows easy date/time adjustment, date/time arithmetic and
 * comparison.
 * All functions which operate on a time_t are based on seconds since the Epoch,
 * and the timezone is irrelevant.
 * DateTime cannot represent dates before the Epoch 
 * (00:00:00 UTC, January 1, 1970), and the upper limit is system dependent.
 * On a system where time_t is 32-bits, the latest date that can be
 * represented is Mon Jan 18 20:14:07 2038.  If time_t is 64-bits, then it's
 * a date so far in the future, that the sun will probably turn into a black
 * hole before then. (About 500 billion years.)
 * If an invalid time is created, then get() will return time_t(-1), and the
 * DateTime represented will be 1 second before the Epoch (23:59:59 UTC, 
 * December 31, 1969)
 *
 * If the non-standard function timegm() is available, it will be used, 
 * otherwise this class' implementation relies on the global variable timezone 
 * (from time.h) which reflects the system's timezone.
 */
class OW_COMMON_API DateTime
{
public:
	/**
	 * The values of this enum are passed to various functions to as a
	 * flag to indicate the timezone context desired.
	 * E_LOCAL_TIME means the operation will apply the local system
	 *  timezone (which may include an offset for daylight saving time).
	 * E_UTC_TIME means the operation will apply to UTC (or GMT) time.
	 */
	enum ETimeOffset
	{
		E_LOCAL_TIME,
		E_UTC_TIME
	};

	/**
	 * Create a new DateTime object that represents the Epoch 
	 * (00:00:00 UTC, January 1, 1970)
	 */
	DateTime();
	/**
	 * Create a new DateTime object from a string that contains one of:
	 * (1) a CIM DateTime formated string (e.g. "19980525133015.000000-300")
	 * (2) a ctime() formatted string (e.g. "Wed Jun 30 21:49:08 1993")
	 *     The current local timezone is assumed.
	 * (3) a free-form date which must have a month, day, year, and time.
	 *     Optional fields are a weekday, and timezone, and the seconds
	 *     subfield of the time field.
	 *
	 *     There are no restrictions on the ordering of fields, with the
	 *     exception of the day of month and year.
	 *
	 *     Assumptions made:
	 *     The first free-standing number found is assumed to be the day of
	 *     month.  The second is the day of year.
	 *
	 *     Fields:
	 *
	 *       Month (required)
	 *       The month field may be one of the three-letter English
	 *       abbreviations (eg. Jan, Feb, Mar, etc), or the full English
	 *       month name (eg. January, February, March, etc). Note that they
	 *       are case sensitive, and only the first character is uppercase.
	 *
	 *
	 *       Day of month (required)
	 *       A number.  Currently, the first free-standing number found is
	 *       assumed to be the day of month. 
	 *
	 *       Year (required)
	 *       A number.  Currently, the second free-standing number found is
	 *       assumed to be the year.
	 *
	 *       Time (required)
	 *       The hours, minutes, and optionally seconds of the time.  It is
	 *       in the form HH:MM or HH:MM:SS.  The hour is expressed in the
	 *       24 hour format (eg. 4pm would be hour 16).
	 *
	 *       Day of week (optional)
	 *       The day of week field may be one of the three-letter English
	 *       abbreviations (eg. Sun, Mon, Tue, etc), or the full English
	 *       day name (eg. Sunday, Monday, Tuesday, etc).  Note that they
	 *       are case sensitive, and only the first character is uppercase.
	 *
	 *       Timezone (optional)
	 *       The timezone of the date.  The date and time will be
	 *       transformed into UTC by this time zone.  If the time zone is
	 *       not present, then the date is assumed to be in local time.
	 *
	 *       The following time zones are recognized (case sensitive):
	 *         UTC, GMT, BST, IST, WET, WEST, CET, CEST, EET, EEST, MSK,
	 *         MSD, AST, ADT, EST, EDT, ET, CST, CDT, CT, MST, MDT, MT,
	 *         PST, PDT, PT, HST, AKST, AKDT, WST, and single letters [A-Z]
	 *
	 *       Timezones ET, CT, MT, and PT are all assumed to be standard
	 *       (not daylight) time.
	 *       NOTE: Some Australian time zones (EST, and CST), which
	 *       conflict with US time zones are assumed to be in the US.
	 *
	 *     Valid Examples of free-form date strings:
	 *       Thu Jan 29 11:02:42 MST 2004
	 *       Oct 3 17:52:56 EDT 1998
	 *       5:41:26 PDT Aug 15 1984	  
	 *       7 November 2003 13:35:03 GMT
	 *       Monday May 1 8:27:39 UTC 2003
	 *       June 1 23:49 2003	 
	 *
	 * Note that DateTime cannot hold an interval, so you cannot construct
	 * one from an CIM DateTime interval.
	 * If str is a CIM DateTime, the timezone is part of the string and
	 * will be honored.
	 * Otherwise, if str is a ctime() formatted string, the current C
	 * timezone will be used.
	 *
	 * @param str	An String object that contains the DateTime string
	 * @throws DateTimeException if str does not contain a valid DateTime
	 */
	explicit DateTime(const String& str);
	/**
	 * Create a DateTime object that represents the given time.
	 *
	 * @param t      The epoch time that this DateTime object will respresent
	 * @param microseconds The microsecond component of the time.
	 *  This is the number of seconds since the Epoch, and should not be
	 *  adjusted for time zone.
	 */
	explicit DateTime(time_t t, UInt32 microseconds=0);
	/**
	 * Create a new DateTime object that represent the given date and time
	 *
	 * @param year   The year component of the date
	 * @param month  The month component of the date
	 * @param day    The day component of the date
	 * @param hour   The hour component of the time
	 * @param minute The minute component of the time.
	 * @param second The second component of the time.
	 * @param microsecond The microsecond component of the time.
	 */
	DateTime(int year, int month, int day, int hour=0, int minute=0,
		int second=0, UInt32 microsecond=0, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Destructor
	 */
	~DateTime();
	/**
	 * Get the hour of the day for this DateTime object 0-23
	 *
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 * @return
	 */
	int getHour(ETimeOffset timeOffset = E_LOCAL_TIME) const;
	/**
	 * Get the minute of the hour for this DateTime object 0-59
	 *
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 * @return
	 */
	int getMinute(ETimeOffset timeOffset = E_LOCAL_TIME) const;
	/**
	 * Get the second of the minute for this DateTime object normally in
	 * the range 0-59, but can be up to 60 to allow for leap seconds.
	 *
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 * @return
	 */
	int getSecond(ETimeOffset timeOffset = E_LOCAL_TIME) const;
	/**
	 * Get the microsecond of the second for this DateTime object.
	 * In the range 0-999999.  There are 1 million (1000000) microseconds 
	 * per second.
	 *
	 * @return
	 */
	UInt32 getMicrosecond() const;
	/**
	 * Get the day of the month (1-31)
	 *
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 * @return
	 */
	int getDay(ETimeOffset timeOffset = E_LOCAL_TIME) const;
	/**
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 * @return The day of the week for the date represented by this object. This
	 * will be a value in the range 0-6.
	 */
	int getDow(ETimeOffset timeOffset = E_LOCAL_TIME) const;
	/**
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 * @return The month of the year. This will be a value in the range 1-12.
	 */
	int getMonth(ETimeOffset timeOffset = E_LOCAL_TIME) const;
	/**
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 * @return The year. This value will include the century (e.g. 2001).
	 */
	int getYear(ETimeOffset timeOffset = E_LOCAL_TIME) const;
	/**
	 * @return The number of seconds since Jan 1, 1970.
	 */
	time_t get() const;
	/**
	 * Set the hour component of this DateTime object.
	 * @param hour The new hour for this DateTime object.
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 */
	void setHour(int hour, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Set the minute component of this DateTime object.
	 * @param minute The new minute for this DateTime object.
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 */
	void setMinute(int minute, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Set the second component of this DateTime object.
	 * @param second The new second for this DateTime object.
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 */
	void setSecond(int second, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Set the microsecond component of this DateTime object.
	 * @param microsecond The new microsecond for this DateTime object.
	 *  Acceptable range is 0-999999
	 */
	void setMicrosecond(UInt32 microsecond);
	/**
	 * Set the time component of this DateTime object.
	 * @param hour The hour component of the time.
	 * @param minute The minute component of the time.
	 * @param second The second component of the time.
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 */
	void setTime(int hour, int minute, int second, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Set the day component of this DateTime object.
	 * @param day The new day for this DateTime object.
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 */
	void setDay(int day, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Set the month component of this DateTime object.
	 * @param month The new month for this DateTime object.
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 */
	void setMonth(int month, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Set the year component of this DateTime object.
	 * @param year The new year for this DateTime object.
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 */
	void setYear(int year, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Set this DateTime object with a time_t value.
	 * @param t A time_t value that represents the number of seconds from
	 *		Jan 1, 1970.
	 * @param microseconds The microsecond component of the time.
	 */
	void set(time_t t, UInt32 microseconds=0)
	{
		m_time = t;
		m_microseconds = microseconds;
	}
	/**
	 * Set the date and time for this DateTime.
	 * @param year The new year component for this object.
	 * @param month The new month component for this object.
	 * @param day The new day component for this object.
	 * @param hour The new hour component for this object.
	 * @param minute The new minute component for this object.
	 * @param second The new second component for this object.
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 */
	void set(int year, int month, int day, int hour, int minute, int second, UInt32 microseconds, ETimeOffset timeOffset = E_LOCAL_TIME);
	/**
	 * Set this DateTime to the current system time.
	 */
	void setToCurrent();
	/**
	 * Add days to the date represented by this object.
	 * @param days The number of days to add to this object.
	 */
	void addDays(int days);
	/**
	 * Add week to the date represented by this object.
	 * @param weeks The number of weeks to add to this object.
	 */
	void addWeeks(int weeks)
	{
		addDays(weeks * 7);
	}
	/**
	 * Add months to the date represented by this object.
	 * @param months The number of months to add to this object.
	 */
	void addMonths(int months);
	/**
	 * Add years to the date represent by this object.
	 * @param years The number of years to add to this object.
	 */
	void addYears(int years);
	/**
	 * Add seconds to the date represented by this object.
	 * @param seconds The number of seconds to add to this object.
	 */
	void addSeconds(long seconds)
	{
		m_time += seconds;
	}
	/**
	 * Add minutes to the date represented by this object.
	 * @param minutes The number of minutes to add to this object.
	 */
	void addMinutes(long minutes)
	{
		m_time += minutes * 60;
	}
	/**
	 * Add hours to the date represented by this object.
	 * @param hours The number of hours to add to this object.
	 */
	void addHours(long hours) {  m_time += hours * 60 * 60; }
	/**
	 * Less than operator
	 * @param tm The DateTime object to compare this one to.
	 * @return true if this object is less than the given DateTime object.
	 */
	bool operator< ( const DateTime& tm ) const
	{
		if (m_time == tm.m_time)
		{
			return m_microseconds < tm.m_microseconds;
		}
		return m_time < tm.m_time;
	}
	/**
	 * Greater than operator
	 * @param tm The DateTime object to compare this one to.
	 * @return true if this object is greater than the given DateTime object.
	 */
	bool operator> ( const DateTime& tm ) const
	{
		return tm < *this;
	}
	/**
	 * Equality operator
	 * @param tm The DateTime object to compare this one to.
	 * @return true if this object is equal to the given DateTime object.
	 */
	bool operator== ( const DateTime& tm ) const
	{
		return m_time == tm.m_time && m_microseconds == tm.m_microseconds;
	}
	/**
	 * Inequality operator
	 * @param tm The DateTime object to compare this one to.
	 * @return true if this object is not equal to the given DateTime object.
	 */
	bool operator!= ( const DateTime& tm ) const
	{
		return !(*this == tm);
	}
	/**
	 * Less than or equal operator
	 * @param tm The DateTime object to compare this one to.
	 * @return true if this object is less than or equal to the given DateTime
	 * object.
	 */
	bool operator<= ( const DateTime& tm ) const
	{
		return !(tm < *this);
	}
	/**
	 * Greate than or equal operator
	 * @param tm The DateTime object to compare this one to.
	 * @return true if this object is greater than or equal to the given
	 * DateTime object.
	 */
	bool operator>= ( const DateTime& tm ) const
	{
		return !(*this < tm);
	}
	/**
	 * Add a given number of seconds to this DateTime object.
	 * @param seconds The number of seconds to add to this object.
	 * @return A reference to this object.
	 */
	DateTime& operator+= (long seconds)
	{
		addSeconds(seconds);
		return *this;
	}
	/**
	 * Subtract a given number of seconds from this DateTime object.
	 * @param seconds The number of seconds to subtract from this object.
	 * @return A reference to this object.
	 */
	DateTime& operator-= (long seconds)
	{
		addSeconds(-seconds);
		return *this;
	}
	/**
	 * @param timeOffset Indicates whether to use the local timezone or UTC
	 * @return The string representation of this DateTime object.  The format
	 * is like "Wed Jun 30 21:49:08 1993\n" as returned by ctime().
	 */
	String toString(ETimeOffset timeOffset = E_LOCAL_TIME) const;
	/**
	 * This is the same as toString(E_UTC_TIME).  This function is 
	 * deprecated and exists for backward compatibility purposes.
	 */
	String toStringGMT() const OW_DEPRECATED; // in 3.0.0

	/**
	 * Returns the GMT Offset (number of hours) of the system's timezone.
	 */
	static Int16 getGMTOffset();

private:
	time_t	m_time;
	UInt32	m_microseconds;
	tm getTm(ETimeOffset timeOffset) const;
	void setTime(tm& tmarg, ETimeOffset timeOffset);
};

} // end namespace OpenWBEM

#endif
