/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_WQLSelectStatementGen.hpp"
#include "OW_WQLAst.hpp"
#include "OW_WQLScanUtils.hpp"
#include "OW_CIMException.hpp"

namespace OpenWBEM
{

WQLSelectStatementGen::~WQLSelectStatementGen()
{
}

WQLSelectStatementGen::WQLSelectStatementGen()
	: m_isPropertyList(false)
{
}
void WQLSelectStatementGen::visit_stmt_selectStmt_optSemicolon(
	const stmt_selectStmt_optSemicolon* pstmt_selectStmt_optSemicolon
	)
{
	pstmt_selectStmt_optSemicolon->m_pselectStmt1->accept_interface(this);
	if (pstmt_selectStmt_optSemicolon->m_poptSemicolon2)
	{
		pstmt_selectStmt_optSemicolon->m_poptSemicolon2->accept_interface(this);
	}
}
void WQLSelectStatementGen::visit_stmt_updateStmt_optSemicolon(
	const stmt_updateStmt_optSemicolon*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_stmt_insertStmt_optSemicolon(
	const stmt_insertStmt_optSemicolon*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_stmt_deleteStmt_optSemicolon(
	const stmt_deleteStmt_optSemicolon*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optSemicolon_empty(
	const optSemicolon_empty* poptSemicolon_empty
	)
{
}
void WQLSelectStatementGen::visit_optSemicolon_SEMICOLON(
	const optSemicolon_SEMICOLON* poptSemicolon_SEMICOLON
	)
{
}
void WQLSelectStatementGen::visit_insertStmt(
	const insertStmt*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
	const insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_insertRest_DEFAULT_VALUES(
	const insertRest_DEFAULT_VALUES*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
	const insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_deleteStmt(
	const deleteStmt*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_updateStmt(
	const updateStmt*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
/*********************************************************
 * SELECT statement
 *  SELECT [distinct] targetEl* [FromClause] [Where Clause]
 *   [Group Clause] [Having Clause] [Sort Clause]
 *********************************************************/
void WQLSelectStatementGen::visit_selectStmt(
	const selectStmt* pselectStmt
	)
{
	// visit the unused ones, to detect an invalid query
	if (pselectStmt->m_poptDistinct2)
	{
		pselectStmt->m_poptDistinct2->accept_interface(this);
	}
	if (pselectStmt->m_poptGroupClause6)
	{
		pselectStmt->m_poptGroupClause6->accept_interface(this);
	}
	if (pselectStmt->m_poptHavingClause7)
	{
		pselectStmt->m_poptHavingClause7->accept_interface(this);
	}
	if (pselectStmt->m_poptSortClause8)
	{
		pselectStmt->m_poptSortClause8->accept_interface(this);
	}
	// start fresh
	m_stmt.clear();
	// FROM clause
	if (pselectStmt->m_poptFromClause4)
	{
		pselectStmt->m_poptFromClause4->accept_interface(this);
	}
	// WHERE
	if (pselectStmt->m_poptWhereClause5)
	{
		pselectStmt->m_poptWhereClause5->accept_interface(this);
	}
	// desired properties
	for (List<targetEl*>::const_iterator i = pselectStmt->m_ptargetList3->begin();
		i != pselectStmt->m_ptargetList3->end();
		++i )
	{
		(*i)->accept_interface(this);
	}
}
void WQLSelectStatementGen::visit_exprSeq_aExpr(
	const exprSeq_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_exprSeq_exprSeq_COMMA_aExpr(
	const exprSeq_exprSeq_COMMA_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_exprSeq_exprSeq_USING_aExpr(
	const exprSeq_exprSeq_USING_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optDistinct_empty(
	const optDistinct_empty* poptDistinct_empty
	)
{
}
void WQLSelectStatementGen::visit_optDistinct_DISTINCT(
	const optDistinct_DISTINCT*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN(
	const optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optDistinct_ALL(
	const optDistinct_ALL*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_sortClause(
	const sortClause*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optSortClause_empty(
	const optSortClause_empty* poptSortClause_empty
	)
{
}
void WQLSelectStatementGen::visit_optSortClause_sortClause(
	const optSortClause_sortClause*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_sortby(
	const sortby*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optGroupClause_empty(
	const optGroupClause_empty* poptGroupClause_empty
	)
{
}
void WQLSelectStatementGen::visit_optGroupClause_GROUP_BY_exprSeq(
	const optGroupClause_GROUP_BY_exprSeq*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optHavingClause_empty(
	const optHavingClause_empty*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optHavingClause_HAVING_aExpr(
	const optHavingClause_HAVING_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optFromClause_empty(
	const optFromClause_empty*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optFromClause_FROM_fromList(
	const optFromClause_FROM_fromList* poptFromClause_FROM_fromList
	)
{
	int count = 0;
	for (List<tableRef*>::const_iterator i = poptFromClause_FROM_fromList->m_pfromList2->begin();
		i != poptFromClause_FROM_fromList->m_pfromList2->end();
		++i, ++count )
	{
		(*i)->accept_interface(this);
		// only handle one class name in the FROM clause for now.
		if (count != 0)
		{
			OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only one class name allowed in FROM clause");
		}
	}
}
void WQLSelectStatementGen::visit_tableRef_relationExpr(
	const tableRef_relationExpr* ptableRef_relationExpr
	)
{
	ptableRef_relationExpr->m_prelationExpr1->accept_interface(this);
}
void WQLSelectStatementGen::visit_tableRef_relationExpr_aliasClause(
	const tableRef_relationExpr_aliasClause*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_tableRef_joinedTable(
	const tableRef_joinedTable*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause(
	const tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN(
	const joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinedTable_tableRef_CROSS_JOIN_tableRef(
	const joinedTable_tableRef_CROSS_JOIN_tableRef*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinedTable_tableRef_UNIONJOIN_tableRef(
	const joinedTable_tableRef_UNIONJOIN_tableRef*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinedTable_tableRef_joinType_JOIN_tableRef_joinQual(
	const joinedTable_tableRef_joinType_JOIN_tableRef_joinQual*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinedTable_tableRef_JOIN_tableRef_joinQual(
	const joinedTable_tableRef_JOIN_tableRef_joinQual*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef(
	const joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinedTable_tableRef_NATURAL_JOIN_tableRef(
	const joinedTable_tableRef_NATURAL_JOIN_tableRef*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN(
	const aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aliasClause_AS_strColId(
	const aliasClause_AS_strColId*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN(
	const aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aliasClause_strColId(
	const aliasClause_strColId*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinType_FULL_strOptJoinOuter(
	const joinType_FULL_strOptJoinOuter*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinType_LEFT_strOptJoinOuter(
	const joinType_LEFT_strOptJoinOuter*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinType_RIGHT_strOptJoinOuter(
	const joinType_RIGHT_strOptJoinOuter*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinType_INNERP(
	const joinType_INNERP*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN(
	const joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_joinQual_ON_aExpr(
	const joinQual_ON_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_relationExpr_strRelationName(
	const relationExpr_strRelationName* prelationExpr_strRelationName
	)
{
	m_stmt.setClassName(*prelationExpr_strRelationName->m_pstrRelationName1);
}
void WQLSelectStatementGen::visit_relationExpr_strRelationName_ASTERISK(
	const relationExpr_strRelationName_ASTERISK*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_relationExpr_ONLY_strRelationName(
	const relationExpr_ONLY_strRelationName*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optWhereClause_empty(
	const optWhereClause_empty* poptWhereClause_empty
	)
{
}
void WQLSelectStatementGen::visit_optWhereClause_WHERE_aExpr(
	const optWhereClause_WHERE_aExpr* poptWhereClause_WHERE_aExpr
	)
{
	poptWhereClause_WHERE_aExpr->m_paExpr2->accept_interface(this);
}
void WQLSelectStatementGen::visit_rowExpr(
	const rowExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_rowDescriptor(
	const rowDescriptor*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_cExpr(
	const aExpr_cExpr* paExpr_cExpr
	)
{
	paExpr_cExpr->m_pcExpr1->accept_interface(this);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_AT_TIME_ZONE_cExpr(
	const aExpr_aExpr_AT_TIME_ZONE_cExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_PLUS_aExpr(
	const aExpr_PLUS_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_MINUS_aExpr(
	const aExpr_MINUS_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_BITINVERT_aExpr(
	const aExpr_BITINVERT_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_PLUS_aExpr(
	const aExpr_aExpr_PLUS_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_MINUS_aExpr(
	const aExpr_aExpr_MINUS_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_ASTERISK_aExpr(
	const aExpr_aExpr_ASTERISK_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_SOLIDUS_aExpr(
	const aExpr_aExpr_SOLIDUS_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_PERCENT_aExpr(
	const aExpr_aExpr_PERCENT_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_BITAND_aExpr(
	const aExpr_aExpr_BITAND_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_BITOR_aExpr(
	const aExpr_aExpr_BITOR_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_BITSHIFTLEFT_aExpr(
	const aExpr_aExpr_BITSHIFTLEFT_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_BITSHIFTRIGHT_aExpr(
	const aExpr_aExpr_BITSHIFTRIGHT_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_LESSTHAN_aExpr(
	const aExpr_aExpr_LESSTHAN_aExpr* paExpr_aExpr_LESSTHAN_aExpr
	)
{
	paExpr_aExpr_LESSTHAN_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_LESSTHAN_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_LT);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_LESSTHANOREQUALS_aExpr(
	const aExpr_aExpr_LESSTHANOREQUALS_aExpr* paExpr_aExpr_LESSTHANOREQUALS_aExpr
	)
{
	paExpr_aExpr_LESSTHANOREQUALS_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_LESSTHANOREQUALS_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_LE);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_GREATERTHAN_aExpr(
	const aExpr_aExpr_GREATERTHAN_aExpr* paExpr_aExpr_GREATERTHAN_aExpr
	)
{
	paExpr_aExpr_GREATERTHAN_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_GREATERTHAN_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_GT);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_GREATERTHANOREQUALS_aExpr(
	const aExpr_aExpr_GREATERTHANOREQUALS_aExpr* paExpr_aExpr_GREATERTHANOREQUALS_aExpr
	)
{
	paExpr_aExpr_GREATERTHANOREQUALS_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_GREATERTHANOREQUALS_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_GE);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_EQUALS_aExpr(
	const aExpr_aExpr_EQUALS_aExpr* paExpr_aExpr_EQUALS_aExpr
	)
{
	paExpr_aExpr_EQUALS_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_EQUALS_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_EQ);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_NOTEQUALS_aExpr(
	const aExpr_aExpr_NOTEQUALS_aExpr* paExpr_aExpr_NOTEQUALS_aExpr
	)
{
	paExpr_aExpr_NOTEQUALS_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_NOTEQUALS_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_NE);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_AND_aExpr(
	const aExpr_aExpr_AND_aExpr* paExpr_aExpr_AND_aExpr
	)
{
	paExpr_aExpr_AND_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_AND_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_AND);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_OR_aExpr(
	const aExpr_aExpr_OR_aExpr* paExpr_aExpr_OR_aExpr
	)
{
	paExpr_aExpr_OR_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_OR_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_OR);
}
void WQLSelectStatementGen::visit_aExpr_NOT_aExpr(
	const aExpr_NOT_aExpr* paExpr_NOT_aExpr
	)
{
	paExpr_NOT_aExpr->m_paExpr2->accept_interface(this);
	m_stmt.appendOperation(WQL_NOT);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_CONCATENATION_aExpr(
	const aExpr_aExpr_CONCATENATION_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_LIKE_aExpr(
	const aExpr_aExpr_LIKE_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr(
	const aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_NOT_LIKE_aExpr(
	const aExpr_aExpr_NOT_LIKE_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr(
	const aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_ISNULL(
	const aExpr_aExpr_ISNULL* paExpr_aExpr_ISNULL
	)
{
	paExpr_aExpr_ISNULL->m_paExpr1->accept_interface(this);
	m_stmt.appendOperand(WQLOperand()); // default constructor creates NULL operand
	m_stmt.appendOperation(WQL_EQ);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_IS_NULLP(
	const aExpr_aExpr_IS_NULLP* paExpr_aExpr_IS_NULLP
	)
{
	paExpr_aExpr_IS_NULLP->m_paExpr1->accept_interface(this);
	m_stmt.appendOperand(WQLOperand()); // default constructor creates NULL operand
	m_stmt.appendOperation(WQL_EQ);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_NOTNULL(
	const aExpr_aExpr_NOTNULL* paExpr_aExpr_NOTNULL
	)
{
	paExpr_aExpr_NOTNULL->m_paExpr1->accept_interface(this);
	m_stmt.appendOperand(WQLOperand()); // default constructor creates NULL operand
	m_stmt.appendOperation(WQL_NE);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_IS_NOT_NULLP(
	const aExpr_aExpr_IS_NOT_NULLP* paExpr_aExpr_IS_NOT_NULLP
	)
{
	paExpr_aExpr_IS_NOT_NULLP->m_paExpr1->accept_interface(this);
	m_stmt.appendOperand(WQLOperand()); // default constructor creates NULL operand
	m_stmt.appendOperation(WQL_NE);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_IS_TRUEP(
	const aExpr_aExpr_IS_TRUEP* paExpr_aExpr_IS_TRUEP
	)
{
	paExpr_aExpr_IS_TRUEP->m_paExpr1->accept_interface(this);
	m_stmt.appendOperand(WQLOperand(true, WQL_BOOLEAN_VALUE_TAG));
	m_stmt.appendOperation(WQL_EQ);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_IS_NOT_FALSEP(
	const aExpr_aExpr_IS_NOT_FALSEP* paExpr_aExpr_IS_NOT_FALSEP
	)
{
	paExpr_aExpr_IS_NOT_FALSEP->m_paExpr1->accept_interface(this);
	m_stmt.appendOperand(WQLOperand(false, WQL_BOOLEAN_VALUE_TAG));
	m_stmt.appendOperation(WQL_NE);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_IS_FALSEP(
	const aExpr_aExpr_IS_FALSEP* paExpr_aExpr_IS_FALSEP
	)
{
	paExpr_aExpr_IS_FALSEP->m_paExpr1->accept_interface(this);
	m_stmt.appendOperand(WQLOperand(false, WQL_BOOLEAN_VALUE_TAG));
	m_stmt.appendOperation(WQL_EQ);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_IS_NOT_TRUEP(
	const aExpr_aExpr_IS_NOT_TRUEP* paExpr_aExpr_IS_NOT_TRUEP
	)
{
	paExpr_aExpr_IS_NOT_TRUEP->m_paExpr1->accept_interface(this);
	m_stmt.appendOperand(WQLOperand(true, WQL_BOOLEAN_VALUE_TAG));
	m_stmt.appendOperation(WQL_NE);
}
void WQLSelectStatementGen::visit_aExpr_aExpr_ISA_aExpr(
		const aExpr_aExpr_ISA_aExpr* paExpr_aExpr_ISA_aExpr
		)
{
	paExpr_aExpr_ISA_aExpr->m_paExpr1->accept_interface(this);
	paExpr_aExpr_ISA_aExpr->m_paExpr3->accept_interface(this);
	m_stmt.appendOperation(WQL_ISA);
}
void WQLSelectStatementGen::visit_aExpr_rowExpr(
	const aExpr_rowExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_cExpr(
	const bExpr_cExpr* pbExpr_cExpr
	)
{
	pbExpr_cExpr->m_pcExpr1->accept_interface(this);
}
void WQLSelectStatementGen::visit_bExpr_PLUS_bExpr(
	const bExpr_PLUS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_MINUS_bExpr(
	const bExpr_MINUS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_BITINVERT_bExpr(
	const bExpr_BITINVERT_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_PLUS_bExpr(
	const bExpr_bExpr_PLUS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_MINUS_bExpr(
	const bExpr_bExpr_MINUS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_ASTERISK_bExpr(
	const bExpr_bExpr_ASTERISK_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_SOLIDUS_bExpr(
	const bExpr_bExpr_SOLIDUS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_PERCENT_bExpr(
	const bExpr_bExpr_PERCENT_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_BITAND_bExpr(
	const bExpr_bExpr_BITAND_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_BITOR_bExpr(
	const bExpr_bExpr_BITOR_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_BITSHIFTLEFT_bExpr(
	const bExpr_bExpr_BITSHIFTLEFT_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_BITSHIFTRIGHT_bExpr(
	const bExpr_bExpr_BITSHIFTRIGHT_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_LESSTHAN_bExpr(
	const bExpr_bExpr_LESSTHAN_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_LESSTHANOREQUALS_bExpr(
	const bExpr_bExpr_LESSTHANOREQUALS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_GREATERTHAN_bExpr(
	const bExpr_bExpr_GREATERTHAN_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_GREATERTHANOREQUALS_bExpr(
	const bExpr_bExpr_GREATERTHANOREQUALS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_EQUALS_bExpr(
	const bExpr_bExpr_EQUALS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_NOTEQUALS_bExpr(
	const bExpr_bExpr_NOTEQUALS_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_bExpr_bExpr_CONCATENATION_bExpr(
	const bExpr_bExpr_CONCATENATION_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_attr(
	const cExpr_attr* pcExpr_attr
	)
{
	pcExpr_attr->m_pattr1->accept_interface(this);
}
void WQLSelectStatementGen::visit_cExpr_strColId_optIndirection(
	const cExpr_strColId_optIndirection* pcExpr_strColId_optIndirection
	)
{
	if (m_isPropertyList)
	{
		m_stmt.appendSelectPropertyName(*pcExpr_strColId_optIndirection->m_pstrColId1);
	}
	else
	{
		m_stmt.appendOperand(WQLOperand(*pcExpr_strColId_optIndirection->m_pstrColId1, WQL_PROPERTY_NAME_TAG));
		m_stmt.appendWherePropertyName(*pcExpr_strColId_optIndirection->m_pstrColId1);
	}
	if (pcExpr_strColId_optIndirection->m_poptIndirection2)
	{
		pcExpr_strColId_optIndirection->m_poptIndirection2->accept_interface(this);
	}
}
void WQLSelectStatementGen::visit_cExpr_aExprConst(
	const cExpr_aExprConst* pcExpr_aExprConst
	)
{
	pcExpr_aExprConst->m_paExprConst1->accept_interface(this);
}
void WQLSelectStatementGen::visit_cExpr_LEFTPAREN_aExpr_RIGHTPAREN(
	const cExpr_LEFTPAREN_aExpr_RIGHTPAREN* pcExpr_LEFTPAREN_aExpr_RIGHTPAREN
	)
{
	pcExpr_LEFTPAREN_aExpr_RIGHTPAREN->m_paExpr2->accept_interface(this);
}
void WQLSelectStatementGen::visit_cExpr_strFuncName_LEFTPAREN_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_RIGHTPAREN
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_CURRENTDATE(
	const cExpr_CURRENTDATE*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_CURRENTTIME(
	const cExpr_CURRENTTIME*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN(
	const cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_CURRENTTIMESTAMP(
	const cExpr_CURRENTTIMESTAMP*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN(
	const cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_CURRENTUSER(
	const cExpr_CURRENTUSER*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_SESSIONUSER(
	const cExpr_SESSIONUSER*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_USER(
	const cExpr_USER*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN(
	const cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN(
	const cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN(
	const cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN(
	const cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN(
	const cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN(
	const cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optIndirection_empty(
	const optIndirection_empty* poptIndirection_empty
	)
{
}
void WQLSelectStatementGen::visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET(
	const optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET(
	const optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optExtract_empty(
	const optExtract_empty* poptExtract_empty
	)
{
}
void WQLSelectStatementGen::visit_optExtract_strExtractArg_FROM_aExpr(
	const optExtract_strExtractArg_FROM_aExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_positionExpr_bExpr_IN_bExpr(
	const positionExpr_bExpr_IN_bExpr*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_positionExpr_empty(
	const positionExpr_empty* ppositionExpr_empty
	)
{
}
void WQLSelectStatementGen::visit_optSubstrExpr_empty(
	const optSubstrExpr_empty* poptSubstrExpr_empty
	)
{
}
void WQLSelectStatementGen::visit_optSubstrExpr_aExpr_substrFrom_substrFor(
	const optSubstrExpr_aExpr_substrFrom_substrFor*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optSubstrExpr_aExpr_substrFor_substrFrom(
	const optSubstrExpr_aExpr_substrFor_substrFrom*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optSubstrExpr_aExpr_substrFrom(
	const optSubstrExpr_aExpr_substrFrom*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optSubstrExpr_aExpr_substrFor(
	const optSubstrExpr_aExpr_substrFor*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_optSubstrExpr_exprSeq(
	const optSubstrExpr_exprSeq*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_substrFrom(
	const substrFrom*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_substrFor(
	const substrFor*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_trimExpr_aExpr_FROM_exprSeq(
	const trimExpr_aExpr_FROM_exprSeq*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_trimExpr_FROM_exprSeq(
	const trimExpr_FROM_exprSeq*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_trimExpr_exprSeq(
	const trimExpr_exprSeq*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_attr(
	const attr* pattr
	)
{
	// This handles embedded properties or ClassName.PropertyName
	m_attrName = *pattr->m_pstrRelationName1;
	
	// This call may append onto m_attrName
	pattr->m_pattrs3->accept_interface(this);
	if (m_isPropertyList)
	{
		m_stmt.appendSelectPropertyName(m_attrName);
	}
	else
	{
		m_stmt.appendOperand(WQLOperand(m_attrName, WQL_PROPERTY_NAME_TAG));
		m_stmt.appendWherePropertyName(m_attrName);
	}
		
	// TODO: What does indirection mean? Array index
	if (pattr->m_poptIndirection4)
	{
		pattr->m_poptIndirection4->accept_interface(this);
	}
}
void WQLSelectStatementGen::visit_attrs_strAttrName(
	const attrs_strAttrName* pattrs_strAttrName
	)
{
	m_attrName = m_attrName + "." + *pattrs_strAttrName->m_pstrAttrName1;
}
void WQLSelectStatementGen::visit_attrs_attrs_PERIOD_strAttrName(
	const attrs_attrs_PERIOD_strAttrName* pattrs_attrs_PERIOD_strAttrName
	)
{
	// This handles embedded properties or ClassName.PropertyName
	pattrs_attrs_PERIOD_strAttrName->m_pattrs1->accept_interface(this);
	m_attrName = m_attrName + "." + *pattrs_attrs_PERIOD_strAttrName->m_pstrAttrName3;
}
void WQLSelectStatementGen::visit_attrs_attrs_PERIOD_ASTERISK(
	const attrs_attrs_PERIOD_ASTERISK* pattrs_attrs_PERIOD_ASTERISK
	)
{
	// This handles embedded properties or ClassName.PropertyName
	pattrs_attrs_PERIOD_ASTERISK->m_pattrs1->accept_interface(this);
	m_attrName = m_attrName + ".*";
}
void WQLSelectStatementGen::visit_targetEl_aExpr_AS_strColLabel(
	const targetEl_aExpr_AS_strColLabel*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_targetEl_aExpr(
	const targetEl_aExpr* ptargetEl_aExpr
	)
{
	// setting this causes appendSelectPropertyName to be called
	m_isPropertyList = true;
	ptargetEl_aExpr->m_paExpr1->accept_interface(this);
	m_isPropertyList = false;
}
void WQLSelectStatementGen::visit_targetEl_strRelationName_PERIOD_ASTERISK(
	const targetEl_strRelationName_PERIOD_ASTERISK* ptargetEl_strRelationName_PERIOD_ASTERISK
	)
{
	m_stmt.appendSelectPropertyName(*ptargetEl_strRelationName_PERIOD_ASTERISK->m_pstrRelationName1 + ".*");
	// this isn't correct because it could be an embedded property, we need to keep the relation name
	//m_stmt.appendSelectPropertyName("*");
}
void WQLSelectStatementGen::visit_targetEl_ASTERISK(
	const targetEl_ASTERISK*
	)
{
	m_stmt.appendSelectPropertyName("*");
}
void WQLSelectStatementGen::visit_updateTargetEl(
	const updateTargetEl*
	)
{
	OW_THROWCIM(CIMException::INVALID_QUERY);
}
void WQLSelectStatementGen::visit_aExprConst_ICONST(
	const aExprConst_ICONST* paExprConst_ICONST
	)
{
	try
	{
		m_stmt.appendOperand(WQLOperand(paExprConst_ICONST->m_pICONST1->toInt64(), WQL_INTEGER_VALUE_TAG));
	}
	catch (const StringConversionException& e)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, e.getMessage());
	}
}
void WQLSelectStatementGen::visit_aExprConst_FCONST(
	const aExprConst_FCONST* paExprConst_FCONST
	)
{
	try
	{
		m_stmt.appendOperand(WQLOperand(paExprConst_FCONST->m_pFCONST1->toReal64(), WQL_DOUBLE_VALUE_TAG));
	}
	catch (const StringConversionException& e)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, e.getMessage());
	}
}
void WQLSelectStatementGen::visit_aExprConst_SCONST(
	const aExprConst_SCONST* paExprConst_SCONST
	)
{
	m_stmt.appendOperand(WQLOperand(WQLRemoveStringEscapes(*paExprConst_SCONST->m_pSCONST1), WQL_STRING_VALUE_TAG));
}
void WQLSelectStatementGen::visit_aExprConst_BITCONST(
	const aExprConst_BITCONST* paExprConst_BITCONST
	)
{
	try
	{
		m_stmt.appendOperand(WQLOperand(paExprConst_BITCONST->m_pBITCONST1->toInt64(2), WQL_INTEGER_VALUE_TAG));
	}
	catch (const StringConversionException& e)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, e.getMessage());
	}
}
void WQLSelectStatementGen::visit_aExprConst_HEXCONST(
	const aExprConst_HEXCONST* paExprConst_HEXCONST
	)
{
	try
	{
		m_stmt.appendOperand(WQLOperand(paExprConst_HEXCONST->m_pHEXCONST1->toInt64(16), WQL_INTEGER_VALUE_TAG));
	}
	catch (const StringConversionException& e)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, e.getMessage());
	}
}
void WQLSelectStatementGen::visit_aExprConst_TRUEP(
	const aExprConst_TRUEP*
	)
{
	m_stmt.appendOperand(WQLOperand(true, WQL_BOOLEAN_VALUE_TAG));
}
void WQLSelectStatementGen::visit_aExprConst_FALSEP(
	const aExprConst_FALSEP*
	)
{
	m_stmt.appendOperand(WQLOperand(false, WQL_BOOLEAN_VALUE_TAG));
}
void WQLSelectStatementGen::visit_aExprConst_NULLP(
	const aExprConst_NULLP*
	)
{
	m_stmt.appendOperand(WQLOperand());  // WQLOperand defaults to NULL
}

} // end namespace OpenWBEM

