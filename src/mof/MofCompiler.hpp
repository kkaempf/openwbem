/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef MOF_COMPILER_HPP_
#define MOF_COMPILER_HPP_

#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_String.hpp"
#include "OW_MofParserErrorHandlerIFC.hpp"
#include "OW_Reference.hpp"
#include "Grammar.h"

class MofCompiler
{
public:
	MofCompiler( OW_Reference<OW_CIMOMHandleIFC> ch, const OW_String& nameSpace, OW_Reference<OW_MofParserErrorHandlerIFC> mpeh );
	~MofCompiler();

	long compile( const OW_String& filename );

	static OW_String fixParsedString(const OW_String& s);
	static OW_Reference<OW_MofParserErrorHandlerIFC> theErrorHandler;
	static OW_AutoPtr<MOFSpecification> mofSpecification;
	static OW_String basepath;

	// This variable is only for convenience for the lexer and parser.
	// After parsing is complete, it should not be used.  The filename and
	// line numbers are stored in the AST.
	static lineInfo theLineInfo;

private:
	OW_Reference<OW_CIMOMHandleIFC> m_ch;
	OW_String m_nameSpace;

};

#endif // MOF_COMPILER_HPP_
