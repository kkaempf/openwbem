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

#ifndef OW_CIMPROPERTY_HPP_INCLUDE_GUARD_
#define OW_CIMPROPERTY_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_COWIntrusiveReference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMNULL.hpp"
#include "OW_WBEMFlags.hpp"

namespace OpenWBEM
{

/**
 * The CIMProperty class encapsulates all data and functionality petinent to
 * a CIM property found in a CIM class or a CIM Instance.
 */
class OW_COMMON_API CIMProperty : public CIMElement
{
public:
	struct PROPData;
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
	explicit CIMProperty(const String& name);
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
	CIMProperty(const String& name, const CIMValue& value);
	/**
	 * Create an CIMProperty object with a name and a type.
	 * @param name The name for this CIMProperty object.
	 * @param dt The type of this CIMProperty
	 */
	CIMProperty(const String& name, const CIMDataType& dt);
	/**
	 * Copy constructor
	 * @param arg The CIMProperty to make this object a copy of.
	 */
	CIMProperty(const CIMProperty& arg);
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

	typedef COWIntrusiveReference<PROPData> CIMProperty::*safe_bool;
	/**
	 * @return true If this CIMProperty has an implementation.
	 */
	operator safe_bool () const
		{  return m_pdata ? &CIMProperty::m_pdata : 0; }
	bool operator!() const
		{  return !m_pdata; }
	/**
	 * @return A copy of this CIMProperty object.
	 */
	CIMProperty clone() const {  return clone(WBEMFlags::E_INCLUDE_QUALIFIERS, WBEMFlags::E_INCLUDE_CLASS_ORIGIN); }
	/**
	 * Create a copy of this CIMProperty object base on filtering criteria.
	 * @param includeQualifiers If true, include qualifiers of this object.
	 * @param includeClassOrigin If true, include the class origin of this obj.
	 * @return A copy of this CIMProperty object
	 */
	CIMProperty clone(WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin) const;
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
	String getOriginClass() const;
	/**
	 * Set the origin class for this property.
	 * @param originCls	The name of the origin class of this property.
	 * @return a reference to *this
	 */
	CIMProperty& setOriginClass(const String& originCls);
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
	CIMProperty& setOverridingProperty(const String& opname);
	/**
	 * @return The name of the overriding property.
	 */
	String getOverridingProperty() const;
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
	CIMQualifier getQualifier(const String& name) const;
	/**
	 * Get the qualifier associated with the given name from this property.
	 * @param name	The name of the qualifier to retrieve.
	 * @return The CIMQualifier associated with the given name if there is
	 * one. Otherwise a NoSuchQualifierException is thrown.
	 */
	CIMQualifier getQualifierT(const String& name) const;
	/**
	 * Set the value of a qaulifier on this property.
	 * @param qual	An CIMQualifier to use to update the property qualifier.
	 * @return a reference to *this
	 */
	CIMProperty& setQualifier(const CIMQualifier& qual);
	/**
	 * Add a qualifier to this property.
	 * @param qual	The CIMQualifier to add to this property.
	 * @return a reference to *this
	 */
	CIMProperty& addQualifier(const CIMQualifier& qual);
	/**
	 * Remove a qualifier from this property.
	 * @param qname	The name of the qualifier to remove.
	 * @return true if the qualifier was found and removed. Otherwise false.
	 */
	bool removeQualifier(const String& name);
	/**
	 * @return true if this property has the key qualifier on it.
	 */
	bool isKey() const;
	/**
	 * Create a filtered version of this property.
	 * @param localOnly If true and this property was inherited from another
	 *		class, then an empty CIMProprty will be return.
	 * @param includeQualifiers If true, all qualifier on this property will be
	 *		included in the new CIMProperty.
	 * @return A CIMProperty object based on the given filtering criteria.
	 */
	CIMProperty filter(WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers) const;
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
	 * Clear all qualifiers from this property.
	 * @return a reference to *this
	 */
	CIMProperty& clearQualifiers();
	/**
	 * Clear all qualifiers except for the Key qualifier from this property.
	 * @return a reference to *this
	 */
	CIMProperty& clearNonKeyQualifiers();
	/**
	 * @return The name of this CIMProperty
	 */
	virtual String getName() const;
	/**
	 * Set the name of this CIMProperty object.
	 * @param name	The new name for this CIMProperty object.
	 */
	virtual void setName(const String& name);
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
	bool hasTrueQualifier(const String& name) const;
private:
	COWIntrusiveReference<PROPData> m_pdata;
	friend bool operator<(const CIMProperty& x, const CIMProperty& y);
};

} // end namespace OpenWBEM

typedef OpenWBEM::CIMProperty OW_CIMProperty OW_DEPRECATED; // in 3.0.0

#endif
