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

#ifndef OW_CIMPARAMETER_HPP_INCLUDE_GUARD_
#define OW_CIMPARAMETER_HPP_INCLUDE_GUARD_


#include "OW_config.h"

#include "OW_COWReference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMNULL.hpp"

/**
 * The OW_CIMParameter class encapsulates all data and functionality pertinent
 * to a parameter to a method within a CIM class.
 */
class OW_CIMParameter : public OW_CIMElement
{
public:

	struct PARMData;

	/**
	 * Create a new OW_CIMParameter object.
	 */
	OW_CIMParameter();

	/**
	 * Create a NULL OW_CIMParameter object.
	 */
	explicit OW_CIMParameter(OW_CIMNULL_t);

	/**
	 * Create an OW_CIMParameter object with a given name.
	 * @param name	The name for this parameter.
	 */
	explicit OW_CIMParameter(const OW_String& name);

	/**
	 * Create an OW_CIMParameter object with a given name.
	 * @param name	The name for this parameter as a NULL terminated string.
	 */
	explicit OW_CIMParameter(const char* name);

	/**
	 * Copy constructor
	 * @param arg The OW_CIMParameter object to make this one a copy of.
	 */
	OW_CIMParameter(const OW_CIMParameter& arg);

	/**
	 * Destroy this OW_CIMParameter object.
	 */
	~OW_CIMParameter();

	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMParameter object to assign to this one.
	 * @return A reference to this object after the assignment has been made.
	 */
	OW_CIMParameter& operator= (const OW_CIMParameter& arg);

	/**
	 * @return true if this a valid OW_CIMParameter object.
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
	 * Set the qualifiers for this parameter
	 * @param quals An OW_CIMQualifierArray that contains the qualifiers for
	 * 	this parameter.
	 */
	void setQualifiers(const OW_CIMQualifierArray& quals);

	/**
	 * Get the qualifiers for this parameter.
	 * @return An OW_CIMQualifierArray that contains the qualifiers for this
	 * parameter.
	 */
	OW_CIMQualifierArray getQualifiers() const;

	/**
	 * Set the data type for this parameter.
	 * @param type	The OW_CIMDataType for this parameter.
	 */
	void setDataType(const OW_CIMDataType& type);

	/**
	 * Get the data type for this parameter.
	 * @return An OW_CIMDataType for this parameter.
	 */
	OW_CIMDataType getType() const;

	/**
	 * @return The size of the data for this parameter.
	 */
	OW_Int32 getDataSize() const;

	/**
	 * Get a qualifier by name for this parameter.
	 * @param name	The name of the qualifier to retrieve.
	 * @return The OW_CIMQualifier associated with the given name if there is
	 * one. Otherwise a NULL OW_CIMQualifier.
	 */
	OW_CIMQualifier getQualifier(const OW_String& name) const;

	/**
	 * @return The name of this OW_CIMParameter.
	 */
	virtual OW_String getName() const;

	/**
	 * Set the name of this OW_CIMParameter.
	 * @param name	The new name for this OW_CIMParameter.
	 */
	virtual void setName(const OW_String& name);

	/**
	 * Write this object to the given output stream.
	 * @param ostrm The output stream to write this OW_CIMElement to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * Read this object from the given input stream.
	 * @param istrm The input stream to read this OW_CIMElement from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * @return a string representation of this OW_CIMParameter.
	 */
	virtual OW_String toString() const;

	/**
	 * @return An OW_String that contains the MOF representation of this
	 * OW_CIMParameter object.
	 */
	virtual OW_String toMOF() const;
	
	/**
	 * Test if this property has a qualifier that is a boolean type with a
	 * value of true.
	 * @param name	The name of the qualifier to test.
	 * @return true if the qualifier exists and has a value of true.
	 */
	OW_Bool hasTrueQualifier(const OW_String& name) const;

private:

	OW_COWReference<PARMData> m_pdata;

	friend bool operator<(const OW_CIMParameter& x, const OW_CIMParameter& y);
};

#endif


