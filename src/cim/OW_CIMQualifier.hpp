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

#ifndef __OW_CIMQUALIFIER_HPP__
#define __OW_CIMQUALIFIER_HPP__


#include "OW_config.h"

#include "OW_CIMElement.hpp"
#include "OW_COWReference.hpp"
#include "OW_CIMFwd.hpp"

/**
 * The OW_CIMQualifier class encapsulates the data and functionality pertinent
 * to a CIM qualifier that is found on classes, properties and methods.
 * OW_CIMQualifier objects are ref counted and copy on write.
 */
class OW_CIMQualifier : public OW_CIMElement
{
public:

	struct QUALData;

	// Meta qualifiers
	static const char* const CIM_QUAL_ASSOCIATION;
	static const char* const CIM_QUAL_INDICATION;

	// Standard qualifiers
	static const char* const CIM_QUAL_ABSTRACT;
	static const char* const CIM_QUAL_AGGREGATE;
	static const char* const CIM_QUAL_AGGREGATION;
	static const char* const CIM_QUAL_ALIAS;
	static const char* const CIM_QUAL_ARRAYTYPE;
	static const char* const CIM_QUAL_BITMAP;
	static const char* const CIM_QUAL_BITVALUES;
	static const char* const CIM_QUAL_COUNTER;
	static const char* const CIM_QUAL_DESCRIPTION;
	static const char* const CIM_QUAL_DISPLAYNAME;
	static const char* const CIM_QUAL_EMBEDDEDOBJECT;
	static const char* const CIM_QUAL_GAUGE;
	static const char* const CIM_QUAL_IN;
	static const char* const CIM_QUAL_KEY;
	static const char* const CIM_QUAL_MAPPINGSTRINGS;
	static const char* const CIM_QUAL_MAX;
	static const char* const CIM_QUAL_MAXLEN;
	static const char* const CIM_QUAL_MAXVALUE;
	static const char* const CIM_QUAL_MIN;
	static const char* const CIM_QUAL_MINVALUE;
	static const char* const CIM_QUAL_NONLOCAL;
	static const char* const CIM_QUAL_NONLOCALTYPE;
	static const char* const CIM_QUAL_NULLVALUE;
	static const char* const CIM_QUAL_OUT;
	static const char* const CIM_QUAL_OVERRIDE;
	static const char* const CIM_QUAL_PROPAGATED;
	static const char* const CIM_QUAL_READ;
	static const char* const CIM_QUAL_REQUIRED;
	static const char* const CIM_QUAL_REVISION;
	static const char* const CIM_QUAL_SCHEMA;
	static const char* const CIM_QUAL_SOURCE;
	static const char* const CIM_QUAL_SOURCETYPE;
	static const char* const CIM_QUAL_STATIC;
	static const char* const CIM_QUAL_TERMINAL;
	static const char* const CIM_QUAL_UNITS;
	static const char* const CIM_QUAL_VALUEMAP;
	static const char* const CIM_QUAL_VALUES;
	static const char* const CIM_QUAL_VERSION;
	static const char* const CIM_QUAL_WEAK;
	static const char* const CIM_QUAL_WRITE;
	static const char* const CIM_QUAL_PROVIDER;

	/**
	 * Create a new OW_CIMQualifier object.
	 * @param notNull If false, this object will not have any data or
	 *		implementation associated with it.
	 */
	explicit OW_CIMQualifier(OW_Bool notNull=false);

	/**
	 * Create an OW_CIMQualifier with a given name.
	 * @param name	The name for this OW_CIMQualifier
	 */
	OW_CIMQualifier(const OW_String& name);

	/**
	 * Create an OW_CIMQualifier with a given name.
	 * @param name	The name for this OW_CIMQualifier as a NULL terminated
	 *		string.
	 */
	OW_CIMQualifier(const char* name);

	/**
	 * Create an OW_CIMQualifier with a name and type
	 * @param name The name for this OW_CIMQualifier
	 * @param qtype The qualifier type that this qualifier is based upon.
	 */
	OW_CIMQualifier(const OW_CIMQualifierType& qtype);

