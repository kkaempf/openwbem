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

#ifndef OW_CIMOBJECTPATH_HPP_INCLUDE_GUARD_
#define OW_CIMOBJECTPATH_HPP_INCLUDE_GUARD_


#include "OW_config.h"
#include "OW_COWReference.hpp"
#include "OW_CIMBase.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_Bool.hpp"
#include "OW_CIMNULL.hpp"

/**
 * The OW_CIMOMObjectPath class represents the location of CIM classes and
 * instances. OW_CIMObjectPath is a ref counted, copy on write object. It is
 * possible to have a NULL OW_CIMObjectPath.
 */							
class OW_CIMObjectPath : public OW_CIMBase
{
private:
	struct OPData;

public:

	/**
	 * Escapes quotes and '/'
	 * @param inString The string to escape
	 * @return The escaped string
	 */
	static OW_String escape(const OW_String& inString);

	/**
	 * Unescapes quotes and '/'
	 * @param inString The string in an escaped form
	 * @return The string with escape sequences removed
	 */
	static OW_String unEscape(const OW_String& inString);

	/**
	 * Create a new OW_CIMObjectPath object.
	 */
	OW_CIMObjectPath();

	/**
	 * Create a NULL OW_CIMObjectPath object.
	 */
	explicit OW_CIMObjectPath(OW_CIMNULL_t);

	/**
	 * Create an OW_CIMObjectPath to access the specified object
	 * @param className The name of the CIM class this object path is for.
	 */
	explicit OW_CIMObjectPath(const OW_String& className);

	/**
	 * Create an OW_CIMObjectPath to access the specified object
	 * @param className The name of the CIM class this object path is for as a
	 * 	NULL terminated string.
	 */
	explicit OW_CIMObjectPath(const char* className);

	/**
	 * Create an OW_CIMObjectPath to access the specified object (a qualifier
	 * or a class) in a particular namespace.
	 * @param className The name of the CIM class this object refers to.
	 * @param nspace The string representation of the name space
	 *		(e.g. "root/cimv2")
	 */
	OW_CIMObjectPath(const OW_String& className, const OW_String& nspace);

	/**
	 * Create an OW_CIMObjectPath for an instance.
	 * @param className	The name of the class for the instance
	 * @param keys			An OW_CIMPropertyArray that contains the keys for
	 *							the instance.
	 */
	OW_CIMObjectPath(const OW_String& className,
		const OW_CIMPropertyArray& keys);

	/**
	 * Create an OW_CIMObjectPath for an instance and namespace.
	 * @param ns The namespace
	 * @param inst An instance.
	 */
	OW_CIMObjectPath(const OW_String& className,
		const OW_CIMInstance& inst);

	/**
	 * Create an OW_CIMObjectPath for an instance.
	 * @param inst An instance.
	 */
	explicit OW_CIMObjectPath(const OW_CIMInstance& inst);

	/**
	 * Create a new OW_CIMObject path from another.
	 * @param arg The OW_CIMObjectPath this object will be a copy of.
	 */
	OW_CIMObjectPath(const OW_CIMObjectPath& arg);

	/**
	 * Destroy this OW_CIMObjectPath object.
	 */
	~OW_CIMObjectPath();

	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMObjectPath to assign to this one.
	 * @return A reference to this object after the assignment has been made.
	 */
	OW_CIMObjectPath& operator= (const OW_CIMObjectPath& arg);

	/**
	 * Add another key to this object path (for instance paths).
	 * @param keyname The name of the key property
	 * @param value The value for the key property
	 * @return a reference to *this
	 */
	OW_CIMObjectPath& addKey(const OW_String& keyname, const OW_CIMValue& value);

	/**
	 * Get the keys for this object path
	 * @return An OW_CIMPropertyArray containing the keys for this object path.
	 */
	OW_CIMPropertyArray getKeys() const;

