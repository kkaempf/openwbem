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
#include "OW_WQLSelectStatement.hpp"
#include "OW_Stack.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_Assertion.hpp"

#include <iostream>


template<class T>
inline static bool _Compare(const T& x, const T& y, OW_WQLOperation op)
{
	switch (op)
	{
		case WQL_EQ: 
			return x == y;

		case WQL_NE: 
			return x != y;

		case WQL_LT: 
			return x < y;
		case WQL_LE: 
			return x <= y;

		case WQL_GT: 
			return x > y;

		case WQL_GE: 
			return x >= y;

		default:
			OW_ASSERT(0);
	}

	return false;
}

static bool _Evaluate(
		const OW_WQLOperand& lhs, 
		const OW_WQLOperand& rhs, 
		OW_WQLOperation op)
{
	switch (lhs.getType())
	{
		case OW_WQLOperand::NULL_VALUE:
			{
				// This cannot happen since expressions of the form
				// OPERAND OPERATOR NULL are converted to unary form.
				// For example: "count IS NULL" is treated as a unary
				// operation in which IS_NULL is the unary operation
				// and count is the the unary operand.

				OW_ASSERT(0);
				break;
			}

		case OW_WQLOperand::INTEGER_VALUE:
			{
				return _Compare(
						lhs.getIntegerValue(),
						rhs.getIntegerValue(),
						op);
			}

		case OW_WQLOperand::DOUBLE_VALUE:
			{
				return _Compare(
						lhs.getDoubleValue(),
						rhs.getDoubleValue(),
						op);
			}

		case OW_WQLOperand::BOOLEAN_VALUE:
			{
				return _Compare(
						lhs.getBooleanValue(),
						rhs.getBooleanValue(),
						op);
			}

		case OW_WQLOperand::STRING_VALUE:
			{
				return _Compare(
						lhs.getStringValue(),
						rhs.getStringValue(),
						op);
			}

		default:
			OW_ASSERT(0);
	}

	return false;
}

OW_WQLSelectStatement::OW_WQLSelectStatement()
{
	_operations.reserve(16);
	_operands.reserve(16);
}

OW_WQLSelectStatement::~OW_WQLSelectStatement()
{

}

void OW_WQLSelectStatement::clear()
{
	_className.erase();
	_selectPropertyNames.clear();
	_operations.clear();
	_operands.clear();
}

bool OW_WQLSelectStatement::appendWherePropertyName(const OW_String& x)
{
	//
	// Reject duplicate property names by returning false.
	//

	for (size_t i = 0, n = _wherePropertyNames.size(); i < n; i++)
	{
		if (_wherePropertyNames[i] == x)
			return false;
	}

	//
	// Append the new property.
	//

	_wherePropertyNames.append(x);
	return true;
}

static inline void _ResolveProperty(
		OW_WQLOperand& op,
		const OW_WQLPropertySource* source)
{
	//
	// Resolve the operand: if it's a property name, look up its value:
	//

	if (op.getType() == OW_WQLOperand::PROPERTY_NAME)
	{
		const OW_String& propertyName = op.getPropertyName();

		if (!source->getValue(propertyName, op))
			OW_THROW(OW_NoSuchPropertyException, propertyName.c_str());
	}
}

