%{
#define YYDEBUG 1
/*-------------------------------------------------------------------------
 *
 * OW_WQLImpl.yy
 *	  OpenWBEM WQL YACC rules/actions
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * NOTES
 *	  CAPITALS are used to represent terminal symbols.
 *	  non-capitals are used to represent non-terminals.
 *-------------------------------------------------------------------------*/
/*******************************************************************************
* Portions Copyright (C) 2001 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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


#include <stdlib.h>
#include "OW_WQLAst.hpp"
#include "OW_WQLImpl.hpp"
#include "OW_WQLParseError.hpp"

void owwqlerror(OpenWBEM::WQL::ParseError* p_err, const char* s);

using namespace OpenWBEM;
using namespace OpenWBEM::WQL;
using namespace blocxx;

%}

%defines
%error-verbose
%parse-param {ParseError * p_err}
%lex-param {ParseError* p_err}

%{
#define YYLEX_PARAM p_err
int owwqllex(OpenWBEM::WQL::ParseError* p_err);
%}

%token ALL
%token AND
%token AS
%token ASC
%token ASTERISK
%token AT
%token BITAND
%token BITCONST
%token BITINVERT
%token BITOR
%token BITSHIFTLEFT
%token BITSHIFTRIGHT
%token BY
%token COLON
%token COMMA
%token CONCATENATION
%token CROSS
%token CURRENTDATE
%token CURRENTTIME
%token CURRENTTIMESTAMP
%token CURRENTUSER
%token DAYP
%token DEFAULT
%token DELETE
%token DESC
%token DISTINCT
%token EQUALS
%token ESCAPE
%token EXTRACT
%token FALSEP
%token FCONST
%token FOR
%token FROM
%token FULL
%token GREATERTHAN
%token GREATERTHANOREQUALS
%token GROUP
%token HAVING
%token HEXCONST
%token HOURP
%token ICONST
%token IDENT
%token IN
%token INNERP
%token INSERT
%token INTERVAL
%token INTO
%token IS
%token ISA
%token ISNULL
%token JOIN
%token LEADING
%token LEFT
%token LEFTBRACKET
%token LEFTPAREN
%token LESSTHAN
%token LESSTHANOREQUALS
%token LIKE
%token MINUS
%token MINUTEP
%token MONTHP
%token NATIONAL
%token NATURAL
%token NOT
%token NOTEQUALS
%token NOTNULL
%token NULLP
%token ON
%token ONLY
%token OR
%token ORDER
%token OUTERP
%token PERCENT
%token PERIOD
%token PLUS
%token POSITION
%token RIGHT
%token RIGHTBRACKET
%token RIGHTPAREN
%token SCONST
%token SECONDP
%token SELECT
%token SEMICOLON
%token SESSIONUSER
%token SET
%token SOLIDUS
%token SUBSTRING
%token TIME
%token TIMESTAMP
%token TIMEZONEHOUR
%token TIMEZONEMINUTE
%token TRAILING
%token TRIM
%token TRUEP
%token UNION
%token UNIONJOIN
%token UPDATE
%token USER
%token USING
%token VALUES
%token WHERE
%token YEARP
%token ZONE

%token SCANNER_ERROR

/* precedence: lowest to highest */
%left		UNION
%left		JOIN UNIONJOIN CROSS LEFT FULL RIGHT INNERP NATURAL
%left		OR
%left		AND
%right		NOT
%right		EQUALS NOTEQUALS
%nonassoc	LESSTHAN LESSTHANOREQUALS GREATERTHAN GREATERTHANOREQUALS
%nonassoc	LIKE
%nonassoc	ESCAPE
%nonassoc	IN
%nonassoc	NOTNULL
%nonassoc	ISNULL
%nonassoc	IS NULLP TRUEP FALSEP ISA	/* sets precedence for IS NULL, etc */
%left		CONCATENATION
%left		BITOR
%left		BITAND
%left		BITSHIFTLEFT BITSHIFTRIGHT
%left		PLUS MINUS
%left		ASTERISK SOLIDUS PERCENT
/* Unary Operators */
%left		AT ZONE			/* sets precedence for AT TIME ZONE */
%right	UMINUS
%left		PERIOD
%left		LEFTBRACKET RIGHTBRACKET
%left		LEFTPAREN RIGHTPAREN



%union {
	OpenWBEM::WQL::stmt* pstmt;
	OpenWBEM::WQL::optSemicolon* poptSemicolon;
	OpenWBEM::WQL::insertStmt* pinsertStmt;
	blocxx::List<OpenWBEM::WQL::targetEl*>* ptargetList;
	blocxx::List<blocxx::String*>* pcolumnList;
	OpenWBEM::WQL::insertRest* pinsertRest;
	OpenWBEM::WQL::deleteStmt* pdeleteStmt;
	blocxx::List<OpenWBEM::WQL::updateTargetEl*>* pupdateTargetList;
	OpenWBEM::WQL::updateStmt* pupdateStmt;
	OpenWBEM::WQL::selectStmt* pselectStmt;
	OpenWBEM::WQL::exprSeq* pexprSeq;
	OpenWBEM::WQL::optDistinct* poptDistinct;
	blocxx::List<OpenWBEM::WQL::sortby*>* psortbyList;
	OpenWBEM::WQL::sortClause* psortClause;
	OpenWBEM::WQL::optSortClause* poptSortClause;
	OpenWBEM::WQL::sortby* psortby;
	blocxx::List<blocxx::String*>* pnameList;
	OpenWBEM::WQL::optGroupClause* poptGroupClause;
	OpenWBEM::WQL::optHavingClause* poptHavingClause;
	blocxx::List<OpenWBEM::WQL::tableRef*>* pfromList;
	OpenWBEM::WQL::optFromClause* poptFromClause;
	OpenWBEM::WQL::tableRef* ptableRef;
	OpenWBEM::WQL::joinedTable* pjoinedTable;
	OpenWBEM::WQL::aliasClause* paliasClause;
	OpenWBEM::WQL::joinType* pjoinType;
	OpenWBEM::WQL::joinQual* pjoinQual;
	OpenWBEM::WQL::relationExpr* prelationExpr;
	OpenWBEM::WQL::optWhereClause* poptWhereClause;
	OpenWBEM::WQL::rowExpr* prowExpr;
	blocxx::List<OpenWBEM::WQL::aExpr*>* prowList;
	OpenWBEM::WQL::rowDescriptor* prowDescriptor;
	OpenWBEM::WQL::aExpr* paExpr;
	OpenWBEM::WQL::bExpr* pbExpr;
	OpenWBEM::WQL::cExpr* pcExpr;
	OpenWBEM::WQL::optIndirection* poptIndirection;
	OpenWBEM::WQL::optExtract* poptExtract;
	OpenWBEM::WQL::positionExpr* ppositionExpr;
	OpenWBEM::WQL::optSubstrExpr* poptSubstrExpr;
	OpenWBEM::WQL::substrFrom* psubstrFrom;
	OpenWBEM::WQL::substrFor* psubstrFor;
	OpenWBEM::WQL::trimExpr* ptrimExpr;
	OpenWBEM::WQL::attr* pattr;
	OpenWBEM::WQL::attrs* pattrs;
	OpenWBEM::WQL::targetEl* ptargetEl;
	OpenWBEM::WQL::updateTargetEl* pupdateTargetEl;
	OpenWBEM::WQL::aExprConst* paExprConst;
	blocxx::String* pstring;
}

%type <pstmt> stmt
%type <poptSemicolon> optSemicolon
%destructor { delete $$; } optSemicolon
%type <pinsertStmt> insertStmt
%destructor { delete $$; } insertStmt
%type <ptargetList> targetList
%destructor { delete $$; } targetList
%type <pcolumnList> columnList
%destructor { delete $$; } columnList
%type <pinsertRest> insertRest
%destructor { delete $$; } insertRest
%type <pdeleteStmt> deleteStmt
%destructor { delete $$; } deleteStmt
%type <pupdateTargetList> updateTargetList
%destructor { delete $$; } updateTargetList
%type <pupdateStmt> updateStmt
%destructor { delete $$; } updateStmt
%type <pselectStmt> selectStmt
%destructor { delete $$; } selectStmt
%type <pexprSeq> exprSeq
%destructor { delete $$; } exprSeq
%type <poptDistinct> optDistinct
%destructor { delete $$; } optDistinct
%type <psortbyList> sortbyList
%destructor { delete $$; } sortbyList
%type <psortClause> sortClause
%destructor { delete $$; } sortClause
%type <poptSortClause> optSortClause
%destructor { delete $$; } optSortClause
%type <psortby> sortby
%destructor { delete $$; } sortby
%type <pnameList> nameList
%destructor { delete $$; } nameList
%type <poptGroupClause> optGroupClause
%destructor { delete $$; } optGroupClause
%type <poptHavingClause> optHavingClause
%destructor { delete $$; } optHavingClause
%type <pfromList> fromList
%destructor { delete $$; } fromList
%type <poptFromClause> optFromClause
%destructor { delete $$; } optFromClause
%type <ptableRef> tableRef
%destructor { delete $$; } tableRef
%type <pjoinedTable> joinedTable
%destructor { delete $$; } joinedTable
%type <paliasClause> aliasClause
%destructor { delete $$; } aliasClause
%type <pjoinType> joinType
%destructor { delete $$; } joinType
%type <pjoinQual> joinQual
%destructor { delete $$; } joinQual
%type <prelationExpr> relationExpr
%destructor { delete $$; } relationExpr
%type <poptWhereClause> optWhereClause
%destructor { delete $$; } optWhereClause
%type <prowExpr> rowExpr
%destructor { delete $$; } rowExpr
%type <prowList> rowList
%destructor { delete $$; } rowList
%type <prowDescriptor> rowDescriptor
%destructor { delete $$; } rowDescriptor
%type <paExpr> aExpr
%destructor { delete $$; } aExpr
%type <pbExpr> bExpr
%destructor { delete $$; } bExpr
%type <pcExpr> cExpr
%destructor { delete $$; } cExpr
%type <poptIndirection> optIndirection
%destructor { delete $$; } optIndirection
%type <poptExtract> optExtract
%destructor { delete $$; } optExtract
%type <ppositionExpr> positionExpr
%destructor { delete $$; } positionExpr
%type <poptSubstrExpr> optSubstrExpr
%destructor { delete $$; } optSubstrExpr
%type <psubstrFrom> substrFrom
%destructor { delete $$; } substrFrom
%type <psubstrFor> substrFor
%destructor { delete $$; } substrFor
%type <ptrimExpr> trimExpr
%destructor { delete $$; } trimExpr
%type <pattr> attr
%destructor { delete $$; } attr
%type <pattrs> attrs
%destructor { delete $$; } attrs
%type <ptargetEl> targetEl
%destructor { delete $$; } targetEl
%type <pupdateTargetEl> updateTargetEl
%destructor { delete $$; } updateTargetEl
%type <paExprConst> aExprConst
%destructor { delete $$; } aExprConst
%type <pstring> strColumnElem strOptOrderSpecification strOptJoinOuter strDatetime strAllOp strExtractArg strRelationName strName strAttrName strFuncName strColId strColLabel ALL AND AS ASC ASTERISK AT BITAND BITCONST BITINVERT BITOR BITSHIFTLEFT BITSHIFTRIGHT BY COLON COMMA CONCATENATION CROSS CURRENTDATE CURRENTTIME CURRENTTIMESTAMP CURRENTUSER DAYP DEFAULT DELETE DESC DISTINCT EQUALS ESCAPE EXTRACT FALSEP FCONST FOR FROM FULL GREATERTHAN GREATERTHANOREQUALS GROUP HAVING HEXCONST HOURP ICONST IDENT IN INNERP INSERT INTERVAL INTO IS ISA ISNULL JOIN LEADING LEFT LEFTBRACKET LEFTPAREN LESSTHAN LESSTHANOREQUALS LIKE MINUS MINUTEP MONTHP NATIONAL NATURAL NOT NOTEQUALS NOTNULL NULLP ON ONLY OR ORDER OUTERP PERCENT PERIOD PLUS POSITION RIGHT RIGHTBRACKET RIGHTPAREN SCONST SECONDP SELECT SEMICOLON SESSIONUSER SET SOLIDUS SUBSTRING TIME TIMESTAMP TIMEZONEHOUR TIMEZONEMINUTE TRAILING TRIM TRUEP UNION UNIONJOIN UPDATE USER USING VALUES WHERE YEARP ZONE
%destructor { delete $$; } strColumnElem strOptOrderSpecification strOptJoinOuter strDatetime strAllOp strExtractArg strRelationName strName strAttrName strFuncName strColId strColLabel ALL AND AS ASC ASTERISK AT BITAND BITCONST BITINVERT BITOR BITSHIFTLEFT BITSHIFTRIGHT BY COLON COMMA CONCATENATION CROSS CURRENTDATE CURRENTTIME CURRENTTIMESTAMP CURRENTUSER DAYP DEFAULT DELETE DESC DISTINCT EQUALS ESCAPE EXTRACT FALSEP FCONST FOR FROM FULL GREATERTHAN GREATERTHANOREQUALS GROUP HAVING HEXCONST HOURP ICONST IDENT IN INNERP INSERT INTERVAL INTO IS ISA ISNULL JOIN LEADING LEFT LEFTBRACKET LEFTPAREN LESSTHAN LESSTHANOREQUALS LIKE MINUS MINUTEP MONTHP NATIONAL NATURAL NOT NOTEQUALS NOTNULL NULLP ON ONLY OR ORDER OUTERP PERCENT PERIOD PLUS POSITION RIGHT RIGHTBRACKET RIGHTPAREN SCONST SECONDP SELECT SEMICOLON SESSIONUSER SET SOLIDUS SUBSTRING TIME TIMESTAMP TIMEZONEHOUR TIMEZONEMINUTE TRAILING TRIM TRUEP UNION UNIONJOIN UPDATE USER USING VALUES WHERE YEARP ZONE


%%

stmt:
	selectStmt optSemicolon
	  { $$ = WQLImpl::setStatement(new stmt_selectStmt_optSemicolon($1, $2)); }
	| updateStmt optSemicolon
	  { $$ = WQLImpl::setStatement(new stmt_updateStmt_optSemicolon($1, $2)); }
	| insertStmt optSemicolon
	  { $$ = WQLImpl::setStatement(new stmt_insertStmt_optSemicolon($1, $2)); }
	| deleteStmt optSemicolon
	  { $$ = WQLImpl::setStatement(new stmt_deleteStmt_optSemicolon($1, $2)); }
	;


optSemicolon:
	/* EMPTY */
	  { $$ = 0; }
	| SEMICOLON
	  { $$ = new optSemicolon_SEMICOLON($1); }
	;


insertStmt:
	INSERT INTO strRelationName insertRest
	  { $$ = new insertStmt($1, $2, $3, $4); }
	;


targetList:
	targetEl
	  { $$ = new List<targetEl*>(1, $1); }
	| targetList COMMA targetEl
	  { $1->push_back($3); delete $2; $$ = $1; }
	;


columnList:
	strColumnElem
	  { $$ = new List<String*>(1, $1); }
	| columnList COMMA strColumnElem
	  { $1->push_back($3); delete $2; $$ = $1; }
	;


insertRest:
	VALUES LEFTPAREN targetList RIGHTPAREN
	  { $$ = new insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN($1, $2, $3, $4); }
	| DEFAULT VALUES
	  { $$ = new insertRest_DEFAULT_VALUES($1, $2); }
	| LEFTPAREN columnList RIGHTPAREN VALUES LEFTPAREN targetList RIGHTPAREN
	  { $$ = new insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN($1, $2, $3, $4, $5, $6, $7); }
	;


strColumnElem:
	strColId
	  { $$ = $1; }
	;


deleteStmt:
	DELETE FROM strRelationName optWhereClause
	  { $$ = new deleteStmt($1, $2, $3, $4); }
	;


updateTargetList:
	updateTargetEl
	  { $$ = new List<updateTargetEl*>(1, $1); }
	| updateTargetList COMMA updateTargetEl
	  { $1->push_back($3); delete $2; $$ = $1; }
	;


updateStmt:
	UPDATE strRelationName SET updateTargetList optWhereClause
	  { $$ = new updateStmt($1, $2, $3, $4, $5); }
	;


selectStmt:
	SELECT optDistinct targetList optFromClause optWhereClause optGroupClause optHavingClause optSortClause
	  { $$ = new selectStmt($1, $2, $3, $4, $5, $6, $7, $8); }
	;


exprSeq:
	aExpr
	  { $$ = new exprSeq_aExpr($1); }
	| exprSeq COMMA aExpr
	  { $$ = new exprSeq_exprSeq_COMMA_aExpr($1, $2, $3); }
	| exprSeq USING aExpr
	  { $$ = new exprSeq_exprSeq_USING_aExpr($1, $2, $3); }
	;


optDistinct:
	/* EMPTY */
	  { $$ = 0; }
	| DISTINCT
	  { $$ = new optDistinct_DISTINCT($1); }
	| DISTINCT ON LEFTPAREN exprSeq RIGHTPAREN
	  { $$ = new optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN($1, $2, $3, $4, $5); }
	| ALL
	  { $$ = new optDistinct_ALL($1); }
	;


sortbyList:
	sortby
	  { $$ = new List<sortby*>(1, $1); }
	| sortbyList COMMA sortby
	  { $1->push_back($3); delete $2; $$ = $1; }
	;


sortClause:
	ORDER BY sortbyList
	  { $$ = new sortClause($1, $2, $3); }
	;


optSortClause:
	/* EMPTY */
	  { $$ = 0; }
	| sortClause
	  { $$ = new optSortClause_sortClause($1); }
	;


sortby:
	aExpr strOptOrderSpecification
	  { $$ = new sortby($1, $2); }
	;


strOptOrderSpecification:
	/* EMPTY */
	  { $$ = 0; }
	| ASC
	  { $$ = $1; }
	| DESC
	  { $$ = $1; }
	;


nameList:
	strName
	  { $$ = new List<String*>(1, $1); }
	| nameList COMMA strName
	  { $1->push_back($3); delete $2; $$ = $1; }
	;


optGroupClause:
	/* EMPTY */
	  { $$ = 0; }
	| GROUP BY exprSeq
	  { $$ = new optGroupClause_GROUP_BY_exprSeq($1, $2, $3); }
	;


optHavingClause:
	/* EMPTY */
	  { $$ = 0; }
	| HAVING aExpr
	  { $$ = new optHavingClause_HAVING_aExpr($1, $2); }
	;


fromList:
	tableRef
	  { $$ = new List<tableRef*>(1, $1); }
	| fromList COMMA tableRef
	  { $1->push_back($3); delete $2; $$ = $1; }
	;


optFromClause:
	/* EMPTY */
	  { $$ = 0; }
	| FROM fromList
	  { $$ = new optFromClause_FROM_fromList($1, $2); }
	;


tableRef:
	relationExpr
	  { $$ = new tableRef_relationExpr($1); }
	| relationExpr aliasClause
	  { $$ = new tableRef_relationExpr_aliasClause($1, $2); }
	| joinedTable
	  { $$ = new tableRef_joinedTable($1); }
	| LEFTPAREN joinedTable RIGHTPAREN aliasClause
	  { $$ = new tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause($1, $2, $3, $4); }
	;


joinedTable:
	LEFTPAREN joinedTable RIGHTPAREN
	  { $$ = new joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN($1, $2, $3); }
	| tableRef CROSS JOIN tableRef
	  { $$ = new joinedTable_tableRef_CROSS_JOIN_tableRef($1, $2, $3, $4); }
	| tableRef UNIONJOIN tableRef
	  { $$ = new joinedTable_tableRef_UNIONJOIN_tableRef($1, $2, $3); }
	| tableRef joinType JOIN tableRef joinQual
	  { $$ = new joinedTable_tableRef_joinType_JOIN_tableRef_joinQual($1, $2, $3, $4, $5); }
	| tableRef JOIN tableRef joinQual
	  { $$ = new joinedTable_tableRef_JOIN_tableRef_joinQual($1, $2, $3, $4); }
	| tableRef NATURAL joinType JOIN tableRef
	  { $$ = new joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef($1, $2, $3, $4, $5); }
	| tableRef NATURAL JOIN tableRef
	  { $$ = new joinedTable_tableRef_NATURAL_JOIN_tableRef($1, $2, $3, $4); }
	;


aliasClause:
	AS strColId LEFTPAREN nameList RIGHTPAREN
	  { $$ = new aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN($1, $2, $3, $4, $5); }
	| AS strColId
	  { $$ = new aliasClause_AS_strColId($1, $2); }
	| strColId LEFTPAREN nameList RIGHTPAREN
	  { $$ = new aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN($1, $2, $3, $4); }
	| strColId
	  { $$ = new aliasClause_strColId($1); }
	;


joinType:
	FULL strOptJoinOuter
	  { $$ = new joinType_FULL_strOptJoinOuter($1, $2); }
	| LEFT strOptJoinOuter
	  { $$ = new joinType_LEFT_strOptJoinOuter($1, $2); }
	| RIGHT strOptJoinOuter
	  { $$ = new joinType_RIGHT_strOptJoinOuter($1, $2); }
	| INNERP
	  { $$ = new joinType_INNERP($1); }
	;


strOptJoinOuter:
	/* EMPTY */
	  { $$ = 0; }
	| OUTERP
	  { $$ = $1; }
	;


joinQual:
	USING LEFTPAREN nameList RIGHTPAREN
	  { $$ = new joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN($1, $2, $3, $4); }
	| ON aExpr
	  { $$ = new joinQual_ON_aExpr($1, $2); }
	;


relationExpr:
	strRelationName
	  { $$ = new relationExpr_strRelationName($1); }
	| strRelationName ASTERISK
	  { $$ = new relationExpr_strRelationName_ASTERISK($1, $2); }
	| ONLY strRelationName
	  { $$ = new relationExpr_ONLY_strRelationName($1, $2); }
	;


optWhereClause:
	/* EMPTY */
	  { $$ = 0; }
	| WHERE aExpr
	  { $$ = new optWhereClause_WHERE_aExpr($1, $2); }
	;


strDatetime:
	YEARP
	  { $$ = new String("YEAR"); }
	| MONTHP
	  { $$ = new String("MONTH"); }
	| DAYP
	  { $$ = new String("DAY"); }
	| HOURP
	  { $$ = new String("HOUR"); }
	| MINUTEP
	  { $$ = new String("MINUTE"); }
	| SECONDP
	  { $$ = new String("SECOND"); }
	;


rowExpr:
	LEFTPAREN rowDescriptor RIGHTPAREN strAllOp LEFTPAREN rowDescriptor RIGHTPAREN
	  { $$ = new rowExpr($1, $2, $3, $4, $5, $6, $7); }
	;


rowList:
	aExpr
	  { $$ = new List<aExpr*>(1, $1); }
	| rowList COMMA aExpr
	  { $1->push_back($3); delete $2; $$ = $1; }
	;


rowDescriptor:
	rowList COMMA aExpr
	  { $$ = new rowDescriptor($1, $2, $3); }
	;


strAllOp:
	PLUS
	  { $$ = $1; }
	| MINUS
	  { $$ = $1; }
	| ASTERISK
	  { $$ = $1; }
	| SOLIDUS
	  { $$ = $1; }
	| PERCENT
	  { $$ = $1; }
	| LESSTHAN
	  { $$ = $1; }
	| LESSTHANOREQUALS
	  { $$ = $1; }
	| GREATERTHAN
	  { $$ = $1; }
	| GREATERTHANOREQUALS
	  { $$ = $1; }
	| EQUALS
	  { $$ = $1; }
	| NOTEQUALS
	  { $$ = $1; }
	| CONCATENATION
	  { $$ = $1; }
	| BITAND
	  { $$ = $1; }
	| BITOR
	  { $$ = $1; }
	| BITSHIFTLEFT
	  { $$ = $1; }
	| BITSHIFTRIGHT
	  { $$ = $1; }
	;


aExpr:
	cExpr
	  { $$ = new aExpr_cExpr($1); }
	| aExpr AT TIME ZONE cExpr
	  { $$ = new aExpr_aExpr_AT_TIME_ZONE_cExpr($1, $2, $3, $4, $5); }
	| PLUS aExpr %prec UMINUS
	  { $$ = new aExpr_PLUS_aExpr($1, $2); }
	| MINUS aExpr %prec UMINUS
	  { $$ = new aExpr_MINUS_aExpr($1, $2); }
	| BITINVERT aExpr %prec UMINUS
	  { $$ = new aExpr_BITINVERT_aExpr($1, $2); }
	| aExpr PLUS aExpr
	  { $$ = new aExpr_aExpr_PLUS_aExpr($1, $2, $3); }
	| aExpr MINUS aExpr
	  { $$ = new aExpr_aExpr_MINUS_aExpr($1, $2, $3); }
	| aExpr ASTERISK aExpr
	  { $$ = new aExpr_aExpr_ASTERISK_aExpr($1, $2, $3); }
	| aExpr SOLIDUS aExpr
	  { $$ = new aExpr_aExpr_SOLIDUS_aExpr($1, $2, $3); }
	| aExpr PERCENT aExpr
	  { $$ = new aExpr_aExpr_PERCENT_aExpr($1, $2, $3); }
	| aExpr BITAND aExpr
	  { $$ = new aExpr_aExpr_BITAND_aExpr($1, $2, $3); }
	| aExpr BITOR aExpr
	  { $$ = new aExpr_aExpr_BITOR_aExpr($1, $2, $3); }
	| aExpr BITSHIFTLEFT aExpr
	  { $$ = new aExpr_aExpr_BITSHIFTLEFT_aExpr($1, $2, $3); }
	| aExpr BITSHIFTRIGHT aExpr
	  { $$ = new aExpr_aExpr_BITSHIFTRIGHT_aExpr($1, $2, $3); }
	| aExpr LESSTHAN aExpr
	  { $$ = new aExpr_aExpr_LESSTHAN_aExpr($1, $2, $3); }
	| aExpr LESSTHANOREQUALS aExpr
	  { $$ = new aExpr_aExpr_LESSTHANOREQUALS_aExpr($1, $2, $3); }
	| aExpr GREATERTHAN aExpr
	  { $$ = new aExpr_aExpr_GREATERTHAN_aExpr($1, $2, $3); }
	| aExpr GREATERTHANOREQUALS aExpr
	  { $$ = new aExpr_aExpr_GREATERTHANOREQUALS_aExpr($1, $2, $3); }
	| aExpr EQUALS aExpr
	  { $$ = new aExpr_aExpr_EQUALS_aExpr($1, $2, $3); }
	| aExpr NOTEQUALS aExpr
	  { $$ = new aExpr_aExpr_NOTEQUALS_aExpr($1, $2, $3); }
	| aExpr AND aExpr
	  { $$ = new aExpr_aExpr_AND_aExpr($1, $2, $3); }
	| aExpr OR aExpr
	  { $$ = new aExpr_aExpr_OR_aExpr($1, $2, $3); }
	| NOT aExpr
	  { $$ = new aExpr_NOT_aExpr($1, $2); }
	| aExpr CONCATENATION aExpr
	  { $$ = new aExpr_aExpr_CONCATENATION_aExpr($1, $2, $3); }
	| aExpr LIKE aExpr
	  { $$ = new aExpr_aExpr_LIKE_aExpr($1, $2, $3); }
	| aExpr LIKE aExpr ESCAPE aExpr
	  { $$ = new aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr($1, $2, $3, $4, $5); }
	| aExpr NOT LIKE aExpr
	  { $$ = new aExpr_aExpr_NOT_LIKE_aExpr($1, $2, $3, $4); }
	| aExpr NOT LIKE aExpr ESCAPE aExpr
	  { $$ = new aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr($1, $2, $3, $4, $5, $6); }
	| aExpr ISNULL
	  { $$ = new aExpr_aExpr_ISNULL($1, $2); }
	| aExpr IS NULLP
	  { $$ = new aExpr_aExpr_IS_NULLP($1, $2, $3); }
	| aExpr NOTNULL
	  { $$ = new aExpr_aExpr_NOTNULL($1, $2); }
	| aExpr IS NOT NULLP
	  { $$ = new aExpr_aExpr_IS_NOT_NULLP($1, $2, $3, $4); }
	| aExpr IS TRUEP
	  { $$ = new aExpr_aExpr_IS_TRUEP($1, $2, $3); }
	| aExpr IS NOT FALSEP
	  { $$ = new aExpr_aExpr_IS_NOT_FALSEP($1, $2, $3, $4); }
	| aExpr IS FALSEP
	  { $$ = new aExpr_aExpr_IS_FALSEP($1, $2, $3); }
	| aExpr IS NOT TRUEP
	  { $$ = new aExpr_aExpr_IS_NOT_TRUEP($1, $2, $3, $4); }
	| aExpr ISA aExpr
	  { $$ = new aExpr_aExpr_ISA_aExpr($1, $2, $3); }
	| rowExpr
	  { $$ = new aExpr_rowExpr($1); }
	;


bExpr:
	cExpr
	  { $$ = new bExpr_cExpr($1); }
	| PLUS bExpr %prec UMINUS
	  { $$ = new bExpr_PLUS_bExpr($1, $2); }
	| MINUS bExpr %prec UMINUS
	  { $$ = new bExpr_MINUS_bExpr($1, $2); }
	| BITINVERT bExpr %prec UMINUS
	  { $$ = new bExpr_BITINVERT_bExpr($1, $2); }
	| bExpr PLUS bExpr
	  { $$ = new bExpr_bExpr_PLUS_bExpr($1, $2, $3); }
	| bExpr MINUS bExpr
	  { $$ = new bExpr_bExpr_MINUS_bExpr($1, $2, $3); }
	| bExpr ASTERISK bExpr
	  { $$ = new bExpr_bExpr_ASTERISK_bExpr($1, $2, $3); }
	| bExpr SOLIDUS bExpr
	  { $$ = new bExpr_bExpr_SOLIDUS_bExpr($1, $2, $3); }
	| bExpr PERCENT bExpr
	  { $$ = new bExpr_bExpr_PERCENT_bExpr($1, $2, $3); }
	| bExpr BITAND bExpr
	  { $$ = new bExpr_bExpr_BITAND_bExpr($1, $2, $3); }
	| bExpr BITOR bExpr
	  { $$ = new bExpr_bExpr_BITOR_bExpr($1, $2, $3); }
	| bExpr BITSHIFTLEFT bExpr
	  { $$ = new bExpr_bExpr_BITSHIFTLEFT_bExpr($1, $2, $3); }
	| bExpr BITSHIFTRIGHT bExpr
	  { $$ = new bExpr_bExpr_BITSHIFTRIGHT_bExpr($1, $2, $3); }
	| bExpr LESSTHAN bExpr
	  { $$ = new bExpr_bExpr_LESSTHAN_bExpr($1, $2, $3); }
	| bExpr LESSTHANOREQUALS bExpr
	  { $$ = new bExpr_bExpr_LESSTHANOREQUALS_bExpr($1, $2, $3); }
	| bExpr GREATERTHAN bExpr
	  { $$ = new bExpr_bExpr_GREATERTHAN_bExpr($1, $2, $3); }
	| bExpr GREATERTHANOREQUALS bExpr
	  { $$ = new bExpr_bExpr_GREATERTHANOREQUALS_bExpr($1, $2, $3); }
	| bExpr EQUALS bExpr
	  { $$ = new bExpr_bExpr_EQUALS_bExpr($1, $2, $3); }
	| bExpr NOTEQUALS bExpr
	  { $$ = new bExpr_bExpr_NOTEQUALS_bExpr($1, $2, $3); }
	| bExpr CONCATENATION bExpr
	  { $$ = new bExpr_bExpr_CONCATENATION_bExpr($1, $2, $3); }
	;


cExpr:
	attr
	  { $$ = new cExpr_attr($1); }
	| strColId optIndirection
	  { $$ = new cExpr_strColId_optIndirection($1, $2); }
	| aExprConst
	  { $$ = new cExpr_aExprConst($1); }
	| LEFTPAREN aExpr RIGHTPAREN
	  { $$ = new cExpr_LEFTPAREN_aExpr_RIGHTPAREN($1, $2, $3); }
	| strFuncName LEFTPAREN RIGHTPAREN
	  { $$ = new cExpr_strFuncName_LEFTPAREN_RIGHTPAREN($1, $2, $3); }
	| strFuncName LEFTPAREN exprSeq RIGHTPAREN
	  { $$ = new cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN($1, $2, $3, $4); }
	| strFuncName LEFTPAREN ALL exprSeq RIGHTPAREN
	  { $$ = new cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN($1, $2, $3, $4, $5); }
	| strFuncName LEFTPAREN DISTINCT exprSeq RIGHTPAREN
	  { $$ = new cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN($1, $2, $3, $4, $5); }
	| strFuncName LEFTPAREN ASTERISK RIGHTPAREN
	  { $$ = new cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN($1, $2, $3, $4); }
	| CURRENTDATE
	  { $$ = new cExpr_CURRENTDATE($1); }
	| CURRENTTIME
	  { $$ = new cExpr_CURRENTTIME($1); }
	| CURRENTTIME LEFTPAREN ICONST RIGHTPAREN
	  { $$ = new cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN($1, $2, $3, $4); }
	| CURRENTTIMESTAMP
	  { $$ = new cExpr_CURRENTTIMESTAMP($1); }
	| CURRENTTIMESTAMP LEFTPAREN ICONST RIGHTPAREN
	  { $$ = new cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN($1, $2, $3, $4); }
	| CURRENTUSER
	  { $$ = new cExpr_CURRENTUSER($1); }
	| SESSIONUSER
	  { $$ = new cExpr_SESSIONUSER($1); }
	| USER
	  { $$ = new cExpr_USER($1); }
	| EXTRACT LEFTPAREN optExtract RIGHTPAREN
	  { $$ = new cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN($1, $2, $3, $4); }
	| POSITION LEFTPAREN positionExpr RIGHTPAREN
	  { $$ = new cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN($1, $2, $3, $4); }
	| SUBSTRING LEFTPAREN optSubstrExpr RIGHTPAREN
	  { $$ = new cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN($1, $2, $3, $4); }
	| TRIM LEFTPAREN LEADING trimExpr RIGHTPAREN
	  { $$ = new cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN($1, $2, $3, $4, $5); }
	| TRIM LEFTPAREN TRAILING trimExpr RIGHTPAREN
	  { $$ = new cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN($1, $2, $3, $4, $5); }
	| TRIM LEFTPAREN trimExpr RIGHTPAREN
	  { $$ = new cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN($1, $2, $3, $4); }
	;


optIndirection:
	/* EMPTY */
	  { $$ = 0; }
	| optIndirection LEFTBRACKET aExpr RIGHTBRACKET
	  { $$ = new optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET($1, $2, $3, $4); }
	| optIndirection LEFTBRACKET aExpr COLON aExpr RIGHTBRACKET
	  { $$ = new optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET($1, $2, $3, $4, $5, $6); }
	;


optExtract:
	/* EMPTY */
	  { $$ = 0; }
	| strExtractArg FROM aExpr
	  { $$ = new optExtract_strExtractArg_FROM_aExpr($1, $2, $3); }
	;


strExtractArg:
	strDatetime
	  { $$ = $1; }
	| SCONST
	  { $$ = $1; }
	| IDENT
	  { $$ = $1; }
	| TIMEZONEHOUR
	  { $$ = new String("TIMEZONE_HOUR"); }
	| TIMEZONEMINUTE
	  { $$ = new String("TIMEZONE_MINUTE"); }
	;


positionExpr:
	bExpr IN bExpr
	  { $$ = new positionExpr_bExpr_IN_bExpr($1, $2, $3); }
	| /* EMPTY */
	  { $$ = 0; }
	;


optSubstrExpr:
	/* EMPTY */
	  { $$ = 0; }
	| aExpr substrFrom substrFor
	  { $$ = new optSubstrExpr_aExpr_substrFrom_substrFor($1, $2, $3); }
	| aExpr substrFor substrFrom
	  { $$ = new optSubstrExpr_aExpr_substrFor_substrFrom($1, $2, $3); }
	| aExpr substrFrom
	  { $$ = new optSubstrExpr_aExpr_substrFrom($1, $2); }
	| aExpr substrFor
	  { $$ = new optSubstrExpr_aExpr_substrFor($1, $2); }
	| exprSeq
	  { $$ = new optSubstrExpr_exprSeq($1); }
	;


substrFrom:
	FROM aExpr
	  { $$ = new substrFrom($1, $2); }
	;


substrFor:
	FOR aExpr
	  { $$ = new substrFor($1, $2); }
	;


trimExpr:
	aExpr FROM exprSeq
	  { $$ = new trimExpr_aExpr_FROM_exprSeq($1, $2, $3); }
	| FROM exprSeq
	  { $$ = new trimExpr_FROM_exprSeq($1, $2); }
	| exprSeq
	  { $$ = new trimExpr_exprSeq($1); }
	;


attr:
	strRelationName PERIOD attrs optIndirection
	  { $$ = new attr($1, $2, $3, $4); }
	;


attrs:
	strAttrName
	  { $$ = new attrs_strAttrName($1); }
	| attrs PERIOD strAttrName
	  { $$ = new attrs_attrs_PERIOD_strAttrName($1, $2, $3); }
	| attrs PERIOD ASTERISK
	  { $$ = new attrs_attrs_PERIOD_ASTERISK($1, $2, $3); }
	;


targetEl:
	aExpr AS strColLabel
	  { $$ = new targetEl_aExpr_AS_strColLabel($1, $2, $3); }
	| aExpr
	  { $$ = new targetEl_aExpr($1); }
	| strRelationName PERIOD ASTERISK
	  { $$ = new targetEl_strRelationName_PERIOD_ASTERISK($1, $2, $3); }
	| ASTERISK
	  { $$ = new targetEl_ASTERISK($1); }
	;


updateTargetEl:
	strColId optIndirection EQUALS aExpr
	  { $$ = new updateTargetEl($1, $2, $3, $4); }
	;


strRelationName:
	strColId
	  { $$ = $1; }
	;


strName:
	strColId
	  { $$ = $1; }
	;


strAttrName:
	strColId
	  { $$ = $1; }
	;


strFuncName:
	strColId
	  { $$ = $1; }
	| IN
	  { $$ = new String("IN"); }
	| IS
	  { $$ = new String("IS"); }
	| ISNULL
	  { $$ = new String("ISNULL"); }
	| LIKE
	  { $$ = new String("LIKE"); }
	| NOTNULL
	  { $$ = new String("NOTNULL"); }
	;


aExprConst:
	ICONST
	  { $$ = new aExprConst_ICONST($1); }
	| FCONST
	  { $$ = new aExprConst_FCONST($1); }
	| SCONST
	  { $$ = new aExprConst_SCONST($1); }
	| BITCONST
	  { $$ = new aExprConst_BITCONST($1); }
	| HEXCONST
	  { $$ = new aExprConst_HEXCONST($1); }
	| TRUEP
	  { $$ = new aExprConst_TRUEP(new String("TRUE")); }
	| FALSEP
	  { $$ = new aExprConst_FALSEP(new String("FALSE")); }
	| NULLP
	  { $$ = new aExprConst_NULLP(new String("NULL")); }
	;


strColId:
	IDENT
	  { $$ = $1; }
	| strDatetime
	  { $$ = $1; }
	| AT
	  { $$ = new String("AT"); }
	| BY
	  { $$ = new String("BY"); }
	| DELETE
	  { $$ = new String("DELETE"); }
	| ESCAPE
	  { $$ = new String("ESCAPE"); }
	| INSERT
	  { $$ = new String("INSERT"); }
	| SET
	  { $$ = new String("SET"); }
	| TIMEZONEHOUR
	  { $$ = new String("TIMEZONE_HOUR"); }
	| TIMEZONEMINUTE
	  { $$ = new String("TIMEZONE_MINUTE"); }
	| UPDATE
	  { $$ = new String("UPDATE"); }
	| VALUES
	  { $$ = new String("VALUES"); }
	| ZONE
	  { $$ = new String("ZONE"); }
	| INTERVAL
	  { $$ = new String("INTERVAL"); }
	| NATIONAL
	  { $$ = new String("NATIONAL"); }
	| TIME
	  { $$ = new String("TIME"); }
	| TIMESTAMP
	  { $$ = new String("TIMESTAMP"); }
	;


strColLabel:
	strColId
	  { $$ = $1; }
	| ALL
	  { $$ = new String("ALL"); }
	| AND
	  { $$ = new String("AND"); }
	| ASC
	  { $$ = new String("ASC"); }
	| CROSS
	  { $$ = new String("CROSS"); }
	| CURRENTDATE
	  { $$ = new String("CURRENT_DATE"); }
	| CURRENTTIME
	  { $$ = new String("CURRENT_TIME"); }
	| CURRENTTIMESTAMP
	  { $$ = new String("CURRENT_TIMESTAMP"); }
	| CURRENTUSER
	  { $$ = new String("CURRENT_USER"); }
	| DEFAULT
	  { $$ = new String("DEFAULT"); }
	| DESC
	  { $$ = new String("DESC"); }
	| DISTINCT
	  { $$ = new String("DISTINCT"); }
	| EXTRACT
	  { $$ = new String("EXTRACT"); }
	| FALSEP
	  { $$ = new String("FALSE"); }
	| FOR
	  { $$ = new String("FOR"); }
	| FROM
	  { $$ = new String("FROM"); }
	| FULL
	  { $$ = new String("FULL"); }
	| GROUP
	  { $$ = new String("GROUP"); }
	| HAVING
	  { $$ = new String("HAVING"); }
	| IN
	  { $$ = new String("IN"); }
	| INNERP
	  { $$ = new String("INNER"); }
	| INTO
	  { $$ = new String("INTO"); }
	| IS
	  { $$ = new String("IS"); }
	| ISA
	  { $$ = new String("ISA"); }
	| ISNULL
	  { $$ = new String("ISNULL"); }
	| JOIN
	  { $$ = new String("JOIN"); }
	| LEADING
	  { $$ = new String("LEADING"); }
	| LEFT
	  { $$ = new String("LEFT"); }
	| LIKE
	  { $$ = new String("LIKE"); }
	| NATURAL
	  { $$ = new String("NATURAL"); }
	| NOT
	  { $$ = new String("NOT"); }
	| NOTNULL
	  { $$ = new String("NOTNULL"); }
	| NULLP
	  { $$ = new String("NULL"); }
	| ON
	  { $$ = new String("ON"); }
	| ONLY
	  { $$ = new String("ONLY"); }
	| OR
	  { $$ = new String("OR"); }
	| ORDER
	  { $$ = new String("ORDER"); }
	| OUTERP
	  { $$ = new String("OUTER"); }
	| POSITION
	  { $$ = new String("POSITION"); }
	| RIGHT
	  { $$ = new String("RIGHT"); }
	| SELECT
	  { $$ = new String("SELECT"); }
	| SESSIONUSER
	  { $$ = new String("SESSION_USER"); }
	| SUBSTRING
	  { $$ = new String("SUBSTRING"); }
	| TRAILING
	  { $$ = new String("TRAILING"); }
	| TRIM
	  { $$ = new String("TRIM"); }
	| TRUEP
	  { $$ = new String("TRUE"); }
	| UNION
	  { $$ = new String("UNION"); }
	| USER
	  { $$ = new String("USER"); }
	| USING
	  { $$ = new String("USING"); }
	| WHERE
	  { $$ = new String("WHERE"); }
	;

%%


void owwqlerror(ParseError* p_err, const char* s)
{
	// If it's not empty, then the lexer reported an error, and we don't want to overwrite it.
	if (p_err->message.empty())
	{
		p_err->message = String(s);
	}
}


