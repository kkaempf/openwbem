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

#include "OWBI1_config.h"
#include "OWBI1_CIMProperty.hpp"
#include "OWBI1_CIMPropertyRep.hpp"
#include "OW_CIMProperty.hpp"
#include "OWBI1_CIMName.hpp"
#include "OWBI1_CIMNameRep.hpp"
#include "OWBI1_CIMValue.hpp"
#include "OWBI1_CIMValueRep.hpp"
#include "OWBI1_CIMDataType.hpp"
#include "OWBI1_CIMDataTypeRep.hpp"
#include "OWBI1_CIMDetail.hpp"
#include "OWBI1_NULLValueException.hpp"
#include "OWBI1_CIMQualifier.hpp"
#include "OWBI1_CIMQualifierRep.hpp"
#include "OWBI1_NoSuchQualifierException.hpp"
#include "OW_CIMException.hpp"
#include "OWBI1_ProxyProviderDetail.hpp"

namespace OWBI1
{

using std::istream;
using std::ostream;
using namespace WBEMFlags;
using namespace detail;

//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty()
	: m_rep(new CIMPropertyRep(OpenWBEM::CIMProperty()))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(CIMNULL_t)
	: m_rep(new CIMPropertyRep(OpenWBEM::CIMProperty(OpenWBEM::CIMNULL)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const CIMName& name) 
	: m_rep(new CIMPropertyRep(OpenWBEM::CIMProperty(name.getRep()->name)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const char* name) 
	: m_rep(new CIMPropertyRep(OpenWBEM::CIMProperty(name)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const CIMName& name, const CIMValue& value) 
	: m_rep(new CIMPropertyRep(OpenWBEM::CIMProperty(name.getRep()->name, value.getRep()->value)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const CIMName& name, const CIMDataType& dt) 
	: m_rep(new CIMPropertyRep(OpenWBEM::CIMProperty(name.getRep()->name, dt.getRep()->datatype)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const CIMProperty& x) 
	: CIMElement(x)
	, m_rep(x.m_rep)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty::CIMProperty(const CIMPropertyRepRef& rep)
	: m_rep(rep)
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
	m_rep->prop.setNull();
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::operator= (const CIMProperty& x)
{
	m_rep = x.m_rep;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierArray
CIMProperty::getQualifiers() const
{
	return wrapArray<CIMQualifierArray>(m_rep->prop.getQualifiers());
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setQualifiers(const CIMQualifierArray& quals)
{
	m_rep->prop.setQualifiers(unwrapArray<OpenWBEM::CIMQualifierArray>(quals));
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMName
CIMProperty::getOriginClass() const
{
	return CIMName(new CIMNameRep(m_rep->prop.getOriginClass()));
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setOriginClass(const CIMName& originCls)
{
	m_rep->prop.setOriginClass(originCls.getRep()->name);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setValue(const CIMValue& val)
{
	m_rep->prop.setValue(val.getRep()->value);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMProperty::getValue() const
{
	return CIMValue(CIMValueRepRef(new CIMValueRep(m_rep->prop.getValue())));
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMProperty::getValueT() const
{
	CIMValue rv(CIMValueRepRef(new CIMValueRep(m_rep->prop.getValue())));
	if (!rv)
	{
		OWBI1_THROW(NULLValueException, getName().c_str());
	}
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setDataType(const CIMDataType& type)
{
	m_rep->prop.setDataType(type.getRep()->datatype);
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
	return CIMDataType(new CIMDataTypeRep(m_rep->prop.getDataType()));
}
//////////////////////////////////////////////////////////////////////////////
Int32
CIMProperty::getSize() const
{
	return m_rep->prop.getSize();
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setDataSize(Int32 size)
{
	m_rep->prop.setDataSize(size);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setOverridingProperty(const CIMName& opname)
{
	m_rep->prop.setOverridingProperty(opname.getRep()->name);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMName
CIMProperty::getOverridingProperty() const
{
	return CIMName(new CIMNameRep(m_rep->prop.getOverridingProperty()));
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::isReference() const
{
	return m_rep->prop.isReference();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier
CIMProperty::getQualifier(const CIMName& name) const
{
	return CIMQualifier(new CIMQualifierRep(m_rep->prop.getQualifier(name.getRep()->name)));
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier
CIMProperty::getQualifierT(const CIMName& name) const
{
	CIMQualifier rval = getQualifier(name);
	if (!rval)
	{
		OWBI1_THROW(NoSuchQualifierException, name.toString().c_str());
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setQualifier(const CIMQualifier& qual)
{
	m_rep->prop.setQualifier(qual.getRep()->qual);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::addQualifier(const CIMQualifier& qual)
{
	try
	{
		m_rep->prop.addQualifier(qual.getRep()->qual);
	}
	catch (OpenWBEM::CIMException& e)
	{
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::removeQualifier(const CIMName& name)
{
	return m_rep->prop.removeQualifier(name.getRep()->name);
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::isKey() const
{
	return m_rep->prop.isKey();
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty&
CIMProperty::setPropagated(bool propagated)
{
	m_rep->prop.setPropagated(propagated);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::getPropagated() const
{
	return m_rep->prop.getPropagated();
}
//////////////////////////////////////////////////////////////////////////////
CIMName
CIMProperty::getName() const
{
	return CIMName(new CIMNameRep(m_rep->prop.getName()));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMProperty::setName(const CIMName& name)
{
	m_rep->prop.setName(name.getRep()->name);
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
	m_rep->prop.writeObject(ostrm, convertWBEMFlag(includeQualifiers));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMProperty::readObject(istream &istrm)
{
	m_rep->prop.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMProperty::toString() const
{
	return m_rep->prop.toString().c_str();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMProperty::toMOF() const
{
	return m_rep->prop.toMOF().c_str();
}
//////////////////////////////////////////////////////////////////////////////
const char* const CIMProperty::NAME_PROPERTY = "Name";
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMProperty& x, const CIMProperty& y)
{
	return *x.getRep() < *y.getRep();
}

//////////////////////////////////////////////////////////////////////////////
bool
CIMProperty::hasTrueQualifier(const CIMName& name) const
{
	return m_rep->prop.hasTrueQualifier(name.getRep()->name);
}

//////////////////////////////////////////////////////////////////////////////
CIMProperty::operator safe_bool () const
{  
	return m_rep->prop ? &CIMProperty::m_rep : 0; 
}

//////////////////////////////////////////////////////////////////////////////
bool 
CIMProperty::operator!() const
{  
	return !(m_rep->prop); 
}

} // end namespace OWBI1

