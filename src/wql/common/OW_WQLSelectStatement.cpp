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
#include "OW_StringBuffer.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_Stack.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_Assertion.hpp"

#if defined(OW_HAVE_OSTREAM)
#include <ostream>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

// TODO: Merge this code with WQLCompile.cpp, it's all duplicated.
template<class T>
inline static bool _Compare(const T& x, const T& y, WQLOperation op)
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
		const WQLOperand& lhs, 
		const WQLOperand& rhs, 
		WQLOperation op)
{
	switch (lhs.getType())
	{
		case WQLOperand::NULL_VALUE:
			{
				// return true if the op is WQL_EQ and the rhs is NULL
				// also if op is WQL_NE and rhs is not NULL
				return !(op == WQL_EQ) ^ (rhs.getType() == WQLOperand::NULL_VALUE);
				break;
			}
		case WQLOperand::INTEGER_VALUE:
			{
				return _Compare(
						lhs.getIntegerValue(),
						rhs.getIntegerValue(),
						op);
			}
		case WQLOperand::DOUBLE_VALUE:
			{
				return _Compare(
						lhs.getDoubleValue(),
						rhs.getDoubleValue(),
						op);
			}
		case WQLOperand::BOOLEAN_VALUE:
			{
				return _Compare(
						lhs.getBooleanValue(),
						rhs.getBooleanValue(),
						op);
			}
		case WQLOperand::STRING_VALUE:
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
WQLSelectStatement::WQLSelectStatement()
{
	_operStack.reserve(32);
}
WQLSelectStatement::~WQLSelectStatement()
{
}
void WQLSelectStatement::clear()
{
	_className.erase();
	_selectPropertyNames.clear();
	_operStack.clear();
}
bool WQLSelectStatement::appendWherePropertyName(const String& x)
{
	//
	// Reject duplicate property names by returning false.
	//
	for (size_t i = 0, n = _wherePropertyNames.size(); i < n; i++)
	{
		if (_wherePropertyNames[i] == x)
		{
			return false;
		}
	}
	//
	// Append the new property.
	//
	_wherePropertyNames.append(x);
	return true;
}
static inline void _ResolveProperty(
		WQLOperand& op,
		const WQLPropertySource* source)
{
	//
	// Resolve the operand: if it's a property name, look up its value:
	//
	if (op.getType() == WQLOperand::PROPERTY_NAME)
	{
		const String& propertyName = op.getPropertyName();
		// it's up to the source to handle embedded properties.
		if (!source->getValue(propertyName, op))
		{
			OW_THROW(NoSuchPropertyException, propertyName.c_str());
		}
	}
}
bool WQLSelectStatement::evaluateWhereClause(
		const WQLPropertySource* source) const
{
	if (!hasWhereClause())
	{
		return true;
	}
	Stack<WQLOperand> stack;
	//
	// Process each of the operations:
	//
	for (UInt32 i = 0, n = _operStack.size(); i < n; i++)
	{
		const WQLSelectStatement::OperandOrOperation& curItem = _operStack[i];
		if (curItem.m_type == WQLSelectStatement::OperandOrOperation::OPERAND)
		{
			// put it onto the stack
			stack.push(curItem.m_operand);
		}
		else
		{
			WQLOperation op = curItem.m_operation;
			switch (op)
			{
				case WQL_OR:
					{
						OW_ASSERT(stack.size() >= 2);
						WQLOperand op1 = stack.top();
						stack.pop();
						WQLOperand& op2 = stack.top();
						bool b1 = op1.getBooleanValue();
						bool b2 = op2.getBooleanValue();
						stack.top() = WQLOperand(b1 || b2, WQL_BOOLEAN_VALUE_TAG);
						break;
					}
				case WQL_AND:
					{
						OW_ASSERT(stack.size() >= 2);
						WQLOperand op1 = stack.top();
						stack.pop();
						WQLOperand& op2 = stack.top();
						bool b1 = op1.getBooleanValue();
						bool b2 = op2.getBooleanValue();
						stack.top() = WQLOperand(b1 && b2, WQL_BOOLEAN_VALUE_TAG);
						break;
					}
				case WQL_NOT:
					{
						OW_ASSERT(stack.size() >= 1);
						WQLOperand& op = stack.top();
						bool b1 = op.getBooleanValue();
						stack.top() = WQLOperand(!b1, WQL_BOOLEAN_VALUE_TAG);
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
						WQLOperand lhs = stack.top();
						stack.pop();
						_ResolveProperty(lhs, source);
						//
						// Resolve the right-hand-side to a value (if not already
						// a value).
						//
						WQLOperand& rhs = stack.top();
						_ResolveProperty(rhs, source);
						//
						// Check for a type mismatch:
						//
						if (rhs.getType() != lhs.getType())
						{
							OW_THROW(TypeMismatchException, "");
						}
						//
						// Now that the types are known to be alike, apply the
						// operation:
						//
						stack.top() = WQLOperand(_Evaluate(lhs, rhs, op), WQL_BOOLEAN_VALUE_TAG);
						break;
					}
				case WQL_ISA:
					{
						OW_ASSERT(stack.size() >= 2);
						WQLOperand lhs = stack.top();
						stack.pop();
						if (lhs.getType() != WQLOperand::PROPERTY_NAME)
						{
							OW_THROW(TypeMismatchException, "First argument of ISA must be a property name");
						}
						WQLOperand& rhs = stack.top();
						String className;
						if (rhs.getType() == WQLOperand::PROPERTY_NAME)
						{
							className = rhs.getPropertyName();
						}
						else if (rhs.getType() == WQLOperand::STRING_VALUE)
						{
							className = rhs.getStringValue();
						}
						else
						{
							OW_THROW(TypeMismatchException, "Second argument of ISA must be a property name or string constant");
						}
						stack.top() = WQLOperand(source->evaluateISA(lhs.getPropertyName(), className), WQL_BOOLEAN_VALUE_TAG);
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
void WQLSelectStatement::print(std::ostream& ostr) const
{
	ostr << "WQLSelectStatement\n";
	ostr << "{\n";
	ostr << "	_className: \"" << _className << "\"\n";
	for (size_t i = 0; i < _selectPropertyNames.size(); i++)
	{
		if (i == 0)
		{
			ostr << '\n';
		}
		ostr << "	_selectPropertyNames[" << i << "]: ";
		ostr << '"' << _selectPropertyNames[i] << '"' << '\n';
	}
	// Print the operations/operands
	for (size_t i = 0; i < _operStack.size(); i++)
	{
		if (i == 0)
		{
			ostr << '\n';
		}
		ostr << "	_operStack[" << i << "]: ";
		ostr << '"' << _operStack[i].toString() << '"' << '\n';
	}
	ostr << "}" << std::endl;
}
String WQLSelectStatement::toString() const
{
	StringBuffer buf("select ");
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
		{
			buf += "\n";
		}
		buf += " _operStack[" + String(UInt32(i)) + "]: ";
		buf += "\"" + _operStack[i].toString() + "\"\n";
	}
	buf += ")";
	return buf.toString();
}
void WQLSelectStatement::compileWhereClause(
		const WQLPropertySource* /*source*/, WQLCompile& wcl)
{
	wcl.compile(this);
}

} // end namespace OpenWBEM

