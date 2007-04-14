%{
/*******************************************************************************
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
*     * Neither the name of Quest Software, Inc., nor Novell, Inc., nor the
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
/**
* @author Kevin S. Van Horn
* @author Bart Whiteley
* @author Dan Nuffer
*/

#include "OW_config.h"
#include <cstring>
#include "OW_Array.hpp"
#include "OW_String.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_PrivilegeConfig.hpp"
#include <cassert>

using OpenWBEM::PrivilegeConfig::Privileges;
using OpenWBEM::PrivilegeConfig::PathPatterns;
using OpenWBEM::PrivilegeConfig::EnvironmentVariablePatterns;
using OpenWBEM::PrivilegeConfig::ExecPatterns;
using OpenWBEM::PrivilegeConfig::MonitoredUserExecPatterns;
using OpenWBEM::PrivilegeConfig::ExecArgsPatterns;
using OpenWBEM::PrivilegeConfig::MonitoredUserExecArgsPatterns;
using OpenWBEM::Array;
using OpenWBEM::String;
using OpenWBEM::PrivilegeConfig::ParseError;

namespace
{
	void addPattern(PathPatterns & pp, char * consumed_c_str);
	void addPattern(PathPatterns & pp1, PathPatterns & pp2, char * consumed_c_str);
	void addPattern(ExecPatterns & ep, char * consumed_exec_path, char * consumed_ident, EnvironmentVariablePatterns* consumedEnvVars);
	//void addPattern(ExecPatterns & ep, char * consumed_exec_path, char * consumed_ident);
	void addPattern(MonitoredUserExecPatterns & ep, char * consumed_exec_path, char * consumed_app_name, char * consumed_user_name, EnvironmentVariablePatterns* consumedEnvVars);
	//void addPattern(MonitoredUserExecPatterns & ep, char * consumed_exec_path, char * consumed_app_name, char * consumed_user_name);
	void addPattern(ExecArgsPatterns & ep, char * consumed_exec_path, Array<ExecArgsPatterns::Arg>* consumed_args, char * consumed_ident, EnvironmentVariablePatterns* consumedEnvVars);
	//void addPattern(ExecArgsPatterns & ep, char * consumed_exec_path, Array<ExecArgsPatterns::Arg>* consumed_args, char * consumed_ident);
	void addPattern(MonitoredUserExecArgsPatterns & ep, char * consumed_exec_path, Array<ExecArgsPatterns::Arg>* consumed_args, char * consumed_app_name, char * consumed_user_name, EnvironmentVariablePatterns* consumedEnvVars);
	//void addPattern(MonitoredUserExecArgsPatterns & ep, char * consumed_exec_path, Array<ExecArgsPatterns::Arg>* consumed_args, char * consumed_app_name, char * consumed_user_name);
	String makeNameOrPath(char * consumed_c_str);
	String makeString(char * consumedStr);
}

void openwbem_privconfig_error(
	YYLTYPE * p_loc, Privileges *, ParseError * p_err,
	openwbem_privconfig_Lexer *, char const * msg
);


#define YYDEBUG 1
%}

%defines
%debug
%name-prefix="openwbem_privconfig_"
%error-verbose
%locations
%pure-parser
%parse-param {Privileges * p_priv}
%parse-param {ParseError * p_err}
%parse-param {openwbem_privconfig_Lexer * p_lexer}
%lex-param   {openwbem_privconfig_Lexer * p_lexer}

%union
{
	char * s;
	::OpenWBEM::Array< ::OpenWBEM::PrivilegeConfig::ExecArgsPatterns::Arg>* ArgArray;
	::OpenWBEM::Array< ::OpenWBEM::String>* StringArray;
	::OpenWBEM::PrivilegeConfig::EnvironmentVariablePatterns* EnvironmentVariablePatterns;
}

%{
int yylex(YYSTYPE * lvalp, YYLTYPE * llocp, openwbem_privconfig_Lexer * lexerp);
%}

%token <s>  SPLAT NAME DIRPATH SUBTREE FILEPATH FPATHWC STRING_VALUE

