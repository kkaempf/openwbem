/*******************************************************************************
* Copyright (C) 2005, Vintela, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates, nor Vintela, Inc., nor the
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

#include "OW_PrivilegeConfig.hpp"
#include "OW_Assertion.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_Format.hpp"
#include "OW_Secure.hpp"
#include "OW_Environ.hpp"
#include "blocxx/LazyGlobal.hpp"
#include "blocxx/SortedVectorMap.hpp"

#include <iterator> // for std::back_insert_iterator
#include <algorithm> // for std::remove_copy_if
#include <numeric> // for std::accumulate
#include <climits> // for CHAR_MAX

namespace OW_NAMESPACE
{
namespace PrivilegeConfig
{

OW_DEFINE_EXCEPTION(UnescapeString)

namespace
{
	String unescapePath(char const * s, std::size_t n)
	{
		StringBuffer sbuf;
		for (std::size_t i = 0; i < n; ++i)
		{
			char c = s[i];
			if (c == '\\')
			{
				++i;
				OW_ASSERT(i < n);
				sbuf += s[i];
			}
			else
			{
				sbuf += c;
			}
		}
		return sbuf.releaseString();
	}

	bool containsDoubleDots(const String& path)
	{
		if((path.indexOf("/../") != String::npos) || path.endsWith("/.."))
		{
			return true;
		}
		return false;
	}

	// REQUIRE: path contains at least one '/'
	//
	std::pair<String, String> splitPath(const String& path)
	{
		std::size_t n = path.lastIndexOf('/');
		return std::make_pair(path.substring(0, n + 1), path.substring(n + 1));
	}

	enum EPatternsType { // numeric assignments are significant (convertPattern)
		E_PATH = 0, E_WILDCARD = 1, E_SUBTREE = 2
	};

	std::pair<String, EPatternsType> convertPattern(char const * s)
	{
		std::size_t n = std::strlen(s);
		OW_ASSERT(n > 0);
		OW_ASSERT(s[0] == '/');
		EPatternsType pattern_type = (
			s[n-1] == '*' ? // then n > 1, since s[0] == '/'
				(s[n-2] == '*' ? // then n > 2, since s[0] == '/'
					(s[n-3] == '\\' ? E_WILDCARD : E_SUBTREE) :
					(s[n-2] == '\\' ? E_PATH : E_WILDCARD)) :
			E_PATH
		);
		n -= std::size_t(pattern_type);
		return std::make_pair(normalizePath(unescapePath(s, n)), pattern_type);
	}

	bool isOctal(int c)
	{
		switch (c)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				return true;
			default:
				return false;
		}
	}

	// A helper function that returns if a path token is useless.
	// Useless tokens are empty or single dots.
	bool isUselessPathToken(const String& tok)
	{
		return tok.empty() || (tok == ".");
	}

	// Append a path to a base path.
	String pathAppender(const String& base, const String& name)
	{
		return base + BLOCXX_FILENAME_SEPARATOR + name;
	}
} // end unnamed namespace

String normalizePath(const String& path)
{
	// There are more efficient ways this function could be implemented.  This
	// way was easy to implement and should be fairly easy to read.

	// TODO: Normalize windows paths.  That is much trickier because "\\\\"
	// has a special meaning and absolute paths start with drive letters.
	if( !path.startsWith('/') )
	{
		// ERROR: non-absolute paths are not accepted.
		return path;
	}
	StringArray pathPortions = path.tokenize("/", String::E_DISCARD_DELIMITERS, String::E_RETURN_EMPTY_TOKENS);

	// Preallocate enough space for the entire path, should it be good.
	StringArray adjustedPathTokens;
	adjustedPathTokens.reserve(pathPortions.size());

	// Useless tokens (from // and /./) are removed from the final string;
	// everything else is copied.  This is begging for a lambda.
	std::remove_copy_if(pathPortions.begin(), pathPortions.end(),
		std::back_insert_iterator<StringArray>(adjustedPathTokens),
		&isUselessPathToken);

	// Combine the adjusted path tokens into a path string.  This is another
	// spot where lambda would be very useful.
	String retval = std::accumulate(adjustedPathTokens.begin(), adjustedPathTokens.end(),
		String(), &pathAppender);

	// The original was non-empty because it had at least a single slash.  If it
	// is now empty, it was nothing but /(\./)* (in regex terms).
	if( retval.empty() )
	{
		retval = "/";
	}

	// If the original path ends with a '/', or stripping the dots would make it
	// so, make sure the adjusted path ends the same way; the result would need
	// to be a directory and not a file.  This should preserve symlink semantics
	// for trailing dots even after stripping the dots.
	if( path.endsWith('/') || path.endsWith("/.") )
	{
		if( !retval.endsWith('/') )
		{
			retval.concat('/');
		}
	}

	return retval;
}


String unescapePath(char const * epath)
{
	return unescapePath(epath, std::strlen(epath));
}

String unescapeString(char const * str)
{
	StringBuffer unescaped;
	size_t len = strlen(str);
	for (size_t i = 0; i < len; ++i)
	{
		if (str[i] == '\\')
		{
			++i;

			/* this can never happen, unless someone messes up the lexer */
			OW_ASSERT(i < len);

			switch (str[i])
			{
				// simple escapes
				case 'a':
					unescaped += '\a';
					break;
				case 'b':
					unescaped += '\b';
					break;
				case 'f':
					unescaped += '\f';
					break;
				case 'n':
					unescaped += '\n';
					break;
				case 'r':
					unescaped += '\r';
					break;
				case 't':
					unescaped += '\t';
					break;
				case 'v':
					unescaped += '\v';
					break;
				case '\'':
					unescaped += '\'';
					break;
				case '"':
					unescaped += '"';
					break;
				case '\?':
					unescaped += '\?';
					break;
				case '\\':
					unescaped += '\\';
					break;
				// hex escapes
				case 'x':
				case 'X':
					{
						++i;
						// The lexer guarantees that there will be from 1-2 hex chars.
						UInt16 hex = 0;
						size_t j = 0;
						for (; j < 2; ++j)
						{
							char c = str[i+j];
							if (isdigit(c))
							{
								hex <<= 4;
								hex |= c - '0';
							}
							else if (isxdigit(c))
							{
								c = toupper(c);
								hex <<= 4;
								hex |= c - 'A' + 0xA;
							}
							else
							{
								break;
							}
						}
						if (hex > CHAR_MAX)
						{
							OW_THROW(UnescapeStringException, Format("Escape sequence (%1) larger than supported maximum (%2)", String(&str[i-2], j+2), int(CHAR_MAX)).c_str());
						}
						unescaped += static_cast<char>(hex);
						i += j - 1;
					}
					break;
				// octal escapes
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					{
						// The lexer guarantees that there will be from 1-3 oct chars.
						UInt16 oct = 0;
						size_t j = 0;
						for (; j < 3; ++j)
						{
							char c = str[i+j];
							if (isOctal(c))
							{
								oct <<= 3;
								oct |= c - '0';
							}
							else
							{
								break;
							}
						}
						if (oct > CHAR_MAX)
						{
							OW_THROW(UnescapeStringException, Format("Escape sequence (%1) larger than supported maximum (%2)", String(&str[i-1], j+1), int(CHAR_MAX)).c_str());
						}
						unescaped += static_cast<char>(oct);
						i += j - 1;
					}
					break;
				default:
					// this could never happen unless someone messes up the lexer
					OW_ASSERTMSG(0, "Invalid escape sequence");
					break;
			}
		}
		else
		{
			unescaped += str[i];
		}
	}

	return unescaped.releaseString();
}

