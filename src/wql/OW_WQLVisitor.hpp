/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_WQLVISITOR_HPP_HPP_GUARD_
#define OW_WQLVISITOR_HPP_HPP_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"

class stmt;
class stmt_selectStmt_optSemicolon;
class stmt_updateStmt_optSemicolon;
class stmt_insertStmt_optSemicolon;
class stmt_deleteStmt_optSemicolon;
class optSemicolon;
class optSemicolon_empty;
class optSemicolon_SEMICOLON;
class insertStmt;
class insertRest;
class insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN;
class insertRest_DEFAULT_VALUES;
class insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN;
class deleteStmt;
class updateStmt;
class selectStmt;
class exprSeq;
class exprSeq_aExpr;
class exprSeq_exprSeq_COMMA_aExpr;
class exprSeq_exprSeq_USING_aExpr;
class optDistinct;
class optDistinct_empty;
class optDistinct_DISTINCT;
class optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN;
class optDistinct_ALL;
class sortClause;
class optSortClause;
class optSortClause_empty;
class optSortClause_sortClause;
class sortby;
class optGroupClause;
class optGroupClause_empty;
class optGroupClause_GROUP_BY_exprSeq;
class optHavingClause;
class optHavingClause_empty;
class optHavingClause_HAVING_aExpr;
class optFromClause;
class optFromClause_empty;
class optFromClause_FROM_fromList;
class tableRef;
class tableRef_relationExpr;
class tableRef_relationExpr_aliasClause;
class tableRef_joinedTable;
class tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause;
class joinedTable;
class joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN;
class joinedTable_tableRef_CROSS_JOIN_tableRef;
class joinedTable_tableRef_UNIONJOIN_tableRef;
class joinedTable_tableRef_joinType_JOIN_tableRef_joinQual;
class joinedTable_tableRef_JOIN_tableRef_joinQual;
class joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef;
class joinedTable_tableRef_NATURAL_JOIN_tableRef;
class aliasClause;
class aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN;
class aliasClause_AS_strColId;
class aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN;
class aliasClause_strColId;
class joinType;
class joinType_FULL_strOptJoinOuter;
class joinType_LEFT_strOptJoinOuter;
class joinType_RIGHT_strOptJoinOuter;
class joinType_INNERP;
class joinQual;
class joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN;
class joinQual_ON_aExpr;
class relationExpr;
class relationExpr_strRelationName;
class relationExpr_strRelationName_ASTERISK;
class relationExpr_ONLY_strRelationName;
class optWhereClause;
class optWhereClause_empty;
class optWhereClause_WHERE_aExpr;
class rowExpr;
class rowDescriptor;
class aExpr;
class aExpr_cExpr;
class aExpr_aExpr_AT_TIME_ZONE_cExpr;
class aExpr_PLUS_aExpr;
class aExpr_MINUS_aExpr;
class aExpr_BITINVERT_aExpr;
class aExpr_aExpr_PLUS_aExpr;
class aExpr_aExpr_MINUS_aExpr;
class aExpr_aExpr_ASTERISK_aExpr;
class aExpr_aExpr_SOLIDUS_aExpr;
class aExpr_aExpr_PERCENT_aExpr;
class aExpr_aExpr_BITAND_aExpr;
class aExpr_aExpr_BITOR_aExpr;
class aExpr_aExpr_BITSHIFTLEFT_aExpr;
class aExpr_aExpr_BITSHIFTRIGHT_aExpr;
class aExpr_aExpr_LESSTHAN_aExpr;
class aExpr_aExpr_LESSTHANOREQUALS_aExpr;
class aExpr_aExpr_GREATERTHAN_aExpr;
class aExpr_aExpr_GREATERTHANOREQUALS_aExpr;
class aExpr_aExpr_EQUALS_aExpr;
class aExpr_aExpr_NOTEQUALS_aExpr;
class aExpr_aExpr_AND_aExpr;
class aExpr_aExpr_OR_aExpr;
class aExpr_NOT_aExpr;
class aExpr_aExpr_CONCATENATION_aExpr;
class aExpr_aExpr_LIKE_aExpr;
class aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr;
class aExpr_aExpr_NOT_LIKE_aExpr;
class aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr;
class aExpr_aExpr_ISNULL;
class aExpr_aExpr_IS_NULLP;
class aExpr_aExpr_NOTNULL;
class aExpr_aExpr_IS_NOT_NULLP;
class aExpr_aExpr_IS_TRUEP;
class aExpr_aExpr_IS_NOT_FALSEP;
class aExpr_aExpr_IS_FALSEP;
class aExpr_aExpr_IS_NOT_TRUEP;
class aExpr_aExpr_ISA_aExpr;
class aExpr_rowExpr;
class bExpr;
class bExpr_cExpr;
class bExpr_PLUS_bExpr;
class bExpr_MINUS_bExpr;
class bExpr_BITINVERT_bExpr;
class bExpr_bExpr_PLUS_bExpr;
class bExpr_bExpr_MINUS_bExpr;
class bExpr_bExpr_ASTERISK_bExpr;
class bExpr_bExpr_SOLIDUS_bExpr;
class bExpr_bExpr_PERCENT_bExpr;
class bExpr_bExpr_BITAND_bExpr;
class bExpr_bExpr_BITOR_bExpr;
class bExpr_bExpr_BITSHIFTLEFT_bExpr;
class bExpr_bExpr_BITSHIFTRIGHT_bExpr;
class bExpr_bExpr_LESSTHAN_bExpr;
class bExpr_bExpr_LESSTHANOREQUALS_bExpr;
class bExpr_bExpr_GREATERTHAN_bExpr;
class bExpr_bExpr_GREATERTHANOREQUALS_bExpr;
class bExpr_bExpr_EQUALS_bExpr;
class bExpr_bExpr_NOTEQUALS_bExpr;
class bExpr_bExpr_CONCATENATION_bExpr;
class cExpr;
class cExpr_attr;
class cExpr_strColId_optIndirection;
class cExpr_aExprConst;
class cExpr_LEFTPAREN_aExpr_RIGHTPAREN;
class cExpr_strFuncName_LEFTPAREN_RIGHTPAREN;
class cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN;
class cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN;
class cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN;
class cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN;
class cExpr_CURRENTDATE;
class cExpr_CURRENTTIME;
class cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN;
class cExpr_CURRENTTIMESTAMP;
class cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN;
class cExpr_CURRENTUSER;
class cExpr_SESSIONUSER;
class cExpr_USER;
class cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN;
class cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN;
class cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN;
class cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN;
class cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN;
class cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN;
class optIndirection;
class optIndirection_empty;
class optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET;
class optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET;
class optExtract;
class optExtract_empty;
class optExtract_strExtractArg_FROM_aExpr;
class positionExpr;
class positionExpr_bExpr_IN_bExpr;
class positionExpr_empty;
class optSubstrExpr;
class optSubstrExpr_empty;
class optSubstrExpr_aExpr_substrFrom_substrFor;
class optSubstrExpr_aExpr_substrFor_substrFrom;
class optSubstrExpr_aExpr_substrFrom;
class optSubstrExpr_aExpr_substrFor;
class optSubstrExpr_exprSeq;
class substrFrom;
class substrFor;
class trimExpr;
class trimExpr_aExpr_FROM_exprSeq;
class trimExpr_FROM_exprSeq;
class trimExpr_exprSeq;
class attr;
class attrs;
class attrs_strAttrName;
class attrs_attrs_PERIOD_strAttrName;
class attrs_attrs_PERIOD_ASTERISK;
class targetEl;
class targetEl_aExpr_AS_strColLabel;
class targetEl_aExpr;
class targetEl_strRelationName_PERIOD_ASTERISK;
class targetEl_ASTERISK;
class updateTargetEl;
class aExprConst;
class aExprConst_ICONST;
class aExprConst_FCONST;
class aExprConst_SCONST;
class aExprConst_BITCONST;
class aExprConst_HEXCONST;
class aExprConst_TRUEP;
class aExprConst_FALSEP;
class aExprConst_NULLP;


