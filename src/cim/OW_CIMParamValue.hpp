/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*	  this list of conditions and the following disclaimer in the documentation
*	  and/or other materials provided with the distribution.
*
*  - Neither the name of Quest Software, Inc. nor the names of its
*	  contributors may be used to endorse or promote products derived from this
* 	  software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#ifndef OW_CIMPARAMVALUE_HPP_INCLUDE_GUARD_
#define OW_CIMPARAMVALUE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/COWIntrusiveReference.hpp"
#include "OW_CIMBase.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMNULL.hpp"
#include "OW_CIMName.hpp" // necessary for implicit conversion (const char* -> CIMName) to work
#include "blocxx/SafeBool.hpp"
#include <iosfwd>

namespace OW_NAMESPACE
{

class OW_COMMON_API CIMParamValue : public CIMBase
{
public:
	struct Data;
	/**
	 * Create an CIMParamValue
	 */
	CIMParamValue();
	/**
	 * Create a null CIMParamValue
	 */
	explicit CIMParamValue(CIMNULL_t);
	/**
	 * Create a new CIMParamValue object.
	 * @param name		The name of the parameter.
	 * @param val		The value of the parameter.
	 */
	CIMParamValue(const CIMName& name, const CIMValue& val);
	/**
	 * Create a new CIMParamValue object.  The value portion will be null.
	 * @param name		The name of the parameter.
	 */
	explicit CIMParamValue(const CIMName& name);
	/**
	 * Copy constructor
	 * @param arg	The CIMParamValue to make a copy of
	 */
	CIMParamValue(const CIMParamValue& arg);

	/**
	 * Destroy this CIMParamValue object.
	 */
	~CIMParamValue();
	/**
	 * Assignment operator
	 * @param arg The CIMParamValue to assign to this one.
	 * @return A reference to this CIMParamValue object.
	 */
	CIMParamValue& operator = (const CIMParamValue& arg);
	blocxx::String getName() const;
	CIMParamValue& setName(const CIMName& name);
	CIMValue getValue() const;
	CIMParamValue& setValue(const CIMValue& val);

	/**
	 * @return true if this CIMParamValue is a valid CIM data type
	 */
	BLOCXX_SAFE_BOOL_IMPL(CIMParamValue, blocxx::COWIntrusiveReference<Data>, CIMParamValue::m_pdata, m_pdata)

	/**
	 * Read this CIMParamValue from an inputstream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::streambuf & istrm);
	/**
	 * Write this CIMParamValue to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::streambuf & ostrm) const;
	virtual void setNull();
	/**
	 * @return The string representation of this CIMParamValue object.
	 */
	virtual blocxx::String toString() const;
	/**
	 * @return The MOF representation of this CIMParamValue as an String.
	 */
	virtual blocxx::String toMOF() const;
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	blocxx::COWIntrusiveReference<Data> m_pdata;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	friend OW_COMMON_API bool operator<(const CIMParamValue& x, const CIMParamValue& y);
};

/**
 * Search for paramName in params, and return the associated CIMValue. If
 * an element with paranName name isn't found, a CIMValue(CIMNULL) will be
 * returned.
 */
CIMValue getParamValue(const blocxx::String& paramName, const CIMParamValueArray& params);



} // end namespace OW_NAMESPACE

#endif
