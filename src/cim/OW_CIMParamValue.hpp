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
*	  this list of conditions and the following disclaimer in the documentation
*	  and/or other materials provided with the distribution.
*
*  - Neither the name of Vintela, Inc. nor the names of its
*	  contributors may be used to endorse or promote products derived from this
* 	  software without specific prior written permission.
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
#ifndef OW_CIMPARAMVALUE_HPP_INCLUDE_GUARD_
#define OW_CIMPARAMVALUE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_COWReference.hpp"
#include "OW_CIMBase.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMNULL.hpp"
#include <iosfwd>

namespace OpenWBEM
{

class CIMParamValue : public CIMBase
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
	CIMParamValue(const String& name, const CIMValue& val);
	/**
	 * Create a new CIMParamValue object.  The value portion will be null.
	 * @param name		The name of the parameter.
	 */
	explicit CIMParamValue(const String& name);
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
	String getName() const;
	CIMParamValue& setName(const String& name);
	CIMValue getValue() const;
	CIMParamValue& setValue(const CIMValue& val);
	/**
	 * @return true if this CIMParamValue is a valid CIM data type
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
	/**
	 * Read this CIMParamValue from an inputstream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);
	/**
	 * Write this CIMParamValue to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;
	virtual void setNull();
	/**
	 * @return The string representation of this CIMParamValue object.
	 */
	virtual String toString() const;
	/**
	 * @return The MOF representation of this CIMParamValue as an String.
	 */
	virtual String toMOF() const;
private:
	COWReference<Data> m_pdata;
	friend bool operator<(const CIMParamValue& x, const CIMParamValue& y);
};

} // end namespace OpenWBEM

typedef OpenWBEM::CIMParamValue OW_CIMParamValue OW_DEPRECATED;

#endif