class OW_WQLVisitor
{
	public:
		virtual ~OW_WQLVisitor() {}
		virtual void visit_stmt_selectStmt_optSemicolon(
			const stmt_selectStmt_optSemicolon* pstmt_selectStmt_optSemicolon
			) = 0;

		virtual void visit_stmt_updateStmt_optSemicolon(
			const stmt_updateStmt_optSemicolon* pstmt_updateStmt_optSemicolon
			) = 0;

		virtual void visit_stmt_insertStmt_optSemicolon(
			const stmt_insertStmt_optSemicolon* pstmt_insertStmt_optSemicolon
			) = 0;

		virtual void visit_stmt_deleteStmt_optSemicolon(
			const stmt_deleteStmt_optSemicolon* pstmt_deleteStmt_optSemicolon
			) = 0;

		virtual void visit_optSemicolon_empty(
			const optSemicolon_empty* poptSemicolon_empty
			) = 0;

		virtual void visit_optSemicolon_SEMICOLON(
			const optSemicolon_SEMICOLON* poptSemicolon_SEMICOLON
			) = 0;

		virtual void visit_insertStmt(
			const insertStmt* pinsertStmt
			) = 0;

		virtual void visit_insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
			const insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN* pinsertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN
			) = 0;

		virtual void visit_insertRest_DEFAULT_VALUES(
			const insertRest_DEFAULT_VALUES* pinsertRest_DEFAULT_VALUES
			) = 0;

		virtual void visit_insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
			const insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN* pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN
			) = 0;

		virtual void visit_deleteStmt(
			const deleteStmt* pdeleteStmt
			) = 0;

		virtual void visit_updateStmt(
			const updateStmt* pupdateStmt
			) = 0;

		virtual void visit_selectStmt(
			const selectStmt* pselectStmt
			) = 0;

		virtual void visit_exprSeq_aExpr(
			const exprSeq_aExpr* pexprSeq_aExpr
			) = 0;

		virtual void visit_exprSeq_exprSeq_COMMA_aExpr(
			const exprSeq_exprSeq_COMMA_aExpr* pexprSeq_exprSeq_COMMA_aExpr
			) = 0;

		virtual void visit_exprSeq_exprSeq_USING_aExpr(
			const exprSeq_exprSeq_USING_aExpr* pexprSeq_exprSeq_USING_aExpr
			) = 0;

		virtual void visit_optDistinct_empty(
			const optDistinct_empty* poptDistinct_empty
			) = 0;

		virtual void visit_optDistinct_DISTINCT(
			const optDistinct_DISTINCT* poptDistinct_DISTINCT
			) = 0;

		virtual void visit_optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN(
			const optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN* poptDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN
			) = 0;

		virtual void visit_optDistinct_ALL(
			const optDistinct_ALL* poptDistinct_ALL
			) = 0;

		virtual void visit_sortClause(
			const sortClause* psortClause
			) = 0;

		virtual void visit_optSortClause_empty(
			const optSortClause_empty* poptSortClause_empty
			) = 0;

		virtual void visit_optSortClause_sortClause(
			const optSortClause_sortClause* poptSortClause_sortClause
			) = 0;

		virtual void visit_sortby(
			const sortby* psortby
			) = 0;

		virtual void visit_optGroupClause_empty(
			const optGroupClause_empty* poptGroupClause_empty
			) = 0;

		virtual void visit_optGroupClause_GROUP_BY_exprSeq(
			const optGroupClause_GROUP_BY_exprSeq* poptGroupClause_GROUP_BY_exprSeq
			) = 0;

		virtual void visit_optHavingClause_empty(
			const optHavingClause_empty* poptHavingClause_empty
			) = 0;

		virtual void visit_optHavingClause_HAVING_aExpr(
			const optHavingClause_HAVING_aExpr* poptHavingClause_HAVING_aExpr
			) = 0;

		virtual void visit_optFromClause_empty(
			const optFromClause_empty* poptFromClause_empty
			) = 0;

		virtual void visit_optFromClause_FROM_fromList(
			const optFromClause_FROM_fromList* poptFromClause_FROM_fromList
			) = 0;

		virtual void visit_tableRef_relationExpr(
			const tableRef_relationExpr* ptableRef_relationExpr
			) = 0;

		virtual void visit_tableRef_relationExpr_aliasClause(
			const tableRef_relationExpr_aliasClause* ptableRef_relationExpr_aliasClause
			) = 0;

		virtual void visit_tableRef_joinedTable(
			const tableRef_joinedTable* ptableRef_joinedTable
			) = 0;

		virtual void visit_tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause(
			const tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause* ptableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause
			) = 0;

		virtual void visit_joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN(
			const joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN* pjoinedTable_LEFTPAREN_joinedTable_RIGHTPAREN
			) = 0;

		virtual void visit_joinedTable_tableRef_CROSS_JOIN_tableRef(
			const joinedTable_tableRef_CROSS_JOIN_tableRef* pjoinedTable_tableRef_CROSS_JOIN_tableRef
			) = 0;

		virtual void visit_joinedTable_tableRef_UNIONJOIN_tableRef(
			const joinedTable_tableRef_UNIONJOIN_tableRef* pjoinedTable_tableRef_UNIONJOIN_tableRef
			) = 0;

		virtual void visit_joinedTable_tableRef_joinType_JOIN_tableRef_joinQual(
			const joinedTable_tableRef_joinType_JOIN_tableRef_joinQual* pjoinedTable_tableRef_joinType_JOIN_tableRef_joinQual
			) = 0;

		virtual void visit_joinedTable_tableRef_JOIN_tableRef_joinQual(
			const joinedTable_tableRef_JOIN_tableRef_joinQual* pjoinedTable_tableRef_JOIN_tableRef_joinQual
			) = 0;

		virtual void visit_joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef(
			const joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef* pjoinedTable_tableRef_NATURAL_joinType_JOIN_tableRef
			) = 0;

		virtual void visit_joinedTable_tableRef_NATURAL_JOIN_tableRef(
			const joinedTable_tableRef_NATURAL_JOIN_tableRef* pjoinedTable_tableRef_NATURAL_JOIN_tableRef
			) = 0;

		virtual void visit_aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN(
			const aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN* paliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN
			) = 0;

		virtual void visit_aliasClause_AS_strColId(
			const aliasClause_AS_strColId* paliasClause_AS_strColId
			) = 0;

		virtual void visit_aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN(
			const aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN* paliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN
			) = 0;

		virtual void visit_aliasClause_strColId(
			const aliasClause_strColId* paliasClause_strColId
			) = 0;

		virtual void visit_joinType_FULL_strOptJoinOuter(
			const joinType_FULL_strOptJoinOuter* pjoinType_FULL_strOptJoinOuter
			) = 0;

		virtual void visit_joinType_LEFT_strOptJoinOuter(
			const joinType_LEFT_strOptJoinOuter* pjoinType_LEFT_strOptJoinOuter
			) = 0;

		virtual void visit_joinType_RIGHT_strOptJoinOuter(
			const joinType_RIGHT_strOptJoinOuter* pjoinType_RIGHT_strOptJoinOuter
			) = 0;

		virtual void visit_joinType_INNERP(
			const joinType_INNERP* pjoinType_INNERP
			) = 0;

		virtual void visit_joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN(
			const joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN* pjoinQual_USING_LEFTPAREN_nameList_RIGHTPAREN
			) = 0;

		virtual void visit_joinQual_ON_aExpr(
			const joinQual_ON_aExpr* pjoinQual_ON_aExpr
			) = 0;

		virtual void visit_relationExpr_strRelationName(
			const relationExpr_strRelationName* prelationExpr_strRelationName
			) = 0;

		virtual void visit_relationExpr_strRelationName_ASTERISK(
			const relationExpr_strRelationName_ASTERISK* prelationExpr_strRelationName_ASTERISK
			) = 0;

		virtual void visit_relationExpr_ONLY_strRelationName(
			const relationExpr_ONLY_strRelationName* prelationExpr_ONLY_strRelationName
			) = 0;

		virtual void visit_optWhereClause_empty(
			const optWhereClause_empty* poptWhereClause_empty
			) = 0;

		virtual void visit_optWhereClause_WHERE_aExpr(
			const optWhereClause_WHERE_aExpr* poptWhereClause_WHERE_aExpr
			) = 0;

		virtual void visit_rowExpr(
			const rowExpr* prowExpr
			) = 0;

		virtual void visit_rowDescriptor(
			const rowDescriptor* prowDescriptor
			) = 0;

		virtual void visit_aExpr_cExpr(
			const aExpr_cExpr* paExpr_cExpr
			) = 0;

		virtual void visit_aExpr_aExpr_AT_TIME_ZONE_cExpr(
			const aExpr_aExpr_AT_TIME_ZONE_cExpr* paExpr_aExpr_AT_TIME_ZONE_cExpr
			) = 0;

		virtual void visit_aExpr_PLUS_aExpr(
			const aExpr_PLUS_aExpr* paExpr_PLUS_aExpr
			) = 0;

		virtual void visit_aExpr_MINUS_aExpr(
			const aExpr_MINUS_aExpr* paExpr_MINUS_aExpr
			) = 0;

		virtual void visit_aExpr_BITINVERT_aExpr(
			const aExpr_BITINVERT_aExpr* paExpr_BITINVERT_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_PLUS_aExpr(
			const aExpr_aExpr_PLUS_aExpr* paExpr_aExpr_PLUS_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_MINUS_aExpr(
			const aExpr_aExpr_MINUS_aExpr* paExpr_aExpr_MINUS_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_ASTERISK_aExpr(
			const aExpr_aExpr_ASTERISK_aExpr* paExpr_aExpr_ASTERISK_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_SOLIDUS_aExpr(
			const aExpr_aExpr_SOLIDUS_aExpr* paExpr_aExpr_SOLIDUS_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_PERCENT_aExpr(
			const aExpr_aExpr_PERCENT_aExpr* paExpr_aExpr_PERCENT_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_BITAND_aExpr(
			const aExpr_aExpr_BITAND_aExpr* paExpr_aExpr_BITAND_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_BITOR_aExpr(
			const aExpr_aExpr_BITOR_aExpr* paExpr_aExpr_BITOR_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_BITSHIFTLEFT_aExpr(
			const aExpr_aExpr_BITSHIFTLEFT_aExpr* paExpr_aExpr_BITSHIFTLEFT_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_BITSHIFTRIGHT_aExpr(
			const aExpr_aExpr_BITSHIFTRIGHT_aExpr* paExpr_aExpr_BITSHIFTRIGHT_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_LESSTHAN_aExpr(
			const aExpr_aExpr_LESSTHAN_aExpr* paExpr_aExpr_LESSTHAN_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_LESSTHANOREQUALS_aExpr(
			const aExpr_aExpr_LESSTHANOREQUALS_aExpr* paExpr_aExpr_LESSTHANOREQUALS_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_GREATERTHAN_aExpr(
			const aExpr_aExpr_GREATERTHAN_aExpr* paExpr_aExpr_GREATERTHAN_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_GREATERTHANOREQUALS_aExpr(
			const aExpr_aExpr_GREATERTHANOREQUALS_aExpr* paExpr_aExpr_GREATERTHANOREQUALS_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_EQUALS_aExpr(
			const aExpr_aExpr_EQUALS_aExpr* paExpr_aExpr_EQUALS_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_NOTEQUALS_aExpr(
			const aExpr_aExpr_NOTEQUALS_aExpr* paExpr_aExpr_NOTEQUALS_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_AND_aExpr(
			const aExpr_aExpr_AND_aExpr* paExpr_aExpr_AND_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_OR_aExpr(
			const aExpr_aExpr_OR_aExpr* paExpr_aExpr_OR_aExpr
			) = 0;

		virtual void visit_aExpr_NOT_aExpr(
			const aExpr_NOT_aExpr* paExpr_NOT_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_CONCATENATION_aExpr(
			const aExpr_aExpr_CONCATENATION_aExpr* paExpr_aExpr_CONCATENATION_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_LIKE_aExpr(
			const aExpr_aExpr_LIKE_aExpr* paExpr_aExpr_LIKE_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr(
			const aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr* paExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_NOT_LIKE_aExpr(
			const aExpr_aExpr_NOT_LIKE_aExpr* paExpr_aExpr_NOT_LIKE_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr(
			const aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr* paExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr
			) = 0;

		virtual void visit_aExpr_aExpr_ISNULL(
			const aExpr_aExpr_ISNULL* paExpr_aExpr_ISNULL
			) = 0;

		virtual void visit_aExpr_aExpr_IS_NULLP(
			const aExpr_aExpr_IS_NULLP* paExpr_aExpr_IS_NULLP
			) = 0;

		virtual void visit_aExpr_aExpr_NOTNULL(
			const aExpr_aExpr_NOTNULL* paExpr_aExpr_NOTNULL
			) = 0;

		virtual void visit_aExpr_aExpr_IS_NOT_NULLP(
			const aExpr_aExpr_IS_NOT_NULLP* paExpr_aExpr_IS_NOT_NULLP
			) = 0;

		virtual void visit_aExpr_aExpr_IS_TRUEP(
			const aExpr_aExpr_IS_TRUEP* paExpr_aExpr_IS_TRUEP
			) = 0;

		virtual void visit_aExpr_aExpr_IS_NOT_FALSEP(
			const aExpr_aExpr_IS_NOT_FALSEP* paExpr_aExpr_IS_NOT_FALSEP
			) = 0;

		virtual void visit_aExpr_aExpr_IS_FALSEP(
			const aExpr_aExpr_IS_FALSEP* paExpr_aExpr_IS_FALSEP
			) = 0;

		virtual void visit_aExpr_aExpr_IS_NOT_TRUEP(
			const aExpr_aExpr_IS_NOT_TRUEP* paExpr_aExpr_IS_NOT_TRUEP
			) = 0;

		virtual void visit_aExpr_aExpr_ISA_aExpr(
			const aExpr_aExpr_ISA_aExpr* paExpr_aExpr_ISA_aExpr
			) = 0;

		virtual void visit_aExpr_rowExpr(
			const aExpr_rowExpr* paExpr_rowExpr
			) = 0;

		virtual void visit_bExpr_cExpr(
			const bExpr_cExpr* pbExpr_cExpr
			) = 0;

		virtual void visit_bExpr_PLUS_bExpr(
			const bExpr_PLUS_bExpr* pbExpr_PLUS_bExpr
			) = 0;

		virtual void visit_bExpr_MINUS_bExpr(
			const bExpr_MINUS_bExpr* pbExpr_MINUS_bExpr
			) = 0;

		virtual void visit_bExpr_BITINVERT_bExpr(
			const bExpr_BITINVERT_bExpr* pbExpr_BITINVERT_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_PLUS_bExpr(
			const bExpr_bExpr_PLUS_bExpr* pbExpr_bExpr_PLUS_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_MINUS_bExpr(
			const bExpr_bExpr_MINUS_bExpr* pbExpr_bExpr_MINUS_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_ASTERISK_bExpr(
			const bExpr_bExpr_ASTERISK_bExpr* pbExpr_bExpr_ASTERISK_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_SOLIDUS_bExpr(
			const bExpr_bExpr_SOLIDUS_bExpr* pbExpr_bExpr_SOLIDUS_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_PERCENT_bExpr(
			const bExpr_bExpr_PERCENT_bExpr* pbExpr_bExpr_PERCENT_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_BITAND_bExpr(
			const bExpr_bExpr_BITAND_bExpr* pbExpr_bExpr_BITAND_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_BITOR_bExpr(
			const bExpr_bExpr_BITOR_bExpr* pbExpr_bExpr_BITOR_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_BITSHIFTLEFT_bExpr(
			const bExpr_bExpr_BITSHIFTLEFT_bExpr* pbExpr_bExpr_BITSHIFTLEFT_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_BITSHIFTRIGHT_bExpr(
			const bExpr_bExpr_BITSHIFTRIGHT_bExpr* pbExpr_bExpr_BITSHIFTRIGHT_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_LESSTHAN_bExpr(
			const bExpr_bExpr_LESSTHAN_bExpr* pbExpr_bExpr_LESSTHAN_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_LESSTHANOREQUALS_bExpr(
			const bExpr_bExpr_LESSTHANOREQUALS_bExpr* pbExpr_bExpr_LESSTHANOREQUALS_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_GREATERTHAN_bExpr(
			const bExpr_bExpr_GREATERTHAN_bExpr* pbExpr_bExpr_GREATERTHAN_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_GREATERTHANOREQUALS_bExpr(
			const bExpr_bExpr_GREATERTHANOREQUALS_bExpr* pbExpr_bExpr_GREATERTHANOREQUALS_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_EQUALS_bExpr(
			const bExpr_bExpr_EQUALS_bExpr* pbExpr_bExpr_EQUALS_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_NOTEQUALS_bExpr(
			const bExpr_bExpr_NOTEQUALS_bExpr* pbExpr_bExpr_NOTEQUALS_bExpr
			) = 0;

		virtual void visit_bExpr_bExpr_CONCATENATION_bExpr(
			const bExpr_bExpr_CONCATENATION_bExpr* pbExpr_bExpr_CONCATENATION_bExpr
			) = 0;

		virtual void visit_cExpr_attr(
			const cExpr_attr* pcExpr_attr
			) = 0;

		virtual void visit_cExpr_strColId_optIndirection(
			const cExpr_strColId_optIndirection* pcExpr_strColId_optIndirection
			) = 0;

		virtual void visit_cExpr_aExprConst(
			const cExpr_aExprConst* pcExpr_aExprConst
			) = 0;

		virtual void visit_cExpr_LEFTPAREN_aExpr_RIGHTPAREN(
			const cExpr_LEFTPAREN_aExpr_RIGHTPAREN* pcExpr_LEFTPAREN_aExpr_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_strFuncName_LEFTPAREN_RIGHTPAREN(
			const cExpr_strFuncName_LEFTPAREN_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN(
			const cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN(
			const cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN(
			const cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN(
			const cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_CURRENTDATE(
			const cExpr_CURRENTDATE* pcExpr_CURRENTDATE
			) = 0;

		virtual void visit_cExpr_CURRENTTIME(
			const cExpr_CURRENTTIME* pcExpr_CURRENTTIME
			) = 0;

		virtual void visit_cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN(
			const cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN* pcExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_CURRENTTIMESTAMP(
			const cExpr_CURRENTTIMESTAMP* pcExpr_CURRENTTIMESTAMP
			) = 0;

		virtual void visit_cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN(
			const cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN* pcExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_CURRENTUSER(
			const cExpr_CURRENTUSER* pcExpr_CURRENTUSER
			) = 0;

		virtual void visit_cExpr_SESSIONUSER(
			const cExpr_SESSIONUSER* pcExpr_SESSIONUSER
			) = 0;

		virtual void visit_cExpr_USER(
			const cExpr_USER* pcExpr_USER
			) = 0;

		virtual void visit_cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN(
			const cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN* pcExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN(
			const cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN* pcExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN(
			const cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN* pcExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN(
			const cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN(
			const cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN
			) = 0;

		virtual void visit_cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN(
			const cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN
			) = 0;

		virtual void visit_optIndirection_empty(
			const optIndirection_empty* poptIndirection_empty
			) = 0;

		virtual void visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET(
			const optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET* poptIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET
			) = 0;

		virtual void visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET(
			const optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET* poptIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET
			) = 0;

		virtual void visit_optExtract_empty(
			const optExtract_empty* poptExtract_empty
			) = 0;

		virtual void visit_optExtract_strExtractArg_FROM_aExpr(
			const optExtract_strExtractArg_FROM_aExpr* poptExtract_strExtractArg_FROM_aExpr
			) = 0;

		virtual void visit_positionExpr_bExpr_IN_bExpr(
			const positionExpr_bExpr_IN_bExpr* ppositionExpr_bExpr_IN_bExpr
			) = 0;

		virtual void visit_positionExpr_empty(
			const positionExpr_empty* ppositionExpr_empty
			) = 0;

		virtual void visit_optSubstrExpr_empty(
			const optSubstrExpr_empty* poptSubstrExpr_empty
			) = 0;

		virtual void visit_optSubstrExpr_aExpr_substrFrom_substrFor(
			const optSubstrExpr_aExpr_substrFrom_substrFor* poptSubstrExpr_aExpr_substrFrom_substrFor
			) = 0;

		virtual void visit_optSubstrExpr_aExpr_substrFor_substrFrom(
			const optSubstrExpr_aExpr_substrFor_substrFrom* poptSubstrExpr_aExpr_substrFor_substrFrom
			) = 0;

		virtual void visit_optSubstrExpr_aExpr_substrFrom(
			const optSubstrExpr_aExpr_substrFrom* poptSubstrExpr_aExpr_substrFrom
			) = 0;

		virtual void visit_optSubstrExpr_aExpr_substrFor(
			const optSubstrExpr_aExpr_substrFor* poptSubstrExpr_aExpr_substrFor
			) = 0;

		virtual void visit_optSubstrExpr_exprSeq(
			const optSubstrExpr_exprSeq* poptSubstrExpr_exprSeq
			) = 0;

		virtual void visit_substrFrom(
			const substrFrom* psubstrFrom
			) = 0;

		virtual void visit_substrFor(
			const substrFor* psubstrFor
			) = 0;

		virtual void visit_trimExpr_aExpr_FROM_exprSeq(
			const trimExpr_aExpr_FROM_exprSeq* ptrimExpr_aExpr_FROM_exprSeq
			) = 0;

		virtual void visit_trimExpr_FROM_exprSeq(
			const trimExpr_FROM_exprSeq* ptrimExpr_FROM_exprSeq
			) = 0;

		virtual void visit_trimExpr_exprSeq(
			const trimExpr_exprSeq* ptrimExpr_exprSeq
			) = 0;

		virtual void visit_attr(
			const attr* pattr
			) = 0;

		virtual void visit_attrs_strAttrName(
			const attrs_strAttrName* pattrs_strAttrName
			) = 0;

		virtual void visit_attrs_attrs_PERIOD_strAttrName(
			const attrs_attrs_PERIOD_strAttrName* pattrs_attrs_PERIOD_strAttrName
			) = 0;

		virtual void visit_attrs_attrs_PERIOD_ASTERISK(
			const attrs_attrs_PERIOD_ASTERISK* pattrs_attrs_PERIOD_ASTERISK
			) = 0;

		virtual void visit_targetEl_aExpr_AS_strColLabel(
			const targetEl_aExpr_AS_strColLabel* ptargetEl_aExpr_AS_strColLabel
			) = 0;

		virtual void visit_targetEl_aExpr(
			const targetEl_aExpr* ptargetEl_aExpr
			) = 0;

		virtual void visit_targetEl_strRelationName_PERIOD_ASTERISK(
			const targetEl_strRelationName_PERIOD_ASTERISK* ptargetEl_strRelationName_PERIOD_ASTERISK
			) = 0;

		virtual void visit_targetEl_ASTERISK(
			const targetEl_ASTERISK* ptargetEl_ASTERISK
			) = 0;

		virtual void visit_updateTargetEl(
			const updateTargetEl* pupdateTargetEl
			) = 0;

		virtual void visit_aExprConst_ICONST(
			const aExprConst_ICONST* paExprConst_ICONST
			) = 0;

		virtual void visit_aExprConst_FCONST(
			const aExprConst_FCONST* paExprConst_FCONST
			) = 0;

		virtual void visit_aExprConst_SCONST(
			const aExprConst_SCONST* paExprConst_SCONST
			) = 0;

		virtual void visit_aExprConst_BITCONST(
			const aExprConst_BITCONST* paExprConst_BITCONST
			) = 0;

		virtual void visit_aExprConst_HEXCONST(
			const aExprConst_HEXCONST* paExprConst_HEXCONST
			) = 0;

		virtual void visit_aExprConst_TRUEP(
			const aExprConst_TRUEP* paExprConst_TRUEP
			) = 0;

		virtual void visit_aExprConst_FALSEP(
			const aExprConst_FALSEP* paExprConst_FALSEP
			) = 0;

		virtual void visit_aExprConst_NULLP(
			const aExprConst_NULLP* paExprConst_NULLP
			) = 0;

};

#endif
