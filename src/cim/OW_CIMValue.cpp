/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
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

#include "OW_config.h"
#include "OW_StringBuffer.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Assertion.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_CIMValueCast.hpp" // for OW_ValueCastException
#include "OW_CIMDateTime.hpp"

#include <new>
#include <cassert>

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
class OW_CIMValue::OW_CIMValueImpl
{
public:
	static OW_CIMValueImpl createSimpleValue(const OW_String& cimtype,
		const OW_String& value);

	OW_CIMValueImpl();
	OW_CIMValueImpl(const OW_CIMValueImpl& arg);
	explicit OW_CIMValueImpl(OW_Bool val);
	explicit OW_CIMValueImpl(bool val);
	explicit OW_CIMValueImpl(OW_UInt8 arg);
	explicit OW_CIMValueImpl(OW_Int8 arg);
	explicit OW_CIMValueImpl(OW_UInt16 arg);
	explicit OW_CIMValueImpl(OW_Int16 arg);
	explicit OW_CIMValueImpl(OW_UInt32 arg);
	explicit OW_CIMValueImpl(OW_Int32 arg);
	explicit OW_CIMValueImpl(OW_UInt64 arg);
	explicit OW_CIMValueImpl(OW_Int64 arg);
	explicit OW_CIMValueImpl(OW_Real32 arg);
	explicit OW_CIMValueImpl(OW_Real64 arg);
	explicit OW_CIMValueImpl(const OW_Char16& arg);
	explicit OW_CIMValueImpl(const OW_String& arg);
	explicit OW_CIMValueImpl(const OW_CIMDateTime& arg);
	explicit OW_CIMValueImpl(const OW_CIMObjectPath& arg);
	explicit OW_CIMValueImpl(const OW_CIMClass& arg);
	explicit OW_CIMValueImpl(const OW_CIMInstance& arg);
	explicit OW_CIMValueImpl(const OW_BoolArray& arg);
	explicit OW_CIMValueImpl(const OW_Char16Array& arg);
	explicit OW_CIMValueImpl(const OW_UInt8Array& arg);
	explicit OW_CIMValueImpl(const OW_Int8Array& arg);
	explicit OW_CIMValueImpl(const OW_UInt16Array& arg);
	explicit OW_CIMValueImpl(const OW_Int16Array& arg);
	explicit OW_CIMValueImpl(const OW_UInt32Array& arg);
	explicit OW_CIMValueImpl(const OW_Int32Array& arg);
	explicit OW_CIMValueImpl(const OW_UInt64Array& arg);
	explicit OW_CIMValueImpl(const OW_Int64Array& arg);
	explicit OW_CIMValueImpl(const OW_Real64Array& arg);
	explicit OW_CIMValueImpl(const OW_Real32Array& arg);
	explicit OW_CIMValueImpl(const OW_StringArray& arg);
	explicit OW_CIMValueImpl(const OW_CIMDateTimeArray& arg);
	explicit OW_CIMValueImpl(const OW_CIMObjectPathArray& arg);
	explicit OW_CIMValueImpl(const OW_CIMClassArray& arg);
	explicit OW_CIMValueImpl(const OW_CIMInstanceArray& arg);
	~OW_CIMValueImpl();

    OW_CIMValueImpl* clone() { return new OW_CIMValueImpl(*this); }

	void get(OW_Bool& val) const;
	void get(OW_Char16& arg) const;
	void get(OW_UInt8& arg) const;
	void get(OW_Int8& arg) const;
	void get(OW_UInt16& arg) const;
	void get(OW_Int16& arg) const;
	void get(OW_UInt32& arg) const;
	void get(OW_Int32& arg) const;
	void get(OW_UInt64& arg) const;
	void get(OW_Int64& arg) const;
	void get(OW_Real32& arg) const;
	void get(OW_Real64& arg) const;
	void get(OW_String& arg) const;
	void get(OW_CIMDateTime& arg) const;
	void get(OW_CIMObjectPath& arg) const;
	void get(OW_CIMClass& arg) const;
	void get(OW_CIMInstance& arg) const;
	void get(OW_Char16Array& arg) const;
	void get(OW_UInt8Array& arg) const;
	void get(OW_Int8Array& arg) const;
	void get(OW_UInt16Array& arg) const;
	void get(OW_Int16Array& arg) const;
	void get(OW_UInt32Array& arg) const;
	void get(OW_Int32Array& arg) const;
	void get(OW_UInt64Array& arg) const;
	void get(OW_Int64Array& arg) const;
	void get(OW_Real64Array& arg) const;
	void get(OW_Real32Array& arg) const;
	void get(OW_StringArray& arg) const;
	void get(OW_BoolArray& arg) const;
	void get(OW_CIMDateTimeArray& arg) const;
	void get(OW_CIMObjectPathArray& arg) const;
	void get(OW_CIMClassArray& arg) const;
	void get(OW_CIMInstanceArray& arg) const;

	OW_UInt32 getArraySize() const;

	OW_CIMValueImpl& operator= (const OW_CIMValueImpl& arg);
	OW_CIMValueImpl& set(const OW_CIMValueImpl& arg);

	OW_Bool equal(const OW_CIMValueImpl& arg) const;
	OW_Bool operator== (const OW_CIMValueImpl& arg) const
	{
		return equal(arg);
	}
	OW_Bool operator!= (const OW_CIMValueImpl& arg) const
	{
		return !(*this == arg);
	}
	OW_Bool operator<= (const OW_CIMValueImpl& arg) const
	{
		return !(arg < *this);
	}
	OW_Bool operator>= (const OW_CIMValueImpl& arg) const
	{
		return !(*this < arg);
	}
	OW_Bool operator< (const OW_CIMValueImpl& arg) const;
	OW_Bool operator> (const OW_CIMValueImpl& arg) const
	{
		return arg < *this;
	}

	OW_CIMDataType::Type getType() const
	{
		return m_type;
	}

	OW_CIMDataType getCIMDataType() const
	{
		OW_CIMDataType rval = OW_CIMDataType(getType());
		if (m_isArray)
		{
			rval.setToArrayType(-1);
		}
		return rval;
	}

	OW_Bool sameType(const OW_CIMValueImpl& arg) const
	{
		return(m_type == arg.m_type && m_isArray == arg.m_isArray);
	}

	OW_Bool isArray() const
	{
		return m_isArray;
	}

	void readObject(std::istream &istrm);
	void writeObject(std::ostream &ostrm) const;
	OW_String toString(OW_Bool forMOF=false) const;
	OW_String toMOF() const;

private:

	union OW_CIMValueData
	{
		OW_UInt8	m_booleanValue;
		OW_UInt8 	m_uint8Value;
		OW_Int8 		m_sint8Value;
		OW_UInt16 	m_uint16Value;
		OW_Int16 	m_sint16Value;
		OW_UInt32 	m_uint32Value;
		OW_Int32 	m_sint32Value;
		OW_UInt64 	m_uint64Value;
		OW_Int64 	m_sint64Value;
		OW_Real32 	m_real32Value;
		OW_Real64 	m_real64Value;

		char bfr1[sizeof(OW_BoolArray)];
		char bfr2[sizeof(OW_UInt8Array)];
		char bfr3[sizeof(OW_Int8Array)];
		char bfr4[sizeof(OW_Char16Array)];
		char bfr5[sizeof(OW_UInt16Array)];
		char bfr6[sizeof(OW_Int16Array)];
		char bfr7[sizeof(OW_UInt32Array)];
		char bfr8[sizeof(OW_Int32Array)];
		char bfr9[sizeof(OW_UInt64Array)];
		char bfr10[sizeof(OW_Int64Array)];
		char bfr11[sizeof(OW_Real64Array)];
		char bfr12[sizeof(OW_Real32Array)];
		char bfr13[sizeof(OW_StringArray)];
		char bfr14[sizeof(OW_CIMObjectPathArray)];
		char bfr15[sizeof(OW_CIMDateTimeArray)];
		char bfr16[sizeof(OW_CIMObjectPath)];
		char bfr17[sizeof(OW_Char16)];
		char bfr18[sizeof(OW_CIMDateTime)];
		char bfr19[sizeof(OW_String)];
		char bfr20[sizeof(OW_CIMClass)];
		char bfr21[sizeof(OW_CIMInstance)];
		char bfr22[sizeof(OW_CIMClassArray)];
		char bfr23[sizeof(OW_CIMInstanceArray)];
	};

	void setupObject(const OW_CIMValueData& odata, OW_CIMDataType::Type type, OW_Bool isArray);
	void destroyObject();

