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

#ifndef OW_CIMOBJECTPATH_HPP_INCLUDE_GUARD_
#define OW_CIMOBJECTPATH_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_COWIntrusiveReference.hpp"
#include "OW_CIMBase.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMNULL.hpp"
#include "OW_CIMName.hpp" // necessary for implicit conversion (const char* -> CIMName) to work

namespace OpenWBEM
{

/**
 * The CIMOMObjectPath class represents the location of CIM classes and
 * instances. CIMObjectPath is a ref counted, copy on write object. It is
 * possible to have a NULL CIMObjectPath.
 */							
class OW_COMMON_API CIMObjectPath : public CIMBase
{
private:
	struct OPData;
	friend bool operator<(const CIMObjectPath::OPData& x, 
			const CIMObjectPath::OPData& y);
public:
	/**
	 * Escapes quotes and '/'
	 * @param inString The string to escape
	 * @return The escaped string
	 */
	static String escape(const String& inString);
	/**
	 * Unescapes quotes and '/'
	 * @param inString The string in an escaped form
	 * @return The string with escape sequences removed
	 */
	static String unEscape(const String& inString);
	/**
	 * Create a new CIMObjectPath object.
	 */
	CIMObjectPath();
	/**
	 * Create a NULL CIMObjectPath object.
	 */
	explicit CIMObjectPath(CIMNULL_t);
	/**
	 * Create an CIMObjectPath to access the specified object
	 * @param className The name of the CIM class this object path is for.
	 */
	explicit CIMObjectPath(const CIMName& className);
	/**
	 * Create an CIMObjectPath to access the specified object
	 * @param className The name of the CIM class this object path is for as a
	 * 	NULL terminated string.
	 */
	explicit CIMObjectPath(const char* className);
	/**
	 * Create an CIMObjectPath to access the specified object (a qualifier
	 * or a class) in a particular namespace.
	 * @param className The name of the CIM class this object refers to.
	 * @param nspace The string representation of the name space
	 *		(e.g. "root/cimv2")
	 */
	CIMObjectPath(const CIMName& className, const String& nspace);
	/**
	 * Create an CIMObjectPath for an instance.
	 * @param className	The name of the class for the instance
	 * @param keys			An CIMPropertyArray that contains the keys for
	 *							the instance.
	 */
	CIMObjectPath(const CIMName& className,
		const CIMPropertyArray& keys);
	/**
	 * Create an CIMObjectPath for an instance and namespace.
	 * @param ns The namespace
	 * @param inst An instance.
	 */
	CIMObjectPath(const String& ns,
		const CIMInstance& inst);
	/**
	 * Create a new CIMObject path from another.
	 * @param arg The CIMObjectPath this object will be a copy of.
	 */
	CIMObjectPath(const CIMObjectPath& arg);
	/**
	 * Destroy this CIMObjectPath object.
	 */
	~CIMObjectPath();
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMObjectPath to assign to this one.
	 * @return A reference to this object after the assignment has been made.
	 */
	CIMObjectPath& operator= (const CIMObjectPath& arg);
	/**
	 * This function is too error prone.  Use setKeyValue() instead.
	 * It's just still here for backward compatibility. It's deprecated and will
	 * be removed in the future.
	 */
	CIMObjectPath& addKey(const CIMName& keyname, const CIMValue& value) OW_DEPRECATED; // in 3.0.0
	/**
	 * This function is too error prone.  Use setKeyValue() instead.
	 * It's just still here for backward compatibility. It's deprecated and will
	 * be removed in the future.
	 */
	CIMObjectPath& addKey(const CIMProperty& key) OW_DEPRECATED; // in 3.0.0
	/**
	 * Get the keys for this object path
	 * @return An CIMPropertyArray containing the keys for this object path.
	 */
	CIMPropertyArray getKeys() const;
	/**
	 * Get a key from this ObjectPath
	 * @param key The Name of the key to get
	 * @return An CIMProperty corresponding to key.  If there is no key
	 *  found, a NULL CIMProperty will be returned.
	 */
	CIMProperty getKey(const CIMName& keyName) const;
	/**
	 * Get a key from this ObjectPath
	 * @param key The Name of the key to get
	 * @return An CIMProperty corresponding to key.  If there is no key
	 *  found, an NoSuchPropertyException exception is thrown.
	 * @throws NoSuchPropertyException if keyName is not a property
	 */
	CIMProperty getKeyT(const CIMName& keyName) const;
	/**
	 * Gets a key's value.
	 * @param name The name of the key value to retrieve.
	 * @return The CIMValue of the specified key.  It will be NULL if the
	 * key doesn't exist or the key's value is NULL.
	 */
	CIMValue getKeyValue(const CIMName& name) const;
	/**
	 * Test whether a key exists and has a non-NULL value.
	 * @param name The name of the key to test.
	 * @return bool true if the key exists and has a non-NULL value.
	 */
	bool keyHasValue(const CIMName& name) const;
	/**
	 * Set the keys of this object path
	 * @param newKeys	An CIMPropertyArray that contains the keys for this
	 * 	object path.
	 * @return a reference to *this
	 */
	CIMObjectPath& setKeys(const CIMPropertyArray& newKeys);
	/**
	 * Set the keys of this object path from the key properties of an instance.
	 * @param instance The CIM instance to get the key properties from.
	 * @return a reference to *this
	 */
	CIMObjectPath& setKeys(const CIMInstance& instance);
	/**
	 * Set a key value. This will update an existing key, or add a new key if
	 * one if a key with name doesn't already exist.
	 * @param name The name of the key
	 * @param value The value of the key
	 * @return a reference to *this
	 */
	CIMObjectPath& setKeyValue(const CIMName& name, const CIMValue& value);
	/**
	 * @return The namespace component of the CIMNameSpace for this object path
	 */
	String getNameSpace() const;
	/**
	 * @return The URL component of the CIMNameSpace for this object path
	 */
	CIMUrl getNameSpaceUrl() const;
	/**
	 * @return The host name from the name space for this object path.
	 */
	String getHost() const;
	/**
	 * This function is deprecated in favor of getClassName()
	 * @return The class name for this object path
	 */
	String getObjectName() const OW_DEPRECATED; // in 3.0.0
	/**
	 * @return The class name for this object path
	 */
	String getClassName() const;
	/**
	 * Convert a string representation of an object path to an CIMObjectPath.
	 * @param instanceName	The object path to convert. Assumed to be an instance
	 *		path.
	 * @return An CIMObjectPath object on success.
	 */
	static CIMObjectPath parse(const String& instanceName);
	/**
	 * Set the host name on the name space for this object path.
	 * @param host	The new name of the host to set on the underlying name space.
	 * @return a reference to *this
	 */
	CIMObjectPath& setHost(const String& host);
	/**
	 * Set the namespace for this object path.
	 * @param ns	The string representation of the namespace.
	 * @return a reference to *this
	 */
	CIMObjectPath& setNameSpace(const String& ns);
	/**
	 * Assign a class name to this object path.
	 * This function is deprecated in favor of setClassName()
	 * @param className	The name of the class to assign to this object path.
	 * @return a reference to *this
	 */
	CIMObjectPath& setObjectName(const CIMName& className) OW_DEPRECATED; // in 3.0.0
	/**
	 * Assign a class name to this object path.
	 * @param className	The name of the class to assign to this object path.
	 * @return a reference to *this
	 */
	CIMObjectPath& setClassName(const CIMName& className);
	/**
	 * Compare this object path with another.
	 * @param op The object path to compare to this one.
	 * @return true if the object paths are equal. Otherwise false.
	 */
	bool equals(const CIMObjectPath& op) const;

