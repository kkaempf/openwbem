#ifndef OW_PRIVILEGE_CONFIG_HPP_HEADER_GUARD_
#define OW_PRIVILEGE_CONFIG_HPP_HEADER_GUARD_

/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates, nor Quest Software, Inc., nor the
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
#include "OW_Array.hpp"
#include "OW_String.hpp"
#include "OW_PrivilegeLexer.hpp"
#include <set>
#include <map>
#include <vector>

/**
 * @author ???
 */

namespace OW_NAMESPACE
{
namespace PrivilegeConfig
{

String unescape_path(char const * epath);

class PathPatterns
{
public:
	// No cases; nothing matches.
	//
	PathPatterns();

	// Converts escapes in pattern and then adds it to this object as another
	// pattern that matches.
	// REQUIRE: pattern matches the FILEPATH, FPATHWC, or FPATHRWC tokens.
	//
	void add_pattern(char const * pattern);

	// RETURNS: does path match any of the patterns specified with
	// add_pattern?
	//
	bool match(String const & path) const;

private:
	// Add another matching case: any absolute path that has dir_path as
	// its directory, and whose file name either begins with fname_prefix
	// (if can_extend) or is equal to fname_prefix (if !can_extend).
	// REQUIRE: dir_path starts and ends with '/'.
	// REQUIRE: fname_prefix has no '/' characters.
	//
	void add_case(
		String const & dir_path, String const & fname_prefix, bool can_extend);

	// Add another matching case: any absolute path that is in the subtree
	// rooted at dir_path.
	// REQUIRE: dir_path starts and ends with '/'.
	void add_subtree(String const & dir_path);

	struct FileNameCase
	{
		String prefix;
		bool can_extend;
	};
	typedef std::vector<FileNameCase> dir_cases_t;
	typedef std::map<String, dir_cases_t> dir_map_t;
	dir_map_t m_map;
	std::vector<String> subtrees;
};

class ExecPatterns
{
public:
	void add_pattern(char const * exec_path_pattern, String const & ident)
	{
		m[ident].add_pattern(exec_path_pattern);
	}

	bool match(String const & exec_path, String const & ident) const
	{
		map_t::const_iterator it = m.find(ident);
		return it != m.end() && it->second.match(exec_path);
	}

private:
	// Key is the user (for user_exec) or app name (for monitored exec)
	typedef std::map<String, PathPatterns> map_t;
	map_t m;
};

class ExecArgsPatterns
{
public:
	enum EArgType
	{
		E_PATH_PATTERN_ARG,
		E_LITERAL_ARG
	};
	struct Arg
	{
		Arg(const String& arg_, EArgType argType_)
			: arg(arg_)
			, argType(argType_)
		{
		}
		String arg;
		EArgType argType;
	};

	void add_pattern(char const * exec_path_pattern, const Array<Arg>& args, String const & ident);

	bool match(String const & exec_path, Array<String> const & args, String const & ident) const;

private:
	// Key is the user (for user_exec) or app name (for monitored exec)
	// Value is an Array with one entry for each executable and args
	typedef std::map<String, Array<std::pair<PathPatterns, Array<Arg> > > > map_t;
	map_t m;
};

class DirPatterns
{
public:
	void add_dir(String const & dirpath);
	void add_subtree(String const & dirpath);
	bool match(String const & dirpath) const;
private:
	std::set<String> dirs;
	std::vector<String> subtrees;
};

struct Privileges
{
	String unpriv_user;

	PathPatterns open_read;
	PathPatterns open_write;
	PathPatterns open_append;
	DirPatterns read_dir;
	PathPatterns read_link;
	PathPatterns check_path;
	PathPatterns rename_from;
	PathPatterns rename_to;
	PathPatterns unlink;
	ExecPatterns monitored_exec;
	ExecPatterns user_exec;
	ExecArgsPatterns monitored_exec_check_args;
	ExecArgsPatterns user_exec_check_args;
};

struct ParseError
{
	String message;
	// column and line for start of token where error detected
	unsigned column;
	unsigned line;
};

} // namespace PrivilegeConfig
} // namespace OW_NAMESPACE

int openwbem_privconfig_parse(
	OpenWBEM::PrivilegeConfig::Privileges * p_priv,
	OpenWBEM::PrivilegeConfig::ParseError * p_err,
	openwbem_privconfig_Lexer * p_lexer
);


#endif
