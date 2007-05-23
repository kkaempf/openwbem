/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */

#ifndef OW_MOF_COMPILER_HPP_INCLUDE_GUARD_
#define OW_MOF_COMPILER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_String.hpp"
#include "OW_MOFParserErrorHandlerIFC.hpp"
#include "OW_Exception.hpp"
#include "OW_Array.hpp"
#include "OW_MOFLineInfo.hpp"
#include "OW_MOFGrammar.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_Logger.hpp"
#include <stack>


#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

// this needs to be at global scope because flex also declares it.
typedef struct yy_buffer_state *YY_BUFFER_STATE;

struct owmofltype;

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(MOFCompiler, OW_MOF_API);

namespace MOF
{

class OW_MOF_API Compiler
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
			, m_removeObjects(false)
		{}
		Options(const String& namespace_,
			bool createNamespaces, bool checkSyntaxOnly, const String& dumpXmlFile,
			bool remove, bool preserve, bool upgrade, const StringArray& includeDirs,
			bool ignoreDoubleIncludes, bool removeDescriptions, bool removeObjects, 
			const String& depSearchDir = "")
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
			, m_removeObjects(removeObjects)
			, m_depSearchDir(depSearchDir)
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
		bool m_removeObjects;
		String m_depSearchDir; 
	};

	Compiler( const CIMOMHandleIFCRef& ch, const Options& opts, const ParserErrorHandlerIFCRef& mpeh );
	~Compiler();
	long compile( const String& filename );
	long compileString( const String& mof );
	static String fixParsedString(const String& s);

	ParserErrorHandlerIFCRef theErrorHandler;

private:
	CIMOMHandleIFCRef m_ch;
	Options m_opts;
	// unimplemented
	Compiler(const Compiler& x);
	Compiler& operator=(const Compiler& x);

};

class OW_MOF_API CompilerState
{
public:

	CompilerState(const ParserErrorHandlerIFCRef& mpeh, const String& initialFilename);
	~CompilerState();

	AutoPtr<MOFSpecification> mofSpecification;

	// This function is only for use by the lexer and parser
	void updateLocation(const char* yytext, owmofltype* yylocp);
	LineInfo getLineInfo() const;
	ParserErrorHandlerIFCRef getErrorHandler() const;
	void startNewFile(const String& filenameWithPath);
	String getBasePath() const;


	// Needed by the code to implement includes
	enum
	{
		E_MAX_INCLUDE_DEPTH = 100
	};

	friend struct include_t;
	struct include_t
	{
		include_t()
		: m_firstColumn(0)
		, m_firstLine(0)
		, m_lastColumn(0)
		, m_lastLine(0)
		, m_nextColumn(1)
		, m_nextLine(1)
		{
		}
		void setLineInfo(const CompilerState& s)
		{
			m_firstColumn = s.m_firstColumn;
			m_firstLine = s.m_firstLine;
			m_lastColumn = s.m_lastColumn;
			m_lastLine = s.m_lastLine;
			m_nextColumn = s.m_nextColumn;
			m_nextLine = s.m_nextLine;
			m_fileName = s.m_fileName;
		}
		YY_BUFFER_STATE owmofBufferState;
		unsigned m_firstColumn;
		unsigned m_firstLine;
		unsigned m_lastColumn;
		unsigned m_lastLine;
		unsigned m_nextColumn;
		unsigned m_nextLine;
		String m_fileName;
	};

	void setLineInfo(const include_t& x);

	std::stack<include_t> include_stack;

private:

	unsigned m_firstColumn;
	unsigned m_firstLine;
	unsigned m_lastColumn;
	unsigned m_lastLine;
	unsigned m_nextColumn;
	unsigned m_nextLine;
	String m_fileName;
	String m_basepath;
	ParserErrorHandlerIFCRef m_errorHandler;

	// unimplemented
	CompilerState(const CompilerState& x);
	CompilerState& operator=(const CompilerState& x);

};


/**
 * @param realhdl If null, a dummy handle will be used which will supply "fake" CIMClasses and CIMQualifierTypes to the mof compiler
 */
OW_MOF_API void compileMOF(const String& mof, const CIMOMHandleIFCRef& realhdl, const String& ns,
	CIMInstanceArray& instances, CIMClassArray& classes, CIMQualifierTypeArray& qualifierTypes);



} // end namespace MOF
} // end namespace OW_NAMESPACE

#ifdef OW_WIN32
#pragma warning (pop)
#endif

#endif // MOF_COMPILER_HPP_
