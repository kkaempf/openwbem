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
#include "OW_CIMValueCast.hpp"
#include "OW_Format.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ExceptionIds.hpp"

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(ValueCast);
static bool isCompatible(CIMDataType::Type from, CIMDataType::Type to);
static void makeValueArray(CIMValue& theValue);
static CIMValue convertString(const String& strValue,
	const CIMDataType& dataType);
static CIMValue convertArray(const CIMValue& value,
	const CIMDataType& dataType);
static StringArray convertToStringArray(const CIMValue& value,
	bool onlyOne);

//////////////////////////////////////////////////////////////////////////////
namespace CIMValueCast
{

CIMValue
castValueToDataType(const CIMValue& value,
		const CIMDataType& dataType)
{
	try
	{
		// If NULL data type, then return NULL value.
		if (!dataType || !value)
		{
			return CIMValue(CIMNULL);
		}
		// If the CIMValue is already what it needs to be, then just return it.
		if (value.getType() == dataType.getType()
			&& value.isArray() == dataType.isArrayType())
		{
			return value;
		}
		// If we can't convert to the data type specified in the dataType argument,
		// then throw an exception
		if (!isCompatible(value.getType(), dataType.getType()))
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				Format("Failed to convert \"%1\" to %2", value.toString(),
					dataType.toString()).c_str());
		}
		// If value is an array, then do special array processing
		if (value.isArray())
		{
			return convertArray(value, dataType);
		}
		// Convert value to string
		String strValue = value.toString();
		CIMValue cv(CIMNULL);
		cv = convertString(strValue, dataType);
		if (dataType.isArrayType())
		{
			makeValueArray(cv);
		}
		return cv;
	}
	catch (const StringConversionException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
} // end namespace CIMValueCast

