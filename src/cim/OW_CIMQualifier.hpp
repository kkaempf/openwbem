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

#ifndef OW_CIMQUALIFIER_HPP_INCLUDE_GUARD_
#define OW_CIMQUALIFIER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMElement.hpp"
#include "OW_COWIntrusiveReference.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMNULL.hpp"

namespace OpenWBEM
{

/**
 * The CIMQualifier class encapsulates the data and functionality pertinent
 * to a CIM qualifier that is found on classes, properties and methods.
 * CIMQualifier objects are ref counted and copy on write.
 */
class CIMQualifier : public CIMElement
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
	static const char* const CIM_QUAL_COMPOSITION;
	static const char* const CIM_QUAL_COUNTER;
	static const char* const CIM_QUAL_DELETE;
	static const char* const CIM_QUAL_DEPRECATED;
	static const char* const CIM_QUAL_DESCRIPTION;
	static const char* const CIM_QUAL_DISPLAYNAME;
	static const char* const CIM_QUAL_DN;
	static const char* const CIM_QUAL_EMBEDDEDOBJECT;
	static const char* const CIM_QUAL_EXCEPTION;
	static const char* const CIM_QUAL_EXPENSIVE;
	static const char* const CIM_QUAL_EXPERIMENTAL;
	static const char* const CIM_QUAL_GAUGE;
	static const char* const CIM_QUAL_IFDELETED;
	static const char* const CIM_QUAL_IN;
	static const char* const CIM_QUAL_INVISIBLE;
	static const char* const CIM_QUAL_KEY;
	static const char* const CIM_QUAL_LARGE;
	static const char* const CIM_QUAL_MAPPINGSTRINGS;
	static const char* const CIM_QUAL_MAX;
	static const char* const CIM_QUAL_MAXLEN;
	static const char* const CIM_QUAL_MAXVALUE;
	static const char* const CIM_QUAL_MIN;
	static const char* const CIM_QUAL_MINLEN;
	static const char* const CIM_QUAL_MINVALUE;
	static const char* const CIM_QUAL_MODELCORRESPONDENCE;
	static const char* const CIM_QUAL_NONLOCAL;
	static const char* const CIM_QUAL_NONLOCALTYPE;
	static const char* const CIM_QUAL_NULLVALUE;
	static const char* const CIM_QUAL_OCTETSTRING;
	static const char* const CIM_QUAL_OUT;
	static const char* const CIM_QUAL_OVERRIDE;
	static const char* const CIM_QUAL_PROPAGATED;
	static const char* const CIM_QUAL_PROPERTYUSAGE;
	static const char* const CIM_QUAL_READ;
	static const char* const CIM_QUAL_REQUIRED;
	static const char* const CIM_QUAL_REVISION;
	static const char* const CIM_QUAL_SCHEMA;
	static const char* const CIM_QUAL_SOURCE;
	static const char* const CIM_QUAL_SOURCETYPE;
	static const char* const CIM_QUAL_STATIC;
	static const char* const CIM_QUAL_SYNTAX;
	static const char* const CIM_QUAL_SYNTAXTYPE;
	static const char* const CIM_QUAL_TERMINAL;
	static const char* const CIM_QUAL_TRIGGERTYPE;
	static const char* const CIM_QUAL_UNITS;
	static const char* const CIM_QUAL_UNKNOWNVALUES;
	static const char* const CIM_QUAL_UNSUPPORTEDVALUES;
	static const char* const CIM_QUAL_VALUEMAP;
	static const char* const CIM_QUAL_VALUES;
	static const char* const CIM_QUAL_VERSION;
	static const char* const CIM_QUAL_WEAK;
	static const char* const CIM_QUAL_WRITE;
	static const char* const CIM_QUAL_PROVIDER;
	/**
	 * Create a new CIMQualifier object.
	 */
	CIMQualifier();
	/**
	 * Create a NULL CIMQualifier object.
	 */
	explicit CIMQualifier(CIMNULL_t);
	/**
	 * Create an CIMQualifier with a given name.
	 * @param name	The name for this CIMQualifier
	 */
	CIMQualifier(const String& name);
	/**
	 * Create an CIMQualifier with a given name.
	 * @param name	The name for this CIMQualifier as a NULL terminated
	 *		string.
	 */
	CIMQualifier(const char* name);
	/**
	 * Create an CIMQualifier with a name and type
	 * @param qtype The qualifier type that this qualifier is based upon.
	 */
	CIMQualifier(const CIMQualifierType& qtype);
	/**
	 * Copy constructor
	 * @param arg The CIMQualifier that this object will be a copy of.
	 */
	CIMQualifier(const CIMQualifier& arg);
	/**
	 * Destroy this CIMQualifier class.
	 */
	~CIMQualifier();
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMQualifier to assign to this object.
	 * @return A reference to this object after the assignment is made.
	 */
	CIMQualifier& operator= (const CIMQualifier& arg);
	/**
	 * @return true if this is the key qualifier. Otherwise false.
	 */
	bool isKeyQualifier() const;
	/**
	 * @return true if this is the association qualifier. Otherwise false.
	 */
	bool isAssociationQualifier() const;
	/**
	 * @return The CIMValue for this qualifier
	 */
	CIMValue getValue() const;
	/**
	 * @return The CIMValue for this qualifier.
	 * @throws NULLValueException if the value is NULL.
	 */
	CIMValue getValueT() const;
	/**
	 * Set the value for this qualifier
	 * @param value The CIMValue for this qualifier.
	 * @return a reference to *this
	 */
	CIMQualifier& setValue(const CIMValue& value);
	/**
	 * Set default type for this qualifier
	 * @param qtype The qualifier type that this qualifier is based upon.
	 * @return a reference to *this
	 */
	CIMQualifier& setDefaults(const CIMQualifierType& qtype);
	/**
	 * Get default type for this qualifier
	 * @return The default type for this qualifier
	 */
	CIMQualifierType getDefaults() const;
	/**
	 * Determine if this qualifier has the given flavor.
	 * @param flavor Check if the qualifier has this flavor.
	 * @return true if this qualifier has the specified flavor. Otherwise false.
	 */
	bool hasFlavor(const CIMFlavor& flavor) const;
	/**
	 * Add an CIMFlavor to this qualifier
	 * @param newFlavor The flavor to add to this qualifier
	 * @return a reference to *this
	 */
	CIMQualifier& addFlavor(const CIMFlavor& flavor);
	/**
	 * Remove a flavor from this CIMQualifier.
	 * @param flavor The integral value of the flavor to remove from this
	 * 	qualifier.
	 * @return a reference to *this
	 */
	CIMQualifier& removeFlavor(Int32 flavor);
	/**
	 * @return true if this qualifier has a value. Otherwise false.
	 */
	bool hasValue() const;
	/**
	 * Check for equality against another qualifier.
	 * @param arg The CIMQualifier to check for equality against.
	 * @return true if this qualifier has the same name as the given qualifier.
	 * Otherwise false.
	 */
	bool equals(const CIMQualifier& arg) const;
	/**
	 * @return The list of flavors this qualifier has.
	 */
	CIMFlavorArray getFlavor() const;
	/**
	 * Flag this qualifier as being propagated.
	 * @param propagated	If true, then qualifier is flagged as propagated.
	 * @return a reference to *this
	 */
	CIMQualifier& setPropagated(bool propagated=true);
	/**
	 * @return true if qualifier is propagated. Otherwise false.
	 */
	bool getPropagated() const;

	typedef COWIntrusiveReference<QUALData> CIMQualifier::*safe_bool;
	operator safe_bool () const
		{  return m_pdata ? &CIMQualifier::m_pdata : 0; }
	bool operator!() const
		{  return !m_pdata; }
	/**
	 * @return The name of this qualifier as an String.
	 */
	virtual String getName() const;
	/**
	 * Set the name of this qualifier.
	 * @param name The new name of this qualifier as an String.
	 */
	virtual void setName(const String& name);
	
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
	 * @return The MOF representation of this qualifier as an String.
	 */
	virtual String toMOF() const;
	/**
	 * @return The string representation of this qualifier.
	 */
	virtual String toString() const;
	/**
	 * Create a key qualifier
	 */
	static CIMQualifier createKeyQualifier();
private:
	COWIntrusiveReference<QUALData> m_pdata;
	friend bool operator<(const CIMQualifier& x, const CIMQualifier& y);
};

} // end namespace OpenWBEM

typedef OpenWBEM::CIMQualifier OW_CIMQualifier OW_DEPRECATED; // in 3.0.0

#endif
