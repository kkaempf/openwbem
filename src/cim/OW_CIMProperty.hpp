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

#ifndef OW_CIMPROPERTY_HPP_INCLUDE_GUARD_
#define OW_CIMPROPERTY_HPP_INCLUDE_GUARD_


#include "OW_config.h"

#include "OW_COWReference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMDataType.hpp"

/**
 * The OW_CIMProperty class encapsulates all data and functionality petinent to
 * a CIM property found in a CIM class or a CIM Instance.
 */
class OW_CIMProperty : public OW_CIMElement
{
public:
	struct PROPData;

	static const char* const NAME_PROPERTY;

	/**
	 * Create a new OW_CIMProperty object.
	 * @param notNull if false, this object will not have any data or
	 *		implementation associated with it.
	 */
	explicit OW_CIMProperty(OW_Bool notNull=false);

	/**
	 * Create an OW_CIMProperty object with a given name.
	 * @param name	The name for this OW_CIMProperty object
	 */
	explicit OW_CIMProperty(const OW_String& name);

	/**
	 * Create an OW_CIMProperty object with a given name.
	 * @param name	The name for this OW_CIMProperty object as a NULL terminated
	 *		string.
	 */
	explicit OW_CIMProperty(const char* name);

	/**
	 * Create an OW_CIMProperty object with a name and a value.
	 * @param name The name for this OW_CIMProperty object.
	 * @param value The value of this OW_CIMProperty
	 */
	OW_CIMProperty(const OW_String& name, const OW_CIMValue& value);

	/**
	 * Create an OW_CIMProperty object with a name and a type.
	 * @param name The name for this OW_CIMProperty object.
	 * @param dt The type of this OW_CIMProperty
	 */
	OW_CIMProperty(const OW_String& name, const OW_CIMDataType& dt);

	/**
	 * Copy constructor
	 * @param arg The OW_CIMProperty to make this object a copy of.
	 */
	OW_CIMProperty(const OW_CIMProperty& arg);

	/**
	 * Destroy this OW_CIMProperty object.
	 */
	~OW_CIMProperty();

	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMProperty object to assign to this one.
	 * @return A reference to this object after the assignment is made.
	 */
	OW_CIMProperty& operator= (const OW_CIMProperty& arg);

	/**
	 * @return true If this OW_CIMProperty has an implementation.
	 */
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
	 * @return A copy of this OW_CIMProperty object.
	 */
	OW_CIMProperty clone() const {  return clone(true, true); }

	/**
	 * Create a copy of this OW_CIMProperty object base on filtering criteria.
	 * @param includeQualifier		If true, include qualifiers of this object.
	 * @param includeClassOrigin	If true, include the class origin of this obj.
	 * @return A copy of this OW_CIMProperty object
	 */
	OW_CIMProperty clone(OW_Bool includeQualifier,
		OW_Bool includeClassOrigin) const;

	/**
	 * Get the qualifiers for this property
	 * @return An OW_CIMQualifierArray with the qualifiers for this property
	 */
	OW_CIMQualifierArray getQualifiers() const;

	/**
	 * Set the qualifiers for this property object.
	 * @param quals	An OW_CIMQualifierArray with the new qualifiers for this
	 *						property object.
	 */
	void setQualifiers(const OW_CIMQualifierArray& quals);

	/**
	 * Get the origin class for this property.
	 * @return The name of the origin class of this property.
	 */
	OW_String getOriginClass() const;

	/**
	 * Set the origin class for this property.
	 * @param originCls	The name of the origin class of this property.
	 */
	void setOriginClass(const OW_String& originCls);

	/**
	 * Set the value of this property.
	 * @param val	The OW_CIMValue for this property.
	 */
	void setValue(const OW_CIMValue& val);

	/**
	 * @return The OW_CIMValue object for this property.  The value may be NULL.
	 */
	OW_CIMValue getValue() const;

	/**
	 * @return The OW_CIMValue object for this property.
	 * @throws OW_NULLValueException if the value is NULL.
	 */
	OW_CIMValue getValueT() const;

	/**
	 * Set the data type for this property.
	 * @param type	The OW_CIMDataType for this property
	 */
	void setDataType(const OW_CIMDataType& type);

