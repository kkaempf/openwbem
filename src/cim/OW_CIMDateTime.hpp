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

#ifndef OW_CIMDATETIME_HPP_INCLUDE_GUARD_
#define OW_CIMDATETIME_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_COWReference.hpp"
#include "OW_CIMException.hpp"
#include "OW_Bool.hpp"
#include "OW_CIMNULL.hpp"

class OW_CIMDateTimeException : OW_CIMException
{
public:
	OW_CIMDateTimeException() : OW_CIMException(OW_CIMException::FAILED) {}

	OW_CIMDateTimeException(const char* file, int line,
		const char* msg) :
		OW_CIMException(file, line, OW_CIMException::FAILED, msg) {}

	virtual const char* type() const
		{  return "OW_CIMDateTimeException"; }
};

class OW_DateTime;

/**
 *
 *	The DateTime class represents the CIM datetime data type.
 *	Recall that a CIM datetime may contain a date or an interval.
 *
 *	A date has the following form:
 *
 *	    yyyymmddhhmmss.mmmmmmsutc
 *
 *	Where
 *
 *	    yyyy = year (0-1999)
 *	    mm = month (1-12)
 *	    dd = day (1-31)
 *	    hh = hour (0-23)
 *	    mm = minute (0-59)
 *	    ss = second (0-59)
 *	    mmmmmm = microseconds.
 *	    s = '+' or '-' to represent the UTC sign.
 *	    utc = UTC offset (same as GMT offset).
 *
 *	An interval has the following form:
 *
 *	    ddddddddhhmmss.mmmmmm:000
 *
 *	Where
 *
 *	    dddddddd = days
 *	    hh = hours
 *	    mm = minutes
 *	    ss = seconds
 *	    mmmmmm = microseconds
 *
 *	Note that intervals always end in ":000" (this is how they
 *	are distinguished from dates).
 *
 *	Intervals are really durations since they do not specify a start and
 *	end time (as one expects when speaking about an interval). It is
 *	better to think of an interval as specifying time elapsed since
 *	some event.
 *
 *	DateTime objects are constructed from C character strings or from
 *	other DateTime objects. These character strings must be exactly
 *	twenty-five characters and conform to one of the forms idententified
 *	above.
 *
 *	DateTime objects which are not explicitly initialized will be
 *	implicitly initialized with the null time interval:
 *
 *	    00000000000000.000000:000
 *
 *	Instances can be tested for nullness with the isNull() method.
 */
class OW_CIMDateTime
{
public:

	//////////////////////////////////////////////////////////////////////////////
	struct OW_DateTimeData
	{
		OW_DateTimeData() :
			m_year(0), m_month(0), m_days(0), m_hours(0),
			m_minutes(0), m_seconds(0), m_microSeconds(0), m_utc(0),
			m_isInterval(1) {}
	
		int compare(const OW_DateTimeData& arg);
	
		OW_UInt16 m_year;
		OW_UInt8 m_month;
		OW_UInt32 m_days;
		OW_UInt8 m_hours;
		OW_UInt8 m_minutes;
		OW_UInt8 m_seconds;
		OW_UInt32 m_microSeconds;
		OW_Int16 m_utc;
		OW_UInt8 m_isInterval;

        OW_DateTimeData* clone() const { return new OW_DateTimeData(*this); }
	};
	

	/**
	 * Create a new interval type of OW_CIMDateTime set to 0's
	 */
	OW_CIMDateTime();

	/**
	 * Create a new interval type of OW_CIMDateTime 
	 * this object will have a null implementation.
	 */
	explicit OW_CIMDateTime(OW_CIMNULL_t);

	/**
	 * Create a new OW_CIMDateTime object that is a copy of another.
	 * @param arg The OW_CIMDateTime object to make a copy of.
	 */
	OW_CIMDateTime(const OW_CIMDateTime& arg);

	/**
	 * Create a new OW_CIMDateTime object from a string representation of a
	 * CIM DateTime (See description of this string format in the class
	 * documentation of OW_CIMDateTime.
	 * @param arg An OW_String that contains the string form of the CIM Date
	 *		time.
	 */
	explicit OW_CIMDateTime(const OW_String& arg);

	/**
	 * Create an OW_CIMDateTime object from a regular OW_DateTime object.
	 * @param arg The OW_DateTime object to use in determining the value of
	 *		this OW_CIMDateTime object.
	 */
	explicit OW_CIMDateTime(const OW_DateTime& arg);

	/**
	 * Create an OW_CIMDateTime object that represents an interval.
	 * @param microSeconds The number of micro seconds that this interval
	 *		represents.
	 */
	explicit OW_CIMDateTime(OW_UInt64 microSeconds);

	/**
	 * Assignment operation
	 * @param arg	The OW_CIMDateTime object to assign to this one.
	 * @return A reference to this OW_CIMDataTime object after the assignment is
	 * made.
	 */
	OW_CIMDateTime& operator= (const OW_CIMDateTime& arg);

