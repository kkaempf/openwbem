//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OW_config.h"
#include "OW_WQLOperand.hpp"
#include "OW_StringBuffer.hpp"

OW_String OW_WQLOperand::toString() const
{
	OW_StringBuffer result;

	switch (_type)
	{
	case PROPERTY_NAME:
		{
			result = "PROPERTY_NAME: ";
			result += _propertyName;
			break;
		}

	case STRING_VALUE:
		{
			result = "STRING_VALUE: ";
			result += _stringValue;
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
				result += "TRUE";
			else
				result += "FALSE";

			break;
		}

	default:
		result = "NULL_VALUE";
		break;
	}

	return result.releaseString();
}



