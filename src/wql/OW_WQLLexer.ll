%option noyywrap

%{
/*-------------------------------------------------------------------------
 *
 * scan.l
 *	  lexical scanner for OpenWBEM WQL
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 *-------------------------------------------------------------------------
 */
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
#include "OW_WQLAst.hpp"
#include "OW_WQLImpl.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"

#include <string.h>
#include <errno.h>
#include <ctype.h>

// this has to be before OW_WQLParser.h is included.
using namespace OpenWBEM;
#include "OW_WQLParser.h"

static const char *parseCh;

/* set up my input handler --- need one flavor for flex, one for lex */
#if defined(FLEX_SCANNER)

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_UNPUT
static int myinput(char* buf, int max);
#undef YY_INPUT
#define YY_INPUT(buf,result,max) {result = myinput(buf,max);}

/* No reason to constrain amount of data slurped per myinput() call. */
#define YY_READ_BUF_SIZE 16777216

/* Avoid exit() on fatal scanner errors (a bit ugly -- see yy_fatal_error) */
#define YY_FATAL_ERROR(msg) \
		OW_THROWCIMMSG( CIMException::INVALID_QUERY, \
			format("Fatal Parser Error: %1", msg).c_str())

#else /* !FLEX_SCANNER */

#undef input
int input();
#undef unput
void unput(char);

#endif /* FLEX_SCANNER */

extern YYSTYPE owwqllval;


static int		xcdepth = 0;	/* depth of nesting in slash-star comments */

static StringBuffer strbuffer;

#define RETURN_VAL(x) owwqllval.pstring = 0; return(x);
#define RETURN_STR(x) owwqllval.pstring = new String(owwqltext); return(x);
#define RETURN_BUFF_VAL(x) owwqllval.pstring = new String(strbuffer.c_str()); return(x);

%}
/*
 * OK, here is a short description of lex/flex rules behavior.
 * The longest pattern which matches an input string is always chosen.
 * For equal-length patterns, the first occurring in the rules list is chosen.
 * INITIAL is the starting state, to which all non-conditional rules apply.
 * Exclusive states change parsing rules while the state is active.  When in
 * an exclusive state, only those rules defined for that state apply.
 *
 * We use exclusive states for quoted strings, extended comments,
 * and to eliminate parsing troubles for numeric strings.
 * Exclusive states:
 *  <xbit> bit string literal
 *  <xc> extended C-style comments - thomas 1997-07-12
 *  <xd> delimited identifiers (double-quoted identifiers) - thomas 1997-10-27
 *  <xh> hexadecimal numeric string - thomas 1997-11-16
 *  <xq> quoted strings - thomas 1997-07-30
 */

%x xbit
%x xc
%x xd
%x xh
%x xq

/* Bit string
 */
