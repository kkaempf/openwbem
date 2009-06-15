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

#include "OW_config.h"
#include "OW_CIMMethod.hpp"
#include "blocxx/StringBuffer.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMName.hpp"
#include "blocxx/Array.hpp"
#include "OW_BinarySerialization.hpp"
#include "blocxx/StrictWeakOrdering.hpp"
#include "blocxx/COWIntrusiveCountableBase.hpp"

namespace OW_NAMESPACE
{

using std::streambuf;
using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////													
struct CIMMethod::METHData : public COWIntrusiveCountableBase
{
	METHData() 
		: m_propagated(false)
	{
	}
	virtual ~METHData();
	CIMName m_name;
	CIMDataType m_returnDatatype;
	CIMQualifierArray m_qualifiers;
	CIMParameterArray m_parameters;
	CIMName m_originClass;
	CIMName m_override;
	Bool m_propagated;
	METHData* clone() const { return new METHData(*this); }
};
//////////////////////////////////////////////////////////////////////////////
CIMMethod::METHData::~METHData()
{
}

//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMMethod::METHData& x, const CIMMethod::METHData& y)
{
	return StrictWeakOrdering(
		x.m_name, y.m_name,
		x.m_returnDatatype, y.m_returnDatatype,
		x.m_qualifiers, y.m_qualifiers,
		x.m_parameters, y.m_parameters,
		x.m_originClass, y.m_originClass,
		x.m_override, y.m_override,
		x.m_propagated, y.m_propagated);
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod::CIMMethod() :
	CIMElement(), m_pdata(new METHData)
{
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod::CIMMethod(CIMNULL_t) :
	CIMElement(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod::CIMMethod(const char* name) :
	CIMElement(), m_pdata(new METHData)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod::CIMMethod(const CIMName& name) :
	CIMElement(), m_pdata(new METHData)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod::CIMMethod(const CIMMethod& x)
	: CIMElement(), m_pdata(x.m_pdata)
{
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod::~CIMMethod()
{
}
//////////////////////////////////////////////////////////////////////////////													
void
CIMMethod::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::operator= (const CIMMethod& x)
{
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::addQualifier(const CIMQualifier& qual)
{
	m_pdata->m_qualifiers.append(qual);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::setQualifiers(const CIMQualifierArray& quals)
{
	m_pdata->m_qualifiers = quals;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
CIMQualifierArray
CIMMethod::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}
//////////////////////////////////////////////////////////////////////////////													
CIMQualifier
CIMMethod::getQualifier(const CIMName& name) const
{
	int tsize = m_pdata->m_qualifiers.size();
	for (int i = 0; i < tsize; i++)
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
CIMMethod::getOriginClass() const
{
	return m_pdata->m_originClass.toString();
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::setOriginClass(const CIMName& originCls)
{
	m_pdata->m_originClass = originCls;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::addParameter(const CIMParameter& param)
{
	m_pdata->m_parameters.append(param);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::setParameters(const CIMParameterArray& inParms)
{
	m_pdata->m_parameters = inParms;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
CIMParameterArray
CIMMethod::getParameters() const
{
	return m_pdata->m_parameters;
}
//////////////////////////////////////////////////////////////////////////////													
CIMParameterArray
CIMMethod::getINParameters() const
{
	CIMParameterArray rval;
	for (size_t i = 0; i < m_pdata->m_parameters.size(); ++i)
	{
		// TODO
		// Note: The following code assumes the default value for the
		// CIM_QUAL_IN qualifier is true. The is per the CIM 2.2 spec.
		// THE SPEC ALSO STATES THAT THE DEFAULT VALUE FOR QUALIFIERS CAN
		// BE CHANGED.
		CIMQualifier q = m_pdata->m_parameters[i].getQualifier(CIMQualifier::CIM_QUAL_IN);
		if (!q || q.getValue() == CIMValue(true))
		{
			rval.push_back(m_pdata->m_parameters[i]);
		}
	}

	return rval;
}
//////////////////////////////////////////////////////////////////////////////													
CIMParameterArray
CIMMethod::getOUTParameters() const
{
	CIMParameterArray rval;
	for (size_t i = 0; i < m_pdata->m_parameters.size(); ++i)
	{
		if (m_pdata->m_parameters[i].hasTrueQualifier(CIMQualifier::CIM_QUAL_OUT))
		{
			rval.push_back(m_pdata->m_parameters[i]);
		}
	}

	return rval;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::setReturnType(const CIMDataType& type)
{
	m_pdata->m_returnDatatype = type;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
CIMDataType
CIMMethod::getReturnType() const
{
	return m_pdata->m_returnDatatype;
}
//////////////////////////////////////////////////////////////////////////////													
Int32
CIMMethod::getReturnDataSize() const
{
	return m_pdata->m_returnDatatype.getSize();
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::setOverridingMethod(const CIMName& omname)
{
	m_pdata->m_override = omname;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
String
CIMMethod::getOverridingMethod() const
{
	return m_pdata->m_override.toString();
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod
CIMMethod::clone(EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin) const
{
	if (!m_pdata)
	{
		return CIMMethod(CIMNULL);
	}
	CIMMethod theMethod;
	theMethod.m_pdata->m_name = m_pdata->m_name;
	theMethod.m_pdata->m_returnDatatype = m_pdata->m_returnDatatype;
	theMethod.m_pdata->m_parameters = m_pdata->m_parameters;
	theMethod.m_pdata->m_override = m_pdata->m_override;
	theMethod.m_pdata->m_propagated = m_pdata->m_propagated;
	if (includeQualifiers)
	{
		theMethod.m_pdata->m_qualifiers = m_pdata->m_qualifiers;
	}
	if (includeClassOrigin)
	{
		theMethod.m_pdata->m_originClass = m_pdata->m_originClass;
	}
	return theMethod;
}
//////////////////////////////////////////////////////////////////////////////													
CIMMethod&
CIMMethod::setPropagated(bool propagated)
{
	m_pdata->m_propagated = propagated;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////													
bool
CIMMethod::getPropagated() const
{
	return m_pdata->m_propagated;
}
//////////////////////////////////////////////////////////////////////////////													
String
CIMMethod::getName() const
{
	return m_pdata->m_name.toString();
}
//////////////////////////////////////////////////////////////////////////////													
void
CIMMethod::setName(const CIMName& name)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////													
void
CIMMethod::readObject(streambuf & istrm)
{
	CIMName name;
	CIMDataType returnDatatype(CIMNULL);
	CIMQualifierArray qualifiers;
	CIMParameterArray parameters;
	CIMName originClass;
	CIMName override;
	Bool propagated;
	CIMBase::readSig( istrm, OW_CIMMETHODSIG );
	name.readObject(istrm);
	returnDatatype.readObject(istrm);
	BinarySerialization::readArray(istrm, qualifiers);
	BinarySerialization::readArray(istrm, parameters);
	originClass.readObject(istrm);
	override.readObject(istrm);
	propagated.readObject(istrm);
	if (!m_pdata)
	{
		m_pdata = new METHData;
	}
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
CIMMethod::writeObject(streambuf & ostrm) const
{
	CIMBase::writeSig( ostrm, OW_CIMMETHODSIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_returnDatatype.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_pdata->m_qualifiers);
	BinarySerialization::writeArray(ostrm, m_pdata->m_parameters);
	m_pdata->m_originClass.writeObject(ostrm);
	m_pdata->m_override.writeObject(ostrm);
	m_pdata->m_propagated.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////													
String
CIMMethod::toMOF() const
{
	StringBuffer rv;
	size_t i;
	if (m_pdata->m_qualifiers.size() > 0)
	{
		rv += '[';
		for (i = 0; i < m_pdata->m_qualifiers.size(); i++)
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
	rv += m_pdata->m_returnDatatype.toMOF();
	rv += ' ';
	rv += m_pdata->m_name.toString();
	rv += '(';
	if (m_pdata->m_parameters.size() > 0)
	{
		for (i = 0; i < m_pdata->m_parameters.size(); i++)
		{
			CIMParameter nq = m_pdata->m_parameters[i];
			if (i > 0)
			{
				rv += ',';
			}
			rv += nq.toMOF();
		}
	}
	rv += ");\n";
	return rv.releaseString();
}
/////////////////////////////////////////////////////////////////////////////													
String
CIMMethod::toString() const
{
	String rv("CIMMethod NAME = ");
	rv += m_pdata->m_name.toString();
	return rv;
}
/////////////////////////////////////////////////////////////////////////////													
bool operator<(const CIMMethod& x, const CIMMethod& y)
{
	return *x.m_pdata < *y.m_pdata;
}

} // end namespace OW_NAMESPACE

