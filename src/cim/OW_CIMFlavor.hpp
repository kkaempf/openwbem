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

#ifndef __OW_CIMFLAVOR_HPP__
#define __OW_CIMFLAVOR_HPP__

#include "OW_config.h"
#include "OW_CIMBase.hpp"
#include "OW_Bool.hpp"
#include <iosfwd>

/**
 * The OW_CIMFlavor class represents the flavor that is associated with a
 * OW_CIMQualifier.
 */
class OW_CIMFlavor : public OW_CIMBase
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

		/** Qualifier appears in instances */
		TOINSTANCE			= 6,

		// Mark limit for scopes
		LASTVALUE			= 7
	};

	/**
	 * Determine if an integral value represents a valid flavor.
	 * @param iflavor The integral value verify.
	 * @return true if the given integral value is a valid flavor. Otherwise
	 * false.
	 */
	static OW_Bool validFlavor(OW_Int32 iflavor)
	{
		return(iflavor > INVALID && iflavor < LASTVALUE);
	}

	/**
	 * Create a new OW_CIMFlavor object.
	 * @param iflavor		The flavor value for this OW_CIMFlavor object. Cabe be
	 * one of the following values:
	 * 	OW_CIMFlavor::ENABLEOVERRIDE	= overridable
	 * 	OW_CIMFlavor::DISABLEOVERRIDE	= cannot be overriden
	 * 	OW_CIMFlavor::RESTRICTED		= applies only to the declaring class
	 * 	OW_CIMFlavor::TOSUBCLASS		= qualifier is inherited
	 * 	OW_CIMFlavor::TRANSLATE			= qualifier can be specified in multiple
	 *												  locales
	 */
	OW_CIMFlavor(Flavor iflavor) :
		OW_CIMBase(), m_flavor(iflavor)
	{
		if(!validFlavor(iflavor))
		{
			m_flavor = INVALID;
		}
	}

	/**
	 * Default constructor - Creates an invalid OW_CIMFlavor.
	 */
	OW_CIMFlavor() :
		OW_CIMBase(), m_flavor(INVALID) {}

	/**
	 * Copy constructor
	 * @param arg	The OW_CIMFlavor to create a copy of.
	 */
	OW_CIMFlavor(const OW_CIMFlavor& arg) :
		OW_CIMBase(), m_flavor(arg.m_flavor)
	{
	}

	/**
	 * @return true if this is a valid flavor
	 */
private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (validFlavor(m_flavor) == true) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (validFlavor(m_flavor) == true) ? 0: &dummy::nonnull; }

	/**
	 * Set this to a null object.
	 */
	virtual void setNull() {  m_flavor = INVALID; }

	/**
	 * Assign the values from a given OW_CIMFlavor object to this one.
	 * @param arg	The OW_CIMFlavor object to assign values from.
	 * @return A reference to this OW_CIMFlavor object after the assignment is
	 * made.
	 */
	OW_CIMFlavor& operator= (const OW_CIMFlavor& arg)
	{
		m_flavor = arg.m_flavor;
		return *this;
	}

	/**
	 * @return true if this OW_CIMFlavor is valid. Otherwise false.
	 */
	OW_Bool isValid()
	{
		return (m_flavor != INVALID);
	}
	
	/**
	 * Check if another OW_CIMFlavor object is equal to this one.
	 * @param arg The OW_CIMFlavor to compare this object to.
	 * @return true if the given OW_CIMFlavor object has the same flavor value
	 * as this one.
	 */
	OW_Bool equals(const OW_CIMFlavor& arg)
	{
		return (m_flavor == arg.m_flavor);
	}

	/**
	 * Equality operator
	 * @param arg The OW_CIMFlavor to compare this object to.
	 * @return true if the given OW_CIMFlavor object has the same flavor value
	 * as this one.
	 */
	OW_Bool operator== (const OW_CIMFlavor& arg)
	{
		return equals(arg);
	}

	/**
	 * Inequality operator
	 * @param arg The OW_CIMFlavor to compare this object to.
	 * @return true if the given OW_CIMFlavor object has a different flavor
	 * value than this one.
	 */
	OW_Bool operator!= (const OW_CIMFlavor& arg)
	{
		return (equals(arg) == false);
	}

	/**
	 * Read this OW_CIMFlavor object from an input stream.
	 * @param istrm The input stream to read this flavor from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * Write this OW_CIMFlavor object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * @return An OW_String object that represents this OW_CIMFlavor.
	 */
	virtual OW_String toString() const;

	/**
	 * @return An OW_String object that contains the MOF representation of this
	 * OW_CIMFlavor object.
	 */
	virtual OW_String toMOF() const;

	/**
	 * @return the integral value of this OW_CIMFlavor object.
	 */
	OW_Int32 getFlavor() const {  return m_flavor; }

private:

	static OW_Bool validScope(OW_Int32 iflavor)
	{
		//
		// NOTE: has implicit knowledge of flavor values!
		//
		return(iflavor >= ENABLEOVERRIDE && iflavor < LASTVALUE);
	}

	/** The integral representation of this flavor */
	OW_Int32 m_flavor;

	friend bool operator<(const OW_CIMFlavor& x, const OW_CIMFlavor& y)
	{
		return x.m_flavor < y.m_flavor;
	}
};

#endif	// __OW_CIMFLAVOR_HPP__

