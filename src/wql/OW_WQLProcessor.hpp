/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_WQL_PROCESSOR_HPP_
#define OW_WQL_PROCESSOR_HPP_
#include "OW_config.h"
#include "OW_WQLVisitor.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_Reference.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Array.hpp"
#include "OW_Bool.hpp"

namespace OpenWBEM
{

class WQLProcessor : public WQLVisitor
{
public:
	WQLProcessor(const Reference<CIMOMHandleIFC>& hdl, const String& ns);
	virtual ~WQLProcessor()
	{
	}
	virtual void visit_stmt_selectStmt_optSemicolon(
		const stmt_selectStmt_optSemicolon* pstmt_selectStmt_optSemicolon
		);
	virtual void visit_stmt_updateStmt_optSemicolon(
		const stmt_updateStmt_optSemicolon* pstmt_updateStmt_optSemicolon
		);
	virtual void visit_stmt_insertStmt_optSemicolon(
		const stmt_insertStmt_optSemicolon* pstmt_insertStmt_optSemicolon
		);
	virtual void visit_stmt_deleteStmt_optSemicolon(
		const stmt_deleteStmt_optSemicolon* pstmt_deleteStmt_optSemicolon
		);
	virtual void visit_optSemicolon_empty(
		const optSemicolon_empty* poptSemicolon_empty
		);
	virtual void visit_optSemicolon_SEMICOLON(
		const optSemicolon_SEMICOLON* poptSemicolon_SEMICOLON
		);
	virtual void visit_insertStmt(
		const insertStmt* pinsertStmt
		);
	virtual void visit_insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
		const insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN* pinsertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN
		);
	virtual void visit_insertRest_DEFAULT_VALUES(
		const insertRest_DEFAULT_VALUES* pinsertRest_DEFAULT_VALUES
		);
	virtual void visit_insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
		const insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN* pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN
		);
	virtual void visit_deleteStmt(
		const deleteStmt* pdeleteStmt
		);
	virtual void visit_updateStmt(
		const updateStmt* pupdateStmt
		);
	virtual void visit_selectStmt(
		const selectStmt* pselectStmt
		);
	virtual void visit_exprSeq_aExpr(
		const exprSeq_aExpr* pexprSeq_aExpr
		);
	virtual void visit_exprSeq_exprSeq_COMMA_aExpr(
		const exprSeq_exprSeq_COMMA_aExpr* pexprSeq_exprSeq_COMMA_aExpr
		);
	virtual void visit_exprSeq_exprSeq_USING_aExpr(
		const exprSeq_exprSeq_USING_aExpr* pexprSeq_exprSeq_USING_aExpr
		);
	virtual void visit_optDistinct_empty(
		const optDistinct_empty* poptDistinct_empty
		);
	virtual void visit_optDistinct_DISTINCT(
		const optDistinct_DISTINCT* poptDistinct_DISTINCT
		);
	virtual void visit_optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN(
		const optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN* poptDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN
		);
	virtual void visit_optDistinct_ALL(
		const optDistinct_ALL* poptDistinct_ALL
		);
	virtual void visit_sortClause(
		const sortClause* psortClause
		);
	virtual void visit_optSortClause_empty(
		const optSortClause_empty* poptSortClause_empty
		);
	virtual void visit_optSortClause_sortClause(
		const optSortClause_sortClause* poptSortClause_sortClause
		);
	virtual void visit_sortby(
		const sortby* psortby
		);
	virtual void visit_optGroupClause_empty(
		const optGroupClause_empty* poptGroupClause_empty
		);
	virtual void visit_optGroupClause_GROUP_BY_exprSeq(
		const optGroupClause_GROUP_BY_exprSeq* poptGroupClause_GROUP_BY_exprSeq
		);
	virtual void visit_optHavingClause_empty(
		const optHavingClause_empty* poptHavingClause_empty
		);
	virtual void visit_optHavingClause_HAVING_aExpr(
		const optHavingClause_HAVING_aExpr* poptHavingClause_HAVING_aExpr
		);
	virtual void visit_optFromClause_empty(
		const optFromClause_empty* poptFromClause_empty
		);
	virtual void visit_optFromClause_FROM_fromList(
		const optFromClause_FROM_fromList* poptFromClause_FROM_fromList
		);
	virtual void visit_tableRef_relationExpr(
		const tableRef_relationExpr* ptableRef_relationExpr
		);
	virtual void visit_tableRef_relationExpr_aliasClause(
		const tableRef_relationExpr_aliasClause* ptableRef_relationExpr_aliasClause
		);
	virtual void visit_tableRef_joinedTable(
		const tableRef_joinedTable* ptableRef_joinedTable
		);
	virtual void visit_tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause(
		const tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause* ptableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause
		);
	virtual void visit_joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN(
		const joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN* pjoinedTable_LEFTPAREN_joinedTable_RIGHTPAREN
		);
	virtual void visit_joinedTable_tableRef_CROSS_JOIN_tableRef(
		const joinedTable_tableRef_CROSS_JOIN_tableRef* pjoinedTable_tableRef_CROSS_JOIN_tableRef
		);
	virtual void visit_joinedTable_tableRef_UNIONJOIN_tableRef(
		const joinedTable_tableRef_UNIONJOIN_tableRef* pjoinedTable_tableRef_UNIONJOIN_tableRef
		);
	virtual void visit_joinedTable_tableRef_joinType_JOIN_tableRef_joinQual(
		const joinedTable_tableRef_joinType_JOIN_tableRef_joinQual* pjoinedTable_tableRef_joinType_JOIN_tableRef_joinQual
		);
	virtual void visit_joinedTable_tableRef_JOIN_tableRef_joinQual(
		const joinedTable_tableRef_JOIN_tableRef_joinQual* pjoinedTable_tableRef_JOIN_tableRef_joinQual
		);
	virtual void visit_joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef(
		const joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef* pjoinedTable_tableRef_NATURAL_joinType_JOIN_tableRef
		);
	virtual void visit_joinedTable_tableRef_NATURAL_JOIN_tableRef(
		const joinedTable_tableRef_NATURAL_JOIN_tableRef* pjoinedTable_tableRef_NATURAL_JOIN_tableRef
		);
	virtual void visit_aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN(
		const aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN* paliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN
		);
	virtual void visit_aliasClause_AS_strColId(
		const aliasClause_AS_strColId* paliasClause_AS_strColId
		);
	virtual void visit_aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN(
		const aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN* paliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN
		);
	virtual void visit_aliasClause_strColId(
		const aliasClause_strColId* paliasClause_strColId
		);
	virtual void visit_joinType_FULL_strOptJoinOuter(
		const joinType_FULL_strOptJoinOuter* pjoinType_FULL_strOptJoinOuter
		);
	virtual void visit_joinType_LEFT_strOptJoinOuter(
		const joinType_LEFT_strOptJoinOuter* pjoinType_LEFT_strOptJoinOuter
		);
	virtual void visit_joinType_RIGHT_strOptJoinOuter(
		const joinType_RIGHT_strOptJoinOuter* pjoinType_RIGHT_strOptJoinOuter
		);
	virtual void visit_joinType_INNERP(
		const joinType_INNERP* pjoinType_INNERP
		);
	virtual void visit_joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN(
		const joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN* pjoinQual_USING_LEFTPAREN_nameList_RIGHTPAREN
		);
	virtual void visit_joinQual_ON_aExpr(
		const joinQual_ON_aExpr* pjoinQual_ON_aExpr
		);
	virtual void visit_relationExpr_strRelationName(
		const relationExpr_strRelationName* prelationExpr_strRelationName
		);
	virtual void visit_relationExpr_strRelationName_ASTERISK(
		const relationExpr_strRelationName_ASTERISK* prelationExpr_strRelationName_ASTERISK
		);
	virtual void visit_relationExpr_ONLY_strRelationName(
		const relationExpr_ONLY_strRelationName* prelationExpr_ONLY_strRelationName
		);
	virtual void visit_optWhereClause_empty(
		const optWhereClause_empty* poptWhereClause_empty
		);
	virtual void visit_optWhereClause_WHERE_aExpr(
		const optWhereClause_WHERE_aExpr* poptWhereClause_WHERE_aExpr
		);
	virtual void visit_rowExpr(
		const rowExpr* prowExpr
		);
	virtual void visit_rowDescriptor(
		const rowDescriptor* prowDescriptor
		);
	virtual void visit_aExpr_cExpr(
		const aExpr_cExpr* paExpr_cExpr
		);
	virtual void visit_aExpr_aExpr_AT_TIME_ZONE_cExpr(
		const aExpr_aExpr_AT_TIME_ZONE_cExpr* paExpr_aExpr_AT_TIME_ZONE_cExpr
		);
	virtual void visit_aExpr_PLUS_aExpr(
		const aExpr_PLUS_aExpr* paExpr_PLUS_aExpr
		);
	virtual void visit_aExpr_MINUS_aExpr(
		const aExpr_MINUS_aExpr* paExpr_MINUS_aExpr
		);
	virtual void visit_aExpr_BITINVERT_aExpr(
		const aExpr_BITINVERT_aExpr* paExpr_BITINVERT_aExpr
		);
	virtual void visit_aExpr_aExpr_PLUS_aExpr(
		const aExpr_aExpr_PLUS_aExpr* paExpr_aExpr_PLUS_aExpr
		);
	virtual void visit_aExpr_aExpr_MINUS_aExpr(
		const aExpr_aExpr_MINUS_aExpr* paExpr_aExpr_MINUS_aExpr
		);
	virtual void visit_aExpr_aExpr_ASTERISK_aExpr(
		const aExpr_aExpr_ASTERISK_aExpr* paExpr_aExpr_ASTERISK_aExpr
		);
	virtual void visit_aExpr_aExpr_SOLIDUS_aExpr(
		const aExpr_aExpr_SOLIDUS_aExpr* paExpr_aExpr_SOLIDUS_aExpr
		);
	virtual void visit_aExpr_aExpr_PERCENT_aExpr(
		const aExpr_aExpr_PERCENT_aExpr* paExpr_aExpr_PERCENT_aExpr
		);
	virtual void visit_aExpr_aExpr_BITAND_aExpr(
		const aExpr_aExpr_BITAND_aExpr* paExpr_aExpr_BITAND_aExpr
		);
	virtual void visit_aExpr_aExpr_BITOR_aExpr(
		const aExpr_aExpr_BITOR_aExpr* paExpr_aExpr_BITOR_aExpr
		);
	virtual void visit_aExpr_aExpr_BITSHIFTLEFT_aExpr(
		const aExpr_aExpr_BITSHIFTLEFT_aExpr* paExpr_aExpr_BITSHIFTLEFT_aExpr
		);
	virtual void visit_aExpr_aExpr_BITSHIFTRIGHT_aExpr(
		const aExpr_aExpr_BITSHIFTRIGHT_aExpr* paExpr_aExpr_BITSHIFTRIGHT_aExpr
		);
	virtual void visit_aExpr_aExpr_LESSTHAN_aExpr(
		const aExpr_aExpr_LESSTHAN_aExpr* paExpr_aExpr_LESSTHAN_aExpr
		);
	virtual void visit_aExpr_aExpr_LESSTHANOREQUALS_aExpr(
		const aExpr_aExpr_LESSTHANOREQUALS_aExpr* paExpr_aExpr_LESSTHANOREQUALS_aExpr
		);
	virtual void visit_aExpr_aExpr_GREATERTHAN_aExpr(
		const aExpr_aExpr_GREATERTHAN_aExpr* paExpr_aExpr_GREATERTHAN_aExpr
		);
	virtual void visit_aExpr_aExpr_GREATERTHANOREQUALS_aExpr(
		const aExpr_aExpr_GREATERTHANOREQUALS_aExpr* paExpr_aExpr_GREATERTHANOREQUALS_aExpr
		);
	virtual void visit_aExpr_aExpr_EQUALS_aExpr(
		const aExpr_aExpr_EQUALS_aExpr* paExpr_aExpr_EQUALS_aExpr
		);
	virtual void visit_aExpr_aExpr_NOTEQUALS_aExpr(
		const aExpr_aExpr_NOTEQUALS_aExpr* paExpr_aExpr_NOTEQUALS_aExpr
		);
	virtual void visit_aExpr_aExpr_AND_aExpr(
		const aExpr_aExpr_AND_aExpr* paExpr_aExpr_AND_aExpr
		);
	virtual void visit_aExpr_aExpr_OR_aExpr(
		const aExpr_aExpr_OR_aExpr* paExpr_aExpr_OR_aExpr
		);
	virtual void visit_aExpr_NOT_aExpr(
		const aExpr_NOT_aExpr* paExpr_NOT_aExpr
		);
	virtual void visit_aExpr_aExpr_CONCATENATION_aExpr(
		const aExpr_aExpr_CONCATENATION_aExpr* paExpr_aExpr_CONCATENATION_aExpr
		);
	virtual void visit_aExpr_aExpr_LIKE_aExpr(
		const aExpr_aExpr_LIKE_aExpr* paExpr_aExpr_LIKE_aExpr
		);
	virtual void visit_aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr(
		const aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr* paExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr
		);
	virtual void visit_aExpr_aExpr_NOT_LIKE_aExpr(
		const aExpr_aExpr_NOT_LIKE_aExpr* paExpr_aExpr_NOT_LIKE_aExpr
		);
	virtual void visit_aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr(
		const aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr* paExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr
		);
	virtual void visit_aExpr_aExpr_ISNULL(
		const aExpr_aExpr_ISNULL* paExpr_aExpr_ISNULL
		);
	virtual void visit_aExpr_aExpr_IS_NULLP(
		const aExpr_aExpr_IS_NULLP* paExpr_aExpr_IS_NULLP
		);
	virtual void visit_aExpr_aExpr_NOTNULL(
		const aExpr_aExpr_NOTNULL* paExpr_aExpr_NOTNULL
		);
	virtual void visit_aExpr_aExpr_IS_NOT_NULLP(
		const aExpr_aExpr_IS_NOT_NULLP* paExpr_aExpr_IS_NOT_NULLP
		);
	virtual void visit_aExpr_aExpr_IS_TRUEP(
		const aExpr_aExpr_IS_TRUEP* paExpr_aExpr_IS_TRUEP
		);
	virtual void visit_aExpr_aExpr_IS_NOT_FALSEP(
		const aExpr_aExpr_IS_NOT_FALSEP* paExpr_aExpr_IS_NOT_FALSEP
		);
	virtual void visit_aExpr_aExpr_IS_FALSEP(
		const aExpr_aExpr_IS_FALSEP* paExpr_aExpr_IS_FALSEP
		);
	virtual void visit_aExpr_aExpr_IS_NOT_TRUEP(
		const aExpr_aExpr_IS_NOT_TRUEP* paExpr_aExpr_IS_NOT_TRUEP
		);
	virtual void visit_aExpr_aExpr_ISA_aExpr(
		const aExpr_aExpr_ISA_aExpr* paExpr_aExpr_ISA_aExpr
		);
	
	virtual void visit_aExpr_rowExpr(
		const aExpr_rowExpr* paExpr_rowExpr
		);
	virtual void visit_bExpr_cExpr(
		const bExpr_cExpr* pbExpr_cExpr
		);
	virtual void visit_bExpr_PLUS_bExpr(
		const bExpr_PLUS_bExpr* pbExpr_PLUS_bExpr
		);
	virtual void visit_bExpr_MINUS_bExpr(
		const bExpr_MINUS_bExpr* pbExpr_MINUS_bExpr
		);
	virtual void visit_bExpr_BITINVERT_bExpr(
		const bExpr_BITINVERT_bExpr* pbExpr_BITINVERT_bExpr
		);
	virtual void visit_bExpr_bExpr_PLUS_bExpr(
		const bExpr_bExpr_PLUS_bExpr* pbExpr_bExpr_PLUS_bExpr
		);
	virtual void visit_bExpr_bExpr_MINUS_bExpr(
		const bExpr_bExpr_MINUS_bExpr* pbExpr_bExpr_MINUS_bExpr
		);
	virtual void visit_bExpr_bExpr_ASTERISK_bExpr(
		const bExpr_bExpr_ASTERISK_bExpr* pbExpr_bExpr_ASTERISK_bExpr
		);
	virtual void visit_bExpr_bExpr_SOLIDUS_bExpr(
		const bExpr_bExpr_SOLIDUS_bExpr* pbExpr_bExpr_SOLIDUS_bExpr
		);
	virtual void visit_bExpr_bExpr_PERCENT_bExpr(
		const bExpr_bExpr_PERCENT_bExpr* pbExpr_bExpr_PERCENT_bExpr
		);
	virtual void visit_bExpr_bExpr_BITAND_bExpr(
		const bExpr_bExpr_BITAND_bExpr* pbExpr_bExpr_BITAND_bExpr
		);
	virtual void visit_bExpr_bExpr_BITOR_bExpr(
		const bExpr_bExpr_BITOR_bExpr* pbExpr_bExpr_BITOR_bExpr
		);
	virtual void visit_bExpr_bExpr_BITSHIFTLEFT_bExpr(
		const bExpr_bExpr_BITSHIFTLEFT_bExpr* pbExpr_bExpr_BITSHIFTLEFT_bExpr
		);
	virtual void visit_bExpr_bExpr_BITSHIFTRIGHT_bExpr(
		const bExpr_bExpr_BITSHIFTRIGHT_bExpr* pbExpr_bExpr_BITSHIFTRIGHT_bExpr
		);
	virtual void visit_bExpr_bExpr_LESSTHAN_bExpr(
		const bExpr_bExpr_LESSTHAN_bExpr* pbExpr_bExpr_LESSTHAN_bExpr
		);
	virtual void visit_bExpr_bExpr_LESSTHANOREQUALS_bExpr(
		const bExpr_bExpr_LESSTHANOREQUALS_bExpr* pbExpr_bExpr_LESSTHANOREQUALS_bExpr
		);
	virtual void visit_bExpr_bExpr_GREATERTHAN_bExpr(
		const bExpr_bExpr_GREATERTHAN_bExpr* pbExpr_bExpr_GREATERTHAN_bExpr
		);
	virtual void visit_bExpr_bExpr_GREATERTHANOREQUALS_bExpr(
		const bExpr_bExpr_GREATERTHANOREQUALS_bExpr* pbExpr_bExpr_GREATERTHANOREQUALS_bExpr
		);
	virtual void visit_bExpr_bExpr_EQUALS_bExpr(
		const bExpr_bExpr_EQUALS_bExpr* pbExpr_bExpr_EQUALS_bExpr
		);
	virtual void visit_bExpr_bExpr_NOTEQUALS_bExpr(
		const bExpr_bExpr_NOTEQUALS_bExpr* pbExpr_bExpr_NOTEQUALS_bExpr
		);
	virtual void visit_bExpr_bExpr_CONCATENATION_bExpr(
		const bExpr_bExpr_CONCATENATION_bExpr* pbExpr_bExpr_CONCATENATION_bExpr
		);
	virtual void visit_cExpr_attr(
		const cExpr_attr* pcExpr_attr
		);
	virtual void visit_cExpr_strColId_optIndirection(
		const cExpr_strColId_optIndirection* pcExpr_strColId_optIndirection
		);
	virtual void visit_cExpr_aExprConst(
		const cExpr_aExprConst* pcExpr_aExprConst
		);
	virtual void visit_cExpr_LEFTPAREN_aExpr_RIGHTPAREN(
		const cExpr_LEFTPAREN_aExpr_RIGHTPAREN* pcExpr_LEFTPAREN_aExpr_RIGHTPAREN
		);
	virtual void visit_cExpr_strFuncName_LEFTPAREN_RIGHTPAREN(
		const cExpr_strFuncName_LEFTPAREN_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_RIGHTPAREN
		);
	virtual void visit_cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN(
		const cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN
		);
	virtual void visit_cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN(
		const cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN
		);
	virtual void visit_cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN(
		const cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN
		);
	virtual void visit_cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN(
		const cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN
		);
	virtual void visit_cExpr_CURRENTDATE(
		const cExpr_CURRENTDATE* pcExpr_CURRENTDATE
		);
	virtual void visit_cExpr_CURRENTTIME(
		const cExpr_CURRENTTIME* pcExpr_CURRENTTIME
		);
	virtual void visit_cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN(
		const cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN* pcExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN
		);
	virtual void visit_cExpr_CURRENTTIMESTAMP(
		const cExpr_CURRENTTIMESTAMP* pcExpr_CURRENTTIMESTAMP
		);
	virtual void visit_cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN(
		const cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN* pcExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN
		);
	virtual void visit_cExpr_CURRENTUSER(
		const cExpr_CURRENTUSER* pcExpr_CURRENTUSER
		);
	virtual void visit_cExpr_SESSIONUSER(
		const cExpr_SESSIONUSER* pcExpr_SESSIONUSER
		);
	virtual void visit_cExpr_USER(
		const cExpr_USER* pcExpr_USER
		);
	virtual void visit_cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN(
		const cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN* pcExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN
		);
	virtual void visit_cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN(
		const cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN* pcExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN
		);
	virtual void visit_cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN(
		const cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN* pcExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN
		);
	virtual void visit_cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN(
		const cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN
		);
	virtual void visit_cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN(
		const cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN
		);
	virtual void visit_cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN(
		const cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN
		);
	virtual void visit_optIndirection_empty(
		const optIndirection_empty* poptIndirection_empty
		);
	virtual void visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET(
		const optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET* poptIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET
		);
	virtual void visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET(
		const optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET* poptIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET
		);
	virtual void visit_optExtract_empty(
		const optExtract_empty* poptExtract_empty
		);
	virtual void visit_optExtract_strExtractArg_FROM_aExpr(
		const optExtract_strExtractArg_FROM_aExpr* poptExtract_strExtractArg_FROM_aExpr
		);
	virtual void visit_positionExpr_bExpr_IN_bExpr(
		const positionExpr_bExpr_IN_bExpr* ppositionExpr_bExpr_IN_bExpr
		);
	virtual void visit_positionExpr_empty(
		const positionExpr_empty* ppositionExpr_empty
		);
	virtual void visit_optSubstrExpr_empty(
		const optSubstrExpr_empty* poptSubstrExpr_empty
		);
	virtual void visit_optSubstrExpr_aExpr_substrFrom_substrFor(
		const optSubstrExpr_aExpr_substrFrom_substrFor* poptSubstrExpr_aExpr_substrFrom_substrFor
		);
	virtual void visit_optSubstrExpr_aExpr_substrFor_substrFrom(
		const optSubstrExpr_aExpr_substrFor_substrFrom* poptSubstrExpr_aExpr_substrFor_substrFrom
		);
	virtual void visit_optSubstrExpr_aExpr_substrFrom(
		const optSubstrExpr_aExpr_substrFrom* poptSubstrExpr_aExpr_substrFrom
		);
	virtual void visit_optSubstrExpr_aExpr_substrFor(
		const optSubstrExpr_aExpr_substrFor* poptSubstrExpr_aExpr_substrFor
		);
	virtual void visit_optSubstrExpr_exprSeq(
		const optSubstrExpr_exprSeq* poptSubstrExpr_exprSeq
		);
	virtual void visit_substrFrom(
		const substrFrom* psubstrFrom
		);
	virtual void visit_substrFor(
		const substrFor* psubstrFor
		);
	virtual void visit_trimExpr_aExpr_FROM_exprSeq(
		const trimExpr_aExpr_FROM_exprSeq* ptrimExpr_aExpr_FROM_exprSeq
		);
	virtual void visit_trimExpr_FROM_exprSeq(
		const trimExpr_FROM_exprSeq* ptrimExpr_FROM_exprSeq
		);
	virtual void visit_trimExpr_exprSeq(
		const trimExpr_exprSeq* ptrimExpr_exprSeq
		);
	virtual void visit_attr(
		const attr* pattr
		);
	virtual void visit_attrs_strAttrName(
		const attrs_strAttrName* pattrs_strAttrName
		);
	virtual void visit_attrs_attrs_PERIOD_strAttrName(
		const attrs_attrs_PERIOD_strAttrName* pattrs_attrs_PERIOD_strAttrName
		);
	virtual void visit_attrs_attrs_PERIOD_ASTERISK(
		const attrs_attrs_PERIOD_ASTERISK* pattrs_attrs_PERIOD_ASTERISK
		);
	virtual void visit_targetEl_aExpr_AS_strColLabel(
		const targetEl_aExpr_AS_strColLabel* ptargetEl_aExpr_AS_strColLabel
		);
	virtual void visit_targetEl_aExpr(
		const targetEl_aExpr* ptargetEl_aExpr
		);
	virtual void visit_targetEl_strRelationName_PERIOD_ASTERISK(
		const targetEl_strRelationName_PERIOD_ASTERISK* ptargetEl_strRelationName_PERIOD_ASTERISK
		);
	virtual void visit_targetEl_ASTERISK(
		const targetEl_ASTERISK* ptargetEl_ASTERISK
		);
	virtual void visit_updateTargetEl(
		const updateTargetEl* pupdateTargetEl
		);
	virtual void visit_aExprConst_ICONST(
		const aExprConst_ICONST* paExprConst_ICONST
		);
	virtual void visit_aExprConst_FCONST(
		const aExprConst_FCONST* paExprConst_FCONST
		);
	virtual void visit_aExprConst_SCONST(
		const aExprConst_SCONST* paExprConst_SCONST
		);
	virtual void visit_aExprConst_BITCONST(
		const aExprConst_BITCONST* paExprConst_BITCONST
		);
	virtual void visit_aExprConst_HEXCONST(
		const aExprConst_HEXCONST* paExprConst_HEXCONST
		);
	virtual void visit_aExprConst_TRUEP(
		const aExprConst_TRUEP* paExprConst_TRUEP
		);
	virtual void visit_aExprConst_FALSEP(
		const aExprConst_FALSEP* paExprConst_FALSEP
		);
	virtual void visit_aExprConst_NULLP(
		const aExprConst_NULLP* paExprConst_NULLP
		);
	// End generated code
