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

/* scanner for mof */

%option noyywrap

%{
#include "Grammar.h"
#include "mofparser.h"
#include "MofCompiler.hpp"
#include "OW_Format.hpp"

#define WHITE_RETURN(x) /* skip it */
#define NEWLINE_RETURN() WHITE_RETURN('\n')

#define RETURN_VAL(x) yylval->pString = new OW_String(yytext); return(x);

/* Avoid exit() on fatal scanner errors (a bit ugly -- see yy_fatal_error) */
#define YY_FATAL_ERROR(msg) \
	OW_THROW(OW_Exception, msg);

#define YYLEX_PARAM context
#define YY_DECL int yylex(YYSTYPE *yylval, void* YYLEX_PARAM)
#define MOF_COMPILER ((MofCompiler*)context)
%}

/* here are the definitions */

identifier [a-zA-Z_][0-9a-zA-Z_]*

binaryValue [+-]?[01]+[bB]
octalValue [+-]?"0"[0-7]+
decimalValue [+-]?([1-9]|[0-9]*|"0")
hexValue [+-]?"0"[xX][0-9a-fA-F]+
floatValue [+-]?[0-9]*"."[0-9]+([eE][+-]?[0-9]+)?

simpleEscape [bfnrt'"\\]
hexEscape "x"[0-9a-fA-F]{1,4}

escapeSequence [\\]({simpleEscape}|{hexEscape})
cChar [^'\\\n\r]|{escapeSequence}
sChar [^"\\\n\r]|{escapeSequence}

charValue \'{cChar}\'
	/*stringValue \"{sChar}*\"(({ws}|[\n\r])*\"{sChar}*\")* */
stringValue \"{sChar}*\"

ws		[ \t]+


%x Ccomment CPPcomment OnlyIdentifier
%%


"/*"							BEGIN(Ccomment);
<Ccomment>[^*\n\r]*			/* eat anything that's not a '*' */
<Ccomment>"*"+[^*/\n\r]*	/* eat up '*'s not followed by '/'s */
<Ccomment>"\r\n"				++MOF_COMPILER->theLineInfo.lineNum;
<Ccomment>\n				++MOF_COMPILER->theLineInfo.lineNum;
<Ccomment>\r				++MOF_COMPILER->theLineInfo.lineNum;
<Ccomment>"*"+"/"			BEGIN(INITIAL);
<Ccomment><<EOF>>		{
	MOF_COMPILER->theErrorHandler->fatalError("Unterminated Comment",
		MOF_COMPILER->theLineInfo);
	yyterminate();
}

"//"							BEGIN(CPPcomment);
<CPPcomment>[^\n\r]*		/* eat anything but newline */
<CPPcomment>"\r\n"			++MOF_COMPILER->theLineInfo.lineNum; BEGIN(INITIAL);
<CPPcomment>"\n"			++MOF_COMPILER->theLineInfo.lineNum; BEGIN(INITIAL);
<CPPcomment>"\r"			++MOF_COMPILER->theLineInfo.lineNum; BEGIN(INITIAL);

	/* Shame on whoever put special cases in the mof lexical specs and
	didn't even document it! */
<OnlyIdentifier>{ws}*			/* eat up whitespace */
<OnlyIdentifier>{identifier} {BEGIN(INITIAL);RETURN_VAL(IDENTIFIER_TOK);}

{ws}	/* skip blanks and tabs */
"\r\n"	++MOF_COMPILER->theLineInfo.lineNum;
"\n"	++MOF_COMPILER->theLineInfo.lineNum;
"\r"	++MOF_COMPILER->theLineInfo.lineNum;

any					{RETURN_VAL(ANY_TOK);}
as						{RETURN_VAL(AS_TOK);}
association			{RETURN_VAL(ASSOCIATION_TOK);}
class					{RETURN_VAL(CLASS_TOK);}
disableoverride	{RETURN_VAL(DISABLEOVERRIDE_TOK);}
boolean				{RETURN_VAL(DT_BOOL_TOK);}
char16				{RETURN_VAL(DT_CHAR16_TOK);}
datetime				{RETURN_VAL(DT_DATETIME_TOK);}
real32				{RETURN_VAL(DT_REAL32_TOK);}
real64				{RETURN_VAL(DT_REAL64_TOK);}
sint16				{RETURN_VAL(DT_SINT16_TOK);}
sint32				{RETURN_VAL(DT_SINT32_TOK);}
sint64				{RETURN_VAL(DT_SINT64_TOK);}
sint8					{RETURN_VAL(DT_SINT8_TOK);}
string				{RETURN_VAL(DT_STR_TOK);}
uint16				{RETURN_VAL(DT_UINT16_TOK);}
uint32				{RETURN_VAL(DT_UINT32_TOK);}
uint64				{RETURN_VAL(DT_UINT64_TOK);}
uint8					{RETURN_VAL(DT_UINT8_TOK);}
enableoverride		{RETURN_VAL(ENABLEOVERRIDE_TOK);}
false					{RETURN_VAL(FALSE_TOK);}
flavor				{RETURN_VAL(FLAVOR_TOK);}
indication			{RETURN_VAL(INDICATION_TOK);}
instance				{RETURN_VAL(INSTANCE_TOK);}
method				{RETURN_VAL(METHOD_TOK);}
null					{RETURN_VAL(NULL_TOK);}
of						{RETURN_VAL(OF_TOK);}
parameter			{RETURN_VAL(PARAMETER_TOK);}
#pragma				{RETURN_VAL(PRAGMA_TOK);}
property				{RETURN_VAL(PROPERTY_TOK);}
qualifier			{BEGIN(OnlyIdentifier); RETURN_VAL(QUALIFIER_TOK);}
ref					{RETURN_VAL(REF_TOK);}
reference			{RETURN_VAL(REFERENCE_TOK);}
restricted			{RETURN_VAL(RESTRICTED_TOK);}
schema				{RETURN_VAL(SCHEMA_TOK);}
scope					{RETURN_VAL(SCOPE_TOK);}
tosubclass			{RETURN_VAL(TOSUBCLASS_TOK);}
translatable		{RETURN_VAL(TRANSLATABLE_TOK);}
true					{RETURN_VAL(TRUE_TOK);}
"("					{RETURN_VAL(LPAREN_TOK);}
")"					{RETURN_VAL(RPAREN_TOK);}
"{"					{RETURN_VAL(LBRACE_TOK);}
"}"					{RETURN_VAL(RBRACE_TOK);}
";"					{RETURN_VAL(SEMICOLON_TOK);}
"["					{RETURN_VAL(LBRACK_TOK);}
"]"					{RETURN_VAL(RBRACK_TOK);}
","					{RETURN_VAL(COMMA_TOK);}
"$"					{RETURN_VAL(DOLLAR_TOK);}
":"					{RETURN_VAL(COLON_TOK);}
"="					{RETURN_VAL(EQUALS_TOK);}



{binaryValue}		{RETURN_VAL(binaryValue);}

{octalValue}		{RETURN_VAL(octalValue);}

{decimalValue}		{RETURN_VAL(decimalValue);}

{hexValue}			{RETURN_VAL(hexValue);}

{floatValue}			{RETURN_VAL(floatValue);}

{charValue}			{RETURN_VAL(charValue);}

{stringValue}(({ws}|[\r\n])*{stringValue})*		{
	/* figure out how many lines we passed over */
	int i;
	for (i = 0; i < yyleng; ++i)
	{
		if ( yytext[i] == '\r' || yytext[i] == '\n' )
		{
			++MOF_COMPILER->theLineInfo.lineNum;
			if ( ( i + 1 ) < yyleng )
				if ( yytext[i] == '\r' && yytext[i + 1] == '\n' )
					++i;
		}
	}
	RETURN_VAL(stringValue);
}

{identifier}		{RETURN_VAL(IDENTIFIER_TOK);}

<<EOF>> {
	if ( --(MOF_COMPILER->include_stack_ptr) < 0 )
	{
		yyterminate();
	}
	else
	{
		yy_delete_buffer( YY_CURRENT_BUFFER );
		MOF_COMPILER->theErrorHandler->progressMessage("Finished parsing.", MOF_COMPILER->theLineInfo);
		MOF_COMPILER->theLineInfo = MOF_COMPILER->include_stack[MOF_COMPILER->include_stack_ptr].theLineInfo;
		yy_switch_to_buffer(
			MOF_COMPILER->include_stack[MOF_COMPILER->include_stack_ptr].yyBufferState );
	}
}

%%
/* here is the user code */

void lexIncludeFile( void* context, const OW_String& filename )
{
	if ( MOF_COMPILER->include_stack_ptr >= MAX_INCLUDE_DEPTH )
	{
		// REPORT AN ERROR
		MOF_COMPILER->theErrorHandler->fatalError(
			format("Includes nested too deep (Max of %1 levels)", MAX_INCLUDE_DEPTH).c_str(),
			MOF_COMPILER->theLineInfo);
		return;
	}

	// first try to find the file in the same dir as our original file.
	OW_String filenameWithPath = MOF_COMPILER->basepath + "/" + filename;
	FILE* newfile = fopen( filenameWithPath.c_str(), "r" );

	if ( !newfile )
	{
		// try to find it in the current dir
		newfile = fopen( filename.c_str(), "r" );
		if ( !newfile )
		{
			MOF_COMPILER->theErrorHandler->fatalError(
				format("Could not open include file %1", filename).c_str(), MOF_COMPILER->theLineInfo);
			return;
		}
		filenameWithPath = filename;
	}
	
	yyin = newfile;
	MOF_COMPILER->include_stack[MOF_COMPILER->include_stack_ptr].yyBufferState = YY_CURRENT_BUFFER;
	MOF_COMPILER->include_stack[MOF_COMPILER->include_stack_ptr].theLineInfo = MOF_COMPILER->theLineInfo;
	MOF_COMPILER->theLineInfo.filename = filenameWithPath;
	MOF_COMPILER->theLineInfo.lineNum = 1;
	MOF_COMPILER->theErrorHandler->progressMessage("Starting parsing.", MOF_COMPILER->theLineInfo);
	
	++(MOF_COMPILER->include_stack_ptr);

	yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE ) );


	(void)yyunput; // To get rid of compiler warning about unused variable.
	(void)yy_fatal_error;
}
