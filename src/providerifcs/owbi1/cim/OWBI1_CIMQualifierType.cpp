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
#include "OWBI1_CIMQualifierType.hpp"
#include "OWBI1_CIMQualifierTypeRep.hpp"
#include "OWBI1_CIMName.hpp"
#include "OWBI1_CIMNameRep.hpp"
#include "OWBI1_CIMScope.hpp"
#include "OWBI1_CIMScopeRep.hpp"
#include "OWBI1_Array.hpp"
#include "OWBI1_CIMDetail.hpp"
#include "OWBI1_CIMDataType.hpp"
#include "OWBI1_CIMDataTypeRep.hpp"
#include "OWBI1_CIMValue.hpp"
#include "OWBI1_CIMValueRep.hpp"
#include "OWBI1_CIMFlavor.hpp"
#include "OWBI1_CIMFlavorRep.hpp"


namespace OWBI1
{
using namespace detail;
using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMQualifierType& x, const CIMQualifierType& y)
{
	return *x.getRep() < *y.getRep();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType() 
	: m_rep(new CIMQualifierTypeRep(OpenWBEM::CIMQualifierType()))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(CIMNULL_t) 
	: m_rep(new CIMQualifierTypeRep(OpenWBEM::CIMQualifierType(OpenWBEM::CIMNULL)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(const CIMName& name)
	: m_rep(new CIMQualifierTypeRep(OpenWBEM::CIMQualifierType(name.getRep()->name)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(const char* name)
	: m_rep(new CIMQualifierTypeRep(OpenWBEM::CIMQualifierType(name)))
{
}

//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(const CIMQualifierTypeRepRef& rep)
	: m_rep(rep)
{
}

//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(const CIMQualifierType& x) 
	: CIMElement(x)
	, m_rep(x.m_rep)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::~CIMQualifierType()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::operator= (const CIMQualifierType& x)
{
	m_rep = x.m_rep;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifierType::setNull()
{
	m_rep->qualtype.setNull();
}
//////////////////////////////////////////////////////////////////////////////
CIMScopeArray
CIMQualifierType::getScopes() const
{
	return wrapArray<CIMScopeArray>(m_rep->qualtype.getScope());
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType
CIMQualifierType::getDataType() const
{
	return CIMDataType(new CIMDataTypeRep(m_rep->qualtype.getDataType()));
}
//////////////////////////////////////////////////////////////////////////////
Int32
CIMQualifierType::getDataSize() const
{
	return m_rep->qualtype.getDataSize();
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMQualifierType::getDefaultValue() const
{
	return CIMValue(CIMValueRepRef(new CIMValueRep(m_rep->qualtype.getDefaultValue())));
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::setDataType(const CIMDataType& dataType)
{
	m_rep->qualtype.setDataType(dataType.getRep()->datatype);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::setDataType(const CIMDataType::Type& dataType)
{
	return setDataType(CIMDataType(dataType));
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::setDefaultValue(const CIMValue& defValue)
{
	m_rep->qualtype.setDefaultValue(defValue.getRep()->value);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::addScope(const CIMScope& newScope)
{
	m_rep->qualtype.addScope(newScope.getRep()->scope);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::hasScope(const CIMScope& scopeArg) const
{
	return m_rep->qualtype.hasScope(scopeArg.getRep()->scope);
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::hasFlavor(const CIMFlavor& flavorArg) const
{
	return m_rep->qualtype.hasFlavor(flavorArg.getRep()->flavor);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::addFlavor(const CIMFlavor& newFlavor)
{
	m_rep->qualtype.addFlavor(newFlavor.getRep()->flavor);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::removeFlavor(const Int32 flavor)
{
	m_rep->qualtype.removeFlavor(flavor);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMFlavorArray
CIMQualifierType::getFlavors() const
{
	return wrapArray<CIMFlavorArray>(m_rep->qualtype.getFlavors());
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::hasDefaultValue() const
{
	return m_rep->qualtype.hasDefaultValue();
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::isDefaultValueArray() const
{
	return m_rep->qualtype.isDefaultValueArray();
}
//////////////////////////////////////////////////////////////////////////////
CIMName
CIMQualifierType::getName() const
{
	return CIMName(new CIMNameRep(m_rep->qualtype.getName()));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifierType::setName(const CIMName& name)
{
	m_rep->qualtype.setName(name.getRep()->name);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifierType::writeObject(ostream &ostrm) const
{
	m_rep->qualtype.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifierType::readObject(istream &istrm)
{
	m_rep->qualtype.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifierType::toString() const
{
	return toMOF();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifierType::toMOF() const
{
	return m_rep->qualtype.toMOF().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::operator safe_bool () const
{  
	return m_rep->qualtype ? &CIMQualifierType::m_rep : 0; 
}

//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::operator!() const
{  
	return !m_rep->qualtype; 
}

//////////////////////////////////////////////////////////////////////////////
detail::CIMQualifierTypeRepRef
CIMQualifierType::getRep() const
{
	return m_rep;
}

} // end namespace OWBI1

