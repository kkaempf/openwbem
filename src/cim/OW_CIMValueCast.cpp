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
#include "OW_Format.hpp"

static OW_Bool isCompatible(OW_CIMDataType::Type from, OW_CIMDataType::Type to);

static void makeValueArray(OW_CIMValue& theValue);

static OW_CIMValue convertString(const OW_String& strValue,
	const OW_CIMDataType& dataType);

static OW_CIMValue convertArray(const OW_CIMValue& value,
	const OW_CIMDataType& dataType);

static OW_StringArray convertToStringArray(const OW_CIMValue& value,
	OW_Bool onlyOne);

//////////////////////////////////////////////////////////////////////////////
// STATIC PUBLIC
OW_CIMValue
OW_CIMValueCast::castValueToDataType(const OW_CIMValue& value,
		const OW_CIMDataType& dataType)
{
	// If NULL data type, then return NULL value.
	if(!dataType || !value)
	{
		return OW_CIMValue();
	}

	// If the OW_CIMValue is already what it needs to be, then just return it.
	if(value.getType() == dataType.getType()
		&& value.isArray() == dataType.isArrayType())
	{
		return value;
	}

	// If we can't convert to the data type specified in the dataType argument,
	// then throw an exception
	if(!isCompatible(value.getType(), dataType.getType()))
	{
		OW_String msg("Failed to convert value: ");
		msg += value.toString();
		OW_THROWCIMMSG(OW_CIMException::TYPE_MISMATCH, msg.c_str());
	}

	// If value is an array, then do special array processing
	if(value.isArray())
	{
		return convertArray(value, dataType);
	}

	// Convert value to string
	OW_String strValue = value.toString();
	OW_CIMValue cv = convertString(strValue, dataType);
	if(dataType.isArrayType())
	{
		makeValueArray(cv);
	}

	return cv;
}