PathPatterns::PathPatterns()
{
}

void PathPatterns::addCase(
	const String& dir_path, const String& fname_prefix, bool can_extend)
{
	FileNameCase c;
	c.prefix = fname_prefix;
	c.can_extend = can_extend;
	m_map[dir_path].push_back(c);
}

void PathPatterns::addSubtree(const String& dir_path)
{
	m_subtrees.push_back(normalizePath(dir_path));
}

void PathPatterns::addPattern(char const * pattern)
{
	std::pair<String, EPatternsType> x = convertPattern(pattern);
	const String& path = x.first;
	OW_ASSERT(path.startsWith("/"));
	EPatternsType pat_type = x.second;
	if (pat_type == E_SUBTREE) {
		OW_ASSERT(path.endsWith("/"));
		this->addSubtree(path);
	}
	else {
		bool ext = (pat_type == E_WILDCARD);
		std::pair<String, String> dir_and_name = splitPath(path);
		this->addCase(dir_and_name.first, dir_and_name.second, ext);
	}
}

bool PathPatterns::match(const String& path) const
{
	if ( !path.startsWith('/') )
	{
		// Relative paths are rejected.
		return false;
	}
	else if( containsDoubleDots(path) )
	{
		// Always reject /../ or /..
		return false;
	}

	String cleaned = normalizePath(path);

	if(cleaned.endsWith("/") && (cleaned != "/"))
	{
		// "/" is a special case that is allowed.  Any other path that ends with
		// a trailing slash will be rejected.
		return false;
	}
	std::pair<String, String> x = splitPath(cleaned);
	const String& fname = x.second;
	const String& dirname = x.first;
	// First, see if any subtree pattern is matched
	for (size_t i = 0; i < m_subtrees.size(); ++i) 
	{
		if (dirname.startsWith(m_subtrees[i])) 
		{
			return true;
		}
	}
	// Now try directory-specific patterns
	dir_map_t::const_iterator it_dir = m_map.find(dirname);
	if (it_dir == m_map.end())
	{
		return false;
	}
	dir_cases_t const & cases = it_dir->second;
	std::vector<FileNameCase>::const_iterator it, itend = cases.end();
	for (it = cases.begin(); it != itend; ++it)
	{
		FileNameCase const & c = *it;
		if (c.can_extend ? fname.startsWith(c.prefix) : fname == c.prefix)
		{
			return true;
		}
	}
	return false;
}

