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

#ifndef OW_CIMFLAVOR_HPP_INCLUDE_GUARD_
#define OW_CIMFLAVOR_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMBase.hpp"
#include "OW_Types.hpp"
#include <iosfwd>

namespace OW_NAMESPACE
{

/**
 * The CIMFlavor class represents the flavor that is associated with a
 * CIMQualifier.
 */
class OW_COMMON_API CIMFlavor : public CIMBase
{
public:
	enum Flavor
	{
		/** Invalid flavor */
		INVALID				= 0,
		/** Overridable */
		ENABLEOVERRIDE		= 1,
		/** Cannot be overriden */
		DISABLEOVERRIDE	= 2,
		/** Applies only to the declaring class */
		RESTRICTED			= 3,
		/** Qualifier is inherited */
		TOSUBCLASS			= 4,
		/** Qualifier can be specified in multiple locales */
		TRANSLATE			= 5,
		// Mark limit for scopes
		LASTVALUE			= 6
	};
	/**
	 * Determine if an integral value represents a valid flavor.
	 * @param iflavor The integral value verify.
	 * @return true if the given integral value is a valid flavor. Otherwise
	 * false.
	 */
	static bool validFlavor(Int32 iflavor)
	{
		return(iflavor > INVALID && iflavor < LASTVALUE);
	}
	/**
	 * Create a new CIMFlavor object.
	 * @param iflavor		The flavor value for this CIMFlavor object. Cabe be
	 * one of the following values:
	 * 	CIMFlavor::ENABLEOVERRIDE	= overridable
	 * 	CIMFlavor::DISABLEOVERRIDE	= cannot be overriden
	 * 	CIMFlavor::RESTRICTED		= applies only to the declaring class
	 * 	CIMFlavor::TOSUBCLASS		= qualifier is inherited
	 * 	CIMFlavor::TRANSLATE			= qualifier can be specified in multiple
	 *												  locales
	 */
	CIMFlavor(Flavor iflavor) :
		CIMBase(), m_flavor(iflavor)
	{
		if (!validFlavor(iflavor))
		{
			m_flavor = INVALID;
		}
	}
	/**
	 * Default constructor - Creates an invalid CIMFlavor.
	 */
	CIMFlavor() :
		CIMBase(), m_flavor(INVALID) {}
	/**
	 * Copy constructor
	 * @param arg	The CIMFlavor to create a copy of.
	 */
	CIMFlavor(const CIMFlavor& arg) :
		CIMBase(), m_flavor(arg.m_flavor)
	{
	}

	typedef Int32 CIMFlavor::*safe_bool;
	/**
	 * @return true if this is a valid flavor
	 */
	operator safe_bool () const
		{  return (validFlavor(m_flavor) == true) ? &CIMFlavor::m_flavor : 0; }
	bool operator!() const
		{  return !validFlavor(m_flavor); }
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assign the values from a given CIMFlavor object to this one.
	 * @param arg	The CIMFlavor object to assign values from.
	 * @return A reference to this CIMFlavor object after the assignment is
	 * made.
	 */
	CIMFlavor& operator= (const CIMFlavor& arg)
	{
		m_flavor = arg.m_flavor;
		return *this;
	}
	/**
	 * @return true if this CIMFlavor is valid. Otherwise false.
	 */
	bool isValid()
	{
		return (m_flavor != INVALID);
	}
	
	/**
	 * Check if another CIMFlavor object is equal to this one.
	 * @param arg The CIMFlavor to compare this object to.
	 * @return true if the given CIMFlavor object has the same flavor value
	 * as this one.
	 */
	bool equals(const CIMFlavor& arg)
	{
		return (m_flavor == arg.m_flavor);
	}
	/**
	 * Equality operator
	 * @param arg The CIMFlavor to compare this object to.
	 * @return true if the given CIMFlavor object has the same flavor value
	 * as this one.
	 */
	bool operator== (const CIMFlavor& arg)
	{
		return equals(arg);
	}
	/**
	 * Inequality operator
	 * @param arg The CIMFlavor to compare this object to.
	 * @return true if the given CIMFlavor object has a different flavor
	 * value than this one.
	 */
	bool operator!= (const CIMFlavor& arg)
	{
		return (equals(arg) == false);
	}
	/**
	 * Read this CIMFlavor object from an input stream.
	 * @param istrm The input stream to read this flavor from.
	 */
	virtual void readObject(std::istream &istrm);
	/**
	 * Write this CIMFlavor object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;
	/**
	 * @return An String object that represents this CIMFlavor.
	 */
	virtual String toString() const;
	/**
	 * @return An String object that contains the MOF representation of this
	 * CIMFlavor object.
	 */
	virtual String toMOF() const;
	/**
	 * @return the integral value of this CIMFlavor object.
	 */
	Int32 getFlavor() const {  return m_flavor; }
private:
	static bool validScope(Int32 iflavor)
	{
		//
		// NOTE: has implicit knowledge of flavor values!
		//
		return(iflavor >= ENABLEOVERRIDE && iflavor < LASTVALUE);
	}
	/** The integral representation of this flavor */
	Int32 m_flavor;
	friend bool operator<(const CIMFlavor& x, const CIMFlavor& y)
	{
		return x.m_flavor < y.m_flavor;
	}
};

} // end namespace OW_NAMESPACE

#endif
