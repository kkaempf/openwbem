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
#ifndef OW_WQL_SELECT_STATEMENT_HPP_INCLUDE_GUARD_H_
#define OW_WQL_SELECT_STATEMENT_HPP_INCLUDE_GUARD_H_
#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_String.hpp"
#include "OW_WQLOperation.hpp"
#include "OW_WQLOperand.hpp"
#include "OW_WQLPropertySource.hpp"

namespace OpenWBEM
{

/** This class represents a compiled WQL select statement. 
	See the wql spec for details on the select statement.
	
	There are methods for obtaining the various elements of the select
	statement.
	
	The components of the where clause are stored in two arrays: one for
	operands and one for operators (these are placed in proper order by the
	YACC parser). Evaluation is performed using a Boolean stack. See the
	implementation of evaluateWhereClause() for details.
*/
class WQLCompile;
class WQLSelectStatement
{
public:
	WQLSelectStatement();
	~WQLSelectStatement();
	/** Clears all data members of this object.
	*/
	void clear();
	const String& getClassName() const
	{
		return _className;
	}
	/** Modifier. This method should not be called by the user (only by the
	  parser).
	 */
	void setClassName(const String& className)
	{
		_className = className;
	}
	/** Returns the number of property names which were indicated in the
	  selection list.
	 */
	UInt32 getSelectPropertyNameCount() const
	{
		return _selectPropertyNames.size();
	}
	/** Gets the i-th selected property name in the list.
	 */
	const String& getSelectPropertyName(UInt32 i) const
	{
		return _selectPropertyNames[i];
	}
	/** Gets the selected property names in the list.
	 */
	const StringArray& getSelectPropertyNames() const
	{
		return _selectPropertyNames;
	}
	/** Appends a property name to the property name list. This user should
	  not call this method; it should only be called by the parser.
	 */
	void appendSelectPropertyName(const String& x)
	{
		_selectPropertyNames.append(x);
	}
	/** Returns the number of unique property names from the where clause.
	 */
	UInt32 getWherePropertyNameCount() const
	{
		return _wherePropertyNames.size();
	}
	/** Gets the i-th unique property appearing in the where clause.
	 */
	const String& getWherePropertyName(UInt32 i) const
	{
		return _wherePropertyNames[i];
	}
	/** Appends a property name to the where property name list. This user 
	  should not call this method; it should only be called by the parser.
	  @param x name of the property.
	  @return false if a property with that name already exists.
	 */
	bool appendWherePropertyName(const String& x);
	/** Appends an operation to the operation array. This method should only
	  be called by the parser itself.
	 */
	void appendOperation(WQLOperation x)
	{
		_operStack.push_back(x);
	}
	/** Appends an operand to the operation array. This method should only
	  be called by the parser itself.
	 */
	void appendOperand(const WQLOperand& x)
	{
		_operStack.push_back(x);
	}
	/** Returns true if this class has a where clause.
	 */
	bool hasWhereClause() const
	{
		return !_operStack.empty();
	}
	/** Evalautes the where clause using the symbol table to resolve symbols.
	 * @return true or false if the source passes the query
	 * @throws NoSuchPropertyException if the where clause references a 
	 *		property that is unknown to source.
	 * @throws TypeMismatchException if the there is a type error in
	 *		the where clause or if the property type of the source property
	 *		doesn't match the query.
	 */
	bool evaluateWhereClause(const WQLPropertySource* source) const;
	/** Prints out the members of this class.
	*/
	void print(std::ostream& ostr) const;
	String toString() const;
	void compileWhereClause(const WQLPropertySource* source, WQLCompile& wcl);
private:
	//
	// The name of the target class. For example:
	//
	//	 SELECT * 
	//	 FROM TargetClass
	//	 WHERE ...
	//
	String _className;
	//
	// The list of property names being selected. For example, see "firstName",
	// and "lastName" below.
	//
	//	 SELECT firstName, lastName 
	//	 FROM TargetClass
	//	 WHERE ...
	//
	Array<String> _selectPropertyNames;
	//
	// The unique list of property names appearing in the WHERE clause.
	// Although a property may occur many times in the WHERE clause, it will
	// only appear once in this list.
	//
	Array<String> _wherePropertyNames;
	//
	// The list of operations encountered while parsing the WHERE clause.
	// Consider this query:
	//
	//	 SELECT *
	//	 FROM TargetClass
	//	 WHERE count > 10 OR peak < 20 AND state = "OKAY"
	//
	// This would generate the following stream of WQLOperations:
	//
	//	 WQL_GT
	//	 WQL_LT
	//	 WQL_EQ
	//	 WQL_AND
	//	 WQL_OR
	//
	//Array<WQLOperation> _operations;
	// 
	// The list of operands encountered while parsing the WHERE clause. They
	// query just above would generate the following stream of operands:
	//
	//	 count, 10, peak, 20, state, "OKAY"
	//
	//Array<WQLOperand> _operands;
	struct OperandOrOperation
	{
		enum Type
		{
			OPERATION,
			OPERAND
		};
		OperandOrOperation(WQLOperation o)
			: m_type(OPERATION)
			, m_operation(o)
		{}
		OperandOrOperation(const WQLOperand& o)
			: m_type(OPERAND)
			, m_operand(o)
		{}
		Type m_type;
		WQLOperation m_operation;
		WQLOperand m_operand;
		String toString() const
		{
			if (m_type == OPERATION)
			{
				return WQLOperationToString(m_operation);
			}
			else
			{
				return m_operand.toString();
			}
		}
	};
	Array<OperandOrOperation> _operStack;
	friend class WQLCompile;
};

} // end namespace OpenWBEM

typedef OpenWBEM::WQLSelectStatement OW_WQLSelectStatement OW_DEPRECATED; // in 3.0.0

#endif