// These do not have values
%token K_OPEN_R K_OPEN_W K_OPEN_RW K_OPEN_A K_READ_DIR K_READ_LINK
%token K_CHECK_PATH K_RENAME_FROM K_RENAME_TO K_RENAME_FROM_TO
%token K_UNLINK K_MONITORED_EXEC K_USER_EXEC K_UNPRIV_USER
%token K_MONITORED_EXEC_CHECK_ARGS K_USER_EXEC_CHECK_ARGS
%token K_MONITORED_USER_EXEC_CHECK_ARGS K_MONITORED_USER_EXEC
%token K_INCLUDE K_ALLOWED_ENVIRONMENT_VARIABLES
%token AT
%token SCANNER_ERROR

%type <s>   path_pattern exec_path_pattern user_name
%type <ArgArray>	exec_arg_list
%type <StringArray> include_args
%type <EnvironmentVariablePatterns> env_specification allowed_environment_variables_args

%destructor { delete [] $$; } path_pattern NAME DIRPATH SUBTREE FILEPATH FPATHWC SPLAT
%destructor { delete $$; } exec_arg_list
%destructor { delete $$; } include_args
%destructor { delete $$; } env_specification allowed_environment_variables_args

%%

config:
	/* empty */
|	config config_stmt
;

config_stmt: 
	K_OPEN_R '{' open_r_args '}'
|	K_OPEN_W '{' open_w_args '}'
|	K_OPEN_RW '{' open_rw_args '}'
|	K_OPEN_A '{' open_a_args '}'
|	K_READ_DIR '{' read_dir_args '}'
|	K_READ_LINK '{' read_link_args '}'
|	K_CHECK_PATH '{' check_path_args '}'
|	K_RENAME_FROM '{' rename_from_args '}'
|	K_RENAME_TO '{' rename_to_args '}'
|	K_RENAME_FROM_TO '{' rename_from_to_args '}'
|	K_UNLINK '{' unlink_args '}'
|	K_MONITORED_EXEC '{' monitored_exec_args '}'
|	K_MONITORED_USER_EXEC '{' monitored_user_exec_args '}'
|	K_USER_EXEC '{' user_exec_args '}'
|	K_MONITORED_EXEC_CHECK_ARGS '{' monitored_exec_check_args_args '}'
|	K_MONITORED_USER_EXEC_CHECK_ARGS '{' monitored_user_exec_check_args_args '}'
|	K_USER_EXEC_CHECK_ARGS '{' user_exec_check_args_args '}'
|	K_UNPRIV_USER '{' unpriv_user_arg '}'
|	K_INCLUDE '{' include_args '}'
	{
		// Process the list in reverse order because the include mechanism works on a stack (LIFO).
		for (OpenWBEM::StringArray::reverse_iterator i = $3->rbegin(); i != $3->rend(); ++i)
		{
			if (p_lexer->include(*i) != 0)
			{
				yyerror(&yylloc, p_priv, p_err, p_lexer, "Recursive include");
				return 1;
			}
		}
	}
;

open_r_args:
	/* empty */
|	open_r_args path_pattern { addPattern(p_priv->open_read, $2); }
;

open_w_args:
	/* empty */
|	open_w_args path_pattern { addPattern(p_priv->open_write, $2); }
;

open_rw_args:
	/* empty */
| 	open_rw_args path_pattern {
		addPattern(p_priv->open_read, p_priv->open_write, $2);
	}
;

open_a_args:
	/* empty */
|	open_a_args path_pattern { addPattern(p_priv->open_append, $2); }
;

read_dir_args:
	/* empty */
|	read_dir_args DIRPATH { p_priv->read_dir.addDir(makeNameOrPath($2)); }
|	read_dir_args SUBTREE {
		OpenWBEM::String s = makeNameOrPath($2);
		p_priv->read_dir.addSubtree(s.substring(0, s.lastIndexOf('/')));
	}
;

read_link_args:
	/* empty */
|	read_link_args path_pattern { addPattern(p_priv->read_link, $2); }
;

check_path_args:
	/* empty */
|	check_path_args path_pattern { addPattern(p_priv->check_path, $2); }
;

rename_from_args:
	/* empty */
|	rename_from_args path_pattern { addPattern(p_priv->rename_from, $2); }
;

rename_to_args:
	/* empty */