void DirPatterns::addDir(const String& dirpath)
{
	dirs.insert(normalizePath(dirpath));
}

void DirPatterns::addSubtree(const String& dirpath)
{
	subtrees.push_back(normalizePath(dirpath));
}

bool DirPatterns::match(const String& dirpath) const
{
	String dirp = normalizePath(dirpath + "/");

	if( containsDoubleDots(dirp) )
	{
		// Always reject /../ or /..
		return false;
	}

	if (dirs.find(dirp) != dirs.end()) {
		return true;
	}
	size_t n = subtrees.size();
	for (size_t i = 0; i < n; ++i) {
		if (dirp.startsWith(subtrees[i])) {
			return true;
		}
	}
	return false;
}



void
ExecArgsPatterns::addPattern(char const * exec_path_pattern, const Array<ExecArgsPatterns::Arg>& args, const EnvironmentVariablePatterns& envVarPatterns, const String& ident)
{
	PathPatterns pp;
	pp.addPattern(exec_path_pattern);
	m[ident].push_back(Pattern(pp, args, envVarPatterns));
}

bool 
ExecArgsPatterns::match(const String& exec_path, Array<String> const & args, const StringArray& envVars, const String& ident) const
{
	String cleanedPath = normalizePath(exec_path);

	if( containsDoubleDots(exec_path) )
	{
		// Always reject anything including /../ 
		return false;
	}

	StringArray users; 
	users.push_back(ident); 
	users.push_back("*"); 
	for (StringArray::const_iterator user_it = users.begin(); 
			user_it != users.end(); ++user_it)
	{
		map_t::const_iterator it = m.find(*user_it);
		if (it == m.end())
		{
			continue; 
		}

		for (size_t i = 0; i < it->second.size(); ++i)
		{
			if (!it->second[i].pathPatterns.match(cleanedPath))
			{
				continue;
			}

			// executable matched, now check the args.
			// argv[0] is special, being the name of the executable
			if (args.size() > 0 && args[0] != cleanedPath)
			{
				continue;
			}

			// For the time being, we will treat the args as a string or a path. In the future this will be evaluated as a regular expression
			StringArray argsWithoutExecPath(args.size() != 0 ? args.begin()+1 : args.begin(), args.end());
			const Array<Arg>& argsPattern(it->second[i].args);
			if (argsPattern.size() != argsWithoutExecPath.size())
			{
				continue;
			}

			bool argsMatch = true;
			for (size_t j = 0; j < argsPattern.size(); ++j)
			{
				if (argsPattern[j].argType == E_ANYTHING_ARG)
				{
					// We'll eat anything and like it.
				}
				else if (argsPattern[j].argType == E_PATH_PATTERN_ARG)
				{
					PathPatterns tmppp;
					tmppp.addPattern(argsPattern[j].arg.c_str());
					if (!tmppp.match(argsWithoutExecPath[j]))
					{
						argsMatch = false;
						break;
					}
				}
				else // if (argsPattern[j].argType == E_LITERAL_ARG)
				{
					if (argsPattern[j].arg != argsWithoutExecPath[j])
					{
						argsMatch = false;
						break;
					}
				}
			}

			if (argsMatch)
			{
				if (!it->second[i].environmentVariablePatterns.match(envVars))
				{
					continue;
				}
				return true;
			}
		}
	}
	return false;
}

