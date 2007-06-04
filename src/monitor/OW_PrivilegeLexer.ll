%option noyywrap
%option prefix="openwbem_privconfig_"
%option never-interactive
%option warn

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

#include "OW_config.h"
#include "OW_PrivilegeLexer.hpp"
#include "blocxx/SafeCString.hpp"
#include "blocxx/String.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/Exception.hpp"
#include "OW_PrivilegeConfig.hpp"

#include "OW_PrivilegeParser.h"
#include <vector>
#include <iterator>

/* Avoid exit() on fatal scanner errors (a bit ugly -- see yy_fatal_error) */

namespace OW_NAMESPACE
{
BLOCXX_DECLARE_EXCEPTION(PrivilegeLexer)
BLOCXX_DEFINE_EXCEPTION(PrivilegeLexer)
}
#define YY_FATAL_ERROR(msg) \
	BLOCXX_THROW(OpenWBEM::PrivilegeLexerException, msg);

#define YY_DECL static int openwbem_privconfig_lex_impl(YYSTYPE * lvalp, YYLTYPE * llocp, OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer * lexerp)
#define YYLTYPE openwbem_privconfig_yyltype

#define YY_USER_ACTION lexerp->pre_action();

#define RETURN_VAL(x) lvalp->s = 0; return(x);
#define RETURN_STR(x) lvalp->s = OpenWBEM::SafeCString::str_dup(yytext); return(x);

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

%x ARGSECTION ENVVARSECTION

%%

#.*$          	            ;
<ARGSECTION>#.*$            ;
<ENVVARSECTION>#.*$         ;
[[:space:]]+                ;
<ARGSECTION>[[:space:]]+    ;
<ENVVARSECTION>[[:space:]]+ ;

open_r                  { RETURN_VAL(K_OPEN_R); }
open_w                  { RETURN_VAL(K_OPEN_W); }
open_rw                 { RETURN_VAL(K_OPEN_RW); }
open_a                  { RETURN_VAL(K_OPEN_A); }
read_dir                { RETURN_VAL(K_READ_DIR); }
read_link               { RETURN_VAL(K_READ_LINK); }
check_path              { RETURN_VAL(K_CHECK_PATH); }
rename_from             { RETURN_VAL(K_RENAME_FROM); }
rename_to               { RETURN_VAL(K_RENAME_TO); }
rename_from_to          { RETURN_VAL(K_RENAME_FROM_TO); }
unlink                  { RETURN_VAL(K_UNLINK); }
monitored_exec          { RETURN_VAL(K_MONITORED_EXEC); }
monitored_user_exec     { RETURN_VAL(K_MONITORED_USER_EXEC); }
user_exec               { RETURN_VAL(K_USER_EXEC); }
monitored_exec_check_args          { RETURN_VAL(K_MONITORED_EXEC_CHECK_ARGS); }
monitored_user_exec_check_args     { RETURN_VAL(K_MONITORED_USER_EXEC_CHECK_ARGS); }
user_exec_check_args               { RETURN_VAL(K_USER_EXEC_CHECK_ARGS); }

unpriv_user             { RETURN_VAL(K_UNPRIV_USER); }
include			{ RETURN_VAL(K_INCLUDE); }

\{                      { BEGIN(ARGSECTION); RETURN_VAL(yytext[0]); }
.                       { RETURN_VAL(yytext[0]); }

<ARGSECTION>\}          { BEGIN(INITIAL); RETURN_VAL(yytext[0]); }
<ARGSECTION>{AT}        { RETURN_VAL(AT); }
<ARGSECTION>{SPLAT}     { RETURN_STR(SPLAT); }
<ARGSECTION>allowed_environment_variables { BEGIN(ENVVARSECTION); RETURN_VAL(K_ALLOWED_ENVIRONMENT_VARIABLES); }
<ARGSECTION>{DIRPATH}   { RETURN_STR(DIRPATH); }
<ARGSECTION>{SUBTREE}   { RETURN_STR(SUBTREE); }
<ARGSECTION>{NAME}      { RETURN_STR(NAME); }
<ARGSECTION>{FILEPATH}  { RETURN_STR(FILEPATH); }
<ARGSECTION>{FPATHWC}   { RETURN_STR(FPATHWC); }
<ARGSECTION>{STRING_VALUE}   { RETURN_STR(STRING_VALUE); }
<ARGSECTION>.           { RETURN_VAL(yytext[0]); }

<ENVVARSECTION>\{		{ RETURN_VAL(yytext[0]); }
<ENVVARSECTION>\}		{ BEGIN(ARGSECTION); RETURN_VAL(yytext[0]); }
<ENVVARSECTION>=                { RETURN_VAL(yytext[0]); }
<ENVVARSECTION>{DIRPATH}        { RETURN_STR(DIRPATH); }
<ENVVARSECTION>{SUBTREE}        { RETURN_STR(SUBTREE); }
<ENVVARSECTION>{NAME}           { RETURN_STR(NAME); }
<ENVVARSECTION>{FILEPATH}       { RETURN_STR(FILEPATH); }
<ENVVARSECTION>{FPATHWC}        { RETURN_STR(FPATHWC); }
<ENVVARSECTION>{STRING_VALUE}   { RETURN_STR(STRING_VALUE); }
<ENVVARSECTION>.                { RETURN_VAL(yytext[0]); }

