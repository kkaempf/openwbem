#ifndef OW_PRIVILEGE_CONFIG_HPP_HEADER_GUARD_
#define OW_PRIVILEGE_CONFIG_HPP_HEADER_GUARD_

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
#include "blocxx/Array.hpp"
#include "blocxx/String.hpp"
#include "OW_PrivilegeLexer.hpp"
#include "blocxx/Exception.hpp"
#include <set>
#include <map>
#include <vector>

/**
 * @author Kevin Van Horn
 * @author Bart Whiteley
 */

namespace OW_NAMESPACE
{
namespace PrivilegeConfig
{

String unescapePath(char const * epath);

BLOCXX_DECLARE_EXCEPTION(UnescapeString);
/**
 * Unescape C-style escape sequences as accepted in the privilege files.
 * @param str The string to unescape. This string must contain valid escape sequences (as allowed by the lexer)
 * 
 * @return String The unescaped string.
 * 
 * @throws UnescapeStringException if an invalid escape sequence is encountered.
 */
String unescapeString(char const * str);

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
	void addPattern(char const * pattern);

	// RETURNS: does path match any of the patterns specified with
	// add_pattern?
	//
	bool match(const String& path) const;

private:
	// Add another matching case: any absolute path that has dir_path as
	// its directory, and whose file name either begins with fname_prefix
	// (if can_extend) or is equal to fname_prefix (if !can_extend).
	// REQUIRE: dir_path starts and ends with '/'.
	// REQUIRE: fname_prefix has no '/' characters.
	//
	void addCase(
		const String& dir_path, const String& fname_prefix, bool can_extend);

	// Add another matching case: any absolute path that is in the subtree
	// rooted at dir_path.
	// REQUIRE: dir_path starts and ends with '/'.
	void addSubtree(const String& dir_path);

	struct FileNameCase
	{
		String prefix;
		bool can_extend;
	};
	typedef std::vector<FileNameCase> dir_cases_t;
	typedef std::map<String, dir_cases_t> dir_map_t;
	dir_map_t m_map;
	std::vector<String> m_subtrees;
};

class EnvironmentVariablePatterns
{
public:
	enum EPatternType
	{
		E_PATH_PATTERN,
		E_LITERAL_PATTERN,
		E_ANYTHING_PATTERN
	};
	void addPattern(const char* name, const char* pattern, EPatternType type);
	bool match(const StringArray& env) const;

private:
	struct Pattern
	{
		Pattern(const String& pattern, EPatternType type)
		: m_pattern(pattern)
		, m_type(type)
		{
		}

		String m_pattern;
		EPatternType m_type;
	};
	typedef std::map<String, std::vector<Pattern> > PatternMap_t;
	PatternMap_t m_patterns;

	bool matchPatterns(const String& var, const String& val) const;
};

class ExecPatterns
{
public:
	void addPattern(char const * execPathPattern, const EnvironmentVariablePatterns& environmentVariablePatterns, const String& ident)
	{
		m[ident].pathPatterns.addPattern(execPathPattern);
		m[ident].environmentVariablePatterns = environmentVariablePatterns;
	}

	bool match(const String& execPath, const StringArray& envVars, const String& ident) const
	{
		map_t::const_iterator it = m.find(ident);
		bool rv = it != m.end() && it->second.pathPatterns.match(execPath) && it->second.environmentVariablePatterns.match(envVars);
		if (!rv)
		{
			it = m.find("*"); 
			rv = it != m.end() && it->second.pathPatterns.match(execPath) && it->second.environmentVariablePatterns.match(envVars);
		}
		return rv; 
	}

private:
	// Key is the user (for user_exec) or app name (for monitored exec)
	struct Pattern
	{
		PathPatterns pathPatterns;
		EnvironmentVariablePatterns environmentVariablePatterns;
	};
	typedef std::map<String, Pattern> map_t;
	map_t m;
};

class MonitoredUserExecPatterns
{
public:
	void addPattern(char const * execPathPattern, const EnvironmentVariablePatterns& envVarPatterns, const String& ident, const String& userName)
	{
		m[userName].addPattern(execPathPattern, envVarPatterns, ident); 
	}
	bool match(const String& execPath, const StringArray& envVars, const String& ident, const String& userName) const
	{
		map_t::const_iterator it = m.find(userName); 
		bool rv = it != m.end() && it->second.match(execPath, envVars, ident); 
		if (!rv)
		{
			it = m.find("*"); 
			rv = it != m.end() && it->second.match(execPath, envVars, ident); 
		}
		return rv; 
	}
private: 
	// Key is the userName
	typedef std::map<String, ExecPatterns> map_t; 
	map_t m; 
}; 

class ExecArgsPatterns
{
public:
	enum EArgType
	{
		E_PATH_PATTERN_ARG,
		E_LITERAL_ARG,
		E_ANYTHING_ARG // Not as useful as a regular expression, but this will allow a single argument to contain any value.
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

