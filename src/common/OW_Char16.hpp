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

#ifndef OW_CHAR16_HPP_INCLUDE_GUARD_
#define OW_CHAR16_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ArrayFwd.hpp"
#include "OW_Types.hpp"
#include "OW_Bool.hpp"

#include <iosfwd>

class OW_String;

/**
 * The OW_Char16 class is an abstraction for a double byte character.
 */
class OW_Char16
{
public:

	/**
	 * Create a new OW_Char16 object with a value of zero.
	 */
	OW_Char16() : m_value(0) {}

	/**
	 * Copy constructor
	 * @param arg The OW_Char16 object this object will be a copy of.
	 */
	OW_Char16(const OW_Char16& arg) : m_value(arg.m_value) {}

	/**
	 * Create a new OW_Char16 object from a single byte character.
	 * @param c The character this OW_Char16 object will contain.
	 */
	explicit OW_Char16(char c) : m_value(c) {}

	/**
	 * Create a new OW_Char16 object from the 1st byte of a string.
	 * @param x The string to take the character from that will be contained by
	 *		this OW_Char16 object.
	 */
	explicit OW_Char16(const OW_String& x);

	/**
	 * Create a new OW_Char16 object of an unsigned 16 bit value.
	 * Note that this constructor is explicitly non-explicit :-)
	 * IOW, implicit conversion from OW_UInt16 is desireable for this class.
	 * @param val The unsigned 16 bit value this object will contain.
	 */
	OW_Char16(OW_UInt16 val) : m_value(val) {}

	/**
	 * Create a new OW_Char16 object of an signed 16 bit value.
	 * @param val The signed 16 bit value this object will contain.
	 */
	explicit OW_Char16(OW_Int16 val) : m_value(val) {}

	/**
	 * Create a new OW_Char16 object of an unsigned 8 bit value.
	 * @param val The unsigned 8 bit value this object will contain.
	 */
	explicit OW_Char16(OW_UInt8 val) : m_value(val) {}

	/**
	 * Create a new OW_Char16 object of an signed 8 bit value.
	 * @param val The signed 8 bit value this object will contain.
	 */
	explicit OW_Char16(OW_Int8 val) : m_value(val) {}

	/**
	 * Create a new OW_Char16 object of an unsigned 32 bit value.
	 * @param val The unsigned 32 bit value this object will contain.
	 */
	explicit OW_Char16(OW_UInt32 val) : m_value(val) {}

	/**
	 * Create a new OW_Char16 object of an signed 32 bit value.
	 * @param val The signed 32 bit value this object will contain.
	 */
	explicit OW_Char16(OW_Int32 val) : m_value(val) {}

	/**
	 * Create a new OW_Char16 object of an unsigned 64 bit value.
	 * @param val The unsigned 64 bit value this object will contain.
	 */
	explicit OW_Char16(OW_UInt64 val) : m_value(static_cast<OW_UInt16>(val)) {}

	/**
	 * Create a new OW_Char16 object of an signed 64 bit value.
	 * @param val The signed 64 bit value this object will contain.
	 */
	explicit OW_Char16(OW_Int64 val) : m_value(static_cast<OW_UInt16>(val)) {}

	/**
	 * Create a new OW_Char16 object from a real 32 value.
	 * @param val The real 32 value this object will contain.
	 */
	explicit OW_Char16(OW_Real32 val) : m_value(static_cast<OW_UInt16>(val)) {}

	/**
	 * Create a new OW_Char16 object from a real 64 value.
	 * @param val The real 64 value this object will contain.
	 */
	explicit OW_Char16(OW_Real64 val) : m_value(static_cast<OW_UInt16>(val)) {}

	/**
	 * Create a new OW_Char16 object from a boolean value.
	 * @param val The boolean value this object will contain.
	 */
	explicit OW_Char16(OW_Bool val) : m_value(val) {}

	/**
	 * @return The value of this object as a unsigned 16 bit value.
	 */
	OW_UInt16 getValue() const { return m_value; }

	/**
	 * @return The value of this object as a unsigned 16 bit value.
	 */
	operator OW_UInt16() const { return getValue(); }

	/**
	 * Assignment operator
	 * @param arg The OW_Char16 to assign to this object.
	 * @return A reference to this object after the assignment has been made.
	 */
	OW_Char16& operator= (const OW_Char16& arg)
	{
		m_value = arg.m_value;
		return *this;
	}

	/**
	 * Equality operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if the given OW_Char16 object is equal to this one.
	 */
	bool operator== (const OW_Char16& arg) const
	{
		return m_value == arg.m_value;
	}

