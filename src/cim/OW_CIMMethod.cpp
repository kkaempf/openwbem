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
#include "OW_CIMMethod.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_Array.hpp"
#include "OW_BinIfcIO.hpp"

using std::ostream;
using std::istream;

struct OW_CIMMethod::METHData
{
	METHData() :
		m_name(), m_returnDatatype(true), m_qualifiers(), m_parameters(),
		m_originClass(), m_override(), m_propagated(false)
	{
	}

	METHData(const METHData& x) :
		m_name(x.m_name), m_returnDatatype(x.m_returnDatatype),
		m_qualifiers(x.m_qualifiers), m_parameters(x.m_parameters),
		m_originClass(x.m_originClass), m_override(x.m_override),
		m_propagated(x.m_propagated)
	{
	}

	METHData& operator= (const METHData& x)
	{
		m_name = x.m_name;
		m_returnDatatype = x.m_returnDatatype;
		m_qualifiers = x.m_qualifiers;
		m_parameters = x.m_parameters;
		m_originClass = x.m_originClass;
		m_override = x.m_override;
		m_propagated = x.m_propagated;
		return *this;
	}


	OW_String m_name;
	OW_CIMDataType m_returnDatatype;
	OW_CIMQualifierArray m_qualifiers;
	OW_CIMParameterArray m_parameters;
	OW_String m_originClass;
	OW_String m_override;
	OW_Bool m_propagated;
};

//////////////////////////////////////////////////////////////////////////////													
OW_CIMMethod::OW_CIMMethod(OW_Bool notNull) :
	OW_CIMElement(), m_pdata((notNull) ? new METHData : NULL)
{
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMMethod::OW_CIMMethod(const char* name) :
	OW_CIMElement(), m_pdata(new METHData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMMethod::OW_CIMMethod(const OW_String& name) :
	OW_CIMElement(), m_pdata(new METHData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMMethod::OW_CIMMethod(const OW_CIMMethod& x)
	: OW_CIMElement(), m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMMethod::~OW_CIMMethod()
{
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMMethod&
OW_CIMMethod::operator= (const OW_CIMMethod& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::addQualifier(const OW_CIMQualifier& qual)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_qualifiers.append(qual);
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::setQualifiers(const OW_CIMQualifierArray& quals)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_qualifiers = quals;
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMQualifierArray
OW_CIMMethod::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMQualifier
OW_CIMMethod::getQualifier(const OW_String& name) const
{
	int tsize = m_pdata->m_qualifiers.size();
	for(int i = 0; i < tsize; i++)
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
OW_CIMMethod::getOriginClass() const
{
	return m_pdata->m_originClass;
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::setOriginClass(const OW_String& originCls)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_originClass = originCls;
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::addParameter(const OW_CIMParameter& param)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_parameters.append(param);
}


//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::setParameters(const OW_CIMParameterArray& inParms)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_parameters = inParms;
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMParameterArray
OW_CIMMethod::getParameters() const
{
	return m_pdata->m_parameters;
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::setReturnType(const OW_CIMDataType& type)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_returnDatatype = type;
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMDataType
OW_CIMMethod::getReturnType() const
{
	return m_pdata->m_returnDatatype;
}

//////////////////////////////////////////////////////////////////////////////													
OW_Int32
OW_CIMMethod::getReturnDataSize() const
{
	return m_pdata->m_returnDatatype.getSize();
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::setOverridingMethod(const OW_String& omname)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_override = omname;
}

//////////////////////////////////////////////////////////////////////////////													
OW_String
OW_CIMMethod::getOverridingMethod() const
{
	return m_pdata->m_override;
}

//////////////////////////////////////////////////////////////////////////////													
OW_CIMMethod
OW_CIMMethod::clone(OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin) const
{
	if(m_pdata.isNull())
	{
		return OW_CIMMethod();
	}

	OW_CIMMethod theMethod(true);
	theMethod.m_pdata->m_name = m_pdata->m_name;
	theMethod.m_pdata->m_returnDatatype = m_pdata->m_returnDatatype;
	theMethod.m_pdata->m_parameters = m_pdata->m_parameters;
	theMethod.m_pdata->m_override = m_pdata->m_override;
	theMethod.m_pdata->m_propagated = m_pdata->m_propagated;

	if(includeQualifiers)
	{
		theMethod.m_pdata->m_qualifiers = m_pdata->m_qualifiers;
	}

	if(includeClassOrigin)
	{
		theMethod.m_pdata->m_originClass = m_pdata->m_originClass;
	}

	return theMethod;
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::setPropagated(OW_Bool propagated)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_propagated = propagated;
}

//////////////////////////////////////////////////////////////////////////////													
OW_Bool
OW_CIMMethod::getPropagated() const
{
	return m_pdata->m_propagated;
}

//////////////////////////////////////////////////////////////////////////////													
OW_String
OW_CIMMethod::getName() const
{
	return m_pdata->m_name;
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::setName(const OW_String& name)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::readObject(istream &istrm)
{
	OW_String name;
	OW_CIMDataType returnDatatype;
	OW_CIMQualifierArray qualifiers;
	OW_CIMParameterArray parameters;
	OW_String originClass;
	OW_String override;
	OW_Bool propagated;

	OW_CIMBase::readSig( istrm, OW_CIMMETHODSIG );
	name.readObject(istrm);
	returnDatatype.readObject(istrm);
	qualifiers.readObject(istrm);
	parameters.readObject(istrm);
	originClass.readObject(istrm);
	override.readObject(istrm);
	propagated.readObject(istrm);

	if(m_pdata.isNull())
	{
		m_pdata = new METHData;
	}

	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
	m_pdata->m_originClass = originClass;
	m_pdata->m_override = override;
	m_pdata->m_parameters = parameters;
	m_pdata->m_propagated = propagated;
	m_pdata->m_qualifiers = qualifiers;
	m_pdata->m_returnDatatype = returnDatatype;
}

//////////////////////////////////////////////////////////////////////////////													
void
OW_CIMMethod::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMMETHODSIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_returnDatatype.writeObject(ostrm);
	m_pdata->m_qualifiers.writeObject(ostrm);
	m_pdata->m_parameters.writeObject(ostrm);
	m_pdata->m_originClass.writeObject(ostrm);
	m_pdata->m_override.writeObject(ostrm);
	m_pdata->m_propagated.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////													
OW_String
OW_CIMMethod::toMOF() const
{
	OW_StringBuffer rv;
	size_t i;

	if(m_pdata->m_qualifiers.size() > 0)
	{
		rv += '[';

		for(i = 0; i < m_pdata->m_qualifiers.size(); i++)
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

	rv += m_pdata->m_returnDatatype.toMOF();
	rv += ' ';
	rv += m_pdata->m_name;
	rv += '(';

	if(m_pdata->m_parameters.size() > 0)
	{
		for(i = 0; i < m_pdata->m_parameters.size(); i++)
		{
			OW_CIMParameter nq = m_pdata->m_parameters[i];
			if(i > 0)
			{
				rv += ',';
			}
			rv += nq.toMOF();
		}
	}
	rv += ");\n";
	return rv.toString();
}

/////////////////////////////////////////////////////////////////////////////													
OW_String
OW_CIMMethod::toString() const
{
	OW_String rv("OW_CIMMethod NAME = ");
	rv += m_pdata->m_name;
	return rv;
}


