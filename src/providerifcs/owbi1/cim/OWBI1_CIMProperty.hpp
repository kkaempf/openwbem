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

#ifndef OWBI1_CIMPROPERTY_HPP_INCLUDE_GUARD_
#define OWBI1_CIMPROPERTY_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_COWIntrusiveReference.hpp"
#include "OWBI1_CIMElement.hpp"
#include "OWBI1_CIMFwd.hpp"
#include "OWBI1_CIMDataType.hpp"
#include "OWBI1_CIMNULL.hpp"
#include "OWBI1_WBEMFlags.hpp"
#include "OWBI1_CIMName.hpp" // necessary for implicit conversion (const char* -> CIMName) to work

namespace OWBI1
{

/**
 * The CIMProperty class encapsulates all data and functionality petinent to
 * a CIM property found in a CIM class or a CIM Instance.
 */
class OWBI1_OWBI1PROVIFC_API CIMProperty : public CIMElement
{
public:
	static const char* const NAME_PROPERTY;
	/**
	 * Create a new CIMProperty object.
	 */
	CIMProperty();
	/**
	 * Create a NULL CIMProperty object.
	 */
	explicit CIMProperty(CIMNULL_t);
	/**
	 * Create an CIMProperty object with a given name.
	 * @param name	The name for this CIMProperty object
	 */
	explicit CIMProperty(const CIMName& name);
	/**
	 * Create an CIMProperty object with a given name.
	 * @param name	The name for this CIMProperty object as a NULL terminated
	 *		string.
	 */
	explicit CIMProperty(const char* name);
	/**
	 * Create an CIMProperty object with a name and a value.
	 * @param name The name for this CIMProperty object.
	 * @param value The value of this CIMProperty
	 */
	CIMProperty(const CIMName& name, const CIMValue& value);
	/**
	 * Create an CIMProperty object with a name and a type.
	 * @param name The name for this CIMProperty object.
	 * @param dt The type of this CIMProperty
	 */
	CIMProperty(const CIMName& name, const CIMDataType& dt);
	/**
	 * Copy constructor
	 * @param arg The CIMProperty to make this object a copy of.
	 */
	CIMProperty(const CIMProperty& arg);

	explicit CIMProperty(const detail::CIMPropertyRepRef& rep);
	/**
	 * Destroy this CIMProperty object.
	 */
	~CIMProperty();
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMProperty object to assign to this one.
	 * @return A reference to this object after the assignment is made.
	 */
	CIMProperty& operator= (const CIMProperty& arg);

