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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/* scanner for mof */

#include "Grammar.h"
#include "mofparser.h"
#include "MofCompiler.hpp"
#include "OW_Format.hpp"
#include "lexer.hpp"

using namespace spirit;

int yylex(YYSTYPE* rval, void* mofcomp)
{
	return ((MofCompiler *)mofcomp)->lexer.getNextToken(rval);
}


class moflexer
{
public:
	typedef wchar_t mofchar_t;
	moflexer();

	struct include_t
	{
		OW_Reference<mofchar_t> buf;
		mofchar_t* first;
		mofchar_t* last;
		lineInfo theLineInfo;
	};
	std::stack<include_t> include_stack;
	OW_Reference<mofchar_t> m_cur_buf;
	mofchar_t* m_first;
	mofchar_t* m_last;

        typedef boost::function<void, 
                        const mofchar_t* const&, 
                        const mofchar_t*&, 
                        const mofchar_t* const&, 
                        const int&, 
                        lexer_control<int>&> func_callback_t;
	lexer<mofchar_t const*, int, func_callback_t> lexer;

	// Set a limit to prevent infinite include recursion.
	const int MAX_INCLUDE_DEPTH = 100;
	const int MOF_LEXER_VERSION = 1; // increment this if you ever change the regexes.

	enum MOF_LEXER_states
	{
		Start = 0,
		Ccomment,
		CPPcomment,
		OnlyIdentifier
	};

	template <int State>
	static void begin_state(const mofchar_t* const& /*tok_begin*/, const mofchar_t*& /*tok_end*/,
			const char* const& /*last*/, const int& /*token*/, lexer_control<int>& ctl)
	{
		ctl.set_state(State);
		ctl.ignore_current_token();
	}

	static void ignore_token(const mofchar_t* const& /*tok_begin*/, const mofchar_t*& /*tok_end*/,
			const char* const& /*last*/, const int& /*token*/, lexer_control<int>& ctl)
	{
		ctl.ignore_current_token();
	}

	struct inc_line_num
	{
		inc_line_num(int& line_) : line(line_) {}
		void operator()(const mofchar_t* const& /*tok_begin*/, const mofchar_t*& /*tok_end*/,
			const char* const& /*last*/, const int& /*token*/, lexer_control<int>& ctl)
		{
			++line;
			ctl.ignore_current_token();
		}

		int& line;
	};
	struct inc_line_num_and_begin_Start
	{
		inc_line_num_and_begin_Start(int& line_) : line(line_) {}
		void operator()(const mofchar_t* const& /*tok_begin*/, const mofchar_t*& /*tok_end*/,
			const char* const& /*last*/, const int& /*token*/, lexer_control<int>& ctl)
		{
			++line;
			ctl.set_state(State);
			ctl.ignore_current_token();
		}

		int& line;
	};

	struct calcStringLines
	{
		calcStringLines(int& line_) : line(line_) {}
		/* figure out how many lines we passed over */
		void operator()(const mofchar_t* const& tok_begin, const mofchar_t*& tok_end,
			const char* const& /*last*/, const int& /*token*/, lexer_control<int>& /*ctl*/)
		{
			for (const mofchar_t* b = tok_begin; b != tok_end; ++b)
			{
				if ( *b == '\r' || *b == '\n' )
				{
					++line;
					if ( ( b + 1 ) < tok_end )
						if ( *b == '\r' && *(b + 1) == '\n' )
							++b;
				}
			}
		}
		int& line;
	};

	int getNextToken(YYSTYPE* rval);
	includeFile(const OW_String& filename );
	static OW_String convertToUTF8(mofchar_t* begin, mofchar_t* end);
	void initializeFile(const OW_String& filename);
	void initializeString(const OW_String& mofstring);
};



