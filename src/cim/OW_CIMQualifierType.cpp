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
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMValue.hpp"
#include "OW_COWIntrusiveCountableBase.hpp"

#include <algorithm> // for std::sort

namespace OpenWBEM
{

using std::istream;
using std::ostream;

struct CIMQualifierType::QUALTData : public COWIntrusiveCountableBase
{
	QUALTData()
		: m_defaultValue(CIMNULL)
	{}
	CIMName m_name;
	CIMDataType m_dataType;
	CIMScopeArray m_scope;
	CIMFlavorArray m_flavor;
	CIMValue m_defaultValue;
	QUALTData* clone() const { return new QUALTData(*this); }
};
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMQualifierType::QUALTData& x, const CIMQualifierType::QUALTData& y)
{
	return StrictWeakOrdering(
		x.m_name, y.m_name,
		x.m_dataType, y.m_dataType,
		x.m_scope, y.m_scope,
		x.m_flavor, y.m_flavor,
		x.m_defaultValue, y.m_defaultValue);
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMQualifierType& x, const CIMQualifierType& y)
{
	return *x.m_pdata < *y.m_pdata;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType() :
	CIMElement(), m_pdata(new QUALTData)
{
	addFlavor(CIMFlavor::ENABLEOVERRIDE);
	addFlavor(CIMFlavor::TOSUBCLASS);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(CIMNULL_t) :
	CIMElement(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(const CIMName& name) :
	CIMElement(), m_pdata(new QUALTData)
{
	m_pdata->m_name = name;
	addFlavor(CIMFlavor::ENABLEOVERRIDE);
	addFlavor(CIMFlavor::TOSUBCLASS);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(const char* name) :
	CIMElement(), m_pdata(new QUALTData)
{
	m_pdata->m_name = name;
	addFlavor(CIMFlavor::ENABLEOVERRIDE);
	addFlavor(CIMFlavor::TOSUBCLASS);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType::CIMQualifierType(const CIMQualifierType& x) :
	CIMElement(), m_pdata(x.m_pdata)
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
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifierType::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
const CIMScopeArray&
CIMQualifierType::getScope() const
{
	return m_pdata->m_scope;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType
CIMQualifierType::getDataType() const
{
	return m_pdata->m_dataType;
}
//////////////////////////////////////////////////////////////////////////////
Int32
CIMQualifierType::getDataSize() const
{
	return m_pdata->m_dataType.getSize();
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMQualifierType::getDefaultValue() const
{
	return m_pdata->m_defaultValue;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::setDataType(const CIMDataType& dataType)
{
	m_pdata->m_dataType = dataType;
	if (m_pdata->m_defaultValue)
	{
		m_pdata->m_defaultValue = CIMValueCast::castValueToDataType(
			m_pdata->m_defaultValue, m_pdata->m_dataType);
	}
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
	m_pdata->m_defaultValue = defValue;
	if (m_pdata->m_defaultValue)
	{
		m_pdata->m_defaultValue = CIMValueCast::castValueToDataType(
			m_pdata->m_defaultValue, m_pdata->m_dataType);
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::addScope(const CIMScope& newScope)
{
	if (newScope)
	{
		if (!hasScope(newScope))
		{
			if (newScope == CIMScope::ANY)
			{
				m_pdata->m_scope.clear();
			}
			m_pdata->m_scope.append(newScope);
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::hasScope(const CIMScope& scopeArg) const
{
	if (scopeArg)
	{
		size_t tsize = m_pdata->m_scope.size();
		for (size_t i = 0; i < tsize; i++)
		{
			if (m_pdata->m_scope[i].getScope() == scopeArg.getScope())
			{
				return true;
			}
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::hasFlavor(const CIMFlavor& flavorArg) const
{
	if (flavorArg)
	{
		size_t tsize = m_pdata->m_flavor.size();
		for (size_t i = 0; i < tsize; i++)
		{
			if (m_pdata->m_flavor[i].getFlavor() == flavorArg.getFlavor())
			{
				return true;
			}
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::addFlavor(const CIMFlavor& newFlavor)
{
	Int32 flavor = newFlavor.getFlavor();
	if (newFlavor)
	{
		if (!hasFlavor(newFlavor))
		{
			switch (flavor)
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
			m_pdata->m_flavor.append(newFlavor);
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType&
CIMQualifierType::removeFlavor(const Int32 flavor)
{
	size_t i = 0;
	while (i < m_pdata->m_flavor.size())
	{
		if (m_pdata->m_flavor[i].getFlavor() == flavor)
		{
			m_pdata->m_flavor.remove(i);
		}
		else
		{
			++i;
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMFlavorArray
CIMQualifierType::getFlavors() const
{
	return m_pdata->m_flavor;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::hasDefaultValue() const
{
	return m_pdata->m_defaultValue ? true : false;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifierType::isDefaultValueArray() const
{
	bool isra = false;
	if (m_pdata->m_defaultValue)
	{
		isra = m_pdata->m_defaultValue.isArray();
	}
	return isra;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifierType::getName() const
{
	return m_pdata->m_name.toString();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifierType::setName(const CIMName& name)
{
	m_pdata->m_name = name;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifierType::writeObject(ostream &ostrm) const
{
	CIMBase::writeSig( ostrm, OW_CIMQUALIFIERTYPESIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_dataType.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_pdata->m_scope);
	BinarySerialization::writeArray(ostrm, m_pdata->m_flavor);
	if (m_pdata->m_defaultValue)
	{
		Bool(true).writeObject(ostrm);
		m_pdata->m_defaultValue.writeObject(ostrm);
	}
	else
	{
		Bool(false).writeObject(ostrm);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifierType::readObject(istream &istrm)
{
	CIMName name;
	CIMDataType dataType(CIMNULL);
	CIMScopeArray scope;
	CIMFlavorArray flavor;
	CIMValue defaultValue(CIMNULL);
	CIMBase::readSig( istrm, OW_CIMQUALIFIERTYPESIG );
	name.readObject(istrm);
	dataType.readObject(istrm);
	BinarySerialization::readArray(istrm, scope);
	BinarySerialization::readArray(istrm, flavor);
	Bool isValue;
	isValue.readObject(istrm);
	if (isValue)
	{
		defaultValue.readObject(istrm);
	}
	if (!m_pdata)
	{
		m_pdata = new QUALTData;
	}
	m_pdata->m_name = name;
	m_pdata->m_dataType = dataType;
	m_pdata->m_scope = scope;
	m_pdata->m_flavor = flavor;
	m_pdata->m_defaultValue = defaultValue;
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
	size_t i;
	StringBuffer rv;
	rv = "Qualifier ";
	rv += m_pdata->m_name.toString();
	rv += " : ";
	rv += m_pdata->m_dataType.toMOF();
	if (m_pdata->m_dataType.isArrayType())
	{
		rv += '[';
		if (m_pdata->m_dataType.getSize() != -1) // -1 means unlimited
		{
			rv += m_pdata->m_dataType.getSize();
		}
		rv += ']';
	}
	if (m_pdata->m_defaultValue)
	{
		rv += " = ";
		rv += m_pdata->m_defaultValue.toMOF();
	}
	if (m_pdata->m_scope.size() > 0)
	{
		rv += ", Scope(";
		CIMScopeArray scopes(m_pdata->m_scope);
		std::sort(scopes.begin(), scopes.end());
		for (i = 0; i < scopes.size(); i++)
		{
			if (i > 0)
			{
				rv += ',';
			}
			rv += scopes[i].toMOF();
		}
		rv += ')';
	}
	if (m_pdata->m_flavor.size() > 0)
	{
		CIMFlavorArray toPrint;
		// first filter out the default flavors.
		for (i = 0; i < m_pdata->m_flavor.size(); i++)
		{
			if (m_pdata->m_flavor[i].getFlavor() != CIMFlavor::ENABLEOVERRIDE
				&& m_pdata->m_flavor[i].getFlavor() != CIMFlavor::TOSUBCLASS)
			{
				toPrint.push_back(m_pdata->m_flavor[i]);
			}
		}
		if (toPrint.size() > 0)
		{
			rv += ", Flavor(";
			for (i = 0; i < toPrint.size(); i++)
			{
				if (i > 0)
				{
					rv += ',';
				}
				rv += toPrint[i].toMOF();
			}
			rv += ')';
		}
	}
	rv += ";\n";
	return rv.releaseString();
}

} // end namespace OpenWBEM

