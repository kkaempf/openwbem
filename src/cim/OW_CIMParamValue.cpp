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

#include "OW_config.h"
#include "OW_CIMParamValue.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_StrictWeakOrdering.hpp"

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
struct OW_CIMParamValue::Data
{
	Data()
		: m_val(OW_CIMNULL)
	{}

	OW_String m_name;
	OW_CIMValue m_val;

    Data* clone() const { return new Data(*this); }
};

//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMParamValue::Data& x, const OW_CIMParamValue::Data& y)
{
	return OW_StrictWeakOrdering(
		x.m_name, y.m_name,
		x.m_val, y.m_val);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue::OW_CIMParamValue() :
	m_pdata(new Data)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue::OW_CIMParamValue(OW_CIMNULL_t) :
	m_pdata(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue::OW_CIMParamValue(const OW_String& name) :
	m_pdata(new Data)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue::OW_CIMParamValue(const OW_String& name, const OW_CIMValue& val) :
	m_pdata(new Data)
{
	m_pdata->m_name = name;
	m_pdata->m_val = val;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue::OW_CIMParamValue(const OW_CIMParamValue& x) :
	OW_CIMBase(x), m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue::~OW_CIMParamValue()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue&
OW_CIMParamValue::operator= (const OW_CIMParamValue& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMParamValue::getName() const
{
	return m_pdata->m_name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue&
OW_CIMParamValue::setName(const OW_String& name)
{
	m_pdata->m_name = name;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMParamValue::getValue() const
{
	return m_pdata->m_val;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParamValue&
OW_CIMParamValue::setValue(const OW_CIMValue& val)
{
	m_pdata->m_val = val;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParamValue::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParamValue::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMPARAMVALUESIG );
	m_pdata->m_name.writeObject(ostrm);
	if (m_pdata->m_val)
	{
		OW_Bool(true).writeObject(ostrm);
		m_pdata->m_val.writeObject(ostrm);
	}
	else
	{
		OW_Bool(false).writeObject(ostrm);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParamValue::readObject(istream &istrm)
{
	OW_String name;
	OW_CIMValue val(OW_CIMNULL);

	OW_CIMBase::readSig( istrm, OW_CIMPARAMVALUESIG );
	name.readObject(istrm);
	OW_Bool b;
	b.readObject(istrm);
	if (b)
	{
		val.readObject(istrm);
	}

	m_pdata->m_name = name;
	m_pdata->m_val = val;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMParamValue::toString() const
{
	return "OW_CIMParamValue(" + m_pdata->m_name + "): " + m_pdata->m_val.toString();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMParamValue::toMOF() const
{
	return "ERROR: OW_CIMParamValue cannot be converted to MOF";
}


//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMParamValue& x, const OW_CIMParamValue& y)
{
	return *x.m_pdata < *y.m_pdata;
}
