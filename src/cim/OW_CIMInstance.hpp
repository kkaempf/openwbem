/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_CIMINSTANCE_HPP_INCLUDE_GUARD_
#define OW_CIMINSTANCE_HPP_INCLUDE_GUARD_


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
	 */
	OW_CIMInstance();

	/**
	 * This OW_CIMInstance object will be NULL. All subsequent operations on 
	 * an instance of this type will fail.
	 */
	explicit OW_CIMInstance(OW_CIMNULL_t);

	/**
	 * Copy ctor
	 * @param arg The OW_CIMInstance that this object will be a copy of.
	 */
	OW_CIMInstance(const OW_CIMInstance& arg);

	/**
	 * Create an OW_CIMInstance.
	 * @param name	The class name of this OW_CIMInstance.
	 */
	explicit OW_CIMInstance(const OW_String& name);

	/**
	 * Create an OW_CIMInstance.
	 * @param name	The class name of this OW_CIMInstance as a NULL terminated string.
	 */
	explicit OW_CIMInstance(const char* name);

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
	 * @return a reference to *this
	 */
	OW_CIMInstance& setKeys(const OW_CIMPropertyArray& keys);

	/**
	 * Sets the class name for this instance.
	 * @param name	The new class name for this instance.
	 * @return a reference to *this
	 */
	OW_CIMInstance& setClassName(const OW_String& name);

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
	 * @return a reference to *this
	 */
	OW_CIMInstance& removeQualifier(const OW_String& qualName);

	/**
	 * Set the qualifiers for this instance. Any old qualifiers will removed.
	 * @param quals An OW_CIMQualifierArray with the new qualifers for this
	 *		instance.
	 * @return a reference to *this
	 */
	OW_CIMInstance& setQualifiers(const OW_CIMQualifierArray& quals);

	/**
	 * Set/Add a qualifier to this instance's qualifier list
	 * @param qual	The qualifier to add to this instance.
	 * @return a reference to *this
	 */
	OW_CIMInstance& setQualifier(const OW_CIMQualifier& qual);

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
	 * @return a reference to *this
	 */
	OW_CIMInstance& setProperties(const OW_CIMPropertyArray& props);

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
	 * Gets a property with the specified name.
	 * @param name The name of the property to retrieve.
	 * @return The OW_CIMProperty identified by the name on success.
	 * @throws an OW_NoSuchPropertyException if the property is not found or NULL.
	 */
	OW_CIMProperty getPropertyT(const OW_String& name) const;

	/**
	 * @return An OW_CIMPropertyArray that contains all of the keys for this
	 * instance.
	 */
	OW_CIMPropertyArray getKeyValuePairs() const;

	/**
	 * Update the property values for this instance
	 * @param props An OW_CIMPropertyArray that contains the properties to update
	 *		this instance's properties with.
	 * @return a reference to *this
	 */
	OW_CIMInstance& updatePropertyValues(const OW_CIMPropertyArray& props);

	/**
	 * Update a single property value.
	 * @param prop	The OW_CIMProperty to update in this instance. This
	 *		OW_CIMProperty with contain the new value.
	 * @return a reference to *this
	 */
	OW_CIMInstance& updatePropertyValue(const OW_CIMProperty& prop);

	/**
	 * Update the value of a property if it exists. Otherwise add a new one.
	 * @param name	The name of the property to add or update.
	 * @param cv The OW_CIMValue that contains the new value for the property.
	 * @return a reference to *this
	 */
	OW_CIMInstance& setProperty(const OW_String& name, const OW_CIMValue& cv);

	/**
	 * Update a property in the property list if it exists. Otherwise add a
	 * new one.
	 * @param prop The property to add or update.
	 * @return a reference to *this
	 */
	OW_CIMInstance& setProperty(const OW_CIMProperty& prop);

	/**
	 * Remove a property from this OW_CIMInstance
	 * @param propName The name of the property to remove.
	 * @return a reference to *this
	 */
	OW_CIMInstance& removeProperty(const OW_String& propName);

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
	 * @param includeQualifiers If false, all quailifiers will be omitted.
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
	 * @param includeQualifiers If false, all quailifiers will be omitted.
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
	 * Create a new OW_CIMInstance from this OW_CIMInstance using the specified
	 * criteria.  This is useful for implementing enumInstances correctly
	 * for the localOnly and deep parameters.
	 * @param localOnly If true, all inherited properties and qualifiers will be
	 *		omitted.
	 * @param deep If false, all derived properties and qualifiers will be omitted.
	 * @param includeQualifiers If false, all quailifiers will be omitted.
	 * @param includeClassOrigin If true, all element names will include the
	 *		class origin.
	 * @param propertyList A pointer to an array of property names that
	 *		specifies the only properties to be included in the returned
	 *		instance. If NULL, all properties are included. If not NULL and
	 *		the array is empty, no properties are included. If not NULL and
	 *		the array is not empty, only those properties will be included.
	 * @param requestedClass The class that was requested in the enumInstances
	 * 		call.  This is used in filtering the properties.
	 * @param cimClass The class of this instance. This is used in filtering
	 *		the properties.
	 * @return An OW_CIMInstance object based on this one, filtered according
	 *		to the specified criteria.
	 */
	OW_CIMInstance clone(OW_Bool localOnly, OW_Bool deep, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_CIMClass& requestedClass, const OW_CIMClass& cimClass) const;

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
	 * @return a reference to *this
	 */
	OW_CIMInstance& syncWithClass(const OW_CIMClass& cc, OW_Bool includeQualifiers=false);

	/**
	 * Create an instance with the set of changes that will occur for a 
	 * modifyInstance call.  This instance is the new instance.  The
	 * previous instance is passed in.  The instance that is returned
	 * is what the new instance will be after modifyInstance is applied.
	 * @param previousInstance The instance before being modified.
	 * @param includeQualifiers "If the IncludeQualifiers input parameter 
	 *	is true, this specifies that the Qualifiers are modified as specified
	 *	in the ModifiedInstance.  If false, Qualifiers in the ModifiedInstance 
	 *	are ignored and no Qualifiers are explicitly modified in the specified 
	 *	Instance."
	 * @param propertyList "If the PropertyList input parameter is not NULL, 
	 *	the members of the array define one or more Property names.  Only those
	 *	properties specified in the PropertyList are modified as specified in 
	 *	the ModifiedInstance.  Properties of the ModifiedInstance that are 
	 *	missing from the PropertyList are ignored.  If the PropertyList input 
	 *	parameter is an empty array this signifies that no Properties are 
	 *	explicitly modified in the specified Instance. If the PropertyList 
	 *	input parameter is NULL this specifies that all Properties are updated 
	 *	in the specified Instance."
	 * @param theClass The class of this instance.
	 */
	OW_CIMInstance createModifiedInstance(
		const OW_CIMInstance& previousInstance,
		OW_Bool includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_CIMClass& theClass) const;

	/**
	 * @return The name of the class for this instance.
	 */
	virtual OW_String getName() const;

	/**
	 * Sets the class name for this instance.
	 * @param name	The new class name for this instance.
	 * @return a reference to *this
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

	/**
	 * This compares the properties of the instances, and returns true
	 * if they are equal.  It will sort the properties before comparing,
	 * so the order of the properties is irrelevant.
	 * @param other The instance to compare
	 * @return true if the properties are the same
	 */
	bool propertiesAreEqualTo(const OW_CIMInstance& other) const;

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


#endif

