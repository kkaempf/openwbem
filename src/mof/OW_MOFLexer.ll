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

/**
 * @author Dan Nuffer
 */


/* scanner for mof */

%option noyywrap

%{
#include "OW_MOFGrammar.hpp"
#include "OW_MOFCompiler.hpp"
#include "OW_MOFParserDecls.hpp"
#include <blocxx/Format.hpp>
#include "OW_Exception.hpp"
#include <blocxx/String.hpp>
#include "OW_MOFParseError.hpp"
#include <errno.h>
#include <string.h>

// These files are last because they define non-prefixed macros.
#include "OW_MOFParser.h"
#include "OW_MOFLexerDecls.hpp"


#define WHITE_RETURN(x) /* skip it */
#define NEWLINE_RETURN() WHITE_RETURN('\n')

#define RETURN_VAL(x) owmoflval->pString = 0; return(x);
#define RETURN_STR(x) owmoflval->pString = new OpenWBEM::String(owmoftext); return(x);

namespace OW_NAMESPACE
{
OW_DECLARE_EXCEPTION(MOFLexer)
OW_DEFINE_EXCEPTION(MOFLexer)
}

/* Avoid exit() on fatal scanner errors (a bit ugly -- see yy_fatal_error) */
#define YY_FATAL_ERROR(msg) \
	OW_THROW(OpenWBEM::MOFLexerException, msg);

#define YYLEX_PARAM context
#define YY_DECL static int owmoflex_impl(YYSTYPE *owmoflval, YYLTYPE * llocp, OpenWBEM::MOF::CompilerState* YYLEX_PARAM)
#define MOF_COMPILER_STATE context
#define YY_USER_ACTION MOF_COMPILER_STATE->updateLocation(yytext, llocp);
%}

/* here are the definitions */

/* UTF-8 (from Unicode 4.0.0 standard):
Table 3-6. Well-Formed UTF-8 Byte Sequences Code Points 
                   1st Byte 2nd Byte 3rd Byte 4th Byte 
U+0000..U+007F     00..7F 
U+0080..U+07FF     C2..DF   80..BF 
U+0800..U+0FFF     E0       A0..BF   80..BF 
U+1000..U+CFFF     E1..EC   80..BF   80..BF 
U+D000..U+D7FF     ED       80..9F   80..BF 
U+E000..U+FFFF     EE..EF   80..BF   80..BF 
U+10000..U+3FFFF   F0       90..BF   80..BF   80..BF 
U+40000..U+FFFFF   F1..F3   80..BF   80..BF   80..BF 
U+100000..U+10FFFF F4       80..8F   80..BF   80..BF
*/
utf8_2 [\xC2-\xDF][\x80-\xBF]
utf8_3_1 \xE0[\xA0-\xBF][\x80-\xBF]
utf8_3_2 [\xE1-\xEC][\x80-\xBF][\x80-\xBF]
utf8_3_3 \xED[\x80-\x9F][\x80-\xBF]
utf8_3_4 [\xEE-\xEF][\x80-\xBF][\x80-\xBF]
utf8_4_1 \xF0[\x90-\xBF][\x80-\xBF][\x80-\xBF]
utf8_4_2 [\xF1-\xF3][\x80-\xBF][\x80-\xBF][\x80-\xBF]
utf8_4_3 \xF4[\x80-\x8F][\x80-\xBF][\x80-\xBF]

utf8Char {utf8_2}|{utf8_3_1}|{utf8_3_2}|{utf8_3_3}|{utf8_3_4}|{utf8_4_1}|{utf8_4_2}|{utf8_4_3}

identifier ([a-zA-Z_]|{utf8Char})([0-9a-zA-Z_]|{utf8Char})*

binaryValue [+-]?[01]+[bB]
octalValue [+-]?"0"[0-7]+
decimalValue [+-]?([1-9][0-9]*|"0")
hexValue [+-]?"0"[xX][0-9a-fA-F]+
floatValue [+-]?[0-9]*"."[0-9]+([eE][+-]?[0-9]+)?

