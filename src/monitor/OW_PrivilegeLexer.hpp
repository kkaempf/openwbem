#ifndef OW_PRIVILEGE_LEXER_HPP_INCLUDE_GUARD_
#define OW_PRIVILEGE_LEXER_HPP_INCLUDE_GUARD_

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
#include "blocxx/BLOCXX_config.h"
#include "blocxx/CommonFwd.hpp"
#include "blocxx/String.hpp"
#include <deque>

/**
 * @author Kevin VanHorn
 * @author Dan Nuffer
 */

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.


// these need to be at global scope because flex also declares them.
typedef struct yy_buffer_state *YY_BUFFER_STATE;

// Location of token in file
//
struct openwbem_privconfig_yyltype
{
	unsigned first_column;
	unsigned first_line;
	unsigned last_column;
	unsigned last_line;
};


namespace OW_NAMESPACE
{

namespace PrivilegeConfig
{

class IncludeHandler;

// This class does not store the entire lexer state. Flex has some global variables it uses to store state.
// Thus if more than one instance of this class exist simultaneously, they will interfere with each other.
class openwbem_privconfig_Lexer
{
public:
	openwbem_privconfig_Lexer(std::istream & arg_yyin, OpenWBEM::PrivilegeConfig::IncludeHandler& includeHandler, const blocxx::String& bufferName);
	~openwbem_privconfig_Lexer();

	void get_location(openwbem_privconfig_yyltype & loc) const;

	bool has_value()
	{
		return m_has_value;
	}

	// returns 0 on success. 1 if the include is recursive
	int include(const blocxx::String& includeParam);
	// returns false if no more include buffers are on the stack
	bool endInclude();

	// Everything following this comment is not part of the public API of this class, it
	// should be treated as private, the reason it is public is so the flex generated
	// lexer can access it.
public:

	struct LexerState
	{
		LexerState()
		: m_has_value(false)
		, m_first_column(0)
		, m_first_line(0)
		, m_last_column(0)
		, m_last_line(0)
		, m_next_column(1)
		, m_next_line(1)
		{
		}

		bool m_has_value;
		unsigned m_first_column;
		unsigned m_first_line;
		unsigned m_last_column;
		unsigned m_last_line;
		unsigned m_next_column;
		unsigned m_next_line;
		YY_BUFFER_STATE m_bufferState;
		blocxx::String m_bufferName;
	};

	std::deque<LexerState> m_includeStack;
	OpenWBEM::PrivilegeConfig::IncludeHandler* m_includeHandler;

	void pre_action();

	bool m_has_value;
	unsigned m_first_column;
	unsigned m_first_line;
	unsigned m_last_column;
	unsigned m_last_line;
	unsigned m_next_column;
	unsigned m_next_line;
	blocxx::String m_bufferName;

	static int s_instanceCount;
};

}
}

#endif
