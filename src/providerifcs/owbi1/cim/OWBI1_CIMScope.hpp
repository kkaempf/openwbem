/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.  *
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

#ifndef OWBI1_CIMSCOPE_HPP_
#define OWBI1_CIMSCOPE_HPP_
#include "OWBI1_config.h"
#include "OWBI1_CIMBase.hpp"

#if defined(BAD)
#undef BAD
#endif

namespace OWBI1
{

/**
 * The CIMScope class encapsulated the data and functionality the is
 * associated with CIM scopes.
 */
class OWBI1_OWBI1PROVIFC_API CIMScope : public CIMBase
{
public:
	// An enumeration of the scope values that this class understands
	enum Scope
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
		ANY			= 11,	// Any CIM element
		MAXSCOPE	= 12
	};
	/**
	 * Create an invalid scope.
	 */
	CIMScope() : CIMBase(), m_val(BAD) {}
	/**
	 * Create a scope with a specified scoping value.
	 * @param scopeVal The scoping value.
	 */
	CIMScope(Scope scopeVal) : CIMBase(), m_val(scopeVal)
	{
		if (!validScope(scopeVal))
			m_val = BAD;
	}
	/**
	 * Copy constructor
	 * @param arg The CIMScopy to make this object a copy of.
	 */
	CIMScope(const CIMScope& arg) :
		CIMBase(), m_val(arg.m_val) {}
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMScopy to assign to this object.
	 * @return A reference to this object after the assignment has been made.
	 */
	CIMScope& operator= (const CIMScope& arg)
	{
		m_val = arg.m_val;
		return *this;
	}
	/**
	 * @return The scoping value of this scope.
	 */
	Scope getScope() const {  return m_val; }
	/**
	 * Determine if another scope is equal to this one.
	 * @param arg The scope to check for equality against.
	 * @return true if arg is equal to this scope. Otherwise false.
	 */
	bool equals(const CIMScope& arg) const
	{
		return (m_val == arg.m_val);
	}
	/**
	 * Determine if another scope is equal to this one.
	 * @param arg The scope to check for equality against.
	 * @return true if arg is equal to this scope. Otherwise false.
	 */
	bool operator == (const CIMScope& arg) const
	{
		return equals(arg);
	}
	/**
	 * Determine if another scope is not equal to this one.
	 * @param arg The scope to check for inequality against.
	 * @return true if arg is not equal to this scope. Otherwise false.
	 */
	bool operator != (const CIMScope& arg) const
	{
		return !equals(arg);
	}

	typedef Scope CIMScope::*safe_bool;
	/**
	 * @return true if this is a valid flavor
	 */
	operator safe_bool () const
		{  return (validScope(m_val) == true) ? &CIMScope::m_val : 0; }
	bool operator!() const
		{  return !validScope(m_val); }
	/**
	 * @return The string representation of this scopy
	 */
	virtual String toString() const;
	/**
	 * @return The MOF representation of this scope as an String.
	 */
	virtual String toMOF() const;
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
	static bool validScope(Scope val)
	{
		return (val > BAD && val < MAXSCOPE);
	}
	Scope m_val;
	friend bool operator<(const CIMScope& x, const CIMScope& y)
	{
		return x.m_val < y.m_val;
	}
};

} // end namespace OWBI1

#endif
