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

#ifndef OW_CIMCLASS_HPP_INCLUDE_GUARD_
#define OW_CIMCLASS_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_COWReference.hpp"
#include "OW_Bool.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_CIMNULL.hpp"

/**
 * The OW_CIMClass encapsulates all information that pertains to a CIM class
 * (i.e. properties, methods, qualifiers, etc...). OW_CIMClass objects are
 * ref counted/copy on write objects. It is not considered good practice to
 * hold onto the pointer of an OW_CIMClass object since the ref counting
 * mechanism is driven by the construction and destruction process.
 */
class OW_CIMClass : public OW_CIMElement
{
public:
	struct CLSData;

	/** Name of the internal namespace class */
	static const char* const NAMESPACECLASS;

	/**
	 * Create a new OW_CIMClass object.
	 */
	OW_CIMClass();

	/**
	 * Create a new NULL OW_CIMClass object.  Do not call any methods on it or
	 * your app may crash.
	 */
	explicit OW_CIMClass(OW_CIMNULL_t);

	/**
	 * Create an OW_CIMClass object with the given name.
	 * @param name The name for this OW_CIMClass specified as an OW_String.
	 */
	explicit OW_CIMClass(const OW_String& name);

	/**
	 * Create an OW_CIMClass object with the given name.
	 * @param name The name for this OW_CIMClass specified as a NULL terminated
	 *		string.
	 */
	explicit OW_CIMClass(const char* name);

	OW_CIMClass(const OW_CIMClass& x);
	
	/**
	 * Destroy this OW_CIMClass object and decrement the refcount on the
	 * underlying data.
	 */
	~OW_CIMClass();

	OW_CIMClass& operator=(const OW_CIMClass& x);

	/**
	 * Set this to a null object. All subsequent oprations on this object will
	 * fail.
	 */
	virtual void setNull();

	/**
	 * @return The name of the parent CIM class of this OW_CIMClass object.
	 */
	OW_String getSuperClass() const;

	/**
	 * @return The name of the CIM class that declared the keys for this class.
	 */
	OW_String getKeyClass() const;

	/**
	 * Set the name of the parent CIM class for this OW_CIMClass object.
	 * @param pname The name of the parent CIM class for this object.
	 * @return a reference to *this
	 */
	OW_CIMClass& setSuperClass(const OW_String& pname);

	/**
	 * @return true if this OW_CIMClass object has key properties.
	 */
	OW_Bool isKeyed() const;

	/**
	 * Set the flag that indicates this OW_CIMClass is an association.
	 * @param isKeyedParm If true this class will be flagged as an association.
	 * @return a reference to *this
	 */
	OW_CIMClass& setIsAssociation(OW_Bool isAssocFlag);

	/**
	 * Set the flag that indicates this OW_CIMClass has keys.
	 * @param isKeyedParm If true this class is flagged as having keys.
	 * @return a reference to *this
	 */
	OW_CIMClass& setIsKeyed(OW_Bool isKeyedParm=true);

	/**
	 * @return The properties that are flagged as keys for this OW_CIMClass as
	 * as array of OW_CIMProperty objects.
	 */
	OW_CIMPropertyArray getKeys() const;

	/**
	 * Get a named qualifier from this class.
	 * @param name	The name of the qualifer to retreive
	 * @return The OW_CIMQualifier associated with the given name if found.
	 * Otherwise a NULL OW_CIMQualifier.
	 */
	OW_CIMQualifier getQualifier(const OW_String& name) const;

	/**
	 * Get a named property from this class.
	 * @param name	The name of the property to retrieve. If the name is in the
	 * form "originclass.propertyname" the origin class of the property will be
	 * considered during the search. Otherwise the first property found with the
	 * given name will be returned.
	 * @return OW_CIMProperty associated with the name if found. Otherwise a
	 * NULL OW_CIMProperty object.
	 */
	OW_CIMProperty getProperty(const OW_String& name) const;

	/**
	 * Get a named property from this OW_CIMClass
	 * @param name The name of the property to to retreive.
	 * @param originClass The name of the origin class the property comes from.
	 * @return The OW_CIMProperty associated with the given name and origin, if
	 * found. Otherwise a NULL OW_CIMProperty object.
	 */
	OW_CIMProperty getProperty(const OW_String& name,
		const OW_String& originClass) const;

	/**
	 * Get the method associated with the given name.
	 * @param name The name of the method to retrieve. If the name is in the form
	 * "originclass.methodname" the origin class of the method will be considered
	 * during the search. Otherwise the first method with the given name will be
	 * returned.
	 *
	 * @return The OW_CIMMethod associated with the name, if found. Otherwise a
	 * NULL OW_CIMMethod object.
	 */
	OW_CIMMethod getMethod(const OW_String& name) const;

