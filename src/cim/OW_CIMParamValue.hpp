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
*	  this list of conditions and the following disclaimer in the documentation
*	  and/or other materials provided with the distribution.
*
*  - Neither the name of Center 7 nor the names of its
*	  contributors may be used to endorse or promote products derived from this
* 	  software without specific prior written permission.
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
#ifndef OW_CIMPARAMVALUE_HPP_INCLUDE_GUARD_
#define OW_CIMPARAMVALUE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Bool.hpp"
#include "OW_COWReference.hpp"
#include "OW_CIMBase.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMNULL.hpp"
#include <iosfwd>

class OW_CIMParamValue : public OW_CIMBase
{
public:

	struct Data;

	/**
	 * Create an OW_CIMParamValue
	 */
	OW_CIMParamValue();

	/**
	 * Create a null OW_CIMParamValue
	 */
	explicit OW_CIMParamValue(OW_CIMNULL_t);

	/**
	 * Create a new OW_CIMParamValue object.
	 * @param name		The name of the parameter.
	 * @param val		The value of the parameter.
	 */
	OW_CIMParamValue(const OW_String& name, const OW_CIMValue& val);

	/**
	 * Create a new OW_CIMParamValue object.
	 * @param name		The name of the parameter.
	 */
	explicit OW_CIMParamValue(const OW_String& name);

	/**
	 * Copy constructor
	 * @param arg	The OW_CIMParamValue to make a copy of
	 */
	OW_CIMParamValue(const OW_CIMParamValue& arg);
	
	/**
	 * Destroy this OW_CIMParamValue object.
	 */
	~OW_CIMParamValue();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMParamValue to assign to this one.
	 * @return A reference to this OW_CIMParamValue object.
	 */
	OW_CIMParamValue& operator = (const OW_CIMParamValue& arg);

	OW_String getName() const;
	OW_CIMParamValue& setName(const OW_String& name);

	OW_CIMValue getValue() const;
	OW_CIMParamValue& setValue(const OW_CIMValue& val);

	/**
	 * @return true if this OW_CIMParamValue is a valid CIM data type
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
	 * Read this OW_CIMParamValue from an inputstream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * Write this OW_CIMParamValue to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	virtual void setNull();

	/**
	 * @return The string representation of this OW_CIMParamValue object.
	 */
	virtual OW_String toString() const;

	/**
	 * @return The MOF representation of this OW_CIMParamValue as an OW_String.
	 */
	virtual OW_String toMOF() const;

private:

	OW_COWReference<Data> m_pdata;

	friend bool operator<(const OW_CIMParamValue& x, const OW_CIMParamValue& y);
};

#endif