	void addPattern(char const * execPathPattern, const Array<Arg>& args, const EnvironmentVariablePatterns& envVarPatterns, const String& ident);

	bool match(const String& execPath, Array<String> const & args, const StringArray& envVars, const String& ident) const;

private:
	struct Pattern
	{
		Pattern(const PathPatterns& pathPatterns_, const Array<Arg>& args_, const EnvironmentVariablePatterns& environmentVariablePatterns_)
		: pathPatterns(pathPatterns_)
		, args(args_)
		, environmentVariablePatterns(environmentVariablePatterns_)
		{
		}

		PathPatterns pathPatterns;
		Array<Arg> args;
		EnvironmentVariablePatterns environmentVariablePatterns;
	};
	// Key is the user (for user_exec) or app name (for monitored exec)
	// Value is an Array with one entry for each executable and args
	typedef std::map<String, Array<Pattern> > map_t;
	map_t m;
};

class MonitoredUserExecArgsPatterns
{
public: 
	void addPattern(char const * execPathPattern, const Array<ExecArgsPatterns::Arg>& args, const EnvironmentVariablePatterns& envVarPatterns, const String& ident, const String& userName)
	{
		m[userName].addPattern(execPathPattern, args, envVarPatterns, ident);
	}
	bool match(const String& execPath, Array<String> const & args, const StringArray& envVars, const String& ident, const String& userName) const
	{
		map_t::const_iterator it = m.find(userName); 
		bool rv = it != m.end() && it->second.match(execPath, args, envVars, ident); 
		if (!rv)
		{
			it = m.find("*"); 
			rv = it != m.end() && it->second.match(execPath, args, envVars, ident); 
		}
		return rv; 
	}
private: 
	// Key is userName
	typedef std::map<String, ExecArgsPatterns> map_t; 
	map_t m; 
}; 

class DirPatterns
{
public:
	void addDir(const String& dirpath);
	void addSubtree(const String& dirpath);
	bool match(const String& dirpath) const;
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
	MonitoredUserExecArgsPatterns monitored_user_exec_check_args;
	MonitoredUserExecPatterns monitored_user_exec;
};

struct ParseError
{
	ParseError()
		: column(0)
		, line(0)
	{
	}

	String message;
	// column and line for start of token where error detected
	unsigned column;
	unsigned line;
};

class IncludeHandler
{
public:
	virtual ~IncludeHandler();
	/**
     * Return a pointer to the data to be parsed for an include. The caller does not take ownership and will not free
     * the result.
     * @param includeParam The include parameter. e.g. include { "foo" } would cause foo to be passed.
	 * 
     * @return std::istream* Must not return NULL.
     * 
     * @throws An exception derived from Exception indicating the reason for failure.
	 */
	virtual std::istream* getInclude(const String& includeParam) = 0;

	/**
     * Called when the std::istream* from the last stacked call to getInclude() is no longer needed. The intention is
     * that the derived class can then free it.
	 */
	virtual void endInclude() = 0;
};



} // namespace PrivilegeConfig
} // namespace OW_NAMESPACE

int openwbem_privconfig_parse(
	OpenWBEM::PrivilegeConfig::Privileges * p_priv,
	OpenWBEM::PrivilegeConfig::ParseError * p_err,
	OpenWBEM::PrivilegeConfig::openwbem_privconfig_Lexer * p_lexer
);


#endif