|	rename_to_args path_pattern { addPattern(p_priv->rename_to, $2); }
;

rename_from_to_args:
	/* empty */
|	rename_from_to_args path_pattern {
		addPattern(p_priv->rename_from, p_priv->rename_to, $2);
	}
;

unlink_args:
	/* empty */
|	unlink_args path_pattern { addPattern(p_priv->unlink, $2); }
;

monitored_exec_args:
	/* empty */
|	monitored_exec_args exec_path_pattern AT NAME env_specification {
		addPattern(p_priv->monitored_exec, $2, $4, $5);
	}
;

monitored_user_exec_args:
	/* empty */
|	monitored_user_exec_args exec_path_pattern AT NAME AT user_name env_specification {
		addPattern(p_priv->monitored_user_exec, $2, $4, $6, $7);
	}
;

user_exec_args:
	/* empty */
|	user_exec_args exec_path_pattern AT user_name env_specification {
		addPattern(p_priv->user_exec, $2, $4, $5);
	}
;

monitored_exec_check_args_args:
	/* empty */
|	monitored_exec_check_args_args exec_path_pattern exec_arg_list AT NAME env_specification {
		addPattern(p_priv->monitored_exec_check_args, $2, $3, $5, $6);
	}
;

monitored_user_exec_check_args_args:
	/* empty */
|	monitored_user_exec_check_args_args exec_path_pattern exec_arg_list AT NAME AT user_name env_specification {
		addPattern(p_priv->monitored_user_exec_check_args, $2, $3, $5, $7, $8);
	}
;

user_exec_check_args_args:
	/* empty */
|	user_exec_check_args_args exec_path_pattern exec_arg_list AT user_name env_specification {
		addPattern(p_priv->user_exec_check_args, $2, $3, $5, $6);
	}
;

user_name: 
  SPLAT    { $$ = $1; }
| NAME     { $$ = $1; }
; 

unpriv_user_arg: NAME { p_priv->unpriv_user = makeNameOrPath($1); }
;

include_args:
	/* empty */
	{ $$ = new Array<String>; }
|	include_args STRING_VALUE 
	{
		$1->push_back(makeString($2)); $$ = $1;
	}
;

path_pattern:
  FILEPATH { $$ = $1; }
| FPATHWC  { $$ = $1; }
| SUBTREE  { $$ = $1; }
;

exec_path_pattern:
  FILEPATH { $$ = $1; }
| FPATHWC  { $$ = $1; }
;

exec_arg_list:
	/* empty */ { $$ = new Array<ExecArgsPatterns::Arg>; }
| exec_arg_list path_pattern { $1->push_back(ExecArgsPatterns::Arg($2, ExecArgsPatterns::E_PATH_PATTERN_ARG)); $$ = $1; }
| exec_arg_list STRING_VALUE { $1->push_back(ExecArgsPatterns::Arg(makeString($2), ExecArgsPatterns::E_LITERAL_ARG)); $$ = $1; }
| exec_arg_list SPLAT { $1->push_back(ExecArgsPatterns::Arg($2, ExecArgsPatterns::E_ANYTHING_ARG)); $$ = $1; }
;

env_specification:
	/* empty */ { $$ = 0; }
|	K_ALLOWED_ENVIRONMENT_VARIABLES '{' allowed_environment_variables_args '}' { $$ = $3; }
;

allowed_environment_variables_args:
	/* empty */ { $$ = new EnvironmentVariablePatterns; }
|	allowed_environment_variables_args NAME                  { $1->addPattern($2, 0, EnvironmentVariablePatterns::E_ANYTHING_PATTERN); $$ = $1; }
|	allowed_environment_variables_args NAME '=' path_pattern { $1->addPattern($2, $4, EnvironmentVariablePatterns::E_PATH_PATTERN); $$ = $1; }
|	allowed_environment_variables_args NAME '=' STRING_VALUE { $1->addPattern($2, makeString($4).c_str(), EnvironmentVariablePatterns::E_LITERAL_PATTERN); $$ = $1; }
;


%%

namespace
{
	void addPattern(PathPatterns & pp, char * consumed_c_str)
	{
		OpenWBEM::AutoPtrVec<char> s(consumed_c_str);
		pp.addPattern(consumed_c_str);
	}

