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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_BOOL_HPP_INCLUDE_GUARD_
#define OW_BOOL_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include <iosfwd>

class OW_String;

/**
 * The OW_Bool class is an abstraction for the boolean data type.
 */
class OW_Bool
{
public:
		
	/**
	 * Create an OW_Bool object initialized to false.
	 */
	OW_Bool() : m_val(false) {}

	/**
	 * Create an OW_Bool object initialized to a given boolean value.
	 * @param val The boolean value to initialize this OW_Bool to
	 */
	OW_Bool(bool val) : m_val(val) {}

	/**
	 * Copy constructor
	 * @param arg The OW_Bool to make this object a copy of.
	 */
	OW_Bool(const OW_Bool& arg) : m_val(arg.m_val) {}

	/**
	 * Assignment operator
	 * @param arg The OW_Bool to assign to this one.
	 * @return A reference to this object after the assignment has been made.
	 */
	OW_Bool& operator= (const OW_Bool& arg) { m_val = arg.m_val; return *this; }

	/**
	 * Equality operator
	 * @param arg bool value to compare this OW_Bool object against.
	 * @return true If the given bool value is equal to this one. Otherwise
	 * false
	 */
	bool operator== (const bool arg) const { return m_val == arg; }

	/**
	 * Equality operator
	 * @param arg The OW_Bool object to compare this one with.
	 * @return true If the given OW_Bool object is equal to this one. Otherwise
	 * false
	 */
	bool operator== (const OW_Bool& arg) const { return m_val == arg.m_val; }

	/**
	 * Inequality operator
	 * @param arg bool value to compare this OW_Bool object against.
	 * @return true If the given bool value is not equal to this one. Otherwise
	 * false
	 */
	bool operator!= (const bool arg) const { return m_val != arg; }

	/**
	 * Inequality operator
	 * @param arg The OW_Bool object to compare this one with.
	 * @return true If the given OW_Bool object is not equal to this one.
	 * Otherwise false
	 */
	bool operator!= (const OW_Bool& arg) const { return m_val != arg.m_val; }

	/**
	 * @return The bool value of this OW_Bool object.
	 */
	operator bool() const { return m_val; }

	/**
	 * Negation operator
	 * @return true if this OW_Bool operator has a value of false.
	 */
	bool operator !() const { return !m_val; }

	/**
	 * @return The string representation of this object. If this object contains
	 * a true value, the "true" is returned. Otherwise "false" is returned.
	 */
	OW_String toString() const;

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

	/**
	 * Less than operator (friend function)
	 * @param b1 The first OW_Bool object to use in the comparison.
	 * @param b2 The second OW_Bool object to use in the comparison.
	 * @return true if b1 is less than b2. Otherwise false.
	 */
	friend bool operator< (const OW_Bool& b1, const OW_Bool& b2)
	{
		return b1.m_val < b2.m_val;
	}

private:

	OW_Bool(const void*);
	OW_Bool(void*);
	OW_Bool(volatile const void*);
	OW_Bool(volatile void*);

	bool m_val;
};

std::ostream& operator << (std::ostream& ostrm, const OW_Bool& arg);


#endif

