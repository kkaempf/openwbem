/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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

#ifndef OW_CIMDATETIME_HPP_INCLUDE_GUARD_
#define OW_CIMDATETIME_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "blocxx/COWIntrusiveReference.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNULL.hpp"
#include "blocxx/String.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_SafeBool.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(CIMDateTime, OW_COMMON_API);

/// @todo  Document valid ranges for values in the class and add asserts for them.

/**
 *
 *	The CIMDateTime class represents the CIM datetime data type.
 *	Recall that a CIM datetime may contain a date or an interval.
 *
 *	A date has the following form:
 *
 *	    yyyymmddhhmmss.mmmmmmsutc
 *
 *	Where
 *
 *	    yyyy = year (0000-9999)
 *	    mm = month (01-12)
 *	    dd = day (01-31)
 *	    hh = hour (00-23)
 *	    mm = minute (00-59)
 *	    ss = second (00-60) normally 59, but a leap second may be present
 *	    mmmmmm = microseconds (0-999999).
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
 *	CIMDateTime objects are constructed from C character strings or from
 *	DateTime objects. These character strings must be exactly
 *	twenty-five characters and conform to one of the forms idententified
 *	above.
 *
 *	CIMDateTime objects which are not explicitly initialized will be
 *	implicitly initialized with the null time interval:
 *
 *	    00000000000000.000000:000
 *
 *	Instances can be tested for nullness with the isNull() method.
 */
class OW_COMMON_API CIMDateTime
{
public:
	struct DateTimeData;

	/**
	 * Create a new interval type of CIMDateTime set to 0's
	 */
	CIMDateTime();
	~CIMDateTime();
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

	enum EErrorCodes
	{
		E_INVALID_DATE_TIME_FORMAT
	};