	typedef detail::CIMPropertyRepRef CIMProperty::*safe_bool;
	/**
	 * @return true If this CIMProperty has an implementation.
	 */
	operator safe_bool () const;
	bool operator!() const;
	/**
	 * Get the qualifiers for this property
	 * @return An CIMQualifierArray with the qualifiers for this property
	 */
	CIMQualifierArray getQualifiers() const;
	/**
	 * Set the qualifiers for this property object.
	 * @param quals	An CIMQualifierArray with the new qualifiers for this
	 *						property object.
	 * @return a reference to *this
	 */
	CIMProperty& setQualifiers(const CIMQualifierArray& quals);
	/**
	 * Get the origin class for this property.
	 * @return The name of the origin class of this property.
	 */
	CIMName getOriginClass() const;
	/**
	 * Set the origin class for this property.
	 * @param originCls	The name of the origin class of this property.
	 * @return a reference to *this
	 */
	CIMProperty& setOriginClass(const CIMName& originCls);
	/**
	 * Set the value of this property.
	 * @param val	The CIMValue for this property.
	 * @return a reference to *this
	 */
	CIMProperty& setValue(const CIMValue& val);
	/**
	 * @return The CIMValue object for this property.  The value may be NULL.
	 */
	CIMValue getValue() const;
	/**
	 * @return The CIMValue object for this property.
	 * @throws NULLValueException if the value is NULL.
	 */
	CIMValue getValueT() const;
	/**
	 * Set the data type for this property.
	 * @param type	The CIMDataType for this property
	 * @return a reference to *this
	 */
	CIMProperty& setDataType(const CIMDataType& type);
	/**
	 * Set the data type for this property.
	 * @param type	The CIMDataType::Type for this property
	 * @return a reference to *this
	 */
	CIMProperty& setDataType(const CIMDataType::Type& type);
	/**
	 * @return The CIMDataType for this property.
	 */
	CIMDataType getDataType() const;
	/**
	 * @return The size of the data associated with this property
	 */
	Int32 getSize() const;
	/**
	 * Set the size of the data for this property.
	 * @param size	The size of the data associated with property.
	 * @return a reference to *this
	 */
	CIMProperty& setDataSize(Int32 size);
	/**
	 * Set the overriding property name.
	 * @param opname	The name of the overriding property.
	 * @return a reference to *this
	 */
	CIMProperty& setOverridingProperty(const CIMName& opname);
	/**
	 * @return The name of the overriding property.
	 */
	CIMName getOverridingProperty() const;
	/**
	 * @return true if this property is a reference. Otherwise return false.
	 */
	bool isReference() const;
	/**
	 * Get the qualifier associated with the given name from this property.
	 * @param name	The name of the qualifier to retrieve.
	 * @return The CIMQualifier associated with the given name if there is
	 * one. Otherwise return a NULL CIMQualifier.
	 */
	CIMQualifier getQualifier(const CIMName& name) const;
	/**
	 * Get the qualifier associated with the given name from this property.
	 * @param name	The name of the qualifier to retrieve.
	 * @return The CIMQualifier associated with the given name if there is
	 * one. Otherwise a NoSuchQualifierException is thrown.
	 */
	CIMQualifier getQualifierT(const CIMName& name) const;
	/**
	 * Set the value of a qaulifier on this property.
	 * @param qual	An CIMQualifier to use to update the property qualifier.
	 * @return a reference to *this
	 */
	CIMProperty& setQualifier(const CIMQualifier& qual);
	/**
	 * Add a qualifier to this property.
	 * @param qual	The CIMQualifier to add to this property.
	 * @return true if added, false if it already exists
	 */
	bool addQualifier(const CIMQualifier& qual);
	/**
	 * Remove a qualifier from this property.
	 * @param qname	The name of the qualifier to remove.
	 * @return true if the qualifier was found and removed. Otherwise false.
	 */
	bool removeQualifier(const CIMName& name);
	/**
	 * @return true if this property has the key qualifier on it.
	 */
	bool isKey() const;
	/**
	 * Set the propagated flag on this property.
	 * @param propagated	If true this property is propagated. Otherwise false.
	 * @return a reference to *this
	 */
	CIMProperty& setPropagated(bool propagated=true);
	/**
	 * Get the propagated flag for this property.
	 * @return true if this property is propagated. Otherwise false.
	 */
	bool getPropagated() const;
	/**
	 * @return The name of this CIMProperty
	 */
	virtual CIMName getName() const;
	/**
	 * Set the name of this CIMProperty object.
	 * @param name	The new name for this CIMProperty object.
	 */
	virtual void setName(const CIMName& name);
	/**
	 * Write this CIMProperty to the given output stream.
	 * @param ostrm	The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;
	/**
	 * Write this CIMProperty, optionally omitting all qualifiers except
	 * the key qualifier.
	 * @param ostrm	The output stream to write this object to.
	 * @param includeQulifiers If true write all qualifiers for property
	 */
	virtual void writeObject(std::ostream &ostrm,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers) const;
	
	/**
	 * Read this CIIMProperty object from the given input stream.
	 * @param istrm	The input stream to read this CIMElement from.
	 */
	virtual void readObject(std::istream &istrm);
	/**
	 * @return The string representation of this CIMProperty.
	 */
	virtual String toString() const;
	
	/**
	 * @return An String that contains the MOF representation of this
	 * CIMProperty object.
	 */
	virtual String toMOF() const;
	
	/**
	 * Test if this property has a qualifier that is a boolean type with a
	 * value of true.
	 * @param name	The name of the qualifier to test.
	 * @return true if the qualifier exists and has a value of true.
	 */
	bool hasTrueQualifier(const CIMName& name) const;

	detail::CIMPropertyRepRef getRep() const;
private:

#ifdef OWBI1_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	detail::CIMPropertyRepRef m_rep;

#ifdef OWBI1_WIN32
#pragma warning (pop)
#endif

};

OWBI1_OWBI1PROVIFC_API bool operator<(const CIMProperty& x, const CIMProperty& y);

} // end namespace OWBI1

#endif
