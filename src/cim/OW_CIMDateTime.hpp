/*******************************************************************************
* Copyright (C) 2001-3 Vintela, Inc. All rights reserved.
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
#ifndef OW_CIMDATETIME_HPP_INCLUDE_GUARD_
#define OW_CIMDATETIME_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_COWReference.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNULL.hpp"
#include "OW_String.hpp"

namespace OpenWBEM
{

class CIMDateTimeException : CIMException
{
public:
	CIMDateTimeException(const char* file, int line,
		const char* msg) :
		CIMException(file, line, CIMException::FAILED, msg) {}
	virtual const char* type() const
		{  return "CIMDateTimeException"; }
};

class DateTime;
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
 *	    yyyy = year (00000-9999)
 *	    mm = month (01-12)
 *	    dd = day (01-31)
 *	    hh = hour (00-23)
 *	    mm = minute (00-59)
 *	    ss = second (00-60) normally 59, but a leap second may be present
 *	    mmmmmm = microseconds.
 *	    s = '+' or '-' to represent the UTC sign.
 *	    utc = UTC offset (same as GMT offset).  
 *                utc is the offset from UTC in minutes. It's worth noting that
 *                when daylight saving time is in effect, the utc will be
 *                different then when it's not.
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
 *	Values must be zero-padded so that the entire string is always the 
 *	same 25-character length. Fields which are not significant must be 
 *	replaced with asterisk characters.
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
class CIMDateTime
{
public:
	//////////////////////////////////////////////////////////////////////////////
	struct DateTimeData
	{
		DateTimeData() :
			m_year(0), m_month(0), m_days(0), m_hours(0),
			m_minutes(0), m_seconds(0), m_microSeconds(0), m_utc(0),
			m_isInterval(1) {}
	
		int compare(const DateTimeData& arg);
	
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
	
	/**
	 * Create a new interval type of CIMDateTime set to 0's
	 */
	CIMDateTime();
	/**
	 * Create a new interval type of CIMDateTime 
	 * this object will have a null implementation.
	 */
	explicit CIMDateTime(CIMNULL_t);
	/**
	 * Create a new CIMDateTime object that is a copy of another.
	 * @param arg The CIMDateTime object to make a copy of.
	 */
	CIMDateTime(const CIMDateTime& arg);
	/**
	 * Create a new CIMDateTime object from a string representation of a
	 * CIM DateTime (See description of this string format in the class
	 * documentation of CIMDateTime.
	 * @param arg An String that contains the string form of the CIM Date
	 *		time.
	 */
	explicit CIMDateTime(const String& arg);
	/**
	 * Create an CIMDateTime object from a regular DateTime object.
	 * This constructor is *not* explicit so that a DateTime can be used
	 * where a CIMDateTime may be required.
	 * @param arg The DateTime object to use in determining the value of
	 *		this CIMDateTime object.
	 */
	CIMDateTime(const DateTime& arg);
	/**
	 * Create an CIMDateTime object that represents an interval.
	 * @param microSeconds The number of micro seconds that this interval
	 *		represents.
	 */
	explicit CIMDateTime(UInt64 microSeconds);
	/**
	 * Assignment operation
	 * @param arg	The CIMDateTime object to assign to this one.
	 * @return A reference to this CIMDataTime object after the assignment is
	 * made.
	 */
	CIMDateTime& operator= (const CIMDateTime& arg);
	/**
	 * @return The year component of this CIMDateTime object as an UInt16.
	 */
	UInt16 getYear() const {  return m_dptr->m_year; }
	/**
	 * @return The monthr component of this CIMDateTime object as an UInt8.
	 */
	UInt8 getMonth() const {  return m_dptr->m_month; }
	/**
	 * @return The days component of this CIMDateTime object as an UInt32.
	 */
	UInt32 getDays() const {  return m_dptr->m_days; }
	/**
	 * @return The day component of this CIMDateTime object as an UInt32.
	 */
	UInt32 getDay() const {  return m_dptr->m_days; }
	/**
	 * @return The hours component of this CIMDateTime object as an UInt8.
	 */
	UInt8 getHours() const {  return m_dptr->m_hours; }
	/**
	 * @return The minutes component of this CIMDateTime object as an
	 * UInt8.
	 */
	UInt8 getMinutes() const {  return m_dptr->m_minutes; }
	/**
	 * @return The seconds component of this CIMDateTime object as an
	 * UInt8.
	 */
	UInt8 getSeconds() const {  return m_dptr->m_seconds; }
	/**
	 * @return The microseconds component of this CIMDateTime object as an
	 * UInt32.
	 */
	UInt32 getMicroSeconds() const {  return m_dptr->m_microSeconds; }
	/**
	 * @return The utc offset component of this CIMDateTime object as an
	 * Int16
	 */
	Int16 getUtc() const {  return m_dptr->m_utc; }
	/**
	 * Check if another CIMDateTime object is equal to this one.
	 * @param arg The CIMDateTime object to check for equality with this one.
	 * @return true if the given CIMDateTime object is equal to this one.
	 * Otherwise false.
	 */
	bool equal(const CIMDateTime& arg) const;
	/**
	 * Equality operator
	 * @param arg The CIMDateTime object to check for equality with this one.
	 * @return true if the given CIMDateTime object is equal to this one.
	 * Otherwise false.
	 */
	bool operator== (const CIMDateTime& arg) const
			{  return equal(arg); }
	/**
	 * Set the year component of the CIMDateTime object.
	 * @param arg The new year for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setYear(UInt16 arg);
	/**
	 * Set the month component of the CIMDateTime object.
	 * @param arg The new month for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setMonth(UInt8 arg);
	/**
	 * Set the days component of the CIMDateTime object.
	 * @param arg The new days value for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setDays(UInt32 arg);
	/**
	 * Set the day component of the CIMDateTime object.
	 * @param arg The new day for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setDay(UInt32 arg);
	/**
	 * Set the hours component of the CIMDateTime object.
	 * @param arg The new hours value for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setHours(UInt8 arg);
	/**
	 * Set the minutes component of the CIMDateTime object.
	 * @param arg The new minutes value for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setMinutes(UInt8 arg);
	/**
	 * Set the seconds component of the CIMDateTime object.
	 * @param arg The new seconds value for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setSeconds(UInt8 arg);
	/**
	 * Set the microseconds component of the CIMDateTime object.
	 * @param arg The new microseconds value for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setMicroSeconds(UInt32 arg);
	/**
	 * Set the utc offset component of the CIMDateTime object.
	 * @param arg The new utc offset for this object.
	 * @return a reference to *this
	 */
	CIMDateTime& setUtc(Int16 arg);
	/**
	 * @return true if this CIMDateTime object represents an interval type
	 * of CIM date time.
	 */
	bool isInterval() const {  return bool(m_dptr->m_isInterval != 0);}
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
	 * @return the string representation of this CIMDateTime object. (see
	 * description of string format in documentation of class CIMDateTime)
	 */
	String toString() const;
	/**
	 * @return a DateTime object that corresponds to this CIMDateTime.
	 */
	DateTime toDateTime() const;
	/**
	 * @return true If this CIMDateTime is not comprised of zero values.
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
	COWReference<DateTimeData> m_dptr;
	friend bool operator<(const CIMDateTime& x, const CIMDateTime& y);
};
std::ostream& operator<< (std::ostream& ostr, const CIMDateTime& arg);

} // end namespace OpenWBEM

typedef OpenWBEM::CIMDateTimeException OW_CIMDateTimeException;
typedef OpenWBEM::CIMDateTime OW_CIMDateTime;

#endif