IncludeHandler::~IncludeHandler()
{
}

void
EnvironmentVariablePatterns::addPattern(const char* name, const char* pattern, EPatternType type)
{
	m_patterns[name].push_back(Pattern(pattern, type));
}

namespace
{
	bool parseVarAndVal(const String& varAndVal, String& var, String& val)
	{
		size_t idx = varAndVal.indexOf('=');
		if (idx == String::npos)
		{
			return false;
		}
		var = varAndVal.substring(0, idx);
		val = varAndVal.substring(idx + 1);
		return true;
	}

	bool
	matchEnvironment(const SortedVectorMap<String, String>& env, const String& var, const String& val)
	{
		SortedVectorMap<String, String>::const_iterator i = env.find(var);
		if (i != env.end())
		{
			if (i->second == val)
			{
				return true;
			}
		}
		return false;
	}
	
	struct CurrentEnvironmentConstructor
	{
		static SortedVectorMap<String, String>* create(int dummy)
		{
			AutoPtr<SortedVectorMap<String, String> > retval(new SortedVectorMap<String, String>);
			for (char** p = environ; *p; ++p)
			{
				String var;
				String val;
				if (parseVarAndVal(*p, var, val))
				{
					retval->insert(std::make_pair(var, val));
				}
			}
			return retval.release();
		}
	};

	LazyGlobal<SortedVectorMap<String, String>, int, CurrentEnvironmentConstructor> g_currEnvironment = BLOCXX_LAZY_GLOBAL_INIT(0);

	struct SecureMinimalEnvironmentConstructor
	{
		static SortedVectorMap<String, String>* create(int dummy)
		{
			AutoPtr<SortedVectorMap<String, String> > retval(new SortedVectorMap<String, String>);
			StringArray minEnv = Secure::minimalEnvironment();
			for (StringArray::const_iterator i = minEnv.begin(); i != minEnv.end(); ++i)
			{
				String var;
				String val;
				if (parseVarAndVal(*i, var, val))
				{
					retval->insert(std::make_pair(var, val));
				}
			}
			return retval.release();
		}
	};

	LazyGlobal<SortedVectorMap<String, String>, int, SecureMinimalEnvironmentConstructor> g_secureMinimalEnvironment = BLOCXX_LAZY_GLOBAL_INIT(0);

	bool
	matchSecureMinimalEnvironment(const String& var, const String& val)
	{
		return matchEnvironment(g_secureMinimalEnvironment, var, val);
	}
	
	bool
	matchCurrentEnvironment(const String& var, const String& val)
	{
		return matchEnvironment(g_currEnvironment, var, val);
	}

} // end unnamed namespace

bool
EnvironmentVariablePatterns::match(const StringArray& env) const
{
	for (size_t i = 0; i < env.size(); ++i)
	{
		const String& curEnvVarAndVal(env[i]);
		String var;
		String val;
		if (!parseVarAndVal(curEnvVarAndVal, var, val))
		{
			return false;
		}

		// first see if the variable is allowed in m_patterns
		if (matchPatterns(var, val))
		{
			continue;
		}

		if (matchSecureMinimalEnvironment(var, val))
		{
			continue;
		}

		if (matchCurrentEnvironment(var, val))
		{
			continue;
		}

		return false;
	}
	return true;
}

bool
EnvironmentVariablePatterns::matchPatterns(const String& var, const String& val) const
{
	// first see if the variable is allowed in m_patterns
	PatternMap_t::const_iterator patternIter = m_patterns.find(var);
	if (patternIter == m_patterns.end())
	{
		return false;
	}

	bool foundMatch = false;
	for (size_t j = 0; j < patternIter->second.size() && !foundMatch; ++j)
	{
		const Pattern& curPattern(patternIter->second[j]);
		switch (curPattern.m_type)
		{
			case E_ANYTHING_PATTERN:
				foundMatch = true;
				break;
			case E_LITERAL_PATTERN:
				if (val == curPattern.m_pattern)
				{
					foundMatch = true;
				}
				break;
			case E_PATH_PATTERN:
				{
					PathPatterns tmppp;
					tmppp.addPattern(curPattern.m_pattern.c_str());
					if (tmppp.match(val))
					{
						foundMatch = true;
					}
				}
				break;
		}
	}
	return foundMatch;
}



} // namespace PrivilegeConfig	
} // namespace OW_NAMESPACE