moflexer::moflexer(MofCompiler& mofcompiler)
{
	/* here are the definitions */

	// increment MOF_LEXER_VERSION if any of these ever change
#define MOF_identifier L"[a-zA-Z_][0-9a-zA-Z_]*"

#define MOF_binaryValue L"[+-]?[01]+[bB]"
#define MOF_octalValue L"[+-]?"0"[0-7]+"
#define MOF_decimalValue L"[+-]?([1-9]|[0-9]*|\"0\")"
#define MOF_hexValue L"[+-]?\"0\"[xX][0-9a-fA-F]+"
#define MOF_floatValue L"[+-]?[0-9]*\".\"[0-9]+([eE][+-]?[0-9]+)?"

#define MOF_simpleEscape L"[bfnrt'\"\\\\]"
#define MOF_hexEscape L"\"x\"[0-9a-fA-F]{1,4}"

#define MOF_escapeSequence L"[\\\\](" MOF_simpleEscape L"|" MOF_hexEscape L")"
#define MOF_cChar L"[^'\\\\\\n\\r]|" MOF_escapeSequence
#define MOF_sChar L"[^\"\\\\\\n\\r]|" MOF_escapeSequence

#define MOF_charValue L"\\'" MOF_cChar L"\\'"
		/*stringValue \"{sChar}*\"(({ws}|[\n\r])*\"{sChar}*\")* */
#define MOF_stringValue L"\\\"" MOF_sChar L"*\\""

#define MOF_ws		L"[ \\t]+"


	/* C comment state regexes */
	lexer.register_regex(L"/*", NOID_TOK, &begin_state<Ccomment>);
	lexer.register_regex(L"[^*\\n\\r]*", NOID_TOK, &ignore_token, Ccomment); 			/* eat anything that's not a '*' */
	lexer.register_regex(L"\"*\"+[^*/\\n\\r]*", NOID_TOK, &ignore_token, Ccomment);	/* eat up '*'s not followed by '/'s */
	lexer.register_regex(L"\\r\\n", NOID_TOK, inc_line_num(mofcompiler.theLineInfo.lineNum), Ccomment);
	lexer.register_regex(L"\\n", NOID_TOK, inc_line_num(mofcompiler.theLineInfo.lineNum), Ccomment);
	lexer.register_regex(L"\\r", NOID_TOK, inc_line_num(mofcompiler.theLineInfo.lineNum), Ccomment);
	lexer.register_regex(L"\"*\"+\"/\"", NOID_TOK, &begin_state<Start>, Ccomment);

	/* end C comment state regexes */

	/* c++ comment state regexes */
	lexer.register_regex(L"\"//\"", NOID_TOK, &begin_state<CPPcomment>);
	lexer.register_regex(L"[^\\n\\r]*", NOID_TOK, &ignore_token, CPPcomment);		/* eat anything but newline */
	lexer.register_regex(L"\\r\\n", NOID_TOK, inc_line_num_and_begin_Start(mofcompiler.theLineInfo.lineNum), CPPcomment);
	lexer.register_regex(L"\\n", NOID_TOK, inc_line_num_and_begin_Start(mofcompiler.theLineInfo.lineNum), CPPcomment);
	lexer.register_regex(L"\\r", NOID_TOK, inc_line_num_and_begin_Start(mofcompiler.theLineInfo.lineNum), CPPcomment);
	/* end c++ comment state regexes */

	/* OnlyIdentifier state regexes */
		/* Shame on whoever put special cases in the mof lexical specs and
		didn't even document it! */
	lexer.register_regex(MOF_ws L"*", NOID_TOK, &ignore_token, OnlyIdentifier); 			/* eat up whitespace */
	lexer.register_regex(MOF_identifier, IDENTIFIER_TOK, &begin_state<Start>, OnlyIdentifier);

	/* end OnlyIdentifier state regexes */

	lexer.register_regex(MOF_ws, NOID_TOK, &ignore_token);	/* skip blanks and tabs */
	lexer.register_regex(L"\\r\\n", NOID_TOK, inc_line_num(mofcompiler.theLineInfo.lineNum));
	lexer.register_regex(L"\\n", NOID_TOK, inc_line_num(mofcompiler.theLineInfo.lineNum));
	lexer.register_regex(L"\\r", NOID_TOK, inc_line_num(mofcompiler.theLineInfo.lineNum));

	lexer.register_regex(L"any", ANY_TOK);
	lexer.register_regex(L"as", AS_TOK);
	lexer.register_regex(L"association", ASSOCIATION_TOK);
	lexer.register_regex(L"class", CLASS_TOK);
	lexer.register_regex(L"disableoverride", DISABLEOVERRIDE_TOK);
	lexer.register_regex(L"boolean", DT_BOOL_TOK);
	lexer.register_regex(L"char16", DT_CHAR16_TOK);
	lexer.register_regex(L"datetime", DT_DATETIME_TOK);
	lexer.register_regex(L"real32", DT_REAL32_TOK);
	lexer.register_regex(L"real64", DT_REAL64_TOK);
	lexer.register_regex(L"sint16", DT_SINT16_TOK);
	lexer.register_regex(L"sint32", DT_SINT32_TOK);
	lexer.register_regex(L"sint64", DT_SINT64_TOK);
	lexer.register_regex(L"sint8", DT_SINT8_TOK);
	lexer.register_regex(L"string", DT_STR_TOK);
	lexer.register_regex(L"uint16", DT_UINT16_TOK);
	lexer.register_regex(L"uint32", DT_UINT32_TOK);
	lexer.register_regex(L"uint64", DT_UINT64_TOK);
	lexer.register_regex(L"uint8", DT_UINT8_TOK);
	lexer.register_regex(L"enableoverride", ENABLEOVERRIDE_TOK);
	lexer.register_regex(L"false", FALSE_TOK);
	lexer.register_regex(L"flavor", FLAVOR_TOK);
	lexer.register_regex(L"indication", INDICATION_TOK);
	lexer.register_regex(L"instance", INSTANCE_TOK);
	lexer.register_regex(L"method", METHOD_TOK);
	lexer.register_regex(L"null", NULL_TOK);
	lexer.register_regex(L"of", OF_TOK);
	lexer.register_regex(L"parameter", PARAMETER_TOK);
	lexer.register_regex(L"#pragma", PRAGMA_TOK);
	lexer.register_regex(L"property", PROPERTY_TOK);
	lexer.register_regex(L"qualifier", QUALIFIER_TOK, begin_OnlyIdentifier);
	lexer.register_regex(L"ref", REF_TOK);
	lexer.register_regex(L"reference", REFERENCE_TOK);
	lexer.register_regex(L"restricted", RESTRICTED_TOK);
	lexer.register_regex(L"schema", SCHEMA_TOK);
	lexer.register_regex(L"scope", SCOPE_TOK);
	lexer.register_regex(L"tosubclass", TOSUBCLASS_TOK);
	lexer.register_regex(L"translatable", TRANSLATABLE_TOK);
	lexer.register_regex(L"true", TRUE_TOK);
	lexer.register_regex(L"(", LPAREN_TOK);
	lexer.register_regex(L")", RPAREN_TOK);
	lexer.register_regex(L"{", LBRACE_TOK);
	lexer.register_regex(L"}", RBRACE_TOK);
	lexer.register_regex(L";", SEMICOLON_TOK);
	lexer.register_regex(L"[", LBRACK_TOK);
	lexer.register_regex(L"]", RBRACK_TOK);
	lexer.register_regex(L",", COMMA_TOK);
	lexer.register_regex(L"$", DOLLAR_TOK);
	lexer.register_regex(L":", COLON_TOK);
	lexer.register_regex(L"=", EQUALS_TOK);



	lexer.register_regex(MOF_binaryValue, binaryValue);

	lexer.register_regex(MOF_octalValue, octalValue);

	lexer.register_regex(MOF_decimalValue, decimalValue);

	lexer.register_regex(MOF_hexValue, hexValue);

	lexer.register_regex(MOF_floatValue, floatValue);

	lexer.register_regex(MOF_charValue, charValue);

	lexer.register_regex(MOF_stringValue L"((" MOF_ws L"|[\\r\\n])*" MOF_stringValue L")*", stringValue, calcStringLines(mofcompiler.theLineInfo.lineNum));

	lexer.register_regex(MOF_identifier, IDENTIFIER_TOK);


	OW_String moflexerstates(OW_DEFAULT_STATE_DIR);
	moflexerstates += "/moflexer.states";
	bool lexerstatesloaded = false;
	if (OW_FileSystem::exists(moflexerstates))
	{
		ifstream in(moflexerstates.c_str());
		if (lexer.load(in, lexerversion))
		{
			lexerstatesloaded = true;
		}
		else
		{
			// failed, delete the file, it's probably bad
			OW_FileSystem::removeFile(moflexerstates);
		}
	}
	if (!lexerstatesloaded)
	{
		lexer.compile_dfa();
		ofstream out(moflexerstates.c_str());
		if (out.good())
		{
			if (!lexer.save(in, lexerversion))
			{
				// failed to write the states, disk is probably full or something...
			}
		}
	}
}