//////////////////////////////////////////////////////////////////////////////
void
makeValueArray(OW_CIMValue& theValue)
{
	if(theValue.isArray())
	{
		return;
	}

	switch(theValue.getType())
	{
		case OW_CIMDataType::UINT8:
		{
			OW_UInt8 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_UInt8Array(1, v));
			break;
		}

		case OW_CIMDataType::SINT8:
		{
			OW_Int8 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_Int8Array(1, v));
			break;
		}

		case OW_CIMDataType::UINT16:
		{
			OW_UInt16 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_UInt16Array(1, v));
			break;
		}

		case OW_CIMDataType::SINT16:
		{
			OW_Int16 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_Int16Array(1, v));
			break;
		}

		case OW_CIMDataType::UINT32:
		{
			OW_UInt32 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_UInt32Array(1, v));
			break;
		}

		case OW_CIMDataType::SINT32:
		{
			OW_Int32 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_Int32Array(1, v));
			break;
		}

		case OW_CIMDataType::UINT64:
		{
			OW_UInt64 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_UInt64Array(1, v));
			break;
		}

		case OW_CIMDataType::SINT64:
		{
			OW_Int64 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_Int64Array(1, v));
			break;
		}

		case OW_CIMDataType::STRING:
		{
			OW_String v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_StringArray(1, v));
			break;
		}

		case OW_CIMDataType::BOOLEAN:
		{
			OW_Bool v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_BoolArray(1, v));
			break;
		}

		case OW_CIMDataType::REAL32:
		{
			OW_Real32 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_Real32Array(1, v));
			break;
		}

		case OW_CIMDataType::REAL64:
		{
			OW_Real64 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_Real64Array(1, v));
			break;
		}

		case OW_CIMDataType::DATETIME:
		{
			OW_CIMDateTime v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_CIMDateTimeArray(1, v));
			break;
		}

		case OW_CIMDataType::CHAR16:
		{
			OW_Char16 v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_Char16Array(1, v));
			break;
		}

		case OW_CIMDataType::REFERENCE:
		{
			OW_CIMObjectPath v;
			theValue.get(v);
			theValue = OW_CIMValue(OW_CIMObjectPathArray(1, v));
			break;
		}

		default:
			OW_THROW(OW_Exception, format("Invalid data type: %1",
				theValue.getType()).c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
isCompatible(OW_CIMDataType::Type from, OW_CIMDataType::Type to)
{
	if(from == to									// Same data types
		|| from == OW_CIMDataType::STRING	// String can convert to anything
		|| to == OW_CIMDataType::STRING)		// Anything can convert to string
	{
		return true;
	}

	if(to == OW_CIMDataType::DATETIME
		|| to == OW_CIMDataType::REFERENCE
		|| from == OW_CIMDataType::DATETIME
		|| from == OW_CIMDataType::REFERENCE)
	{
		// Only string can convert to/from these types, and neither the from or
		// to data types are a string type
		return false;
	}

	//---------
	// At this point we know we are not converting to/from any DATETIME,
	// REFERENCE or STRING data types
	//---------

	OW_Bool fromNumeric = OW_CIMDataType::isNumericType(from);
	OW_Bool toNumeric = OW_CIMDataType::isNumericType(to);

	// If we're converting to any numeric data type
	if(toNumeric
		|| to == OW_CIMDataType::CHAR16
		|| to == OW_CIMDataType::BOOLEAN)
	{
		if(fromNumeric
			|| from == OW_CIMDataType::BOOLEAN
			|| from == OW_CIMDataType::CHAR16)
		{
			return true;
		}

		return false;
	}

	OW_THROW(OW_Exception, format("Invalid to datatype: %1", to).c_str());
	return false;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
convertString(const OW_String& strValue, const OW_CIMDataType& dataType)
{
	return OW_CIMValue::createSimpleValue(dataType.toString(), strValue);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
convertArray(const OW_CIMValue& value, const OW_CIMDataType& dataType)
{
	OW_CIMValue rcv;
	OW_Bool onlyOne = !dataType.isArrayType();
	OW_StringArray strArray = convertToStringArray(value, onlyOne);
	size_t sz = strArray.size();

	if(onlyOne)
	{
		if(sz)
		{
			rcv = convertString(strArray[0], dataType);
		}

		return rcv;
	}

	switch(dataType.getType())
	{
		case OW_CIMDataType::UINT8:
		{
			OW_UInt8Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toUInt8();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::SINT8:
		{
			OW_Int8Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toInt8();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::UINT16:
		{
			OW_UInt16Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toUInt16();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::SINT16:
		{
			OW_Int16Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toInt16();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::UINT32:
		{
			OW_UInt32Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toUInt32();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::SINT32:
		{
			OW_Int32Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toInt32();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::UINT64:
		{
			OW_UInt64Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toUInt64();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::SINT64:
		{
			OW_Int64Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toInt64();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::STRING:
			rcv = OW_CIMValue(strArray);
			break;

		case OW_CIMDataType::BOOLEAN:
		{
			OW_BoolArray ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toBool();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::REAL32:
		{
			OW_Real32Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toReal32();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::REAL64:
		{
			OW_Real64Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toReal64();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::DATETIME:
		{
			OW_CIMDateTimeArray ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toDateTime();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::CHAR16:
		{
			OW_Char16Array ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = strArray[i].toChar16();
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		case OW_CIMDataType::REFERENCE:
		{
			OW_CIMObjectPathArray ra(sz);
			for(size_t i = 0; i < sz; i++)
			{
				ra[i] = OW_CIMObjectPath::parse(strArray[i]);
			}
			rcv = OW_CIMValue(ra);
			break;
		}

		default:
			OW_THROW(OW_Exception, "LOGIC ERROR");
	}

	return rcv;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
convertToStringArray(const OW_CIMValue& value, OW_Bool onlyOne)
{
	size_t rasize = (onlyOne) ? 1 : value.getArraySize();
	OW_StringArray rvra(rasize);

	switch(value.getType())
	{
		case OW_CIMDataType::UINT8:
		{
			OW_UInt8Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(OW_UInt32(ra[i]));
			}
			break;
		}

		case OW_CIMDataType::SINT8:
		{
			OW_Int8Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(OW_Int32(ra[i]));
			}
			break;
		}

		case OW_CIMDataType::UINT16:
		{
			OW_UInt16Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(OW_UInt32(ra[i]));
			}
			break;
		}

		case OW_CIMDataType::SINT16:
		{
			OW_Int16Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(OW_Int32(ra[i]));
			}
			break;
		}

		case OW_CIMDataType::UINT32:
		{
			OW_UInt32Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(ra[i]);
			}
			break;
		}

		case OW_CIMDataType::SINT32:
		{
			OW_Int32Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(ra[i]);
			}
			break;
		}

		case OW_CIMDataType::UINT64:
		{
			OW_UInt64Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(ra[i]);
			}
			break;
		}

		case OW_CIMDataType::SINT64:
		{
			OW_Int64Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(ra[i]);
			}
			break;
		}

		case OW_CIMDataType::STRING:
		{
			OW_StringArray ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i];
			}
			break;
		}

		case OW_CIMDataType::BOOLEAN:
		{
			OW_BoolArray ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i].toString();
			}
			break;
		}

		case OW_CIMDataType::REAL32:
		{
			OW_Real32Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(OW_Real64(ra[i]));
			}
			break;
		}

		case OW_CIMDataType::REAL64:
		{
			OW_Real64Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(ra[i]);
			}
			break;
		}

		case OW_CIMDataType::DATETIME:
		{
			OW_CIMDateTimeArray ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i].toString();
			}
			break;
		}

		case OW_CIMDataType::CHAR16:
		{
			OW_Char16Array ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = OW_String(ra[i]);
			}
			break;
		}

		case OW_CIMDataType::REFERENCE:
		{
			OW_CIMObjectPathArray ra;
			value.get(ra);
			for(size_t i = 0; i < rasize; i++)
			{
				rvra[i] = ra[i].toString();
			}
			break;
		}

		default:
			OW_THROW(OW_Exception, "LOGIC ERROR");
	}

	return rvra;
}



