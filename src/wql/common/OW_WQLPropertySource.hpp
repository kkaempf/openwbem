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

#ifndef OW_WQL_PROPERTY_SOURCE_HPP_INCLUDE_GUARD_
#define OW_WQL_PROPERTY_SOURCE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_WQLOperand.hpp"

/** This class must be implemented to provide a source of properties for the
	WQLSelectStatement::evaluateWhereClause() method which calls methods of
	this class to obtain real values for property names used in the where
	clause.
*/
class OW_WQLPropertySource
{
public:

	/** Virtual destructor.
	*/
	virtual ~OW_WQLPropertySource() {}

	/** Returns the value whose property has the given name. The output
		parameter value is populated with the value of that parameter.
		Note that only integer, double, and string types are supported
		(see the WQLOperand class). The implementer of this method must
		perform appropriate conversions to the appropriate type.

		@param propertyName name of property to be gotten.  This can be of the form:
		  name(.subname)*
		  examples:
		  propertyName
		  className.propertyName
		  propertyName.embeddedPropertyName1
		  propertyName.embeddedPropertyName1.embeddedPropertyName2
		  className.propertyName.embeddedPropertyName1
		@param value holds the value of the property upon return.
		@return true if such a property was found and false otherwise.
	*/
	virtual bool getValue(
		const OW_String& propertyName, 
		OW_WQLOperand& value) const = 0;

	/** Returns true if the property identified by propertyName is an
		EmbeddedObject instance or class that is either className or
		is derived from className.

		@param propertyName name of property to be gotten.  This can be of the form:
		  name(.subname)*
		  examples:
		  propertyName
		  className.propertyName
		  propertyName.embeddedPropertyName1
		  propertyName.embeddedPropertyName1.embeddedPropertyName2
		  className.propertyName.embeddedPropertyName1
		@param className The name of the class against which the property is 
		  to be checked.
		@return true or false
	*/
	virtual bool evaluateISA(
		const OW_String& propertyName,
		const OW_String& className) const = 0;

};

#endif