	void addPattern(PathPatterns & pp1, PathPatterns & pp2, char * consumed_c_str)
	{
		OpenWBEM::AutoPtrVec<char> s(consumed_c_str);
		pp1.addPattern(consumed_c_str);
		pp2.addPattern(consumed_c_str);
	}

	void addPattern(
		ExecPatterns & ep, char * consumed_exec_path, char * consumed_ident, EnvironmentVariablePatterns* consumedEnvVars)
	{
		OpenWBEM::AutoPtrVec<char> s(consumed_exec_path);
		String ident(makeNameOrPath(consumed_ident));
		if (consumedEnvVars)
		{
			ep.addPattern(consumed_exec_path, *consumedEnvVars, ident);
		}
		else
		{
			ep.addPattern(consumed_exec_path, EnvironmentVariablePatterns(), ident);
		}
	}

	void addPattern(
		MonitoredUserExecPatterns & ep, char * consumed_exec_path, char * consumed_app_name, char * consumed_user_name, EnvironmentVariablePatterns* consumedEnvVars)
	{
		OpenWBEM::AutoPtrVec<char> s(consumed_exec_path);
		String app_name(makeNameOrPath(consumed_app_name));
		String user_name(makeNameOrPath(consumed_user_name));
		if (consumedEnvVars)
		{
			ep.addPattern(consumed_exec_path, *consumedEnvVars, app_name, user_name);
		}
		else
		{
			ep.addPattern(consumed_exec_path, EnvironmentVariablePatterns(), app_name, user_name);
		}
	}

	void addPattern(ExecArgsPatterns & ep, char * consumed_exec_path, Array<ExecArgsPatterns::Arg>* consumed_args, char * consumed_ident, EnvironmentVariablePatterns* consumedEnvVars)
	{
		OpenWBEM::AutoPtrVec<char> s(consumed_exec_path);
		String ident(makeNameOrPath(consumed_ident));
		OpenWBEM::AutoPtr<Array<ExecArgsPatterns::Arg> > args(consumed_args);
		if (consumedEnvVars)
		{
			ep.addPattern(consumed_exec_path, *consumed_args, *consumedEnvVars, ident);
		}
		else
		{
			ep.addPattern(consumed_exec_path, *consumed_args, EnvironmentVariablePatterns(), ident);
		}
	}

	void addPattern(MonitoredUserExecArgsPatterns & ep, char * consumed_exec_path, Array<ExecArgsPatterns::Arg>* consumed_args, char * consumed_app_name, char * consumed_user_name, EnvironmentVariablePatterns* consumedEnvVars)
	{
		OpenWBEM::AutoPtrVec<char> s(consumed_exec_path);
		String app_name(makeNameOrPath(consumed_app_name));
		String user_name(makeNameOrPath(consumed_user_name));
		OpenWBEM::AutoPtr<Array<ExecArgsPatterns::Arg> > args(consumed_args);
		if (consumedEnvVars)
		{
			ep.addPattern(consumed_exec_path, *consumed_args, *consumedEnvVars, app_name, user_name);
		}
		else
		{
			ep.addPattern(consumed_exec_path, *consumed_args, EnvironmentVariablePatterns(), app_name, user_name);
		}
	}

	String makeNameOrPath(char * consumed_c_str)
	{
		OpenWBEM::AutoPtrVec<char> s(consumed_c_str);
		return OpenWBEM::PrivilegeConfig::unescapePath(consumed_c_str);
	}

	String makeString(char * consumedStr)
	{
		int len = strlen(consumedStr);
		assert(len >= 2);
		assert(consumedStr[0] == '"');
		assert(consumedStr[len-1] == '"');
		consumedStr[len-1] = 0;
		return OpenWBEM::PrivilegeConfig::unescapeString(consumedStr+1);
	}
}

void openwbem_privconfig_error(
	YYLTYPE * p_loc, Privileges *, ParseError * p_err,
	openwbem_privconfig_Lexer *, char const * msg
)
{
	p_err->message = String(msg);
	p_err->column = p_loc->first_column;
	p_err->line = p_loc->first_line;
}