xbitstart		[bB]{quote}
xbitstop		{quote}
xbitinside		[^']*
xbitcat			{quote}{whitespace_with_newline}{quote}

/* Hexadecimal number
 */
xhstart			[xX]{quote}
xhstop			{quote}
xhinside		[^']+
xhcat			{quote}{whitespace_with_newline}{quote}

/* Extended quote
 * xqdouble implements SQL92 embedded quote
 * xqcat allows strings to cross input lines
 * Note: reduction of '' and \ sequences to output text is done in scanstr(),
 * not by rules here.  But we do get rid of xqcat sequences here.
 */
quote			'
xqstart			{quote}
xqstop			{quote}
xqdouble		{quote}{quote}
xqinside		[^\\']+
xqliteral		[\\](.|\n)
xqcat			{quote}{whitespace_with_newline}{quote}

/* Delimited quote
 * Allows embedded spaces and other special characters into identifiers.
 */
dquote			\"
xdstart			{dquote}
xdstop			{dquote}
xddouble		{dquote}{dquote}
xdinside		([^"]|\\\")+

/* C-style comments */
xcstart			\/\*
xcstop			\*+\/
xcinside		[^*/]+

digit			[0-9]

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

letter			[_A-Za-z]|{utf8Char}
letter_or_digit		[_A-Za-z0-9]|{utf8Char}

identifier		{letter}{letter_or_digit}*

typecast		"::"

/*
 * "self" is the set of chars that should be returned as single-character
 * tokens.  "op_chars" is the set of chars that can make up "Op" tokens,
 * which can be one or more characters long (but if a single-char token
 * appears in the "self" set, it is not to be returned as an Op).  Note
 * that the sets overlap, but each has some chars that are not in the other.
 *
 * If you change either set, adjust the character lists appearing in the
 * rule for "operator"!
self			[,()\[\].;$\:\+\-\*\/\%\^\<\>\=]
op_chars		[\~\!\@\#\^\&\|\`\?\$\+\-\*\/\%\<\>\=]
operator		{op_chars}+
 */

/* we no longer allow unary minus in numbers.
 * instead we pass it separately to parser. there it gets
 * coerced via doNegate() -- Leon aug 20 1999
 */

integer			{digit}+
decimal			(({digit}*\.{digit}+)|({digit}+\.{digit}*))
real			((({digit}*\.{digit}+)|({digit}+\.{digit}*)|({digit}+))([Ee][-+]?{digit}+))

/*
 * In order to make the world safe for Windows and Mac clients as well as
 * Unix ones, we accept either \n or \r as a newline.  A DOS-style \r\n
 * sequence will be seen as two successive newlines, but that doesn't cause
 * any problems.  SQL92-style comments, which start with -- and extend to the
 * next newline, are treated as equivalent to a single whitespace character.
 *
 * NOTE a fine point: if there is no newline following --, we will absorb
 * everything to the end of the input as a comment.  This is correct.  Older
 * versions of Postgres failed to recognize -- as a comment if the input
 * did not end with a newline.
 *
 * XXX perhaps \f (formfeed) should be treated as a newline as well?
 */

space			[ \t\n\r\f]
horiz_space		[ \t\f]
newline			[\n\r]
non_newline		[^\n\r]

comment			("--"{non_newline}*)

whitespace		({space}|{comment})

/*
 * SQL92 requires at least one newline in the whitespace separating
 * string literals that are to be concatenated.  Silly, but who are we
 * to argue?  Note that {whitespace_with_newline} should not have * after
 * it, whereas {whitespace} should generally have a * after it...
 */

horiz_whitespace	({horiz_space}|{comment})
whitespace_with_newline	({horiz_whitespace}*{newline}{whitespace}*)

other			.

/* DO NOT PUT ANY COMMENTS IN THE FOLLOWING SECTION.
 * AT&T lex does not properly handle C-style comments in this second lex block.
 * So, put comments here. thomas - 1997-09-08
 *
 * Quoted strings must allow some special characters such as single-quote
 *  and newline.
 * Embedded single-quotes are implemented both in the SQL92-standard
 *  style of two adjacent single quotes "''" and in the Postgres/Java style
 *  of escaped-quote "\'".
 * Other embedded escaped characters are matched explicitly and the leading
 *  backslash is dropped from the string. - thomas 1997-09-24
 * Note that xcstart must appear before operator, as explained above!
 *  Also whitespace (comment) must appear before operator.
 */

%%
{whitespace}			{ /* ignore */ }

{xcstart}			{
					xcdepth = 0;
					BEGIN(xc);
				}

<xc>{xcstart}			{
					xcdepth++;
				}

<xc>{xcstop}			{
					if (xcdepth <= 0)
						BEGIN(INITIAL);
					else
						xcdepth--;
				}

<xc>{xcinside}			{ /* ignore */ }

<xc><<EOF>>			{ OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Unterminated /* comment"); }

{xbitstart}			{
					BEGIN(xbit);
					strbuffer.reset();
					//strbuffer += "b";
				}
				
<xbit>{xbitstop}		{
					BEGIN(INITIAL);
					const char* buf = strbuffer.c_str();
					if (buf[strspn(buf, "01") + 1] != '\0')
						OW_THROWCIMMSG(CIMException::INVALID_QUERY, format( "invalid bit string input: '%1'",
							 buf).c_str());
					RETURN_BUFF_VAL(BITCONST);
				}
				
<xh>{xhinside} |
<xbit>{xbitinside}		{
					strbuffer.append(owwqltext, owwqlleng);
				}
				
<xh>{xhcat} |
<xbit>{xbitcat}			{ /* ignore */ }

<xbit><<EOF>>			{ OW_THROWCIMMSG( CIMException::INVALID_QUERY, "unterminated bit string literal" ); }

{xhstart}			{
					BEGIN(xh);
					strbuffer.reset();
				}
				
<xh>{xhstop}			{
					long val;
					char* endptr;

					BEGIN(INITIAL);
					const char* literalbuf = strbuffer.c_str();
					val = strtol(literalbuf, &endptr, 16);
					if (*endptr != '\0' || errno == ERANGE)
						OW_THROWCIMMSG( CIMException::INVALID_QUERY, format( "Bad hexadecimal integer input '%1'",
							 literalbuf).c_str() );
					RETURN_BUFF_VAL( HEXCONST );
				}
				
<xh><<EOF>>			{ OW_THROWCIMMSG( CIMException::INVALID_QUERY, "Unterminated hexadecimal integer"); }

{xqstart}			{
					BEGIN(xq);
					strbuffer.reset();
				}
				
<xq>{xqstop}			{
					BEGIN(INITIAL);
					RETURN_BUFF_VAL(SCONST);
				}
				
<xq>{xqdouble}	|
<xq>{xqinside}	|
<xq>{xqliteral} 		{
					strbuffer.append(owwqltext, owwqlleng);
				}
				
<xq>{xqcat}			{ /* ignore */ }

<xq><<EOF>>			{ OW_THROWCIMMSG( CIMException::INVALID_QUERY, "Unterminated quoted string"); }


{xdstart}			{
					BEGIN(xd);
					strbuffer.reset();
				}
				
<xd>{xdstop}			{
					BEGIN(INITIAL);
//					if (strbuffer.length() == 0)
//						OW_THROWCIMMSG( CIMException::INVALID_QUERY, "zero-length delimited identifier");
					RETURN_BUFF_VAL(SCONST);
				}
				
<xd>{xddouble} 			{
					strbuffer.append(owwqltext, owwqlleng-1);
				}
				
<xd>{xdinside}			{
					strbuffer.append(owwqltext, owwqlleng);
				}
				
<xd><<EOF>>			{ OW_THROWCIMMSG( CIMException::INVALID_QUERY, "Unterminated quoted identifier"); }


"," { RETURN_VAL(COMMA); }
"(" { RETURN_VAL(LEFTPAREN); }
")" { RETURN_VAL(RIGHTPAREN); }
"[" { RETURN_VAL(LEFTBRACKET); }
"]" { RETURN_VAL(RIGHTBRACKET); }
"." { RETURN_VAL(PERIOD); }
";" { RETURN_VAL(SEMICOLON); }
":" { RETURN_VAL(COLON); }
"+" { RETURN_VAL(PLUS); }
"-" { RETURN_VAL(MINUS); }
"*" { RETURN_VAL(ASTERISK); }
"/" { RETURN_VAL(SOLIDUS); }
"%" { RETURN_VAL(PERCENT); }
"<" { RETURN_VAL(LESSTHAN); }
"<=" { RETURN_VAL(LESSTHANOREQUALS); }
">" { RETURN_VAL(GREATERTHAN); }
">=" { RETURN_VAL(GREATERTHANOREQUALS); }
"=" { RETURN_VAL(EQUALS); }
"<>" { RETURN_VAL(NOTEQUALS); }
"||" { RETURN_VAL(CONCATENATION); }
"!" { RETURN_VAL(NOT); }
"&&" { RETURN_VAL(AND); }
"&" { RETURN_VAL(BITAND); }
"|" { RETURN_VAL(BITOR); }
"<<" { RETURN_VAL(BITSHIFTLEFT); }
">>" { RETURN_VAL(BITSHIFTRIGHT); }
"~" { RETURN_VAL(BITINVERT); }

ALL { RETURN_VAL(ALL); }
AND { RETURN_VAL(AND); }
AS { RETURN_VAL(AS); }
ASC { RETURN_VAL(ASC); }
AT { RETURN_VAL(AT); }
BY { RETURN_VAL(BY); }
CROSS { RETURN_VAL(CROSS); }
CURRENT_DATE { RETURN_VAL(CURRENTDATE); }
CURRENT_TIME { RETURN_VAL(CURRENTTIME); }
CURRENT_TIMESTAMP { RETURN_VAL(CURRENTTIMESTAMP); }
CURRENT_USER { RETURN_VAL(CURRENTUSER); }
DAY { RETURN_VAL(DAYP); }
DEFAULT { RETURN_VAL(DEFAULT); }
DELETE { RETURN_VAL(DELETE); }
DESC { RETURN_VAL(DESC); }
DISTINCT { RETURN_VAL(DISTINCT); }
ESCAPE { RETURN_VAL(ESCAPE); }
EXTRACT { RETURN_VAL(EXTRACT); }
FALSE { RETURN_VAL(FALSEP); }
FOR { RETURN_VAL(FOR); }
FROM { RETURN_VAL(FROM); }
FULL { RETURN_VAL(FULL); }
GROUP { RETURN_VAL(GROUP); }
HAVING { RETURN_VAL(HAVING); }
HOUR { RETURN_VAL(HOURP); }
IN { RETURN_VAL(IN); }
INNER { RETURN_VAL(INNERP); }
INSERT { RETURN_VAL(INSERT); }
INTERVAL { RETURN_VAL(INTERVAL); }
INTO { RETURN_VAL(INTO); }
IS { RETURN_VAL(IS); }
ISA { RETURN_VAL(ISA); }
ISNULL { RETURN_VAL(ISNULL); }
JOIN { RETURN_VAL(JOIN); }
LEADING { RETURN_VAL(LEADING); }
LEFT { RETURN_VAL(LEFT); }
LIKE { RETURN_VAL(LIKE); }
MINUTE { RETURN_VAL(MINUTEP); }
MONTH { RETURN_VAL(MONTHP); }
NATIONAL { RETURN_VAL(NATIONAL); }
NATURAL { RETURN_VAL(NATURAL); }
NOT { RETURN_VAL(NOT); }
NOTNULL { RETURN_VAL(NOTNULL); }
NULL { RETURN_VAL(NULLP); }
ON { RETURN_VAL(ON); }
ONLY { RETURN_VAL(ONLY); }
OR { RETURN_VAL(OR); }
ORDER { RETURN_VAL(ORDER); }
OUTER { RETURN_VAL(OUTERP); }
POSITION { RETURN_VAL(POSITION); }
RIGHT { RETURN_VAL(RIGHT); }
SECOND { RETURN_VAL(SECONDP); }
SELECT { RETURN_VAL(SELECT); }
SESSION_USER { RETURN_VAL(SESSIONUSER); }
SET { RETURN_VAL(SET); }
SUBSTRING { RETURN_VAL(SUBSTRING); }
TIME { RETURN_VAL(TIME); }
TIMESTAMP { RETURN_VAL(TIMESTAMP); }
TIMEZONE_HOUR { RETURN_VAL(TIMEZONEHOUR); }
TIMEZONE_MINUTE { RETURN_VAL(TIMEZONEMINUTE); }
TRAILING { RETURN_VAL(TRAILING); }
TRIM { RETURN_VAL(TRIM); }
TRUE { RETURN_VAL(TRUEP); }
UNION { RETURN_VAL(UNION); }
UPDATE { RETURN_VAL(UPDATE); }
USER { RETURN_VAL(USER); }
USING { RETURN_VAL(USING); }
VALUES { RETURN_VAL(VALUES); }
WHERE { RETURN_VAL(WHERE); }
YEAR { RETURN_VAL(YEARP); }
ZONE { RETURN_VAL(ZONE); }


{integer}			{
					long val;
					char* endptr;

					errno = 0;
					val = strtol(static_cast<char *>(owwqltext), &endptr, 10);
					if (*endptr != '\0' || errno == ERANGE)
					{
						/* integer too large, treat it as a float */
						RETURN_STR(FCONST);
					}
					RETURN_STR(ICONST);
				}
				
{decimal}			{
					RETURN_STR(FCONST);
				}
				
{real}				{
					RETURN_STR(FCONST);
				}

{identifier}			{
					RETURN_STR(IDENT);
				}

{other}				{ return owwqltext[0]; }

%%

void
owwqlerror(const char *message)
{
	printf( "parser: %s at or near \"%s\"", message, owwqltext);
}

/*
 scanner_init:
	called by postgres before any actual parsing is done
*/
void
WQLscanner_init(void)
{
	/* it's important to set this to NULL
	   because input()/myinput() checks the non-nullness of parseCh
	   to know when to pass the string to lex/flex */
	parseCh = NULL;

	strbuffer.reset();
	
#if defined(FLEX_SCANNER)
	if (YY_CURRENT_BUFFER)
		owwql_flush_buffer(YY_CURRENT_BUFFER);
#endif /* FLEX_SCANNER */
	BEGIN INITIAL;
}
#if !defined(FLEX_SCANNER)

/* get lex input from a string instead of from stdin */
int
input()
{
	if (parseCh == NULL)
		parseCh = WQLParser::parserInput;
	if (*parseCh == '\0')
		return(0);
	else
		return(*parseCh++);
}

/* undo lex input from a string instead of from stdin */
void
unput(char c)
{
	if (parseCh == NULL)
		elog(FATAL, "Unput() failed.\n");
	else if (c != 0)
		*--parseCh = c;
}

#endif /* !defined(FLEX_SCANNER) */

#ifdef FLEX_SCANNER

/* input routine for flex to read input from a string instead of a file */
static int
myinput(char* buf, int max)
{
	int len;

	if (parseCh == NULL)
		parseCh = WQLImpl::parserInput;
	len = strlen(parseCh);		/* remaining data available */
	/* Note: this code used to think that flex wants a null-terminated
	 * string.  It does NOT, and returning 1 less character than it asks
	 * for will cause failure under the right boundary conditions.  So
	 * shut up and fill the buffer to the limit, you hear?
	 */
	if (len > max)
		len = max;
	if (len > 0)
		memcpy(buf, parseCh, len);
	parseCh += len;
	return len;

	(void)yy_fatal_error;

}


#endif /* FLEX_SCANNER */
