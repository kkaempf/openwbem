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

namespace OpenWBEM
{

using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////
class CIMValue::CIMValueImpl
{
public:
	static CIMValueImpl createSimpleValue(const String& cimtype,
		const String& value);
	CIMValueImpl();
	CIMValueImpl(const CIMValueImpl& arg);
	explicit CIMValueImpl(Bool val);
	explicit CIMValueImpl(bool val);
	explicit CIMValueImpl(UInt8 arg);
	explicit CIMValueImpl(Int8 arg);
	explicit CIMValueImpl(UInt16 arg);
	explicit CIMValueImpl(Int16 arg);
	explicit CIMValueImpl(UInt32 arg);
	explicit CIMValueImpl(Int32 arg);
	explicit CIMValueImpl(UInt64 arg);
	explicit CIMValueImpl(Int64 arg);
	explicit CIMValueImpl(Real32 arg);
	explicit CIMValueImpl(Real64 arg);
	explicit CIMValueImpl(const Char16& arg);
	explicit CIMValueImpl(const String& arg);
	explicit CIMValueImpl(const CIMDateTime& arg);
	explicit CIMValueImpl(const CIMObjectPath& arg);
	explicit CIMValueImpl(const CIMClass& arg);
	explicit CIMValueImpl(const CIMInstance& arg);
	explicit CIMValueImpl(const BoolArray& arg);
	explicit CIMValueImpl(const Char16Array& arg);
	explicit CIMValueImpl(const UInt8Array& arg);
	explicit CIMValueImpl(const Int8Array& arg);
	explicit CIMValueImpl(const UInt16Array& arg);
	explicit CIMValueImpl(const Int16Array& arg);
	explicit CIMValueImpl(const UInt32Array& arg);
	explicit CIMValueImpl(const Int32Array& arg);
	explicit CIMValueImpl(const UInt64Array& arg);
	explicit CIMValueImpl(const Int64Array& arg);
	explicit CIMValueImpl(const Real64Array& arg);
	explicit CIMValueImpl(const Real32Array& arg);
	explicit CIMValueImpl(const StringArray& arg);
	explicit CIMValueImpl(const CIMDateTimeArray& arg);
	explicit CIMValueImpl(const CIMObjectPathArray& arg);
	explicit CIMValueImpl(const CIMClassArray& arg);
	explicit CIMValueImpl(const CIMInstanceArray& arg);
	~CIMValueImpl();
	CIMValueImpl* clone() { return new CIMValueImpl(*this); }
	void get(Bool& val) const;
	void get(Char16& arg) const;
	void get(UInt8& arg) const;
	void get(Int8& arg) const;
	void get(UInt16& arg) const;
	void get(Int16& arg) const;
	void get(UInt32& arg) const;
	void get(Int32& arg) const;
	void get(UInt64& arg) const;
	void get(Int64& arg) const;
	void get(Real32& arg) const;
	void get(Real64& arg) const;
	void get(String& arg) const;
	void get(CIMDateTime& arg) const;
	void get(CIMObjectPath& arg) const;
	void get(CIMClass& arg) const;
	void get(CIMInstance& arg) const;
	void get(Char16Array& arg) const;
	void get(UInt8Array& arg) const;
	void get(Int8Array& arg) const;
	void get(UInt16Array& arg) const;
	void get(Int16Array& arg) const;
	void get(UInt32Array& arg) const;
	void get(Int32Array& arg) const;
	void get(UInt64Array& arg) const;
	void get(Int64Array& arg) const;
	void get(Real64Array& arg) const;
	void get(Real32Array& arg) const;
	void get(StringArray& arg) const;
	void get(BoolArray& arg) const;
	void get(CIMDateTimeArray& arg) const;
	void get(CIMObjectPathArray& arg) const;
	void get(CIMClassArray& arg) const;
	void get(CIMInstanceArray& arg) const;
	UInt32 getArraySize() const;
	CIMValueImpl& operator= (const CIMValueImpl& arg);
	CIMValueImpl& set(const CIMValueImpl& arg);
	bool equal(const CIMValueImpl& arg) const;
	bool operator== (const CIMValueImpl& arg) const
	{
		return equal(arg);
	}
	bool operator!= (const CIMValueImpl& arg) const
	{
		return !(*this == arg);
	}
	bool operator<= (const CIMValueImpl& arg) const
	{
		return !(arg < *this);
	}
	bool operator>= (const CIMValueImpl& arg) const
	{
		return !(*this < arg);
	}
	bool operator< (const CIMValueImpl& arg) const;
	bool operator> (const CIMValueImpl& arg) const
	{
		return arg < *this;
	}
	CIMDataType::Type getType() const
	{
		return m_type;
	}
	CIMDataType getCIMDataType() const
	{
		CIMDataType rval = CIMDataType(getType());
		if (m_isArray)
		{
			rval.setToArrayType(-1);
		}
		return rval;
	}
	bool sameType(const CIMValueImpl& arg) const
	{
		return(m_type == arg.m_type && m_isArray == arg.m_isArray);
	}
	bool isArray() const
	{
		return m_isArray;
	}
	void readObject(std::istream &istrm);
	void writeObject(std::ostream &ostrm) const;
	String toString(bool forMOF=false) const;
	String toMOF() const;
private:
	union CIMValueData
	{
		UInt8	m_booleanValue;
		UInt8 	m_uint8Value;
		Int8 		m_sint8Value;
		UInt16 	m_uint16Value;
		Int16 	m_sint16Value;
		UInt32 	m_uint32Value;
		Int32 	m_sint32Value;
		UInt64 	m_uint64Value;
		Int64 	m_sint64Value;
		Real32 	m_real32Value;
		Real64 	m_real64Value;
		char bfr1[sizeof(BoolArray)];
		char bfr2[sizeof(UInt8Array)];
		char bfr3[sizeof(Int8Array)];
		char bfr4[sizeof(Char16Array)];
		char bfr5[sizeof(UInt16Array)];
		char bfr6[sizeof(Int16Array)];
		char bfr7[sizeof(UInt32Array)];
		char bfr8[sizeof(Int32Array)];
		char bfr9[sizeof(UInt64Array)];
		char bfr10[sizeof(Int64Array)];
		char bfr11[sizeof(Real64Array)];
		char bfr12[sizeof(Real32Array)];
		char bfr13[sizeof(StringArray)];
		char bfr14[sizeof(CIMObjectPathArray)];
		char bfr15[sizeof(CIMDateTimeArray)];
		char bfr16[sizeof(CIMObjectPath)];
		char bfr17[sizeof(Char16)];
		char bfr18[sizeof(CIMDateTime)];
		char bfr19[sizeof(String)];
		char bfr20[sizeof(CIMClass)];
		char bfr21[sizeof(CIMInstance)];
		char bfr22[sizeof(CIMClassArray)];
		char bfr23[sizeof(CIMInstanceArray)];
	};
	void setupObject(const CIMValueData& odata, CIMDataType::Type type, bool isArray);
	void destroyObject();
	CIMDataType::Type m_type;
	Bool m_isArray;
	Bool m_objDestroyed;
	CIMValueData m_obj;
};
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMValue
CIMValue::createSimpleValue(const String& cimtype,
	const String& value)
{
	int type = CIMDataType::strToSimpleType(cimtype);
	if(type == CIMDataType::INVALID)
	{
		return CIMValue(CIMNULL);
	}
	CIMValueImpl impl = CIMValueImpl::createSimpleValue(cimtype, value);
	CIMValue cv(CIMNULL);
	cv.m_impl = new CIMValueImpl(impl);
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
void				
CIMValue::readObject(istream &istrm)
{
	if(m_impl.isNull())
	{
		m_impl = new CIMValueImpl;
	}
	m_impl->readObject(istrm);
}
#ifdef OW_NEW
#undef new
#endif
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(CIMNULL_t)
	: CIMBase(), m_impl(0) { }
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMValue& x)
	: CIMBase(), m_impl(x.m_impl) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Bool x)
	: CIMBase(), m_impl(new CIMValueImpl(x)){}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(bool x)
	: CIMBase(), m_impl(new CIMValueImpl(x)){}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(UInt8 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Int8 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(UInt16 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Int16 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(UInt32 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Int32 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(UInt64 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Int64 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Real32 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Real64 x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Char16& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const String& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const char* x)
	: CIMBase(), m_impl(new CIMValueImpl(String(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMDateTime& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMObjectPath& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMClass& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMInstance& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMObjectPathArray& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const BoolArray& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Char16Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const UInt8Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Int8Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const UInt16Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Int16Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const UInt32Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Int32Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const UInt64Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Int64Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Real64Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Real32Array& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const StringArray& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMDateTimeArray& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMClassArray& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMInstanceArray& x)
	: CIMBase(), m_impl(new CIMValueImpl(x)) {}
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMValue::getArraySize() const
{
	return m_impl->getArraySize();
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Bool& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Char16& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt8& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int8& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt16& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int16& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt32& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int32& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt64& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int64& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Real32& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Real64& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(String& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMDateTime& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMObjectPath& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMClass& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMInstance& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Char16Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt8Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int8Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt16Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int16Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt32Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int32Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt64Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int64Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Real64Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Real32Array& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(StringArray& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMObjectPathArray& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(BoolArray& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMDateTimeArray& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMClassArray& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMInstanceArray& x) const
{
	m_impl->get(x);
}
//////////////////////////////////////////////////////////////////////////////
Bool CIMValue::toBool() const
{
	Bool rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Char16 CIMValue::toChar16() const
{
	Char16 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
UInt8 CIMValue::toUInt8() const
{
	UInt8 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Int8 CIMValue::toInt8() const
{
	Int8 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
UInt16 CIMValue::toUInt16() const
{
	UInt16 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Int16 CIMValue::toInt16() const
{
	Int16 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
UInt32 CIMValue::toUInt32() const
{
	UInt32 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Int32 CIMValue::toInt32() const
{
	Int32 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
UInt64 CIMValue::toUInt64() const
{
	UInt64 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Int64 CIMValue::toInt64() const
{
	Int64 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Real32 CIMValue::toReal32() const
{
	Real32 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Real64 CIMValue::toReal64() const
{
	Real64 rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime CIMValue::toCIMDateTime() const
{
	CIMDateTime rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath CIMValue::toCIMObjectPath() const
{
	CIMObjectPath rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClass CIMValue::toCIMClass() const
{
	CIMClass rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance CIMValue::toCIMInstance() const
{
	CIMInstance rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathArray CIMValue::toCIMObjectPathArray() const
{
	CIMObjectPathArray rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Char16Array CIMValue::toChar16Array() const
{
	Char16Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
UInt8Array CIMValue::toUInt8Array() const
{
	UInt8Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Int8Array CIMValue::toInt8Array() const
{
	Int8Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
UInt16Array CIMValue::toUInt16Array() const
{
	UInt16Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Int16Array CIMValue::toInt16Array() const
{
	Int16Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
UInt32Array CIMValue::toUInt32Array() const
{
	UInt32Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Int32Array CIMValue::toInt32Array() const
{
	Int32Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
UInt64Array CIMValue::toUInt64Array() const
{
	UInt64Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Int64Array CIMValue::toInt64Array() const
{
	Int64Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Real64Array CIMValue::toReal64Array() const
{
	Real64Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
Real32Array CIMValue::toReal32Array() const
{
	Real32Array rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
StringArray CIMValue::toStringArray() const
{
	StringArray rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
BoolArray CIMValue::toBoolArray() const
{
	BoolArray rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTimeArray CIMValue::toCIMDateTimeArray() const
{
	CIMDateTimeArray rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassArray CIMValue::toCIMClassArray() const
{
	CIMClassArray rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray CIMValue::toCIMInstanceArray() const
{
	CIMInstanceArray rval;
	m_impl->get(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::setNull()
{
	m_impl = NULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue& CIMValue::set(const CIMValue& x)
{
	m_impl = x.m_impl;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue& CIMValue::operator= (const CIMValue& x)
{
	set(x);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::equal(const CIMValue& x) const
{
	if(x.m_impl == m_impl)
		return true;
	if(x.m_impl == NULL || m_impl == NULL)
		return false;
	return m_impl->equal(*x.m_impl);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator== (const CIMValue& x) const
{
	return equal(x);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator!= (const CIMValue& x) const
{
	if(x.m_impl == m_impl)
		return false;
	if(x.m_impl == NULL || m_impl == NULL)
		return true;
	return m_impl->operator!=(*x.m_impl);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator<= (const CIMValue& x) const
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
bool CIMValue::operator>= (const CIMValue& x) const
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
bool CIMValue::operator< (const CIMValue& x) const
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
bool CIMValue::operator> (const CIMValue& x) const
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
CIMDataType::Type CIMValue::getType() const {  return m_impl->getType(); }
//////////////////////////////////////////////////////////////////////////////
CIMDataType CIMValue::getCIMDataType() const { return m_impl->getCIMDataType(); }
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::sameType(const CIMValue& x) const
{
	return m_impl->sameType(*x.m_impl);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::isArray() const {  return m_impl->isArray(); }
//////////////////////////////////////////////////////////////////////////////
void CIMValue::writeObject(std::ostream &ostrm) const
{
	m_impl->writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String CIMValue::toString() const
{
	if (m_impl)
		return m_impl->toString();
	else
	{
		return "(null)";
	}
}
//////////////////////////////////////////////////////////////////////////////
String CIMValue::toMOF() const
{
	return m_impl->toMOF();
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMValue::isNumericType() const
{
	return CIMDataType::isNumericType(getType());
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMValue::CIMValueImpl
CIMValue::CIMValueImpl::createSimpleValue(const String& cimtype,
	const String& value)
{
	CIMValueImpl cimValue;
	CIMDataType::Type type = CIMDataType::strToSimpleType(cimtype);
	switch(type)
	{
		case CIMDataType::UINT8:
			cimValue = CIMValueImpl(value.toUInt8());
			break;
			
		case CIMDataType::SINT8:
			cimValue = CIMValueImpl(value.toInt8());
			break;
		case CIMDataType::UINT16:
			cimValue = CIMValueImpl(value.toUInt16());
			break;
		case CIMDataType::SINT16:
			cimValue = CIMValueImpl(value.toInt16());
			break;
		case CIMDataType::UINT32:
			cimValue = CIMValueImpl(value.toUInt32());
			break;
		case CIMDataType::SINT32:
			cimValue = CIMValueImpl(value.toInt32());
			break;
		case CIMDataType::UINT64:
			cimValue = CIMValueImpl(value.toUInt64());
			break;
		case CIMDataType::SINT64:
			cimValue = CIMValueImpl(value.toInt64());
			break;
		case CIMDataType::BOOLEAN:
			cimValue = CIMValueImpl(value.toBool());
			break;
		case CIMDataType::REAL32:
			cimValue = CIMValueImpl(value.toReal32());
			break;
		case CIMDataType::REAL64:
			cimValue = CIMValueImpl(value.toReal64());
			break;
		case CIMDataType::CHAR16:
			cimValue = CIMValueImpl(value.toChar16());
			break;
		case CIMDataType::DATETIME:
			cimValue = CIMValueImpl(value.toDateTime());
			break;
		case CIMDataType::STRING:
			cimValue = CIMValueImpl(value);
			break;
		case CIMDataType::REFERENCE:
			cimValue = CIMValueImpl(CIMObjectPath::parse(value));
			break;
		default:
			OW_ASSERT(0);
	}
	return cimValue;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl():
	m_type(CIMDataType::CIMNULL), m_isArray(false),
	m_objDestroyed(true), m_obj()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMValueImpl& arg) :
	m_type(CIMDataType::CIMNULL), m_isArray(false),
	m_objDestroyed(true), m_obj()
{
	setupObject(arg.m_obj, arg.m_type, arg.m_isArray);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(Bool v) :
	m_type(CIMDataType::BOOLEAN), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_booleanValue = (v) ? 1 : 0;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(bool v) :
	m_type(CIMDataType::BOOLEAN), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_booleanValue = (v) ? 1 : 0;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(UInt8 v) :
	m_type(CIMDataType::UINT8), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_uint8Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(Int8 v) :
	m_type(CIMDataType::SINT8), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_sint8Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const Char16& v) :
	m_type(CIMDataType::CHAR16), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) Char16(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(UInt16 v) :
	m_type(CIMDataType::UINT16), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_uint16Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(Int16 v) :
	m_type(CIMDataType::SINT16), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_sint16Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(UInt32 v) :
	m_type(CIMDataType::UINT32), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_uint32Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(Int32 v) :
	m_type(CIMDataType::SINT32), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_sint32Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(UInt64 v) :
	m_type(CIMDataType::UINT64), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_uint64Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(Int64 v) :
	m_type(CIMDataType::SINT64), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_sint64Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(Real32 v) :
	m_type(CIMDataType::REAL32), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_real32Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(Real64 v) :
	m_type(CIMDataType::REAL64), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	m_obj.m_real64Value = v;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const String& v) :
	m_type(CIMDataType::STRING), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) String(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMDateTime& v) :
	m_type(CIMDataType::DATETIME), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) CIMDateTime(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMObjectPath& v) :
	m_type(CIMDataType::REFERENCE), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) CIMObjectPath(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMClass& v) :
	m_type(CIMDataType::EMBEDDEDCLASS), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) CIMClass(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMInstance& v) :
	m_type(CIMDataType::EMBEDDEDINSTANCE), m_isArray(false),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) CIMInstance(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const Char16Array& v) :
	m_type(CIMDataType::CHAR16), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) Char16Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const UInt8Array& v) :
	m_type(CIMDataType::UINT8), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) UInt8Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const Int8Array& v) :
	m_type(CIMDataType::SINT8), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) Int8Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const UInt16Array& v) :
	m_type(CIMDataType::UINT16), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) UInt16Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const Int16Array& v) :
	m_type(CIMDataType::SINT16), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) Int16Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const UInt32Array& v) :
	m_type(CIMDataType::UINT32), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) UInt32Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const Int32Array& v) :
	m_type(CIMDataType::SINT32), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) Int32Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const UInt64Array& v) :
	m_type(CIMDataType::UINT64), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) UInt64Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const Int64Array& v) :
	m_type(CIMDataType::SINT64), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) Int64Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const Real64Array& v) :
	m_type(CIMDataType::REAL64), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) Real64Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const Real32Array& v) :
	m_type(CIMDataType::REAL32), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) Real32Array(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const StringArray& v) :
	m_type(CIMDataType::STRING), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) StringArray(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMDateTimeArray& v) :
	m_type(CIMDataType::DATETIME), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) CIMDateTimeArray(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMObjectPathArray& v) :
	m_type(CIMDataType::REFERENCE), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) CIMObjectPathArray(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const BoolArray& v) :
	m_type(CIMDataType::BOOLEAN), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) BoolArray(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMClassArray& v) :
	m_type(CIMDataType::EMBEDDEDCLASS), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) CIMClassArray(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::CIMValueImpl(const CIMInstanceArray& v) :
	m_type(CIMDataType::EMBEDDEDINSTANCE), m_isArray(true),
	m_objDestroyed(false), m_obj()
{
	new(&m_obj) CIMInstanceArray(v);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl::~CIMValueImpl()
{
	destroyObject();
}
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMValue::CIMValueImpl::getArraySize() const
{
	if(m_type == CIMDataType::CIMNULL)
	{
		return 0;
	}
	if(!m_isArray)
	{
		return 1;
	}
	UInt32 sz = 0;
	switch(m_type)
	{
		case CIMDataType::UINT8:
			sz = (reinterpret_cast<const UInt8Array*>(&m_obj))->size(); break;
		case CIMDataType::SINT8:
			sz = (reinterpret_cast<const Int8Array*>(&m_obj))->size(); break;
		case CIMDataType::UINT16:
			sz = (reinterpret_cast<const UInt16Array*>(&m_obj))->size(); break;
		case CIMDataType::SINT16:
			sz = (reinterpret_cast<const Int16Array*>(&m_obj))->size(); break;
		case CIMDataType::UINT32:
			sz = (reinterpret_cast<const UInt32Array*>(&m_obj))->size(); break;
		case CIMDataType::SINT32:
			sz = (reinterpret_cast<const Int32Array*>(&m_obj))->size(); break;
		case CIMDataType::UINT64:
			sz = (reinterpret_cast<const UInt64Array*>(&m_obj))->size(); break;
		case CIMDataType::SINT64:
			sz = (reinterpret_cast<const Int64Array*>(&m_obj))->size(); break;
		case CIMDataType::BOOLEAN:
			sz = (reinterpret_cast<const BoolArray*>(&m_obj))->size(); break;
		case CIMDataType::REAL32:
			sz = (reinterpret_cast<const Real32Array*>(&m_obj))->size(); break;
		case CIMDataType::REAL64:
			sz = (reinterpret_cast<const Real64Array*>(&m_obj))->size(); break;
		case CIMDataType::CHAR16:
			sz = (reinterpret_cast<const Char16Array*>(&m_obj))->size(); break;
		case CIMDataType::DATETIME:
			sz = (reinterpret_cast<const CIMDateTimeArray*>(&m_obj))->size(); break;
		case CIMDataType::STRING:
			sz = (reinterpret_cast<const StringArray*>(&m_obj))->size(); break;
		case CIMDataType::REFERENCE:
			sz = (reinterpret_cast<const CIMObjectPathArray*>(&m_obj))->size(); break;
		case CIMDataType::EMBEDDEDCLASS:
			sz = (reinterpret_cast<const CIMClassArray*>(&m_obj))->size(); break;
		case CIMDataType::EMBEDDEDINSTANCE:
			sz = (reinterpret_cast<const CIMInstanceArray*>(&m_obj))->size(); break;
		default:
			sz = 0;
	}
	return sz;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::setupObject(const CIMValueData& odata, CIMDataType::Type type, bool isArray)
{
	destroyObject();
	m_objDestroyed = false;
	m_type = type;
	m_isArray = isArray;
	if(isArray)
	{
		switch(m_type)
		{
			case CIMDataType::UINT8:
				new(&m_obj) UInt8Array(*(reinterpret_cast<const UInt8Array*>(&odata))); break;
			case CIMDataType::SINT8:
				new(&m_obj) Int8Array(*(reinterpret_cast<const Int8Array*>(&odata))); break;
			case CIMDataType::UINT16:
				new(&m_obj) UInt16Array(*(reinterpret_cast<const UInt16Array*>(&odata))); break;
			case CIMDataType::SINT16:
				new(&m_obj) Int16Array(*(reinterpret_cast<const Int16Array*>(&odata))); break;
			case CIMDataType::UINT32:
				new(&m_obj) UInt32Array(*(reinterpret_cast<const UInt32Array*>(&odata))); break;
			case CIMDataType::SINT32:
				new(&m_obj) Int32Array(*(reinterpret_cast<const Int32Array*>(&odata))); break;
			case CIMDataType::UINT64:
				new(&m_obj) UInt64Array(*(reinterpret_cast<const UInt64Array*>(&odata))); break;
			case CIMDataType::SINT64:
				new(&m_obj) Int64Array(*(reinterpret_cast<const Int64Array*>(&odata))); break;
			case CIMDataType::BOOLEAN:
				new(&m_obj) BoolArray(*(reinterpret_cast<const BoolArray*>(&odata))); break;
			case CIMDataType::REAL32:
				new(&m_obj) Real32Array(*(reinterpret_cast<const Real32Array*>(&odata))); break;
			case CIMDataType::REAL64:
				new(&m_obj) Real64Array(*(reinterpret_cast<const Real64Array*>(&odata))); break;
			case CIMDataType::CHAR16:
				new(&m_obj) Char16Array(*(reinterpret_cast<const Char16Array*>(&odata))); break;
			case CIMDataType::DATETIME:
				new(&m_obj) CIMDateTimeArray(*(reinterpret_cast<const CIMDateTimeArray*>(&odata))); break;
			case CIMDataType::STRING:
				new(&m_obj) StringArray(*(reinterpret_cast<const StringArray*>(&odata))); break;
			case CIMDataType::REFERENCE:
				new(&m_obj) CIMObjectPathArray(*(reinterpret_cast<const CIMObjectPathArray*>(&odata))); break;
			case CIMDataType::EMBEDDEDCLASS:
				new(&m_obj) CIMClassArray(*(reinterpret_cast<const CIMClassArray*>(&odata))); break;
			case CIMDataType::EMBEDDEDINSTANCE:
				new(&m_obj) CIMInstanceArray(*(reinterpret_cast<const CIMInstanceArray*>(&odata))); break;
			default:
				m_objDestroyed = true;
				m_type = CIMDataType::CIMNULL;
				break;
		}
	}
	else
	{
		switch(m_type)
		{
			case CIMDataType::UINT8:
			case CIMDataType::SINT8:
			case CIMDataType::UINT16:
			case CIMDataType::SINT16:
			case CIMDataType::UINT32:
			case CIMDataType::SINT32:
			case CIMDataType::UINT64:
			case CIMDataType::SINT64:
			case CIMDataType::BOOLEAN:
			case CIMDataType::REAL32:
			case CIMDataType::REAL64:
				::memmove(&m_obj, &odata, sizeof(m_obj));
				break;
			case CIMDataType::CHAR16:
				new(&m_obj) Char16(*(reinterpret_cast<const Char16*>(&odata))); break;
			case CIMDataType::REFERENCE:
				new(&m_obj) CIMObjectPath(*(reinterpret_cast<const CIMObjectPath*>(&odata))); break;
			case CIMDataType::DATETIME:
				new(&m_obj) CIMDateTime(*(reinterpret_cast<const CIMDateTime*>(&odata))); break;
			case CIMDataType::STRING:
				new(&m_obj) String(*(reinterpret_cast<const String*>(&odata))); break;
			case CIMDataType::EMBEDDEDCLASS:
				new(&m_obj) CIMClass(*(reinterpret_cast<const CIMClass*>(&odata))); break;
			case CIMDataType::EMBEDDEDINSTANCE:
				new(&m_obj) CIMInstance(*(reinterpret_cast<const CIMInstance*>(&odata))); break;
			default:
				m_objDestroyed = true;
				m_type = CIMDataType::CIMNULL;
				break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::destroyObject()
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
			case CIMDataType::UINT8:
				(reinterpret_cast<UInt8Array*>(&m_obj))->~UInt8Array(); break;
			case CIMDataType::SINT8:
				(reinterpret_cast<Int8Array*>(&m_obj))->~Int8Array(); break;
			case CIMDataType::UINT16:
				(reinterpret_cast<UInt16Array*>(&m_obj))->~UInt16Array(); break;
			case CIMDataType::SINT16:
				(reinterpret_cast<Int16Array*>(&m_obj))->~Int16Array(); break;
			case CIMDataType::UINT32:
				(reinterpret_cast<UInt32Array*>(&m_obj))->~UInt32Array(); break;
			case CIMDataType::SINT32:
				(reinterpret_cast<Int32Array*>(&m_obj))->~Int32Array(); break;
			case CIMDataType::UINT64:
				(reinterpret_cast<UInt64Array*>(&m_obj))->~UInt64Array(); break;
			case CIMDataType::SINT64:
				(reinterpret_cast<Int64Array*>(&m_obj))->~Int64Array(); break;
			case CIMDataType::BOOLEAN:
				(reinterpret_cast<BoolArray*>(&m_obj))->~BoolArray(); break;
			case CIMDataType::REAL32:
				(reinterpret_cast<Real32Array*>(&m_obj))->~Real32Array(); break;
			case CIMDataType::REAL64:
				(reinterpret_cast<Real64Array*>(&m_obj))->~Real64Array(); break;
			case CIMDataType::CHAR16:
				(reinterpret_cast<Char16Array*>(&m_obj))->~Char16Array(); break;
			case CIMDataType::DATETIME:
				(reinterpret_cast<CIMDateTimeArray*>(&m_obj))->~CIMDateTimeArray(); break;
			case CIMDataType::STRING:
				(reinterpret_cast<StringArray*>(&m_obj))->~StringArray(); break;
			case CIMDataType::REFERENCE:
				(reinterpret_cast<CIMObjectPathArray*>(&m_obj))->~CIMObjectPathArray(); break;
			case CIMDataType::EMBEDDEDCLASS:
				(reinterpret_cast<CIMClassArray*>(&m_obj))->~CIMClassArray(); break;
			case CIMDataType::EMBEDDEDINSTANCE:
				(reinterpret_cast<CIMInstanceArray*>(&m_obj))->~CIMInstanceArray(); break;
			default:
				assert(0); // don't want to throw from a destructor, just segfault
		}
	}
	else
	{
		switch(m_type)
		{
			case CIMDataType::UINT8:
			case CIMDataType::SINT8:
			case CIMDataType::UINT16:
			case CIMDataType::SINT16:
			case CIMDataType::UINT32:
			case CIMDataType::SINT32:
			case CIMDataType::UINT64:
			case CIMDataType::SINT64:
			case CIMDataType::BOOLEAN:
			case CIMDataType::REAL32:
			case CIMDataType::REAL64:
				break;
			case CIMDataType::CHAR16:
				(reinterpret_cast<Char16*>(&m_obj))->~Char16();
				break;
			case CIMDataType::REFERENCE:
				(reinterpret_cast<CIMObjectPath*>(&m_obj))->~CIMObjectPath();
				break;
			case CIMDataType::DATETIME:
				(reinterpret_cast<CIMDateTime*>(&m_obj))->~CIMDateTime();
				break;
			case CIMDataType::STRING:
				(reinterpret_cast<String*>(&m_obj))->~String();
				break;
			case CIMDataType::EMBEDDEDCLASS:
				(reinterpret_cast<CIMClass*>(&m_obj))->~CIMClass();
				break;
			case CIMDataType::EMBEDDEDINSTANCE:
				(reinterpret_cast<CIMInstance*>(&m_obj))->~CIMInstance();
				break;
			default:
				assert(0); // don't want to throw from a destructor, just segfault
		}
	}
	m_type = CIMDataType::CIMNULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl&
CIMValue::CIMValueImpl::set(const CIMValueImpl& arg)
{
	setupObject(arg.m_obj, arg.m_type, arg.m_isArray);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValueImpl&
CIMValue::CIMValueImpl::operator= (const CIMValueImpl& arg)
{
	set(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMValue::CIMValueImpl::equal(const CIMValueImpl& arg) const
{
	bool cc = false;
	if(sameType(arg))
	{
		if(m_isArray)
		{
			switch(m_type)
			{
				case CIMDataType::CHAR16:
					cc = *(reinterpret_cast<const Char16Array*>(&m_obj)) ==
						*(reinterpret_cast<const Char16Array*>(&arg.m_obj));
					break;
				case CIMDataType::UINT8:
					cc = *(reinterpret_cast<const UInt8Array*>(&m_obj)) ==
						*(reinterpret_cast<const UInt8Array*>(&arg.m_obj));
					break;
				case CIMDataType::SINT8:
					cc = *(reinterpret_cast<const Int8Array*>(&m_obj)) == *(reinterpret_cast<const Int8Array*>(&arg.m_obj));
					break;
				case CIMDataType::UINT16:
					cc = *(reinterpret_cast<const UInt16Array*>(&m_obj)) ==
						*(reinterpret_cast<const UInt16Array*>(&arg.m_obj));
					break;
				case CIMDataType::SINT16:
					cc = *(reinterpret_cast<const Int16Array*>(&m_obj)) ==
						*(reinterpret_cast<const Int16Array*>(&arg.m_obj));
					break;
				case CIMDataType::UINT32:
					cc = *(reinterpret_cast<const UInt32Array*>(&m_obj)) ==
						*(reinterpret_cast<const UInt32Array*>(&arg.m_obj));
					break;
				case CIMDataType::SINT32:
					cc = *(reinterpret_cast<const Int32Array*>(&m_obj)) ==
						*(reinterpret_cast<const Int32Array*>(&arg.m_obj));
					break;
				case CIMDataType::UINT64:
					cc = *(reinterpret_cast<const UInt64Array*>(&m_obj)) ==
						*(reinterpret_cast<const UInt64Array*>(&arg.m_obj));
					break;
				case CIMDataType::SINT64:
					cc = *(reinterpret_cast<const Int64Array*>(&m_obj)) ==
						*(reinterpret_cast<const Int64Array*>(&arg.m_obj));
					break;
				case CIMDataType::STRING:
					cc = *(reinterpret_cast<const StringArray*>(&m_obj)) ==
						*(reinterpret_cast<const StringArray*>(&arg.m_obj));
					break;
				case CIMDataType::REFERENCE:
					cc = *(reinterpret_cast<const CIMObjectPathArray*>(&m_obj)) ==
						*(reinterpret_cast<const CIMObjectPathArray*>(&arg.m_obj));
					break;
				case CIMDataType::BOOLEAN:
					cc = *(reinterpret_cast<const BoolArray*>(&m_obj)) == *(reinterpret_cast<const BoolArray*>(&arg.m_obj));
					break;
				case CIMDataType::REAL32:
					cc = *(reinterpret_cast<const Real32Array*>(&m_obj)) ==
						*(reinterpret_cast<const Real32Array*>(&arg.m_obj));
					break;
				case CIMDataType::REAL64:
					cc = *(reinterpret_cast<const Real64Array*>(&m_obj)) ==
						*(reinterpret_cast<const Real64Array*>(&arg.m_obj));
					break;
				case CIMDataType::DATETIME:
					cc = *(reinterpret_cast<const CIMDateTimeArray*>(&m_obj)) ==
						*(reinterpret_cast<const CIMDateTimeArray*>(&arg.m_obj));
					break;
				case CIMDataType::EMBEDDEDCLASS:
					cc = *(reinterpret_cast<const CIMClassArray*>(&m_obj)) ==
						*(reinterpret_cast<const CIMClassArray*>(&arg.m_obj));
					break;
				case CIMDataType::EMBEDDEDINSTANCE:
					cc = *(reinterpret_cast<const CIMInstanceArray*>(&m_obj)) ==
						*(reinterpret_cast<const CIMInstanceArray*>(&arg.m_obj));
					break;
				default:
					OW_ASSERT(0);
			}
		}
		else
		{
			switch(m_type)
			{
				case CIMDataType::UINT8:
					cc = m_obj.m_uint8Value == arg.m_obj.m_uint8Value;
					break;
				case CIMDataType::SINT8:
					cc = m_obj.m_sint8Value == arg.m_obj.m_sint8Value;
					break;
				case CIMDataType::UINT16:
					cc = m_obj.m_uint16Value == arg.m_obj.m_uint16Value;
					break;
				case CIMDataType::SINT16:
					cc = m_obj.m_sint16Value == arg.m_obj.m_sint16Value;
					break;
				case CIMDataType::UINT32:
					cc = m_obj.m_uint32Value == arg.m_obj.m_uint32Value;
					break;
				case CIMDataType::SINT32:
					cc = m_obj.m_sint32Value == arg.m_obj.m_sint32Value;
					break;
				case CIMDataType::UINT64:
					cc = m_obj.m_uint64Value == arg.m_obj.m_uint64Value;
					break;
				case CIMDataType::SINT64:
					cc = m_obj.m_sint64Value == arg.m_obj.m_sint64Value;
					break;
				case CIMDataType::CHAR16:
					cc = *(reinterpret_cast<const Char16*>(&m_obj)) == *(reinterpret_cast<const Char16*>(&arg.m_obj));
					break;
				case CIMDataType::STRING:
					cc = *(reinterpret_cast<const String*>(&m_obj)) == *(reinterpret_cast<const String*>(&arg.m_obj));
					break;
				case CIMDataType::BOOLEAN:
					cc = m_obj.m_booleanValue == arg.m_obj.m_booleanValue;
					break;
				case CIMDataType::REAL32:
					cc = m_obj.m_real32Value == arg.m_obj.m_real32Value;
					break;
				case CIMDataType::REAL64:
					cc = m_obj.m_real64Value == arg.m_obj.m_real64Value;
					break;
				case CIMDataType::DATETIME:
					cc = *(reinterpret_cast<const CIMDateTime*>(&m_obj)) ==
						*(reinterpret_cast<const CIMDateTime*>(&arg.m_obj));
					break;
				case CIMDataType::REFERENCE:
					cc = *(reinterpret_cast<const CIMObjectPath*>(&m_obj)) ==
						*(reinterpret_cast<const CIMObjectPath*>(&arg.m_obj));
					break;
				case CIMDataType::EMBEDDEDCLASS:
					cc = *(reinterpret_cast<const CIMClass*>(&m_obj)) ==
						*(reinterpret_cast<const CIMClass*>(&arg.m_obj));
					break;
				case CIMDataType::EMBEDDEDINSTANCE:
					cc = *(reinterpret_cast<const CIMInstance*>(&m_obj)) ==
						*(reinterpret_cast<const CIMInstance*>(&arg.m_obj));
					break;
				default:
					OW_ASSERT(0);
			}
		}
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMValue::CIMValueImpl::operator<(const CIMValueImpl& arg) const
{
	bool cc = false;
	if(sameType(arg))
	{
		if(m_isArray)
		{
			switch(m_type)
			{
				case CIMDataType::CHAR16:
					cc = *(reinterpret_cast<const Char16Array*>(&m_obj)) <
						*(reinterpret_cast<const Char16Array*>(&arg.m_obj));
					break;
				case CIMDataType::UINT8:
					cc = *(reinterpret_cast<const UInt8Array*>(&m_obj)) <
						*(reinterpret_cast<const UInt8Array*>(&arg.m_obj));
					break;
				case CIMDataType::SINT8:
					cc = *(reinterpret_cast<const Int8Array*>(&m_obj)) < *(reinterpret_cast<const Int8Array*>(&arg.m_obj));
					break;
				case CIMDataType::UINT16:
					cc = *(reinterpret_cast<const UInt16Array*>(&m_obj)) <
						*(reinterpret_cast<const UInt16Array*>(&arg.m_obj));
					break;
				case CIMDataType::SINT16:
					cc = *(reinterpret_cast<const Int16Array*>(&m_obj)) <
						*(reinterpret_cast<const Int16Array*>(&arg.m_obj));
					break;
				case CIMDataType::UINT32:
					cc = *(reinterpret_cast<const UInt32Array*>(&m_obj)) <
						*(reinterpret_cast<const UInt32Array*>(&arg.m_obj));
					break;
				case CIMDataType::SINT32:
					cc = *(reinterpret_cast<const Int32Array*>(&m_obj)) <
						*(reinterpret_cast<const Int32Array*>(&arg.m_obj));
					break;
				case CIMDataType::UINT64:
					cc = *(reinterpret_cast<const UInt64Array*>(&m_obj)) <
						*(reinterpret_cast<const UInt64Array*>(&arg.m_obj));
					break;
				case CIMDataType::SINT64:
					cc = *(reinterpret_cast<const Int64Array*>(&m_obj)) <
						*(reinterpret_cast<const Int64Array*>(&arg.m_obj));
					break;
				case CIMDataType::STRING:
					cc = *(reinterpret_cast<const StringArray*>(&m_obj)) <
						*(reinterpret_cast<const StringArray*>(&arg.m_obj));
					break;
				case CIMDataType::REFERENCE:
					cc = *(reinterpret_cast<const CIMObjectPathArray*>(&m_obj)) <
						*(reinterpret_cast<const CIMObjectPathArray*>(&arg.m_obj));
					break;
				case CIMDataType::BOOLEAN:
					cc = *(reinterpret_cast<const BoolArray*>(&m_obj)) < *(reinterpret_cast<const BoolArray*>(&arg.m_obj));
					break;
				case CIMDataType::REAL32:
					cc = *(reinterpret_cast<const Real32Array*>(&m_obj)) <
						*(reinterpret_cast<const Real32Array*>(&arg.m_obj));
					break;
				case CIMDataType::REAL64:
					cc = *(reinterpret_cast<const Real64Array*>(&m_obj)) <
						*(reinterpret_cast<const Real64Array*>(&arg.m_obj));
					break;
				case CIMDataType::DATETIME:
					cc = *(reinterpret_cast<const CIMDateTimeArray*>(&m_obj)) <
						*(reinterpret_cast<const CIMDateTimeArray*>(&arg.m_obj));
					break;
				case CIMDataType::EMBEDDEDCLASS:
					cc = *(reinterpret_cast<const CIMClassArray*>(&m_obj)) <
						*(reinterpret_cast<const CIMClassArray*>(&arg.m_obj));
					break;
				case CIMDataType::EMBEDDEDINSTANCE:
					cc = *(reinterpret_cast<const CIMInstanceArray*>(&m_obj)) <
						*(reinterpret_cast<const CIMInstanceArray*>(&arg.m_obj));
					break;
				default:
					OW_ASSERT(0);
			}
		}
		else
		{
			switch(m_type)
			{
				case CIMDataType::UINT8:
					cc = m_obj.m_uint8Value < arg.m_obj.m_uint8Value;
					break;
				case CIMDataType::SINT8:
					cc = m_obj.m_sint8Value < arg.m_obj.m_sint8Value;
					break;
				case CIMDataType::UINT16:
					cc = m_obj.m_uint16Value < arg.m_obj.m_uint16Value;
					break;
				case CIMDataType::SINT16:
					cc = m_obj.m_sint16Value < arg.m_obj.m_sint16Value;
					break;
				case CIMDataType::UINT32:
					cc = m_obj.m_uint32Value < arg.m_obj.m_uint32Value;
					break;
				case CIMDataType::SINT32:
					cc = m_obj.m_sint32Value < arg.m_obj.m_sint32Value;
					break;
				case CIMDataType::UINT64:
					cc = m_obj.m_uint64Value < arg.m_obj.m_uint64Value;
					break;
				case CIMDataType::SINT64:
					cc = m_obj.m_sint64Value < arg.m_obj.m_sint64Value;
					break;
				case CIMDataType::CHAR16:
					cc = *(reinterpret_cast<const Char16*>(&m_obj)) < *(reinterpret_cast<const Char16*>(&arg.m_obj));
					break;
				case CIMDataType::STRING:
					cc = *(reinterpret_cast<const String*>(&m_obj)) < *(reinterpret_cast<const String*>(&arg.m_obj));
					break;
				case CIMDataType::BOOLEAN:
					cc = m_obj.m_booleanValue < arg.m_obj.m_booleanValue;
					break;
				case CIMDataType::REAL32:
					cc = m_obj.m_real32Value < arg.m_obj.m_real32Value;
					break;
				case CIMDataType::REAL64:
					cc = m_obj.m_real64Value < arg.m_obj.m_real64Value;
					break;
				case CIMDataType::DATETIME:
					cc = *(reinterpret_cast<const CIMDateTime*>(&m_obj)) <
						*(reinterpret_cast<const CIMDateTime*>(&arg.m_obj));
					break;
				case CIMDataType::REFERENCE:
					cc = *(reinterpret_cast<const CIMObjectPath*>(&m_obj)) <
						*(reinterpret_cast<const CIMObjectPath*>(&arg.m_obj));
					break;
				case CIMDataType::EMBEDDEDCLASS:
					cc = *(reinterpret_cast<const CIMClass*>(&m_obj)) <
						*(reinterpret_cast<const CIMClass*>(&arg.m_obj));
					break;
				case CIMDataType::EMBEDDEDINSTANCE:
					cc = *(reinterpret_cast<const CIMInstance*>(&m_obj)) <
						*(reinterpret_cast<const CIMInstance*>(&arg.m_obj));
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
CIMValue::CIMValueImpl::get(Bool& arg) const
{
	if(m_type != CIMDataType::BOOLEAN || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a BOOLEAN");
	arg = (m_obj.m_booleanValue != 0);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Char16& arg) const
{
	if(m_type != CIMDataType::CHAR16 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a CHAR16");
	arg = *(reinterpret_cast<const Char16*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(UInt8& arg) const
{
	if(m_type != CIMDataType::UINT8 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a UINT8");
	arg = m_obj.m_uint8Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Int8& arg) const
{
	if(m_type != CIMDataType::SINT8 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a SINT8");
	arg = m_obj.m_sint8Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(UInt16& arg) const
{
	if(m_type != CIMDataType::UINT16 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a UINT16");
	arg = m_obj.m_uint16Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Int16& arg) const
{
	if(m_type != CIMDataType::SINT16 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a SINT16");
	arg = m_obj.m_sint16Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(UInt32& arg) const
{
	if(m_type != CIMDataType::UINT32 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a UINT32");
	arg = m_obj.m_uint32Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Int32& arg) const
{
	if(m_type != CIMDataType::SINT32 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a SINT32");
	arg = m_obj.m_sint32Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(UInt64& arg) const
{
	if(m_type != CIMDataType::UINT64 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a UINT64");
	arg = m_obj.m_uint64Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Int64& arg) const
{
	if(m_type != CIMDataType::SINT64 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a SINT64");
	arg = m_obj.m_sint64Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Real32& arg) const
{
	if(m_type != CIMDataType::REAL32 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a REAL32");
	arg = m_obj.m_real32Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Real64& arg) const
{
	if(m_type != CIMDataType::REAL64 || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a REAL64");
	arg = m_obj.m_real64Value;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(String& arg) const
{
	if(m_type != CIMDataType::STRING || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a STRING");
	arg = *(reinterpret_cast<const String*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(CIMDateTime& arg) const
{
	if(m_type != CIMDataType::DATETIME || isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a DATETIME");
	arg = *(reinterpret_cast<const CIMDateTime*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(CIMObjectPath& arg) const
{
	if(m_type != CIMDataType::REFERENCE)
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a REFERENCE");
	arg = *(reinterpret_cast<const CIMObjectPath*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(CIMClass& arg) const
{
	if(m_type != CIMDataType::EMBEDDEDCLASS)
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a EMBEDDEDCLASS");
	arg = *(reinterpret_cast<const CIMClass*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(CIMInstance& arg) const
{
	if(m_type != CIMDataType::EMBEDDEDINSTANCE)
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a EMBEDDEDINSTANCE");
	arg = *(reinterpret_cast<const CIMInstance*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Char16Array& arg) const
{
	if(m_type != CIMDataType::CHAR16 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a CHAR16 ARRAY");
	arg = *(reinterpret_cast<const Char16Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(UInt8Array& arg) const
{
	if(m_type != CIMDataType::UINT8 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a UINT8 ARRAY");
	arg = *(reinterpret_cast<const UInt8Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Int8Array& arg) const
{
	if(m_type != CIMDataType::SINT8 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a SINT8 ARRAY");
	arg = *(reinterpret_cast<const Int8Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(UInt16Array& arg) const
{
	if(m_type != CIMDataType::UINT16 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a UINT16 ARRAY");
	arg = *(reinterpret_cast<const UInt16Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Int16Array& arg) const
{
	if(m_type != CIMDataType::SINT16 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a SINT16 ARRAY");
	arg = *(reinterpret_cast<const Int16Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(UInt32Array& arg) const
{
	if(m_type != CIMDataType::UINT32 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a UINT32 ARRAY");
	arg = *(reinterpret_cast<const UInt32Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Int32Array& arg) const
{
	if(m_type != CIMDataType::SINT32 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a SINT32 ARRAY");
	arg = *(reinterpret_cast<const Int32Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(UInt64Array& arg) const
{
	if(m_type != CIMDataType::UINT64 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a UINT64 ARRAY");
	arg = *(reinterpret_cast<const UInt64Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Int64Array& arg) const
{
	if(m_type != CIMDataType::SINT64 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a SINT64 ARRAY");
	arg = *(reinterpret_cast<const Int64Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Real64Array& arg) const
{
	if(m_type != CIMDataType::REAL64 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a REAL64 ARRAY");
	arg = *(reinterpret_cast<const Real64Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(Real32Array& arg) const
{
	if(m_type != CIMDataType::REAL32 || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a REAL32 ARRAY");
	arg = *(reinterpret_cast<const Real32Array*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(StringArray& arg) const
{
	if(m_type != CIMDataType::STRING || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a STRING ARRAY");
	arg = *(reinterpret_cast<const StringArray*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(BoolArray& arg) const
{
	if(m_type != CIMDataType::BOOLEAN || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a BOOLEAN ARRAY");
	arg = *(reinterpret_cast<const BoolArray*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(CIMDateTimeArray& arg) const
{
	if(m_type != CIMDataType::DATETIME || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a DATETIME ARRAY");
	arg = *(reinterpret_cast<const CIMDateTimeArray*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(CIMObjectPathArray& arg) const
{
	if(m_type != CIMDataType::REFERENCE || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a OBJECTPATH ARRAY");
	arg = *(reinterpret_cast<const CIMObjectPathArray*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(CIMClassArray& arg) const
{
	if(m_type != CIMDataType::EMBEDDEDCLASS || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a EMBEDDEDCLASS ARRAY");
	arg = *(reinterpret_cast<const CIMClassArray*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::get(CIMInstanceArray& arg) const
{
	if(m_type != CIMDataType::EMBEDDEDINSTANCE || !isArray())
		OW_THROW(ValueCastException,
			"CIMValue::CIMValueImpl::get - Value is not a EMBEDDEDINSTANCE ARRAY");
	arg = *(reinterpret_cast<const CIMInstanceArray*>(&m_obj));
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
String raToString(const T& ra, bool forMOF=false)
{
	StringBuffer out;
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
static String
raToString(const Array<String>& ra, bool forMOF=false)
{
	StringBuffer out;
	for(size_t i = 0; i < ra.size(); i++)
	{
		if(i > 0)
			out += ',';
		if (forMOF)
		{
			out += '"';
			out += CIMObjectPath::escape(ra[i]);
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
static String
raToString(const Array<CIMClass>& ra, bool forMOF=false)
{
	StringBuffer out;
	for(size_t i = 0; i < ra.size(); i++)
	{
		if(i > 0)
			out += ',';
		if (forMOF)
		{
			out += '"';
			out += CIMObjectPath::escape(ra[i].toString());
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
static String
raToString(const Array<CIMInstance>& ra, bool forMOF=false)
{
	StringBuffer out;
	for(size_t i = 0; i < ra.size(); i++)
	{
		if(i > 0)
			out += ',';
		if (forMOF)
		{
			out += '"';
			out += CIMObjectPath::escape(ra[i].toString());
			out += '"';
		}
		else
		{
			out += ra[i].toString();
		}
	}
	return out.releaseString();
}
static String
raToString(const Array<Bool>& ra, bool isString=false)
{
	StringBuffer out;
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
String
CIMValue::CIMValueImpl::toString(bool forMOF) const
{
	String out;
	if(m_isArray)
	{
		switch(m_type)
		{
			case CIMDataType::BOOLEAN:
				out = raToString(*(reinterpret_cast<const BoolArray*>(&m_obj)));
				break;
			case CIMDataType::UINT8:
				out = raToString(*(reinterpret_cast<const UInt8Array*>(&m_obj)));
				break;
			case CIMDataType::SINT8:
				out = raToString(*(reinterpret_cast<const Int8Array*>(&m_obj)));
				break;
			// ATTN: UTF8
			case CIMDataType::CHAR16:
				out = raToString(*(reinterpret_cast<const Char16Array*>(&m_obj)));
				break;
			case CIMDataType::UINT16:
				out = raToString(*(reinterpret_cast<const UInt16Array*>(&m_obj)));
				break;
			case CIMDataType::SINT16:
				out = raToString(*(reinterpret_cast<const Int16Array*>(&m_obj)));
				break;
			case CIMDataType::UINT32:
				out = raToString(*(reinterpret_cast<const UInt32Array*>(&m_obj)));
				break;
			case CIMDataType::SINT32:
				out = raToString(*(reinterpret_cast<const Int32Array*>(&m_obj)));
				break;
			case CIMDataType::UINT64:
				out = raToString(*(reinterpret_cast<const UInt64Array*>(&m_obj)));
				break;
			case CIMDataType::SINT64:
				out = raToString(*(reinterpret_cast<const Int64Array*>(&m_obj)));
				break;
			case CIMDataType::REAL32:
				out = raToString(*(reinterpret_cast<const Real32Array*>(&m_obj)));
				break;
			case CIMDataType::REAL64:
				out = raToString(*(reinterpret_cast<const Real64Array*>(&m_obj)));
				break;
			case CIMDataType::STRING:
				out = raToString(*(reinterpret_cast<const StringArray*>(&m_obj)), forMOF);
				break;
			case CIMDataType::DATETIME:
				out = raToString(*(reinterpret_cast<const CIMDateTimeArray*>(&m_obj)));
				break;
			case CIMDataType::REFERENCE:
				out = raToString(*(reinterpret_cast<const CIMObjectPathArray*>(&m_obj)));
				break;
			
			case CIMDataType::EMBEDDEDCLASS:
				out = raToString(*(reinterpret_cast<const CIMClassArray*>(&m_obj)));
				break;
			
			case CIMDataType::EMBEDDEDINSTANCE:
				out = raToString(*(reinterpret_cast<const CIMInstanceArray*>(&m_obj)));
				break;
			default:
				OW_ASSERT(0);
		}
	}
	else
	{
		switch(m_type)
		{
			case CIMDataType::BOOLEAN:
				out = Bool(m_obj.m_booleanValue != 0).toString();
				break;
			case CIMDataType::UINT8:
				out = String(static_cast<UInt32>(m_obj.m_uint8Value));
				break;
			case CIMDataType::SINT8:
				out = String(static_cast<Int32>(m_obj.m_sint8Value));
				break;
			case CIMDataType::CHAR16:
				out = String(static_cast<char>(reinterpret_cast<const Char16*>(&m_obj)->getValue()));
				break;
			case CIMDataType::UINT16:
				out = String(static_cast<UInt32>(m_obj.m_uint16Value));
				break;
			case CIMDataType::SINT16:
				out = String(static_cast<Int32>(m_obj.m_sint16Value));
				break;
			case CIMDataType::UINT32:
				out = String(m_obj.m_uint32Value);
				break;
			case CIMDataType::SINT32:
				out = String(m_obj.m_sint32Value);
				break;
			case CIMDataType::UINT64:
				out = String(m_obj.m_uint64Value);
				break;
			case CIMDataType::SINT64:
				out = String(m_obj.m_sint64Value);
				break;
			case CIMDataType::REAL32:
				out = String(static_cast<Real64>(m_obj.m_real32Value));
				break;
			case CIMDataType::REAL64:
				out = String(m_obj.m_real64Value);
				break;
			case CIMDataType::STRING:
				if(forMOF)
				{
					out = "\"";
					out += CIMObjectPath::escape(*(reinterpret_cast<const String*>(&m_obj)));
					out += "\"";
				}
				else
				{
					out = *(reinterpret_cast<const String*>(&m_obj));
				}
				break;
			case CIMDataType::DATETIME:
				out = (reinterpret_cast<const CIMDateTime*>(&m_obj))->toString();
				break;
			case CIMDataType::REFERENCE:
				if (forMOF)
				{
					out = CIMObjectPath::escape((reinterpret_cast<const CIMObjectPath*>(&m_obj))->toString());
				}
				else
				{
					out = (reinterpret_cast<const CIMObjectPath*>(&m_obj))->toString();
				}
				break;
			
			case CIMDataType::EMBEDDEDCLASS:
				if(forMOF)
				{
					out = "\"";
					out += CIMObjectPath::escape((reinterpret_cast<const CIMClass*>(&m_obj))->toString());
					out += "\"";
				}
				else
				{
					out = (reinterpret_cast<const CIMClass*>(&m_obj))->toString();
				}
				break;
			case CIMDataType::EMBEDDEDINSTANCE:
				if(forMOF)
				{
					out = "\"";
					out += CIMObjectPath::escape((reinterpret_cast<const CIMInstance*>(&m_obj))->toString());
					out += "\"";
				}
				else
				{
					out = (reinterpret_cast<const CIMInstance*>(&m_obj))->toString();
				}
				break;
			default:
				OW_ASSERT(0);
		}
	}
	return out;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMValue::CIMValueImpl::toMOF() const
{
	StringBuffer rv;
	if(m_type == CIMDataType::CIMNULL
		|| m_type == CIMDataType::INVALID)
	{
		return String();
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
	BinarySerialization::read(istrm, &val, sizeof(val));
	switch(convType)
	{
		case 3: { val = static_cast<T>(ntoh64(val)); break; }
		case 2: { val = static_cast<T>(ntoh32(val)); break; }
		case 1: { val = static_cast<T>(ntoh16(val)); break; }
	}
}
//////////////////////////////////////////////////////////////////////////////
static Real64
readReal64Value(istream& istrm)
{
	String rstr;
	rstr.readObject(istrm);
	return rstr.toReal64();
}
//////////////////////////////////////////////////////////////////////////////
static Real32
readReal32Value(istream& istrm)
{
	String rstr;
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
	UInt32 sz;
	BinarySerialization::readLen(istrm, sz);
	for(UInt32 i = 0; i < sz; i++)
	{
		typename T::value_type v;
		BinarySerialization::read(istrm, &v, sizeof(v));
		switch(convType)
		{
			case 3: v = static_cast<typename T::value_type>(ntoh64(v)); break;
			case 2: v = static_cast<typename T::value_type>(ntoh32(v)); break;
			case 1: v = static_cast<typename T::value_type>(ntoh16(v)); break;
		}
		ra.push_back(v);
	}
}
static void
readReal32Array(istream& istrm, Array<Real32>& ra)
{
	ra.clear();
	UInt32 sz;
	BinarySerialization::readLen(istrm, sz);
	for(UInt32 i = 0; i < sz; i++)
	{
		Real32 v = readReal32Value(istrm);
		ra.push_back(v);
	}
}
static void
readReal64Array(istream& istrm, Array<Real64>& ra)
{
	ra.clear();
	UInt32 sz;
	BinarySerialization::readLen(istrm, sz);
	for(UInt32 i = 0; i < sz; i++)
	{
		Real64 v = readReal64Value(istrm);
		ra.push_back(v);
	}
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
void
readObjectArray(istream& istrm, T& ra)
{
	BinarySerialization::readArray(istrm, ra);
//	ra.clear();
//	Int32 sz;
//	readValue(istrm, sz, 2);
//	for(Int32 i = 0; i < sz; i++)
//	{
//		typename T::value_type v;
//		v.readObject(istrm);
//		ra.push_back(v);
//	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::readObject(istream &istrm)
{
	CIMBase::readSig( istrm, OW_CIMVALUESIG );
	destroyObject();
	m_objDestroyed = false;
	UInt32 tmp;
	BinarySerialization::readLen(istrm, tmp);
	m_type = CIMDataType::Type(tmp);
	m_isArray.readObject(istrm);
	if(m_isArray)
	{
		switch(m_type)
		{
			case CIMDataType::BOOLEAN:
				new(&m_obj) BoolArray;
				readArray(istrm, *(reinterpret_cast<BoolArray*>(&m_obj)), 0);
				break;
			case CIMDataType::UINT8:
				new(&m_obj) UInt8Array;
				readArray(istrm, *(reinterpret_cast<UInt8Array*>(&m_obj)), 0);
				break;
			case CIMDataType::SINT8:
				new(&m_obj) Int8Array;
				readArray(istrm, *(reinterpret_cast<Int8Array*>(&m_obj)), 0);
				break;
			case CIMDataType::CHAR16:
				new(&m_obj) Char16Array;
				readObjectArray(istrm, *(reinterpret_cast<Char16Array*>(&m_obj)));
				break;
			case CIMDataType::UINT16:
				new(&m_obj) UInt16Array;
				readArray(istrm, *(reinterpret_cast<UInt16Array*>(&m_obj)), 1);
				break;
			case CIMDataType::SINT16:
				new(&m_obj) Int16Array;
				readArray(istrm, *(reinterpret_cast<Int16Array*>(&m_obj)), 1);
				break;
			case CIMDataType::UINT32:
				new(&m_obj) UInt32Array;
				readArray(istrm, *(reinterpret_cast<UInt32Array*>(&m_obj)), 2);
				break;
			case CIMDataType::SINT32:
				new(&m_obj) Int32Array;
				readArray(istrm, *(reinterpret_cast<Int32Array*>(&m_obj)), 2);
				break;
			case CIMDataType::UINT64:
				new(&m_obj) UInt64Array;
				readArray(istrm, *(reinterpret_cast<UInt64Array*>(&m_obj)), 3);
				break;
			case CIMDataType::SINT64:
				new(&m_obj) Int64Array;
				readArray(istrm, *(reinterpret_cast<Int64Array*>(&m_obj)), 3);
				break;
			case CIMDataType::REAL32:
				new(&m_obj) Real32Array;
				readReal32Array(istrm, *(reinterpret_cast<Real32Array*>(&m_obj)));
				break;
			case CIMDataType::REAL64:
				new(&m_obj) Real64Array;
				readReal64Array(istrm, *(reinterpret_cast<Real64Array*>(&m_obj)));
				break;
			case CIMDataType::STRING:
				new(&m_obj) StringArray;
				readObjectArray(istrm, *(reinterpret_cast<StringArray*>(&m_obj)));
				break;
			case CIMDataType::DATETIME:
				new(&m_obj) CIMDateTimeArray;
				readObjectArray(istrm, *(reinterpret_cast<CIMDateTimeArray*>(&m_obj)));
				break;
			case CIMDataType::REFERENCE:
				new(&m_obj) CIMObjectPathArray;
				readObjectArray(istrm, *(reinterpret_cast<CIMObjectPathArray*>(&m_obj)));
				break;
			case CIMDataType::EMBEDDEDCLASS:
				new(&m_obj) CIMClassArray;
				readObjectArray(istrm, *(reinterpret_cast<CIMClassArray*>(&m_obj)));
				break;
			case CIMDataType::EMBEDDEDINSTANCE:
				new(&m_obj) CIMInstanceArray;
				readObjectArray(istrm, *(reinterpret_cast<CIMInstanceArray*>(&m_obj)));
				break;
			default:
				OW_ASSERT(0);
		}
	}
	else
	{
		switch(m_type)
		{
			case CIMDataType::BOOLEAN:
				readValue(istrm, m_obj.m_booleanValue, 0);
				break;
			case CIMDataType::UINT8:
				readValue(istrm, m_obj.m_uint8Value, 0);
				break;
			case CIMDataType::SINT8:
				readValue(istrm, m_obj.m_sint8Value, 0);
				break;
			case CIMDataType::UINT16:
				readValue(istrm, m_obj.m_uint16Value, 1);
				break;
			case CIMDataType::SINT16:
				readValue(istrm, m_obj.m_sint16Value, 1);
				break;
			case CIMDataType::UINT32:
				readValue(istrm, m_obj.m_uint32Value, 2);
				break;
			case CIMDataType::SINT32:
				readValue(istrm, m_obj.m_sint32Value, 2);
				break;
			case CIMDataType::UINT64:
				readValue(istrm, m_obj.m_uint64Value, 3);
				break;
			case CIMDataType::SINT64:
				readValue(istrm, m_obj.m_sint64Value, 3);
				break;
			case CIMDataType::REAL32:
				m_obj.m_real32Value = readReal32Value(istrm);
				break;
			case CIMDataType::REAL64:
				m_obj.m_real64Value = readReal64Value(istrm);
				break;
			case CIMDataType::CHAR16:
				new(&m_obj) Char16;
				(reinterpret_cast<Char16*>(&m_obj))->readObject(istrm);
				break;
			case CIMDataType::STRING:
				new(&m_obj) String;
				(reinterpret_cast<String*>(&m_obj))->readObject(istrm);
				break;
			case CIMDataType::DATETIME:
				new(&m_obj) CIMDateTime(CIMNULL);
				(reinterpret_cast<CIMDateTime*>(&m_obj))->readObject(istrm);
				break;
			case CIMDataType::REFERENCE:
				new(&m_obj) CIMObjectPath(CIMNULL);
				(reinterpret_cast<CIMObjectPath*>(&m_obj))->readObject(istrm);
				break;
			case CIMDataType::EMBEDDEDCLASS:
				new(&m_obj) CIMClass(CIMNULL);
				(reinterpret_cast<CIMClass*>(&m_obj))->readObject(istrm);
				break;
			case CIMDataType::EMBEDDEDINSTANCE:
				new(&m_obj) CIMInstance(CIMNULL);
				(reinterpret_cast<CIMInstance*>(&m_obj))->readObject(istrm);
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
		case 3: v = static_cast<T>(hton64(val)); break;
		case 2: v = static_cast<T>(hton32(val)); break;
		case 1: v = static_cast<T>(hton16(val)); break;
		default: v = val; break;
	}
	BinarySerialization::write(ostrm, &v, sizeof(v));
}
static void
writeRealValue(ostream& ostrm, Real64 rv)
{
	String v(rv);
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
	UInt32 sz = ra.size();
	BinarySerialization::writeLen(ostrm, sz);
	for(UInt32 i = 0; i < sz; i++)
	{
		typename T::value_type v;
		switch(convType)
		{
			case 3: v = static_cast<typename T::value_type>(hton64(ra[i])); break;
			case 2: v = static_cast<typename T::value_type>(hton32(ra[i])); break;
			case 1: v = static_cast<typename T::value_type>(hton16(ra[i])); break;
			default: v = static_cast<typename T::value_type>(ra[i]); break;
		}
		BinarySerialization::write(ostrm, &v, sizeof(v));
	}
}
static void
writeArray(ostream& ostrm, const Array<Real32>& ra)
{
	UInt32 sz = ra.size();
	BinarySerialization::writeLen(ostrm, sz);
	for(UInt32 i = 0; i < sz; i++)
	{
		writeRealValue(ostrm, static_cast<Real64>(ra[i]));
	}
}
static void
writeArray(ostream& ostrm, const Array<Real64>& ra)
{
	UInt32 sz = ra.size();
	BinarySerialization::writeLen(ostrm, sz);
	for(UInt32 i = 0; i < sz; i++)
	{
		writeRealValue(ostrm, ra[i]);
	}
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
void
writeObjectArray(ostream& ostrm, const T& ra)
{
	BinarySerialization::writeArray(ostrm, ra);
//	Int32 sz = ra.size();
//	writeValue(ostrm, sz, 2);
//	for(Int32 i = 0; i < sz; i++)
//	{
//		ra[i].writeObject(ostrm);
//	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMValue::CIMValueImpl::writeObject(ostream &ostrm) const
{
	CIMBase::writeSig(ostrm, OW_CIMVALUESIG);
	BinarySerialization::writeLen(ostrm, m_type);
	m_isArray.writeObject(ostrm);
	if(m_isArray)
	{
		switch(m_type)
		{
			case CIMDataType::BOOLEAN:
				writeArray(ostrm, *(reinterpret_cast<const BoolArray*>(&m_obj)), 0);
				break;
			case CIMDataType::UINT8:
				writeArray(ostrm, *(reinterpret_cast<const UInt8Array*>(&m_obj)), 0);
				break;
			case CIMDataType::SINT8:
				writeArray(ostrm, *(reinterpret_cast<const Int8Array*>(&m_obj)), 0);
				break;
			case CIMDataType::UINT16:
				writeArray(ostrm, *(reinterpret_cast<const UInt16Array*>(&m_obj)), 1);
				break;
			case CIMDataType::SINT16:
				writeArray(ostrm, *(reinterpret_cast<const Int16Array*>(&m_obj)), 1);
				break;
			case CIMDataType::UINT32:
				writeArray(ostrm, *(reinterpret_cast<const UInt32Array*>(&m_obj)), 2);
				break;
			case CIMDataType::SINT32:
				writeArray(ostrm, *(reinterpret_cast<const Int32Array*>(&m_obj)), 2);
				break;
			case CIMDataType::UINT64:
				writeArray(ostrm, *(reinterpret_cast<const UInt64Array*>(&m_obj)), 3);
				break;
			case CIMDataType::SINT64:
				writeArray(ostrm, *(reinterpret_cast<const Int64Array*>(&m_obj)), 3);
				break;
			case CIMDataType::REAL32:
				writeArray(ostrm, *(reinterpret_cast<const Real32Array*>(&m_obj)));
				break;
			case CIMDataType::REAL64:
				writeArray(ostrm, *(reinterpret_cast<const Real64Array*>(&m_obj)));
				break;
			case CIMDataType::CHAR16:
				writeObjectArray(ostrm, *(reinterpret_cast<const Char16Array*>(&m_obj)));
				break;
			case CIMDataType::STRING:
				writeObjectArray(ostrm, *(reinterpret_cast<const StringArray*>(&m_obj)));
				break;
			case CIMDataType::DATETIME:
				writeObjectArray(ostrm, *(reinterpret_cast<const CIMDateTimeArray*>(&m_obj)));
				break;
			case CIMDataType::REFERENCE:
				writeObjectArray(ostrm, *(reinterpret_cast<const CIMObjectPathArray*>(&m_obj)));
				break;
			case CIMDataType::EMBEDDEDCLASS:
				writeObjectArray(ostrm, *(reinterpret_cast<const CIMClassArray*>(&m_obj)));
				break;
			case CIMDataType::EMBEDDEDINSTANCE:
				writeObjectArray(ostrm, *(reinterpret_cast<const CIMInstanceArray*>(&m_obj)));
				break;
			default:
				OW_ASSERT(0);
		}
	}
	else
	{
		switch(m_type)
		{
			case CIMDataType::BOOLEAN:
				writeValue(ostrm, m_obj.m_booleanValue, 0);
				break;
			case CIMDataType::UINT8:
				writeValue(ostrm, m_obj.m_uint8Value, 0);
				break;
			case CIMDataType::SINT8:
				writeValue(ostrm, m_obj.m_sint8Value, 0);
				break;
			case CIMDataType::UINT16:
				writeValue(ostrm, m_obj.m_uint16Value, 1);
				break;
			case CIMDataType::SINT16:
				writeValue(ostrm, m_obj.m_sint16Value, 1);
				break;
			case CIMDataType::UINT32:
				writeValue(ostrm, m_obj.m_uint32Value, 2);
				break;
			case CIMDataType::SINT32:
				writeValue(ostrm, m_obj.m_sint32Value, 2);
				break;
			case CIMDataType::UINT64:
				writeValue(ostrm, m_obj.m_uint64Value, 3);
				break;
			case CIMDataType::SINT64:
				writeValue(ostrm, m_obj.m_sint64Value, 3);
				break;
			case CIMDataType::REAL32:
				writeRealValue(ostrm, static_cast<Real64>(m_obj.m_real32Value));
				break;
			case CIMDataType::REAL64:
				writeRealValue(ostrm, m_obj.m_real64Value);
				break;
			case CIMDataType::CHAR16:
				(reinterpret_cast<const Char16*>(&m_obj))->writeObject(ostrm);
				break;
			case CIMDataType::STRING:
				(reinterpret_cast<const String*>(&m_obj))->writeObject(ostrm);
				break;
			case CIMDataType::DATETIME:
				(reinterpret_cast<const CIMDateTime*>(&m_obj))->writeObject(ostrm);
				break;
			case CIMDataType::REFERENCE:
				(reinterpret_cast<const CIMObjectPath*>(&m_obj))->writeObject(ostrm);
				break;
			case CIMDataType::EMBEDDEDCLASS:
				(reinterpret_cast<const CIMClass*>(&m_obj))->writeObject(ostrm);
				break;
			case CIMDataType::EMBEDDEDINSTANCE:
				(reinterpret_cast<const CIMInstance*>(&m_obj))->writeObject(ostrm);
				break;
			default:
				OW_ASSERT(0);
		}
	}
}
CIMValue::~CIMValue()
{
}

} // end namespace OpenWBEM

