%option noyywrap
%option yyclass="openwbem_privconfig_Lexer"
%option prefix="openwbem_privconfig_"
%option never-interactive
%option c++

%{
/*******************************************************************************
* Copyright © 2002  Networks Associates Technology, Inc.  All rights reserved.
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* Copyright (C) 2006, Novell, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates, 
*       nor Quest Software, Inc., nor Novell, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#define  OW_PRIVILEGE_LEXER_LL
#include "OW_PrivilegeLexer.hpp"
#include "OW_SafeCString.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_PrivilegeConfig.hpp"

#include "OW_PrivilegeParser.h"


#define YY_USER_ACTION this->pre_action();

%}

ESCCH        \\[\\*?"]
PECH_NO_DOT  ([^/.[:cntrl:][:space:]\\*?"]|{ESCCH})
PECH         ({PECH_NO_DOT}|[.])
PATHELM      (\.{0,2}{PECH_NO_DOT}|\.\.\.){PECH}*
  /* any nonempty sequence of PECH chars, except sequences "." and ".." */
PATHSEP      [/]
AT           @
SPLAT        \*

DIRPATH      {PATHSEP}({PATHELM}{PATHSEP})*
SUBTREE      {DIRPATH}\*\*
NAME         {PATHELM}
FILEPATH     {DIRPATH}{NAME}
FPATHWC      {DIRPATH}{NAME}?\*

simpleEscape [abfnrtv'"?\\]
hexEscape ("x"|"X")[0-9a-fA-F]{1,2}
octalEscape [0-7]{1,3}

escapeSequence [\\]({simpleEscape}|{hexEscape}|{octalEscape})
sChar [^"\\\n\r]|{escapeSequence}

STRING_VALUE \"{sChar}*\"


%x NOKEYWORDS

%%

#.*$          	        ;
<NOKEYWORDS>#.*$        ;
[[:space:]]+            ;
<NOKEYWORDS>[[:space:]]+ ;

open_r                  { return K_OPEN_R; }
open_w                  { return K_OPEN_W; }
open_rw                 { return K_OPEN_RW; }
open_a                  { return K_OPEN_A; }
read_dir                { return K_READ_DIR; }
read_link               { return K_READ_LINK; }
check_path              { return K_CHECK_PATH; }
rename_from             { return K_RENAME_FROM; }
rename_to               { return K_RENAME_TO; }
rename_from_to          { return K_RENAME_FROM_TO; }
unlink                  { return K_UNLINK; }
monitored_exec          { return K_MONITORED_EXEC; }
monitored_user_exec     { return K_MONITORED_USER_EXEC; }
user_exec               { return K_USER_EXEC; }
monitored_exec_check_args          { return K_MONITORED_EXEC_CHECK_ARGS; }
monitored_user_exec_check_args     { return K_MONITORED_USER_EXEC_CHECK_ARGS; }
user_exec_check_args               { return K_USER_EXEC_CHECK_ARGS; }

unpriv_user             { return K_UNPRIV_USER; }

\{                      { BEGIN(NOKEYWORDS); return yytext[0]; }
.                       { return yytext[0]; }

<NOKEYWORDS>\}          { BEGIN(INITIAL); return yytext[0]; }
<NOKEYWORDS>{AT}			{ return AT; }
<NOKEYWORDS>{SPLAT}	   { m_has_value = true; return(SPLAT); }
<NOKEYWORDS>{DIRPATH}   { m_has_value = true; return(DIRPATH); }
<NOKEYWORDS>{SUBTREE}   { m_has_value = true; return(SUBTREE); }
<NOKEYWORDS>{NAME}      { m_has_value = true; return(NAME); }
<NOKEYWORDS>{FILEPATH}  { m_has_value = true; return(FILEPATH); }
<NOKEYWORDS>{FPATHWC}   { m_has_value = true; return(FPATHWC); }
<NOKEYWORDS>{STRING_VALUE}   { m_has_value = true; return(STRING_VALUE); }
<NOKEYWORDS>.           { return yytext[0]; }

%%

openwbem_privconfig_Lexer::openwbem_privconfig_Lexer(std::istream & arg_yyin)
: yyFlexLexer(&arg_yyin, 0),
  m_next_column(1),
  m_next_line(1)
{
}

void openwbem_privconfig_Lexer::pre_action()
{
	m_has_value = false;
	m_first_line = m_last_line = m_next_line;
	m_first_column = m_next_column;
	char c;
    for (std::size_t i = 0; (c = yytext[i]) != '\0'; ++i)
	{
		m_last_column = m_next_column;
		++m_next_column;
		if (c == '\n')
		{
			m_last_line = m_next_line;
			++m_next_line;
			m_next_column = 1;
		}
	}
}

void openwbem_privconfig_Lexer::get_location(YYLTYPE & loc) const
{
	loc.first_column = m_first_column;
	loc.first_line = m_first_line;
	loc.last_column = m_last_column;
	loc.last_line = m_last_line;
}

void openwbem_privconfig_Lexer::LexerOutput(char const *, int)
{
	// Do nothing
}

namespace
{
	struct LexerException
	{
	};
}

void openwbem_privconfig_Lexer::LexerError(char const * msg)
{
	throw LexerException();
}

int openwbem_privconfig_lex(
	YYSTYPE * lvalp, YYLTYPE * llocp, openwbem_privconfig_Lexer * lexerp)
{
	try {
		//lexerp->set_debug(1);
		int retval = lexerp->yylex();
		if (retval)
		{
			lexerp->get_location(*llocp);
			lvalp->s = 0;
			if (lexerp->has_value())
			{
				lvalp->s = OpenWBEM::SafeCString::str_dup(lexerp->YYText());
			}
		}
		return retval;
	}
	catch (LexerException &)
	{
		// Don''t let exception propagate out, as Bison code that calls this
		// function is not exception-safe at all.
		lvalp->s = 0;
		lexerp->get_location(*llocp);
		return SCANNER_ERROR;
	}
}
