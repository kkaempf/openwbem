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

#ifndef OW_CIMINSTANCE_HPP_INCLUDE_GUARD_
#define OW_CIMINSTANCE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_COWIntrusiveReference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMDataType.hpp" // for OW_CIMDataType::INVALID
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_WBEMFlags.hpp"

namespace OpenWBEM
{

/**
 * The CIMInstance class encapsulates all information pertinent to a
 * CIMInstance. CIMInstances are ref counted, copy on write objects.
 * It is possible to have an CIMInstance object that is NULL. The method
 * to check for this condition is as follows:
 *
 *		CIMInstance ci = ch.getInstance(...);
 *		if (!ci)
 *		{
 *			// Null instance
 *		}
 *		else
 *		{
 *			// Valid instance
 *		}
 */
class CIMInstance : public CIMElement
{
public:
	struct INSTData;
	enum { SERIALIZATION_VERSION = 1 };				// Version # for serialization
	/**
	 * Default ctor
	 */
	CIMInstance();
	/**
	 * This CIMInstance object will be NULL. All subsequent operations on 
	 * an instance of this type will fail.
	 */
	explicit CIMInstance(CIMNULL_t);
	/**
	 * Copy ctor
	 * @param arg The CIMInstance that this object will be a copy of.
	 */
	CIMInstance(const CIMInstance& arg);
	/**
	 * Create an CIMInstance.
	 * @param name	The class name of this CIMInstance.
	 */
	explicit CIMInstance(const String& name);
	/**
	 * Create an CIMInstance.
	 * @param name	The class name of this CIMInstance as a NULL terminated string.
	 */
	explicit CIMInstance(const char* name);
	/**
	 * Destroy this CIMInstance object.
	 */
	~CIMInstance();
	/**
	 * Set this to a null object. All subsequent operation will fail after this
	 * call is made.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMInstance to assign to this one.
	 * @return A reference to this object after the assignment is made.
	 */
	CIMInstance& operator= (const CIMInstance& arg);
	/**
	 * @return The name of the class for this instance.
	 */
	String getClassName() const;
	/**
	 * Set the keys for this instance
	 * @return a reference to *this
	 */
	CIMInstance& setKeys(const CIMPropertyArray& keys);
	/**
	 * Sets the class name for this instance.
	 * @param name	The new class name for this instance.
	 * @return a reference to *this
	 */
	CIMInstance& setClassName(const String& name);
	/**
	 * @return the language this CIMClass is using
	 */
	String getLanguage() const;
	/**
	 * Set the language this CIMClass is using
	 * @param language The new language for this CIMClass
	 */
	void setLanguage(const String& language);
	/**
	 * @return The qualifiers for this instance as an array of CIMQualifiers.
	 */
	CIMQualifierArray getQualifiers() const;
	/**
	 * Get the qualifier associated with the given name.
	 * @param qualName The name of the qualifier to retrieve.
	 * @return A valid CIMQalifier on success. Otherwise a null
	 * CIMQualifier
	 */
	CIMQualifier getQualifier(const String& qualName) const;
	/**
	 * Remove the named qualifier from this CIMInstance.
	 * @param qualName The name of the qualifier to remove.
	 * @return a reference to *this
	 */
	CIMInstance& removeQualifier(const String& qualName);
	/**
	 * Set the qualifiers for this instance. Any old qualifiers will removed.
	 * @param quals An CIMQualifierArray with the new qualifers for this
	 *		instance.
	 * @return a reference to *this
	 */
	CIMInstance& setQualifiers(const CIMQualifierArray& quals);
	/**
	 * Set/Add a qualifier to this instance's qualifier list
	 * @param qual	The qualifier to add to this instance.
	 * @return a reference to *this
	 */
	CIMInstance& setQualifier(const CIMQualifier& qual);
	/**
	 * Get all the properties of a specific datatype from this instance.
	 * @param valueDataType	All properties returned must have this datatype.
	 * 	If CIMDataType::INVALID is specified, all properties will be
	 *		returned.
	 * @return The properties associated with this instance if valueDataType is
	 * CIMDataType::INVALID. Otherwise only properties that have a values
	 * that have a data type specified by the valueDataType parameter.
	 */
	CIMPropertyArray getProperties(
		Int32 valueDataType = CIMDataType::INVALID) const;
	/**
	 * Set the properties associated with this instance.
	 * Note: this will clobber qualifiers associated with the 
	 *       properties which may already exist on the instance. 
	 *       You may want to instead consider updatePropertyValues(), 
	 *       which does not overwrite qualifiers. 
	 * @param props An CIMPropertyArray that contains the new properties for
	 * 	this instance.
	 * @return a reference to *this
	 */
	CIMInstance& setProperties(const CIMPropertyArray& props);
	/**
	 * Get the property identified by a given name and origin class.
	 * @param name The name of the property to retrieve.
	 * @param originClass The name of the origin class of the property.
	 * @return The CIMProperty identified by the given parameters on success.
	 * Otherwise a null CIMProperty object.
	 */
	CIMProperty getProperty(const String& name,
		const String& originClass) const;
	/**
	 * Gets a property with the specified name.
	 * @param name The name of the property to retrieve.
	 * @return The CIMProperty identified by the name on success. Otherwise a
	 * null CIMProperty object.
	 */
	CIMProperty getProperty(const String& name) const;
	/**
	 * Gets a property with the specified name.
	 * @param name The name of the property to retrieve.
	 * @return The CIMProperty identified by the name on success.
	 * @throws an NoSuchPropertyException if the property is not found or NULL.
	 */
	CIMProperty getPropertyT(const String& name) const;
	/**
	 * Gets a property's value.
	 * @param name The name of the property value to retrieve.
	 * @return The CIMValue of the specified property.  It will be NULL if the
	 * property doesn't exist or the property's value is NULL.
	 */
	CIMValue getPropertyValue(const String& name) const;
	/**
	 * Test whether a property exists and has a non-NULL value.
	 * @param name The name of the property to test.
	 * @return bool true if the property exists and has a non-NULL value.
	 */
	bool propertyHasValue(const String& name) const;
	/**
	 * @return An CIMPropertyArray that contains all of the keys for this
	 * instance.
	 */
	CIMPropertyArray getKeyValuePairs() const;
	/**
	 * Update the property values for this instance
	 * @param props An CIMPropertyArray that contains the properties to update
	 *		this instance's properties with.
	 * @return a reference to *this
	 */
	CIMInstance& updatePropertyValues(const CIMPropertyArray& props);
	/**
	 * Update a single property value.
	 * @param prop	The CIMProperty to update in this instance. This
	 *		CIMProperty with contain the new value.
	 * @return a reference to *this
	 */
	CIMInstance& updatePropertyValue(const CIMProperty& prop);
	/**
	 * Update the value of a property if it exists. Otherwise add a new one.
	 * @param name	The name of the property to add or update.
	 * @param cv The CIMValue that contains the new value for the property.
	 * @return a reference to *this
	 */
	CIMInstance& setProperty(const String& name, const CIMValue& cv);
	/**
	 * Update a property in the property list if it exists. Otherwise add a
	 * new one.
	 * In most cases, updatePropertyValue() should be called instead of this
	 * function.
	 * @param prop The property to add or update.
	 * @return a reference to *this
	 */
	CIMInstance& setProperty(const CIMProperty& prop);
	/**
	 * Remove a property from this CIMInstance
	 * @param propName The name of the property to remove.
	 * @return a reference to *this
	 */
	CIMInstance& removeProperty(const String& propName);
	/**
	 * Create an CIMInstance with properties and qualifiers from this
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
	 * @return An CIMInstance with properties and qualifiers from this
	 * instance based on the filtering criteria. All properties and qualifiers
	 * that designate keys will be retained.
	 */
	CIMInstance filterProperties(const StringArray& propertyList,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		bool ignorePropertyList=false) const;
	/**
	 * Create a new CIMInstance from this CIMInstance using the specified
	 * criteria
	 * @param localOnly If true, all inherited properties and qualifiers will be
	 *		omitted.
	 * @param includeQualifiers If false, all quailifiers will be omitted.
	 * @param includeClassOrigin If true, all element names will include the
	 *		class origin.
	 * @param propertyList An array of property names that specifies the only
	 *		properties to be included in the returned instance.
	 * @param noProps If true, all properties will be omitted from the returned
	 *		CIMInstance object regardless of the property list parameter.
	 * @return An CIMInstance object based on this one, filtered according
	 *		to the specified criteria.
	 */
	CIMInstance clone(WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray& propertyList=StringArray(),
		bool noProps=false) const;
	/**
	 * Create a new CIMInstance from this CIMInstance using the specified
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
	 * @return An CIMInstance object based on this one, filtered according
	 *		to the specified criteria.
	 */
	CIMInstance clone(WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList) const;
	/**
	 * Create a new CIMInstance from this CIMInstance using the specified
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
	 * @return An CIMInstance object based on this one, filtered according
	 *		to the specified criteria.
	 */
	CIMInstance clone(WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EDeepFlag deep, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		const CIMClass& requestedClass, const CIMClass& cimClass) const;
	/**
	 * Synchronize this instance with the given class. This will ensure that
	 * all properties found on the class exist on this instance. It will also
	 * optionally move all qualifiers from the class to the instance.
	 * @param cc	The class to synchronize this instance with.
	 * @param includeQualifiers If E_EXCLUDE_QUALIFIERS, the instance will not contain any
	 *		qualifiers (except key designators) after the operation. If true, all
	 *		relevant qualifiers are copied from the class to the instance.
	 * @return a reference to *this
	 */
	CIMInstance& syncWithClass(const CIMClass& cc, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS);
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
	CIMInstance createModifiedInstance(
		const CIMInstance& previousInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass) const;
	/**
	 * @return The name of the class for this instance.
	 */
	virtual String getName() const;
	/**
	 * Sets the class name for this instance.
	 * @param name	The new class name for this instance.
	 * @return a reference to *this
	 */
	virtual void setName(const String& name);
	/**
	 * Read this CIMInstance from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);
	/**
	 * Write this CIMInstance to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;
	/**
	 * @return The MOF representation of this CIMInstance as an String.
	 */
	virtual String toMOF() const;
	/**
	 * @return The string representation of the CIMInstance object.
	 */
	virtual String toString() const;
	/**
	 * This compares the properties of the instances, and returns true
	 * if they are equal.  It will sort the properties before comparing,
	 * so the order of the properties is irrelevant.
	 * @param other The instance to compare
	 * @return true if the properties are the same
	 */
	bool propertiesAreEqualTo(const CIMInstance& other) const;

	typedef COWIntrusiveReference<INSTData> CIMInstance::*safe_bool;
	operator safe_bool () const
		{  return m_pdata ? &CIMInstance::m_pdata : 0; }
	bool operator!() const
		{  return !m_pdata; }
	
protected:
	void _buildKeys();
	COWIntrusiveReference<INSTData> m_pdata;
	
	friend bool operator<(const CIMInstance& x, const CIMInstance& y);
};

} // end namespace OpenWBEM

typedef OpenWBEM::CIMInstance OW_CIMInstance OW_DEPRECATED; // in 3.0.0

#endif
