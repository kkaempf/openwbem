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

#ifndef __OW_CHAR16_HPP__
#define __OW_CHAR16_HPP__

#include "OW_config.h"
#include "OW_ArrayFwd.hpp"
#include "OW_Types.h"
#include "OW_Bool.hpp"

#include <iosfwd>

class OW_String;

/**
 * The OW_Char16 class is an abstraction for a double byte character.
 * No operation involving double byte character have been check in OpenWBEM.
 * Currently they are treated like single byte characters.
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
	OW_Char16(char c) : m_value((OW_UInt16)c) {}

	/**
	 * Create a new OW_Char16 object from the 1st byte of a string.
	 * @param x The string to take the character from that will be contained by
	 *		this OW_Char16 object.
	 */
	OW_Char16(const OW_String& x);

	/**
	 * Create a new OW_Char16 object of an unsigned 16 bit value.
	 * @param val The unsigned 16 bit value this object will contain.
	 */
	OW_Char16(OW_UInt16 val) : m_value(val) {}

	/**
	 * Create a new OW_Char16 object of an signed 16 bit value.
	 * @param val The signed 16 bit value this object will contain.
	 */
	OW_Char16(OW_Int16 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object of an unsigned 8 bit value.
	 * @param val The unsigned 8 bit value this object will contain.
	 */
	OW_Char16(OW_UInt8 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object of an signed 8 bit value.
	 * @param val The signed 8 bit value this object will contain.
	 */
	OW_Char16(OW_Int8 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object of an unsigned 32 bit value.
	 * @param val The unsigned 32 bit value this object will contain.
	 */
	OW_Char16(OW_UInt32 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object of an signed 32 bit value.
	 * @param val The signed 32 bit value this object will contain.
	 */
	OW_Char16(OW_Int32 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object of an unsigned 64 bit value.
	 * @param val The unsigned 64 bit value this object will contain.
	 */
	OW_Char16(OW_UInt64 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object of an signed 64 bit value.
	 * @param val The signed 64 bit value this object will contain.
	 */
	OW_Char16(OW_Int64 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object from a real 32 value.
	 * @param val The real 32 value this object will contain.
	 */
	OW_Char16(OW_Real32 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object from a real 64 value.
	 * @param val The real 64 value this object will contain.
	 */
	OW_Char16(OW_Real64 val) : m_value((OW_UInt16)val) {}

	/**
	 * Create a new OW_Char16 object from a boolean value.
	 * @param val The boolean value this object will contain.
	 */
	OW_Char16(OW_Bool val) : m_value((OW_UInt16)val) {}

	/**
	 * @return The value of this object as a unsigned 16 bit value.
	 */
	OW_UInt16 getValue() const { return m_value; }

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
	OW_Bool operator== (const OW_Char16& arg) const
	{
		return m_value == arg.m_value;
	}

	/**
	 * Inequality operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if the given OW_Char16 object is not equal to this one.
	 */
	OW_Bool operator!= (const OW_Char16& arg) const
	{
		return m_value != arg.m_value;
	}

	/**
	 * Less than operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if this object is less than the given OW_Char16 object.
	 */
	OW_Bool operator< (const OW_Char16& arg) const
	{
		return m_value < arg.m_value;
	}

	/**
	 * Less than or equal operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if this object is less than or equal to the given OW_Char16
	 * object.
	 */
	OW_Bool operator<= (const OW_Char16& arg) const
	{
		return m_value <= arg.m_value;
	}

	/**
	 * Greater than operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if this object is greater than the given OW_Char16 object.
	 */
	OW_Bool operator> (const OW_Char16& arg) const
	{
		return m_value > arg.m_value;
	}

	/**
	 * Greater than or equal operator
	 * @param arg The OW_Char16 object to compare this object with.
	 * @return true if this object is greater than or equal to the given
	 * OW_Char16 object.
	 */
	OW_Bool operator>= (const OW_Char16& arg) const
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

	/**
	 * @return The unsigned 16 bit value of this object.
	 */
	operator OW_UInt16() const { return m_value; }

	/**
	 * @return The single character value of this object.
	 */
	operator char() const { return (char) m_value; }

	/**
	 * @return The value of this object as a void pointer.
	 */
	operator void*() const { return reinterpret_cast<void*>(m_value); }

	/**
	 * @return The unsigned 8 bit value of this object.
	 */
	operator OW_UInt8() const { return (OW_UInt8) m_value; }

	/**
	 * @return The signed 8 bit value of this object.
	 */
	operator OW_Int8() const { return (OW_Int8) m_value; }

	/**
	 * @return The signed 16 bit value of this object.
	 */
	operator OW_Int16() const { return (OW_Int16) m_value; }

	/**
	 * @return The unsigned 32 bit value of this object.
	 */
	operator OW_UInt32() const { return (OW_UInt32) m_value; }

	/**
	 * @return The signed 32 bit value of this object.
	 */
	operator OW_Int32() const { return (OW_Int32) m_value; }

	/**
	 * @return The unsigned 64 bit value of this object.
	 */
	operator OW_UInt64() const { return (OW_UInt64) m_value; }

	/**
	 * @return The signed 64 bit value of this object.
	 */
	operator OW_Int64() const { return (OW_Int64) m_value; }

	/**
	 * @return The real 32 value of this object.
	 */
	operator OW_Real32() const { return (OW_Real32) m_value; }

	/**
	 * @return The real 64 value of this object.
	 */
	operator OW_Real64() const { return (OW_Real64) m_value; }

	/**
	 * Escape an OW_Char16 object for XML output
	 * @param c16 The OW_Char16 object to escape.
	 * @param bfr The buffer to place the output of the escape operation.
	 * @return A pointer to the beginning of buffer on success. Otherwise NULL.
	 * ATTN: UTF8 support
	 */
	static const char* xmlExcape(OW_UInt16 c16, char bfr[20]);

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

inline OW_Bool operator== (char c, const OW_Char16& arg)
{
	return c == char(arg);
}

inline OW_Bool operator== (const OW_Char16& arg, int v)
{
	return (arg.getValue() == (OW_UInt32)v);
}

inline OW_Bool operator== (int v, const OW_Char16& arg)
{
	return (arg.getValue() == (OW_UInt32)v);
}

inline OW_Bool operator!= (const OW_Char16& arg, int v)
{
	return (arg.getValue() != (OW_UInt32)v);
}

inline OW_Bool operator!= (int v, const OW_Char16& arg)
{
	return (arg.getValue() != (OW_UInt32)v);
}

inline OW_Bool operator!= (char c, const OW_Char16& arg)
{
	return c != char(arg);
}

inline OW_Bool operator< (char c, const OW_Char16& arg)
{
	return c < char(arg);
}

inline OW_Bool operator<= (char c, const OW_Char16& arg)
{
	return c <= char(arg);
}

inline OW_Bool operator> (char c, const OW_Char16& arg)
{
	return c > char(arg);
}

inline OW_Bool operator>= (char c, const OW_Char16& arg)
{
	return c >= char(arg);
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

#endif	// __OW_CHAR16_HPP__
