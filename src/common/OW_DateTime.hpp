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
#ifndef _OW_DATETIME_HPP__
#define _OW_DATETIME_HPP__

#include "OW_config.h"
#include "OW_Bool.hpp"

class OW_String;

extern "C"
{
#include <time.h>
}

/**
 * The OW_DateTime class is an abstraction for date time data.
 * It allows easy date/time adjustment, date/time arithmetic and
 * comparison.
 */
class OW_DateTime
{
public:
	/**
	 * Create a new OW_DateTime object that represents the current time
	 */
	OW_DateTime();

	/**
	 * Create a new OW_DateTime object from a string that contains a
	 * CIM DateTime formated string
	 * @param str	An OW_String object that contains the CIM DateTime string
	 */
	OW_DateTime(const OW_String& str);

	/**
	 * Create a OW_DateTime object that represents the given time.
	 *
	 * @param t      The epoch time that this OW_DateTime object will respresent
	 */
	OW_DateTime(time_t t);

	/**
	 * Create a new OW_DateTime object that is a copy of the given OW_DateTime
	 * object
	 *
	 * @param t      The OW_DateTime object to create a copy of.
	 */
	OW_DateTime(const OW_DateTime& t);

	/**
	 * Create a new OW_DateTime object that represent the given date and time
	 *
	 * @param year   The year component of the date
	 * @param month  The month component of the date
	 * @param day    The day component of the date
	 * @param hour   The hour component of the time
	 * @param minute The minute component of the time.
	 * @param second The second component of the time.
	 */
	OW_DateTime(int year, int month, int day, int hour=0, int minute=0,
		int second=0);

	/**
	 * Destructor
	 */
	~OW_DateTime();

	/**
	 * @return true if this OW_DateTime represents an interval.
	 */
	OW_Bool isInterval() const {  return m_isInterval; }

	/**
	 * Get the hour of the day for this OW_DateTime object 0-23
	 *
	 * @return
	 */
	int getHour() const;

	/**
	 * Get the minute of the hour for this OW_DateTime object 0-59
	 *
	 * @return
	 */
	int getMinute() const;

	/**
	 * Get the second of the minute for this OW_DateTime object 0-59
	 *
	 * @return
	 */
	int getSecond() const;

	/**
	 * Get the day of the month (1-31)
	 *
	 * @return
	 */
	int getDay() const;

	/**
	 * @return The day of the week for the date represented by this object. This
	 * will be a value in the range 0-6.
	 */
	int getDow() const;

	/**
	 * @return The month of the year. This will be a value in the range 1-12.
	 */
	int getMonth() const;

	/**
	 * @return The year. This value will include the century (e.g. 2001).
	 */
	int getYear() const;

	/**
	 * @return The number of seconds since Jan 1, 1970.
	 */
	time_t get() const;

	/**
	 * Set the hour component of this OW_DateTime object.
	 * @param hour The new hour for this OW_DateTime object.
	 */
	void setHour(int hour);

	/**
	 * Set the minute component of this OW_DateTime object.
	 * @param minute The new minute for this OW_DateTime object.
	 */
	void setMinute(int minute);

	/**
	 * Set the second component of this OW_DateTime object.
	 * @param second The new second for this OW_DateTime object.
	 */
	void setSecond(int second);

	/**
	 * Set the time component of this OW_DateTime object.
	 * @param hour The hour component of the time.
	 * @param minute The minute component of the time.
	 * @param second The second component of the time.
	 */
	void setTime(int hour, int minute, int second);

	/**
	 * Set the day component of this OW_DateTime object.
	 * @param day The new day for this OW_DateTime object.
	 */
	void setDay(int day);

	/**
	 * Set the month component of this OW_DateTime object.
	 * @param month The new month for this OW_DateTime object.
	 */
	void setMonth(int month);

	/**
	 * Set the year component of this OW_DateTime object.
	 * @param year The new year for this OW_DateTime object.
	 */
	void setYear(int year);

	/**
	 * Set this OW_DateTime object with a time_t value.
	 * @param t A time_t value that represents the number of seconds from
	 *		Jan 1, 1970.
	 */
	void set(time_t t)
	{
		m_time = t;
	}

