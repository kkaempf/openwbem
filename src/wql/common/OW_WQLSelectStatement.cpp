//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
// Portions Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_StringBuffer.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_Stack.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_Assertion.hpp"

#include <iostream>


// TODO: Merge this code with OW_WQLCompile.cpp, it's all duplicated.
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
				// return true if the op is WQL_EQ and the rhs is NULL
				// also if op is WQL_NE and rhs is not NULL
				return !(op == WQL_EQ) ^ (rhs.getType() == OW_WQLOperand::NULL_VALUE);
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
	_operStack.reserve(32);
}

OW_WQLSelectStatement::~OW_WQLSelectStatement()
{

}

void OW_WQLSelectStatement::clear()
{
	_className.erase();
	_selectPropertyNames.clear();
	_operStack.clear();
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

		// it's up to the source to handle embedded properties.
		if (!source->getValue(propertyName, op))
			OW_THROW(OW_NoSuchPropertyException, propertyName.c_str());
	}
}

bool OW_WQLSelectStatement::evaluateWhereClause(
		const OW_WQLPropertySource* source) const
{
	if (!hasWhereClause())
		return true;

	OW_Stack<OW_WQLOperand> stack;

	//
	// Process each of the operations:
	//

	for (OW_UInt32 i = 0, n = _operStack.size(); i < n; i++)
	{
		const OW_WQLSelectStatement::OperandOrOperation& curItem = _operStack[i];
		if (curItem.m_type == OW_WQLSelectStatement::OperandOrOperation::OPERAND)
		{
			// put it onto the stack
			stack.push(curItem.m_operand);
		}
		else
		{
			OW_WQLOperation op = curItem.m_operation;

			switch (op)
			{
				case WQL_OR:
					{
						OW_ASSERT(stack.size() >= 2);

						OW_WQLOperand op1 = stack.top();
						stack.pop();

						OW_WQLOperand& op2 = stack.top();

						bool b1 = op1.getBooleanValue();
						bool b2 = op2.getBooleanValue();
						stack.top() = OW_WQLOperand(b1 || b2, WQL_BOOLEAN_VALUE_TAG);
						break;
					}

				case WQL_AND:
					{
						OW_ASSERT(stack.size() >= 2);

						OW_WQLOperand op1 = stack.top();
						stack.pop();

						OW_WQLOperand& op2 = stack.top();

						bool b1 = op1.getBooleanValue();
						bool b2 = op2.getBooleanValue();
						stack.top() = OW_WQLOperand(b1 && b2, WQL_BOOLEAN_VALUE_TAG);
						break;
					}

				case WQL_NOT:
					{
						OW_ASSERT(stack.size() >= 1);

						OW_WQLOperand& op = stack.top();
						bool b1 = op.getBooleanValue();
						stack.top() = OW_WQLOperand(!b1, WQL_BOOLEAN_VALUE_TAG);
						break;
					}

				case WQL_EQ:
				case WQL_NE:
				case WQL_LT:
				case WQL_LE:
				case WQL_GT:
				case WQL_GE:
					{
						OW_ASSERT(stack.size() >= 2);

						//
						// Resolve the left-hand-side to a value (if not already
						// a value).
						//

						OW_WQLOperand lhs = stack.top();
						stack.pop();
						_ResolveProperty(lhs, source);

						//
						// Resolve the right-hand-side to a value (if not already
						// a value).
						//

						OW_WQLOperand& rhs = stack.top();
						_ResolveProperty(rhs, source);

						//
						// Check for a type mismatch:
						//

						if (rhs.getType() != lhs.getType())
							OW_THROW(OW_TypeMismatchException, "");

						//
						// Now that the types are known to be alike, apply the
						// operation:
						//

						stack.top() = OW_WQLOperand(_Evaluate(lhs, rhs, op), WQL_BOOLEAN_VALUE_TAG);
						break;
					}

				case WQL_ISA:
					{
						OW_ASSERT(stack.size() >= 2);

						OW_WQLOperand lhs = stack.top();
						stack.pop();
						if (lhs.getType() != OW_WQLOperand::PROPERTY_NAME)
						{
							OW_THROW(OW_TypeMismatchException, "First argument of ISA must be a property name");
						}

						OW_WQLOperand& rhs = stack.top();
						OW_String className;
						if (rhs.getType() == OW_WQLOperand::PROPERTY_NAME)
						{
							className = rhs.getPropertyName();
						}
						else if (rhs.getType() == OW_WQLOperand::STRING_VALUE)
						{
							className = rhs.getStringValue();
						}
						else
						{
							OW_THROW(OW_TypeMismatchException, "Second argument of ISA must be a property name or string constant");
						}

						stack.top() = OW_WQLOperand(source->evaluateISA(lhs.getPropertyName(), className), WQL_BOOLEAN_VALUE_TAG);
						break;
						break;
					}

				case WQL_DO_NOTHING:
					{
						OW_ASSERT(0); // should never happen
						break;
					}

			}
		}
	}

	OW_ASSERT(stack.size() == 1);
	return stack.top().getBooleanValue();
}

void OW_WQLSelectStatement::print(std::ostream& ostr) const
{
	ostr << "OW_WQLSelectStatement\n";
	ostr << "{\n";

	ostr << "	_className: \"" << _className << "\"\n";

	for (size_t i = 0; i < _selectPropertyNames.size(); i++)
	{
		if (i == 0)
			ostr << '\n';

		ostr << "	_selectPropertyNames[" << i << "]: ";
		ostr << '"' << _selectPropertyNames[i] << '"' << '\n';
	}

	// Print the operations/operands
	for (size_t i = 0; i < _operStack.size(); i++)
	{
		if (i == 0)
			ostr << '\n';

		ostr << "	_operStack[" << i << "]: ";
		ostr << '"' << _operStack[i].toString() << '"' << '\n';
	}

	ostr << "}" << std::endl;
}

OW_String OW_WQLSelectStatement::toString() const
{
	OW_StringBuffer buf("select ");
	if (_selectPropertyNames.size())
	{
		for (size_t i = 0; i < _selectPropertyNames.size(); i++)
		{
			if (i != 0)
			{
				buf += ", ";
			}
			buf += _selectPropertyNames[i];
		}
	}
	else
	{
		// can this happen?
		buf += " *";
	}

	buf += " from ";
	buf += _className;

	// Print the operations/operands
	for (size_t i = 0; i < _operStack.size(); i++)
	{
		if (i == 0)
			buf += "\n";

		buf += " _operStack[" + OW_String(i) + "]: ";
		buf += "\"" + _operStack[i].toString() + "\"\n";
	}

	buf += ")";
	return buf.toString();
}

void OW_WQLSelectStatement::compileWhereClause(
		const OW_WQLPropertySource* /*source*/, OW_WQLCompile& wcl)
{
	wcl.compile(this);
}


