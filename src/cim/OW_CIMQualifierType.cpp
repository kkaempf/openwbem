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
#include "OW_MutexLock.hpp"

using std::istream;
using std::ostream;

struct OW_CIMQualifierType::QUALTData
{
	QUALTData()
		: m_name()
		, m_dataType(true)
		, m_scope()
		, m_flavor()
		, m_defaultValue(OW_Bool(true))
	{
	}

	QUALTData(const QUALTData& x)
		: m_name(x.m_name)
		, m_dataType(x.m_dataType)
		, m_scope(x.m_scope)
		, m_flavor(x.m_flavor)
		, m_defaultValue(x.m_defaultValue)
	{
	}

	QUALTData& operator= (const QUALTData& x)
	{
		m_name = x.m_name;
		m_dataType = x.m_dataType;
		m_scope = x.m_scope;
		m_flavor = x.m_flavor;
		m_defaultValue = x.m_defaultValue;
		return *this;
	}

	OW_String m_name;
	OW_CIMDataType m_dataType;
	OW_CIMScopeArray m_scope;
	OW_CIMFlavorArray m_flavor;
	OW_CIMValue m_defaultValue;
};

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType::OW_CIMQualifierType(OW_Bool notNull) :
	OW_CIMElement(), m_pdata((notNull) ? new QUALTData : NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType::OW_CIMQualifierType(const OW_String& name) :
	OW_CIMElement(), m_pdata(new QUALTData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType::OW_CIMQualifierType(const char* name) :
	OW_CIMElement(), m_pdata(new QUALTData)
{
	m_pdata->m_name = name;
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
int
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
	OW_MutexLock l = m_pdata.getWriteLock();
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
OW_CIMQualifierType::setDefaultValue(const OW_CIMValue& defValue)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	try
	{
		m_pdata->m_defaultValue = defValue;
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
OW_CIMQualifierType::addScope(const OW_CIMScope& newScope)
{
	if(newScope)
	{
		OW_MutexLock l = m_pdata.getWriteLock();
		if(!hasScope(newScope))
		{
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
	int flavor = newFlavor.getFlavor();
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

			OW_MutexLock l = m_pdata.getWriteLock();
			m_pdata->m_flavor.append(newFlavor);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::removeFlavor(const int flavor)
{
	OW_MutexLock l = m_pdata.getWriteLock();
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
	return OW_Bool((void*)m_pdata->m_defaultValue != NULL);
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
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMQualifierType::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMQUALIFIERTYPESIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_dataType.writeObject(ostrm);

	size_t len = m_pdata->m_scope.size();
	int nl = OW_hton32(len);
	if(!ostrm.write((const char*)&nl, sizeof(nl)))
	{
		OW_THROW(OW_IOException, "failed to write len of scope array");
	}

	for(size_t i = 0; i < len; i++)
	{
		m_pdata->m_scope[i].writeObject(ostrm);
	}

	len = m_pdata->m_flavor.size();
	nl = OW_hton32(len);
	if(!ostrm.write((const char*)&nl, sizeof(nl)))
	{
		OW_THROW(OW_IOException, "failed to write len of flavor array");
	}

	for(size_t i = 0; i < len; i++)
	{
		m_pdata->m_flavor[i].writeObject(ostrm);
	}

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
	OW_CIMDataType dataType;
	OW_CIMScopeArray scope;
	OW_CIMFlavorArray flavor;
	OW_CIMValue defaultValue;
	size_t len;

	OW_CIMBase::readSig( istrm, OW_CIMQUALIFIERTYPESIG );
	name.readObject(istrm);
	dataType.readObject(istrm);

	if(!istrm.read((char*)&len, sizeof(len)))
	{
		OW_THROW(OW_IOException, "failed to read len of scope array");
	}
	len = OW_ntoh32(len);

	for(size_t i = 0; i < len; i++)
	{
		OW_CIMScope s;
		s.readObject(istrm);
		scope.append(s);
	}

	if(!istrm.read((char*)&len, sizeof(len)))
	{
		OW_THROW(OW_IOException, "failed to read len of flavor array");
	}
	len = OW_ntoh32(len);

	for(size_t i = 0; i < len; i++)
	{
		OW_CIMFlavor f;
		f.readObject(istrm);
		flavor.append(f);
	}

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

	OW_MutexLock l = m_pdata.getWriteLock();
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

	rv += " : ";
	rv += m_pdata->m_dataType.toMOF();

	if(m_pdata->m_dataType.isArrayType())
	{
		rv += '[';
		rv += m_pdata->m_dataType.getSize() + ']';
	}

	if(m_pdata->m_defaultValue)
	{
		rv += '=';
		rv += m_pdata->m_defaultValue.toMOF();
	}

	if(m_pdata->m_scope.size() > 0)
	{
		rv += ",scope(";

		for(i = 0; i < m_pdata->m_scope.size(); i++)
		{
			if(i > 0)
			{
				rv += ',';
			}
			rv += m_pdata->m_scope[i].toMOF();
		}
		rv += ')';
	}

	if(m_pdata->m_flavor.size() > 0)
	{
		for(i = 0; i < m_pdata->m_flavor.size(); i++)
		{
			if(i > 0)
			{
				rv += ',';
			}
			rv += m_pdata->m_flavor[i].toMOF();
		}
		rv += ')';
	}

	rv += ";\n";
	return rv.toString();
}

//////////////////////////////////////////////////////////////////////////////
/*
void
OW_CIMQualifierType::toXML(ostream& ostr) const
{
	OW_CIMFlavor fv;

	if(m_pdata->m_name.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "qualifierType must have a name");
	}

	//
	// If type isn't set then the CIMOM has stored a qualifier
	// thats bad and an exception is generated
	//
	if(!m_pdata->m_dataType)
	{
		OW_String msg("QualifierType (");
		msg += m_pdata->m_name;
		msg += ") does not have a data type set";
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}

	ostr
		<< "<QUALIFIER.DECLARATION NAME=\""
		<< m_pdata->m_name
		<< "\" TYPE=\"";

	m_pdata->m_dataType.toXML(ostr);
	ostr << "\" ";

	if(m_pdata->m_dataType.isArrayType())
	{
		ostr << "ISARRAY=\"true\" ";
	}
	else
	{
		ostr << "ISARRAY=\"false\" ";
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::ENABLEOVERRIDE);
	if(hasFlavor(fv))
	{
		// NOT NECESSARY, default is TRUE
		// text += fv.toXML() + "=\"true\" ";
	}
	else
	{
		fv = OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE);
		if(hasFlavor(fv))
		{
			fv.toXML(ostr);
			ostr << "=\"false\" ";
		}
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS);
	if(hasFlavor(fv))
	{
		// NOT NECESSARY, default is TRUE
		// text += fv.toXML() + "=\"true\" ";
	}
	else
	{
		fv = OW_CIMFlavor(OW_CIMFlavor::RESTRICTED);
		if(hasFlavor(fv))
		{
			fv.toXML(ostr);
			ostr << "=\"false\" ";
		}
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TOINSTANCE);
	if(hasFlavor(fv))
	{
		fv.toXML(ostr);
		ostr << "=\"true\" ";
	}
	else
	{
		//
		// Not needed, because TOINSTANCE defaults to false!
		//text += fv.toXML() + "=\"false\" ";
	}


	fv = OW_CIMFlavor(OW_CIMFlavor::TRANSLATE);
	if(hasFlavor(fv))
	{
		fv.toXML(ostr);
		ostr << "=\"true\" ";
	}
	//
	// NOT NECESSARY, default is FALSE
	//else
	//  text += fv.toXML() + "=\"false\" ";

	ostr << "><SCOPE ";

	//
	// Write scope information
	//
	OW_String scope;
	OW_Bool scopeWritten = false;
	OW_Bool any = hasScope(OW_CIMScope(OW_CIMScope::ANY));
	if(any || hasScope(OW_CIMScope(OW_CIMScope::CLASS)))
	{
		ostr << "CLASS=\"true\" ";
		scopeWritten = true;
	}
	if(any || hasScope(OW_CIMScope(OW_CIMScope::INSTANCE)))
	{
		ostr << "INSTANCE=\"true\" ";
		scopeWritten = true;
	}
	if(any || hasScope(OW_CIMScope(OW_CIMScope::ASSOCIATION)))
	{
		ostr << "ASSOCIATION=\"true\" ";
		scopeWritten = true;
	}
	if(any || hasScope(OW_CIMScope(OW_CIMScope::REFERENCE)))
	{
		ostr << "REFERENCE=\"true\" ";
		scopeWritten = true;
	}
	if(any || hasScope(OW_CIMScope(OW_CIMScope::PROPERTY)))
	{
		ostr << "PROPERTY=\"true\" ";
		scopeWritten = true;
	}
	if(any || hasScope(OW_CIMScope(OW_CIMScope::METHOD)))
	{
		ostr << "METHOD=\"true\" ";
		scopeWritten = true;
	}
	if(any || hasScope(OW_CIMScope(OW_CIMScope::PARAMETER)))
	{
		ostr << "PARAMETER=\"true\" ";
		scopeWritten = true;
	}
	if(any || hasScope(OW_CIMScope(OW_CIMScope::INDICATION)))
	{
		ostr << "INDICATION=\"true\" ";
		scopeWritten = true;
	}

	if(!scopeWritten)
	{
		OW_String msg("Scope not set on qaulifier type: ");
		msg += m_pdata->m_name;
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}

	ostr << "></SCOPE>";

	if(m_pdata->m_defaultValue)
	{
		m_pdata->m_defaultValue.toXML(ostr);
	}

	ostr << "</QUALIFIER.DECLARATION>";
}
*/