int moflexer::getNextToken(YYSTYPE* rval)
{
	mofchar_t* tok_begin;
	mofchar_t* tok_end;
	int tok = lexer.next_token(m_first, m_last, tok_begin, tok_end);
	rval->pString = new OW_String(convertToUTF8(tok_begin, tok_end));
	if (m_first == m_last)
	{
		// reached the end of the current file.
		if (lexer.get_current_state() == Ccomment)
		{
			mofcompiler.theErrorHandler->fatalError("Unterminated Comment",
				mofcompiler.theLineInfo);
			return 0;
		}
		else
		{
			if ( include_stack.size() == 0 )
			{
				return 0;
			}
			else
			{
				mofcompiler.theErrorHandler->progressMessage("Finished parsing.", mofcompiler.theLineInfo);
				mofcompiler.theLineInfo = include_stack.top().theLineInfo;
				m_cur_buf = include_stack.top().buf;
				m_first = include_stack.top().first;
				m_last = include_stack.top().last;
				include_stack.pop();
			}
		}
	}
	return tok;
}

namespace
{
	struct fileCloser
	{
		fileCloser(FILE* f_) : f(f_) {}
		~fileCloser() { fclose(f); }
		FILE* f;
	};
}


void 
moflexer::includeFile( const OW_String& filename )
{
	if ( include_stack.size() >= MAX_INCLUDE_DEPTH )
	{
		// REPORT AN ERROR
		mofcompiler.theErrorHandler->fatalError(
			format("Includes nested too deep (Max of %1 levels)", MAX_INCLUDE_DEPTH).c_str(),
			mofcompiler.theLineInfo);
		return;
	}

	// first try to find the file in the same dir as our original file.
	OW_String filenameWithPath = mofcompiler.basepath + "/" + filename;
	FILE* newfile = fopen( filenameWithPath.c_str(), "r" );

	if ( !newfile )
	{
		// try to find it in the current dir
		newfile = fopen( filename.c_str(), "r" );
		if ( !newfile )
		{
			mofcompiler.theErrorHandler->fatalError(
				format("Could not open include file %1", filename).c_str(), mofcompiler.theLineInfo);
			return;
		}
		filenameWithPath = filename;
	}
	fileCloser closer(newfile);

	OW_UInt32 filesize;
	if (!OW_FileSystem::getFileSize(filenameWithPath, filesize))
	{
		mofcompiler.theErrorHandler->fatalError(
			format("Could not get filesize for %1", filenameWithPath).c_str(), mofcompiler.theLineInfo);
	}
	include_t x;
	x.buf = m_cur_buf;
	x.first = m_first;
	x.last = m_last;
	x.theLineInfo = mofcompiler.theLineInfo;
	include.stack.push(x);
	mofcompiler.theLineInfo.filename = filenameWithPath;
	mofcompiler.theLineInfo.lineNum = 1;
	mofcompiler.theErrorHandler->progressMessage("Starting parsing.", mofcompiler.theLineInfo);
	buf = new mofchar_t[filesize]; // TODO: This is storing an array in an OW_Reference.  Memory leak!
	m_first = buf.getPtr();
	m_last = m_first + filesize;
	// TODO: read into a temp buffer, and translate from ASCII or UNICODE
	if (fread(m_first, 1, filesize, newfile) != filesize)
	{
		mofcompiler.theErrorHandler->fatalError(
			format("Failed reading data from %1", filename).c_str(), mofcompiler.theLineInfo);
		return;
	}
}
	

OW_String moflexer::convertToUTF8(mofchar_t* begin, mofchar_t* end)
{
	// TODO: Figure out how to do this.
}

void moflexer::initializeFile(const OW_String& filename)
{
}

void moflexer::initializeString(const OW_String& mofstring)
{
}

