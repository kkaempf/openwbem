/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*	this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*	this list of conditions and the following disclaimer in the documentation
*	and/or other materials provided with the distribution.
*
*  - Neither the name of Center 7 nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
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
#include "OW_MOFCompiler.hpp"
#include "OW_Format.hpp"
#include "OW_MOFParserDecls.hpp"
#include "OW_MOFCIMOMVisitor.hpp"
#include "OW_Assertion.hpp"
#include "OW_ThreadCancelledException.hpp"
#include "OW_MOFGrammar.hpp"

#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"

#include <assert.h>
#include <cctype>

// forward declarations of some lex/yacc functions we need to call.
void yy_delete_buffer(YY_BUFFER_STATE b);
YY_BUFFER_STATE yy_scan_bytes( const char *bytes, int len );

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION(MOFCompiler)

namespace MOF
{

Compiler::Compiler( Reference<CIMOMHandleIFC> ch, const Options& opts, Reference<ParserErrorHandlerIFC> mpeh )
	: theErrorHandler(mpeh)
	, include_stack_ptr(0)
	, m_ch(ch)
	, m_opts(opts)
{
}
Compiler::~Compiler()
{
}
long Compiler::compile( const String& filename )
{
	theLineInfo = lineInfo(filename,1);
	try
	{
		try
		{
			size_t i = filename.lastIndexOf('/');
			if (i != String::npos)
			{
				basepath = filename.substring(0,i);
			}
			else
			{
				basepath = String();
			}
			if (filename != "-")
			{
				yyin = fopen(filename.c_str(), "r");
				if (!yyin)
				{
					theErrorHandler->fatalError("Unable to open file", lineInfo(filename, 0));
					return 1;
				}
			}
			theErrorHandler->progressMessage("Starting parsing",
					lineInfo(filename, 0));
			#ifdef YYOW_DEBUG
			yydebug = 1;
			#endif
			yyparse(this);
			theErrorHandler->progressMessage("Finished parsing",
					theLineInfo);
			CIMOMVisitor v(m_ch, m_opts, theErrorHandler);
			mofSpecification->Accept(&v);
		}
		catch (const ParseFatalErrorException&)
		{
			// error has already been reported.
		}
		catch (Assertion& e)
		{
			theErrorHandler->fatalError(format( "INTERNAL COMPILER ERROR: %1", e).c_str(), theLineInfo);
		}
		catch (Exception& e)
		{
			theErrorHandler->fatalError(format( "ERROR: %1", e).c_str(), theLineInfo);
		}
		catch (std::exception& e)
		{
			theErrorHandler->fatalError(format( "INTERNAL COMPILER ERROR: %1", e.what() ).c_str(), theLineInfo);
		}
		catch (ThreadCancelledException&)
		{
			theErrorHandler->fatalError("INTERNAL COMPILER ERROR: Thread cancelled", theLineInfo);
			throw;
		}
		catch(...)
		{
			theErrorHandler->fatalError( "INTERNAL COMPILER ERROR: Unknown exception", theLineInfo);
		}
	}
	catch (const ParseFatalErrorException&)
	{
		// error has already been reported.
	}
	return theErrorHandler->errorCount();
}
namespace {
	struct yyBufferDeleter
	{
		yyBufferDeleter(YY_BUFFER_STATE buf) : m_buf(buf) {}
		~yyBufferDeleter() {yy_delete_buffer(m_buf);}
		YY_BUFFER_STATE m_buf;
	};
}
long Compiler::compileString( const String& mof )
{
	String filename = "string";
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
			CIMOMVisitor v(m_ch, m_opts, theErrorHandler);
			mofSpecification->Accept(&v);
		}
		catch (const ParseFatalErrorException&)
		{
			// error has already been reported.
		}
		catch (Assertion& e)
		{
			theErrorHandler->fatalError(format( "INTERNAL COMPILER ERROR: %1", e).c_str(), lineInfo("(none)", 0));
		}
		catch (Exception& e)
		{
			theErrorHandler->fatalError(format( "ERROR: %1", e).c_str(), lineInfo("(none)", 0));
		}
		catch (std::exception& e)
		{
			theErrorHandler->fatalError(format( "INTERNAL COMPILER ERROR: %1", e.what() ).c_str(), lineInfo("(none)", 0));
		}
		catch (ThreadCancelledException&)
		{
			theErrorHandler->fatalError("INTERNAL COMPILER ERROR: Thread cancelled", theLineInfo);
			throw;
		}
		catch(...)
		{
			theErrorHandler->fatalError( "INTERNAL COMPILER ERROR: Unknown exception", lineInfo("(none)", 0));
		}
	}
	catch (const ParseFatalErrorException&)
	{
		// error has already been reported.
	}
	return theErrorHandler->errorCount();
}
// STATIC
String Compiler::fixParsedString(const String& s)
{
	StringArray sa = s.tokenize("\n\r");
	String retval;
	for (size_t i = 0; i < sa.size(); ++i)
	{
		// trim off whitespace
		String trimmed = sa[i];
		trimmed.trim();
		// cut off the quotes and concatenate
		if (trimmed.length() > 2)
		{
			assert(trimmed[0] == '"' && trimmed[trimmed.length()-1] == '"');
			retval += trimmed.substring(1, trimmed.length() - 2);
		}
	}
	StringBuffer unescaped;
	for(size_t i = 0; i < retval.length(); ++i)
	{
		if (retval[i] == '\\')
		{
			/* this can never happen, unless someone messes up the lexer
			if (i+1 >= retval.length())
			{
				OW_THROW(Exception, "String cannot end with '\\'");
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
						UInt16 hex = 0;
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
							OW_THROW(MOFCompilerException, "Escape sequence larger than supported maximum");
						}
						unescaped += static_cast<char>(hex);
					}
					break;
				default:
					// this could never happen unless someone messes up the lexer
					OW_ASSERT("Invalid escape sequence" == 0);
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

class StoreLocalInstancesHandle : public CIMOMHandleIFC
{
public:
	virtual CIMObjectPath createInstance(const String &ns, const CIMInstance &instance)
	{
		m_instances.push_back(instance);
		return CIMObjectPath(ns, instance);
	}
	virtual CIMClass getClass(const String &ns, const String &className, WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_INCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_INCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		// just give back an empty class, with just the name set.
		return CIMClass(className);
	}

	CIMInstanceArray getInstances() const { return m_instances; }

private:
	CIMInstanceArray m_instances;
	CIMOMHandleIFCRef m_realhdl;


	// all these throw FAILED
	virtual CIMQualifierType getQualifierType(const String &ns, const String &qualifierName)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void setQualifierType(const String &ns, const CIMQualifierType &qualifierType)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void createClass(const String &ns, const CIMClass &cimClass)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void enumClassNames(const String &ns, const String &className, StringResultHandlerIFC &result, WBEMFlags:: EDeepFlag deep=WBEMFlags:: E_DEEP)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void close()
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void enumClass(const String &ns, const String &className, CIMClassResultHandlerIFC &result, WBEMFlags:: EDeepFlag deep=WBEMFlags:: E_SHALLOW, WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_INCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_INCLUDE_CLASS_ORIGIN)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void enumInstanceNames(const String &ns, const String &className, CIMObjectPathResultHandlerIFC &result)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void enumInstances(const String &ns, const String &className, CIMInstanceResultHandlerIFC &result, WBEMFlags:: EDeepFlag deep=WBEMFlags:: E_DEEP, WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual CIMValue invokeMethod(const String &ns, const CIMObjectPath &path, const String &methodName, const CIMParamValueArray &inParams, CIMParamValueArray &outParams)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual CIMInstance getInstance(const String &ns, const CIMObjectPath &instanceName, WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void enumQualifierTypes(const String &ns, CIMQualifierTypeResultHandlerIFC &result)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void deleteQualifierType(const String &ns, const String &qualName)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void deleteClass(const String &ns, const String &className)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void modifyInstance(const String &ns, const CIMInstance &modifiedInstance, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_INCLUDE_QUALIFIERS, const StringArray *propertyList=0)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void modifyClass(const String &ns, const CIMClass &cimClass)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void setProperty(const String &ns, const CIMObjectPath &instanceName, const String &propertyName, const CIMValue &newValue)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual CIMValue getProperty(const String &ns, const CIMObjectPath &instanceName, const String &propertyName)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void deleteInstance(const String &ns, const CIMObjectPath &path)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void associators(const String &ns, const CIMObjectPath &path, CIMInstanceResultHandlerIFC &result, const String &assocClass=String(), const String &resultClass=String(), const String &role=String(), const String &resultRole=String(), WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void associatorNames(const String &ns, const CIMObjectPath &objectName, CIMObjectPathResultHandlerIFC &result, const String &assocClass=String(), const String &resultClass=String(), const String &role=String(), const String &resultRole=String())
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void associatorsClasses(const String &ns, const CIMObjectPath &path, CIMClassResultHandlerIFC &result, const String &assocClass=String(), const String &resultClass=String(), const String &role=String(), const String &resultRole=String(), WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void referenceNames(const String &ns, const CIMObjectPath &path, CIMObjectPathResultHandlerIFC &result, const String &resultClass=String(), const String &role=String())
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void references(const String &ns, const CIMObjectPath &path, CIMInstanceResultHandlerIFC &result, const String &resultClass=String(), const String &role=String(), WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void referencesClasses(const String &ns, const CIMObjectPath &path, CIMClassResultHandlerIFC &result, const String &resultClass=String(), const String &role=String(), WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
	virtual void execQuery(const String &ns, CIMInstanceResultHandlerIFC &result, const String &query, const String &queryLanguage)
	{
		OW_THROWCIM(CIMException::FAILED);
	}
};

class NULLErrHandler : public ParserErrorHandlerIFC
{
protected:
	virtual void doProgressMessage(const char *message, const lineInfo &li)
	{
	}
	virtual void doFatalError(const char *error, const lineInfo &li)
	{
	}
	virtual EParserAction doRecoverableError(const char *error, const lineInfo &li)
	{
		return ParserErrorHandlerIFC::E_ABORT;
	}
};

CIMInstance compileInstanceFromMOF(const String& instMOF)
{
	CIMOMHandleIFCRef hdl(new StoreLocalInstancesHandle);
	MOF::Compiler::Options opts;
	Reference<ParserErrorHandlerIFC> errHandler(new NULLErrHandler);
	MOF::Compiler comp(hdl, opts, errHandler);
	comp.compileString(instMOF);
	CIMInstanceArray cia = hdl.cast_to<StoreLocalInstancesHandle>()->getInstances();
	if (cia.size() == 1)
	{
		return cia[0];
	}
	OW_THROW(MOFCompilerException, "MOF did not contain one instance");
}



} // end namespace MOF
} // end namespace OpenWBEM