	/**
	 * Create a new CIMDateTime object from a string representation of a
	 * CIM DateTime (See description of this string format in the class
	 * documentation of CIMDateTime.
	 * @param arg An String that contains the string form of the CIM Date
	 *		time.
	 * @throws CIMDateTimeException E_INVALID_DATE_TIME_FORMAT if arg
	 *  isn't a valid CIM date time string.
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
	 * Range (0-9999) for a date/time, 0 for an interval
	 */
	UInt16 getYear() const;
	/**
	 * @return The month component of this CIMDateTime object as an UInt8.
	 * Range (1-12) for a date/time, 0 for an interval
	 */
	UInt8 getMonth() const;
	/**
	 * @return The days component of this CIMDateTime object as an UInt32.
	 * Range (1-31) for a date/time, (0-99999999) for an interval
	 */
	UInt32 getDays() const;
	/**
	 * @return The day component of this CIMDateTime object as an UInt32.
	 * Range (1-31) for a date/time, (0-99999999) for an interval
	 */
	UInt32 getDay() const;
	/**
	 * @return The hours component of this CIMDateTime object as an UInt8.
	 * Range (0-23)
	 */
	UInt8 getHours() const;
	/**
	 * @return The minutes component of this CIMDateTime object as an
	 * UInt8.
	 * Range (0-59)
	 */
	UInt8 getMinutes() const;
	/**
	 * @return The seconds component of this CIMDateTime object as an
	 * UInt8.
	 * Range (0-60) - 60 in the case of a leap second
	 */
	UInt8 getSeconds() const;
	/**
	 * @return The microseconds component of this CIMDateTime object as an
	 * UInt32.
	 * Range (0-999999)
	 */
	UInt32 getMicroSeconds() const;
	/**
	 * @return The utc offset component of this CIMDateTime object as an
	 * Int16
	 */
	Int16 getUtc() const;
	/**
	 * Check if another CIMDateTime object is equal to this one.
	 * @param arg The CIMDateTime object to check for equality with this one.
	 * @return true if the given CIMDateTime object is equal to this one.
	 * Otherwise false.
	 */
	bool equal(const CIMDateTime& arg) const;
	/**
	 * Set the year component of the CIMDateTime object.
	 * @param arg The new year for this object. Valid values are 0-9999 for a date/time, 0 for an interval
	 * @return a reference to *this
	 */
	CIMDateTime& setYear(UInt16 arg);
	/**
	 * Set the month component of the CIMDateTime object.
	 * @param arg The new month for this object. Valid values are 1-12 for a date/time, 0 for an interval
	 * @return a reference to *this
	 */
	CIMDateTime& setMonth(UInt8 arg);
	/**
	 * Set the days component of the CIMDateTime object.
	 * @param arg The new days value for this object. Valid values are 1-31 for a date/time, 0-99999999 for an interval
	 * @return a reference to *this
	 */
	CIMDateTime& setDays(UInt32 arg);
	/**
	 * Set the day component of the CIMDateTime object.
	 * @param arg The new day for this object. Valid values are 1-31 for a date/time, 0-99999999 for an interval
	 * @return a reference to *this
	 */
	CIMDateTime& setDay(UInt32 arg);
	/**
	 * Set the hours component of the CIMDateTime object.
	 * @param arg The new hours value for this object. Valid values are 0-23
	 * @return a reference to *this
	 */
	CIMDateTime& setHours(UInt8 arg);
	/**
	 * Set the minutes component of the CIMDateTime object.
	 * @param arg The new minutes value for this object. Valid values are 0-59
	 * @return a reference to *this
	 */
	CIMDateTime& setMinutes(UInt8 arg);
	/**
	 * Set the seconds component of the CIMDateTime object.
	 * @param arg The new seconds value for this object. Valid values are 0-60 (60 only for minutes that have leap seconds)
	 * @return a reference to *this
	 */
	CIMDateTime& setSeconds(UInt8 arg);
	/**
	 * Set the microseconds component of the CIMDateTime object.
	 * @param arg The new microseconds value for this object. Valid values are 0-999999
	 * @return a reference to *this
	 */
	CIMDateTime& setMicroSeconds(UInt32 arg);
	/**
	 * Set the utc offset component of the CIMDateTime object.
	 * @param arg The new utc offset for this object. For an interval, the only valid value is 0.
	 * @return a reference to *this
	 */
	CIMDateTime& setUtc(Int16 arg);
	/**
	 * @return true if this CIMDateTime object represents an interval type
	 * of CIM date time.
	 */
	bool isInterval() const;
	/**
	 * @param val bool indicating whether this is an interval
	 */
	void setInterval(bool val);
	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	void readObject(std::streambuf & istrm);
	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	void writeObject(std::streambuf & ostrm) const;
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
	OW_SAFE_BOOL_IMPL(CIMDateTime, COWIntrusiveReference<DateTimeData>, CIMDateTime::m_dptr, !isZero())

	/**
	 * @return true If this CIMDateTime is comprised of zero values.
	 */
	bool isZero() const;
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	COWIntrusiveReference<DateTimeData> m_dptr;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	friend OW_COMMON_API bool operator==(const CIMDateTime& x, const CIMDateTime& y);
	/**
	 * Compare 2 CIMDateTime instances.
	 * An interval is always less than a date/time.  Comparing a CIMNULL instance is undefined,
	 * and may throw a COWNULLReference exception or dereference a NULL pointer.
	 * The result of a comparison with an invalid date/time (any value out of range) is undefined.
	 */
	friend OW_COMMON_API bool operator<(const CIMDateTime& x, const CIMDateTime& y);
};
OW_COMMON_API std::ostream& operator<< (std::ostream& ostr, const CIMDateTime& arg);

bool operator!=(const CIMDateTime& x, const CIMDateTime& y);
bool operator>(const CIMDateTime& x, const CIMDateTime& y);
bool operator<=(const CIMDateTime& x, const CIMDateTime& y);
bool operator>=(const CIMDateTime& x, const CIMDateTime& y);



} // end namespace OW_NAMESPACE

#endif
