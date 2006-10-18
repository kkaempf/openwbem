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

namespace OW_NAMESPACE
{
namespace PrivilegeConfig
{

OW_DEFINE_EXCEPTION(UnescapeString)

namespace
{
	String unescape_path(char const * s, std::size_t n)
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

	// REQUIRE: path contains at least one '/'
	//
	std::pair<String, String> split_path(String const & path)
	{
		std::size_t n = path.lastIndexOf('/');
		return std::make_pair(path.substring(0, n + 1), path.substring(n + 1));
	}

	enum EPatternsType { // numeric assignments are significant (convert_pattern)
		E_PATH = 0, E_WILDCARD = 1, E_SUBTREE = 2
	};

	std::pair<String, EPatternsType> convert_pattern(char const * s)
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
		return std::make_pair(unescape_path(s, n), pattern_type);
	}

	bool isoctal(int c)
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
} // end unnamed namespace

String unescape_path(char const * epath)
{
	return unescape_path(epath, std::strlen(epath));
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
							if (isoctal(c))
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

void PathPatterns::add_case(
	String const & dir_path, String const & fname_prefix, bool can_extend)
{
	FileNameCase c;
	c.prefix = fname_prefix;
	c.can_extend = can_extend;
	m_map[dir_path].push_back(c);
}

void PathPatterns::add_subtree(String const & dir_path)
{
	subtrees.push_back(dir_path);
}

void PathPatterns::add_pattern(char const * pattern)
{
	std::pair<String, EPatternsType> x = convert_pattern(pattern);
	String const & path = x.first;
	OW_ASSERT(path.startsWith("/"));
	EPatternsType pat_type = x.second;
	if (pat_type == E_SUBTREE) {
		OW_ASSERT(path.endsWith("/"));
		this->add_subtree(path);
	}
	else {
		bool ext = (pat_type == E_WILDCARD);
		std::pair<String, String> dir_and_name = split_path(path);
		this->add_case(dir_and_name.first, dir_and_name.second, ext);
	}
}

bool PathPatterns::match(String const & path) const
{
	if (!path.startsWith("/") || path.endsWith("/"))
	{
		return false;
	}
	std::pair<String, String> x = split_path(path);
	String const & fname = x.second;
	if (fname == "." || fname == "..")
	{
		return false;
	}
	String const & dirname = x.first;
	// First, see if any subtree pattern is matched
	for (size_t i = 0; i < subtrees.size(); ++i) {
		if (dirname.startsWith(subtrees[i])) {
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

void DirPatterns::add_dir(String const & dirpath)
{
	dirs.insert(dirpath);
}

void DirPatterns::add_subtree(String const & dirpath)
{
	subtrees.push_back(dirpath);
}

bool DirPatterns::match(String const & dirpath) const
{
	String dirp = dirpath.endsWith("/") ? dirpath : dirpath + "/";
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
ExecArgsPatterns::add_pattern(char const * exec_path_pattern, const Array<ExecArgsPatterns::Arg>& args, String const & ident)
{
	PathPatterns pp;
	pp.add_pattern(exec_path_pattern);
	m[ident].push_back(std::make_pair(pp, args));
}

bool 
ExecArgsPatterns::match(String const & exec_path, Array<String> const & args, String const & ident) const
{
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
			if (!it->second[i].first.match(exec_path))
			{
				continue;
			}

			// executable matched, now check the args.
			// For the time being, we will treat the args as a string or a path. In the future this will be evaluated as a regular expression
			const Array<Arg>& argsPattern(it->second[i].second);
			if (argsPattern.size() != args.size())
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
					tmppp.add_pattern(argsPattern[j].arg.c_str());
					if (!tmppp.match(args[j]))
					{
						argsMatch = false;
						break;
					}
				}
				else // if (argsPattern[j].argType == E_LITERAL_ARG)
				{
					if (argsPattern[j].arg != args[j])
					{
						argsMatch = false;
						break;
					}
				}
			}

			if (argsMatch)
			{
				return true;
			}
		}
	}
	return false;
}



} // namespace PrivilegeConfig	
} // namespace OW_NAMESPACE