	typedef COWIntrusiveReference<OPData> CIMObjectPath::*safe_bool;
	/**
	 * @return true if this is not a null object.
	 */
	operator safe_bool () const
		{  return m_pdata ? &CIMObjectPath::m_pdata : 0; }
	bool operator!() const
		{  return !m_pdata; }
	/**
	 * Equality operator
	 * @param op The object path to compare to this one.
	 * @return true if the object paths are equal. Otherwise false.
	 */
	bool operator== (const CIMObjectPath& op) const
	{
		return equals(op);
	}

	/**
	 * Not Equal operator
	 * @param op The object path to compare to this one.
	 * @return true if the object paths are not equal. Otherwise false.
	 */
	bool operator!= (const CIMObjectPath& op) const
	{
		return !equals(op);
	}
	/**
	 * @return The full namespace for this object path
	 */
	CIMNameSpace getFullNameSpace() const;
	/**
	 * @return true if this object is a class path
	 */
	bool isClassPath() const;
	/**
	 * @return true if this object is an instance path
	 */
	bool isInstancePath() const;
	/**
	 * @return The string representation of this CIMObjectPath.
	 */
	virtual String toString() const;
	/**
	 * @return The model path component of this CIMObjectPath as an
	 * String
	 */
	virtual String modelPath() const;
	/**
	 * @return The MOF representation of this CIMObjectPath as an
	 * String
	 */
	virtual String toMOF() const;
	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream& istrm);
	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream& ostrm) const;

	/**
	 * Synchronize this object path with the given class. This will ensure that
	 * all keys found on the class exist.  All CIMValue types will be cast to
	 * the proper type, this is helpful because CIM-XML does not preserve the
	 * detailed type information of key-value pairs.
	 * @param cc	The class to synchronize with.
	 * @return a reference to *this
	 */
	CIMObjectPath& syncWithClass(const CIMClass& theClass);

private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	COWIntrusiveReference<OPData> m_pdata;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	friend OW_COMMON_API bool operator<(const CIMObjectPath& lhs, const CIMObjectPath& rhs);
};

} // end namespace OpenWBEM

#endif
