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

#include "MofCompiler.hpp"
#include "OW_Format.hpp"
#include "MofParserDecls.hpp"
#include "CIMOMVisitor.hpp"
#include "OW_Assertion.hpp"
#include <assert.h>
#include <cctype>

MofCompiler::MofCompiler( OW_Reference<OW_CIMOMHandleIFC> ch, const OW_String& nameSpace, OW_Reference<OW_MofParserErrorHandlerIFC> mpeh )
	: theErrorHandler(mpeh)
	, include_stack_ptr(0)
	, m_ch(ch)
	, m_nameSpace(nameSpace)
{
}

MofCompiler::~MofCompiler()
{
}

long MofCompiler::compile( const OW_String& filename )
{
	theLineInfo = lineInfo(filename,1);

    try
    {
        try
        {
            int i = filename.lastIndexOf('/');
            if (i != -1)
            {
                basepath = filename.substring(0,i);
            }
            else
            {
                basepath = OW_String();
            }
            yyin = fopen(filename.c_str(), "r");
            if (!yyin)
            {
                theErrorHandler->fatalError("Unable to open file", lineInfo(filename, 0));
                return 1;
            }

            theErrorHandler->progressMessage("Starting parsing",
                    lineInfo(filename, 0));
            #ifdef YYOW_DEBUG
            yydebug = 1;
            #endif

            yyparse(this);

            theErrorHandler->progressMessage("Finished parsing",
                    theLineInfo);

            CIMOMVisitor v(m_ch, m_nameSpace, theErrorHandler);
            mofSpecification->Accept(&v);
        }
        catch (const OW_MofParseFatalErrorException&)
        {
            // error has already been reported.
        }
        catch (OW_Assertion& e)
        {
            theErrorHandler->fatalError(format( "INTERNAL COMPILER ERROR: %1", e).c_str(), lineInfo("(none)", 0));
        }
        catch (OW_Exception& e)
        {
            theErrorHandler->fatalError(format( "ERROR: %1", e).c_str(), lineInfo("(none)", 0));
        }
        catch (std::exception& e)
        {
            theErrorHandler->fatalError(format( "INTERNAL COMPILER ERROR: %1", e.what() ).c_str(), lineInfo("(none)", 0));
        }
        catch(...)
        {
            theErrorHandler->fatalError( "INTERNAL COMPILER ERROR: Unknown exception", lineInfo("(none)", 0));
        }
    }
    catch (const OW_MofParseFatalErrorException&)
    {
        // error has already been reported.
    }
    return theErrorHandler->errorCount();
}

void yy_delete_buffer(YY_BUFFER_STATE b);
YY_BUFFER_STATE yy_scan_bytes( const char *bytes, int len );
namespace {

	struct yyBufferDeleter
	{
		yyBufferDeleter(YY_BUFFER_STATE buf) : m_buf(buf) {}
		~yyBufferDeleter() {yy_delete_buffer(m_buf);}
		YY_BUFFER_STATE m_buf;
	};
}

long MofCompiler::compileString( const OW_String& mof )
{
	OW_String filename = "string";
	theLineInfo = lineInfo(filename,1);

    try
    {
        try
        {
			YY_BUFFER_STATE buf = yy_scan_bytes(mof.c_str(), mof.length());
			yyBufferDeleter deleter(buf);
            theErrorHandler->progressMessage("Starting parsing",
                    lineInfo(filename, 0));
            #ifdef YYOW_DEBUG
            yydebug = 1;
            #endif

            yyparse(this);

            theErrorHandler->progressMessage("Finished parsing",
                    theLineInfo);

            CIMOMVisitor v(m_ch, m_nameSpace, theErrorHandler);
            mofSpecification->Accept(&v);
        }
        catch (const OW_MofParseFatalErrorException&)
        {
            // error has already been reported.
        }
        catch (OW_Assertion& e)
        {
            theErrorHandler->fatalError(format( "INTERNAL COMPILER ERROR: %1", e).c_str(), lineInfo("(none)", 0));
        }
        catch (OW_Exception& e)
        {
            theErrorHandler->fatalError(format( "ERROR: %1", e).c_str(), lineInfo("(none)", 0));
        }
        catch (std::exception& e)
        {
            theErrorHandler->fatalError(format( "INTERNAL COMPILER ERROR: %1", e.what() ).c_str(), lineInfo("(none)", 0));
        }
        catch(...)
        {
            theErrorHandler->fatalError( "INTERNAL COMPILER ERROR: Unknown exception", lineInfo("(none)", 0));
        }
    }
    catch (const OW_MofParseFatalErrorException&)
    {
        // error has already been reported.
    }
    return theErrorHandler->errorCount();
}

// STATIC
OW_String MofCompiler::fixParsedString(const OW_String& s)
{
	OW_Array<OW_String> sa = s.tokenize("\n\r");
	OW_String retval;
	for (size_t i = 0; i < sa.size(); ++i)
	{
		// trim off whitespace
		OW_String trimmed = sa[i];
		trimmed.trim();
		// cut off the quotes and concatenate
		if (trimmed.length() > 2)
		{
			assert(trimmed[0] == '"' && trimmed[trimmed.length()-1] == '"');
			retval += trimmed.substring(1, trimmed.length() - 2);
		}
	}
	OW_StringBuffer unescaped;

	for(size_t i = 0; i < retval.length(); ++i)
	{
		if (retval[i] == '\\')
		{
			/* this can never happen, unless someone messes up the lexer
			if (i+1 >= retval.length())
			{
				OW_THROW(OW_Exception, "String cannot end with '\\'");
			}*/
			++i;
			switch(retval[i])
			{
				case 'b':
					unescaped += '\b';
					break;
				case 't':
					unescaped += '\t';
					break;
				case 'n':
					unescaped += '\n';
					break;
				case 'f':
					unescaped += '\f';
					break;
				case 'r':
					unescaped += '\r';
					break;
				case '"':
					unescaped += '"';
					break;
				case '\'':
					unescaped += '\'';
					break;
				case '\\':
					unescaped += '\\';
					break;
				case 'x':
				case 'X':
					{
						// The lexer guarantees that there will be from 1-4 hex chars.
						OW_UInt16 hex = 0;
						for (size_t j = 0; j < 4; ++j)
						{
							hex <<= 4;
							char c = retval[i+j];
							if (isdigit(c))
							{
								hex |= c - '0';
							}
							else if (isxdigit(c))
							{
								c = toupper(c);
								hex |= c - 'A' + 0xA;
							}
							else
							{
								break;
							}
						}
						if (hex > CHAR_MAX)
						{
							OW_THROW(OW_Exception, "Escape sequence larger than supported maximum");
						}
						unescaped += static_cast<char>(hex);
					}
					break;

				default:
					/* this could never happen unless someone meses up the lexer
					OW_THROW(OW_Exception, "Invalid escape sequence"); */
					break;
			}
		}
		else
		{
			unescaped += retval[i];
		}
	}
	
	return unescaped.releaseString();
}