	/**
	 * Get a key from this ObjectPath
	 * @param key The Name of the key to get
	 * @return An OW_CIMProperty corresponding to key.  If there is no key
	 *  found, a NULL OW_CIMProperty will be returned.
	 */
	OW_CIMProperty getKey(const OW_String& keyName) const;

	/**
	 * Get a key from this ObjectPath
	 * @param key The Name of the key to get
	 * @return An OW_CIMProperty corresponding to key.  If there is no key
	 *  found, an OW_NoSuchPropertyException exception is thrown.
	 * @throws OW_NoSuchPropertyException if keyName is not a property
	 */
	OW_CIMProperty getKeyT(const OW_String& keyName) const;

	/**
	 * Set the keys of this object path
	 * @param newKeys	An OW_CIMPropertyArray that contains the keys for this
	 * 	object path.
	 * @return a reference to *this
	 */
	OW_CIMObjectPath& setKeys(const OW_CIMPropertyArray& newKeys);


	/**
	 * Set the keys of this object path from the key properties of an instance.
	 * @param instance The CIM instance to get the key properties from.
	 * @return a reference to *this
	 */
	OW_CIMObjectPath& setKeys(const OW_CIMInstance& instance);

	/**
	 * @return The namespace component of the OW_CIMNameSpace for this object path
	 */
	OW_String getNameSpace() const;

	/**
	 * @return The URL component of the OW_CIMNameSpace for this object path
	 */
	OW_CIMUrl getNameSpaceUrl() const;

	/**
	 * @return The host name from the name space for this object path.
	 */
	OW_String getHost() const;

	/**
	 * @return The object name for this object path
	 */
	OW_String getObjectName() const;

	/**
	 * Convert a string representation of an object path to an OW_CIMObjectPath.
	 * @param instanceName	The object path to convert. Assumed to be an instance
	 *		path.
	 * @return An OW_CIMObjectPath object on success.
	 */
	static OW_CIMObjectPath parse(const OW_String& instanceName);

	/**
	 * Set the host name on the name space for this object path.
	 * @param host	The new name of the host to set on the underlying name space.
	 * @return a reference to *this
	 */
	OW_CIMObjectPath& setHost(const OW_String& host);

	/**
	 * Set the namespace for this object path.
	 * @param ns	The string representation of the namespace.
	 * @return a reference to *this
	 */
	OW_CIMObjectPath& setNameSpace(const OW_String& ns);

	/**
	 * Assign an object name to this object path.
	 * @param objectName	The name of the object to assign to this object path.
	 * @return a reference to *this
	 */
	OW_CIMObjectPath& setObjectName(const OW_String& objectName);

	/**
	 * Compare this object path with another.
	 * @param op The object path to compare to this one.
	 * @param ignoreClasOrigins If true class origins are ignored
	 * @return true if the object paths are equal. Otherwise false.
	 *
	 * unclear why classorigins would ever be checked!
	 */
	OW_Bool equals(const OW_CIMObjectPath& op,
		OW_Bool ignoreClassOrigins=true) const;

	/**
	 * @return true if this is not a null object.
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
	 * Equality operator
	 * @param op The object path to compare to this one.
	 * @return true if the object paths are equal. Otherwise false.
	 */
	OW_Bool operator== (const OW_CIMObjectPath& op) const
	{
		return equals(op, true);
	}

	/**
	 * @return The full namespace for this object path
	 */
	OW_CIMNameSpace getFullNameSpace() const;

	/**
	 * @return The string representation of this OW_CIMObjectPath.
	 */
	virtual OW_String toString() const;

	/**
	 * @return The model path component of this OW_CIMObjectPath as an
	 * OW_String
	 */
	virtual OW_String modelPath() const;

	/**
	 * @return The MOF representation of this OW_CIMObjectPath as an
	 * OW_String
	 */
	virtual OW_String toMOF() const;

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

private:

	OW_COWReference<OPData> m_pdata;
};

bool operator<(const OW_CIMObjectPath& lhs, const OW_CIMObjectPath& rhs);

#endif
