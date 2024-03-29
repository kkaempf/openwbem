%{
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
%token FCONST
%token SIGN
%token HEXCONST
%token BITCONST
%token DATETIMEVALUE
/* This shouldn't be a token, I did it to try and resolve the conflicts
%token CLASSNAME
*/


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
      SELECT select_criteria FROM from_criteria opt_where_clause 
        opt_setconstant_clause opt_sortedby_clause opt_calculation_clause
;

opt_where_clause:
      /* empty */
    | WHERE search_condition
;

opt_setconstant_clause:
      /* empty */
    | SETCONSTANT set_clause_list
;

opt_sortedby_clause:
      /* empty */
    | SORTEDBY sort_spec_list
;

opt_calculation_clause:
      /* empty */
    | calculation_clause
;

select_criteria:
      select_list
    | select_criteria COMMA select_list
;

/* this is ambiguous
select_list:
      property_list
    | NULLP
    | select_string_literal
;
*/

select_list:
      class_property_identifier
    | NULLP
    | select_string_literal
;

select_string_literal:
      select_literal_identifier EQUALS SCONST
;

select_literal_identifier:
      IDENT
;

/* not used anymore
property_list:
      class_property_identifier
    | property_list COMMA class_property_identifier
;
*/

/* this is ambiguous with embedded objects
class_property_identifier:
      class_name PERIOD property_identifier
    | property_identifier
;
*/

class_property_identifier:
      class_property_identifier_sub PERIOD property_identifier /* this could be an embedded_object or classname.propertyname */
      class_property_identifier_sub PERIOD terminal_property_identifier /* this could be an embedded_object or classname.propertyname */
    | property_identifier
    | terminal_property_identifier
;

class_property_identifier_sub:
      class_property_identifier_sub PERIOD property_identifier
    | class_name_or_property_identifier
;

/* embedded_object is ambiguous with class_property_identifier
property_identifier:
      ASTERISK
    | property_name
    | array_property
    | embedded_object
    | KEY
    | CLASS
;
*/

property_identifier:
      property_name
    | array_property
;

class_name_or_property_identifier:
      IDENT /* either a class name or a property name */
    | array_property
;

terminal_property_identifier:
      ASTERISK
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

/* property_identifier_list is ambiguous with select_criteria
embedded_object:
      property_name PERIOD property_identifier_list
      property_name PERIOD property_identifier
;
*/

/*
embedded_object:
      property_name PERIOD property_identifier
;
*/

/* not used - see embedded_object
property_identifier_list:
      property_identifier
    | property_identifier_list COMMA property_identifier
;
*/

from_criteria:
      class_list
;

class_list:
      class_name
    | class_list COMMA class_name
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
      EQUALS
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
      class_name REF reference_name ASSOCIATEDBY association_class_name
    | class_name REF reference_name COMMA class_name REF reference_name ASSOCIATEDBY association_class_name
;

time_predicate:
      BETWEEN DATETIMEVALUE COMMA DATETIMEVALUE
;

set_clause_list:
      set_clause
    | set_clause_list COMMA set_clause
;

/* ambiguous, no way to differentiate class_property_identifier and select_literal_identifier
set_clause:
      class_property_identifier EQUALS set_source
    | select_literal_identifier EQUALS set_source
;
*/

set_clause:
      class_property_identifier EQUALS set_source
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

class_name:
      IDENT
;

association_class_name:
      IDENT
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



