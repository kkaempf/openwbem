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
* Portions Copyright (C) 2001 Caldera International, Inc All rights reserved.
	;
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

int yyerror( const char* s);
int yylex();

%}


%token SELECT
%token FROM
%token WHERE
%token SETCONSTANT
%token SORTEDBY
%token COMMA
%token EQUALS
%token IDENT
%token SCONST
%token PERIOD
%token ASTERISK
%token KEY
%token CLASS
%token LEFTBRACKET
%token RIGHTBRACKET
%token ICONST
%token PERIODPERIOD
%token OR
%token AND
%token NOT
%token LEFTPAREN
%token RIGHTPAREN
%token NOTEQUALS
%token LESSTHAN
%token LESSTHANOREQUALS
%token GREATERTHAN
%token GREATERTHANOREQUALS
%token IS
%token REF
%token ASSOCIATEDBY
%token BETWEEN
%token DATESPECIFICATION
%token TIMESPECIFICATION
%token DEFAULT
%token ASCENDING
%token DESCENDING
%token AVERAGE
%token MIN
%token MAX
%token COUNT
%token SUM
%token TRUEP
%token FALSEP
%token NULLP
%token QUOTE
%token FCONST
%token SIGN
%token HEXCONST
%token BITCONST
/* This shouldn't be a token, I did it to try and resolve the conflicts */
%token CLASSNAME


/* unused
%token ALL
%token AS
%token AT
%token BITAND
%token BITINVERT
%token BITOR
%token BITSHIFTLEFT
%token BITSHIFTRIGHT
%token BY
%token COLON
%token CONCATENATION
%token CROSS
%token CURRENTDATE
%token CURRENTTIME
%token CURRENTTIMESTAMP
%token CURRENTUSER
%token DAYP
%token DELETE
%token DISTINCT
%token ESCAPE
%token EXTRACT
%token FOR
%token FULL
%token GROUP
%token HAVING
%token HOURP
%token IN
%token INNERP
%token INSERT
%token INTERVAL
%token INTO
%token ISA
%token ISNULL
%token JOIN
%token LEADING
%token LEFT
%token LIKE
%token MINUS
%token MINUTEP
%token MONTHP
%token NATIONAL
%token NATURAL
%token NOTNULL
%token ON
%token ONLY
%token ORDER
%token OUTERP
%token PERCENT
%token PLUS
%token POSITION
%token RIGHT
%token SECONDP
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
%token UNION
%token UNIONJOIN
%token UPDATE
%token USER
%token USING
%token VALUES
%token YEARP
%token ZONE
*/


%%

start:
	select_statement
;

select_statement:
      SELECT select_criteria FROM from_criteria 
    | SELECT select_criteria FROM from_criteria calculation_clause
    | SELECT select_criteria FROM from_criteria SORTEDBY sort_spec_list
    | SELECT select_criteria FROM from_criteria SORTEDBY sort_spec_list calculation_clause
    | SELECT select_criteria FROM from_criteria SETCONSTANT set_clause_list 
    | SELECT select_criteria FROM from_criteria SETCONSTANT set_clause_list calculation_clause
    | SELECT select_criteria FROM from_criteria SETCONSTANT set_clause_list SORTEDBY sort_spec_list
    | SELECT select_criteria FROM from_criteria SETCONSTANT set_clause_list SORTEDBY sort_spec_list calculation_clause
    | SELECT select_criteria FROM from_criteria WHERE search_condition 
    | SELECT select_criteria FROM from_criteria WHERE search_condition calculation_clause
    | SELECT select_criteria FROM from_criteria WHERE search_condition SORTEDBY sort_spec_list
    | SELECT select_criteria FROM from_criteria WHERE search_condition SORTEDBY sort_spec_list calculation_clause
    | SELECT select_criteria FROM from_criteria WHERE search_condition SETCONSTANT set_clause_list 
    | SELECT select_criteria FROM from_criteria WHERE search_condition SETCONSTANT set_clause_list calculation_clause
    | SELECT select_criteria FROM from_criteria WHERE search_condition SETCONSTANT set_clause_list SORTEDBY sort_spec_list
    | SELECT select_criteria FROM from_criteria WHERE search_condition SETCONSTANT set_clause_list SORTEDBY sort_spec_list calculation_clause