	/**
	 * Set the date and time for this OW_DateTime.
	 * @param year The new year component for this object.
	 * @param month The new month component for this object.
	 * @param day The new day component for this object.
	 * @param hour The new hour component for this object.
	 * @param minute The new minute component for this object.
	 * @param second The new second component for this object.
	 */
	void set(int year, int month, int day, int hour, int minute, int second);

	/**
	 * Set this OW_DateTime to the current system time.
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
	 * long operator
	 * @return The value of this OW_DateTime object as a long value that
	 * represents the number of seconds since Jan 1, 1970.
	 */
	operator long () const
	{
		return m_time;
	}

	/**
	 * unsigned long operator
	 * @return The value of this OW_DateTime object as an unsigned long value
	 * that represents the number of seconds since Jan 1, 1970.
	 */
	operator unsigned long () const
	{
		return (unsigned long) m_time;
	}

	/**
	 * Less than operator
	 * @param tm The OW_DateTime object to compare this one to.
	 * @return true if this object is less than the given OW_DateTime object.
	 */
	OW_Bool operator< ( const OW_DateTime& tm ) const
	{
		return m_time < tm.m_time;
	}

	/**
	 * Greater than operator
	 * @param tm The OW_DateTime object to compare this one to.
	 * @return true if this object is greater than the given OW_DateTime object.
	 */
	OW_Bool operator> ( const OW_DateTime& tm ) const
	{
		return m_time > tm.m_time;
	}

	/**
	 * Equality operator
	 * @param tm The OW_DateTime object to compare this one to.
	 * @return true if this object is equal to the given OW_DateTime object.
	 */
	OW_Bool operator== ( const OW_DateTime& tm ) const
	{
		return m_time == tm.m_time;
	}

	/**
	 * Inequality operator
	 * @param tm The OW_DateTime object to compare this one to.
	 * @return true if this object is not equal to the given OW_DateTime object.
	 */
	OW_Bool operator!= ( const OW_DateTime& tm ) const
	{
		return m_time != tm.m_time;
	}

	/**
	 * Less than or equal operator
	 * @param tm The OW_DateTime object to compare this one to.
	 * @return true if this object is less than or equal to the given OW_DateTime
	 * object.
	 */
	OW_Bool operator<= ( const OW_DateTime& tm ) const
	{
		return m_time <= tm.m_time;
	}

	/**
	 * Greate than or equal operator
	 * @param tm The OW_DateTime object to compare this one to.
	 * @return true if this object is greater than or equal to the given
	 * OW_DateTime object.
	 */
	OW_Bool operator>= ( const OW_DateTime& tm ) const
	{
		return m_time >= tm.m_time;
	}

	/**
	 * Add a given number of seconds to this OW_DateTime object.
	 * @param seconds The number of seconds to add to this object.
	 * @return A reference to this object.
	 */
	OW_DateTime& operator+= (long seconds)
	{
		addSeconds(seconds);
		return *this;
	}

	/**
	 * Subtract a given number of seconds from this OW_DateTime object.
	 * @param seconds The number of seconds to subtract from this object.
	 * @return A reference to this object.
	 */
	OW_DateTime& operator-= (long seconds)
	{
		addSeconds(-seconds);
		return *this;
	}

	/**
	 * Assignment operator
	 * @param tm The OW_DateTime object to assign to this one.
	 * @return A reference to this object.
	 */
	OW_DateTime& operator= (const OW_DateTime& tm)
	{
		m_time = tm.m_time;
		return *this;
	}

	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	void writeObject(std::ostream &ostrm) const;

	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	void readObject(std::istream &istrm);

	/**
	 * @return The string representation of this OW_DateTime object.
	 */
	OW_String toString() const;

	/**
	 * @return The string representation of this OW_DateTime object adjust to
	 * GMT.
	 */
	OW_String toStringGMT() const;

private:

	time_t	m_time;
	OW_Bool		m_isInterval;

	tm getTm() const;
};

#endif	// __OW_DATETIME_HPP__


