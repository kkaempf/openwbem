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

#ifndef OWBI1_CIMINSTANCE_HPP_INCLUDE_GUARD_
#define OWBI1_CIMINSTANCE_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_COWIntrusiveReference.hpp"
#include "OWBI1_CIMElement.hpp"
#include "OWBI1_CIMFwd.hpp"
#include "OWBI1_CIMDataType.hpp" // for OWBI1_CIMDataType::INVALID
#include "OWBI1_WBEMFlags.hpp"
#include "OWBI1_CIMName.hpp" // necessary for implicit conversion (const char* -> CIMName) to work
#include "OWBI1_CIMPropertyList.hpp"

namespace OWBI1
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
 *
 * Providers generally follow these steps to create a CIMInstance object:
 *
 * 1. Obtain a CIMOMHandleIFC object hdl.
 *
 * 2. Use hdl to create a CIMClass object of the desired CIM class:
 *
 * CIMClass cc = hdl.getClass(namespace, class_name);
 *
 * 3. Use CIMClass cc to create a CIMInstance of the desired CIM class:
 *
 * CIMInstance inst = cc.newInstance();
 *
 * 4. Assign properties of inst as desired using setProperty().
 *
 */
class OWBI1_OWBI1PROVIFC_API CIMInstance : public CIMElement
{
public:
	enum { SERIALIZATION_VERSION = 1 };				// Version # for serialization
	
	enum EErrorCodes
	{
		E_INSTANCE_HAS_NO_SUCH_PROPERTY
	};

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
	explicit CIMInstance(const CIMName& name);
	/**
	 * Create an CIMInstance.
	 * @param name	The class name of this CIMInstance as a NULL terminated string.
	 */
	explicit CIMInstance(const char* name);

	explicit CIMInstance(const detail::CIMInstanceRepRef& rep);

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
	CIMName getClassName() const;
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
	CIMInstance& setClassName(const CIMName& name);
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
	CIMProperty getProperty(const CIMName& name,
		const CIMName& originClass) const;
	/**
	 * Gets a property with the specified name.
	 * @param name The name of the property to retrieve.
	 * @return The CIMProperty identified by the name on success. Otherwise a
	 * null CIMProperty object.
	 */
	CIMProperty getProperty(const CIMName& name) const;
	/**
	 * Gets a property with the specified name.
	 * @param name The name of the property to retrieve.
	 * @return The CIMProperty identified by the name on success.
	 * @throws an NoSuchPropertyException (E_INSTANCE_HAS_NO_SUCH_PROPERTY) if the property is not found or NULL.
	 */
	CIMProperty getPropertyT(const CIMName& name) const;
	/**
	 * Gets a property's value.
	 * @param name The name of the property value to retrieve.
	 * @return The CIMValue of the specified property.  It will be NULL if the
	 * property doesn't exist or the property's value is NULL.
	 */
	CIMValue getPropertyValue(const CIMName& name) const;
	/**
	 * Test whether a property exists and has a non-NULL value.
	 * @param name The name of the property to test.
	 * @return bool true if the property exists and has a non-NULL value.
	 */
	bool propertyHasValue(const CIMName& name) const;
	/**
	 * @return An CIMPropertyArray that contains all of the keys for this
	 * instance.
	 */
	CIMPropertyArray getKeyValuePairs() const;
	/**
	 * Update the property values for this instance. Does not add properties
	 * if they don't exist.
	 * @param props An CIMPropertyArray that contains the properties to update
	 *		this instance's properties with.
	 * @return a reference to *this
	 */
	CIMInstance& updatePropertyValues(const CIMPropertyArray& props);
	/**
	 * Update a single property value. Does not add prop is it doesn't exist.
	 * @param prop	The CIMProperty to update in this instance. This
	 *		CIMProperty with contain the new value.
	 * @return a reference to *this
	 */
	CIMInstance& updatePropertyValue(const CIMProperty& prop);
	/**
	 * Update a single property value. Does not add prop is it doesn't exist.
	 * @param name	The name of the property to update.
	 * @param cv The new value for the property.
	 * @return a reference to *this
	 */
	CIMInstance& updatePropertyValue(const CIMName& name, const CIMValue& cv);
	/**
	 * Update the value of a property if it exists. Otherwise add a new one.
	 * In most cases, updatePropertyValue() should be called instead of this
	 * function, because it won't add a new property. An instance is
	 * initialized will all it's properties by CIMClass::newInstance().
	 * @param name	The name of the property to add or update.
	 * @param cv The CIMValue that contains the new value for the property.
	 * @return a reference to *this
	 */
	CIMInstance& setProperty(const CIMName& name, const CIMValue& cv);
	/**
	 * Update a property in the property list if it exists. Otherwise add a
	 * new one.
	 * In most cases, updatePropertyValue() should be called instead of this
	 * function, because it won't add a new property. An instance is
	 * initialized will all it's properties by CIMClass::newInstance().
	 * @param prop The property to add or update.
	 * @return a reference to *this
	 */
	CIMInstance& setProperty(const CIMProperty& prop);
	/**
	 * Remove a property from this CIMInstance
	 * @param propName The name of the property to remove.
	 * @return a reference to *this
	 */
	CIMInstance& removeProperty(const CIMName& propName);
	/**
	 * Create a new CIMInstance from this CIMInstance using the specified
	 * criteria
	 * @param propertyList An array of property names that specifies the only
	 *		properties to be included in the returned instance.
	 * @return An CIMInstance object based on this one, filtered according
	 *		to the specified criteria.
	 */
	CIMInstance clone(const CIMPropertyList& propertyList=CIMPropertyList()) const;
	/**
	 * Synchronize this instance with the given class. This will ensure that
	 * all properties found on the class exist on this instance. It will also
	 * optionally move all qualifiers from the class to the instance.
	 * @param cc	The class to synchronize this instance with.
	 * @return a reference to *this
	 */
	CIMInstance& syncWithClass(const CIMClass& cc);
	/**
	 * Create an instance with the set of changes that will occur for a
	 * modifyInstance call.  This instance is the new instance.  The
	 * previous instance is passed in.  The instance that is returned
	 * is what the new instance will be after modifyInstance is applied.
	 * @param previousInstance The instance before being modified.
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
		const CIMPropertyList& propertyList,
		const CIMClass& theClass) const;
	/**
	 * @return The name of the class for this instance.
	 */
	virtual CIMName getName() const;
	/**
	 * Sets the class name for this instance.
	 * @param name	The new class name for this instance.
	 * @return a reference to *this
	 */
	virtual void setName(const CIMName& name);
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

	typedef detail::CIMInstanceRepRef CIMInstance::*safe_bool;
	operator safe_bool () const;
	bool operator!() const;
	
	detail::CIMInstanceRepRef getRep() const;
protected:
	void _buildKeys();

#ifdef OWBI1_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	detail::CIMInstanceRepRef m_rep;
	
#ifdef OWBI1_WIN32
#pragma warning (pop)
#endif

	friend OWBI1_OWBI1PROVIFC_API bool operator<(const CIMInstance& x, const CIMInstance& y);
};

} // end namespace OWBI1

#endif
