/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_CIMDataType.hpp"
#include "OW_StringStream.hpp"
#include "OW_Assertion.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_StrictWeakOrdering.hpp"

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
struct OW_CIMDataType::DTData
{
	DTData() :
		m_type(OW_CIMDataType::CIMNULL), m_numberOfElements(0), m_sizeRange(0)
	{}

	OW_CIMDataType::Type m_type;
	OW_Int32 m_numberOfElements;
	OW_Int32 m_sizeRange;
	OW_String m_reference;

    DTData* clone() const { return new DTData(*this); }
};

//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMDataType::DTData& x, const OW_CIMDataType::DTData& y)
{
	return OW_StrictWeakOrdering(
		x.m_type, y.m_type,
		x.m_numberOfElements, y.m_numberOfElements,
		x.m_sizeRange, y.m_sizeRange,
		x.m_reference, y.m_reference);
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::OW_CIMDataType() :
	OW_CIMBase(), m_pdata(new DTData)
{
	m_pdata->m_type = CIMNULL;
	m_pdata->m_numberOfElements = 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::OW_CIMDataType(OW_CIMNULL_t) :
	OW_CIMBase(), m_pdata(NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::OW_CIMDataType(OW_CIMDataType::Type type) :
	OW_CIMBase(), m_pdata(new DTData)
{
	OW_ASSERT(type >= CIMNULL && type < MAXDATATYPE);
	m_pdata->m_type = type;
	m_pdata->m_numberOfElements = 1;
	m_pdata->m_sizeRange = SIZE_SINGLE;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::OW_CIMDataType(OW_CIMDataType::Type type, OW_Int32 size) :
	OW_CIMBase(), m_pdata(new DTData)
{
	OW_ASSERT(type >= CIMNULL && type < MAXDATATYPE);

	m_pdata->m_type = type;
	m_pdata->m_numberOfElements = (size < 1) ? -1 : size;
	m_pdata->m_sizeRange = m_pdata->m_numberOfElements >= 1 ? SIZE_LIMITED
		: SIZE_UNLIMITED;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::OW_CIMDataType(const OW_CIMDataType& arg) :
	OW_CIMBase(), m_pdata(arg.m_pdata) {}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::~OW_CIMDataType()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDataType::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType&
OW_CIMDataType::operator = (const OW_CIMDataType& arg)
{
	m_pdata = arg.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMDataType::isArrayType() const
{
	return (m_pdata->m_sizeRange != SIZE_SINGLE);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMDataType::isNumericType() const
{
	return isNumericType(m_pdata->m_type);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_CIMDataType::isNumericType(OW_CIMDataType::Type type)
{
	switch(type)
	{
		case UINT8:
		case SINT8:
		case UINT16:
		case SINT16:
		case UINT32:
		case SINT32:
		case UINT64:
		case SINT64:
		case REAL32:
		case REAL64:
			return true;
		default:
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMDataType::isReferenceType() const
{
	return (m_pdata->m_type == REFERENCE);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::Type
OW_CIMDataType::getType() const
{
	return m_pdata->m_type;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_CIMDataType::getSize() const
{
	return m_pdata->m_numberOfElements;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMDataType::getRefClassName() const
{
	return m_pdata->m_reference;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::operator OW_CIMDataType::safe_bool () const
{
	safe_bool cc = 0;
	if(!m_pdata.isNull())
	{
		cc = int(m_pdata->m_type != CIMNULL && m_pdata->m_type != INVALID)
			? &dummy::nonnull : 0;
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::safe_bool
OW_CIMDataType::operator!() const
{
	safe_bool cc = &dummy::nonnull;
	if(!m_pdata.isNull())
	{
		cc = int(m_pdata->m_type != CIMNULL && m_pdata->m_type != INVALID)
			? 0: &dummy::nonnull;
	}
	return cc;
}


//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMDataType::setToArrayType(OW_Int32 size)
{
	m_pdata->m_numberOfElements = (size < 1) ? -1 : size;
	m_pdata->m_sizeRange = m_pdata->m_numberOfElements >= 1 ? SIZE_LIMITED
		: SIZE_UNLIMITED;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMDataType::syncWithValue(const OW_CIMValue& value)
{
	if(!value && !(*this))
		return false;

	OW_Bool rv(false);

	if(m_pdata.isNull())
	{
		m_pdata = new DTData;
		m_pdata->m_type = CIMNULL;
	}

	if(!value)
	{
		m_pdata->m_type = CIMNULL;
		m_pdata->m_numberOfElements = 0;
		m_pdata->m_sizeRange = SIZE_SINGLE;
		rv = true;
	}
	else
	{
		if((m_pdata->m_type != value.getType())
			|| (isArrayType() != value.isArray()))
		{
			m_pdata->m_type = value.getType();
			m_pdata->m_sizeRange = (value.isArray()) ? SIZE_UNLIMITED : SIZE_SINGLE;
			m_pdata->m_numberOfElements = (m_pdata->m_sizeRange == SIZE_UNLIMITED)
				? -1 : 1;
			rv = true;
		}
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDataType::OW_CIMDataType(const OW_String& refClassName) :
	OW_CIMBase(), m_pdata(new DTData)
{
	m_pdata->m_type = REFERENCE;
	m_pdata->m_numberOfElements = 1;
	m_pdata->m_sizeRange = SIZE_SINGLE;
	m_pdata->m_reference = refClassName;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMDataType::equals(const OW_CIMDataType& arg) const
{
	return (m_pdata->m_type == arg.m_pdata->m_type
		&& m_pdata->m_sizeRange == arg.m_pdata->m_sizeRange);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDataType::readObject(istream &istrm)
{
	OW_UInt32 type;
	OW_UInt32 numberOfElements;
	OW_UInt32 sizeRange;
	OW_String ref;

	OW_CIMBase::readSig( istrm, OW_CIMDATATYPESIG );

	OW_BinarySerialization::readLen(istrm, type);
	OW_BinarySerialization::readLen(istrm, numberOfElements);
	OW_BinarySerialization::readLen(istrm, sizeRange);

	ref.readObject(istrm);

	if(m_pdata.isNull())
	{
		m_pdata = new DTData;
	}

	m_pdata->m_type = OW_CIMDataType::Type(type);
	m_pdata->m_numberOfElements = numberOfElements;
	m_pdata->m_sizeRange = sizeRange;
	m_pdata->m_reference = ref;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMDataType::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMDATATYPESIG );

	OW_BinarySerialization::writeLen(ostrm, m_pdata->m_type);
	OW_BinarySerialization::writeLen(ostrm, m_pdata->m_numberOfElements);
	OW_BinarySerialization::writeLen(ostrm, m_pdata->m_sizeRange);

	m_pdata->m_reference.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMDataType::toString() const
{
	switch(m_pdata->m_type)
	{
		case UINT8: return "uint8"; break;
		case SINT8: return "sint8"; break;
		case UINT16: return "uint16"; break;
		case SINT16: return "sint16"; break;
		case UINT32: return "uint32"; break;
		case SINT32: return "sint32"; break;
		case UINT64: return "uint64"; break;
		case SINT64: return "sint64"; break;
		case REAL64: return "real64"; break;
		case REAL32: return "real32"; break;
		case CHAR16: return "char16"; break;
		case STRING: return "string"; break;
		case BOOLEAN: return "boolean"; break;
		case DATETIME: return "datetime"; break;
		case REFERENCE: return "REF"; break;
		case EMBEDDEDCLASS: case EMBEDDEDINSTANCE: return "string"; break;
		default:
			return "** INVALID DATA TYPE IN OW_CIMDATATYPE - toString **";
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMDataType::toMOF() const
{
	if (m_pdata->m_type == REFERENCE)
	{
		return m_pdata->m_reference + " REF";
	}
	else
	{
		return toString();
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMDataType::Type
OW_CIMDataType::strToSimpleType(const OW_String& strType)
{
	if(strType.empty())
	{
		return INVALID;
	}

	if(strType.equalsIgnoreCase("uint8"))
		return UINT8;

	else if(strType.equalsIgnoreCase("sint8"))
		return SINT8;

	else if(strType.equalsIgnoreCase("uint16"))
		return UINT16;

	else if(strType.equalsIgnoreCase("sint16"))
		return SINT16;

	else if(strType.equalsIgnoreCase("uint32"))
		return UINT32;

	else if(strType.equalsIgnoreCase("sint32"))
		return SINT32;

	else if(strType.equalsIgnoreCase("uint64"))
		return UINT64;

	else if(strType.equalsIgnoreCase("sint64"))
		return SINT64;

	else if(strType.equalsIgnoreCase("real64"))
		return REAL64;

	else if(strType.equalsIgnoreCase("real32"))
		return REAL32;

	else if(strType.equalsIgnoreCase("char16"))
		return CHAR16;

	else if(strType.equalsIgnoreCase("string"))
		return STRING;

	else if(strType.equalsIgnoreCase("boolean"))
		return BOOLEAN;

	else if(strType.equalsIgnoreCase("datetime"))
		return DATETIME;

	else if(strType.equalsIgnoreCase("REF"))
		return REFERENCE;

	else if(strType.equalsIgnoreCase("reference"))
		return REFERENCE;

	return INVALID;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMDataType
OW_CIMDataType::getDataType(const OW_String& strType)
{
	if(strType.empty())
	{
		return OW_CIMDataType();
	}

	Type type = strToSimpleType(strType);
	if(type != INVALID)
	{
		return OW_CIMDataType(type);
	}

	return OW_CIMDataType(OW_CIMNULL);
}

bool operator<(const OW_CIMDataType& x, const OW_CIMDataType& y)
{
	return *x.m_pdata < *y.m_pdata;
}
