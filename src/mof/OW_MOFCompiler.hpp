/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
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
#include "OW_Exception.hpp"
#include "OW_Array.hpp"

// these 2 need to be at global scope because flex also declares them.
struct owmof_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(MOFCompiler)

class CIMInstance;

namespace MOF
{

class MOFSpecification;

class Compiler
{
public:
	struct Options
	{
		Options() 
			: m_createNamespaces(false)
			, m_checkSyntaxOnly(false)
			, m_remove(false)
			, m_preserve(false)
			, m_upgrade(true)
			, m_ignoreDoubleIncludes(false)
			, m_removeDescriptions(false)
		{}
		Options(const String& namespace_,
			bool createNamespaces, bool checkSyntaxOnly, const String& dumpXmlFile,
			bool remove, bool preserve, bool upgrade, const StringArray& includeDirs,
			bool ignoreDoubleIncludes, bool removeDescriptions)
			: m_namespace(namespace_)
			, m_createNamespaces(createNamespaces)
			, m_checkSyntaxOnly(checkSyntaxOnly)
			, m_dumpXmlFile(dumpXmlFile)
			, m_remove(remove)
			, m_preserve(preserve)
			, m_upgrade(upgrade)
			, m_includeDirs(includeDirs)
			, m_ignoreDoubleIncludes(ignoreDoubleIncludes)
			, m_removeDescriptions(removeDescriptions)
		{
		}

		String m_namespace;
		bool m_createNamespaces;
		bool m_checkSyntaxOnly;
		String m_dumpXmlFile;
		bool m_remove;
		bool m_preserve;
		bool m_upgrade;
		StringArray m_includeDirs;
		bool m_ignoreDoubleIncludes;
		bool m_removeDescriptions;
	};

	Compiler( Reference<CIMOMHandleIFC> ch, const Options& opts, Reference<ParserErrorHandlerIFC> mpeh );
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
		YY_BUFFER_STATE owmofBufferState;
		lineInfo theLineInfo;
	};

	include_t include_stack[E_MAX_INCLUDE_DEPTH];
	int include_stack_ptr;

private:
	Reference<CIMOMHandleIFC> m_ch;
	Options m_opts;
};


CIMInstance compileInstanceFromMOF(const String& instMOF);
CIMInstanceArray compileInstancesFromMOF(const String& instMOF);
CIMInstanceArray compileInstancesFromMOF(const String& instMOF, const CIMOMHandleIFCRef& realhdl, const String& ns);


} // end namespace MOF
} // end namespace OpenWBEM

#endif // MOF_COMPILER_HPP_
