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

#ifndef __OW_CIMINSTANCE_HPP__
#define __OW_CIMINSTANCE_HPP__


#include "OW_config.h"

#include "OW_COWReference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMDataType.hpp" // for OW_CIMDataType::INVALID
#include "OW_String.hpp"
#include "OW_Array.hpp"

/**
 * The OW_CIMInstance class encapsulates all information pertinent to a
 * CIMInstance. OW_CIMInstances are ref counted, copy on write objects.
 * It is possible to have an OW_CIMInstance object that is NULL. The method
 * to check for this condition is as follows:
 *
 *		OW_CIMInstance ci = ch.getInstance(...);
 *		if(!ci)
 *		{
 *			// Null instance
 *		}
 *		else
 *		{
 *			// Valid instance
 *		}
 */
class OW_CIMInstance : public OW_CIMElement
{
public:

	struct INSTData;

	/**
	 * Default ctor
	 * @param notNull If false, This OW_CIMInstance object will not contain any
	 * underlying implementation/data. All subsequent operations on an instance
	 * of this type will fail. If true, this instance will be set to default
	 * values.
	 */
	explicit OW_CIMInstance(OW_Bool notNull=false);

	/**
	 * Copy ctor
	 * @param arg The OW_CIMInstance that this object will be a copy of.
	 */
	OW_CIMInstance(const OW_CIMInstance& arg);

	/**
	 * Create an OW_CIMInstance with a name.
	 * @param name	The name of this OW_CIMInstance as an OW_String.
	 */
	explicit OW_CIMInstance(const OW_String& name);

	/**
	 * Create an OW_CIMInstance with a name.
	 * @param name	The name of this OW_CIMInstance as a NULL terminated string.
	 */
	explicit OW_CIMInstance(const char* name);

	/**
	 * Create an OW_CIMInstance from an OW_XMLNode.
	 * @param node	The OW_XMLNode that contains the infor for this instance.
	 */
	//OW_CIMInstance(const OW_XMLNode& node);

	/**
	 * Destroy this OW_CIMInstance object.
	 */
	~OW_CIMInstance();

	/**
	 * Set this to a null object. All subsequent operation will fail after this
	 * call is made.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMInstance to assign to this one.
	 * @return A reference to this object after the assignment is made.
	 */
	OW_CIMInstance& operator= (const OW_CIMInstance& arg);

	/**
	 * @return The name of the class for this instance.
	 */
	OW_String getClassName() const;

	/**
	 * Set the keys for this instance
	 */
	void setKeys(const OW_CIMPropertyArray& keys);

	/**
	 * Sets the class name for this instance.
	 * @param name	The new class name for this instance.
	 */
	void setClassName(const OW_String& name);

	/**
	 * @return The qualifiers for this instance as an array of OW_CIMQualifiers.
	 */
	OW_CIMQualifierArray getQualifiers() const;

	/**
	 * Get the qualifier associated with the given name.
	 * @param qualName The name of the qualifier to retrieve.
	 * @return A valid OW_CIMQalifier on success. Otherwise a null
	 * OW_CIMQualifier
	 */
	OW_CIMQualifier getQualifier(const OW_String& qualName) const;

	/**
	 * Remove the named qualifier from this OW_CIMInstance.
	 * @param qualName The name of the qualifier to remove.
	 */
	void removeQualifier(const OW_String& qualName);

	/**
	 * Set the qualifiers for this instance. Any old qualifiers will removed.
	 * @param quals An OW_CIMQualifierArray with the new qualifers for this
	 *		instance.
	 */
	void setQualifiers(const OW_CIMQualifierArray& quals);

	/**
	 * Set/Add a qualifier to this instance's qualifier list
	 * @param qual	The qualifier to add to this instance.
	 */
	void setQualifier(const OW_CIMQualifier& qual);

	/**
	 * Get all the properties of a specific datatype from this instance.
	 * @param valueDataType	All properties returned must have this datatype.
	 * 	If OW_CIMDataType::INVALID is specified, all properties will be
	 *		returned.
	 * @return The properties associated with this instance if valueDataType is
	 * OW_CIMDataType::INVALID. Otherwise only properties that have a values
	 * that have a data type specified by the valueDataType parameter.
	 */
	OW_CIMPropertyArray getProperties(
		OW_Int32 valueDataType = OW_CIMDataType::INVALID) const;

	/**
	 * Set the properties associated with this instance.
	 * @param props An OW_CIMPropertyArray that contains the new properties for
	 * 	this instance.
	 */
	void setProperties(const OW_CIMPropertyArray& props);

	/**
	 * Get the property identified by a given name and origin class.
	 * @param name The name of the property to retrieve.
	 * @param originClass The name of the origin class of the property.
	 * @return The OW_CIMProperty identified by the given parameters on success.
	 * Otherwise a null CIMProperty object.
	 */
	OW_CIMProperty getProperty(const OW_String& name,
		const OW_String& originClass) const;

	/**
	 * Gets a property with the specified name.
	 * @param name The name of the property to retrieve.
	 * @return The OW_CIMProperty identified by the name on success. Otherwise a
	 * null CIMProperty object.
	 */
	OW_CIMProperty getProperty(const OW_String& name) const;

	/**
	 * Get the alias name associated with this instance.
	 * @return The alias name for this instance if it has one. Otherwise return
	 * an empty string.
	 */
	OW_String getAlias() const;

	/**
	 * Set the alias name for this instance.
	 * @param aliasName The new alias name for this instance.
	 */
	void setAlias(const OW_String& aliasName);

