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
#include "OW_CIMParamValue.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_StrictWeakOrdering.hpp"

namespace OpenWBEM
{

using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////
struct CIMParamValue::Data
{
	Data()
		: m_val(CIMNULL)
	{}
	String m_name;
	CIMValue m_val;
	Data* clone() const { return new Data(*this); }
};
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMParamValue::Data& x, const CIMParamValue::Data& y)
{
	return StrictWeakOrdering(
		x.m_name, y.m_name,
		x.m_val, y.m_val);
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue::CIMParamValue() :
	m_pdata(new Data)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue::CIMParamValue(CIMNULL_t) :
	m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue::CIMParamValue(const String& name) :
	m_pdata(new Data)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue::CIMParamValue(const String& name, const CIMValue& val) :
	m_pdata(new Data)
{
	m_pdata->m_name = name;
	m_pdata->m_val = val;
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue::CIMParamValue(const CIMParamValue& x) :
	CIMBase(x), m_pdata(x.m_pdata)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue::~CIMParamValue()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue&
CIMParamValue::operator= (const CIMParamValue& x)
{
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMParamValue::getName() const
{
	return m_pdata->m_name;
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue&
CIMParamValue::setName(const String& name)
{
	m_pdata->m_name = name;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMParamValue::getValue() const
{
	return m_pdata->m_val;
}
//////////////////////////////////////////////////////////////////////////////
CIMParamValue&
CIMParamValue::setValue(const CIMValue& val)
{
	m_pdata->m_val = val;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMParamValue::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMParamValue::writeObject(ostream &ostrm) const
{
	CIMBase::writeSig( ostrm, OW_CIMPARAMVALUESIG );
	m_pdata->m_name.writeObject(ostrm);
	if (m_pdata->m_val)
	{
		Bool(true).writeObject(ostrm);
		m_pdata->m_val.writeObject(ostrm);
	}
	else
	{
		Bool(false).writeObject(ostrm);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMParamValue::readObject(istream &istrm)
{
	String name;
	CIMValue val(CIMNULL);
	CIMBase::readSig( istrm, OW_CIMPARAMVALUESIG );
	name.readObject(istrm);
	Bool b;
	b.readObject(istrm);
	if (b)
	{
		val.readObject(istrm);
	}
	m_pdata->m_name = name;
	m_pdata->m_val = val;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMParamValue::toString() const
{
	return "CIMParamValue(" + m_pdata->m_name + "): " + m_pdata->m_val.toString();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMParamValue::toMOF() const
{
	return "ERROR: CIMParamValue cannot be converted to MOF";
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMParamValue& x, const CIMParamValue& y)
{
	return *x.m_pdata < *y.m_pdata;
}

} // end namespace OpenWBEM

