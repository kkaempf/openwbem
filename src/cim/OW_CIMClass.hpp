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

#ifndef OW_CIMCLASS_HPP_INCLUDE_GUARD_
#define OW_CIMCLASS_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_COWIntrusiveReference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_CIMNULL.hpp"
#include "OW_WBEMFlags.hpp"

namespace OpenWBEM
{

/**
 * The CIMClass encapsulates all information that pertains to a CIM class
 * (i.e. properties, methods, qualifiers, etc...). CIMClass objects are
 * ref counted/copy on write objects. It is not considered good practice to
 * hold onto the pointer of an CIMClass object since the ref counting
 * mechanism is driven by the construction and destruction process.
 */
class OW_COMMON_API CIMClass : public CIMElement
{
public:
	struct CLSData;
	/** Name of the internal namespace class */
	static const char* const NAMESPACECLASS;

	/**
	 * Binary serialization version.
	 * Version 1 had a language.
	 * Since a class doesn't actually have a language it was removed for version 2
	 */
	enum { SERIALIZATION_VERSION = 2 };
	/**
	 * Create a new CIMClass object.
	 */
	CIMClass();
	/**
	 * Create a new NULL CIMClass object.  Do not call any methods on it or
	 * your app may crash.
	 */
	explicit CIMClass(CIMNULL_t);
	/**
	 * Create an CIMClass object with the given name.
	 * @param name The name for this CIMClass specified as an String.
	 */
	explicit CIMClass(const String& name);
	/**
	 * Create an CIMClass object with the given name.
	 * @param name The name for this CIMClass specified as a NULL terminated
	 *		string.
	 */
	explicit CIMClass(const char* name);
	CIMClass(const CIMClass& x);
	
