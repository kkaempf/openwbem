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

#ifndef __OW_CIMSCOPE_HPP__
#define __OW_CIMSCOPE_HPP__

#include "OW_config.h"
#include "OW_CIMBase.hpp"
#include "OW_Bool.hpp"

/**
 * The OW_CIMScope class encapsulated the data and functionality the is
 * associated with CIM scopes.
 */
class OW_CIMScope : public OW_CIMBase
{
public:

	// An enumeration of the scope values that this class understands
	enum
	{
		BAD			= 0,	// Indicates unset or bad
		SCHEMA		= 1,	// A CIM schema
		CLASS		= 2,	// A CIM class
		ASSOCIATION	= 3,	// A CIM association
		INDICATION	= 4,	// A CIM indication
		PROPERTY	= 5,	// A CIM property
		REFERENCE	= 6,	// A CIM reference
		METHOD		= 7,	// A CIM method
		PARAMETER	= 8,	// A CIM parameter
		QUALIFIER	= 9,  	// A CIM qualifier
		INSTANCE	= 10,	// An instance (NOT SUN)
		ANY			= 11,	// Any CIM element
		MAXSCOPE	= 12
	};

	/**
	 * Create an invalid scope.
	 */
	OW_CIMScope() : OW_CIMBase(), m_val(BAD) {}

	/**
	 * Create a scope with a specified scoping value.
	 * @param scopeVal The scoping value.
	 */
	OW_CIMScope(int scopeVal) : OW_CIMBase(), m_val(scopeVal)
	{
		if(!validScope(scopeVal))
			m_val = BAD;
	}

	/**
	 * Copy constructor
	 * @param arg The OW_CIMScopy to make this object a copy of.
	 */
	OW_CIMScope(const OW_CIMScope& arg) :
		OW_CIMBase(), m_val(arg.m_val) {}

	/**
	 * Set this to a null object.
	 */
	virtual void setNull() {  m_val = BAD; }

	/**
	 * Assignment operator
	 * @param arg The OW_CIMScopy to assign to this object.
	 * @return A reference to this object after the assignment has been made.
	 */
	OW_CIMScope& operator= (const OW_CIMScope& arg)
	{
		m_val = arg.m_val;
		return *this;
	}

	/**
	 * @return The scoping value of this scope.
	 */
	int getScope() const {  return m_val; }

	/**
	 * Determine if another scope is equal to this one.
	 * @param arg The scope to check for equality against.
	 * @return true if arg is equal to this scope. Otherwise false.
	 */
	OW_Bool equals(const OW_CIMScope& arg) const
	{
		return (m_val == arg.m_val);
	}

	/**
	 * Determine if another scope is equal to this one.
	 * @param arg The scope to check for equality against.
	 * @return true if arg is equal to this scope. Otherwise false.
	 */
	OW_Bool operator == (const OW_CIMScope& arg) const
	{
		return equals(arg);
	}

	/**
	 * Determine if another scope is not equal to this one.
	 * @param arg The scope to check for inequality against.
	 * @return true if arg is not equal to this scope. Otherwise false.
	 */
	OW_Bool operator != (const OW_CIMScope& arg) const
	{
		return !equals(arg);
	}

	/**
	 * @return A NULL pointer if this scope is invalid. Otherwise return a
	 * non-NULL pointer. THIS POINTER SHOULD NEVER BE DEREFERENCE!
	 */
	operator void*() const {  return (void*)(validScope(m_val) == true); }

	/**
	 * @return The string representation of this scopy
	 */
	virtual OW_String toString() const;

	/**
	 * @return The MOF representation of this scope as an OW_String.
	 */
	virtual OW_String toMOF() const;

	/**
	 * Write the XML representation of this scope to an output stream.
	 * @param ostr The output stream to write the XML to.
	 */
	//virtual void toXML(std::ostream& ostr) const;

	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

private:

	static OW_Bool validScope(int val)
	{
		return (val > BAD && val < MAXSCOPE);
	}

	int m_val;
};

#endif	//f __OW_CIMSCOPE_HPP__