	/**
	 * Copy constructor
	 * @param arg The OW_CIMQualifier that this object will be a copy of.
	 */
	OW_CIMQualifier(const OW_CIMQualifier& arg);

	/**
	 * Destroy this OW_CIMQualifier class.
	 */
	~OW_CIMQualifier();

	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMQualifier to assign to this object.
	 * @return A reference to this object after the assignment is made.
	 */
	OW_CIMQualifier& operator= (const OW_CIMQualifier& arg);

	/**
	 * @return true if this is the key qualifier. Otherwise false.
	 */
	OW_Bool isKeyQualifier() const;

	/**
	 * @return true if this is the association qualifier. Otherwise false.
	 */
	OW_Bool isAssociationQualifier() const;

	/**
	 * @return The OW_CIMValue for this qualifier
	 */
	OW_CIMValue getValue() const;

	/**
	 * Set the value for this qualifier
	 * @param value The OW_CIMValue for this qualifier.
	 */
	void setValue(const OW_CIMValue& value);

	/**
	 * Set default type for this qualifier
	 * @param qtype The qualifier type that this qualifier is based upon.
	 */
	void setDefaults(const OW_CIMQualifierType& qtype);

	/**
	 * Get default type for this qualifier
	 * @return The default type for this qualifier
	 */
	OW_CIMQualifierType getDefaults() const;

	/**
	 * Determine if this qualifier has the given flavor.
	 * @param flavor Check if the qualifier has this flavor.
	 * @return true if this qualifier has the specified flavor. Otherwise false.
	 */
	OW_Bool hasFlavor(const OW_CIMFlavor& flavor) const;

	/**
	 * Add an OW_CIMFlavor to this qualifier
	 * @param newFlavor The flavor to add to this qualifier
	 */
	void addFlavor(const OW_CIMFlavor& flavor);

	/**
	 * Remove a flavor from this OW_CIMQualifier.
	 * @param flavor The integral value of the flavor to remove from this
	 * 	qualifier.
	 */
	void removeFlavor(OW_Int32 flavor);

	/**
	 * @return true if this qualifier has a value. Otherwise false.
	 */
	OW_Bool hasValue() const;

	/**
	 * Check for equality against another qualifier.
	 * @param arg The OW_CIMQualifier to check for equality against.
	 * @return true if this qualifier has the same name as the given qualifier.
	 * Otherwise false.
	 */
	OW_Bool equals(const OW_CIMQualifier& arg) const;

	/**
	 * @return The list of flavors this qualifier has.
	 */
	OW_CIMFlavorArray getFlavor() const;

	/**
	 * Flag this qualifier as being propagated.
	 * @param propagated	If true, then qualifier is flagged as propagated.
	 */
	void setPropagated(OW_Bool propagated=true);

	/**
	 * @return true if qualifier is propagated. Otherwise false.
	 */
	OW_Bool getPropagated() const;

private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (!m_pdata.isNull()) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (!m_pdata.isNull()) ? 0: &dummy::nonnull; }

	/**
	 * @return The name of this qualifier as an OW_String.
	 */
	virtual OW_String getName() const;

	/**
	 * Set the name of this qualifier.
	 * @param name The new name of this qualifier as an OW_String.
	 */
	virtual void setName(const OW_String& name);
	
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

	/**
	 * @return The MOF representation of this qualifier as an OW_String.
	 */
	virtual OW_String toMOF() const;

	/**
	 * @return The string representation of this qualifier.
	 */
	virtual OW_String toString() const;

	/**
	 * Create a key qualifier
	 */
	static OW_CIMQualifier createKeyQualifier();

private:

	OW_COWReference<QUALData> m_pdata;

	friend bool operator<(const OW_CIMQualifier& x, const OW_CIMQualifier& y);
};

#endif	// __OW_CIMQUALIFIER_HPP__
