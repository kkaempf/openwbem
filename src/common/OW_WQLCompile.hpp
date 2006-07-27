//%///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//==============================================================================//
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Dan Nuffer
//
//%/////////////////////////////////////////////////////////////////////////////
#ifndef OW_WQLCOMPILE_HPP_INCLUDE_GUARD_H_
#define OW_WQLCOMPILE_HPP_INCLUDE_GUARD_H_
#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_WQLOperation.hpp"
#include "OW_WQLOperand.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_NoSuchPropertyException.hpp"

namespace OW_NAMESPACE
{

class OW_COMMON_API WQLCompile
{
public:
	struct term_el
	{
		term_el()
		{}
		term_el(bool mark_, WQLOperation op_, const WQLOperand& opn1_, const WQLOperand& opn2_)
			: mark(mark_)
			, op(op_)
			, opn1(opn1_)
			, opn2(opn2_)
		{}
		
		bool mark;
		WQLOperation op;
		WQLOperand opn1;
		WQLOperand opn2;
	
		void negate();
	};
	
	enum el_type
	{
		EVAL_HEAP,
		TERMINAL_HEAP,
		OPERAND
	};
	struct stack_el
	{
		stack_el()
		{}
		stack_el(int opn_, el_type type_)
			: opn(opn_)
			, type(type_)
		{}
		int  opn;     // either to terminals or eval_heap
		el_type type;
	};
	
	
	struct eval_el
	{
		eval_el()
		{}
		eval_el(bool mark_, WQLOperation op_, int opn1_, el_type is_terminal1_, int opn2_, el_type is_terminal2_)
			: mark(mark_)
			, op(op_)
			, opn1(opn1_)
			, is_terminal1(is_terminal1_)
			, opn2(opn2_)
			, is_terminal2(is_terminal2_)
		{}
	
		bool mark;
		WQLOperation op;
		int opn1;
		el_type is_terminal1; // if yes, look in terminal Array
		int opn2;
		el_type is_terminal2; // if no, look in eval heap
	
		stack_el getFirst();
	
		stack_el getSecond();
	
		void setFirst(const stack_el s);
		
		void setSecond(const stack_el s);
		
		void assign_unary_to_first(const eval_el & assignee);
	
		void assign_unary_to_second(const eval_el & assignee);
	
		// Ordering operators, so that op1 > op2 for all non-terminals
		// and terminals appear in the second operand first
		void order();
	};
	
	typedef Array<term_el> TableauRow;
	
	typedef Array<TableauRow> Tableau;
	
	WQLCompile();
	// calls compile()
	WQLCompile(const WQLSelectStatement& wqs);
	~WQLCompile();
	void compile (const WQLSelectStatement * wqs);
	const Tableau& getTableau() const {return _tableau;}
	/** Evalautes the where clause using the symbol table to resolve symbols.
	 * @return true or false if the source passes the query
	 * @throws NoSuchPropertyException if the where clause references a 
	 *		property that is unknown to source.
	 * @throws TypeMismatchException if the there is a type error in
	 *		the where clause or if the property type of the source property
	 *		doesn't match the query.
	 */
	bool evaluate(const WQLPropertySource& source) const;
	void print(std::ostream& ostr);
	void printTableau(std::ostream& ostr);
private:
	void _buildEvalHeap(const WQLSelectStatement * wqs);
	void _pushNOTDown();
	void _factoring();
	void _gatherDisj(Array<stack_el>& stk);
	void _gatherConj(Array<stack_el>& stk, stack_el sel);
	void _gather(Array<stack_el>& stk, stack_el sel, bool or_flag);
	void _sortTableau();
	static inline void _ResolveProperty(
		WQLOperand& op,
		const WQLPropertySource& source)
	{
		//
		// Resolve the operand: if it's a property name, look up its value:
		//
		if (op.getType() == WQLOperand::PROPERTY_NAME)
		{
			const String& propertyName = op.getPropertyName();
			if (!source.getValue(propertyName, op))
				OW_THROW(NoSuchPropertyException, "No such property");
		}
	}

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	// Structure to contain the compiled DNF form
	Tableau _tableau;
	//
	// The eval_heap structure contains an ordered tree of non-terminal
	// expressions, the term_heap structure the corresponding terminal
	// expressions
	//
	Array<term_el> terminal_heap;
	Array<eval_el> eval_heap;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

};
OW_COMMON_API bool operator==(const WQLCompile::term_el& x, const WQLCompile::term_el& y);
OW_COMMON_API bool operator!=(const WQLCompile::term_el& x, const WQLCompile::term_el& y);

} // end namespace OW_NAMESPACE

#endif