	/**
	 * Set the data type for this property.
	 * @param type	The OW_CIMDataType::Type for this property
	 */
	void setDataType(const OW_CIMDataType::Type& type);

	/**
	 * @return The OW_CIMDataType for this property.
	 */
	OW_CIMDataType getDataType() const;

	/**
	 * @return The size of the data associated with this property
	 */
	OW_Int32 getSize() const;

	/**
	 * Set the size of the data for this property.
	 * @param size	The size of the data associated with property.
	 */
	void setDataSize(OW_Int32 size);

	/**
	 * Set the overriding property name.
	 * @param opname	The name of the overriding property.
	 */
	void setOverridingProperty(const OW_String& opname);

	/**
	 * @return The name of the overriding property.
	 */
	OW_String getOverridingProperty() const;

	/**
	 * @return true if this property is a reference. Otherwise return false.
	 */
	OW_Bool isReference() const;

	/**
	 * Get the qualifier associated with the given name from this property.
	 * @param name	The name of the qualifier to retrieve.
	 * @return The OW_CIMQualifier associated with the given name if there is
	 * one. Otherwise return a NULL OW_CIMQualifier.
	 */
	OW_CIMQualifier getQualifier(const OW_String& name) const;

	/**
	 * Set the value of a qaulifier on this property.
	 * @param qual	An OW_CIMQualifier to use to update the property qualifier.
	 * @return An OW_CIMQualifier that represent the value of the
	 * property after it is set.
	 */
	OW_CIMQualifier setQualifier(const OW_CIMQualifier& qual);

	/**
	 * Add a qualifier to this property.
	 * @param qual	The OW_CIMQualifier to add to this property.
	 */
	void addQualifier(const OW_CIMQualifier& qual);

	/**
	 * Remove a qualifier from this property.
	 * @param qname	The name of the qualifier to remove.
	 * @return true if the qualifier was found and removed. Otherwise false.
	 */
	OW_Bool removeQualifier(const OW_String& name);

	/**
	 * @return true if this property has the key qualifier on it.
	 */
	OW_Bool isKey() const;

	/**
	 * Create a filtered version of this property.
	 * @param localOnly If true and this property was inherited from another
	 *		class, then an empty OW_CIMProprty will be return.
	 * @param includeQualifiers If true, all qualifier on this property will be
	 *		included in the new OW_CIMProperty.
	 * @return A OW_CIMProperty object based on the given filtering criteria.
	 */
	OW_CIMProperty filter(OW_Bool localOnly, OW_Bool includeQualifiers) const;

	/**
	 * Set the propagated flag on this property.
	 * @param propagated	If true this property is propagated. Otherwise false.
	 */
	void setPropagated(OW_Bool propagated=true);

	/**
	 * Get the propagated flag for this property.
	 * @return true if this property is propagated. Otherwise false.
	 */
	OW_Bool getPropagated() const;

	/**
	 * Clear all qualifier from property.
	 */
	void clearQualifiers();

	/**
	 * @return The name of this OW_CIMProperty
	 */
	virtual OW_String getName() const;

	/**
	 * Set the name of this OW_CIMProperty object.
	 * @param name	The new name for this OW_CIMProperty object.
	 */
	virtual void setName(const OW_String& name);

	/**
	 * Write this OW_CIMProperty to the given output stream.
	 * @param ostrm	The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * Write this OW_CIMProperty, optionally omitting all qualifiers except
	 * the key qualifier.
	 * @param ostrm	The output stream to write this object to.
	 * @param includeQulifiers If true write all qualifiers for property
	 */
	virtual void writeObject(std::ostream &ostrm,
		OW_Bool includeQualifiers) const;
	
	/**
	 * Read this OW_CIIMProperty object from the given input stream.
	 * @param istrm	The input stream to read this OW_CIMElement from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * @return The string representation of this OW_CIMProperty.
	 */
	virtual OW_String toString() const;
	
	/**
	 * @return An OW_String that contains the MOF representation of this
	 * OW_CIMProperty object.
	 */
	virtual OW_String toMOF() const;
	
private:

	OW_COWReference<PROPData> m_pdata;

	friend bool operator<(const OW_CIMProperty& x, const OW_CIMProperty& y);
};


#endif