	/**
	 * Inequality operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if the given OW_Char16 object is not equal to this one.
	 */
	bool operator!= (const OW_Char16& arg) const
	{
		return m_value != arg.m_value;
	}

	/**
	 * Less than operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if this object is less than the given OW_Char16 object.
	 */
	bool operator< (const OW_Char16& arg) const
	{
		return m_value < arg.m_value;
	}

	/**
	 * Less than or equal operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if this object is less than or equal to the given OW_Char16
	 * object.
	 */
	bool operator<= (const OW_Char16& arg) const
	{
		return m_value <= arg.m_value;
	}

	/**
	 * Greater than operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if this object is greater than the given OW_Char16 object.
	 */
	bool operator> (const OW_Char16& arg) const
	{
		return m_value > arg.m_value;
	}

	/**
	 * Greater than or equal operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if this object is greater than or equal to the given
	 * OW_Char16 object.
	 */
	bool operator>= (const OW_Char16& arg) const
	{
		return m_value >= arg.m_value;
	}

	/**
	 * Add another OW_Char16 object to this one.
	 * @param arg The OW_Char16 object to add to this one.
	 * @return A reference to this object.
	 */
	OW_Char16& operator+= (const OW_Char16& arg)
	{
		m_value += arg.m_value;
		return *this;
	}

	/**
	 * Subtract another OW_Char16 object from this one.
	 * @param arg The OW_Char16 object to subtract from this one.
	 * @return A reference to this object.
	 */
	OW_Char16& operator-= (const OW_Char16& arg)
	{
		m_value -= arg.m_value;
		return *this;
	}

	/**
	 * Multiply this OW_Char16 object by another.
	 * @param arg The OW_Char16 object to multiply this one by.
	 * @return A reference to this object.
	 */
	OW_Char16& operator*= (const OW_Char16& arg)
	{
		m_value *= arg.m_value;
		return *this;
	}

	/**
	 * Divide this OW_Char16 object by another.
	 * @param arg The OW_Char16 object to divide this one by.
	 * @return A reference to this object.
	 */
	OW_Char16& operator/= (const OW_Char16& arg)
	{
		m_value /= arg.m_value;
		return *this;
	}

private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (m_value) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (m_value) ? 0: &dummy::nonnull; }

	/**
	 * Convert this to UTF8
	 */
	OW_String toUTF8() const;

	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	void writeObject(std::ostream& ostrm) const;

	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	void readObject(std::istream& istrm);

private:

	OW_UInt16 m_value;
};

inline bool operator== (char c, const OW_Char16& arg)
{
	return OW_Char16(c) == arg;
}

inline bool operator== (const OW_Char16& arg, int v)
{
	return (arg.getValue() == v);
}

inline bool operator== (int v, const OW_Char16& arg)
{
	return (arg.getValue() == v);
}

inline bool operator!= (const OW_Char16& arg, int v)
{
	return (arg.getValue() != v);
}

inline bool operator!= (int v, const OW_Char16& arg)
{
	return (arg.getValue() != v);
}

inline bool operator!= (char c, const OW_Char16& arg)
{
	return OW_Char16(c) != arg;
}

inline bool operator< (char c, const OW_Char16& arg)
{
	return OW_Char16(c) < arg;
}

inline bool operator<= (char c, const OW_Char16& arg)
{
	return OW_Char16(c) <= arg;
}

inline bool operator> (char c, const OW_Char16& arg)
{
	return OW_Char16(c) > arg;
}

inline bool operator>= (char c, const OW_Char16& arg)
{
	return OW_Char16(c) >= arg;
}

inline OW_Char16 operator+ (const OW_Char16& arg1, const OW_Char16& arg2)
{
	return OW_Char16(OW_UInt16(arg1.getValue() + arg2.getValue()));
}

inline OW_Char16 operator- (const OW_Char16& arg1, const OW_Char16& arg2)
{
	return OW_Char16(OW_UInt16(arg1.getValue() - arg2.getValue()));
}

inline OW_Char16 operator* (const OW_Char16& arg1, const OW_Char16& arg2)
{
	return OW_Char16(OW_UInt16(arg1.getValue() * arg2.getValue()));
}

inline OW_Char16 operator/ (const OW_Char16& arg1, const OW_Char16& arg2)
{
	return OW_Char16(OW_UInt16(arg1.getValue() / arg2.getValue()));
}

std::ostream& operator<< (std::ostream& ostrm, const OW_Char16& arg);

typedef OW_Array<OW_Char16> OW_Char16Array;

#endif