	/**
	 * Destroy this CIMClass object and decrement the refcount on the
	 * underlying data.
	 */
	~CIMClass();
	CIMClass& operator=(const CIMClass& x);
	/**
	 * Set this to a null object. All subsequent oprations on this object will
	 * fail.
	 */
	virtual void setNull();
	/**
	 * @return The name of the parent CIM class of this CIMClass object.
	 */
	String getSuperClass() const;
	/**
	 * @return The name of the CIM class that declared the keys for this class.
	 */
	String getKeyClass() const;
	/**
	 * Set the name of the parent CIM class for this CIMClass object.
	 * @param pname The name of the parent CIM class for this object.
	 * @return a reference to *this
	 */
	CIMClass& setSuperClass(const String& pname);
	/**
	 * @return true if this CIMClass object has key properties.
	 */
	bool isKeyed() const;
	/**
	 * Set the flag that indicates this CIMClass is an association.
	 * @param isKeyedParm If true this class will be flagged as an association.
	 * @return a reference to *this
	 */
	CIMClass& setIsAssociation(bool isAssocFlag);
	/**
	 * Set the flag that indicates this CIMClass has keys.
	 * @param isKeyedParm If true this class is flagged as having keys.
	 * @return a reference to *this
	 */
	CIMClass& setIsKeyed(bool isKeyedParm=true);
	/**
	 * @return The properties that are flagged as keys for this CIMClass as
	 * as array of CIMProperty objects.
	 */
	CIMPropertyArray getKeys() const;
	/**
	 * Get a named qualifier from this class.
	 * @param name	The name of the qualifer to retreive
	 * @return The CIMQualifier associated with the given name if found.
	 * Otherwise a NULL CIMQualifier.
	 */
	CIMQualifier getQualifier(const String& name) const;
	/**
	 * Get a named property from this class.
	 * @param name	The name of the property to retrieve. If the name is in the
	 * form "originclass.propertyname" the origin class of the property will be
	 * considered during the search. Otherwise the first property found with the
	 * given name will be returned.
	 * @return CIMProperty associated with the name if found. Otherwise a
	 * NULL CIMProperty object.
	 */
	CIMProperty getProperty(const String& name) const;
	/**
	 * Get a named property from this CIMClass
	 * @param name The name of the property to to retreive.
	 * @param originClass The name of the origin class the property comes from.
	 * @return The CIMProperty associated with the given name and origin, if
	 * found. Otherwise a NULL CIMProperty object.
	 */
	CIMProperty getProperty(const String& name,
		const String& originClass) const;
	/**
	 * Get the method associated with the given name.
	 * @param name The name of the method to retrieve. If the name is in the form
	 * "originclass.methodname" the origin class of the method will be considered
	 * during the search. Otherwise the first method with the given name will be
	 * returned.
	 *
	 * @return The CIMMethod associated with the name, if found. Otherwise a
	 * NULL CIMMethod object.
	 */
	CIMMethod getMethod(const String& name) const;
	/**
	 * Get the method associated with the given name from this CIMClass
	 * considering the given origin class in the process.
	 * @param name	The name of the method to retrieve
	 * @param originClass The name of the CIM class where the method originated.
	 *
	 * @return The CIMMethod associated with the given name on success.
	 * 	Otherwise a NULL CIMMethod object.
	 */
	CIMMethod getMethod(const String& name,
		const String& originClass) const;
	/**
	 * @return true if this CIMClass is an association.
	 */
	bool isAssociation() const;
	/**
	 * @return The qualifiers associated with the CIMClass object.
	 */
	CIMQualifierArray getQualifiers() const;
	/**
	 * @return All properties for this CIMClass, including overides.
	 */
	CIMPropertyArray getAllProperties() const;
	/**
	 * @return The properties for this CIMClass, excluding the overides.
	 */
	CIMPropertyArray getProperties() const;
	/**
	 * @return All the methods for this class, including overides.
	 */
	CIMMethodArray getAllMethods() const;
	/**
	 * @return The methods for this class, excluding the overides.
	 */
	CIMMethodArray getMethods() const;
	/**
	 * Add a property to this class.
	 * @param prop The property to add to this class.
	 * @return a reference to *this
	 */
	CIMClass& addProperty(const CIMProperty& prop);
	/**
	 * @return The number of properties for this class.
	 */
	int numberOfProperties() const;
	/**
	 * Set the properties for this class. All of the old properties are removed.
	 * @param props An CIMPropertyArray containing the new properties for
	 * 	this CIMClass object.
	 * @return a reference to *this
	 */
	CIMClass& setProperties(const CIMPropertyArray& props);
	/**
	 * Update a property on this CIMClass if it exists. If it doesn't exist,
	 * this add it.
	 * @param prop The property to update/add on this class.
	 * @return a reference to *this
	 */
	CIMClass& setProperty(const CIMProperty& prop);
	/**
	 * Add a qualifier to this class.
	 * @param qual The CIMQualifier to add to this class.
	 * @return a reference to *this
	 */
	CIMClass& addQualifier(const CIMQualifier& qual);
	/**
	 * Check for the existence of a qualifier in this class.
	 * @param qual The CIMQualifier to check the existence of.
	 * @return true if this class has the qualifer. Otherwise false.
	 */
	bool hasQualifier(const CIMQualifier& qual) const;
	/**
	 * @return The number of the qualifiers for this class.
	 */
	int numberOfQualifiers() const;
	/**
	 * Remove a qualifier from this class.
	 * @param qual The qualifier to remove from this class.
	 * @return true if the qualifier was removed. Otherwise false.
	 */
	bool removeQualifier(const CIMQualifier& qual);
	/**
	 * Remove a qualifier from this class.
	 * @param name The name of the qualifier to remove from this class.
	 * @return true if the qualifier was removed. Otherwise false.
	 */
	bool removeQualifier(const String& name);
	/**
	 * Remove a property from this class.
	 * @param name The name of the property to remove from this class.
	 * @return true if the property was removed. Otherwise false.
	 */
	bool removeProperty(const String& name);
	/**
	 * Set the qualifiers for this class. The old qualifiers will be removed.
	 * @param quals An CIMQualifierArray that contains the new
	 * 	qualifiers.
	 * @return a reference to *this
	 */
	CIMClass& setQualifiers(const CIMQualifierArray& quals);
	/**
	 * Set the given qualifier on this CIMClass if it is present. If it
	 * is not, then add it.
	 * @param qual The qualifier to set or add to this CIMClass.
	 * @return a reference to *this
	 */
	CIMClass& setQualifier(const CIMQualifier& qual);
	/**
	 * Add a method to this class
	 * @param meth The CIMMethod to add to this class
	 * @return a reference to *this
	 */
	CIMClass& addMethod(const CIMMethod& meth);
	/**
	 * Set the methods for this class. The old methods will be removed.
	 * @param meths An CIMMethodArray containing the new methods for this
	 *		class.
	 * @return a reference to *this
	 */
	CIMClass& setMethods(const CIMMethodArray& meths);
	/**
	 * Update/add a method on this CIMClass object.
	 * @param meth The method to add/update on this class.
	 * @return a reference to *this
	 */
	CIMClass& setMethod(const CIMMethod& meth);
	/**
	 * Create a new instance of this CIM Class. Properties on the instance will
	 * be set to the default values specified in this class.
	 * @return A new CIMInstance of this class appropriately initialized.
	 */
	CIMInstance newInstance() const;
	/**
	 * Create an CIMClass based on this object, using the specified
	 * criteria.
	 * @param localOnly If true, only include local elements.
	 * @param includeQualifiers If true, include the qualifiers for this class.
	 * @param includeClassOrigin If true, include the class origin.
	 * @param propertyList A list that specifies the properties to allow in the
	 *		XML.
	 * @param noProps If true, no properties will be included in the xml
	 * @return A new CIMClass object that is based on this one, with the
	 *		appropriate component filtered according to the parameters.
	 */
	CIMClass clone(
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN,
		const StringArray& propertyList=StringArray(),
		bool noProps=false) const;
	/**
	 * Create a CIMClass that contains properties from this CIMClass that
	 * are found in the given list of property names. Optionally include
	 * qualifiers and the class origin.
	 * @param propertyList An StringArray that contains the property names of
	 *		the properties that should be copied from this CIMClass.
	 * @param includeQualifiers	Include the qualifiers if this is true.
	 * @param includeClassOrigin	Include the class origin if this is true.
	 * @return A new CIMClass constructed per the given parameters.
	 */
	CIMClass filterProperties(const StringArray& propertyList,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin) const;
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
	StringArray getCloneProps(
		WBEMFlags::ELocalOnlyFlag localOnly, 
		WBEMFlags::EDeepFlag deep,
		const StringArray* propertyList,
		const CIMClass& requestedClass) const;
	
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
	StringArray getCloneProps(WBEMFlags::ELocalOnlyFlag localOnly,
		const StringArray* propertyList) const;
	//////////////////////////////////////////////////////////////////////
	// CIMElement implementation
	//////////////////////////////////////////////////////////////////////
	/**
	 * @return The name associated with this CIMClass object as an String
	 * object.
	 */
	virtual String getName() const;
	/**
	 * Set the name associated with this CIMClass object.
	 * @param name	The new name for this CIMClass object.
	 */
	virtual void setName(const String& name);
	/**
	 * Read this CIMClass object from an input stream.
	 * @param istrm The input stream to read this CIMClass from.
	 */
	virtual void readObject(std::istream &istrm);
	/**
	 * Write this CIMClass object to an output stream
	 * @param ostrm The output stream to write this CIMClass to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;
	/**
	 * @return The MOF representation of this CIMClass object as an
	 * String object.
	 */
	virtual String toMOF() const;
	/**
	 * @return The string representation of this CIMClass. This yields the
	 * same results as toMOF.
	 */
	virtual String toString() const;

	typedef COWIntrusiveReference<CLSData> CIMClass::*safe_bool;
	/**
	 * @return true if this CIMClass in not a NULL object.
	 */
	operator safe_bool () const
	{
		return m_pdata ? &CIMClass::m_pdata : 0;
	}
	bool operator!() const
	{
		return !this->m_pdata;
	}
private:
	static String splitName1(const String& inName);
	static String splitName2(const String& inName);
	COWIntrusiveReference<CLSData> m_pdata;
	friend bool operator<(const CIMClass& x, const CIMClass& y);
};

} // end namespace OpenWBEM

#endif
