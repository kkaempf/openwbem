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
#ifndef OW_CIMPARAMETER_HPP_INCLUDE_GUARD_
#define OW_CIMPARAMETER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_COWReference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMNULL.hpp"

namespace OpenWBEM
{

/**
 * The CIMParameter class encapsulates all data and functionality pertinent
 * to a parameter to a method within a CIM class.
 */
class CIMParameter : public CIMElement
{
public:
	struct PARMData;
	/**
	 * Create a new CIMParameter object.
	 */
	CIMParameter();
	/**
	 * Create a NULL CIMParameter object.
	 */
	explicit CIMParameter(CIMNULL_t);
	/**
	 * Create an CIMParameter object with a given name.
	 * @param name	The name for this parameter.
	 */
	explicit CIMParameter(const String& name);
	/**
	 * Create an CIMParameter object with a given name.
	 * @param name	The name for this parameter as a NULL terminated string.
	 */
	explicit CIMParameter(const char* name);
	/**
	 * Copy constructor
	 * @param arg The CIMParameter object to make this one a copy of.
	 */
	CIMParameter(const CIMParameter& arg);
	/**
	 * Destroy this CIMParameter object.
	 */
	~CIMParameter();
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMParameter object to assign to this one.
	 * @return A reference to this object after the assignment has been made.
	 */
	CIMParameter& operator= (const CIMParameter& arg);
	/**
	 * @return true if this a valid CIMParameter object.
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
	 * @param quals An CIMQualifierArray that contains the qualifiers for
	 * 	this parameter.
	 * @return a reference to *this
	 */
	CIMParameter& setQualifiers(const CIMQualifierArray& quals);
	/**
	 * Get the qualifiers for this parameter.
	 * @return An CIMQualifierArray that contains the qualifiers for this
	 * parameter.
	 */
	CIMQualifierArray getQualifiers() const;
	/**
	 * Set the data type for this parameter.
	 * @param type	The CIMDataType for this parameter.
	 * @return a reference to *this
	 */
	CIMParameter& setDataType(const CIMDataType& type);
	/**
	 * Get the data type for this parameter.
	 * @return An CIMDataType for this parameter.
	 */
	CIMDataType getType() const;
	/**
	 * @return The size of the data for this parameter.
	 */
	Int32 getDataSize() const;
	/**
	 * Get a qualifier by name for this parameter.
	 * @param name	The name of the qualifier to retrieve.
	 * @return The CIMQualifier associated with the given name if there is
	 * one. Otherwise a NULL CIMQualifier.
	 */
	CIMQualifier getQualifier(const String& name) const;
	/**
	 * @return The name of this CIMParameter.
	 */
	virtual String getName() const;
	/**
	 * Set the name of this CIMParameter.
	 * @param name	The new name for this CIMParameter.
	 */
	virtual void setName(const String& name);
	/**
	 * Write this object to the given output stream.
	 * @param ostrm The output stream to write this CIMElement to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;
	/**
	 * Read this object from the given input stream.
	 * @param istrm The input stream to read this CIMElement from.
	 */
	virtual void readObject(std::istream &istrm);
	/**
	 * @return a string representation of this CIMParameter.
	 */
	virtual String toString() const;
	/**
	 * @return An String that contains the MOF representation of this
	 * CIMParameter object.
	 */
	virtual String toMOF() const;
	
	/**
	 * Test if this property has a qualifier that is a boolean type with a
	 * value of true.
	 * @param name	The name of the qualifier to test.
	 * @return true if the qualifier exists and has a value of true.
	 */
	bool hasTrueQualifier(const String& name) const;
private:
	COWReference<PARMData> m_pdata;
	friend bool operator<(const CIMParameter& x, const CIMParameter& y);
};

} // end namespace OpenWBEM

typedef OpenWBEM::CIMParameter OW_CIMParameter OW_DEPRECATED;

#endif
