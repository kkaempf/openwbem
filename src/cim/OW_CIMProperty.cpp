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
#include "OW_CIMValueCast.hpp"
#include "OW_MutexLock.hpp"

using std::istream;
using std::ostream;

struct OW_CIMProperty::PROPData
{
	PROPData();
	PROPData(const PROPData& x);
	PROPData& operator= (const PROPData& x);

	OW_String m_name;
	OW_CIMQualifierArray m_qualifiers;

	//
	// Note that we can't rely on the cimValue's datatype
	// because that is determined by what gets stored  as
	// a value rather than what the MOF declared which is
	// what is stored in propertyDataType.
	//
	OW_CIMDataType m_propertyDataType;

	//
	// For an array type property this states how large it
	// was declared as
	//
	OW_Int32 m_sizeDataType;
	OW_String m_override;
	OW_String m_originClass;
	OW_CIMValue m_cimValue;

	// propagated means inherited without change
	OW_Bool m_propagated;
};

OW_CIMProperty::PROPData::PROPData() :
	m_name(), m_qualifiers(), m_propertyDataType(true), m_sizeDataType(-1),
	m_override(), m_originClass(), m_cimValue(), m_propagated(false)
{
}

OW_CIMProperty::PROPData::PROPData(const PROPData& x) :
	m_name(x.m_name), m_qualifiers(x.m_qualifiers),
	m_propertyDataType(x.m_propertyDataType), m_sizeDataType(x.m_sizeDataType),
	m_override(x.m_override), m_originClass(x.m_originClass),
	m_cimValue(x.m_cimValue), m_propagated(x.m_propagated)
{
}
	
