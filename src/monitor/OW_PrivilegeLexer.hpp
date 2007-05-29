#ifndef OW_PRIVILEGE_LEXER_HPP_INCLUDE_GUARD_
#define OW_PRIVILEGE_LEXER_HPP_INCLUDE_GUARD_

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

// Including FlexLexer.h multiple times with the same definition of the
// yyFlexLexer symbol causes problems -- the yyFlexLexer class gets defined
// multiple times.  When this header file is #include'd from O_PrivilegeLexer.ll
// we omit the #include of FlexLexer.h, as the code flex generates from
// OW_PrivilegeLexer.ll already already #includes FlexLexer.h.
//  
#ifndef OW_PRIVILEGE_LEXER_LL
#undef yyFlexLexer
#define yyFlexLexer openwbem_privconfig_FlexLexer
#include <FlexLexer.h>
#endif
#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_String.hpp"
#include <deque>

/**
 * @author ???
 */

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


namespace OW_NAMESPACE { namespace PrivilegeConfig { class IncludeHandler; } }

class openwbem_privconfig_Lexer : public yyFlexLexer
{
public:
	openwbem_privconfig_Lexer(std::istream & arg_yyin, OpenWBEM::PrivilegeConfig::IncludeHandler& includeHandler, const OpenWBEM::String& bufferName);

	virtual int yylex();

	void get_location(openwbem_privconfig_yyltype & loc) const;

	bool has_value()
	{
		return m_has_value;
	}

	// returns 0 on success. 1 if the include is recursive
	int include(const OpenWBEM::String& includeParam);
	// returns false if no more include buffers are on the stack
	bool endInclude();

private:

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
		OpenWBEM::String m_bufferName;
	};

	std::deque<LexerState> m_includeStack;
	OpenWBEM::PrivilegeConfig::IncludeHandler* m_includeHandler;

	virtual void LexerOutput(char const *, int);
	virtual void LexerError(char const * msg);

	void pre_action();

	bool m_has_value;
	unsigned m_first_column;
	unsigned m_first_line;
	unsigned m_last_column;
	unsigned m_last_line;
	unsigned m_next_column;
	unsigned m_next_line;
	OpenWBEM::String m_bufferName;
};

#endif