<<EOF>> {
	if (!lexerp->endInclude())
	{
		yyterminate();
	}
}


%%

OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer::openwbem_privconfig_Lexer(std::istream & arg_yyin, OpenWBEM::PrivilegeConfig::IncludeHandler& includeHandler, const OpenWBEM::String& bufferName)
: m_includeHandler(&includeHandler)
, m_has_value(false)
, m_first_column(0)
, m_first_line(0)
, m_last_column(0)
, m_last_line(0)
, m_next_column(1)
, m_next_line(1)
, m_bufferName(bufferName)
{
	if (s_instanceCount == 1)
	{
		BLOCXX_THROW(OpenWBEM::PrivilegeLexerException, "Error, only one instance of openwbem_privconfig_Lexer can exist at a time");
	}
	++s_instanceCount;

	std::vector<char> data = std::vector<char>(std::istreambuf_iterator<char>(arg_yyin.rdbuf()), std::istreambuf_iterator<char>());
	if (data.size() > 0)
	{
		yy_switch_to_buffer(yy_scan_bytes(&data[0], data.size()));
	}
	else
	{
		yy_switch_to_buffer(yy_scan_string(""));
	}
}

OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer::~openwbem_privconfig_Lexer()
{
	--s_instanceCount;
}



void OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer::pre_action()
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

void OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer::get_location(YYLTYPE & loc) const
{
	loc.first_column = m_first_column;
	loc.first_line = m_first_line;
	loc.last_column = m_last_column;
	loc.last_line = m_last_line;
}

int OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer::include(const OpenWBEM::String& includeParam)
{
	// report an error if the include is recursive
	for (std::deque<LexerState>::const_iterator i = m_includeStack.begin(); i != m_includeStack.end(); ++i)
	{
		if (i->m_bufferName == includeParam)
		{
			return 1;
		}
	}

	// first, get the new stream
	if (!m_includeHandler)
	{
		BLOCXX_THROW(OpenWBEM::PrivilegeLexerException, "include not supported");
	}

	std::istream* newistr = m_includeHandler->getInclude(includeParam);

	// save the current state into the include stack

	m_includeStack.push_back(LexerState());

	m_includeStack.back().m_has_value = m_has_value;
	m_includeStack.back().m_first_column = m_first_column;
	m_includeStack.back().m_first_line = m_first_line;
	m_includeStack.back().m_last_column = m_last_column;
	m_includeStack.back().m_last_line = m_last_line;
	m_includeStack.back().m_next_column = m_next_column;
	m_includeStack.back().m_next_line = m_next_line;
	m_includeStack.back().m_bufferState = YY_CURRENT_BUFFER;
	m_includeStack.back().m_bufferName = m_bufferName;

	// set up new state
	m_has_value = false;
	m_first_column = 0;
	m_first_line = 0;
	m_last_column = 0;
	m_last_line = 0;
	m_next_column = 1;
	m_next_line = 1;
	m_bufferName = includeParam;

	std::vector<char> data = std::vector<char>(std::istreambuf_iterator<char>(newistr->rdbuf()), std::istreambuf_iterator<char>());
	if (data.size() > 0)
	{
		yy_switch_to_buffer(yy_scan_bytes(&data[0], data.size()));
	}
	else
	{
		yy_switch_to_buffer(yy_scan_string(""));
	}

	return 0;
}

bool OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer::endInclude()
{
	if (m_includeStack.size() == 0)
	{
		return false;
	}

	yy_delete_buffer( YY_CURRENT_BUFFER );
	m_has_value = m_includeStack.back().m_has_value;
	m_first_column = m_includeStack.back().m_first_column;
	m_first_line = m_includeStack.back().m_first_line;
	m_last_column = m_includeStack.back().m_last_column;
	m_last_line = m_includeStack.back().m_last_line;
	m_next_column = m_includeStack.back().m_next_column;
	m_next_line = m_includeStack.back().m_next_line;
	m_bufferName = m_includeStack.back().m_bufferName;

	yy_switch_to_buffer(m_includeStack.back().m_bufferState);

	m_includeStack.pop_back();
	m_includeHandler->endInclude();

	return true;
}

// static
int OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer::s_instanceCount = 0;


int openwbem_privconfig_lex(YYSTYPE * lvalp, YYLTYPE * llocp, OW_NAMESPACE::PrivilegeConfig::openwbem_privconfig_Lexer * lexerp)
{
	try {
		//lexerp->set_debug(1);
		int retval = openwbem_privconfig_lex_impl(lvalp, llocp, lexerp);
		if (retval)
		{
			lexerp->get_location(*llocp);
		}
		return retval;
	}
	catch (OpenWBEM::PrivilegeLexerException &)
	{
		// Don''t let exception propagate out, as Bison code that calls this
		// function is not exception-safe at all.
		lvalp->s = 0;
		lexerp->get_location(*llocp);
		return SCANNER_ERROR;
	}
}