//////////////////////////////////////////////////////////////////////////////
void
makeValueArray(CIMValue& theValue)
{
	if (theValue.isArray())
	{
		return;
	}
	switch (theValue.getType())
	{
		case CIMDataType::UINT8:
		{
			UInt8 v;
			theValue.get(v);
			theValue = CIMValue(UInt8Array(1, v));
			break;
		}
		case CIMDataType::SINT8:
		{
			Int8 v;
			theValue.get(v);
			theValue = CIMValue(Int8Array(1, v));
			break;
		}
		case CIMDataType::UINT16:
		{
			UInt16 v;
			theValue.get(v);
			theValue = CIMValue(UInt16Array(1, v));
			break;
		}
		case CIMDataType::SINT16:
		{
			Int16 v;
			theValue.get(v);
			theValue = CIMValue(Int16Array(1, v));
			break;
		}
		case CIMDataType::UINT32:
		{
			UInt32 v;
			theValue.get(v);
			theValue = CIMValue(UInt32Array(1, v));
			break;
		}
		case CIMDataType::SINT32:
		{
			Int32 v;
			theValue.get(v);
			theValue = CIMValue(Int32Array(1, v));
			break;
		}
		case CIMDataType::UINT64:
		{
			UInt64 v;
			theValue.get(v);
			theValue = CIMValue(UInt64Array(1, v));
			break;
		}
		case CIMDataType::SINT64:
		{
			Int64 v;
			theValue.get(v);
			theValue = CIMValue(Int64Array(1, v));
			break;
		}
		case CIMDataType::STRING:
		{
			String v;
			theValue.get(v);
			theValue = CIMValue(StringArray(1, v));
			break;
		}
		case CIMDataType::BOOLEAN:
		{
			Bool v;
			theValue.get(v);
			theValue = CIMValue(BoolArray(1, v));
			break;
		}
		case CIMDataType::REAL32:
		{
			Real32 v;
			theValue.get(v);
			theValue = CIMValue(Real32Array(1, v));
			break;
		}
		case CIMDataType::REAL64:
		{
			Real64 v;
			theValue.get(v);
			theValue = CIMValue(Real64Array(1, v));
			break;
		}
		case CIMDataType::DATETIME:
		{
			CIMDateTime v(CIMNULL);
			theValue.get(v);
			theValue = CIMValue(CIMDateTimeArray(1, v));
			break;
		}
		case CIMDataType::CHAR16:
		{
			Char16 v;
			theValue.get(v);
			theValue = CIMValue(Char16Array(1, v));
			break;
		}
		case CIMDataType::REFERENCE:
		{
			CIMObjectPath v(CIMNULL);
			theValue.get(v);
			theValue = CIMValue(CIMObjectPathArray(1, v));
			break;
		}
		default:
			OW_THROW(ValueCastException, Format("Invalid data type: %1",
				theValue.getType()).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
bool
isCompatible(CIMDataType::Type from, CIMDataType::Type to)
{
	if (from == to									// Same data types
		|| from == CIMDataType::STRING	// String can convert to anything
		|| to == CIMDataType::STRING)		// Anything can convert to string
	{
		return true;
	}
	if (to == CIMDataType::DATETIME
		|| to == CIMDataType::REFERENCE
		|| from == CIMDataType::DATETIME
		|| from == CIMDataType::REFERENCE)
	{
		// Only string can convert to/from these types, and neither the from or
		// to data types are a string type
		return false;
	}
	//---------
	// At this point we know we are not converting to/from any DATETIME,
	// REFERENCE or STRING data types
	//---------
	bool fromNumeric = CIMDataType::isNumericType(from);
	bool toNumeric = CIMDataType::isNumericType(to);
	// If we're converting to any numeric data type
	if (toNumeric
		|| to == CIMDataType::CHAR16
		|| to == CIMDataType::BOOLEAN)
	{
		if (fromNumeric
			|| from == CIMDataType::BOOLEAN
			|| from == CIMDataType::CHAR16)
		{
			return true;
		}
		return false;
	}
	OW_THROW(ValueCastException, Format("Invalid to datatype: %1", to).c_str());
	return false;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
convertString(const String& strValue, const CIMDataType& dataType)
{
	return CIMValue::createSimpleValue(dataType.toString(), strValue);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
convertArray(const CIMValue& value, const CIMDataType& dataType)
{
	CIMValue rcv(CIMNULL);
	bool onlyOne = !dataType.isArrayType();
	StringArray strArray = convertToStringArray(value, onlyOne);
	size_t sz = strArray.size();
	if (onlyOne)
	{
		if (sz)
		{
			rcv = convertString(strArray[0], dataType);
		}
		return rcv;
	}
	switch (dataType.getType())
	{
		case CIMDataType::UINT8:
		{
			UInt8Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toUInt8();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::SINT8:
		{
			Int8Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toInt8();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::UINT16:
		{
			UInt16Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toUInt16();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::SINT16:
		{
			Int16Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toInt16();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::UINT32:
		{
			UInt32Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toUInt32();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::SINT32:
		{
			Int32Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toInt32();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::UINT64:
		{
			UInt64Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toUInt64();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::SINT64:
		{
			Int64Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toInt64();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::STRING:
			rcv = CIMValue(strArray);
			break;
		case CIMDataType::BOOLEAN:
		{
			BoolArray ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toBool();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::REAL32:
		{
			Real32Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toReal32();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::REAL64:
		{
			Real64Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toReal64();
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::DATETIME:
		{
			CIMDateTimeArray ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = CIMDateTime(strArray[i]);
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::CHAR16:
		{
			Char16Array ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = Char16(strArray[i]);
			}
			rcv = CIMValue(ra);
			break;
		}
		case CIMDataType::REFERENCE:
		{
			CIMObjectPathArray ra(sz);
			for (size_t i = 0; i < sz; i++)
			{
				ra[i] = CIMObjectPath::parse(strArray[i]);
			}
			rcv = CIMValue(ra);
			break;
		}
		default:
			OW_THROW(ValueCastException, "LOGIC ERROR");
	}
	return rcv;
}
//////////////////////////////////////////////////////////////////////////////
StringArray
convertToStringArray(const CIMValue& value, bool onlyOne)
{
	size_t rasize = (onlyOne) ? 1 : value.getArraySize();
	StringArray rvra(rasize);
	switch (value.getType())
	{
		case CIMDataType::UINT8:
		{
			UInt8Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(UInt32(ra[i]));
			}
			break;
		}
		case CIMDataType::SINT8:
		{
			Int8Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(Int32(ra[i]));
			}
			break;
		}
		case CIMDataType::UINT16:
		{
			UInt16Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(UInt32(ra[i]));
			}
			break;
		}
		case CIMDataType::SINT16:
		{
			Int16Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(Int32(ra[i]));
			}
			break;
		}
		case CIMDataType::UINT32:
		{
			UInt32Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(ra[i]);
			}
			break;
		}
		case CIMDataType::SINT32:
		{
			Int32Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(ra[i]);
			}
			break;
		}
		case CIMDataType::UINT64:
		{
			UInt64Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(ra[i]);
			}
			break;
		}
		case CIMDataType::SINT64:
		{
			Int64Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(ra[i]);
			}
			break;
		}
		case CIMDataType::STRING:
		{
			StringArray ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i];
			}
			break;
		}
		case CIMDataType::BOOLEAN:
		{
			BoolArray ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i].toString();
			}
			break;
		}
		case CIMDataType::REAL32:
		{
			Real32Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(Real64(ra[i]));
			}
			break;
		}
		case CIMDataType::REAL64:
		{
			Real64Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = String(ra[i]);
			}
			break;
		}
		case CIMDataType::DATETIME:
		{
			CIMDateTimeArray ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i].toString();
			}
			break;
		}
		case CIMDataType::CHAR16:
		{
			Char16Array ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i].toString();
			}
			break;
		}
		case CIMDataType::REFERENCE:
		{
			CIMObjectPathArray ra;
			value.get(ra);
			for (size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i].toString();
			}
			break;
		}
		default:
			OW_THROW(ValueCastException, "LOGIC ERROR");
	}
	return rvra;
}

} // end namespace OW_NAMESPACE

