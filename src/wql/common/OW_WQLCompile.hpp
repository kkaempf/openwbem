//%///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef OW_WQLCOMPILE_HPP_INCLUDE_GUARD_H_
#define OW_WQLCOMPILE_HPP_INCLUDE_GUARD_H_

#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_Stack.hpp"
#include "OW_WQLOperation.hpp"
#include "OW_WQLOperand.hpp"
#include "OW_WQLSelectStatement.hpp"

#include "OW_Exception.hpp"

DEFINE_EXCEPTION(NoSuchProperty);


class OW_WQLCompile
{
public:

	class term_el
	{
	public:
		term_el()
		{}
		term_el(bool mark_, OW_WQLOperation op_, OW_WQLOperand opn1_, OW_WQLOperand opn2_)
			: mark(mark_)
			, op(op_)
			, opn1(opn1_)
			, opn2(opn2_)
		{}
		
		bool mark;
		OW_WQLOperation op;
		OW_WQLOperand opn1;
		OW_WQLOperand opn2;
	
		void negate(void);
	};
	
	class stack_el
	{
	public:
		stack_el()
		{}
		stack_el(int opn_, bool is_terminal_)
			: opn(opn_)
			, is_terminal(is_terminal_)
		{}
		int   opn;     // either to terminals or eval_heap
		bool is_terminal;
	};
	
	
	class eval_el
	{
	public:
		eval_el()
		{}
		eval_el(bool mark_, OW_WQLOperation op_, int opn1_, bool is_terminal1_, int opn2_, bool is_terminal2_)
			: mark(mark_)
			, op(op_)
			, opn1(opn1_)
			, is_terminal1(is_terminal1_)
			, opn2(opn2_)
			, is_terminal2(is_terminal2_)
		{}
	

		bool mark;
		OW_WQLOperation op;
		int opn1;
		bool is_terminal1; // if yes, look in terminal Array
		int opn2;
		bool is_terminal2; // if no, look in eval heap
	
		stack_el getFirst();
	
		stack_el getSecond();
	
		void setFirst(const stack_el s);
		
		void setSecond(const stack_el s);
		
		void assign_unary_to_first(const eval_el & assignee);
	
		void assign_unary_to_second(const eval_el & assignee);
	
		// Ordering operators, so that op1 > op2 for all non-terminals
		// and terminals appear in the second operand first
		void order(void);
	};
	
	typedef OW_Array<term_el> TableauRow;
	
	typedef OW_Array<TableauRow> Tableau;
	


    OW_WQLCompile();

    OW_WQLCompile(const OW_WQLSelectStatement& wqs);

    OW_WQLCompile(const OW_WQLSelectStatement * wqs);

    ~OW_WQLCompile();

    void compile (const OW_WQLSelectStatement * wqs);

    Tableau& getTableau() {return _tableau;}

    bool evaluate(OW_WQLPropertySource * source) const;

    void print(std::ostream& ostr);

    void printTableau(std::ostream& ostr);


protected:
    void _buildEvalHeap(const OW_WQLSelectStatement * wqs);

    void _pushNOTDown(void);

    void _factoring(void);

    void _gatherDisj(OW_Array<stack_el>& stk);

    void _gatherConj(OW_Array<stack_el>& stk, stack_el sel);

    void _gather(OW_Array<stack_el>& stk, stack_el sel, bool or_flag);

    void _sortTableau();

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
				OW_THROW(OW_NoSuchPropertyException, "No such property");
        }
    }


    // Structure to contain the compiled DNF form

    Tableau _tableau;

    //
    // The eval_heap structure contains an ordered tree of non-terminal
    // expressions, the term_heap structure the corresponding terminal
    // expressions
    //

    OW_Array<term_el> terminal_heap;

    OW_Array<eval_el> eval_heap;

};


#endif

