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
#include "OWBI1_CIMDataType.hpp"
#include "OWBI1_CIMDataTypeRep.hpp"
#include "OWBI1_CIMValue.hpp"
#include "OWBI1_CIMValueRep.hpp"
#include "OWBI1_CIMName.hpp"
#include "OWBI1_CIMNameRep.hpp"

namespace OWBI1
{
using namespace detail;
using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType() :
	CIMBase(), m_rep(new CIMDataTypeRep(OpenWBEM::CIMDataType()))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(CIMNULL_t) :
	CIMBase(), m_rep(new CIMDataTypeRep(OpenWBEM::CIMDataType(OpenWBEM::CIMNULL)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(CIMDataType::Type type) :
	CIMBase(), m_rep(new CIMDataTypeRep(OpenWBEM::CIMDataType(OpenWBEM::CIMDataType::Type(type))))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(CIMDataType::Type type, Int32 size) :
	CIMBase(), m_rep(new CIMDataTypeRep(OpenWBEM::CIMDataType(OpenWBEM::CIMDataType::Type(type), size)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(const CIMDataType& arg) :
	CIMBase(), m_rep(arg.m_rep) {}

//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(const CIMDataTypeRepRef& rep)
	: m_rep(rep)
{
}

//////////////////////////////////////////////////////////////////////////////
CIMDataType::~CIMDataType()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDataType::setNull()
{
	m_rep->datatype.setNull();
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType&
CIMDataType::operator = (const CIMDataType& arg)
{
	m_rep = arg.m_rep;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::isArrayType() const
{
	return m_rep->datatype.isArrayType();
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::isNumericType() const
{
	return m_rep->datatype.isNumericType();
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
CIMDataType::isNumericType(CIMDataType::Type type)
{
	return OpenWBEM::CIMDataType::isNumericType(OpenWBEM::CIMDataType::Type(type));
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::isReferenceType() const
{
	return m_rep->datatype.isReferenceType();
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::Type
CIMDataType::getType() const
{
	return CIMDataType::Type(m_rep->datatype.getType());
}
//////////////////////////////////////////////////////////////////////////////
Int32
CIMDataType::getSize() const
{
	return m_rep->datatype.getSize();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMDataType::getRefClassName() const
{
	return m_rep->datatype.getRefClassName().c_str();
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::operator CIMDataType::safe_bool () const
{
	return m_rep->datatype ? &CIMDataType::m_rep : 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::operator!() const
{
	return !m_rep->datatype;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::setToArrayType(Int32 size)
{
	return m_rep->datatype.setToArrayType(size);
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::syncWithValue(const CIMValue& value)
{
	return m_rep->datatype.syncWithValue(value.getRep()->value);
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(const CIMName& refClassName) 
	: m_rep(new CIMDataTypeRep(OpenWBEM::CIMDataType(refClassName.getRep()->name)))
{
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::equals(const CIMDataType& arg) const
{
	return m_rep->datatype.equals(arg.m_rep->datatype);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDataType::readObject(istream &istrm)
{
	m_rep->datatype.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDataType::writeObject(ostream &ostrm) const
{
	m_rep->datatype.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMDataType::toString() const
{
	return m_rep->datatype.toString().c_str();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMDataType::toMOF() const
{
	return m_rep->datatype.toMOF().c_str();
}

//////////////////////////////////////////////////////////////////////////////
String
CIMDataType::getArrayMOF() const
{
	return m_rep->datatype.getArrayMOF().c_str();
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMDataType::Type
CIMDataType::strToSimpleType(const String& strType)
{
	return CIMDataType::Type(OpenWBEM::CIMDataType::strToSimpleType(strType.c_str()));
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMDataType
CIMDataType::getDataType(const String& strType)
{
	return CIMDataType(new CIMDataTypeRep(OpenWBEM::CIMDataType::getDataType(strType.c_str())));
}

//////////////////////////////////////////////////////////////////////////////
CIMDataTypeRepRef
CIMDataType::getRep() const
{
	return m_rep;
}

//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMDataType& x, const CIMDataType& y)
{
	return *x.m_rep < *y.m_rep;
}

//////////////////////////////////////////////////////////////////////////////
bool operator==(const CIMDataType& x, const CIMDataType& y)
{
	return *x.m_rep == *y.m_rep;
}

//////////////////////////////////////////////////////////////////////////////
bool operator<=(const CIMDataType& x, const CIMDataType& y)
{
    return !(y < x);
}

//////////////////////////////////////////////////////////////////////////////
bool operator>(const CIMDataType& x, const CIMDataType& y)
{
    return y < x;
}

//////////////////////////////////////////////////////////////////////////////
bool operator>=(const CIMDataType& x, const CIMDataType& y)
{
    return !(x < y);
}

//////////////////////////////////////////////////////////////////////////////
bool operator!=(const CIMDataType& x, const CIMDataType& y)
{
	return !(x == y);
}

} // end namespace OWBI1