	/**
	 * Get the method associated with the given name from this OW_CIMClass
	 * considering the given origin class in the process.
	 * @param name	The name of the method to retrieve
	 * @param originClass The name of the CIM class where the method originated.
	 *
	 * @return The OW_CIMMethod associated with the given name on success.
	 * 	Otherwise a NULL OW_CIMMethod object.
	 */
	OW_CIMMethod getMethod(const OW_String& name,
		const OW_String& originClass) const;

	/**
	 * @return true if this OW_CIMClass is an association.
	 */
	OW_Bool isAssociation() const;

	/**
	 * @return The qualifiers associated with the OW_CIMClass object.
	 */
	OW_CIMQualifierArray getQualifiers() const;

	/**
	 * @return All properties for this OW_CIMClass, including overides.
	 */
	OW_CIMPropertyArray getAllProperties() const;

	/**
	 * @return The properties for this OW_CIMClass, excluding the overides.
	 */
	OW_CIMPropertyArray getProperties() const;

	/**
	 * @return All the methods for this class, including overides.
	 */
	OW_CIMMethodArray getAllMethods() const;

	/**
	 * @return The methods for this class, excluding the overides.
	 */
	OW_CIMMethodArray getMethods() const;

	/**
	 * Add a property to this class.
	 * @param prop The property to add to this class.
	 * @return a reference to *this
	 */
	OW_CIMClass& addProperty(const OW_CIMProperty& prop);

	/**
	 * @return The number of properties for this class.
	 */
	int numberOfProperties() const;

	/**
	 * Set the properties for this class. All of the old properties are removed.
	 * @param props An OW_CIMPropertyArray containing the new properties for
	 * 	this OW_CIMClass object.
	 * @return a reference to *this
	 */
	OW_CIMClass& setProperties(const OW_CIMPropertyArray& props);

	/**
	 * Update a property on this OW_CIMClass if it exists. If it doesn't exist,
	 * this add it.
	 * @param prop The property to update/add on this class.
	 * @return a reference to *this
	 */
	OW_CIMClass& setProperty(const OW_CIMProperty& prop);

	/**
	 * Add a qualifier to this class.
	 * @param qual The OW_CIMQualifier to add to this class.
	 * @return a reference to *this
	 */
	OW_CIMClass& addQualifier(const OW_CIMQualifier& qual);

	/**
	 * Check for the existence of a qualifier in this class.
	 * @param qual The OW_CIMQualifier to check the existence of.
	 * @return true if this class has the qualifer. Otherwise false.
	 */
	OW_Bool hasQualifier(const OW_CIMQualifier& qual) const;

	/**
	 * @return The number of the qualifiers for this class.
	 */
	int numberOfQualifiers() const;

	/**
	 * Remove a qualifier from this class.
	 * @param qual The qualifier to remove from this class.
	 * @return true if the qualifier was removed. Otherwise false.
	 */
	OW_Bool removeQualifier(const OW_CIMQualifier& qual);

	/**
	 * Remove a qualifier from this class.
	 * @param name The name of the qualifier to remove from this class.
	 * @return true if the qualifier was removed. Otherwise false.
	 */
	OW_Bool removeQualifier(const OW_String& name);

	/**
	 * Remove a property from this class.
	 * @param name The name of the property to remove from this class.
	 * @return true if the property was removed. Otherwise false.
	 */
	OW_Bool removeProperty(const OW_String& name);

	/**
	 * Set the qualifiers for this class. The old qualifiers will be removed.
	 * @param quals An OW_CIMQualifierArray that contains the new
	 * 	qualifiers.
	 * @return a reference to *this
	 */
	OW_CIMClass& setQualifiers(const OW_CIMQualifierArray& quals);

	/**
	 * Set the given qualifier on this OW_CIMClass if it is present. If it
	 * is not, then add it.
	 * @param qual The qualifier to set or add to this OW_CIMClass.
	 * @return a reference to *this
	 */
	OW_CIMClass& setQualifier(const OW_CIMQualifier& qual);

	/**
	 * Add a method to this class
	 * @param meth The OW_CIMMethod to add to this class
	 * @return a reference to *this
	 */
	OW_CIMClass& addMethod(const OW_CIMMethod& meth);

	/**
	 * Set the methods for this class. The old methods will be removed.
	 * @param meths An OW_CIMMethodArray containing the new methods for this
	 *		class.
	 * @return a reference to *this
	 */
	OW_CIMClass& setMethods(const OW_CIMMethodArray& meths);

