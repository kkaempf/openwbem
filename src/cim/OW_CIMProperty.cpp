/*******************************************************************************
* Copyright (C) 2001-3 Vintela, Inc. All rights reserved.
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
#include "OW_CIMProperty.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_NULLValueException.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include "OW_NoSuchQualifierException.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMException.hpp"

namespace OpenWBEM
{

using std::istream;
using std::ostream;
using namespace WBEMFlags;
struct CIMProperty::PROPData
{
	PROPData();
	String m_name;
	CIMQualifierArray m_qualifiers;
	//
	// Note that we can't rely on the cimValue's datatype
	// because that is determined by what gets stored  as
	// a value rather than what the MOF declared which is
	// what is stored in propertyDataType.
	//
	CIMDataType m_propertyDataType;
	//
	// For an array type property this states how large it
	// was declared as
	//
	Int32 m_sizeDataType;
	String m_override;
	String m_originClass;
	CIMValue m_cimValue;
	// propagated means inherited without change
	Bool m_propagated;
	PROPData* clone() const { return new PROPData(*this); }
};
CIMProperty::PROPData::PROPData() :
	m_sizeDataType(-1), m_cimValue(CIMNULL), m_propagated(false)
{
}
bool operator<(const CIMProperty::PROPData& x, const CIMProperty::PROPData& y)
{
	return StrictWeakOrdering(
		x.m_name, y.m_name,
		x.m_cimValue, y.m_cimValue,
		x.m_qualifiers, y.m_qualifiers,
		x.m_propertyDataType, y.m_propertyDataType,
		x.m_sizeDataType, y.m_sizeDataType,
		x.m_override, y.m_override,
		x.m_originClass, y.m_originClass,
		x.m_propagated, y.m_propagated);
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty() :
	CIMElement(), m_pdata(new PROPData)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(CIMNULL_t) :
	CIMElement(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const String& name) :
	CIMElement(), m_pdata(new PROPData)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const char* name) :
	CIMElement(), m_pdata(new PROPData)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const String& name,
	const CIMValue& value) :
	CIMElement(), m_pdata(new PROPData)
{
	m_pdata->m_name = name;
	m_pdata->m_cimValue = value;
	m_pdata->m_propertyDataType = value.getCIMDataType();
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const String& name,
	const CIMDataType& dt) :
	CIMElement(), m_pdata(new PROPData)
{
	m_pdata->m_name = name;
	m_pdata->m_propertyDataType = dt;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const CIMProperty& x) :
	CIMElement(), m_pdata(x.m_pdata)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::~CIMProperty()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMProperty::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::operator= (const CIMProperty& x)
{
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMProperty::clone(EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin) const
{
	CIMProperty cp;
	if(includeQualifiers)
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
CIMQualifierArray
CIMProperty::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setQualifiers(const CIMQualifierArray& quals)
{
	m_pdata->m_qualifiers = quals;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMProperty::getOriginClass() const
{
	return m_pdata->m_originClass;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setOriginClass(const String& originCls)
{
	m_pdata->m_originClass = originCls;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setValue(const CIMValue& val)
{
	if (m_pdata->m_propertyDataType && val && val.getCIMDataType() != m_pdata->m_propertyDataType)
	{
		m_pdata->m_cimValue = CIMValueCast::castValueToDataType(val, m_pdata->m_propertyDataType);
	}
	else
	{
		m_pdata->m_cimValue = val;
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMProperty::getValue() const
{
	return m_pdata->m_cimValue;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMProperty::getValueT() const
{
	if (!m_pdata->m_cimValue)
	{
		OW_THROW(NULLValueException, m_pdata->m_name.c_str());
	}
	return m_pdata->m_cimValue;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setDataType(const CIMDataType& type)
{
	m_pdata->m_propertyDataType = type;
	if(m_pdata->m_cimValue)
	{
		if(m_pdata->m_propertyDataType.getType() != m_pdata->m_cimValue.getType()
			|| m_pdata->m_propertyDataType.isArrayType() !=
			m_pdata->m_cimValue.isArray())
		{
			m_pdata->m_cimValue = CIMValueCast::castValueToDataType(
				m_pdata->m_cimValue, m_pdata->m_propertyDataType);
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setDataType(const CIMDataType::Type& type)
{
	return setDataType(CIMDataType(type));
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType
CIMProperty::getDataType() const
{
	return m_pdata->m_propertyDataType;
}
//////////////////////////////////////////////////////////////////////////////
Int32
CIMProperty::getSize() const
{
	return m_pdata->m_sizeDataType;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setDataSize(Int32 size)
{
	m_pdata->m_sizeDataType = size;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setOverridingProperty(const String& opname)
{
	m_pdata->m_override = opname;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMProperty::getOverridingProperty() const
{
	return m_pdata->m_override;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::isReference() const
{
	return m_pdata->m_propertyDataType.isReferenceType();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier
CIMProperty::getQualifier(const String& name) const
{
	size_t tsize = m_pdata->m_qualifiers.size();
	for(size_t i = 0; i < tsize; i++)
	{
		CIMQualifier nq = m_pdata->m_qualifiers[i];
		if(nq.getName().equalsIgnoreCase(name))
		{
			return nq;
		}
	}
	return CIMQualifier(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier
CIMProperty::getQualifierT(const String& name) const
{
	CIMQualifier rval = getQualifier(name);
	if (!rval)
	{
		OW_THROW(NoSuchQualifierException, name.c_str());
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setQualifier(const CIMQualifier& qual)
{
	if(qual)
	{
		String qualName = qual.getName();
		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			if(m_pdata->m_qualifiers[i].getName().equalsIgnoreCase(qualName))
			{
				m_pdata->m_qualifiers[i] = qual;
				return *this;
			}
		}
		m_pdata->m_qualifiers.append(qual);
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::addQualifier(const CIMQualifier& qual)
{
	size_t tsize = m_pdata->m_qualifiers.size();
	for(size_t i = 0; i < tsize; i++)
	{
		CIMQualifier nq = m_pdata->m_qualifiers[i];
		if(nq.getName().equalsIgnoreCase(qual.getName()))
		{
			String msg("Qualifier ");
			msg += qual.getName();
			msg += " already exists";
			OW_THROWCIMMSG(CIMException::ALREADY_EXISTS, msg.c_str());
		}
	}
	m_pdata->m_qualifiers.append(qual);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::removeQualifier(const String& name)
{
	size_t tsize = m_pdata->m_qualifiers.size();
	for(size_t i = 0; i < tsize; i++)
	{
		CIMQualifier nq = m_pdata->m_qualifiers[i];
		if(nq.getName().equalsIgnoreCase(name))
		{
			m_pdata->m_qualifiers.remove(i);
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::isKey() const
{
	// TODO: Evaluate this.  It's not necessarily true that a REF is a key.  That's just the DMTF CIM Schema convention.
	if(getDataType().isReferenceType()
	   || hasTrueQualifier(CIMQualifier::CIM_QUAL_KEY))
	{
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMProperty::filter(ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers) const
{
	//
	// If only local definitions are required and this is a propagated
	// property then nothing to return
	//
	if(localOnly && m_pdata->m_propagated)
	{
		return CIMProperty(CIMNULL);
	}
	CIMProperty cp;
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
CIMProperty&
CIMProperty::setPropagated(bool propagated)
{
	m_pdata->m_propagated = propagated;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::getPropagated() const
{
	return m_pdata->m_propagated;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMProperty::getName() const
{
	return m_pdata->m_name;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMProperty::setName(const String& name)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::clearQualifiers()
{
	m_pdata->m_qualifiers.clear();
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::clearNonKeyQualifiers()
{
	CIMQualifier key = getQualifier(CIMQualifier::CIM_QUAL_KEY);
	m_pdata->m_qualifiers.clear();
	if (key)
	{
		addQualifier(key);
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMProperty::writeObject(ostream &ostrm) const
{
	writeObject(ostrm, E_INCLUDE_QUALIFIERS);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMProperty::writeObject(ostream &ostrm, EIncludeQualifiersFlag includeQualifiers) const
{
	CIMBase::writeSig( ostrm, OW_CIMPROPERTYSIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_override.writeObject(ostrm);
	m_pdata->m_originClass.writeObject(ostrm);
	m_pdata->m_propertyDataType.writeObject(ostrm);
	BinarySerialization::writeLen(ostrm, m_pdata->m_sizeDataType);
	m_pdata->m_propagated.writeObject(ostrm);
	if(includeQualifiers)
	{
		BinarySerialization::writeArray(ostrm, m_pdata->m_qualifiers);
	}
	else
	{
		BinarySerialization::writeArray(ostrm, CIMQualifierArray());
	}
	if(m_pdata->m_cimValue)
	{
		Bool(true).writeObject(ostrm);
		m_pdata->m_cimValue.writeObject(ostrm);
	}
	else
	{
		Bool(false).writeObject(ostrm);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMProperty::readObject(istream &istrm)
{
	String name;
	String override;
	String originClass;
	CIMValue cimValue(CIMNULL);
	CIMDataType propertyDataType(CIMNULL);
	UInt32 sizeDataType;
	Bool propagated;
	CIMQualifierArray qualifiers;
	CIMBase::readSig( istrm, OW_CIMPROPERTYSIG );
	name.readObject(istrm);
	override.readObject(istrm);
	originClass.readObject(istrm);
	propertyDataType.readObject(istrm);
	BinarySerialization::readLen(istrm, sizeDataType);
	propagated.readObject(istrm);
	BinarySerialization::readArray(istrm, qualifiers);
	Bool isValue;
	isValue.readObject(istrm);
	if(isValue)
	{
		cimValue.readObject(istrm);
	}
	if(m_pdata.isNull())
	{
		m_pdata = new PROPData;
	}
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
String
CIMProperty::toString() const
{
	StringBuffer rv = m_pdata->m_propertyDataType.toString() + ":"
		+ m_pdata->m_name + "=";
	if(m_pdata->m_cimValue)
	{
		rv += m_pdata->m_cimValue.toString();
	}
	else
	{
		rv += "null";
	}
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMProperty::toMOF() const
{
	// this outputs a property suitable for a CIM class.
	StringBuffer rv;
	if(m_pdata->m_qualifiers.size() > 0)
	{
		rv += "  [";
		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			CIMQualifier nq = m_pdata->m_qualifiers[i];
			if(i > 0)
			{
				rv += ',';
			}
			rv += nq.toMOF();
		}
		rv += "]\n";
	}
	rv += "  ";
	rv += m_pdata->m_propertyDataType.toMOF();
	rv += ' ';
	rv += m_pdata->m_name;
	// If it is an array, show it.
	rv += m_pdata->m_propertyDataType.getArrayMOF();
	if(m_pdata->m_cimValue)
	{
		rv += '=';
		rv += m_pdata->m_cimValue.toMOF();
	}
	rv += ";\n";
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
const char* const CIMProperty::NAME_PROPERTY = "Name";
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMProperty& x, const CIMProperty& y)
{
	return *x.m_pdata < *y.m_pdata;
}
//////////////////////////////////////////////////////////////////////////////
bool 
CIMProperty::hasTrueQualifier(const String& name) const
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

} // end namespace OpenWBEM