bool OW_WQLSelectStatement::evaluateWhereClause(
		const OW_WQLPropertySource* source) const
{
	if (!hasWhereClause())
		return true;

	OW_WQLSelectStatement* that = (OW_WQLSelectStatement*)this;
	OW_Stack<bool> stack;
	//stack.reserve(16);

	// 
	// Counter for operands:
	//

	OW_UInt32 j = 0;

	//
	// Process each of the operations:
	//

	for (OW_UInt32 i = 0, n = _operations.size(); i < n; i++)
	{
		OW_WQLOperation op = _operations[i];

		switch (op)
		{
			case WQL_OR:
				{
					OW_ASSERT(stack.size() >= 2);

					bool op1 = stack.top();
					stack.pop();

					bool op2 = stack.top();

					stack.top() = op1 || op2;
					break;
				}

			case WQL_AND:
				{
					OW_ASSERT(stack.size() >= 2);

					bool op1 = stack.top();
					stack.pop();

					bool op2 = stack.top();

					stack.top() = op1 && op2;
					break;
				}

			case WQL_NOT:
				{
					OW_ASSERT(stack.size() >= 1);

					bool op = stack.top();
					stack.top() = !op;
					break;
				}

			case WQL_EQ:
			case WQL_NE:
			case WQL_LT:
			case WQL_LE:
			case WQL_GT:
			case WQL_GE:
				{
					OW_ASSERT(_operands.size() >= 2);

					//
					// Resolve the left-hand-side to a value (if not already
					// a value).
					//

					OW_WQLOperand& lhs = that->_operands[j++];
					_ResolveProperty(lhs, source);

					//
					// Resolve the right-hand-side to a value (if not already
					// a value).
					//

					OW_WQLOperand& rhs = that->_operands[j++];
					_ResolveProperty(rhs, source);

					//
					// Check for a type mismatch:
					//

					// PEGASUS_OUT(lhs.toString());
					// PEGASUS_OUT(rhs.toString());

					if (rhs.getType() != lhs.getType())
						OW_THROW(OW_TypeMismatchException, "");

					//
					// Now that the types are known to be alike, apply the
					// operation:
					//

					stack.push(_Evaluate(lhs, rhs, op));
					break;
				}

			case WQL_IS_TRUE:
			case WQL_IS_NOT_FALSE:
				{
					OW_ASSERT(stack.size() >= 1);
					break;
				}

			case WQL_IS_FALSE:
			case WQL_IS_NOT_TRUE:
				{
					OW_ASSERT(stack.size() >= 1);
					stack.top() = !stack.top();
					break;
				}

			case WQL_IS_NULL:
				{
					OW_ASSERT(_operands.size() >= 1);
					OW_WQLOperand& op = that->_operands[j++];
					_ResolveProperty(op, source);
					stack.push(op.getType() == OW_WQLOperand::NULL_VALUE);
					break;
				}

			case WQL_IS_NOT_NULL:
				{
					OW_ASSERT(_operands.size() >= 1);
					OW_WQLOperand& op = that->_operands[j++];
					_ResolveProperty(op, source);
					stack.push(op.getType() != OW_WQLOperand::NULL_VALUE);
					break;
				}
			case WQL_ISA:
				{
					// TODO!
					break;
				}
		}
	}

	OW_ASSERT(stack.size() == 1);
	return stack.top();
}

void OW_WQLSelectStatement::print(std::ostream& ostr) const
{
	//
	// Print the header:
	//

	ostr << "OW_WQLSelectStatement\n";
	ostr << "{\n";

	//
	// Print the class name:
	//

	ostr << "	_className: \"" << _className << "\"\n";

	// 
	// Print the property:
	//

	for (size_t i = 0; i < _selectPropertyNames.size(); i++)
	{
		if (i == 0)
			ostr << '\n';

		ostr << "	_selectPropertyNames[" << i << "]: ";
		ostr << '"' << _selectPropertyNames[i] << '"' << '\n';
	}

	//
	// Print the operations:
	//

	for (size_t i = 0; i < _operations.size(); i++)
	{
		if (i == 0)
			ostr << '\n';

		ostr << "	_operations[" << i << "]: ";
		ostr << '"' << OW_WQLOperationToString(_operations[i]) << '"' << '\n';
	}

	//
	// Print the operands:
	//

	for (size_t i = 0; i < _operands.size(); i++)
	{
		if (i == 0)
			ostr << '\n';

		ostr << "	_operands[" << i << "]: ";
		ostr << '"' << _operands[i].toString() << '"' << '\n';
	}

	//
	// Print the trailer:
	//

	ostr << "}" << std::endl;
}

void OW_WQLSelectStatement::compileWhereClause(
		const OW_WQLPropertySource* /*source*/, OW_WQLCompile& wcl)
{
	wcl.compile(this);
}