	/**
	 * @return An OW_CIMPropertyArray that contains all of the keys for this
	 * instance.
	 */
	OW_CIMPropertyArray getKeyValuePairs() const;

	/**
	 * Update the property values for this instance
	 * @param props An OW_CIMPropertyArray that contains the properties to update
	 *		this instance's properties with.
	 */
	void updatePropertyValues(const OW_CIMPropertyArray& props);

	/**
	 * Update a single property value.
	 * @param prop	The OW_CIMProperty to update in this instance. This
	 *		OW_CIMProperty with contain the new value.
	 */
	void updatePropertyValue(const OW_CIMProperty& prop);

	/**
	 * Update the value of a property if it exists. Otherwise add a new one.
	 * @param name	The name of the property to add or update.
	 * @param cv The OW_CIMValue that contains the new value for the property.
	 */
	void setProperty(const OW_String& name, const OW_CIMValue& cv);

	/**
	 * Update a property in the property list if it exists. Otherwise add a
	 * new one.
	 * @param prop The property to add or update.
	 */
	void setProperty(const OW_CIMProperty& prop);

	/**
	 * Remove a property from this OW_CIMInstance
	 * @param propName The name of the property to remove.
	 */
	void removeProperty(const OW_String& propName);

	/**
	 * Create an OW_CIMInstance with properties and qualifiers from this
	 * instance based on filtering criteria. All properties and qualifiers that
	 * designate keys will be retained.
	 * @param propertyList The list of property names to include in the instance
	 *		that is created.
	 * @param includeQualifiers If true, All qualifiers will be retained on the
	 *		instance and properties. Otherwise all qualifiers will be omitted,
	 *		except those that designate key properties.
	 * @param includeClassOrigin If true, the class origin will be included on
	 * 	all properties and qualifiers.
	 * @param ignorePropertyList If true the propertyList parameter will be
	 *		ignored.
	 * @return An OW_CIMInstance with properties and qualifiers from this
	 * instance based on the filtering criteria. All properties and qualifiers
	 * that designate keys will be retained.
	 */
	OW_CIMInstance filterProperties(const OW_StringArray& propertyList,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		OW_Bool ignorePropertyList=false) const;

	/**
	 * Create a new OW_CIMInstance from this OW_CIMInstance using the specified
	 * criteria
	 * @param localOnly If true, all inherited properties and qualifiers will be
	 *		omitted.
	 * @param includeQualifiers If true, all quailifiers will be omitted.
	 * @param includeClassOrigin If true, all element names will include the
	 *		class origin.
	 * @param propertyList An array of property names that specifies the only
	 *		properties to be included in the returned instance.
	 * @param noProps If true, all properties will be omitted from the returned
	 *		OW_CIMInstance object regardless of the property list parameter.
	 * @return An OW_CIMInstance object based on this one, filtered according
	 *		to the specified criteria.
	 */
	OW_CIMInstance clone(OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray& propertyList=OW_StringArray(),
		OW_Bool noProps=false) const;

	/**
	 * Create a new OW_CIMInstance from this OW_CIMInstance using the specified
	 * criteria
	 * @param localOnly If true, all inherited properties and qualifiers will be
	 *		omitted.
	 * @param includeQualifiers If true, all quailifiers will be omitted.
	 * @param includeClassOrigin If true, all element names will include the
	 *		class origin.
	 * @param propertyList A pointer to an array of property names that
	 *		specifies the only properties to be included in the returned
	 *		instance. If NULL, all properties are included. If not NULL and
	 *		the array is empty, no properties are included. If not NULL and
	 *		the array is not empty, only those properties will be included.
	 * @return An OW_CIMInstance object based on this one, filtered according
	 *		to the specified criteria.
	 */
	OW_CIMInstance clone(OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList) const;

	/**
	 * Synchronize this instance with the given class. This will ensure that
	 * all properties found on the class exist on thist instance. It will also
	 * optionally move all qualifiers from the class to the instance.
	 * @param cc	The class to synchronize this instance with.
	 * @param includeQualifiers If false, the instance will not contain any
	 *		qualifiers (except key designators) after the operation. If true, all
	 *		relevant qualifiers are copied from the class to the instance.
	 * @param localOnly If true, only non-propagated properties,qualifier are
	 *		kept in the instance.
	 */
	void syncWithClass(const OW_CIMClass& cc, OW_Bool includeQualifiers=false);

	/**
	 * Gets the name of this instance.
	 * The name is made by concating all key properties and their values
	 * @return The name of this instance.
	 */
	virtual OW_String getName() const;

	/**
	 * Set the name of this instance.
	 * @param name The new name of the class for this instance.
	 */
	virtual void setName(const OW_String& name);

	/**
	 * Read this OW_CIMInstance from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * Write this OW_CIMInstance to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * @return The MOF representation of this OW_CIMInstance as an OW_String.
	 */
	virtual OW_String toMOF() const;

	/**
	 * @return The string representation of the OW_CIMInstance object.
	 */
	virtual OW_String toString() const;

private:
	struct dummy { void nonnull() {}; };
	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (!m_pdata.isNull()) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (!m_pdata.isNull()) ? 0: &dummy::nonnull; }
	
protected:

	void _buildKeys();

	OW_COWReference<INSTData> m_pdata;
	
	friend bool operator<(const OW_CIMInstance& x, const OW_CIMInstance& y);
};


#endif	// __OW_CIMINSTANCE_HPP__