OW_CIMProperty::PROPData&
OW_CIMProperty::PROPData::operator= (const PROPData& x)
{
	m_name = x.m_name;
	m_qualifiers = x.m_qualifiers;
	m_propertyDataType = x.m_propertyDataType;
	m_sizeDataType = x.m_sizeDataType;
	m_override = x.m_override;
	m_originClass = x.m_originClass;
	m_cimValue = x.m_cimValue;
	m_propagated = x.m_propagated;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty::OW_CIMProperty(OW_Bool notNull) :
	OW_CIMElement(), m_pdata((notNull) ? new PROPData : NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty::OW_CIMProperty(const OW_String& name) :
	OW_CIMElement(), m_pdata(new PROPData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty::OW_CIMProperty(const char* name) :
	OW_CIMElement(), m_pdata(new PROPData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty::OW_CIMProperty(const OW_String& name,
	const OW_CIMValue& value) :
	OW_CIMElement(), m_pdata(new PROPData)
{
	m_pdata->m_name = name;
	m_pdata->m_cimValue = value;
	m_pdata->m_propertyDataType = value.getCIMDataType();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty::OW_CIMProperty(const OW_String& name,
	const OW_CIMDataType& dt) :
	OW_CIMElement(), m_pdata(new PROPData)
{
	m_pdata->m_name = name;
	m_pdata->m_propertyDataType = dt;
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty::OW_CIMProperty(const OW_CIMProperty& x) :
	OW_CIMElement(), m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty::~OW_CIMProperty()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty&
OW_CIMProperty::operator= (const OW_CIMProperty& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMProperty::clone(OW_Bool includeQualifier,
	OW_Bool includeClassOrigin) const
{
	OW_CIMProperty cp(OW_Bool(true));

	if(includeQualifier)
	{
		cp.m_pdata->m_qualifiers = m_pdata->m_qualifiers;
	}

	if(includeClassOrigin)
	{
		cp.m_pdata->m_originClass = m_pdata->m_originClass;
	}

	cp.m_pdata->m_propertyDataType = m_pdata->m_propertyDataType;
	cp.m_pdata->m_sizeDataType = m_pdata->m_sizeDataType;
	cp.m_pdata->m_name = m_pdata->m_name;
	cp.m_pdata->m_override = m_pdata->m_override;
	cp.m_pdata->m_cimValue = m_pdata->m_cimValue;
	cp.m_pdata->m_propagated = m_pdata->m_propagated;

	return cp;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierArray
OW_CIMProperty::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setQualifiers(const OW_CIMQualifierArray& quals)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_qualifiers = quals;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMProperty::getOriginClass() const
{
	return m_pdata->m_originClass;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setOriginClass(const OW_String& originCls)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_originClass = originCls;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setValue(const OW_CIMValue& val)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_cimValue = val;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMProperty::getValue() const
{
	return m_pdata->m_cimValue;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setDataType(const OW_CIMDataType& type)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_propertyDataType = type;
	if(m_pdata->m_cimValue)
	{
		if(m_pdata->m_propertyDataType.getType() != m_pdata->m_cimValue.getType()
			|| m_pdata->m_propertyDataType.isArrayType() !=
			m_pdata->m_cimValue.isArray())
		{
			m_pdata->m_cimValue = OW_CIMValueCast::castValueToDataType(
				m_pdata->m_cimValue, m_pdata->m_propertyDataType);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setDataType(const OW_CIMDataType::Type& type)
{
	setDataType(OW_CIMDataType(type));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType
OW_CIMProperty::getDataType() const
{
	return m_pdata->m_propertyDataType;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_CIMProperty::getSize() const
{
	return m_pdata->m_sizeDataType;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setDataSize(OW_Int32 size)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_sizeDataType = size;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setOverridingProperty(const OW_String& opname)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_override = opname;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMProperty::getOverridingProperty() const
{
	return m_pdata->m_override;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMProperty::isReference() const
{
	return m_pdata->m_propertyDataType.isReferenceType();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier
OW_CIMProperty::getQualifier(const OW_String& name) const
{
	size_t tsize = m_pdata->m_qualifiers.size();
	for(size_t i = 0; i < tsize; i++)
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
OW_CIMQualifier
OW_CIMProperty::setQualifier(const OW_CIMQualifier& qual)
{
	if(!qual)
		return qual;

	OW_MutexLock l = m_pdata.getWriteLock();
	size_t tsize = m_pdata->m_qualifiers.size();
	for(size_t i = 0; i < tsize; i++)
	{
		OW_CIMQualifier nq = m_pdata->m_qualifiers[i];
		if(nq.getName().equalsIgnoreCase(qual.getName()))
		{
			m_pdata->m_qualifiers[i] = qual;
			return qual;
		}
	}

	OW_String msg("Qualifier ");
	msg += qual.getName();
	msg += " not found";

	OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, msg.c_str());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::addQualifier(const OW_CIMQualifier& qual)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	size_t tsize = m_pdata->m_qualifiers.size();
	for(size_t i = 0; i < tsize; i++)
	{
		OW_CIMQualifier nq = m_pdata->m_qualifiers[i];
		if(nq.getName().equalsIgnoreCase(qual.getName()))
		{
			OW_String msg("Qualifier ");
			msg += qual.getName();
			msg += " already exists";

			OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS, msg.c_str());
		}
	}
	m_pdata->m_qualifiers.append(qual);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMProperty::removeQualifier(const OW_String& name)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	size_t tsize = m_pdata->m_qualifiers.size();
	for(size_t i = 0; i < tsize; i++)
	{
		OW_CIMQualifier nq = m_pdata->m_qualifiers[i];
		if(nq.getName().equalsIgnoreCase(name))
		{
			m_pdata->m_qualifiers.remove(i);
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMProperty::isKey() const
{
	if(getDataType().isReferenceType()
	   || getQualifier(OW_CIMQualifier::CIM_QUAL_KEY))
	{
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMProperty::filter(OW_Bool localOnly, OW_Bool includeQualifiers) const
{
	//
	// If only local definitions are required and this is a propagated
	// property then nothing to return
	//
	if(localOnly && m_pdata->m_propagated)
	{
		return OW_CIMProperty();
	}

	OW_CIMProperty cp(OW_Bool(true));
	cp.m_pdata->m_propertyDataType = m_pdata->m_propertyDataType;
	cp.m_pdata->m_sizeDataType = m_pdata->m_sizeDataType;
	cp.m_pdata->m_name = m_pdata->m_name;
	cp.m_pdata->m_override = m_pdata->m_override;
	cp.m_pdata->m_originClass = m_pdata->m_originClass;
	cp.m_pdata->m_cimValue = m_pdata->m_cimValue;
	cp.m_pdata->m_propagated = m_pdata->m_propagated;

	if(includeQualifiers)
	{
		cp.m_pdata->m_qualifiers = m_pdata->m_qualifiers;
	}

	return cp;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setPropagated(OW_Bool propagated)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_propagated = propagated;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMProperty::getPropagated() const
{
	return m_pdata->m_propagated;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMProperty::getName() const
{
	return m_pdata->m_name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::setName(const OW_String& name)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::clearQualifiers()
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_qualifiers.clear();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::writeObject(ostream &ostrm) const
{
	writeObject(ostrm, true);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::writeObject(ostream &ostrm, OW_Bool includeQualifiers) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMPROPERTYSIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_override.writeObject(ostrm);
	m_pdata->m_originClass.writeObject(ostrm);
	m_pdata->m_propertyDataType.writeObject(ostrm);

	OW_Int32 nv = OW_hton32(m_pdata->m_sizeDataType);
	if(!ostrm.write((const char*)&nv, sizeof(nv)))
	{
		OW_THROW(OW_IOException, "failed to write data type size");
	}

	m_pdata->m_propagated.writeObject(ostrm);

	size_t len;
	if(includeQualifiers)
	{
		len = m_pdata->m_qualifiers.size();
		nv = OW_hton32(len);
		if(!ostrm.write((const char*)&nv, sizeof(nv)))
		{
			OW_THROW(OW_IOException, "failed to write len of qualifier array");
		}

		for(size_t i = 0; i < len; i++)
		{
			OW_CIMQualifier cq = m_pdata->m_qualifiers[i];
			cq.writeObject(ostrm);
		}
	}
	else
	{
		nv = 0;
		if(!ostrm.write((const char*)&nv, sizeof(nv)))
		{
			OW_THROW(OW_IOException, "failed to write len of qualifier array");
		}
	}

	if(m_pdata->m_cimValue)
	{
		OW_Bool(true).writeObject(ostrm);
		m_pdata->m_cimValue.writeObject(ostrm);
	}
	else
	{
		OW_Bool(false).writeObject(ostrm);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMProperty::readObject(istream &istrm)
{
	OW_String name;
	OW_String override;
	OW_String originClass;
	OW_CIMValue cimValue;
	OW_CIMDataType propertyDataType;
	OW_Int32 sizeDataType;
	OW_Bool propagated;
	OW_CIMQualifierArray qualifiers;

	OW_CIMBase::readSig( istrm, OW_CIMPROPERTYSIG );
	name.readObject(istrm);
	override.readObject(istrm);
	originClass.readObject(istrm);
	propertyDataType.readObject(istrm);

	if(!istrm.read((char*)&sizeDataType, sizeof(sizeDataType)))
	{
		OW_THROW(OW_IOException, "failed to read data type size");
	}
	sizeDataType = OW_ntoh32(sizeDataType);

	propagated.readObject(istrm);

	size_t len;
	if(!istrm.read((char*)&len, sizeof(len)))
	{
		OW_THROW(OW_IOException, "failed to read len of qualifier array");
	}
	len = OW_ntoh32(len);

	for(size_t i = 0; i < len; i++)
	{
		OW_CIMQualifier cq;
		cq.readObject(istrm);
		qualifiers.append(cq);
	}

	OW_Bool isValue;
	isValue.readObject(istrm);

	if(isValue)
	{
		cimValue.readObject(istrm);
	}

	if(m_pdata.isNull())
	{
		m_pdata = new PROPData;
	}

	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
	m_pdata->m_override = override;
	m_pdata->m_originClass = originClass;
	m_pdata->m_cimValue = cimValue;
	m_pdata->m_propertyDataType = propertyDataType;
	m_pdata->m_sizeDataType = sizeDataType;
	m_pdata->m_propagated = propagated;
	m_pdata->m_qualifiers = qualifiers;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMProperty::toString() const
{
	OW_String rv = m_pdata->m_propertyDataType.toString() + ":"
		+ m_pdata->m_name + "=";

	if(m_pdata->m_cimValue)
	{
		rv += m_pdata->m_cimValue.toString();
	}
	else
	{
		rv += "null";
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMProperty::toMOF() const
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

	rv += m_pdata->m_propertyDataType.toMOF();
	rv += ' ';
	rv += m_pdata->m_name;
	if(m_pdata->m_cimValue)
	{
		rv += '=';
		rv += m_pdata->m_cimValue.toMOF();
	}
	rv += ";\n";
	return rv.toString();
}

const char* const OW_CIMProperty::NAME_PROPERTY = "Name";