	/**
	 * @return The year component of this OW_CIMDateTime object as an OW_UInt16.
	 */
	OW_UInt16 getYear() const {  return m_dptr->m_year; }

	/**
	 * @return The monthr component of this OW_CIMDateTime object as an OW_UInt8.
	 */
	OW_UInt8 getMonth() const {  return m_dptr->m_month; }

	/**
	 * @return The days component of this OW_CIMDateTime object as an OW_UInt32.
	 */
	OW_UInt32 getDays() const {  return m_dptr->m_days; }

	/**
	 * @return The day component of this OW_CIMDateTime object as an OW_UInt32.
	 */
	OW_UInt32 getDay() const {  return m_dptr->m_days; }

	/**
	 * @return The hours component of this OW_CIMDateTime object as an OW_UInt8.
	 */
	OW_UInt8 getHours() const {  return m_dptr->m_hours; }

	/**
	 * @return The minutes component of this OW_CIMDateTime object as an
	 * OW_UInt8.
	 */
	OW_UInt8 getMinutes() const {  return m_dptr->m_minutes; }

	/**
	 * @return The seconds component of this OW_CIMDateTime object as an
	 * OW_UInt8.
	 */
	OW_UInt8 getSeconds() const {  return m_dptr->m_seconds; }

	/**
	 * @return The microseconds component of this OW_CIMDateTime object as an
	 * OW_UInt32.
	 */
	OW_UInt32 getMicroSeconds() const {  return m_dptr->m_microSeconds; }

	/**
	 * @return The utc offset component of this OW_CIMDateTime object as an
	 * OW_Int16
	 */
	OW_Int16 getUtc() const {  return m_dptr->m_utc; }

	/**
	 * Check if another OW_CIMDateTime object is equal to this one.
	 * @param arg The OW_CIMDateTime object to check for equality with this one.
	 * @return true if the given OW_CIMDateTime object is equal to this one.
	 * Otherwise false.
	 */
	OW_Bool equal(const OW_CIMDateTime& arg) const;

	/**
	 * Equality operator
	 * @param arg The OW_CIMDateTime object to check for equality with this one.
	 * @return true if the given OW_CIMDateTime object is equal to this one.
	 * Otherwise false.
	 */
	OW_Bool operator== (const OW_CIMDateTime& arg) const
			{  return equal(arg); }

	/**
	 * Set the year component of the OW_CIMDateTime object.
	 * @param arg The new year for this object.
	 */
	void setYear(OW_UInt16 arg);

	/**
	 * Set the month component of the OW_CIMDateTime object.
	 * @param arg The new month for this object.
	 */
	void setMonth(OW_UInt8 arg);

	/**
	 * Set the days component of the OW_CIMDateTime object.
	 * @param arg The new days value for this object.
	 */
	void setDays(OW_UInt32 arg);

	/**
	 * Set the day component of the OW_CIMDateTime object.
	 * @param arg The new day for this object.
	 */
	void setDay(OW_UInt32 arg);

	/**
	 * Set the hours component of the OW_CIMDateTime object.
	 * @param arg The new hours value for this object.
	 */
	void setHours(OW_UInt8 arg);

	/**
	 * Set the minutes component of the OW_CIMDateTime object.
	 * @param arg The new minutes value for this object.
	 */
	void setMinutes(OW_UInt8 arg);

	/**
	 * Set the seconds component of the OW_CIMDateTime object.
	 * @param arg The new seconds value for this object.
	 */
	void setSeconds(OW_UInt8 arg);

	/**
	 * Set the microseconds component of the OW_CIMDateTime object.
	 * @param arg The new microseconds value for this object.
	 */
	void setMicroSeconds(OW_UInt32 arg);

	/**
	 * Set the utc offset component of the OW_CIMDateTime object.
	 * @param arg The new utc offset for this object.
	 */
	void setUtc(OW_Int16 arg);

	/**
	 * @return true if this OW_CIMDateTime object represents an interval type
	 * of CIM date time.
	 */
	OW_Bool isInterval() const {  return OW_Bool(bool(m_dptr->m_isInterval));}

	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	void readObject(std::istream &istrm);

	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	void writeObject(std::ostream &ostrm) const;

	/**
	 * @return the string representation of this OW_CIMDateTime object. (see
	 * description of string format in documentation of class OW_CIMDateTime)
	 */
	OW_String toString() const;

	/**
	 * @return true If this OW_CIMDateTime is not comprised of zero values.
	 */
private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const;
	safe_bool operator!() const;

private:
	OW_COWReference<OW_DateTimeData> m_dptr;

	friend bool operator<(const OW_CIMDateTime& x, const OW_CIMDateTime& y);
};

std::ostream& operator<< (std::ostream& ostr, const OW_CIMDateTime& arg);

#endif


