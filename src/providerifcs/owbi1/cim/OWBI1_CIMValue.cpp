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

#include "OWBI1_config.h"
#include "OWBI1_CIMValue.hpp"
#include "OWBI1_CIMValueRep.hpp"
#include "OWBI1_Bool.hpp"
#include "OWBI1_Char16.hpp"
#include "OWBI1_CIMDateTime.hpp"
#include "OWBI1_CIMDateTimeRep.hpp"
#include "OWBI1_CIMObjectPath.hpp"
#include "OWBI1_CIMObjectPathRep.hpp"
#include "OWBI1_CIMClass.hpp"
#include "OWBI1_CIMClassRep.hpp"
#include "OWBI1_CIMInstance.hpp"
#include "OWBI1_CIMInstanceRep.hpp"
#include "OWBI1_CIMDetail.hpp"
#include "OWBI1_ProxyProviderDetail.hpp"
#include "OW_Bool.hpp"
#include "OW_Char16.hpp"
#include "OWBI1_CIMDataType.hpp"
#include "OWBI1_CIMDataTypeRep.hpp"

namespace OWBI1
{

using std::istream;
using std::ostream;
using namespace detail;
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMValue
CIMValue::createSimpleValue(const String& cimtype,
	const String& value)
{
	return CIMValue(new CIMValueRep(OpenWBEM::CIMValue::createSimpleValue(cimtype.c_str(), value.c_str())));
}

//////////////////////////////////////////////////////////////////////////////
void				
CIMValue::readObject(istream &istrm)
{
	m_rep->value.readObject(istrm);
}

//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(CIMNULL_t)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::CIMNULL)))
{
}

//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMValue& x)
	: CIMBase(x)
	, m_rep(x.m_rep) 
{
}

//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Bool x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))){}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(bool x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))){}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(UInt8 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Int8 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(UInt16 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Int16 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(UInt32 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Int32 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(UInt64 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Int64 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Real32 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(Real64 x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Char16& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const String& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x.c_str()))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const char* x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::String(x)))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMDateTime& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x.getRep()->datetime))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMObjectPath& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x.getRep()->objectpath))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMClass& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x.getRep()->cls))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMInstance& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(x.getRep()->inst))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMObjectPathArray& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(unwrapArray<OpenWBEM::CIMObjectPathArray>(x)))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const BoolArray& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::BoolArray(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Char16Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::Char16Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const UInt8Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::UInt8Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Int8Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::Int8Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const UInt16Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::UInt16Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Int16Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::Int16Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const UInt32Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::UInt32Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Int32Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::Int32Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const UInt64Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::UInt64Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Int64Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::Int64Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Real64Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::Real64Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const Real32Array& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(OpenWBEM::Real32Array(x.begin(), x.end())))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const StringArray& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(convertStringArray(x)))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMDateTimeArray& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(unwrapArray<OpenWBEM::CIMDateTimeArray>(x)))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMClassArray& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(unwrapArray<OpenWBEM::CIMClassArray>(x)))) {}
//////////////////////////////////////////////////////////////////////////////
CIMValue::CIMValue(const CIMInstanceArray& x)
	: m_rep(new CIMValueRep(OpenWBEM::CIMValue(unwrapArray<OpenWBEM::CIMInstanceArray>(x)))) {}
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMValue::getArraySize() const
{
	return m_rep->value.getArraySize();
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Bool& x) const
{
	OpenWBEM::Bool owb;
	m_rep->value.get(owb);
	x = static_cast<bool>(owb);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Char16& x) const
{
	OpenWBEM::Char16 owc;
	m_rep->value.get(owc);
	x = owc.getValue();
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt8& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int8& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt16& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int16& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt32& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int32& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt64& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int64& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Real32& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Real64& x) const
{
	m_rep->value.get(x);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(String& x) const
{
	OpenWBEM::String ows;
	m_rep->value.get(ows);
	x = ows.c_str();
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMDateTime& x) const
{
	OpenWBEM::CIMDateTime owcdt(OpenWBEM::CIMNULL);
	m_rep->value.get(owcdt);
	x = CIMDateTime(new CIMDateTimeRep(owcdt));
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMObjectPath& x) const
{
	OpenWBEM::CIMObjectPath owcop(OpenWBEM::CIMNULL);
	m_rep->value.get(owcop);
	x = CIMObjectPath(new CIMObjectPathRep(owcop));
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMClass& x) const
{
	OpenWBEM::CIMClass owcls(OpenWBEM::CIMNULL);
	m_rep->value.get(owcls);
	x = CIMClass(new CIMClassRep(owcls));
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMInstance& x) const
{
	OpenWBEM::CIMInstance owinst(OpenWBEM::CIMNULL);
	m_rep->value.get(owinst);
	x = CIMInstance(new CIMInstanceRep(owinst));
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Char16Array& x) const
{
	OpenWBEM::Char16Array ow;
	m_rep->value.get(ow);
	x = Char16Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt8Array& x) const
{
	OpenWBEM::UInt8Array ow;
	m_rep->value.get(ow);
	x = UInt8Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int8Array& x) const
{
	OpenWBEM::Int8Array ow;
	m_rep->value.get(ow);
	x = Int8Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt16Array& x) const
{
	OpenWBEM::UInt16Array ow;
	m_rep->value.get(ow);
	x = UInt16Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int16Array& x) const
{
	OpenWBEM::Int16Array ow;
	m_rep->value.get(ow);
	x = Int16Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt32Array& x) const
{
	OpenWBEM::UInt32Array ow;
	m_rep->value.get(ow);
	x = UInt32Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int32Array& x) const
{
	OpenWBEM::Int32Array ow;
	m_rep->value.get(ow);
	x = Int32Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(UInt64Array& x) const
{
	OpenWBEM::UInt64Array ow;
	m_rep->value.get(ow);
	x = UInt64Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Int64Array& x) const
{
	OpenWBEM::Int64Array ow;
	m_rep->value.get(ow);
	x = Int64Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Real64Array& x) const
{
	OpenWBEM::Real64Array ow;
	m_rep->value.get(ow);
	x = Real64Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(Real32Array& x) const
{
	OpenWBEM::Real32Array ow;
	m_rep->value.get(ow);
	x = Real32Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(StringArray& x) const
{
	OpenWBEM::StringArray ow;
	m_rep->value.get(ow);
	x = convertStringArray(ow);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMObjectPathArray& x) const
{
	OpenWBEM::CIMObjectPathArray ow;
	m_rep->value.get(ow);
	wrapArray(x, ow);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(BoolArray& x) const
{
	OpenWBEM::BoolArray ow;
	m_rep->value.get(ow);
	x = BoolArray(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMDateTimeArray& x) const
{
	OpenWBEM::CIMDateTimeArray ow;
	m_rep->value.get(ow);
	wrapArray(x, ow);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMClassArray& x) const
{
	OpenWBEM::CIMClassArray ow;
	m_rep->value.get(ow);
	wrapArray(x, ow);
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::get(CIMInstanceArray& x) const
{
	OpenWBEM::CIMInstanceArray ow;
	m_rep->value.get(ow);
	wrapArray(x, ow);
}
//////////////////////////////////////////////////////////////////////////////
Bool CIMValue::toBool() const
{
	return static_cast<bool>(m_rep->value.toBool());
}
//////////////////////////////////////////////////////////////////////////////
Char16 CIMValue::toChar16() const
{
	return m_rep->value.toChar16().getValue();
}
//////////////////////////////////////////////////////////////////////////////
UInt8 CIMValue::toUInt8() const
{
	return m_rep->value.toUInt8();
}
//////////////////////////////////////////////////////////////////////////////
Int8 CIMValue::toInt8() const
{
	return m_rep->value.toInt8();
}
//////////////////////////////////////////////////////////////////////////////
UInt16 CIMValue::toUInt16() const
{
	return m_rep->value.toUInt16();
}
//////////////////////////////////////////////////////////////////////////////
Int16 CIMValue::toInt16() const
{
	return m_rep->value.toInt16();
}
//////////////////////////////////////////////////////////////////////////////
UInt32 CIMValue::toUInt32() const
{
	return m_rep->value.toUInt32();
}
//////////////////////////////////////////////////////////////////////////////
Int32 CIMValue::toInt32() const
{
	return m_rep->value.toInt32();
}
//////////////////////////////////////////////////////////////////////////////
UInt64 CIMValue::toUInt64() const
{
	return m_rep->value.toUInt64();
}
//////////////////////////////////////////////////////////////////////////////
Int64 CIMValue::toInt64() const
{
	return m_rep->value.toInt64();
}
//////////////////////////////////////////////////////////////////////////////
Real32 CIMValue::toReal32() const
{
	return m_rep->value.toReal32();
}
//////////////////////////////////////////////////////////////////////////////
Real64 CIMValue::toReal64() const
{
	return m_rep->value.toReal64();
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime CIMValue::toCIMDateTime() const
{
	return CIMDateTime(new CIMDateTimeRep(m_rep->value.toCIMDateTime()));
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath CIMValue::toCIMObjectPath() const
{
	return CIMObjectPath(new CIMObjectPathRep(m_rep->value.toCIMObjectPath()));
}
//////////////////////////////////////////////////////////////////////////////
CIMClass CIMValue::toCIMClass() const
{
	return CIMClass(new CIMClassRep(m_rep->value.toCIMClass()));
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance CIMValue::toCIMInstance() const
{
	return CIMInstance(new CIMInstanceRep(m_rep->value.toCIMInstance()));
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathArray CIMValue::toCIMObjectPathArray() const
{
	return wrapArray<CIMObjectPathArray>(m_rep->value.toCIMObjectPathArray());
}
//////////////////////////////////////////////////////////////////////////////
Char16Array CIMValue::toChar16Array() const
{
	OpenWBEM::Char16Array ow(m_rep->value.toChar16Array());
	return Char16Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
UInt8Array CIMValue::toUInt8Array() const
{
	OpenWBEM::UInt8Array ow(m_rep->value.toUInt8Array());
	return UInt8Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
Int8Array CIMValue::toInt8Array() const
{
	OpenWBEM::Int8Array ow(m_rep->value.toInt8Array());
	return Int8Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
UInt16Array CIMValue::toUInt16Array() const
{
	OpenWBEM::UInt16Array ow(m_rep->value.toUInt16Array());
	return UInt16Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
Int16Array CIMValue::toInt16Array() const
{
	OpenWBEM::Int16Array ow(m_rep->value.toInt16Array());
	return Int16Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
UInt32Array CIMValue::toUInt32Array() const
{
	OpenWBEM::UInt32Array ow(m_rep->value.toUInt32Array());
	return UInt32Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
Int32Array CIMValue::toInt32Array() const
{
	OpenWBEM::Int32Array ow(m_rep->value.toInt32Array());
	return Int32Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
UInt64Array CIMValue::toUInt64Array() const
{
	OpenWBEM::UInt64Array ow(m_rep->value.toUInt64Array());
	return UInt64Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
Int64Array CIMValue::toInt64Array() const
{
	OpenWBEM::Int64Array ow(m_rep->value.toInt64Array());
	return Int64Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
Real64Array CIMValue::toReal64Array() const
{
	OpenWBEM::Real64Array ow(m_rep->value.toReal64Array());
	return Real64Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
Real32Array CIMValue::toReal32Array() const
{
	OpenWBEM::Real32Array ow(m_rep->value.toReal32Array());
	return Real32Array(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
StringArray CIMValue::toStringArray() const
{
	return convertStringArray(m_rep->value.toStringArray());
}
//////////////////////////////////////////////////////////////////////////////
BoolArray CIMValue::toBoolArray() const
{
	OpenWBEM::BoolArray ow(m_rep->value.toBoolArray());
	return BoolArray(ow.begin(), ow.end());
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTimeArray CIMValue::toCIMDateTimeArray() const
{
	return wrapArray<CIMDateTimeArray>(m_rep->value.toCIMDateTimeArray());
}
//////////////////////////////////////////////////////////////////////////////
CIMClassArray CIMValue::toCIMClassArray() const
{
	return wrapArray<CIMClassArray>(m_rep->value.toCIMClassArray());
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray CIMValue::toCIMInstanceArray() const
{
	return wrapArray<CIMInstanceArray>(m_rep->value.toCIMInstanceArray());
}
//////////////////////////////////////////////////////////////////////////////
void CIMValue::setNull()
{
	m_rep->value.setNull();
}
//////////////////////////////////////////////////////////////////////////////
CIMValue& CIMValue::operator= (const CIMValue& x)
{
	m_rep = x.m_rep;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::equal(const CIMValue& x) const
{
	return m_rep->value.equal(x.m_rep->value);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator== (const CIMValue& x) const
{
	return equal(x);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator!= (const CIMValue& x) const
{
	return !(*this == x);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator<= (const CIMValue& x) const
{
	return !(x < *this);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator>= (const CIMValue& x) const
{
	return !(*this < x);
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator< (const CIMValue& x) const
{
	return m_rep->value < x.m_rep->value;
}
//////////////////////////////////////////////////////////////////////////////
bool CIMValue::operator> (const CIMValue& x) const
{
	return x < *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::Type CIMValue::getType() const 
{  
	return CIMDataType::Type(m_rep->value.getType());
}

//////////////////////////////////////////////////////////////////////////////
CIMDataType CIMValue::getCIMDataType() const 
{ 
	return CIMDataType(new CIMDataTypeRep(m_rep->value.getCIMDataType())); 
}

//////////////////////////////////////////////////////////////////////////////
bool CIMValue::sameType(const CIMValue& x) const
{
	return m_rep->value.sameType(x.m_rep->value);
}

//////////////////////////////////////////////////////////////////////////////
bool CIMValue::isArray() const 
{  
	return m_rep->value.isArray(); 
}

//////////////////////////////////////////////////////////////////////////////
void CIMValue::writeObject(std::ostream &ostrm) const
{
	m_rep->value.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String CIMValue::toString() const
{
	return m_rep->value.toString().c_str();
}
//////////////////////////////////////////////////////////////////////////////
String CIMValue::toMOF() const
{
	return m_rep->value.toMOF().c_str();
}

//////////////////////////////////////////////////////////////////////////////
bool
CIMValue::isNumericType() const
{
	return m_rep->value.isNumericType();
}

//////////////////////////////////////////////////////////////////////////////
CIMValue::~CIMValue()
{
}

//////////////////////////////////////////////////////////////////////////////
CIMValue::operator safe_bool () const
{  
	return m_rep->value ? &CIMValue::m_rep : 0; 
}

//////////////////////////////////////////////////////////////////////////////
bool 
CIMValue::operator!() const
{  
	return !(m_rep->value); 
}

} // end namespace OWBI1

