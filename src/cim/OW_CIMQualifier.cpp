/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_CIMQualifier.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_Assertion.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include "OW_NULLValueException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMFlavor.hpp"

namespace OpenWBEM
{

using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////
struct CIMQualifier::QUALData
{
	QUALData() 
		: m_qualifierValue(CIMNULL)
		, m_propagated(false) 
	{}
	String m_name;
	CIMValue m_qualifierValue;
	CIMQualifierType m_qualifierType;
	Bool m_propagated;
	CIMFlavorArray m_flavors;
	QUALData* clone() const { return new QUALData(*this); }
};
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMQualifier::QUALData& x, const CIMQualifier::QUALData& y)
{
	return StrictWeakOrdering(
		x.m_name, y.m_name,
		x.m_qualifierValue, y.m_qualifierValue,
		x.m_qualifierType, y.m_qualifierType,
		x.m_propagated, y.m_propagated,
		x.m_flavors, y.m_flavors);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier() :
	CIMElement(), m_pdata(new QUALData)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(CIMNULL_t) :
	CIMElement(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const String& name) :
	CIMElement(), m_pdata(new QUALData)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const char* name) :
	CIMElement(), m_pdata(new QUALData)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const CIMQualifierType& cgt) :
	CIMElement(), m_pdata(new QUALData)
{
	m_pdata->m_name = cgt.getName();
	m_pdata->m_qualifierType = cgt;
	m_pdata->m_qualifierValue = cgt.getDefaultValue();
	m_pdata->m_flavors = cgt.getFlavors();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const CIMQualifier& x) :
	CIMElement(), m_pdata(x.m_pdata)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::~CIMQualifier()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifier::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::operator= (const CIMQualifier& x)
{
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::isKeyQualifier() const
{
	return m_pdata->m_name.equalsIgnoreCase(CIMQualifier::CIM_QUAL_KEY);
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::isAssociationQualifier() const
{
	return m_pdata->m_name.equalsIgnoreCase(
		CIMQualifier::CIM_QUAL_ASSOCIATION);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMQualifier::getValue() const
{
	return m_pdata->m_qualifierValue;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMQualifier::getValueT() const
{
	if (!m_pdata->m_qualifierValue)
	{
		OW_THROW(NULLValueException, "");
	}
	return m_pdata->m_qualifierValue;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::setValue(const CIMValue& value)
{
	m_pdata->m_qualifierValue = value;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::setDefaults(const CIMQualifierType& qtype)
{
	m_pdata->m_qualifierType = qtype;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
CIMQualifier::getDefaults() const
{
	return m_pdata->m_qualifierType;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::hasFlavor(const CIMFlavor& flavor) const
{
	if(flavor)
	{
		size_t tsize = m_pdata->m_flavors.size();
		for(size_t i = 0; i < tsize; i++)
		{
			if(m_pdata->m_flavors[i].getFlavor() == flavor.getFlavor())
			{
				return true;
			}
		}
	}
	return(false);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::addFlavor(const CIMFlavor& flavor)
{
	if(flavor)
	{
		Int32 flavorValue = flavor.getFlavor();
		//
		// Don't add it if its already present
		//
		size_t tsize = m_pdata->m_flavors.size();
		for(size_t i = 0; i < tsize; i++)
		{
			if(m_pdata->m_flavors[i].getFlavor() == flavorValue)
			{
				return *this;
			}
		}
		switch(flavorValue)
		{
			case CIMFlavor::ENABLEOVERRIDE:
				removeFlavor(CIMFlavor::DISABLEOVERRIDE);
				break;
			case CIMFlavor::DISABLEOVERRIDE:
				removeFlavor(CIMFlavor::ENABLEOVERRIDE);
				break;
			case CIMFlavor::RESTRICTED:
				removeFlavor(CIMFlavor::TOSUBCLASS);
				break;
			case CIMFlavor::TOSUBCLASS:
				removeFlavor(CIMFlavor::RESTRICTED);
				break;
		}
		m_pdata->m_flavors.append(flavor);
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::removeFlavor(Int32 flavor)
{
	for(size_t i = 0; i < m_pdata->m_flavors.size(); i++)
	{
		if(m_pdata->m_flavors[i].getFlavor() == flavor)
		{
			m_pdata->m_flavors.remove(i--);
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::hasValue() const
{
	return (m_pdata->m_qualifierValue) ? true : false;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::equals(const CIMQualifier& arg) const
{
	return m_pdata->m_name.equals(arg.getName());
}
//////////////////////////////////////////////////////////////////////////////
CIMFlavorArray
CIMQualifier::getFlavor() const
{
	return m_pdata->m_flavors;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::setPropagated(bool propagated)
{
	m_pdata->m_propagated = propagated;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::getPropagated() const
{
	return m_pdata->m_propagated;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifier::getName() const
{
	return m_pdata->m_name;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifier::setName(const String& name)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifier::readObject(istream &istrm)
{
	String name;
	CIMValue qualifierValue(CIMNULL);
	CIMQualifierType qualifierType(CIMNULL);
	Bool propagated;
	CIMFlavorArray flavors;
	Bool isValue;
	CIMBase::readSig( istrm, OW_CIMQUALIFIERSIG );
	name.readObject(istrm);
	isValue.readObject(istrm);
	if(isValue)
	{
		qualifierValue.readObject(istrm);
	}
	qualifierType.readObject(istrm);
	propagated.readObject(istrm);
	BinarySerialization::readArray(istrm, flavors);
	if(m_pdata.isNull())
	{
		m_pdata = new QUALData;
	}
	m_pdata->m_name = name;
	m_pdata->m_qualifierValue = qualifierValue;
	m_pdata->m_qualifierType = qualifierType;
	m_pdata->m_propagated = propagated;
	m_pdata->m_flavors = flavors;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifier::writeObject(ostream &ostrm) const
{
	CIMBase::writeSig(ostrm, OW_CIMQUALIFIERSIG);
	m_pdata->m_name.writeObject(ostrm);
	CIMValue qv = m_pdata->m_qualifierValue;
	if(!qv && m_pdata->m_qualifierType)
	{
		qv = m_pdata->m_qualifierType.getDefaultValue();
	}
	if(m_pdata->m_qualifierValue)
	{
		Bool(true).writeObject(ostrm);
		qv.writeObject(ostrm);
	}
	else
	{
		Bool(false).writeObject(ostrm);
	}
	m_pdata->m_qualifierType.writeObject(ostrm);
	m_pdata->m_propagated.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_pdata->m_flavors);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifier::toMOF() const
{
	StringBuffer rv;
	rv += m_pdata->m_name;
	if(m_pdata->m_qualifierValue)
	{
		rv += '(';
		rv += m_pdata->m_qualifierValue.toMOF();
		rv += ')';
	}
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifier::toString() const
{
	String rv = "CIMQualifier(" + m_pdata->m_name + ')';
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMQualifier
CIMQualifier::createKeyQualifier()
{
	CIMQualifier cq(CIMQualifier::CIM_QUAL_KEY);
	cq.setValue(CIMValue(Bool(true)));
	return cq;
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMQualifier& x, const CIMQualifier& y)
{
	return *x.m_pdata < *y.m_pdata;
}
// Meta qualifiers
const char* const CIMQualifier::CIM_QUAL_ASSOCIATION		= "Association";
const char* const CIMQualifier::CIM_QUAL_INDICATION		= "Indication";
// Standard qualifiers
const char* const CIMQualifier::CIM_QUAL_ABSTRACT 		= "abstract";
const char* const CIMQualifier::CIM_QUAL_AGGREGATE 		= "aggregate";
const char* const CIMQualifier::CIM_QUAL_AGGREGATION 	= "aggregation";
const char* const CIMQualifier::CIM_QUAL_ALIAS 			= "alias";
const char* const CIMQualifier::CIM_QUAL_ARRAYTYPE 		= "arraytype";
const char* const CIMQualifier::CIM_QUAL_BITMAP 			= "bitmap";
const char* const CIMQualifier::CIM_QUAL_BITVALUES 		= "bitvalues";
const char* const CIMQualifier::CIM_QUAL_COUNTER 		= "counter";
const char* const CIMQualifier::CIM_QUAL_DESCRIPTION 	= "description";
const char* const CIMQualifier::CIM_QUAL_DISPLAYNAME 	= "displayname";
const char* const CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT 	= "embeddedobject";
const char* const CIMQualifier::CIM_QUAL_GAUGE 			= "gauge";
const char* const CIMQualifier::CIM_QUAL_IN 				= "in";
const char* const CIMQualifier::CIM_QUAL_KEY 			= "key";
const char* const CIMQualifier::CIM_QUAL_MAPPINGSTRINGS	= "mappingstrings";
const char* const CIMQualifier::CIM_QUAL_MAX 			= "max";
const char* const CIMQualifier::CIM_QUAL_MAXLEN 			= "maxlen";
const char* const CIMQualifier::CIM_QUAL_MAXVALUE 		= "maxvalue";
const char* const CIMQualifier::CIM_QUAL_MIN 			= "min";
const char* const CIMQualifier::CIM_QUAL_MINVALUE 		= "minvalue";
const char* const CIMQualifier::CIM_QUAL_NONLOCAL 		= "nonlocal";
const char* const CIMQualifier::CIM_QUAL_NONLOCALTYPE 	= "nonlocaltype";
const char* const CIMQualifier::CIM_QUAL_NULLVALUE 		= "nullvalue";
const char* const CIMQualifier::CIM_QUAL_OUT 			= "out";
const char* const CIMQualifier::CIM_QUAL_OVERRIDE 		= "override";
const char* const CIMQualifier::CIM_QUAL_PROPAGATED 		= "propagated";
const char* const CIMQualifier::CIM_QUAL_READ 			= "read";
const char* const CIMQualifier::CIM_QUAL_REQUIRED 		= "required";
const char* const CIMQualifier::CIM_QUAL_REVISION 		= "revision";
const char* const CIMQualifier::CIM_QUAL_SCHEMA 			= "schema";
const char* const CIMQualifier::CIM_QUAL_SOURCE 			= "source";
const char* const CIMQualifier::CIM_QUAL_SOURCETYPE 		= "sourcetype";
const char* const CIMQualifier::CIM_QUAL_STATIC 			= "static";
const char* const CIMQualifier::CIM_QUAL_TERMINAL 		= "terminal";
const char* const CIMQualifier::CIM_QUAL_UNITS 			= "units";
const char* const CIMQualifier::CIM_QUAL_VALUEMAP 		= "valuemap";
const char* const CIMQualifier::CIM_QUAL_VALUES 			= "values";
const char* const CIMQualifier::CIM_QUAL_VERSION 		= "version";
const char* const CIMQualifier::CIM_QUAL_WEAK 			= "weak";
const char* const CIMQualifier::CIM_QUAL_WRITE 			= "write";
const char* const CIMQualifier::CIM_QUAL_PROVIDER		= "provider";

} // end namespace OpenWBEM

