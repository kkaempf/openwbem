// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
// Copyright (c) 2002 Vintela, Inc..
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
//==============================================================================//
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Dan Nuffer
//
///////////////////////////////////////////////////////////////////////////////
#include "OW_config.h"
#include "OW_Assertion.hpp"
#include "OW_WQLOperation.hpp"
#include "OW_WQLOperand.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_Format.hpp"
#ifdef OW_HAVE_OSTREAM
#include <ostream>
#else
#include <iostream>
#endif
#include <algorithm> // for std::unique

namespace OpenWBEM
{

//
// Terminal element methods 
//
void WQLCompile::term_el::negate()
{
	switch (op)
	{
		case WQL_EQ: op = WQL_NE; break;
		case WQL_NE: op = WQL_EQ; break;
		case WQL_LT: op = WQL_GE; break;
		case WQL_LE: op = WQL_GT; break;
		case WQL_GT: op = WQL_LE; break;
		case WQL_GE: op = WQL_LT; break;
		default: break;
	}
};
bool operator==(const WQLCompile::term_el& x, const WQLCompile::term_el& y)
{
	return x.op == y.op && x.opn1 == y.opn1 && x.opn2 == y.opn2;
}
bool operator!=(const WQLCompile::term_el& x, const WQLCompile::term_el& y)
{
	return !(x == y);
}
//
// Evaluation heap element methods 
//
WQLCompile::stack_el
WQLCompile::eval_el::getFirst()
{
	return stack_el(opn1, is_terminal1);
}
WQLCompile::stack_el
WQLCompile::eval_el::getSecond()
{
	return stack_el(opn2, is_terminal2);
}
void WQLCompile::eval_el::setFirst(const WQLCompile::stack_el s)
{
	opn1 = s.opn;
	is_terminal1 = s.type;
}
void WQLCompile::eval_el::setSecond(const WQLCompile::stack_el s)
{
	opn2 = s.opn;
	is_terminal2 = s.type;
}
void WQLCompile::eval_el::assign_unary_to_first(const WQLCompile::eval_el & assignee)
{
	opn1 = assignee.opn1;
	is_terminal1 = assignee.is_terminal1;
}
void WQLCompile::eval_el::assign_unary_to_second(const WQLCompile::eval_el & assignee)
{
	opn2 = assignee.opn1;
	is_terminal2 = assignee.is_terminal1;
}
// Ordering operators, so that op1 > op2 for all non-terminals
// and terminals appear in the second operand first
void WQLCompile::eval_el::order()
{
	if ((is_terminal1 == EVAL_HEAP) && (is_terminal2 == EVAL_HEAP))
	{
		if (opn2 > opn1)
		{
			std::swap(opn1, opn2);
		}
	}
	else if ((is_terminal1 != EVAL_HEAP) && (is_terminal2 == EVAL_HEAP))
	{
		if (opn2 > opn1)
		{
			std::swap(opn1, opn2);
			std::swap(is_terminal1, is_terminal2);
		}
	}
}
//
// Helper function copied from WQLSelectStatement
// 
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
//
// WQL Compiler methods
//
WQLCompile::WQLCompile()
{
}
WQLCompile::WQLCompile(const WQLSelectStatement & wqs)
{
	compile(&wqs);
}
WQLCompile::~WQLCompile()
{
}
void WQLCompile::compile(const WQLSelectStatement * wqs)
{
	if (!wqs->hasWhereClause())	
	{
		return;
	}
	_tableau.clear();
	_buildEvalHeap(wqs);
	_pushNOTDown();
	_factoring();
	Array<stack_el> disj;
	_gatherDisj(disj);
	if (disj.size() == 0)
	{
		if (terminal_heap.size() > 0)
		{
			// point to the remaining terminal element
			disj.append(stack_el(0, TERMINAL_HEAP));
		}
	}

	for (UInt32 i=0, n =disj.size(); i< n; i++)
	{
		TableauRow tr;
		Array<stack_el> conj;
		if (disj[i].type == EVAL_HEAP)
		{
			_gatherConj(conj, disj[i]);
			for ( UInt32 j=0, m = conj.size(); j < m; j++)
			{
				tr.append(terminal_heap[conj[j].opn]);
			}
		}
		else
		{
			tr.append(terminal_heap[disj[i].opn]);
		}
		_tableau.append(tr);
	}
	eval_heap.clear();
	//printTableau();
	_sortTableau();
}
bool WQLCompile::evaluate(const WQLPropertySource& source) const
{
	bool b = false;
	WQLOperand lhs, rhs;
	UInt32 tableauSize = _tableau.size();
	if (tableauSize == 0)
	{
		return true;
	}
	for (UInt32 i = 0; i < tableauSize; i++)
	{
		TableauRow tr = _tableau[i];
		for (UInt32 j=0,m = tr.size(); j < m; j++)
		{
			if (tr[j].op == WQL_ISA)
			{
				String propertyName = tr[j].opn1.getPropertyName();
				String className;
				if (tr[j].opn2.getType() == WQLOperand::PROPERTY_NAME)
				{
					className = tr[j].opn2.getPropertyName();
				}
				else
				{
					className = tr[j].opn2.getStringValue(); // this will throw if it's not a string
				}
				b = source.evaluateISA(propertyName, className);
			}
			else
			{
				lhs = tr[j].opn1;
				WQLCompile::_ResolveProperty(lhs,source);
				rhs = tr[j].opn2;
				WQLCompile::_ResolveProperty(rhs,source);
				if (rhs.getType() != lhs.getType())
				{
					OW_THROW(TypeMismatchException, Format("Type mismatch: lhs: %1 rhs: %2", lhs.toString(), rhs.toString()).c_str());
				}
				if (!_Evaluate(lhs, rhs, tr[j].op))
				{
					b = false;
					break;
				}
				else
				{
					b = true;
				}
			}
		}

		if (b) 
		{
			return true;
		}
	}
	return false;
}
void WQLCompile::print(std::ostream& ostr)
{
	for (UInt32 i=0, n=eval_heap.size();i < n;i++)
	{
		WQLOperation wop = eval_heap[i].op; 
		if (wop == WQL_DO_NOTHING)	
		{
			continue;
		}
		ostr << "Eval element " << i << ": "; 
		if (eval_heap[i].is_terminal1 == TERMINAL_HEAP) 
		{
			ostr << "T(";
		}
		else if (eval_heap[i].is_terminal1 == EVAL_HEAP)
		{
			ostr << "E(";
		}
		else
		{
			ostr << "O(";
		}
		ostr << eval_heap[i].opn1 << ") "; 
		ostr << WQLOperationToString(eval_heap[i].op); 
		if (eval_heap[i].is_terminal2 == TERMINAL_HEAP) 
		{
			ostr << " T(";
		}
		else if (eval_heap[i].is_terminal2 == EVAL_HEAP)
		{
			ostr << "E(";
		}
		else
		{
			ostr << "O(";
		}
		ostr << eval_heap[i].opn2 << ")" << std::endl; 
	} 
	for (UInt32 i=0, n=terminal_heap.size();i < n;i++)
	{
		ostr << "Terminal expression " << i << ": "; 
		ostr << terminal_heap[i].opn1.toString() << " "; 
		ostr << WQLOperationToString(terminal_heap[i].op) << " " 
			<< terminal_heap[i].opn2.toString() << std::endl; 
	}
}
void WQLCompile::printTableau(std::ostream& ostr)
{
	for (UInt32 i=0, n = _tableau.size(); i < n; i++)
	{
		ostr << "Tableau " << i << std::endl;
		TableauRow tr = _tableau[i];
		for (UInt32 j=0, m = tr.size(); j < m; j++)
		{
			ostr << tr[j].opn1.toString() << " "; 
			ostr << WQLOperationToString(tr[j].op) << " " 
				<< tr[j].opn2.toString() << std::endl; 
		}
	}
}
void WQLCompile::_buildEvalHeap(const WQLSelectStatement * wqs)
{
	Stack<stack_el> stack;
	for (UInt32 i = 0, n = wqs->_operStack.size(); i < n; i++)
	{
		const WQLSelectStatement::OperandOrOperation& curItem = wqs->_operStack[i];
		if (curItem.m_type == WQLSelectStatement::OperandOrOperation::OPERAND)
		{
			// put pointer to it onto the stack
			stack.push(stack_el(i, OPERAND));
		}
		else
		{
			WQLOperation op = curItem.m_operation;
	
			switch (op)
			{
				// unary
				case WQL_NOT:
					{
						OW_ASSERT(stack.size() >= 1);
	
						stack_el op1 = stack.top();
	
						// generate Eval expression
						eval_heap.append(eval_el(false, op, op1.opn, op1.type,
							-1, TERMINAL_HEAP));
	
						stack.top() = stack_el(eval_heap.size()-1, EVAL_HEAP);
	
						break;
					}
	
				// binary
				case WQL_OR:
				case WQL_AND:
				case WQL_EQ:
				case WQL_NE:
				case WQL_LT:
				case WQL_LE:
				case WQL_GT:
				case WQL_GE:
				case WQL_ISA:
					{
						OW_ASSERT(stack.size() >= 2);
	
						stack_el op2 = stack.top();
						stack.pop();
	
						stack_el op1 = stack.top();
						if (op1.type == OPERAND && op2.type == OPERAND)
						{
							OW_ASSERT(op1.type == OPERAND);
							OW_ASSERT(wqs->_operStack[op1.opn].m_type == WQLSelectStatement::OperandOrOperation::OPERAND);
							WQLOperand lhs = wqs->_operStack[op1.opn].m_operand;
							OW_ASSERT(op2.type == OPERAND);
							OW_ASSERT(wqs->_operStack[op2.opn].m_type == WQLSelectStatement::OperandOrOperation::OPERAND);
							WQLOperand rhs = wqs->_operStack[op2.opn].m_operand;
							terminal_heap.push_back(term_el(false, op, lhs, rhs));
							stack.top() = stack_el(terminal_heap.size()-1, TERMINAL_HEAP);
						}
						else
						{
							// generate Eval expression
							eval_heap.append(eval_el(false, op, op1.opn, op1.type,
								op2.opn , op2.type));
							stack.top() = stack_el(eval_heap.size()-1, EVAL_HEAP);
						}
	
						break;
					}
				case WQL_DO_NOTHING:
					{
						OW_ASSERT(0); // this should never happen
						break;
					}
			}
		}
	}
	OW_ASSERT(stack.size() == 1);
}
void WQLCompile::_pushNOTDown()
{
	for (Int32 i=eval_heap.size()-1; i >= 0; i--)
	{
		bool _found = false;
		// Order all operators, so that op1 > op2 for non-terminals
		// and terminals appear as second operand
		eval_heap[i].order();
		// First solve the unary NOT operator
		if (eval_heap[i].op == WQL_NOT)
		{
			// This serves as the equivalent of an empty operator
			eval_heap[i].op = WQL_DO_NOTHING;
			// Substitute this expression in all higher order eval statements
			// so that this node becomes disconnected from the tree
			for (Int32 j=eval_heap.size()-1; j > i;j--)
			{
				// Test first operand
				if ((eval_heap[j].is_terminal1 == EVAL_HEAP) && (eval_heap[j].opn1 == i))
				{
					eval_heap[j].assign_unary_to_first(eval_heap[i]);
				}
				// Test second operand
				if ((eval_heap[j].is_terminal2 == EVAL_HEAP) && (eval_heap[j].opn2 == i))
				{
					eval_heap[j].assign_unary_to_second(eval_heap[i]);
				}
			}
			// Test: Double NOT created by moving down
			if (eval_heap[i].mark)
			{
				eval_heap[i].mark = false;
			}
			else
			{
				_found = true;
			}
			// else indicate a pending NOT to be pushed down further
		}
		// Simple NOT created by moving down
		if (eval_heap[i].mark)
		{
			// Remove the mark, indicate a pending NOT to be pushed down
			// further and switch operators (AND / OR)
			eval_heap[i].mark=false;
			if (eval_heap[i].op == WQL_OR) 
			{
				eval_heap[i].op = WQL_AND;
			}
			else if (eval_heap[i].op == WQL_AND) 
			{
				eval_heap[i].op = WQL_OR;
			}
			// NOT operator is already ruled out
			_found = true;
		}
		// Push a pending NOT further down
		if (_found)
		{
			// First operand
			int j = eval_heap[i].opn1;
			if (eval_heap[i].is_terminal1 == TERMINAL_HEAP)
			{
				// Flip NOT mark
				terminal_heap[j].negate();
			}
			else if (eval_heap[i].is_terminal1 == EVAL_HEAP)
			{
				eval_heap[j].mark = !(eval_heap[j].mark);
			}
			//Second operand (if it exists)
			if ((j = eval_heap[i].opn2) >= 0)
			{
				if (eval_heap[i].is_terminal2 == TERMINAL_HEAP)
				{
					// Flip NOT mark
					terminal_heap[j].negate();
				}
				else if (eval_heap[i].is_terminal2 == EVAL_HEAP)
				{
					eval_heap[j].mark = !(eval_heap[j].mark);
				}
			}
		}
	}
}
void WQLCompile::_factoring()
{
	int i = 0,n = eval_heap.size();
	//for (int i=eval_heap.size()-1; i >= 0; i--)
	while (i < n)
	{
		int _found = 0;
		int index = 0;
		// look for expressions (A | B) & C  ---> A & C | A & B
		if (eval_heap[i].op == WQL_AND)
		{
			if (eval_heap[i].is_terminal1 == EVAL_HEAP)
			{
				index = eval_heap[i].opn1; // remember the index
				if (eval_heap[index].op == WQL_OR) 
				{
					_found = 1;
				}
			}
			if ((_found == 0) && (eval_heap[i].is_terminal2 == EVAL_HEAP))
			{
				index = eval_heap[i].opn2; // remember the index
				if (eval_heap[index].op == WQL_OR) 
				{
					_found = 2;
				}
			}
			if (_found != 0)
			{
				stack_el s;
				if (_found == 1)
				{
					s = eval_heap[i].getSecond();
				}
				else
				{
					s = eval_heap[i].getFirst();
				}
				// insert two new expression before entry i
				eval_el evl(false, WQL_OR, i+1, EVAL_HEAP, i, EVAL_HEAP);
				if (i < static_cast<int>(eval_heap.size())-1)
				{
					eval_heap.insert(i+1, evl);
				}
				else
				{
					eval_heap.append(evl);
				}
				eval_heap.insert(i+1, evl);
				for (int j=eval_heap.size()-1; j > i + 2; j--)
				{
					//eval_heap[j] = eval_heap[j-2];
					// adjust pointers
					if ((eval_heap[j].is_terminal1 == EVAL_HEAP) &&
						(eval_heap[j].opn1 >= i))
					{
						eval_heap[j].opn1 += 2;
					}
					if ((eval_heap[j].is_terminal2 == EVAL_HEAP) &&
						(eval_heap[j].opn2 >= i))
					{
						eval_heap[j].opn2 += 2;
					}
				}
				n+=2; // increase size of array
				// generate the new expressions : new OR expression
				// first new AND expression
				eval_heap[i+1].mark = false;
				eval_heap[i+1].op = WQL_AND;
				eval_heap[i+1].setFirst(s);
				eval_heap[i+1].setSecond( eval_heap[index].getFirst());
				eval_heap[i+1].order();
				// second new AND expression
				eval_heap[i].mark = false;
				eval_heap[i].op = WQL_AND;
				eval_heap[i].setFirst(s);
				eval_heap[i].setSecond( eval_heap[index].getSecond());
				eval_heap[i].order();
				// mark the indexed expression as inactive
				//eval_heap[index].op = WQL_IS_TRUE; possible disconnects
				i--;
			} /* endif _found > 0 */
		} /* endif found AND operator */
		i++; // increase pointer
	}
}
void WQLCompile::_gatherDisj(Array<stack_el>& stk)
{
	_gather(stk, stack_el(0, TERMINAL_HEAP), true);
}
void WQLCompile::_gatherConj(Array<stack_el>& stk, stack_el sel)
{
	_gather(stk, sel, false);
}
void WQLCompile::_gather(Array<stack_el>& stk, stack_el sel, bool or_flag)
{
	UInt32 i = 0;
	stk.empty();
	if ((i = eval_heap.size()) == 0) 
	{
		return;
	}
	while (eval_heap[i-1].op == WQL_DO_NOTHING)
	{
		eval_heap.remove(i-1);
		i--;
		if (i == 0)	
		{
			return;
		}
	}
	if (or_flag)
	{
		stk.append(stack_el(i-1, EVAL_HEAP));
	}
	else
	{
		if (sel.type != EVAL_HEAP) 
		{
			return;
		}
		stk.append(sel);
	}
	i = 0;
	while (i<stk.size())
	{
		int k = stk[i].opn;
		if ((k < 0) || (stk[i].type != EVAL_HEAP))
		{
			i++;
		}
		else
		{
			if ( ((eval_heap[k].op != WQL_OR) && (or_flag)) ||
				((eval_heap[k].op != WQL_AND) && (!or_flag))  )
			{
				i++;
			}
			else
			{
				// replace the element with disjunction
				stk[i] = eval_heap[k].getSecond();
				stk.insert(i, eval_heap[k].getFirst());
				if (or_flag)
				{
					eval_heap[k].op = WQL_DO_NOTHING;
				}
			}
		}
	}
}
void WQLCompile::_sortTableau()
{
	// sorting key is lexical scope of properties
	// Bubblesort should be sufficient for tableauRows
	for (UInt32 i = 0, n = _tableau.size(); i < n; i++)
	{
		TableauRow tr = _tableau[i];
		WQLOperand wop;
		WQLOperation wopt;
		// try to order row, so that properties are (preferrably) first args
		// comparisons of two properties are moved to the end
		for (UInt32 j = 0, m = tr.size(); j < m; j++)
		{
			if (tr[j].opn2.getType() == WQLOperand::PROPERTY_NAME)
			{
				if (tr[j].opn1.getType() != WQLOperand::PROPERTY_NAME)
				{
					wop = tr[j].opn1;
					tr[j].opn1 = tr[j].opn2;
					tr[j].opn2 = wop;
					// turn operator
					switch (tr[j].op)
					{
						case WQL_LT: tr[j].op = WQL_GT; break;
						case WQL_GT: tr[j].op = WQL_LT; break;
						case WQL_LE: tr[j].op = WQL_GE; break;
						case WQL_GE: tr[j].op = WQL_LE; break;
						default: break;
					}
				}
			}
		}
		String key1, key2;
		// switch this to use std::sort ?
		for (UInt32 j = 0, m = tr.size(); j < m; j++)
		{
			if ((tr[j].opn1.getType() == WQLOperand::PROPERTY_NAME)
				&& (tr[j].opn2.getType() != WQLOperand::PROPERTY_NAME))
			{
				key1 = tr[j].opn1.getPropertyName();
			}
			else
			{
				key1.erase();
			}
			for (UInt32 k = j; k < m; k++)
			{
				if ((tr[k].opn1.getType() == WQLOperand::PROPERTY_NAME)
					&& (tr[k].opn2.getType() != WQLOperand::PROPERTY_NAME))
				{
					key2 = tr[k].opn1.getPropertyName();
				}
				else
				{
					key2.erase();
				}
				if (key1 > key2)
				{
					wop = tr[j].opn1;
					tr[j].opn1 = tr[k].opn1;
					tr[k].opn1 = wop;
					wop = tr[j].opn2;
					tr[j].opn2 = tr[k].opn2;
					tr[k].opn2 = wop;
					wopt = tr[j].op;
					tr[j].op = tr[k].op;
					tr[k].op = wopt;
				}
			}
		}
		// remove duplicates
		tr.erase(std::unique(tr.begin(), tr.end()), tr.end());
		// save it
		_tableau[i] = tr;
	}
}

} // end namespace OpenWBEM