select_criteria:
      select_list
    | select_criteria COMMA select_list
;

select_list:
      property_list
    | NULLP
    | select_string_literal
;

select_string_literal:
      select_literal_identifier EQUALS SCONST
;

select_literal_identifier:
      IDENT
;

property_list:
      class_property_identifier
/* this is ambiguous with select_criteria
    | property_list COMMA class_property_identifier
*/
;

class_property_identifier:
      CLASSNAME PERIOD property_identifier
    | property_identifier
;

property_identifier:
      ASTERISK
    | property_name
    | array_property
    | embedded_object
    | KEY
    | CLASS
;

array_property:
      property_name LEFTBRACKET array_list RIGHTBRACKET
;

array_list:
      ASTERISK
    | array_index_list
;

array_index_list:
      array_index
    | array_index_list COMMA array_index
;

array_index:
      ICONST
    | ICONST PERIODPERIOD ICONST
;

embedded_object:
/* this is ambiguous
      property_name PERIOD property_identifier_list
*/
      property_name PERIOD property_identifier
;

/*
property_identifier_list:
      property_identifier
    | property_identifier_list COMMA property_identifier
;
*/

from_criteria:
      class_list
;

class_list:
      CLASSNAME
    | class_list COMMA CLASSNAME
;

search_condition:
      boolean_term
    | search_condition OR boolean_term
;

boolean_term:
      boolean_factor
    | boolean_term AND boolean_factor
;

boolean_factor:
      boolean_primary
    | NOT boolean_primary
;

boolean_primary:
      predicate
    | LEFTPAREN search_condition RIGHTPAREN
;

predicate:
      compareison_predicate
    | null_predicate
    | association_predicate
    | time_predicate
;

compareison_predicate:
      class_property_identifier comp_op comparison_term
;

comp_op:
      EQUALS /* EQUALSTO? */
    | NOTEQUALS
    | LESSTHAN
    | GREATERTHAN
    | LESSTHANOREQUALS
    | GREATERTHANOREQUALS
;

null_predicate:
      class_property_identifier IS NULLP
    | class_property_identifier IS NOT NULLP
;

association_predicate:
      CLASSNAME REF reference_name ASSOCIATEDBY association_class_name
    | CLASSNAME REF reference_name COMMA CLASSNAME REF reference_name ASSOCIATEDBY association_class_name
;

time_predicate:
      BETWEEN date_time_value COMMA date_time_value
;

date_time_value:
      QUOTE DATESPECIFICATION TIMESPECIFICATION QUOTE
;

set_clause_list:
      set_clause
    | set_clause_list COMMA set_clause
;

set_clause:
      class_property_identifier EQUALS set_source
/* This is a conflict with the rule above.  The grammar has no way of knowing
a difference between a class property IDENT and a select literal IDENT.
    | select_literal_identifier EQUALS set_source
*/
;

set_source:
      numeric_value_expression
    | string_value_expression
    | truth_value
    | NULLP
    | DEFAULT
;

sort_spec_list:
      class_property_identifier ASCENDING
    | class_property_identifier DESCENDING
    | sort_spec_list COMMA class_property_identifier ASCENDING
    | sort_spec_list COMMA class_property_identifier DESCENDING
;

calculation_clause:
      AVERAGE class_property_identifier
    | MIN class_property_identifier
    | MAX class_property_identifier
    | COUNT class_property_identifier
    | SUM class_property_identifier
;

reference_name:
      property_name
;

property_name:
      IDENT
;

/*
class_name:
      IDENT
;
*/

association_class_name:
      CLASSNAME
;

comparison_term:
      class_property_identifier
    | numeric_value_expression
    | string_value_expression
    | truth_value
;

truth_value:
      TRUEP
    | FALSEP
;

numeric_value_expression:
      SIGN unsigned_numeric_literal
;

unsigned_numeric_literal:
      ICONST
    | FCONST
;

string_value_expression:
      SCONST
    | HEXCONST
    | BITCONST
;


%%