	OW_CIMDataType::Type m_type;
	OW_Bool m_isArray;
	OW_Bool m_objDestroyed;
	OW_CIMValueData m_obj;
};
//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMValue
OW_CIMValue::createSimpleValue(const OW_String& cimtype,
	const OW_String& value)
{
	int type = OW_CIMDataType::strToSimpleType(cimtype);
	if(type == OW_CIMDataType::INVALID)
	{
		return OW_CIMValue(OW_CIMNULL);
	}

	OW_CIMValueImpl impl = OW_CIMValueImpl::createSimpleValue(cimtype, value);
	OW_CIMValue cv(OW_CIMNULL);
	cv.m_impl = new OW_CIMValueImpl(impl);
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
void				
OW_CIMValue::readObject(istream &istrm)
{
	if(m_impl.isNull())
	{
		m_impl = new OW_CIMValueImpl;
	}

	m_impl->readObject(istrm);
}

#ifdef OW_NEW
#undef new
#endif

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_CIMNULL_t)
	: OW_CIMBase(), m_impl(0) { }

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMValue& x)
	: OW_CIMBase(), m_impl(x.m_impl) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_Bool x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)){}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(bool x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)){}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_UInt8 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_Int8 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_UInt16 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_Int16 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_UInt32 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_Int32 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_UInt64 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_Int64 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_Real32 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(OW_Real64 x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_Char16& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_String& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const char* x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(OW_String(x))) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMDateTime& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMObjectPath& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMClass& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMInstance& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMObjectPathArray& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_BoolArray& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_Char16Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_UInt8Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_Int8Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_UInt16Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_Int16Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_UInt32Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_Int32Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_UInt64Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_Int64Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_Real64Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_Real32Array& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_StringArray& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMDateTimeArray& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMClassArray& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValue(const OW_CIMInstanceArray& x)
	: OW_CIMBase(), m_impl(new OW_CIMValueImpl(x)) {}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_CIMValue::getArraySize() const
{
	return m_impl->getArraySize();
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Bool& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Char16& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_UInt8& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Int8& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_UInt16& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Int16& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_UInt32& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Int32& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_UInt64& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Int64& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Real32& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Real64& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_String& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_CIMDateTime& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_CIMObjectPath& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_CIMClass& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_CIMInstance& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Char16Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_UInt8Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Int8Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_UInt16Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Int16Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_UInt32Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Int32Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_UInt64Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Int64Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Real64Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_Real32Array& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_StringArray& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_CIMObjectPathArray& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_BoolArray& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_CIMDateTimeArray& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_CIMClassArray& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::get(OW_CIMInstanceArray& x) const
{
	m_impl->get(x);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::toBool() const
{
	OW_Bool rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Char16 OW_CIMValue::toChar16() const
{
	OW_Char16 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt8 OW_CIMValue::toUInt8() const
{
	OW_UInt8 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int8 OW_CIMValue::toInt8() const
{
	OW_Int8 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt16 OW_CIMValue::toUInt16() const
{
	OW_UInt16 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int16 OW_CIMValue::toInt16() const
{
	OW_Int16 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32 OW_CIMValue::toUInt32() const
{
	OW_UInt32 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32 OW_CIMValue::toInt32() const
{
	OW_Int32 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt64 OW_CIMValue::toUInt64() const
{
	OW_UInt64 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int64 OW_CIMValue::toInt64() const
{
	OW_Int64 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Real32 OW_CIMValue::toReal32() const
{
	OW_Real32 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Real64 OW_CIMValue::toReal64() const
{
	OW_Real64 rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime OW_CIMValue::toCIMDateTime() const
{
	OW_CIMDateTime rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath OW_CIMValue::toCIMObjectPath() const
{
	OW_CIMObjectPath rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass OW_CIMValue::toCIMClass() const
{
	OW_CIMClass rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance OW_CIMValue::toCIMInstance() const
{
	OW_CIMInstance rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathArray OW_CIMValue::toCIMObjectPathArray() const
{
	OW_CIMObjectPathArray rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Char16Array OW_CIMValue::toChar16Array() const
{
	OW_Char16Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt8Array OW_CIMValue::toUInt8Array() const
{
	OW_UInt8Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int8Array OW_CIMValue::toInt8Array() const
{
	OW_Int8Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt16Array OW_CIMValue::toUInt16Array() const
{
	OW_UInt16Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int16Array OW_CIMValue::toInt16Array() const
{
	OW_Int16Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32Array OW_CIMValue::toUInt32Array() const
{
	OW_UInt32Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32Array OW_CIMValue::toInt32Array() const
{
	OW_Int32Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt64Array OW_CIMValue::toUInt64Array() const
{
	OW_UInt64Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int64Array OW_CIMValue::toInt64Array() const
{
	OW_Int64Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Real64Array OW_CIMValue::toReal64Array() const
{
	OW_Real64Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Real32Array OW_CIMValue::toReal32Array() const
{
	OW_Real32Array rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray OW_CIMValue::toStringArray() const
{
	OW_StringArray rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_BoolArray OW_CIMValue::toBoolArray() const
{
	OW_BoolArray rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTimeArray OW_CIMValue::toCIMDateTimeArray() const
{
	OW_CIMDateTimeArray rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassArray OW_CIMValue::toCIMClassArray() const
{
	OW_CIMClassArray rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray OW_CIMValue::toCIMInstanceArray() const
{
	OW_CIMInstanceArray rval;
	m_impl->get(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::setNull()
{
	m_impl = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue& OW_CIMValue::set(const OW_CIMValue& x)
{
	m_impl = x.m_impl;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue& OW_CIMValue::operator= (const OW_CIMValue& x)
{
	set(x);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::equal(const OW_CIMValue& x) const
{
	if(x.m_impl == m_impl)
		return true;

	if(x.m_impl == NULL || m_impl == NULL)
		return false;

	return m_impl->equal(*x.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::operator== (const OW_CIMValue& x) const
{
	return equal(x);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::operator!= (const OW_CIMValue& x) const
{
	if(x.m_impl == m_impl)
		return false;

	if(x.m_impl == NULL || m_impl == NULL)
		return true;

	return m_impl->operator!=(*x.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::operator<= (const OW_CIMValue& x) const
{
	if(x.m_impl == m_impl)
		return true;

	if(m_impl == NULL)
		return true;

	if(x.m_impl == NULL)
		return false;

	return m_impl->operator<=(*x.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::operator>= (const OW_CIMValue& x) const
{
	if(x.m_impl == m_impl)
		return true;

	if(m_impl == NULL)
		return false;

	if(x.m_impl == NULL)
		return true;

	return m_impl->operator >=(*x.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::operator< (const OW_CIMValue& x) const
{
	if(x.m_impl == m_impl)
		return false;

	if(m_impl == NULL)
		return true;

	if(x.m_impl == NULL)
		return false;

	return m_impl->operator <(*x.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::operator> (const OW_CIMValue& x) const
{
	if(x.m_impl == m_impl)
		return false;

	if(m_impl == NULL)
		return false;

	if(x.m_impl == NULL)
		return true;

	return m_impl->operator >(*x.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::Type OW_CIMValue::getType() const {  return m_impl->getType(); }

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType OW_CIMValue::getCIMDataType() const { return m_impl->getCIMDataType(); }

//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::sameType(const OW_CIMValue& x) const
{
	return m_impl->sameType(*x.m_impl);
}


//////////////////////////////////////////////////////////////////////////////
OW_Bool OW_CIMValue::isArray() const {  return m_impl->isArray(); }

//////////////////////////////////////////////////////////////////////////////
void OW_CIMValue::writeObject(std::ostream &ostrm) const
{
	m_impl->writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
OW_String OW_CIMValue::toString() const
{
	if (m_impl)
		return m_impl->toString();
	else
	{
		return "(null)";
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String OW_CIMValue::toMOF() const
{
	return m_impl->toMOF();
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMValue::isNumericType() const
{
	return OW_CIMDataType::isNumericType(getType());
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMValue::OW_CIMValueImpl
OW_CIMValue::OW_CIMValueImpl::createSimpleValue(const OW_String& cimtype,
	const OW_String& value)
{
	OW_CIMValueImpl cimValue;
	OW_CIMDataType::Type type = OW_CIMDataType::strToSimpleType(cimtype);

	switch(type)
	{
		case OW_CIMDataType::UINT8:
			cimValue = OW_CIMValueImpl(value.toUInt8());
			break;
			
		case OW_CIMDataType::SINT8:
			cimValue = OW_CIMValueImpl(value.toInt8());
			break;

		case OW_CIMDataType::UINT16:
			cimValue = OW_CIMValueImpl(value.toUInt16());
			break;

		case OW_CIMDataType::SINT16:
			cimValue = OW_CIMValueImpl(value.toInt16());
			break;

		case OW_CIMDataType::UINT32:
			cimValue = OW_CIMValueImpl(value.toUInt32());
			break;

		case OW_CIMDataType::SINT32:
			cimValue = OW_CIMValueImpl(value.toInt32());
			break;

		case OW_CIMDataType::UINT64:
			cimValue = OW_CIMValueImpl(value.toUInt64());
			break;

		case OW_CIMDataType::SINT64:
			cimValue = OW_CIMValueImpl(value.toInt64());
			break;

		case OW_CIMDataType::BOOLEAN:
			cimValue = OW_CIMValueImpl(value.toBool());
			break;

		case OW_CIMDataType::REAL32:
			cimValue = OW_CIMValueImpl(value.toReal32());
			break;

		case OW_CIMDataType::REAL64:
			cimValue = OW_CIMValueImpl(value.toReal64());
			break;

		case OW_CIMDataType::CHAR16:
			cimValue = OW_CIMValueImpl(value.toChar16());
			break;

		case OW_CIMDataType::DATETIME:
			cimValue = OW_CIMValueImpl(value.toDateTime());
			break;

		case OW_CIMDataType::STRING:
			cimValue = OW_CIMValueImpl(value);
			break;

		case OW_CIMDataType::REFERENCE:
			cimValue = OW_CIMValueImpl(OW_CIMObjectPath::parse(value));
			break;

		default:
			OW_ASSERT(0);
	}

	return cimValue;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl():
	m_type(OW_CIMDataType::CIMNULL), m_isArray(false),
	m_objDestroyed(true), m_obj()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMValueImpl& arg) :
	m_type(OW_CIMDataType::CIMNULL), m_isArray(false),
	m_objDestroyed(true), m_obj()
{
	setupObject(arg.m_obj, arg.m_type, arg.m_isArray);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_Bool v) :
	m_type(OW_CIMDataType::BOOLEAN), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_booleanValue = (v) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(bool v) :
	m_type(OW_CIMDataType::BOOLEAN), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_booleanValue = (v) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_UInt8 v) :
	m_type(OW_CIMDataType::UINT8), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_uint8Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_Int8 v) :
	m_type(OW_CIMDataType::SINT8), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_sint8Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_Char16& v) :
	m_type(OW_CIMDataType::CHAR16), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_Char16(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_UInt16 v) :
	m_type(OW_CIMDataType::UINT16), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_uint16Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_Int16 v) :
	m_type(OW_CIMDataType::SINT16), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_sint16Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_UInt32 v) :
	m_type(OW_CIMDataType::UINT32), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_uint32Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_Int32 v) :
	m_type(OW_CIMDataType::SINT32), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_sint32Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_UInt64 v) :
	m_type(OW_CIMDataType::UINT64), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_uint64Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_Int64 v) :
	m_type(OW_CIMDataType::SINT64), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_sint64Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_Real32 v) :
	m_type(OW_CIMDataType::REAL32), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_real32Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(OW_Real64 v) :
	m_type(OW_CIMDataType::REAL64), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_real64Value = v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_String& v) :
	m_type(OW_CIMDataType::STRING), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_String(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMDateTime& v) :
	m_type(OW_CIMDataType::DATETIME), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_CIMDateTime(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMObjectPath& v) :
	m_type(OW_CIMDataType::REFERENCE), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_CIMObjectPath(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMClass& v) :
	m_type(OW_CIMDataType::EMBEDDEDCLASS), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_CIMClass(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMInstance& v) :
	m_type(OW_CIMDataType::EMBEDDEDINSTANCE), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_CIMInstance(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_Char16Array& v) :
	m_type(OW_CIMDataType::CHAR16), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_Char16Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_UInt8Array& v) :
	m_type(OW_CIMDataType::UINT8), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_UInt8Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_Int8Array& v) :
	m_type(OW_CIMDataType::SINT8), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_Int8Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_UInt16Array& v) :
	m_type(OW_CIMDataType::UINT16), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_UInt16Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_Int16Array& v) :
	m_type(OW_CIMDataType::SINT16), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_Int16Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_UInt32Array& v) :
	m_type(OW_CIMDataType::UINT32), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_UInt32Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_Int32Array& v) :
	m_type(OW_CIMDataType::SINT32), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_Int32Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_UInt64Array& v) :
	m_type(OW_CIMDataType::UINT64), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_UInt64Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_Int64Array& v) :
	m_type(OW_CIMDataType::SINT64), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_Int64Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_Real64Array& v) :
	m_type(OW_CIMDataType::REAL64), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_Real64Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_Real32Array& v) :
	m_type(OW_CIMDataType::REAL32), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_Real32Array(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_StringArray& v) :
	m_type(OW_CIMDataType::STRING), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_StringArray(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMDateTimeArray& v) :
	m_type(OW_CIMDataType::DATETIME), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_CIMDateTimeArray(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMObjectPathArray& v) :
	m_type(OW_CIMDataType::REFERENCE), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_CIMObjectPathArray(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_BoolArray& v) :
	m_type(OW_CIMDataType::BOOLEAN), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_BoolArray(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMClassArray& v) :
	m_type(OW_CIMDataType::EMBEDDEDCLASS), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_CIMClassArray(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::OW_CIMValueImpl(const OW_CIMInstanceArray& v) :
	m_type(OW_CIMDataType::EMBEDDEDINSTANCE), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) OW_CIMInstanceArray(v);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl::~OW_CIMValueImpl()
{
	destroyObject();
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_CIMValue::OW_CIMValueImpl::getArraySize() const
{
	if(m_type == OW_CIMDataType::CIMNULL)
	{
		return 0;
	}

	if(!m_isArray)
	{
		return 1;
	}

	OW_UInt32 sz = 0;
	switch(m_type)
	{
		case OW_CIMDataType::UINT8:
			sz = (reinterpret_cast<const OW_UInt8Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::SINT8:
			sz = (reinterpret_cast<const OW_Int8Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::UINT16:
			sz = (reinterpret_cast<const OW_UInt16Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::SINT16:
			sz = (reinterpret_cast<const OW_Int16Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::UINT32:
			sz = (reinterpret_cast<const OW_UInt32Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::SINT32:
			sz = (reinterpret_cast<const OW_Int32Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::UINT64:
			sz = (reinterpret_cast<const OW_UInt64Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::SINT64:
			sz = (reinterpret_cast<const OW_Int64Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::BOOLEAN:
			sz = (reinterpret_cast<const OW_BoolArray*>(&m_obj))->size(); break;
		case OW_CIMDataType::REAL32:
			sz = (reinterpret_cast<const OW_Real32Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::REAL64:
			sz = (reinterpret_cast<const OW_Real64Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::CHAR16:
			sz = (reinterpret_cast<const OW_Char16Array*>(&m_obj))->size(); break;
		case OW_CIMDataType::DATETIME:
			sz = (reinterpret_cast<const OW_CIMDateTimeArray*>(&m_obj))->size(); break;
		case OW_CIMDataType::STRING:
			sz = (reinterpret_cast<const OW_StringArray*>(&m_obj))->size(); break;
		case OW_CIMDataType::REFERENCE:
			sz = (reinterpret_cast<const OW_CIMObjectPathArray*>(&m_obj))->size(); break;
		case OW_CIMDataType::EMBEDDEDCLASS:
			sz = (reinterpret_cast<const OW_CIMClassArray*>(&m_obj))->size(); break;
		case OW_CIMDataType::EMBEDDEDINSTANCE:
			sz = (reinterpret_cast<const OW_CIMInstanceArray*>(&m_obj))->size(); break;
		default:
			sz = 0;
	}

	return sz;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::setupObject(const OW_CIMValueData& odata, OW_CIMDataType::Type type, OW_Bool isArray)
{
	destroyObject();
	m_objDestroyed = false;
	m_type = type;
	m_isArray = isArray;
	if(isArray)
	{
		switch(m_type)
		{
			case OW_CIMDataType::UINT8:
				new(&m_obj) OW_UInt8Array(*(reinterpret_cast<const OW_UInt8Array*>(&odata))); break;
			case OW_CIMDataType::SINT8:
				new(&m_obj) OW_Int8Array(*(reinterpret_cast<const OW_Int8Array*>(&odata))); break;
			case OW_CIMDataType::UINT16:
				new(&m_obj) OW_UInt16Array(*(reinterpret_cast<const OW_UInt16Array*>(&odata))); break;
			case OW_CIMDataType::SINT16:
				new(&m_obj) OW_Int16Array(*(reinterpret_cast<const OW_Int16Array*>(&odata))); break;
			case OW_CIMDataType::UINT32:
				new(&m_obj) OW_UInt32Array(*(reinterpret_cast<const OW_UInt32Array*>(&odata))); break;
			case OW_CIMDataType::SINT32:
				new(&m_obj) OW_Int32Array(*(reinterpret_cast<const OW_Int32Array*>(&odata))); break;
			case OW_CIMDataType::UINT64:
				new(&m_obj) OW_UInt64Array(*(reinterpret_cast<const OW_UInt64Array*>(&odata))); break;
			case OW_CIMDataType::SINT64:
				new(&m_obj) OW_Int64Array(*(reinterpret_cast<const OW_Int64Array*>(&odata))); break;
			case OW_CIMDataType::BOOLEAN:
				new(&m_obj) OW_BoolArray(*(reinterpret_cast<const OW_BoolArray*>(&odata))); break;
			case OW_CIMDataType::REAL32:
				new(&m_obj) OW_Real32Array(*(reinterpret_cast<const OW_Real32Array*>(&odata))); break;
			case OW_CIMDataType::REAL64:
				new(&m_obj) OW_Real64Array(*(reinterpret_cast<const OW_Real64Array*>(&odata))); break;
			case OW_CIMDataType::CHAR16:
				new(&m_obj) OW_Char16Array(*(reinterpret_cast<const OW_Char16Array*>(&odata))); break;
			case OW_CIMDataType::DATETIME:
				new(&m_obj) OW_CIMDateTimeArray(*(reinterpret_cast<const OW_CIMDateTimeArray*>(&odata))); break;
			case OW_CIMDataType::STRING:
				new(&m_obj) OW_StringArray(*(reinterpret_cast<const OW_StringArray*>(&odata))); break;
			case OW_CIMDataType::REFERENCE:
				new(&m_obj) OW_CIMObjectPathArray(*(reinterpret_cast<const OW_CIMObjectPathArray*>(&odata))); break;
			case OW_CIMDataType::EMBEDDEDCLASS:
				new(&m_obj) OW_CIMClassArray(*(reinterpret_cast<const OW_CIMClassArray*>(&odata))); break;
			case OW_CIMDataType::EMBEDDEDINSTANCE:
				new(&m_obj) OW_CIMInstanceArray(*(reinterpret_cast<const OW_CIMInstanceArray*>(&odata))); break;
			default:
				m_objDestroyed = true;
				m_type = OW_CIMDataType::CIMNULL;
				break;
		}
	}
	else
	{
		switch(m_type)
		{
			case OW_CIMDataType::UINT8:
			case OW_CIMDataType::SINT8:
			case OW_CIMDataType::UINT16:
			case OW_CIMDataType::SINT16:
			case OW_CIMDataType::UINT32:
			case OW_CIMDataType::SINT32:
			case OW_CIMDataType::UINT64:
			case OW_CIMDataType::SINT64:
			case OW_CIMDataType::BOOLEAN:
			case OW_CIMDataType::REAL32:
			case OW_CIMDataType::REAL64:
				::memmove(&m_obj, &odata, sizeof(m_obj));
				break;

			case OW_CIMDataType::CHAR16:
				new(&m_obj) OW_Char16(*(reinterpret_cast<const OW_Char16*>(&odata))); break;
			case OW_CIMDataType::REFERENCE:
				new(&m_obj) OW_CIMObjectPath(*(reinterpret_cast<const OW_CIMObjectPath*>(&odata))); break;
			case OW_CIMDataType::DATETIME:
				new(&m_obj) OW_CIMDateTime(*(reinterpret_cast<const OW_CIMDateTime*>(&odata))); break;
			case OW_CIMDataType::STRING:
				new(&m_obj) OW_String(*(reinterpret_cast<const OW_String*>(&odata))); break;
			case OW_CIMDataType::EMBEDDEDCLASS:
				new(&m_obj) OW_CIMClass(*(reinterpret_cast<const OW_CIMClass*>(&odata))); break;
			case OW_CIMDataType::EMBEDDEDINSTANCE:
				new(&m_obj) OW_CIMInstance(*(reinterpret_cast<const OW_CIMInstance*>(&odata))); break;

			default:
				m_objDestroyed = true;
				m_type = OW_CIMDataType::CIMNULL;
				break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::destroyObject()
{
	if(m_objDestroyed)
	{
		return;
	}

	m_objDestroyed = true;

	if(m_isArray)
	{
		switch(m_type)
		{
			case OW_CIMDataType::UINT8:
				(reinterpret_cast<OW_UInt8Array*>(&m_obj))->~OW_UInt8Array(); break;
			case OW_CIMDataType::SINT8:
				(reinterpret_cast<OW_Int8Array*>(&m_obj))->~OW_Int8Array(); break;
			case OW_CIMDataType::UINT16:
				(reinterpret_cast<OW_UInt16Array*>(&m_obj))->~OW_UInt16Array(); break;
			case OW_CIMDataType::SINT16:
				(reinterpret_cast<OW_Int16Array*>(&m_obj))->~OW_Int16Array(); break;
			case OW_CIMDataType::UINT32:
				(reinterpret_cast<OW_UInt32Array*>(&m_obj))->~OW_UInt32Array(); break;
			case OW_CIMDataType::SINT32:
				(reinterpret_cast<OW_Int32Array*>(&m_obj))->~OW_Int32Array(); break;
			case OW_CIMDataType::UINT64:
				(reinterpret_cast<OW_UInt64Array*>(&m_obj))->~OW_UInt64Array(); break;
			case OW_CIMDataType::SINT64:
				(reinterpret_cast<OW_Int64Array*>(&m_obj))->~OW_Int64Array(); break;
			case OW_CIMDataType::BOOLEAN:
				(reinterpret_cast<OW_BoolArray*>(&m_obj))->~OW_BoolArray(); break;
			case OW_CIMDataType::REAL32:
				(reinterpret_cast<OW_Real32Array*>(&m_obj))->~OW_Real32Array(); break;
			case OW_CIMDataType::REAL64:
				(reinterpret_cast<OW_Real64Array*>(&m_obj))->~OW_Real64Array(); break;
			case OW_CIMDataType::CHAR16:
				(reinterpret_cast<OW_Char16Array*>(&m_obj))->~OW_Char16Array(); break;
			case OW_CIMDataType::DATETIME:
				(reinterpret_cast<OW_CIMDateTimeArray*>(&m_obj))->~OW_CIMDateTimeArray(); break;
			case OW_CIMDataType::STRING:
				(reinterpret_cast<OW_StringArray*>(&m_obj))->~OW_StringArray(); break;
			case OW_CIMDataType::REFERENCE:
				(reinterpret_cast<OW_CIMObjectPathArray*>(&m_obj))->~OW_CIMObjectPathArray(); break;
			case OW_CIMDataType::EMBEDDEDCLASS:
				(reinterpret_cast<OW_CIMClassArray*>(&m_obj))->~OW_CIMClassArray(); break;
			case OW_CIMDataType::EMBEDDEDINSTANCE:
				(reinterpret_cast<OW_CIMInstanceArray*>(&m_obj))->~OW_CIMInstanceArray(); break;
			default:
				assert(0); // don't want to throw from a destructor, just segfault
		}
	}
	else
	{
		switch(m_type)
		{
			case OW_CIMDataType::UINT8:
			case OW_CIMDataType::SINT8:
			case OW_CIMDataType::UINT16:
			case OW_CIMDataType::SINT16:
			case OW_CIMDataType::UINT32:
			case OW_CIMDataType::SINT32:
			case OW_CIMDataType::UINT64:
			case OW_CIMDataType::SINT64:
			case OW_CIMDataType::BOOLEAN:
			case OW_CIMDataType::REAL32:
			case OW_CIMDataType::REAL64:
				break;
			case OW_CIMDataType::CHAR16:
				(reinterpret_cast<OW_Char16*>(&m_obj))->~OW_Char16();
				break;
			case OW_CIMDataType::REFERENCE:
				(reinterpret_cast<OW_CIMObjectPath*>(&m_obj))->~OW_CIMObjectPath();
				break;
			case OW_CIMDataType::DATETIME:
				(reinterpret_cast<OW_CIMDateTime*>(&m_obj))->~OW_CIMDateTime();
				break;
			case OW_CIMDataType::STRING:
				(reinterpret_cast<OW_String*>(&m_obj))->~OW_String();
				break;
			case OW_CIMDataType::EMBEDDEDCLASS:
				(reinterpret_cast<OW_CIMClass*>(&m_obj))->~OW_CIMClass();
				break;
			case OW_CIMDataType::EMBEDDEDINSTANCE:
				(reinterpret_cast<OW_CIMInstance*>(&m_obj))->~OW_CIMInstance();
				break;
			default:
				assert(0); // don't want to throw from a destructor, just segfault
		}
	}

	m_type = OW_CIMDataType::CIMNULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl&
OW_CIMValue::OW_CIMValueImpl::set(const OW_CIMValueImpl& arg)
{
	setupObject(arg.m_obj, arg.m_type, arg.m_isArray);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue::OW_CIMValueImpl&
OW_CIMValue::OW_CIMValueImpl::operator= (const OW_CIMValueImpl& arg)
{
	set(arg);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMValue::OW_CIMValueImpl::equal(const OW_CIMValueImpl& arg) const
{
	OW_Bool cc = false;

	if(sameType(arg))
	{
		if(m_isArray)
		{
			switch(m_type)
			{
				case OW_CIMDataType::CHAR16:
					cc = *(reinterpret_cast<const OW_Char16Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_Char16Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::UINT8:
					cc = *(reinterpret_cast<const OW_UInt8Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_UInt8Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::SINT8:
					cc = *(reinterpret_cast<const OW_Int8Array*>(&m_obj)) == *(reinterpret_cast<const OW_Int8Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::UINT16:
					cc = *(reinterpret_cast<const OW_UInt16Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_UInt16Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::SINT16:
					cc = *(reinterpret_cast<const OW_Int16Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_Int16Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::UINT32:
					cc = *(reinterpret_cast<const OW_UInt32Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_UInt32Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::SINT32:
					cc = *(reinterpret_cast<const OW_Int32Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_Int32Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::UINT64:
					cc = *(reinterpret_cast<const OW_UInt64Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_UInt64Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::SINT64:
					cc = *(reinterpret_cast<const OW_Int64Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_Int64Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::STRING:
					cc = *(reinterpret_cast<const OW_StringArray*>(&m_obj)) ==
						*(reinterpret_cast<const OW_StringArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::REFERENCE:
					cc = *(reinterpret_cast<const OW_CIMObjectPathArray*>(&m_obj)) ==
						*(reinterpret_cast<const OW_CIMObjectPathArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::BOOLEAN:
					cc = *(reinterpret_cast<const OW_BoolArray*>(&m_obj)) == *(reinterpret_cast<const OW_BoolArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::REAL32:
					cc = *(reinterpret_cast<const OW_Real32Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_Real32Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::REAL64:
					cc = *(reinterpret_cast<const OW_Real64Array*>(&m_obj)) ==
						*(reinterpret_cast<const OW_Real64Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::DATETIME:
					cc = *(reinterpret_cast<const OW_CIMDateTimeArray*>(&m_obj)) ==
						*(reinterpret_cast<const OW_CIMDateTimeArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::EMBEDDEDCLASS:
					cc = *(reinterpret_cast<const OW_CIMClassArray*>(&m_obj)) ==
						*(reinterpret_cast<const OW_CIMClassArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::EMBEDDEDINSTANCE:
					cc = *(reinterpret_cast<const OW_CIMInstanceArray*>(&m_obj)) ==
						*(reinterpret_cast<const OW_CIMInstanceArray*>(&arg.m_obj));
					break;
				default:
					OW_ASSERT(0);
			}
		}
		else
		{
			switch(m_type)
			{
				case OW_CIMDataType::UINT8:
					cc = m_obj.m_uint8Value == arg.m_obj.m_uint8Value;
					break;

				case OW_CIMDataType::SINT8:
					cc = m_obj.m_sint8Value == arg.m_obj.m_sint8Value;
					break;

				case OW_CIMDataType::UINT16:
					cc = m_obj.m_uint16Value == arg.m_obj.m_uint16Value;
					break;

				case OW_CIMDataType::SINT16:
					cc = m_obj.m_sint16Value == arg.m_obj.m_sint16Value;
					break;

				case OW_CIMDataType::UINT32:
					cc = m_obj.m_uint32Value == arg.m_obj.m_uint32Value;
					break;

				case OW_CIMDataType::SINT32:
					cc = m_obj.m_sint32Value == arg.m_obj.m_sint32Value;
					break;

				case OW_CIMDataType::UINT64:
					cc = m_obj.m_uint64Value == arg.m_obj.m_uint64Value;
					break;

				case OW_CIMDataType::SINT64:
					cc = m_obj.m_sint64Value == arg.m_obj.m_sint64Value;
					break;

				case OW_CIMDataType::CHAR16:
					cc = *(reinterpret_cast<const OW_Char16*>(&m_obj)) == *(reinterpret_cast<const OW_Char16*>(&arg.m_obj));
					break;

				case OW_CIMDataType::STRING:
					cc = *(reinterpret_cast<const OW_String*>(&m_obj)) == *(reinterpret_cast<const OW_String*>(&arg.m_obj));
					break;

				case OW_CIMDataType::BOOLEAN:
					cc = m_obj.m_booleanValue == arg.m_obj.m_booleanValue;
					break;

				case OW_CIMDataType::REAL32:
					cc = m_obj.m_real32Value == arg.m_obj.m_real32Value;
					break;

				case OW_CIMDataType::REAL64:
					cc = m_obj.m_real64Value == arg.m_obj.m_real64Value;
					break;

				case OW_CIMDataType::DATETIME:
					cc = *(reinterpret_cast<const OW_CIMDateTime*>(&m_obj)) ==
						*(reinterpret_cast<const OW_CIMDateTime*>(&arg.m_obj));
					break;

				case OW_CIMDataType::REFERENCE:
					cc = *(reinterpret_cast<const OW_CIMObjectPath*>(&m_obj)) ==
						*(reinterpret_cast<const OW_CIMObjectPath*>(&arg.m_obj));
					break;

				case OW_CIMDataType::EMBEDDEDCLASS:
					cc = *(reinterpret_cast<const OW_CIMClass*>(&m_obj)) ==
						*(reinterpret_cast<const OW_CIMClass*>(&arg.m_obj));
					break;

				case OW_CIMDataType::EMBEDDEDINSTANCE:
					cc = *(reinterpret_cast<const OW_CIMInstance*>(&m_obj)) ==
						*(reinterpret_cast<const OW_CIMInstance*>(&arg.m_obj));
					break;
				default:
					OW_ASSERT(0);
			}
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMValue::OW_CIMValueImpl::operator<(const OW_CIMValueImpl& arg) const
{
	OW_Bool cc = false;

	if(sameType(arg))
	{
		if(m_isArray)
		{
			switch(m_type)
			{
				case OW_CIMDataType::CHAR16:
					cc = *(reinterpret_cast<const OW_Char16Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_Char16Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::UINT8:
					cc = *(reinterpret_cast<const OW_UInt8Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_UInt8Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::SINT8:
					cc = *(reinterpret_cast<const OW_Int8Array*>(&m_obj)) < *(reinterpret_cast<const OW_Int8Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::UINT16:
					cc = *(reinterpret_cast<const OW_UInt16Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_UInt16Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::SINT16:
					cc = *(reinterpret_cast<const OW_Int16Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_Int16Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::UINT32:
					cc = *(reinterpret_cast<const OW_UInt32Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_UInt32Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::SINT32:
					cc = *(reinterpret_cast<const OW_Int32Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_Int32Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::UINT64:
					cc = *(reinterpret_cast<const OW_UInt64Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_UInt64Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::SINT64:
					cc = *(reinterpret_cast<const OW_Int64Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_Int64Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::STRING:
					cc = *(reinterpret_cast<const OW_StringArray*>(&m_obj)) <
						*(reinterpret_cast<const OW_StringArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::REFERENCE:
					cc = *(reinterpret_cast<const OW_CIMObjectPathArray*>(&m_obj)) <
						*(reinterpret_cast<const OW_CIMObjectPathArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::BOOLEAN:
					cc = *(reinterpret_cast<const OW_BoolArray*>(&m_obj)) < *(reinterpret_cast<const OW_BoolArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::REAL32:
					cc = *(reinterpret_cast<const OW_Real32Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_Real32Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::REAL64:
					cc = *(reinterpret_cast<const OW_Real64Array*>(&m_obj)) <
						*(reinterpret_cast<const OW_Real64Array*>(&arg.m_obj));
					break;

				case OW_CIMDataType::DATETIME:
					cc = *(reinterpret_cast<const OW_CIMDateTimeArray*>(&m_obj)) <
						*(reinterpret_cast<const OW_CIMDateTimeArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::EMBEDDEDCLASS:
					cc = *(reinterpret_cast<const OW_CIMClassArray*>(&m_obj)) <
						*(reinterpret_cast<const OW_CIMClassArray*>(&arg.m_obj));
					break;

				case OW_CIMDataType::EMBEDDEDINSTANCE:
					cc = *(reinterpret_cast<const OW_CIMInstanceArray*>(&m_obj)) <
						*(reinterpret_cast<const OW_CIMInstanceArray*>(&arg.m_obj));
					break;
				default:
					OW_ASSERT(0);
			}
		}
		else
		{
			switch(m_type)
			{
				case OW_CIMDataType::UINT8:
					cc = m_obj.m_uint8Value < arg.m_obj.m_uint8Value;
					break;

				case OW_CIMDataType::SINT8:
					cc = m_obj.m_sint8Value < arg.m_obj.m_sint8Value;
					break;

				case OW_CIMDataType::UINT16:
					cc = m_obj.m_uint16Value < arg.m_obj.m_uint16Value;
					break;

				case OW_CIMDataType::SINT16:
					cc = m_obj.m_sint16Value < arg.m_obj.m_sint16Value;
					break;

				case OW_CIMDataType::UINT32:
					cc = m_obj.m_uint32Value < arg.m_obj.m_uint32Value;
					break;

				case OW_CIMDataType::SINT32:
					cc = m_obj.m_sint32Value < arg.m_obj.m_sint32Value;
					break;

				case OW_CIMDataType::UINT64:
					cc = m_obj.m_uint64Value < arg.m_obj.m_uint64Value;
					break;

				case OW_CIMDataType::SINT64:
					cc = m_obj.m_sint64Value < arg.m_obj.m_sint64Value;
					break;

				case OW_CIMDataType::CHAR16:
					cc = *(reinterpret_cast<const OW_Char16*>(&m_obj)) < *(reinterpret_cast<const OW_Char16*>(&arg.m_obj));
					break;

				case OW_CIMDataType::STRING:
					cc = *(reinterpret_cast<const OW_String*>(&m_obj)) < *(reinterpret_cast<const OW_String*>(&arg.m_obj));
					break;

				case OW_CIMDataType::BOOLEAN:
					cc = m_obj.m_booleanValue < arg.m_obj.m_booleanValue;
					break;

				case OW_CIMDataType::REAL32:
					cc = m_obj.m_real32Value < arg.m_obj.m_real32Value;
					break;

				case OW_CIMDataType::REAL64:
					cc = m_obj.m_real64Value < arg.m_obj.m_real64Value;
					break;

				case OW_CIMDataType::DATETIME:
					cc = *(reinterpret_cast<const OW_CIMDateTime*>(&m_obj)) <
						*(reinterpret_cast<const OW_CIMDateTime*>(&arg.m_obj));
					break;

				case OW_CIMDataType::REFERENCE:
					cc = *(reinterpret_cast<const OW_CIMObjectPath*>(&m_obj)) <
						*(reinterpret_cast<const OW_CIMObjectPath*>(&arg.m_obj));
					break;

				case OW_CIMDataType::EMBEDDEDCLASS:
					cc = *(reinterpret_cast<const OW_CIMClass*>(&m_obj)) <
						*(reinterpret_cast<const OW_CIMClass*>(&arg.m_obj));
					break;

				case OW_CIMDataType::EMBEDDEDINSTANCE:
					cc = *(reinterpret_cast<const OW_CIMInstance*>(&m_obj)) <
						*(reinterpret_cast<const OW_CIMInstance*>(&arg.m_obj));
					break;
				default:
					OW_ASSERT(0);
			}
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Bool& arg) const
{
	if(m_type != OW_CIMDataType::BOOLEAN || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a BOOLEAN");

	arg = (m_obj.m_booleanValue != 0);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Char16& arg) const
{
	if(m_type != OW_CIMDataType::CHAR16 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a CHAR16");

	arg = *(reinterpret_cast<const OW_Char16*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_UInt8& arg) const
{
	if(m_type != OW_CIMDataType::UINT8 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a UINT8");

	arg = m_obj.m_uint8Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Int8& arg) const
{
	if(m_type != OW_CIMDataType::SINT8 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a SINT8");

	arg = m_obj.m_sint8Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_UInt16& arg) const
{
	if(m_type != OW_CIMDataType::UINT16 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a UINT16");

	arg = m_obj.m_uint16Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Int16& arg) const
{
	if(m_type != OW_CIMDataType::SINT16 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a SINT16");

	arg = m_obj.m_sint16Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_UInt32& arg) const
{
	if(m_type != OW_CIMDataType::UINT32 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a UINT32");

	arg = m_obj.m_uint32Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Int32& arg) const
{
	if(m_type != OW_CIMDataType::SINT32 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a SINT32");

	arg = m_obj.m_sint32Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_UInt64& arg) const
{
	if(m_type != OW_CIMDataType::UINT64 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a UINT64");

	arg = m_obj.m_uint64Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Int64& arg) const
{
	if(m_type != OW_CIMDataType::SINT64 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a SINT64");

	arg = m_obj.m_sint64Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Real32& arg) const
{
	if(m_type != OW_CIMDataType::REAL32 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a REAL32");

	arg = m_obj.m_real32Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Real64& arg) const
{
	if(m_type != OW_CIMDataType::REAL64 || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a REAL64");

	arg = m_obj.m_real64Value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_String& arg) const
{
	if(m_type != OW_CIMDataType::STRING || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a STRING");

	arg = *(reinterpret_cast<const OW_String*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_CIMDateTime& arg) const
{
	if(m_type != OW_CIMDataType::DATETIME || isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a DATETIME");

	arg = *(reinterpret_cast<const OW_CIMDateTime*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_CIMObjectPath& arg) const
{
	if(m_type != OW_CIMDataType::REFERENCE)
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a REFERENCE");

	arg = *(reinterpret_cast<const OW_CIMObjectPath*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_CIMClass& arg) const
{
	if(m_type != OW_CIMDataType::EMBEDDEDCLASS)
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a EMBEDDEDCLASS");

	arg = *(reinterpret_cast<const OW_CIMClass*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_CIMInstance& arg) const
{
	if(m_type != OW_CIMDataType::EMBEDDEDINSTANCE)
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a EMBEDDEDINSTANCE");

	arg = *(reinterpret_cast<const OW_CIMInstance*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Char16Array& arg) const
{
	if(m_type != OW_CIMDataType::CHAR16 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a CHAR16 ARRAY");

	arg = *(reinterpret_cast<const OW_Char16Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_UInt8Array& arg) const
{
	if(m_type != OW_CIMDataType::UINT8 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a UINT8 ARRAY");

	arg = *(reinterpret_cast<const OW_UInt8Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Int8Array& arg) const
{
	if(m_type != OW_CIMDataType::SINT8 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a SINT8 ARRAY");

	arg = *(reinterpret_cast<const OW_Int8Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_UInt16Array& arg) const
{
	if(m_type != OW_CIMDataType::UINT16 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a UINT16 ARRAY");

	arg = *(reinterpret_cast<const OW_UInt16Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Int16Array& arg) const
{
	if(m_type != OW_CIMDataType::SINT16 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a SINT16 ARRAY");

	arg = *(reinterpret_cast<const OW_Int16Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_UInt32Array& arg) const
{
	if(m_type != OW_CIMDataType::UINT32 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a UINT32 ARRAY");

	arg = *(reinterpret_cast<const OW_UInt32Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Int32Array& arg) const
{
	if(m_type != OW_CIMDataType::SINT32 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a SINT32 ARRAY");

	arg = *(reinterpret_cast<const OW_Int32Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_UInt64Array& arg) const
{
	if(m_type != OW_CIMDataType::UINT64 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a UINT64 ARRAY");

	arg = *(reinterpret_cast<const OW_UInt64Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Int64Array& arg) const
{
	if(m_type != OW_CIMDataType::SINT64 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a SINT64 ARRAY");

	arg = *(reinterpret_cast<const OW_Int64Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Real64Array& arg) const
{
	if(m_type != OW_CIMDataType::REAL64 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a REAL64 ARRAY");

	arg = *(reinterpret_cast<const OW_Real64Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_Real32Array& arg) const
{
	if(m_type != OW_CIMDataType::REAL32 || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a REAL32 ARRAY");

	arg = *(reinterpret_cast<const OW_Real32Array*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_StringArray& arg) const
{
	if(m_type != OW_CIMDataType::STRING || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a STRING ARRAY");

	arg = *(reinterpret_cast<const OW_StringArray*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_BoolArray& arg) const
{
	if(m_type != OW_CIMDataType::BOOLEAN || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a BOOLEAN ARRAY");

	arg = *(reinterpret_cast<const OW_BoolArray*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_CIMDateTimeArray& arg) const
{
	if(m_type != OW_CIMDataType::DATETIME || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a DATETIME ARRAY");

	arg = *(reinterpret_cast<const OW_CIMDateTimeArray*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_CIMObjectPathArray& arg) const
{
	if(m_type != OW_CIMDataType::REFERENCE || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a OBJECTPATH ARRAY");

	arg = *(reinterpret_cast<const OW_CIMObjectPathArray*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_CIMClassArray& arg) const
{
	if(m_type != OW_CIMDataType::EMBEDDEDCLASS || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a EMBEDDEDCLASS ARRAY");

	arg = *(reinterpret_cast<const OW_CIMClassArray*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::get(OW_CIMInstanceArray& arg) const
{
	if(m_type != OW_CIMDataType::EMBEDDEDINSTANCE || !isArray())
		OW_THROW(OW_ValueCastException,
			"OW_CIMValue::OW_CIMValueImpl::get - Value is not a EMBEDDEDINSTANCE ARRAY");

	arg = *(reinterpret_cast<const OW_CIMInstanceArray*>(&m_obj));
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_String raToString(const T& ra, OW_Bool forMOF=false)
{
	OW_StringBuffer out;

	for(size_t i = 0; i < ra.size(); i++)
	{
		if(i > 0)
			out += ',';

		if (forMOF)
		{
			out += '"';
			out += ra[i];
			out += '"';
		}
		else
		{
			out += ra[i];
		}
	}

	return out.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
static OW_String
raToString(const OW_Array<OW_String>& ra, OW_Bool forMOF=false)
{
	OW_StringBuffer out;

	for(size_t i = 0; i < ra.size(); i++)
	{
		if(i > 0)
			out += ',';

		if (forMOF)
		{
			out += '"';
			out += OW_CIMObjectPath::escape(ra[i]);
			out += '"';
		}
		else
		{
			out += ra[i];
		}
	}

	return out.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
static OW_String
raToString(const OW_Array<OW_CIMClass>& ra, OW_Bool forMOF=false)
{
	OW_StringBuffer out;

	for(size_t i = 0; i < ra.size(); i++)
	{
		if(i > 0)
			out += ',';

		if (forMOF)
		{
			out += '"';
			out += OW_CIMObjectPath::escape(ra[i].toString());
			out += '"';
		}
		else
		{
			out += ra[i].toString();
		}
	}

	return out.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
static OW_String
raToString(const OW_Array<OW_CIMInstance>& ra, OW_Bool forMOF=false)
{
	OW_StringBuffer out;

	for(size_t i = 0; i < ra.size(); i++)
	{
		if(i > 0)
			out += ',';

		if (forMOF)
		{
			out += '"';
			out += OW_CIMObjectPath::escape(ra[i].toString());
			out += '"';
		}
		else
		{
			out += ra[i].toString();
		}
	}

	return out.releaseString();
}

static OW_String
raToString(const OW_Array<OW_Bool>& ra, OW_Bool isString=false)
{
	OW_StringBuffer out;

	for(size_t i = 0; i < ra.size(); i++)
	{
		if(i > 0)
			out += ',';

		if(isString)
			out += '"';

		out += ra[i].toString();

		if(isString)
			out += '"';
	}

	return out.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMValue::OW_CIMValueImpl::toString(OW_Bool forMOF) const
{
	OW_String out;

	if(m_isArray)
	{
		switch(m_type)
		{
			case OW_CIMDataType::BOOLEAN:
				out = raToString(*(reinterpret_cast<const OW_BoolArray*>(&m_obj)));
				break;

			case OW_CIMDataType::UINT8:
				out = raToString(*(reinterpret_cast<const OW_UInt8Array*>(&m_obj)));
				break;

			case OW_CIMDataType::SINT8:
				out = raToString(*(reinterpret_cast<const OW_Int8Array*>(&m_obj)));
				break;

			// ATTN: UTF8
			case OW_CIMDataType::CHAR16:
				out = raToString(*(reinterpret_cast<const OW_Char16Array*>(&m_obj)));
				break;

			case OW_CIMDataType::UINT16:
				out = raToString(*(reinterpret_cast<const OW_UInt16Array*>(&m_obj)));
				break;

			case OW_CIMDataType::SINT16:
				out = raToString(*(reinterpret_cast<const OW_Int16Array*>(&m_obj)));
				break;

			case OW_CIMDataType::UINT32:
				out = raToString(*(reinterpret_cast<const OW_UInt32Array*>(&m_obj)));
				break;

			case OW_CIMDataType::SINT32:
				out = raToString(*(reinterpret_cast<const OW_Int32Array*>(&m_obj)));
				break;

			case OW_CIMDataType::UINT64:
				out = raToString(*(reinterpret_cast<const OW_UInt64Array*>(&m_obj)));
				break;

			case OW_CIMDataType::SINT64:
				out = raToString(*(reinterpret_cast<const OW_Int64Array*>(&m_obj)));
				break;

			case OW_CIMDataType::REAL32:
				out = raToString(*(reinterpret_cast<const OW_Real32Array*>(&m_obj)));
				break;

			case OW_CIMDataType::REAL64:
				out = raToString(*(reinterpret_cast<const OW_Real64Array*>(&m_obj)));
				break;

			case OW_CIMDataType::STRING:
				out = raToString(*(reinterpret_cast<const OW_StringArray*>(&m_obj)), forMOF);
				break;

			case OW_CIMDataType::DATETIME:
				out = raToString(*(reinterpret_cast<const OW_CIMDateTimeArray*>(&m_obj)));
				break;

			case OW_CIMDataType::REFERENCE:
				out = raToString(*(reinterpret_cast<const OW_CIMObjectPathArray*>(&m_obj)));
				break;
			
			case OW_CIMDataType::EMBEDDEDCLASS:
				out = raToString(*(reinterpret_cast<const OW_CIMClassArray*>(&m_obj)));
				break;
			
			case OW_CIMDataType::EMBEDDEDINSTANCE:
				out = raToString(*(reinterpret_cast<const OW_CIMInstanceArray*>(&m_obj)));
				break;
			default:
				OW_ASSERT(0);
		}
	}
	else
	{
		switch(m_type)
		{
			case OW_CIMDataType::BOOLEAN:
				out = OW_Bool(m_obj.m_booleanValue != 0).toString();
				break;

			case OW_CIMDataType::UINT8:
				out = OW_String(static_cast<OW_UInt32>(m_obj.m_uint8Value));
				break;

			case OW_CIMDataType::SINT8:
				out = OW_String(static_cast<OW_Int32>(m_obj.m_sint8Value));
				break;

			case OW_CIMDataType::CHAR16:
				out = OW_String(static_cast<char>(reinterpret_cast<const OW_Char16*>(&m_obj)->getValue()));
				break;

			case OW_CIMDataType::UINT16:
				out = OW_String(static_cast<OW_UInt32>(m_obj.m_uint16Value));
				break;

			case OW_CIMDataType::SINT16:
				out = OW_String(static_cast<OW_Int32>(m_obj.m_sint16Value));
				break;

			case OW_CIMDataType::UINT32:
				out = OW_String(m_obj.m_uint32Value);
				break;

			case OW_CIMDataType::SINT32:
				out = OW_String(m_obj.m_sint32Value);
				break;

			case OW_CIMDataType::UINT64:
				out = OW_String(m_obj.m_uint64Value);
				break;

			case OW_CIMDataType::SINT64:
				out = OW_String(m_obj.m_sint64Value);
				break;

			case OW_CIMDataType::REAL32:
				out = OW_String(static_cast<OW_Real64>(m_obj.m_real32Value));
				break;

			case OW_CIMDataType::REAL64:
				out = OW_String(m_obj.m_real64Value);
				break;

			case OW_CIMDataType::STRING:
				if(forMOF)
				{
					out = "\"";
					out += OW_CIMObjectPath::escape(*(reinterpret_cast<const OW_String*>(&m_obj)));
					out += "\"";
				}
				else
				{
					out = *(reinterpret_cast<const OW_String*>(&m_obj));
				}
				break;

			case OW_CIMDataType::DATETIME:
				out = (reinterpret_cast<const OW_CIMDateTime*>(&m_obj))->toString();
				break;

			case OW_CIMDataType::REFERENCE:
				if (forMOF)
				{
					out = OW_CIMObjectPath::escape((reinterpret_cast<const OW_CIMObjectPath*>(&m_obj))->toString());
				}
				else
				{
					out = (reinterpret_cast<const OW_CIMObjectPath*>(&m_obj))->toString();
				}
				break;
			
			case OW_CIMDataType::EMBEDDEDCLASS:
				if(forMOF)
				{
					out = "\"";
					out += OW_CIMObjectPath::escape((reinterpret_cast<const OW_CIMClass*>(&m_obj))->toString());
					out += "\"";
				}
				else
				{
					out = (reinterpret_cast<const OW_CIMClass*>(&m_obj))->toString();
				}
				break;

			case OW_CIMDataType::EMBEDDEDINSTANCE:
				if(forMOF)
				{
					out = "\"";
					out += OW_CIMObjectPath::escape((reinterpret_cast<const OW_CIMInstance*>(&m_obj))->toString());
					out += "\"";
				}
				else
				{
					out = (reinterpret_cast<const OW_CIMInstance*>(&m_obj))->toString();
				}
				break;
			default:
				OW_ASSERT(0);

		}
	}

	return out;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMValue::OW_CIMValueImpl::toMOF() const
{
	OW_StringBuffer rv;

	if(m_type == OW_CIMDataType::CIMNULL
		|| m_type == OW_CIMDataType::INVALID)
	{
		return OW_String();
	}

	if(m_isArray)
		rv += '{';

	rv += toString(true);

	if(m_isArray)
		rv += '}';

	return rv.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
// convType:	0 = no conversion
//					1 = 16 bit
//					2 = 32 bit
//					3 = 64 bit
template<class T>
void
readValue(istream& istrm, T& val, int convType)
{
	OW_BinarySerialization::read(istrm, &val, sizeof(val));

	switch(convType)
	{
		case 3: { val = static_cast<T>(OW_ntoh64(val)); break; }
		case 2: { val = static_cast<T>(OW_ntoh32(val)); break; }
		case 1: { val = static_cast<T>(OW_ntoh16(val)); break; }
	}
}

//////////////////////////////////////////////////////////////////////////////
static OW_Real64
readReal64Value(istream& istrm)
{
	OW_String rstr;
	rstr.readObject(istrm);
	return rstr.toReal64();
}

//////////////////////////////////////////////////////////////////////////////
static OW_Real32
readReal32Value(istream& istrm)
{
	OW_String rstr;
	rstr.readObject(istrm);
	return rstr.toReal32();
}

//////////////////////////////////////////////////////////////////////////////
// convType:	0 = no conversion
//					1 = 16 bit
//					2 = 32 bit
//					3 = 64 bit
template<class T>
void
readArray(istream& istrm, T& ra, int convType)
{
	ra.clear();
	OW_UInt32 sz;

	OW_BinarySerialization::readLen(istrm, sz);
	for(OW_UInt32 i = 0; i < sz; i++)
	{
		typename T::value_type v;

		OW_BinarySerialization::read(istrm, &v, sizeof(v));

		switch(convType)
		{
			case 3: v = static_cast<typename T::value_type>(OW_ntoh64(v)); break;
			case 2: v = static_cast<typename T::value_type>(OW_ntoh32(v)); break;
			case 1: v = static_cast<typename T::value_type>(OW_ntoh16(v)); break;
		}

		ra.push_back(v);
	}
}

static void
readReal32Array(istream& istrm, OW_Array<OW_Real32>& ra)
{
	ra.clear();
	OW_UInt32 sz;

	OW_BinarySerialization::readLen(istrm, sz);
	for(OW_UInt32 i = 0; i < sz; i++)
	{
		OW_Real32 v = readReal32Value(istrm);
		ra.push_back(v);
	}
}

static void
readReal64Array(istream& istrm, OW_Array<OW_Real64>& ra)
{
	ra.clear();
	OW_UInt32 sz;

	OW_BinarySerialization::readLen(istrm, sz);
	for(OW_UInt32 i = 0; i < sz; i++)
	{
		OW_Real64 v = readReal64Value(istrm);
		ra.push_back(v);
	}
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
void
readObjectArray(istream& istrm, T& ra)
{
	OW_BinarySerialization::readArray(istrm, ra);
//	ra.clear();
//	OW_Int32 sz;

//	readValue(istrm, sz, 2);
//	for(OW_Int32 i = 0; i < sz; i++)
//	{
//		typename T::value_type v;
//		v.readObject(istrm);
//		ra.push_back(v);
//	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::readObject(istream &istrm)
{
	OW_CIMBase::readSig( istrm, OW_CIMVALUESIG );
	destroyObject();
	m_objDestroyed = false;
	OW_UInt32 tmp;
	OW_BinarySerialization::readLen(istrm, tmp);
	m_type = OW_CIMDataType::Type(tmp);
	m_isArray.readObject(istrm);

	if(m_isArray)
	{
		switch(m_type)
		{
			case OW_CIMDataType::BOOLEAN:
				new(&m_obj) OW_BoolArray;
				readArray(istrm, *(reinterpret_cast<OW_BoolArray*>(&m_obj)), 0);
				break;

			case OW_CIMDataType::UINT8:
				new(&m_obj) OW_UInt8Array;
				readArray(istrm, *(reinterpret_cast<OW_UInt8Array*>(&m_obj)), 0);
				break;

			case OW_CIMDataType::SINT8:
				new(&m_obj) OW_Int8Array;
				readArray(istrm, *(reinterpret_cast<OW_Int8Array*>(&m_obj)), 0);
				break;

			case OW_CIMDataType::CHAR16:
				new(&m_obj) OW_Char16Array;
				readObjectArray(istrm, *(reinterpret_cast<OW_Char16Array*>(&m_obj)));
				break;

			case OW_CIMDataType::UINT16:
				new(&m_obj) OW_UInt16Array;
				readArray(istrm, *(reinterpret_cast<OW_UInt16Array*>(&m_obj)), 1);
				break;

			case OW_CIMDataType::SINT16:
				new(&m_obj) OW_Int16Array;
				readArray(istrm, *(reinterpret_cast<OW_Int16Array*>(&m_obj)), 1);
				break;

			case OW_CIMDataType::UINT32:
				new(&m_obj) OW_UInt32Array;
				readArray(istrm, *(reinterpret_cast<OW_UInt32Array*>(&m_obj)), 2);
				break;

			case OW_CIMDataType::SINT32:
				new(&m_obj) OW_Int32Array;
				readArray(istrm, *(reinterpret_cast<OW_Int32Array*>(&m_obj)), 2);
				break;

			case OW_CIMDataType::UINT64:
				new(&m_obj) OW_UInt64Array;
				readArray(istrm, *(reinterpret_cast<OW_UInt64Array*>(&m_obj)), 3);
				break;

			case OW_CIMDataType::SINT64:
				new(&m_obj) OW_Int64Array;
				readArray(istrm, *(reinterpret_cast<OW_Int64Array*>(&m_obj)), 3);
				break;

			case OW_CIMDataType::REAL32:
				new(&m_obj) OW_Real32Array;
				readReal32Array(istrm, *(reinterpret_cast<OW_Real32Array*>(&m_obj)));
				break;

			case OW_CIMDataType::REAL64:
				new(&m_obj) OW_Real64Array;
				readReal64Array(istrm, *(reinterpret_cast<OW_Real64Array*>(&m_obj)));
				break;

			case OW_CIMDataType::STRING:
				new(&m_obj) OW_StringArray;
				readObjectArray(istrm, *(reinterpret_cast<OW_StringArray*>(&m_obj)));
				break;

			case OW_CIMDataType::DATETIME:
				new(&m_obj) OW_CIMDateTimeArray;
				readObjectArray(istrm, *(reinterpret_cast<OW_CIMDateTimeArray*>(&m_obj)));
				break;

			case OW_CIMDataType::REFERENCE:
				new(&m_obj) OW_CIMObjectPathArray;
				readObjectArray(istrm, *(reinterpret_cast<OW_CIMObjectPathArray*>(&m_obj)));
				break;

			case OW_CIMDataType::EMBEDDEDCLASS:
				new(&m_obj) OW_CIMClassArray;
				readObjectArray(istrm, *(reinterpret_cast<OW_CIMClassArray*>(&m_obj)));
				break;

			case OW_CIMDataType::EMBEDDEDINSTANCE:
				new(&m_obj) OW_CIMInstanceArray;
				readObjectArray(istrm, *(reinterpret_cast<OW_CIMInstanceArray*>(&m_obj)));
				break;
			default:
				OW_ASSERT(0);
		}
	}
	else
	{
		switch(m_type)
		{
			case OW_CIMDataType::BOOLEAN:
				readValue(istrm, m_obj.m_booleanValue, 0);
				break;

			case OW_CIMDataType::UINT8:
				readValue(istrm, m_obj.m_uint8Value, 0);
				break;

			case OW_CIMDataType::SINT8:
				readValue(istrm, m_obj.m_sint8Value, 0);
				break;

			case OW_CIMDataType::UINT16:
				readValue(istrm, m_obj.m_uint16Value, 1);
				break;

			case OW_CIMDataType::SINT16:
				readValue(istrm, m_obj.m_sint16Value, 1);
				break;

			case OW_CIMDataType::UINT32:
				readValue(istrm, m_obj.m_uint32Value, 2);
				break;

			case OW_CIMDataType::SINT32:
				readValue(istrm, m_obj.m_sint32Value, 2);
				break;

			case OW_CIMDataType::UINT64:
				readValue(istrm, m_obj.m_uint64Value, 3);
				break;

			case OW_CIMDataType::SINT64:
				readValue(istrm, m_obj.m_sint64Value, 3);
				break;

			case OW_CIMDataType::REAL32:
				m_obj.m_real32Value = readReal32Value(istrm);
				break;

			case OW_CIMDataType::REAL64:
				m_obj.m_real64Value = readReal64Value(istrm);
				break;

			case OW_CIMDataType::CHAR16:
				new(&m_obj) OW_Char16;
				(reinterpret_cast<OW_Char16*>(&m_obj))->readObject(istrm);
				break;

			case OW_CIMDataType::STRING:
				new(&m_obj) OW_String;
				(reinterpret_cast<OW_String*>(&m_obj))->readObject(istrm);
				break;

			case OW_CIMDataType::DATETIME:
				new(&m_obj) OW_CIMDateTime(OW_CIMNULL);
				(reinterpret_cast<OW_CIMDateTime*>(&m_obj))->readObject(istrm);
				break;

			case OW_CIMDataType::REFERENCE:
				new(&m_obj) OW_CIMObjectPath(OW_CIMNULL);
				(reinterpret_cast<OW_CIMObjectPath*>(&m_obj))->readObject(istrm);
				break;

			case OW_CIMDataType::EMBEDDEDCLASS:
				new(&m_obj) OW_CIMClass(OW_CIMNULL);
				(reinterpret_cast<OW_CIMClass*>(&m_obj))->readObject(istrm);
				break;

			case OW_CIMDataType::EMBEDDEDINSTANCE:
				new(&m_obj) OW_CIMInstance(OW_CIMNULL);
				(reinterpret_cast<OW_CIMInstance*>(&m_obj))->readObject(istrm);
				break;
			default:
				OW_ASSERT(0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// convType:	0 = no conversion
//					1 = 16 bit
//					2 = 32 bit
//					3 = 64 bit
template<class T>
void
writeValue(ostream& ostrm, T val, int convType)
{
	T v;
	switch(convType)
	{
		case 3: v = static_cast<T>(OW_hton64(val)); break;
		case 2: v = static_cast<T>(OW_hton32(val)); break;
		case 1: v = static_cast<T>(OW_hton16(val)); break;
		default: v = val; break;
	}

	OW_BinarySerialization::write(ostrm, &v, sizeof(v));
}

static void
writeRealValue(ostream& ostrm, OW_Real64 rv)
{
	OW_String v(rv);
	v.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
// convType:	0 = no conversion
//					1 = 16 bit
//					2 = 32 bit
//					3 = 64 bit
template<class T>
void
writeArray(ostream& ostrm, const T& ra, int convType)
{
	OW_UInt32 sz = ra.size();
	OW_BinarySerialization::writeLen(ostrm, sz);

	for(OW_UInt32 i = 0; i < sz; i++)
	{
		typename T::value_type v;
		switch(convType)
		{
			case 3: v = static_cast<typename T::value_type>(OW_hton64(ra[i])); break;
			case 2: v = static_cast<typename T::value_type>(OW_hton32(ra[i])); break;
			case 1: v = static_cast<typename T::value_type>(OW_hton16(ra[i])); break;
			default: v = static_cast<typename T::value_type>(ra[i]); break;
		}

		OW_BinarySerialization::write(ostrm, &v, sizeof(v));
	}
}

static void
writeArray(ostream& ostrm, const OW_Array<OW_Real32>& ra)
{
	OW_UInt32 sz = ra.size();
	OW_BinarySerialization::writeLen(ostrm, sz);

	for(OW_UInt32 i = 0; i < sz; i++)
	{
		writeRealValue(ostrm, static_cast<OW_Real64>(ra[i]));
	}
}

static void
writeArray(ostream& ostrm, const OW_Array<OW_Real64>& ra)
{
	OW_UInt32 sz = ra.size();
	OW_BinarySerialization::writeLen(ostrm, sz);

	for(OW_UInt32 i = 0; i < sz; i++)
	{
		writeRealValue(ostrm, ra[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
void
writeObjectArray(ostream& ostrm, const T& ra)
{
	OW_BinarySerialization::writeArray(ostrm, ra);
//	OW_Int32 sz = ra.size();
//	writeValue(ostrm, sz, 2);

//	for(OW_Int32 i = 0; i < sz; i++)
//	{
//		ra[i].writeObject(ostrm);
//	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMValue::OW_CIMValueImpl::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig(ostrm, OW_CIMVALUESIG);
	OW_BinarySerialization::writeLen(ostrm, m_type);
	m_isArray.writeObject(ostrm);

	if(m_isArray)
	{
		switch(m_type)
		{
			case OW_CIMDataType::BOOLEAN:
				writeArray(ostrm, *(reinterpret_cast<const OW_BoolArray*>(&m_obj)), 0);
				break;

			case OW_CIMDataType::UINT8:
				writeArray(ostrm, *(reinterpret_cast<const OW_UInt8Array*>(&m_obj)), 0);
				break;

			case OW_CIMDataType::SINT8:
				writeArray(ostrm, *(reinterpret_cast<const OW_Int8Array*>(&m_obj)), 0);
				break;

			case OW_CIMDataType::UINT16:
				writeArray(ostrm, *(reinterpret_cast<const OW_UInt16Array*>(&m_obj)), 1);
				break;

			case OW_CIMDataType::SINT16:
				writeArray(ostrm, *(reinterpret_cast<const OW_Int16Array*>(&m_obj)), 1);
				break;

			case OW_CIMDataType::UINT32:
				writeArray(ostrm, *(reinterpret_cast<const OW_UInt32Array*>(&m_obj)), 2);
				break;

			case OW_CIMDataType::SINT32:
				writeArray(ostrm, *(reinterpret_cast<const OW_Int32Array*>(&m_obj)), 2);
				break;

			case OW_CIMDataType::UINT64:
				writeArray(ostrm, *(reinterpret_cast<const OW_UInt64Array*>(&m_obj)), 3);
				break;

			case OW_CIMDataType::SINT64:
				writeArray(ostrm, *(reinterpret_cast<const OW_Int64Array*>(&m_obj)), 3);
				break;

			case OW_CIMDataType::REAL32:
				writeArray(ostrm, *(reinterpret_cast<const OW_Real32Array*>(&m_obj)));
				break;

			case OW_CIMDataType::REAL64:
				writeArray(ostrm, *(reinterpret_cast<const OW_Real64Array*>(&m_obj)));
				break;

			case OW_CIMDataType::CHAR16:
				writeObjectArray(ostrm, *(reinterpret_cast<const OW_Char16Array*>(&m_obj)));
				break;

			case OW_CIMDataType::STRING:
				writeObjectArray(ostrm, *(reinterpret_cast<const OW_StringArray*>(&m_obj)));
				break;

			case OW_CIMDataType::DATETIME:
				writeObjectArray(ostrm, *(reinterpret_cast<const OW_CIMDateTimeArray*>(&m_obj)));
				break;

			case OW_CIMDataType::REFERENCE:
				writeObjectArray(ostrm, *(reinterpret_cast<const OW_CIMObjectPathArray*>(&m_obj)));
				break;

			case OW_CIMDataType::EMBEDDEDCLASS:
				writeObjectArray(ostrm, *(reinterpret_cast<const OW_CIMClassArray*>(&m_obj)));
				break;

			case OW_CIMDataType::EMBEDDEDINSTANCE:
				writeObjectArray(ostrm, *(reinterpret_cast<const OW_CIMInstanceArray*>(&m_obj)));
				break;
			default:
				OW_ASSERT(0);
		}
	}
	else
	{
		switch(m_type)
		{
			case OW_CIMDataType::BOOLEAN:
				writeValue(ostrm, m_obj.m_booleanValue, 0);
				break;

			case OW_CIMDataType::UINT8:
				writeValue(ostrm, m_obj.m_uint8Value, 0);
				break;

			case OW_CIMDataType::SINT8:
				writeValue(ostrm, m_obj.m_sint8Value, 0);
				break;


			case OW_CIMDataType::UINT16:
				writeValue(ostrm, m_obj.m_uint16Value, 1);
				break;

			case OW_CIMDataType::SINT16:
				writeValue(ostrm, m_obj.m_sint16Value, 1);
				break;

			case OW_CIMDataType::UINT32:
				writeValue(ostrm, m_obj.m_uint32Value, 2);
				break;

			case OW_CIMDataType::SINT32:
				writeValue(ostrm, m_obj.m_sint32Value, 2);
				break;

			case OW_CIMDataType::UINT64:
				writeValue(ostrm, m_obj.m_uint64Value, 3);
				break;

			case OW_CIMDataType::SINT64:
				writeValue(ostrm, m_obj.m_sint64Value, 3);
				break;

			case OW_CIMDataType::REAL32:
				writeRealValue(ostrm, static_cast<OW_Real64>(m_obj.m_real32Value));
				break;

			case OW_CIMDataType::REAL64:
				writeRealValue(ostrm, m_obj.m_real64Value);
				break;

			case OW_CIMDataType::CHAR16:
				(reinterpret_cast<const OW_Char16*>(&m_obj))->writeObject(ostrm);
				break;

			case OW_CIMDataType::STRING:
				(reinterpret_cast<const OW_String*>(&m_obj))->writeObject(ostrm);
				break;

			case OW_CIMDataType::DATETIME:
				(reinterpret_cast<const OW_CIMDateTime*>(&m_obj))->writeObject(ostrm);
				break;

			case OW_CIMDataType::REFERENCE:
				(reinterpret_cast<const OW_CIMObjectPath*>(&m_obj))->writeObject(ostrm);
				break;

			case OW_CIMDataType::EMBEDDEDCLASS:
				(reinterpret_cast<const OW_CIMClass*>(&m_obj))->writeObject(ostrm);
				break;

			case OW_CIMDataType::EMBEDDEDINSTANCE:
				(reinterpret_cast<const OW_CIMInstance*>(&m_obj))->writeObject(ostrm);
				break;
			default:
				OW_ASSERT(0);
		}
	}
}

OW_CIMValue::~OW_CIMValue()
{
}




