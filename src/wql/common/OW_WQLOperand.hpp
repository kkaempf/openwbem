////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems, Vintela, Inc..
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
////////////////////////////////////////////////////////////////////////////////
#ifndef OW_WQL_OPERAND_HPP_INCLUDE_GUARD_
#define OW_WQL_OPERAND_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_String.hpp"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(TypeMismatch);
/** Tag used to force invocation of the integer value form of the WQLOperand
  Constructor.
 */
enum WQLIntegerValueTag 
{ 
	WQL_INTEGER_VALUE_TAG 
};
/** Tag used to force invocation of the double value form of the WQLOperand
  Constructor.
 */
enum WQLDoubleValueTag 
{ 
	WQL_DOUBLE_VALUE_TAG 
};
/** Tag used to force invocation of the boolean value form of the WQLOperand
  Constructor.
 */
enum WQLBoolValueTag 
{ 
	WQL_BOOLEAN_VALUE_TAG 
};
/** Tag used to force invocation of the string value form of the WQLOperand
  Constructor.
 */
enum WQLStringValueTag 
{ 
	WQL_STRING_VALUE_TAG 
};
/** Tag used to force invocation of the property name form of the WQLOperand
  Constructor.
 */
enum WQLPropertyNameTag 
{ 
	WQL_PROPERTY_NAME_TAG 
};
/** Used to represent SQL where clause operands.
  Instances of WQLOperand are used to represent the operands of the
  SQL where clause. Instances of this class are created while parsing
  a SQL where clause and added to the WQLSelectStatement by calling the
  WQLSelectStatement::appendOperand() method. Consider the following
example:
<pre>
SELECT ratio, size, name, str
FROM MyClass
WHERE ratio &gt; 1.4 AND size = 3 AND name = "Hello" AND str IS NULL
</pre>
In this example, the following are operands:
<pre>
ratio
1.4
size
3
name
"Hello"
str
</pre>
Operands are of one of the following types:
<ul>
<li>NULL_VALUE - contains a null value of any type</li>
<li>INTEGER_VALUE - an integer literal (e.g., 10, -22)</li>
<li>DOUBLE_VALUE - a double literal (e.g., 1.4, 1.375e-5)</li>
<li>BOOLEAN_VALUE - a boolean literal (e.g., TRUE or FALSE)</li>
<li>STRING_VALUE - a string literal (e.g., "Hello World")</li>
<li>PROPERTY_NAME- the name of a property (e.g., count, size)</li>
</ul>
*/
class WQLOperand
{
public:
	/** Defines allowed types of WQL operands (NULL_VALUE, INTEGER_VALUE,
	  DOUBLE_VALUE, BOOLEAN_VALUE, STRING_VALUE, PROPERTY_NAME).
	 */
	enum Type
	{
		NULL_VALUE,
		INTEGER_VALUE,
		DOUBLE_VALUE,
		BOOLEAN_VALUE,
		STRING_VALUE,
		PROPERTY_NAME
	};
	/** Desfault constructor. Initializes to null value.
	 */
	WQLOperand()
		: _type(NULL_VALUE) 
	{
	}
	/** Initializes object as INTEGER_VALUE.
	 */
	WQLOperand(Int64 x, WQLIntegerValueTag)
	{
		_integerValue = x;
		_type = INTEGER_VALUE;
	}
	/** Initializes object as DOUBLE_VALUE.
	 */
	WQLOperand(Real64 x, WQLDoubleValueTag)
	{
		_doubleValue = x;
		_type = DOUBLE_VALUE;
	}
	/** Initializes object as BOOLEAN_VALUE.
	 */
	WQLOperand(bool x, WQLBoolValueTag)
	{
		_booleanValue = x;
		_type = BOOLEAN_VALUE;
	}
	/** Initializes object as STRING_VALUE.
	 */
	WQLOperand(const String& x, WQLStringValueTag)
	{
		_string = x;
		_type = STRING_VALUE;
	}
	/** Initializes object as PROPERTY_NAME.
	 */
	WQLOperand(const String& x, WQLPropertyNameTag)
	{
		_string = x;
		_type = PROPERTY_NAME;
	}
	/** Accessor for getting the type of the operand.
	 */
	Type getType() const { return _type; }
	/** Sets this object to an INTEGER_VALUE.
	 */
	void setIntegerValue(Int32 x)
	{
		_integerValue = x;
		_type = INTEGER_VALUE;
	}
	/** Sets this object to an DOUBLE_VALUE.
	 */
	void setDoubleValue(Real64 x)
	{
		_doubleValue = x;
		_type = DOUBLE_VALUE;
	}
	/** Sets this object to a BOOLEAN_VALUE.
	 */
	void setBoolValue(bool x)
	{
		_booleanValue = x;
		_type = BOOLEAN_VALUE;
	}
	/** Sets this object to a STRING_VALUE.
	 */
	void setStringValue(const String& x)
	{
		_string = x;
		_type = STRING_VALUE;
	}
	/** Sets this object to a PROPERTY_NAME.
	 */
	void setPropertyName(const String& x)
	{
		_string = x;
		_type = PROPERTY_NAME;
	}
	/** Gets this object as an INTEGER_VALUE.
	 */
	Int64 getIntegerValue() const
	{
		if (_type != INTEGER_VALUE)
			OW_THROW(TypeMismatchException, "Type mismatch");
		return _integerValue;
	}
	/** Gets this object as an DOUBLE_VALUE.
	  @exception TypeMismatchException is not the expected type.
	 */
	Real32 getDoubleValue() const
	{
		if (_type != DOUBLE_VALUE)
			OW_THROW(TypeMismatchException, "Type mismatch");
		return _doubleValue;
	}
	/** Gets this object as an BOOLEAN_VALUE.
	  @exception TypeMismatchException is not the expected type.
	 */
	bool getBooleanValue() const
	{
		if (_type != BOOLEAN_VALUE)
			OW_THROW(TypeMismatchException, "Type mismatch");
		return _booleanValue;
	}
	/** Gets this object as a STRING_VALUE.
	  @exception TypeMismatchException is not the expected type.
	 */
	const String& getStringValue() const
	{
		if (_type != STRING_VALUE)
			OW_THROW(TypeMismatchException, "Type mismatch");
		return _string;
	}
	/** Gets this object as a PROPERTY_NAME.
	  @exception TypeMismatchException is not the expected type.
	 */
	const String& getPropertyName() const
	{
		if (_type != PROPERTY_NAME)
			OW_THROW(TypeMismatchException, "Type mismatch");
		return _string;
	}
	/** Converts this object to a string for output purposes.
	 */
	String toString() const;
private:
	union
	{
		Int64 _integerValue;
		Real64 _doubleValue;
		bool _booleanValue;
	};
	String _string;
	Type _type;
};
bool operator==(const WQLOperand& x, const WQLOperand& y);

} // end namespace OpenWBEM

typedef OpenWBEM::WQLOperand OW_WQLOperand OW_DEPRECATED;

#endif
