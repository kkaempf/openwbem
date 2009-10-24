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
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMParameter.hpp"
#include "blocxx/StringBuffer.hpp"
#include "OW_BinarySerialization.hpp"
#include "blocxx/StrictWeakOrdering.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMName.hpp"
#include "blocxx/COWIntrusiveCountableBase.hpp"

namespace OW_NAMESPACE
{

using std::streambuf;
//////////////////////////////////////////////////////////////////////////////
struct CIMParameter::PARMData : public COWIntrusiveCountableBase
{
	virtual ~PARMData();
	CIMName m_name;
	CIMDataType m_dataType;
	CIMQualifierArray m_qualifiers;
	PARMData* clone() const { return new PARMData(*this); }
};
//////////////////////////////////////////////////////////////////////////////
CIMParameter::PARMData::~PARMData()
{
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMParameter::PARMData& x, const CIMParameter::PARMData& y)
{
	return StrictWeakOrdering(
		x.m_name, y.m_name,
		x.m_dataType, y.m_dataType,
		x.m_qualifiers, y.m_qualifiers);
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter::CIMParameter() :
	CIMElement(), m_pdata(new PARMData)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter::CIMParameter(CIMNULL_t) :
	CIMElement(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter::CIMParameter(const char* name) :
	CIMElement(), m_pdata(new PARMData)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter::CIMParameter(const CIMName& name) :
	CIMElement(), m_pdata(new PARMData)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter::CIMParameter(const CIMParameter& x) :
	CIMElement(), m_pdata(x.m_pdata)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter::~CIMParameter()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMParameter::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter&
CIMParameter::operator= (const CIMParameter& x)
{
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter&
CIMParameter::setQualifiers(const CIMQualifierArray& quals)
{
	m_pdata->m_qualifiers = quals;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierArray
CIMParameter::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}
//////////////////////////////////////////////////////////////////////////////
CIMParameter&
CIMParameter::setDataType(const CIMDataType& type)
{
	m_pdata->m_dataType = type;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType
CIMParameter::getType() const
{
	return m_pdata->m_dataType;
}
//////////////////////////////////////////////////////////////////////////////
Int32
CIMParameter::getDataSize() const
{
	return m_pdata->m_dataType.getSize();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier
CIMParameter::getQualifier(const CIMName& name) const
{
	for (size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		CIMQualifier nq = m_pdata->m_qualifiers[i];
		if (nq.getName() == name)
		{
			return nq;
		}
	}
	return CIMQualifier(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMParameter::getName() const
{
	return m_pdata->m_name.toString();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMParameter::setName(const CIMName& name)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMParameter::writeObject(streambuf & ostrm) const
{
	CIMBase::writeSig( ostrm, OW_CIMPARAMETERSIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_dataType.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_pdata->m_qualifiers);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMParameter::readObject(streambuf & istrm)
{
	CIMName name;
	CIMDataType dataType(CIMNULL);
	CIMQualifierArray qualifiers;
	CIMBase::readSig( istrm, OW_CIMPARAMETERSIG );
	name.readObject(istrm);
	dataType.readObject(istrm);
	BinarySerialization::readArray(istrm, qualifiers);
	if (!m_pdata)
	{
		m_pdata = new PARMData;
	}
	m_pdata->m_name = name;
	m_pdata->m_dataType = dataType;
	m_pdata->m_qualifiers = qualifiers;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMParameter::toString() const
{
	return "CIMParameter(" + m_pdata->m_name.toString() + ")";
}
//////////////////////////////////////////////////////////////////////////////
String
CIMParameter::toMOF() const
{
	StringBuffer rv;
	if (m_pdata->m_qualifiers.size() > 0)
	{
		rv += '[';
		for (size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			CIMQualifier nq = m_pdata->m_qualifiers[i];
			if (i > 0)
			{
				rv += ',';
			}
			rv += nq.toMOF();
		}
		rv += ']';
	}
	rv += m_pdata->m_dataType.toMOF();
	rv += ' ';
	rv += m_pdata->m_name.toString();
	if (m_pdata->m_dataType.isArrayType())
	{
		rv += '[';
		int arraySize = m_pdata->m_dataType.getSize();
		if (arraySize != -1)
		{
			rv += arraySize;
		}
		rv += ']';
	}
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMParameter& x, const CIMParameter& y)
{
	return *x.m_pdata < *y.m_pdata;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMParameter::hasTrueQualifier(const CIMName& name) const
{
	CIMQualifier q = getQualifier(name);
	if (!q)
	{
		return false;
	}
	CIMValue v = q.getValue();
	if (!v)
	{
		return false;
	}
	if (v.getType() != CIMDataType::BOOLEAN)
	{
		return false;
	}
	Bool b;
	v.get(b);
	return b;
}

} // end namespace OW_NAMESPACE

