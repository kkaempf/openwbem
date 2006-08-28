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

/**
 * @author ???
 */

// Location of token in file
//
struct openwbem_privconfig_yyltype
{
	unsigned first_column;
	unsigned first_line;
	unsigned last_column;
	unsigned last_line;
};

#define YYLTYPE openwbem_privconfig_yyltype

class openwbem_privconfig_Lexer : public yyFlexLexer
{
public:
	openwbem_privconfig_Lexer(std::istream & arg_yyin);

	virtual int yylex();

	void get_location(YYLTYPE & loc) const;

	bool has_value()
	{
		return m_has_value;
	}	

private:
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
};

#endif