simpleEscape [bfnrt'"\\]
hexEscape "x"[0-9a-fA-F]{1,4}

escapeSequence [\\]({simpleEscape}|{hexEscape})
cChar [^'\\]|{escapeSequence}
sChar [^"\\]|{escapeSequence}

charValue \'{cChar}\'
stringValue \"{sChar}*\"

ws [ \t]+


%x Ccomment CPPcomment OnlyIdentifier
%%


"/*"                            BEGIN(Ccomment);
<Ccomment>[^*\n\r]*             /* eat anything that's not a '*' */
<Ccomment>"*"+[^*/\n\r]*        /* eat up '*'s not followed by '/'s */
<Ccomment>"\r\n"                
<Ccomment>\n                    
<Ccomment>\r                    
<Ccomment>"*"+"/"               BEGIN(INITIAL);
<Ccomment><<EOF>>               {
	MOF_COMPILER_STATE->getErrorHandler()->fatalError("Unterminated Comment",
		MOF_COMPILER_STATE->getLineInfo());
	yyterminate();
}

"//"                            BEGIN(CPPcomment);
<CPPcomment>[^\n\r]*            /* eat anything but newline */
<CPPcomment>"\r\n"              BEGIN(INITIAL);
<CPPcomment>"\n"                BEGIN(INITIAL);
<CPPcomment>"\r"                BEGIN(INITIAL);

	/* Shame on whoever put special cases in the mof lexical specs and
	didn't even document it! */
<OnlyIdentifier>{ws}*           /* eat up whitespace */
<OnlyIdentifier>{identifier}    {BEGIN(INITIAL);RETURN_STR(IDENTIFIER_TOK);}

{ws}                            /* skip blanks and tabs */
"\r\n"                          
"\n"                            
"\r"                            

any                 {RETURN_STR(ANY_TOK);}
as                  {RETURN_STR(AS_TOK);}
association         {RETURN_STR(ASSOCIATION_TOK);}
class               {RETURN_STR(CLASS_TOK);}
disableoverride     {RETURN_STR(DISABLEOVERRIDE_TOK);}
boolean             {RETURN_STR(DT_BOOL_TOK);}
char16              {RETURN_STR(DT_CHAR16_TOK);}
datetime            {RETURN_STR(DT_DATETIME_TOK);}
real32              {RETURN_STR(DT_REAL32_TOK);}
real64              {RETURN_STR(DT_REAL64_TOK);}
sint16              {RETURN_STR(DT_SINT16_TOK);}
sint32              {RETURN_STR(DT_SINT32_TOK);}
sint64              {RETURN_STR(DT_SINT64_TOK);}
sint8               {RETURN_STR(DT_SINT8_TOK);}
string              {RETURN_STR(DT_STR_TOK);}
uint16              {RETURN_STR(DT_UINT16_TOK);}
uint32              {RETURN_STR(DT_UINT32_TOK);}
uint64              {RETURN_STR(DT_UINT64_TOK);}
uint8               {RETURN_STR(DT_UINT8_TOK);}
enableoverride      {RETURN_STR(ENABLEOVERRIDE_TOK);}
false               {RETURN_STR(FALSE_TOK);}
flavor              {RETURN_STR(FLAVOR_TOK);}
indication          {RETURN_STR(INDICATION_TOK);}
instance            {RETURN_STR(INSTANCE_TOK);}
method              {RETURN_STR(METHOD_TOK);}
null                {RETURN_STR(NULL_TOK);}
of                  {RETURN_STR(OF_TOK);}
parameter           {RETURN_STR(PARAMETER_TOK);}
#pragma             {RETURN_STR(PRAGMA_TOK);}
property            {RETURN_STR(PROPERTY_TOK);}
qualifier           {BEGIN(OnlyIdentifier); RETURN_STR(QUALIFIER_TOK);}
ref                 {RETURN_VAL(REF_TOK);}
reference           {RETURN_STR(REFERENCE_TOK);}
restricted          {RETURN_STR(RESTRICTED_TOK);}
schema              {RETURN_STR(SCHEMA_TOK);}
scope               {RETURN_STR(SCOPE_TOK);}
tosubclass          {RETURN_STR(TOSUBCLASS_TOK);}
translatable        {RETURN_STR(TRANSLATABLE_TOK);}
true                {RETURN_STR(TRUE_TOK);}
"("                 {RETURN_VAL(LPAREN_TOK);}
")"                 {RETURN_VAL(RPAREN_TOK);}
"{"                 {RETURN_VAL(LBRACE_TOK);}
"}"                 {RETURN_VAL(RBRACE_TOK);}
";"                 {RETURN_VAL(SEMICOLON_TOK);}
"["                 {RETURN_VAL(LBRACK_TOK);}
"]"                 {RETURN_VAL(RBRACK_TOK);}
","                 {RETURN_VAL(COMMA_TOK);}
"$"                 {RETURN_VAL(DOLLAR_TOK);}
":"                 {RETURN_VAL(COLON_TOK);}
"="                 {RETURN_VAL(EQUALS_TOK);}



{binaryValue}       {RETURN_STR(binaryValue);}

{octalValue}        {RETURN_STR(octalValue);}

{decimalValue}      {RETURN_STR(decimalValue);}

{hexValue}          {RETURN_STR(hexValue);}

{floatValue}        {RETURN_STR(floatValue);}

{charValue}         {RETURN_STR(charValue);}

{stringValue}       { RETURN_STR(stringValue);}

{identifier}        {RETURN_STR(IDENTIFIER_TOK);}

<<EOF>> {
	if (MOF_COMPILER_STATE->include_stack.empty() )
	{
		yyterminate();
	}
	else
	{
		owmof_delete_buffer( YY_CURRENT_BUFFER );
		/* flex doesn't ever close the file handles.  If we don't free them up, on HPUX where we're limited to 60 files, you can't import the CIM schema! */
		if (owmofin != NULL && owmofin != stdin)
		{
			fclose(owmofin);
		}
		MOF_COMPILER_STATE->getErrorHandler()->progressMessage("Finished parsing.", MOF_COMPILER_STATE->getLineInfo());
		MOF_COMPILER_STATE->setLineInfo(MOF_COMPILER_STATE->include_stack.top());
		owmof_switch_to_buffer(MOF_COMPILER_STATE->include_stack.top().owmofBufferState );
		MOF_COMPILER_STATE->include_stack.pop();
	}
}

%%
/* here is the user code */

void lexIncludeFile( OpenWBEM::MOF::CompilerState* context, const OpenWBEM::String& filename )
{
	using namespace OpenWBEM;
	using namespace OpenWBEM::MOF;

	if ( MOF_COMPILER_STATE->include_stack.size() >= CompilerState::E_MAX_INCLUDE_DEPTH )
	{
		// REPORT AN ERROR
		MOF_COMPILER_STATE->getErrorHandler()->fatalError(
			Format("Includes nested too deep (Max of %1 levels)", (int)CompilerState::E_MAX_INCLUDE_DEPTH).c_str(),
			MOF_COMPILER_STATE->getLineInfo());
		return;
	}

	// first try to find the file in the same dir as our original file.
	String filenameWithPath = MOF_COMPILER_STATE->getBasePath() + "/" + filename;
	FILE* newfile = fopen( filenameWithPath.c_str(), "r" );

	if ( !newfile )
	{
		// try to find it in the current dir
		newfile = fopen( filename.c_str(), "r" );
		if ( !newfile )
		{
			MOF_COMPILER_STATE->getErrorHandler()->fatalError(
				Format("Could not open include file %1: %2(%3)", filename, errno, strerror(errno)).c_str(), MOF_COMPILER_STATE->getLineInfo());
			return;
		}
		filenameWithPath = filename;
	}
	
	owmofin = newfile;
	OpenWBEM::MOF::CompilerState::include_t newInclude;
	newInclude.owmofBufferState = YY_CURRENT_BUFFER;
	newInclude.setLineInfo(*MOF_COMPILER_STATE);
	MOF_COMPILER_STATE->startNewFile(filenameWithPath);
	MOF_COMPILER_STATE->getErrorHandler()->progressMessage("Starting parsing.", MOF_COMPILER_STATE->getLineInfo());
	
	MOF_COMPILER_STATE->include_stack.push(newInclude);

	owmof_switch_to_buffer( owmof_create_buffer( owmofin, YY_BUF_SIZE ) );

}

/* Lexer entry point. The default YYLEX isn't used so that exceptions can be caught */
int owmoflex(YYSTYPE *owmoflval, YYLTYPE * llocp, OpenWBEM::MOF::CompilerState* YYLEX_PARAM, OpenWBEM::MOF::ParseError * p_err)
{
	try
	{
		return owmoflex_impl(owmoflval, llocp, YYLEX_PARAM);
	}
	catch (std::exception& e)
	{
		p_err->message = e.what();
		p_err->column = llocp->first_column;
		p_err->line = llocp->first_line;
	}
	catch (...)
	{
		p_err->message = "Unknown exception";
		p_err->column = llocp->first_column;
		p_err->line = llocp->first_line;
	}
	return SCANNER_ERROR;
}

