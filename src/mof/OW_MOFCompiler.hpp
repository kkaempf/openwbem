/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_MOF_COMPILER_HPP_INCLUDE_GUARD_
#define OW_MOF_COMPILER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_String.hpp"
#include "OW_MOFParserErrorHandlerIFC.hpp"
#include "OW_Reference.hpp"
#include "OW_MOFGrammar.hpp"

// these 2 need to be at global scope because flex also declares them.
struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;

namespace OpenWBEM
{

namespace MOF
{

class Compiler
{
public:
	Compiler( Reference<CIMOMHandleIFC> ch, const String& nameSpace, Reference<ParserErrorHandlerIFC> mpeh );
	~Compiler();

	long compile( const String& filename );
	long compileString( const String& mof );
	static String fixParsedString(const String& s);

	Reference<ParserErrorHandlerIFC> theErrorHandler;
	AutoPtr<MOFSpecification> mofSpecification;
	String basepath;

	// This variable is only for convenience for the lexer and parser.
	// After parsing is complete, it should not be used.  The filename and
	// line numbers are stored in the AST.
	lineInfo theLineInfo;

	// Needed by the code to implement includes
	enum
	{
		E_MAX_INCLUDE_DEPTH = 100
	};

	struct include_t
	{
		YY_BUFFER_STATE yyBufferState;
		lineInfo theLineInfo;
	};

	include_t include_stack[E_MAX_INCLUDE_DEPTH];
	int include_stack_ptr;

private:
	Reference<CIMOMHandleIFC> m_ch;
	String m_nameSpace;
};

} // end namespace MOF
} // end namespace OpenWBEM

#endif // MOF_COMPILER_HPP_
