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
#include "OW_CIMValueCast.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_BinIfcIO.hpp"

#include <algorithm> // for std::sort

using std::istream;
using std::ostream;

struct OW_CIMQualifierType::QUALTData
{
	OW_String m_name;
	OW_CIMDataType m_dataType;
	OW_CIMScopeArray m_scope;
	OW_CIMFlavorArray m_flavor;
	OW_CIMValue m_defaultValue;

    QUALTData* clone() const { return new QUALTData(*this); }
};

//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMQualifierType::QUALTData& x, const OW_CIMQualifierType::QUALTData& y)
{
	if (x.m_name == y.m_name)
	{
		if (x.m_dataType == y.m_dataType)
		{
			if (x.m_scope == y.m_scope)
			{
				if (x.m_flavor == y.m_flavor)
				{
					return x.m_defaultValue < y.m_defaultValue;
				}
				return x.m_flavor < y.m_flavor;
			}
			return x.m_scope < y.m_scope;
		}
		return x.m_dataType < y.m_dataType;
	}
	return x.m_name < y.m_name;
}

//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMQualifierType& x, const OW_CIMQualifierType& y)
{
	return *x.m_pdata < *y.m_pdata;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType::OW_CIMQualifierType(OW_Bool notNull) :
	OW_CIMElement(), m_pdata((notNull) ? new QUALTData : NULL)
{
	if (m_pdata)
	{
		addFlavor(OW_CIMFlavor::ENABLEOVERRIDE);
		addFlavor(OW_CIMFlavor::TOSUBCLASS);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType::OW_CIMQualifierType(const OW_String& name) :
	OW_CIMElement(), m_pdata(new QUALTData)
{
	m_pdata->m_name = name;
	addFlavor(OW_CIMFlavor::ENABLEOVERRIDE);
	addFlavor(OW_CIMFlavor::TOSUBCLASS);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType::OW_CIMQualifierType(const char* name) :
	OW_CIMElement(), m_pdata(new QUALTData)
{
	m_pdata->m_name = name;
	addFlavor(OW_CIMFlavor::ENABLEOVERRIDE);
	addFlavor(OW_CIMFlavor::TOSUBCLASS);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType::OW_CIMQualifierType(const OW_CIMQualifierType& x) :
	OW_CIMElement(), m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType::~OW_CIMQualifierType()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType&
OW_CIMQualifierType::operator= (const OW_CIMQualifierType& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
const OW_CIMScopeArray&
OW_CIMQualifierType::getScope() const
{
	return m_pdata->m_scope;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType
OW_CIMQualifierType::getDataType() const
{
	return m_pdata->m_dataType;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_CIMQualifierType::getDataSize() const
{
	return m_pdata->m_dataType.getSize();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMQualifierType::getDefaultValue() const
{
	return m_pdata->m_defaultValue;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::setDataType(const OW_CIMDataType& dataType)
{
	try
	{
		m_pdata->m_dataType = dataType;
		if(m_pdata->m_defaultValue)
		{
			m_pdata->m_defaultValue = OW_CIMValueCast::castValueToDataType(
				m_pdata->m_defaultValue, m_pdata->m_dataType);
		}
	}
	catch(...)
	{
		// Ignore?
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::setDataType(const OW_CIMDataType::Type& dataType)
{
	setDataType(OW_CIMDataType(dataType));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::setDefaultValue(const OW_CIMValue& defValue)
{
	m_pdata->m_defaultValue = defValue;
	if(m_pdata->m_defaultValue)
	{
		m_pdata->m_defaultValue = OW_CIMValueCast::castValueToDataType(
			m_pdata->m_defaultValue, m_pdata->m_dataType);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::addScope(const OW_CIMScope& newScope)
{
	if(newScope)
	{
		if(!hasScope(newScope))
		{
			if (newScope == OW_CIMScope::ANY)
			{
				m_pdata->m_scope.clear();
			}
			m_pdata->m_scope.append(newScope);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifierType::hasScope(const OW_CIMScope& scopeArg) const
{
	if(scopeArg)
	{
		size_t tsize = m_pdata->m_scope.size();
		for(size_t i = 0; i < tsize; i++)
		{
			if(m_pdata->m_scope[i].getScope() == scopeArg.getScope())
				return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifierType::hasFlavor(const OW_CIMFlavor& flavorArg) const
{
	if(flavorArg)
	{
		size_t tsize = m_pdata->m_flavor.size();
		for(size_t i = 0; i < tsize; i++)
		{
			if(m_pdata->m_flavor[i].getFlavor() == flavorArg.getFlavor())
				return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::addFlavor(const OW_CIMFlavor& newFlavor)
{
	OW_Int32 flavor = newFlavor.getFlavor();
	if(newFlavor)
	{
		if(!hasFlavor(newFlavor))
		{
			switch(flavor)
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

			m_pdata->m_flavor.append(newFlavor);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::removeFlavor(const OW_Int32 flavor)
{
	for(size_t i = 0; i < m_pdata->m_flavor.size(); i++)
	{
		if(m_pdata->m_flavor[i].getFlavor() == flavor)
		{
			m_pdata->m_flavor.remove(i--);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFlavorArray
OW_CIMQualifierType::getFlavors() const
{
	return m_pdata->m_flavor;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifierType::hasDefaultValue() const
{
	return m_pdata->m_defaultValue ? true : false;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMQualifierType::isDefaultValueArray() const
{
	OW_Bool isra = false;
	if(m_pdata->m_defaultValue)
	{
		isra = m_pdata->m_defaultValue.isArray();
	}
	return isra;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMQualifierType::getName() const
{
	return m_pdata->m_name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::setName(const OW_String& name)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMQUALIFIERTYPESIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_dataType.writeObject(ostrm);

	m_pdata->m_scope.writeObject(ostrm);
	m_pdata->m_flavor.writeObject(ostrm);

	if(m_pdata->m_defaultValue)
	{
		OW_Bool(true).writeObject(ostrm);
		m_pdata->m_defaultValue.writeObject(ostrm);
	}
	else
	{
		OW_Bool(false).writeObject(ostrm);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::readObject(istream &istrm)
{
	OW_String name;
	OW_CIMDataType dataType(OW_CIMNULL);
	OW_CIMScopeArray scope;
	OW_CIMFlavorArray flavor;
	OW_CIMValue defaultValue;

	OW_CIMBase::readSig( istrm, OW_CIMQUALIFIERTYPESIG );
	name.readObject(istrm);
	dataType.readObject(istrm);
	scope.readObject(istrm);
	flavor.readObject(istrm);

	OW_Bool isValue;
	isValue.readObject(istrm);
	if(isValue)
	{
		defaultValue.readObject(istrm);
	}

	if(m_pdata.isNull())
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
OW_String
OW_CIMQualifierType::toString() const
{
	return toMOF();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMQualifierType::toMOF() const
{
	size_t i;
	OW_StringBuffer rv;

	rv = "Qualifier ";
	rv += m_pdata->m_name;
	rv += " : ";
	rv += m_pdata->m_dataType.toMOF();

	if(m_pdata->m_dataType.isArrayType())
	{
		rv += '[';
		if (m_pdata->m_dataType.getSize() != -1) // -1 means unlimited
		{
			rv += m_pdata->m_dataType.getSize();
		}
		rv += ']';
	}

	if(m_pdata->m_defaultValue)
	{
		rv += " = ";
		rv += m_pdata->m_defaultValue.toMOF();
	}

	if(m_pdata->m_scope.size() > 0)
	{
		rv += ", Scope(";
        OW_CIMScopeArray scopes(m_pdata->m_scope);
		std::sort(scopes.begin(), scopes.end());
		for(i = 0; i < scopes.size(); i++)
		{
			if(i > 0)
			{
				rv += ',';
			}
			rv += scopes[i].toMOF();
		}
		rv += ')';
	}

	if(m_pdata->m_flavor.size() > 0)
	{
		OW_CIMFlavorArray toPrint;
		// first filter out the default flavors.
		for(i = 0; i < m_pdata->m_flavor.size(); i++)
		{
			if (m_pdata->m_flavor[i].getFlavor() != OW_CIMFlavor::ENABLEOVERRIDE
				&& m_pdata->m_flavor[i].getFlavor() != OW_CIMFlavor::TOSUBCLASS)
			{
				toPrint.push_back(m_pdata->m_flavor[i]);
			}
		}

		if (toPrint.size() > 0)
		{
			rv += ", Flavor(";
			for(i = 0; i < toPrint.size(); i++)
			{
				if(i > 0)
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



