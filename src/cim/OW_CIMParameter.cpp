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
#include "OW_CIM.hpp"
#include "OW_StringBuffer.hpp"
//#include "OW_XMLNode.hpp"
//#include "OW_XMLParameters.hpp"
#include "OW_MutexLock.hpp"

using std::istream;
using std::ostream;

struct OW_CIMParameter::PARMData
{
	PARMData() :
		m_name(), m_dataType(true), m_qualifiers() {}

	PARMData(const PARMData& x) :
		m_name(x.m_name), m_dataType(x.m_dataType),
		m_qualifiers(x.m_qualifiers) {}

	PARMData& operator= (const PARMData& x)
	{
		m_name = x.m_name;
		m_dataType = x.m_dataType;
		m_qualifiers = x.m_qualifiers;
		return *this;
	}

	OW_String m_name;
	OW_CIMDataType m_dataType;
	OW_CIMQualifierArray m_qualifiers;
};

//////////////////////////////////////////////////////////////////////////////
OW_CIMParameter::OW_CIMParameter(OW_Bool notNull) :
	OW_CIMElement(), m_pdata((notNull) ? new PARMData : NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParameter::OW_CIMParameter(const char* name) :
	OW_CIMElement(), m_pdata(new PARMData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParameter::OW_CIMParameter(const OW_String& name) :
	OW_CIMElement(), m_pdata(new PARMData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParameter::OW_CIMParameter(const OW_CIMParameter& x) :
	OW_CIMElement(), m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParameter::~OW_CIMParameter()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParameter::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMParameter&
OW_CIMParameter::operator= (const OW_CIMParameter& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParameter::setQualifiers(const OW_CIMQualifierArray& quals)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_qualifiers = quals;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierArray
OW_CIMParameter::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParameter::setDataType(const OW_CIMDataType& type)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_dataType = type;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType
OW_CIMParameter::getType() const
{
	return m_pdata->m_dataType;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_CIMParameter::getDataSize() const
{
	return m_pdata->m_dataType.getSize();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier
OW_CIMParameter::getQualifier(const OW_String& name) const
{
	for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		OW_CIMQualifier nq = m_pdata->m_qualifiers[i];
		if(nq.getName().equalsIgnoreCase(name))
		{
			return nq;
		}
	}

	return OW_CIMQualifier();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMParameter::getName() const
{
	return m_pdata->m_name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParameter::setName(const OW_String& name)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParameter::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMPARAMETERSIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_dataType.writeObject(ostrm);
	OW_Int32 len = m_pdata->m_qualifiers.size();
	OW_Int32 nl = OW_hton32(len);
	if(!ostrm.write((const char*)&nl, sizeof(nl)))
		OW_THROW(OW_IOException, "failed to write length of qualifier array");

	for(OW_Int32 i = 0; i < len; i++)
	{
		m_pdata->m_qualifiers[i].writeObject(ostrm);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMParameter::readObject(istream &istrm)
{
	OW_Int32 len;
	OW_String name;
	OW_CIMDataType dataType;
	OW_CIMQualifierArray qualifiers;

	OW_CIMBase::readSig( istrm, OW_CIMPARAMETERSIG );
	name.readObject(istrm);
	dataType.readObject(istrm);

	if(!istrm.read((char*)&len, sizeof(len)))
		OW_THROW(OW_IOException, "failed to read length of qualifier array");

	len = OW_ntoh32(len);

	for(OW_Int32 i = 0; i < len; i++)
	{
		OW_CIMQualifier cq;
		cq.readObject(istrm);
		qualifiers.append(cq);
	}

	if(m_pdata.isNull())
	{
		m_pdata = new PARMData;
	}

	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
	m_pdata->m_dataType = dataType;
	m_pdata->m_qualifiers = qualifiers;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMParameter::toString() const
{
	return "OW_CIMParameter(" + m_pdata->m_name + ")";
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMParameter::toMOF() const
{
	OW_StringBuffer rv;

	if(m_pdata->m_qualifiers.size() > 0)
	{
		rv += '[';

		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			OW_CIMQualifier nq = m_pdata->m_qualifiers[i];

			if(i > 0)
			{
				rv += ',';
			}

			rv += nq.toMOF();
		}

		rv += ']';
	}

	rv += m_pdata->m_dataType.toMOF();
	rv += ' ';
	rv += m_pdata->m_name;
	return rv.toString();
}