	/**
	 * Update/add a method on this OW_CIMClass object.
	 * @param meth The method to add/update on this class.
	 * @return a reference to *this
	 */
	OW_CIMClass& setMethod(const OW_CIMMethod& meth);

	/**
	 * Create a new instance of this CIM Class. Properties on the instance will
	 * be set to the default values specified in this class.
	 * @return A new OW_CIMInstance of this class appropriately initialized.
	 */
	OW_CIMInstance newInstance() const;

	/**
	 * Create an OW_CIMClass based on this object, using the specified
	 * criteria.
	 * @param localOnly If true, only include local elements.
	 * @param includeQualifiers If true, include the qualifiers for this class.
	 * @param includeClassOrigin If true, include the class origin.
	 * @param propertyList A list that specifies the properties to allow in the
	 *		XML.
	 * @param noProps If true, no properties will be included in the xml
	 * @return A new OW_CIMClass object that is based on this one, with the
	 *		appropriate component filtered according to the parameters.
	 */
	OW_CIMClass clone(OW_Bool localOnly=false, OW_Bool includeQualifiers=true,
		OW_Bool includeClassOrigin=true,
		const OW_StringArray& propertyList=OW_StringArray(),
		OW_Bool noProps=false) const;

	/**
	 * Create a OW_CIMClass that contains properties from this OW_CIMClass that
	 * are found in the given list of property names. Optionally include
	 * qualifiers and the class origin.
	 * @param propertyList An OW_StringArray that contains the property names of
	 *		the properties that should be copied from this OW_CIMClass.
	 * @param includeQualifiers	Include the qualifiers if this is true.
	 * @param includeClassOrigin	Include the class origin if this is true.
	 * @return A new OW_CIMClass constructed per the given parameters.
	 */
	OW_CIMClass filterProperties(const OW_StringArray& propertyList,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin) const;

	/**
	 * Return a list of all the properties that will be required to clone an
	 * instance.  This is also useful for indication providers to get a list
	 * of the properties that the client requested, so it can avoid doing
	 * expensive operations needlessly.
	 * @param localOnly If true, don't include superclass properties.
	 * @param deep If true, include subclass properties.
	 * @param propertyList A list that specifies the properties list of 
	 *		properties that can be returned.
	 * @param requestedClass The class that was requested in enumInstances.
	 * @return A list of properties that are requested based on the parameters.
	 */
	OW_StringArray getCloneProps(OW_Bool localOnly, OW_Bool deep,
		const OW_StringArray* propertyList,
		const OW_CIMClass& requestedClass) const;
	
	/**
	 * Return a list of all the properties that will be required to clone an
	 * instance.  This is also useful for indication providers to get a list
	 * of the properties that the client requested, so it can avoid doing
	 * expensive operations needlessly.
	 * @param localOnly If true, don't include superclass properties.
	 * @param propertyList A list that specifies the properties list of 
	 *		properties that can be returned.
	 * @return A list of properties that are requested based on the parameters.
	 */
	OW_StringArray getCloneProps(OW_Bool localOnly,
		const OW_StringArray* propertyList) const;

	//////////////////////////////////////////////////////////////////////
	// OW_CIMElement implementation
	//////////////////////////////////////////////////////////////////////

	/**
	 * @return The name associated with this OW_CIMClass object as an OW_String
	 * object.
	 */
	virtual OW_String getName() const;

	/**
	 * Set the name associated with this OW_CIMClass object.
	 * @param name	The new name for this OW_CIMClass object.
	 */
	virtual void setName(const OW_String& name);

	/**
	 * Read this OW_CIMClass object from an input stream.
	 * @param istrm The input stream to read this OW_CIMClass from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * Write this OW_CIMClass object to an output stream
	 * @param ostrm The output stream to write this OW_CIMClass to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * @return The MOF representation of this OW_CIMClass object as an
	 * OW_String object.
	 */
	virtual OW_String toMOF() const;

	/**
	 * @return The string representation of this OW_CIMClass. This yields the
	 * same results as toMOF.
	 */
	virtual OW_String toString() const;

	/**
	 * @return true if this OW_CIMClass in not a NULL object.
	 */
private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
	{
		return (this->m_pdata.isNull()) ? 0 : &dummy::nonnull;
	}

	safe_bool operator!() const
	{
		return (this->m_pdata.isNull()) ? &dummy::nonnull : 0;
	}

private:

	static OW_String splitName1(const OW_String& inName);
	static OW_String splitName2(const OW_String& inName);

	OW_COWReference<CLSData> m_pdata;

	friend bool operator<(const OW_CIMClass& x, const OW_CIMClass& y);
};

#endif