public:
	struct DataType
	{
		enum Type
		{
			CIMInstanceArrayType,
			StringType,
			IntType,
			RealType,
			BoolType,
			ColumnNameType,
			NullType,
			InvalidType
		};
		DataType()
		: type(InvalidType)
		{
		}
		DataType(const Type t)
		: type(t), cia(), str(), i(), r(), b()
		{
		}
		DataType(const String& s, const Type t)
		: type(t), cia(), str(s), i(), r(), b()
		{
		}
		DataType(const CIMInstanceArray& c)
		: type(CIMInstanceArrayType), cia(c), str(), i(), r(), b()
		{
		}
		DataType(const Int64& newInt)
		: type(IntType), cia(), str(), i(newInt), r(), b()
		{
		}
		DataType(const Bool& newBool)
		: type (BoolType), cia(), str(), i(), r(), b(newBool)
		{
		}
		DataType(const Real64& newReal)
		: type (RealType), cia(), str(), i(), r(newReal), b()
		{
		}
		Type type;
		CIMInstanceArray cia;
		String str;
		Int64 i;
		Real64 r;
		Bool b;
	};
	struct CompareImpl
	{
		virtual bool operator()(const CIMValue& lhs, const CIMValue& rhs) const = 0;
		virtual const char* c_str() const = 0;
		virtual CompareImpl* reverseOrder() const = 0;
	};
	struct Equals : public CompareImpl
	{
		virtual bool operator()(const CIMValue& lhs, const CIMValue& rhs) const;
		virtual const char* c_str() const { return "="; }
		virtual CompareImpl* reverseOrder() const { return new Equals; }
	};
	
	struct NotEquals : public CompareImpl
	{
		virtual bool operator()(const CIMValue& lhs, const CIMValue& rhs) const;
		virtual const char* c_str() const { return "<>"; }
		virtual CompareImpl* reverseOrder() const { return new NotEquals; }
	};
	struct GreaterThanOrEquals : public CompareImpl
	{
		virtual bool operator()(const CIMValue& lhs, const CIMValue& rhs) const;
		virtual const char* c_str() const { return ">="; }
		virtual CompareImpl* reverseOrder() const { return new LessThanOrEquals; }
	};
	
	struct LessThanOrEquals : public CompareImpl
	{
		virtual bool operator()(const CIMValue& lhs, const CIMValue& rhs) const;
		virtual const char* c_str() const { return "<="; }
		virtual CompareImpl* reverseOrder() const { return new GreaterThanOrEquals; }
	};
	struct GreaterThan : public CompareImpl
	{
		virtual bool operator()(const CIMValue& lhs, const CIMValue& rhs) const;
		virtual const char* c_str() const { return ">"; }
		virtual CompareImpl* reverseOrder() const { return new LessThan; }
	};
	
	struct LessThan : public CompareImpl
	{
		virtual bool operator()(const CIMValue& lhs, const CIMValue& rhs) const;
		virtual const char* c_str() const { return "<"; }
		virtual CompareImpl* reverseOrder() const { return new GreaterThan; }
	};
	class Compare
	{
	public:
		enum CompareTypes
		{
			EqualsType,
			NotEqualsType,
			GreaterThanOrEqualsType,
			LessThanOrEqualsType,
			GreaterThanType,
			LessThanType
		};
		Compare( const CompareTypes type )
		{
			switch(type)
			{
				case EqualsType:
					m_ref = Reference<CompareImpl>(new Equals);
					break;
				case NotEqualsType:
					m_ref = Reference<CompareImpl>(new NotEquals);
					break;
				case GreaterThanOrEqualsType:
					m_ref = Reference<CompareImpl>(new GreaterThanOrEquals);
					break;
				case LessThanOrEqualsType:
					m_ref = Reference<CompareImpl>(new LessThanOrEquals);
					break;
				case GreaterThanType:
					m_ref = Reference<CompareImpl>(new GreaterThan);
					break;
				case LessThanType:
					m_ref = Reference<CompareImpl>(new LessThan);
					break;
			}
		}
		
		bool operator()(const CIMValue& lhs, const CIMValue& rhs) const
		{
			return m_ref->operator()(lhs, rhs);
		}
		
		const char* c_str() const
		{
			return m_ref->c_str();
		}
		
		Compare& reverseOrder()
		{
			m_ref = m_ref->reverseOrder();
			return *this;
		}
		
	private:
		Reference<CompareImpl> m_ref;
	};
public:
	CIMInstanceArray instances; // return value after processing is done.
	
private:
	DataType m_exprValue;
	Array<DataType> m_valueArray;
	Reference<CIMOMHandleIFC> m_hdl;
	String m_ns;
	String m_tableRef;
	bool m_doingSelect;
	StringArray m_propertyArray;
	CIMInstanceArray filterInstancesOnPropertyValue(const String& propName, const CIMValue& val, const Compare& compare);
	void doComparison(const DataType& lhs, const DataType& rhs, Compare compare);
	void populateInstances(const String& className);
	void populateInstances();
	bool instanceIsDerivedFrom(const CIMInstance& inst,
		const String& className);
	bool classIsDerivedFrom(const String& cls,
		const String& className);
};
bool CIMInstanceSortCriterion( const CIMInstance& lhs, const CIMInstance& rhs);

} // end namespace OpenWBEM

typedef OpenWBEM::WQLProcessor OW_WQLProcessor;

#endif
