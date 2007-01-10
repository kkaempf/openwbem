//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
// Portions Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Dan Nuffer
//
//%/////////////////////////////////////////////////////////////////////////////
#include "OW_config.h"
#include "OW_WQLOperand.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_BinarySerialization.hpp"

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(TypeMismatch);
String WQLOperand::toString() const
{
	StringBuffer result;
	switch (_type)
	{
	case PROPERTY_NAME:
		{
			result = "PROPERTY_NAME: ";
			result += _string;
			break;
		}
	case STRING_VALUE:
		{
			result = "STRING_VALUE: ";
			result += _string;
			break;
		}
	case INTEGER_VALUE:
		{
			result = "INTEGER_VALUE: ";
			result += _integerValue;
			break;
		}
	case DOUBLE_VALUE:
		{
			result = "DOUBLE_VALUE: ";
			result += _doubleValue;
			break;
		} 
	case BOOLEAN_VALUE:
		{
			result = "BOOLEAN_VALUE: ";
			if (_booleanValue)
			{
				result += "TRUE";
			}
			else
			{
				result += "FALSE";
			}
			break;
		}
	default:
		result = "NULL_VALUE";
		break;
	}
	return result.releaseString();
}
bool operator==(const WQLOperand& x, const WQLOperand& y)
{
	if (x.getType() != y.getType())
	{
		return false;
	}
	switch (x.getType())
	{
		case WQLOperand::NULL_VALUE:
			return true;
		case WQLOperand::INTEGER_VALUE:
			return x.getIntegerValue() == y.getIntegerValue();
		case WQLOperand::DOUBLE_VALUE:
			return x.getDoubleValue() == y.getDoubleValue();
		case WQLOperand::BOOLEAN_VALUE:
			return x.getBooleanValue() == y.getBooleanValue();
		case WQLOperand::STRING_VALUE:
			return x.getStringValue() == y.getStringValue();
		case WQLOperand::PROPERTY_NAME:
			return x.getPropertyName() == y.getPropertyName();
	}
	return false;
}
	
void
WQLOperand::readObject(std::streambuf & istrm)
{
	UInt8 ltype;
	BinarySerialization::read(istrm, ltype);
	switch (ltype)
	{
		case NULL_VALUE:
			break;

		case INTEGER_VALUE:
			{
				Int64 integerValue;
				BinarySerialization::read(istrm, integerValue);
				_integerValue = integerValue;
			}
			break;

		case DOUBLE_VALUE:
			{
				String dv;
				dv.readObject(istrm);
				_doubleValue = dv.toReal64();
			}
			break;

		case BOOLEAN_VALUE:
			{
				Bool bv;
				bv.readObject(istrm);
				_booleanValue = bv;
			}
			break;

		case STRING_VALUE:
		case PROPERTY_NAME:
			{
				String s;
				s.readObject(istrm);
				_string.swap(s);
			}
			break;
	}
	_type = Type(ltype);
}

void
WQLOperand::writeObject(std::streambuf & ostrm) const
{
	UInt8 typeAsUInt8 = _type;
	BinarySerialization::write(ostrm, typeAsUInt8);
	switch (_type)
	{
		case NULL_VALUE:
			break;

		case INTEGER_VALUE:
			BinarySerialization::write(ostrm, _integerValue);
			break;

		case DOUBLE_VALUE:
			{
				String dv(_doubleValue);
				dv.writeObject(ostrm);
			}
			break;

		case BOOLEAN_VALUE:
			Bool(_booleanValue).writeObject(ostrm);
			break;

		case STRING_VALUE:
		case PROPERTY_NAME:
			_string.writeObject(ostrm);
			break;

	}

}

} // end namespace OW_NAMESPACE

