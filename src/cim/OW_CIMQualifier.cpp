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
#include "OW_Assertion.hpp"
#include "OW_MutexLock.hpp"

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
struct OW_CIMQualifier::QUALData
{
	QUALData() :
		m_name(), m_qualifierValue(), m_qualifierType(OW_Bool(true)),
		m_propagated(false), m_flavors() {}

	QUALData(const QUALData& x) :
		m_name(x.m_name), m_qualifierValue(x.m_qualifierValue),
		m_qualifierType(x.m_qualifierType), m_propagated(x.m_propagated),
		m_flavors(x.m_flavors) {}

	QUALData& operator= (const QUALData& x)
	{
		m_name = x.m_name;
		m_qualifierValue = x.m_qualifierValue;
		m_qualifierType = x.m_qualifierType;
		m_propagated = x.m_propagated;
		m_flavors = x.m_flavors;
		return *this;
	}

	OW_String m_name;
	OW_CIMValue m_qualifierValue;
	OW_CIMQualifierType m_qualifierType;
	OW_Bool m_propagated;
	OW_CIMFlavorArray m_flavors;
};

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier::OW_CIMQualifier(OW_Bool notNull) :
	OW_CIMElement(), m_pdata((notNull) ? new QUALData : NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier::OW_CIMQualifier(const OW_String& name) :
	OW_CIMElement(), m_pdata(new QUALData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier::OW_CIMQualifier(const char* name) :
	OW_CIMElement(), m_pdata(new QUALData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier::OW_CIMQualifier(const OW_String& name,
	const OW_CIMQualifierType& cgt) :
	OW_CIMElement(), m_pdata(new QUALData)
{
	m_pdata->m_name = name;
	m_pdata->m_qualifierType = cgt;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier::OW_CIMQualifier(const OW_CIMQualifier& x) :
	OW_CIMElement(), m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier::~OW_CIMQualifier()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifier::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier&
OW_CIMQualifier::operator= (const OW_CIMQualifier& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifier::isKeyQualifier() const
{
	return m_pdata->m_name.equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_KEY);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifier::isAssociationQualifier() const
{
	return m_pdata->m_name.equalsIgnoreCase(
		OW_CIMQualifier::CIM_QUAL_ASSOCIATION);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMQualifier::getValue() const
{
	return m_pdata->m_qualifierValue;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifier::setValue(const OW_CIMValue& value)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_qualifierValue = value;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifier::setDefaults(const OW_CIMQualifierType& qtype)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_qualifierType = qtype;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMQualifier::getDefaults() const
{
	return m_pdata->m_qualifierType;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifier::hasFlavor(const OW_CIMFlavor& flavor) const
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
void
OW_CIMQualifier::addFlavor(const OW_CIMFlavor& flavor)
{
	if(flavor)
	{
		int flavorValue = flavor.getFlavor();

		//
		// Don't add it if its already present
		//
		size_t tsize = m_pdata->m_flavors.size();
		for(size_t i = 0; i < tsize; i++)
		{
			if(m_pdata->m_flavors[i].getFlavor() == flavorValue)
			{
				return;
			}
		}

		switch(flavorValue)
		{
			case OW_CIMFlavor::ENABLEOVERRIDE:
				removeFlavor(OW_CIMFlavor::DISABLEOVERRIDE);
				break;
			case OW_CIMFlavor::DISABLEOVERRIDE:
				removeFlavor(OW_CIMFlavor::ENABLEOVERRIDE);
				break;

			case OW_CIMFlavor::RESTRICTED:
				removeFlavor(OW_CIMFlavor::TOSUBCLASS);
				break;

			case OW_CIMFlavor::TOSUBCLASS:
				removeFlavor(OW_CIMFlavor::RESTRICTED);
				break;
		}

		OW_MutexLock l = m_pdata.getWriteLock();
		m_pdata->m_flavors.append(flavor);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifier::removeFlavor(int flavor)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	for(size_t i = 0; i < m_pdata->m_flavors.size(); i++)
	{
		if(m_pdata->m_flavors[i].getFlavor() == flavor)
		{
			m_pdata->m_flavors.remove(i--);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifier::hasValue() const
{
	return OW_Bool((void*)m_pdata->m_qualifierValue != 0);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifier::equals(const OW_CIMQualifier& arg) const
{
	return m_pdata->m_name.equals(arg.getName());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFlavorArray
OW_CIMQualifier::getFlavor() const
{
	return m_pdata->m_flavors;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifier::setPropagated(OW_Bool propagated)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_propagated = propagated;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifier::getPropagated() const
{
	return m_pdata->m_propagated;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier::operator void*() const
{
	return (void*)(!m_pdata.isNull());
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMQualifier::getName() const
{
	return m_pdata->m_name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifier::setName(const OW_String& name)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifier::readObject(istream &istrm)
{
	OW_String name;
	OW_CIMValue qualifierValue;
	OW_CIMQualifierType qualifierType;
	OW_Bool propagated;
	OW_CIMFlavorArray flavors;
	OW_Bool isValue;

	OW_CIMBase::readSig( istrm, OW_CIMQUALIFIERSIG );
	name.readObject(istrm);

	isValue.readObject(istrm);
	if(isValue)
	{
		qualifierValue.readObject(istrm);
	}

	qualifierType.readObject(istrm);
	propagated.readObject(istrm);

	size_t len;
	if(!istrm.read((char*)&len, sizeof(len)))
	{
		OW_THROW(OW_IOException, "failed to read size of flavor array");
	}
	len = OW_ntoh32(len);

	for(size_t i = 0; i < len; i++)
	{
		OW_CIMFlavor f;
		f.readObject(istrm);
		flavors.append(f);
	}

	if(m_pdata.isNull())
	{
		m_pdata = new QUALData;
	}

	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
	m_pdata->m_qualifierValue = qualifierValue;
	m_pdata->m_qualifierType = qualifierType;
	m_pdata->m_propagated = propagated;
	m_pdata->m_flavors = flavors;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifier::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig(ostrm, OW_CIMQUALIFIERSIG);
	m_pdata->m_name.writeObject(ostrm);

	if(!m_pdata->m_qualifierValue && m_pdata->m_qualifierType)
	{
		m_pdata->m_qualifierValue = m_pdata->m_qualifierType.getDefaultValue();
	}

	if(m_pdata->m_qualifierValue)
	{
		OW_Bool(true).writeObject(ostrm);
		m_pdata->m_qualifierValue.writeObject(ostrm);
	}
	else
	{
		OW_Bool(false).writeObject(ostrm);
	}

	m_pdata->m_qualifierType.writeObject(ostrm);
	m_pdata->m_propagated.writeObject(ostrm);

	size_t len = m_pdata->m_flavors.size();
	int nl = OW_hton32(len);
	if(!ostrm.write((const char*)&nl, sizeof(nl)))
	{
		OW_THROW(OW_IOException, "failed to write size of flavor array");
	}

	for(size_t i = 0; i < len; i++)
	{
		OW_CIMFlavor f = m_pdata->m_flavors[i];
		f.writeObject(ostrm);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMQualifier::toMOF() const
{
	OW_StringBuffer rv;

	rv += m_pdata->m_name;
	if(m_pdata->m_qualifierValue)
	{
		rv += '(';
		rv += m_pdata->m_qualifierValue.toMOF();
		rv += ')';
	}
	return rv.toString();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMQualifier::toString() const
{
	OW_String rv = "OW_CIMQualifier(" + m_pdata->m_name + ')';
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMQualifier
OW_CIMQualifier::createKeyQualifier()
{
	OW_CIMQualifier cq(OW_CIMQualifier::CIM_QUAL_KEY);
	cq.setValue(OW_CIMValue(OW_Bool(true)));
	return cq;
}


// Meta qualifiers
const char* const OW_CIMQualifier::CIM_QUAL_ASSOCIATION		= "association";
const char* const OW_CIMQualifier::CIM_QUAL_INDICATION		= "indication";

// Standard qualifiers
const char* const OW_CIMQualifier::CIM_QUAL_ABSTRACT 		= "abstract";
const char* const OW_CIMQualifier::CIM_QUAL_AGGREGATE 		= "aggregate";
const char* const OW_CIMQualifier::CIM_QUAL_AGGREGATION 	= "aggregation";
const char* const OW_CIMQualifier::CIM_QUAL_ALIAS 			= "alias";
const char* const OW_CIMQualifier::CIM_QUAL_ARRAYTYPE 		= "arraytype";
const char* const OW_CIMQualifier::CIM_QUAL_BITMAP 			= "bitmap";
const char* const OW_CIMQualifier::CIM_QUAL_BITVALUES 		= "bitvalues";
const char* const OW_CIMQualifier::CIM_QUAL_COUNTER 		= "counter";
const char* const OW_CIMQualifier::CIM_QUAL_DESCRIPTION 	= "description";
const char* const OW_CIMQualifier::CIM_QUAL_DISPLAYNAME 	= "displayname";
const char* const OW_CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT 	= "embeddedobject";
const char* const OW_CIMQualifier::CIM_QUAL_GAUGE 			= "gauge";
const char* const OW_CIMQualifier::CIM_QUAL_IN 				= "in";
const char* const OW_CIMQualifier::CIM_QUAL_KEY 			= "key";
const char* const OW_CIMQualifier::CIM_QUAL_MAPPINGSTRINGS	= "mappingstrings";
const char* const OW_CIMQualifier::CIM_QUAL_MAX 			= "max";
const char* const OW_CIMQualifier::CIM_QUAL_MAXLEN 			= "maxlen";
const char* const OW_CIMQualifier::CIM_QUAL_MAXVALUE 		= "maxvalue";
const char* const OW_CIMQualifier::CIM_QUAL_MIN 			= "min";
const char* const OW_CIMQualifier::CIM_QUAL_MINVALUE 		= "minvalue";
const char* const OW_CIMQualifier::CIM_QUAL_NONLOCAL 		= "nonlocal";
const char* const OW_CIMQualifier::CIM_QUAL_NONLOCALTYPE 	= "nonlocaltype";
const char* const OW_CIMQualifier::CIM_QUAL_NULLVALUE 		= "nullvalue";
const char* const OW_CIMQualifier::CIM_QUAL_OUT 			= "out";
const char* const OW_CIMQualifier::CIM_QUAL_OVERRIDE 		= "override";
const char* const OW_CIMQualifier::CIM_QUAL_PROPAGATED 		= "propagated";
const char* const OW_CIMQualifier::CIM_QUAL_READ 			= "read";
const char* const OW_CIMQualifier::CIM_QUAL_REQUIRED 		= "required";
const char* const OW_CIMQualifier::CIM_QUAL_REVISION 		= "revision";
const char* const OW_CIMQualifier::CIM_QUAL_SCHEMA 			= "schema";
const char* const OW_CIMQualifier::CIM_QUAL_SOURCE 			= "source";
const char* const OW_CIMQualifier::CIM_QUAL_SOURCETYPE 		= "sourcetype";
const char* const OW_CIMQualifier::CIM_QUAL_STATIC 			= "static";
const char* const OW_CIMQualifier::CIM_QUAL_TERMINAL 		= "terminal";
const char* const OW_CIMQualifier::CIM_QUAL_UNITS 			= "units";
const char* const OW_CIMQualifier::CIM_QUAL_VALUEMAP 		= "valuemap";
const char* const OW_CIMQualifier::CIM_QUAL_VALUES 			= "values";
const char* const OW_CIMQualifier::CIM_QUAL_VERSION 		= "version";
const char* const OW_CIMQualifier::CIM_QUAL_WEAK 			= "weak";
const char* const OW_CIMQualifier::CIM_QUAL_WRITE 			= "write";
const char* const OW_CIMQualifier::CIM_QUAL_PROVIDER		= "provider";
