/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
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

#include "OW_MOFCompiler.hpp"
#include "OW_Format.hpp"
#include "OW_MOFParserDecls.hpp"
#include "OW_MOFCIMOMVisitor.hpp"
#include "OW_Assertion.hpp"
#include "OW_ThreadCancelledException.hpp"
#include "OW_MOFGrammar.hpp"

#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_Enumeration.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Logger.hpp"

#include <assert.h>
#include <cctype>

// forward declarations of some lex/yacc functions we need to call.
void owmof_delete_buffer(YY_BUFFER_STATE b);
YY_BUFFER_STATE owmof_scan_bytes( const char *bytes, int len );

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(MOFCompiler)

namespace MOF
{


namespace
{
// since flex/bison and our implementation aren't re-entrant or thread-safe.
Mutex g_guard;
}

Compiler::Compiler( const CIMOMHandleIFCRef& ch, const Options& opts, const ParserErrorHandlerIFCRef& mpeh )
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
	MutexLock lock(g_guard);
	include_stack_ptr = 0;
	theLineInfo = LineInfo(filename,1);
	try
	{
		try
		{
			size_t i = filename.lastIndexOf(OW_FILENAME_SEPARATOR);
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
				owmofin = fopen(filename.c_str(), "r");
				if (!owmofin)
				{
					theErrorHandler->fatalError("Unable to open file", LineInfo(filename, 0));
					return 1;
				}
			}
			theErrorHandler->progressMessage("Starting parsing",
					LineInfo(filename, 0));
			#ifdef YYOW_DEBUG
			owmofdebug = 1;
			#endif
			owmofparse(this);
			theErrorHandler->progressMessage("Finished parsing",
					theLineInfo);
			CIMOMVisitor v(m_ch, m_opts, theErrorHandler);
			mofSpecification->Accept(&v);
		}
		catch (const ParseFatalErrorException&)
		{
			// error has already been reported.
		}
		catch (AssertionException& e)
		{
			theErrorHandler->fatalError(Format( "INTERNAL COMPILER ERROR: %1", e).c_str(), theLineInfo);
		}
		catch (Exception& e)
		{
			theErrorHandler->fatalError(Format( "ERROR: %1", e).c_str(), theLineInfo);
		}
		catch (std::exception& e)
		{
			theErrorHandler->fatalError(Format( "INTERNAL COMPILER ERROR: %1", e.what() ).c_str(), theLineInfo);
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
	struct owmofBufferDeleter
	{
		owmofBufferDeleter(YY_BUFFER_STATE buf) : m_buf(buf) {}
		~owmofBufferDeleter() {owmof_delete_buffer(m_buf);}
		YY_BUFFER_STATE m_buf;
	};
}
long Compiler::compileString( const String& mof )
{
	MutexLock lock(g_guard);
	include_stack_ptr = 0;
	String filename = "string";
	theLineInfo = LineInfo(filename,1);
	try
	{
		try
		{
			YY_BUFFER_STATE buf = owmof_scan_bytes(mof.c_str(), mof.length());
			owmofBufferDeleter deleter(buf);
			theErrorHandler->progressMessage("Starting parsing",
					LineInfo(filename, 0));
			#ifdef YYOW_DEBUG
			owmofdebug = 1;
			#endif
			owmofparse(this);
			theErrorHandler->progressMessage("Finished parsing",
					theLineInfo);
			CIMOMVisitor v(m_ch, m_opts, theErrorHandler);
			mofSpecification->Accept(&v);
		}
		catch (const ParseFatalErrorException&)
		{
			// error has already been reported.
		}
		catch (AssertionException& e)
		{
			theErrorHandler->fatalError(Format( "INTERNAL COMPILER ERROR: %1", e).c_str(), LineInfo("(none)", 0));
		}
		catch (Exception& e)
		{
			theErrorHandler->fatalError(Format( "ERROR: %1", e).c_str(), LineInfo("(none)", 0));
		}
		catch (std::exception& e)
		{
			theErrorHandler->fatalError(Format( "INTERNAL COMPILER ERROR: %1", e.what() ).c_str(), LineInfo("(none)", 0));
		}
		catch (ThreadCancelledException&)
		{
			theErrorHandler->fatalError("INTERNAL COMPILER ERROR: Thread cancelled", theLineInfo);
			throw;
		}
		catch(...)
		{
			theErrorHandler->fatalError( "INTERNAL COMPILER ERROR: Unknown exception", LineInfo("(none)", 0));
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
	for (size_t i = 0; i < retval.length(); ++i)
	{
		if (retval[i] == '\\')
		{
			/* this can never happen, unless someone messes up the lexer
			if (i+1 >= retval.length())
			{
				OW_THROW(Exception, "String cannot end with '\\'");
			}*/
			++i;
			switch (retval[i])
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
	StoreLocalInstancesHandle(const CIMOMHandleIFCRef& hdl)
		: m_realhdl(hdl)
	{
	}
	virtual CIMObjectPath createInstance(const String &ns, const CIMInstance &instance)
	{
		m_instances.push_back(instance);
		return CIMObjectPath(ns, instance);
	}
	virtual CIMClass getClass(const String &ns, const String &className,
		WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_INCLUDE_QUALIFIERS,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_INCLUDE_CLASS_ORIGIN,
		const StringArray *propertyList=0)
	{
		if (m_realhdl)
		{
			return m_realhdl->getClass(ns, className, localOnly, includeQualifiers, includeClassOrigin, propertyList);
		}
		else
		{
			// just give back an empty class, with just the name set.
			return CIMClass(className);
		}
	}

	virtual CIMQualifierType getQualifierType(const String &ns, const String &qualifierName)
	{
		if (m_realhdl)
		{
			return m_realhdl->getQualifierType(ns, qualifierName);
		}
		else
		{
			OW_THROWCIM(CIMException::FAILED);
		}
	}

	CIMInstanceArray getInstances() const { return m_instances; }

private:
	CIMInstanceArray m_instances;
	CIMOMHandleIFCRef m_realhdl;

#define THROW_ERROR_NOT_IMPLEMENTED(name) OW_THROWCIMMSG(CIMException::FAILED, Format("Not implemented: %1", (name)).c_str())
#define THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME() THROW_ERROR_NOT_IMPLEMENTED(OW_LOGGER_PRETTY_FUNCTION)
	// all these throw FAILED
	virtual void setQualifierType(const String &ns, const CIMQualifierType &qualifierType)
	{
		THROW_ERROR_NOT_IMPLEMENTED(Format("Create qualifier: %1:%2", ns, qualifierType.getName()));
	}
	virtual void createClass(const String &ns, const CIMClass &cimClass)
	{
		THROW_ERROR_NOT_IMPLEMENTED(Format("Create class: %1:%2", ns, cimClass.getName()));
	}
	virtual void enumClassNames(const String &ns, const String &className, StringResultHandlerIFC &result, WBEMFlags:: EDeepFlag deep=WBEMFlags:: E_DEEP)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void close()
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void enumClass(const String &ns, const String &className, CIMClassResultHandlerIFC &result, WBEMFlags:: EDeepFlag deep=WBEMFlags:: E_SHALLOW, WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_INCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_INCLUDE_CLASS_ORIGIN)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void enumInstanceNames(const String &ns, const String &className, CIMObjectPathResultHandlerIFC &result)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void enumInstances(const String &ns, const String &className, CIMInstanceResultHandlerIFC &result, WBEMFlags:: EDeepFlag deep=WBEMFlags:: E_DEEP, WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual CIMValue invokeMethod(const String &ns, const CIMObjectPath &path, const String &methodName, const CIMParamValueArray &inParams, CIMParamValueArray &outParams)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual CIMInstance getInstance(const String &ns, const CIMObjectPath &instanceName, WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void enumQualifierTypes(const String &ns, CIMQualifierTypeResultHandlerIFC &result)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void deleteQualifierType(const String &ns, const String &qualName)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void deleteClass(const String &ns, const String &className)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void modifyInstance(const String &ns, const CIMInstance &modifiedInstance, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_INCLUDE_QUALIFIERS, const StringArray *propertyList=0)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void modifyClass(const String &ns, const CIMClass &cimClass)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void setProperty(const String &ns, const CIMObjectPath &instanceName, const String &propertyName, const CIMValue &newValue)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual CIMValue getProperty(const String &ns, const CIMObjectPath &instanceName, const String &propertyName)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void deleteInstance(const String &ns, const CIMObjectPath &path)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void associators(const String &ns, const CIMObjectPath &path, CIMInstanceResultHandlerIFC &result, const String &assocClass=String(), const String &resultClass=String(), const String &role=String(), const String &resultRole=String(), WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void associatorNames(const String &ns, const CIMObjectPath &objectName, CIMObjectPathResultHandlerIFC &result, const String &assocClass=String(), const String &resultClass=String(), const String &role=String(), const String &resultRole=String())
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void associatorsClasses(const String &ns, const CIMObjectPath &path, CIMClassResultHandlerIFC &result, const String &assocClass=String(), const String &resultClass=String(), const String &role=String(), const String &resultRole=String(), WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void referenceNames(const String &ns, const CIMObjectPath &path, CIMObjectPathResultHandlerIFC &result, const String &resultClass=String(), const String &role=String())
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void references(const String &ns, const CIMObjectPath &path, CIMInstanceResultHandlerIFC &result, const String &resultClass=String(), const String &role=String(), WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void referencesClasses(const String &ns, const CIMObjectPath &path, CIMClassResultHandlerIFC &result, const String &resultClass=String(), const String &role=String(), WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_EXCLUDE_QUALIFIERS, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_EXCLUDE_CLASS_ORIGIN, const StringArray *propertyList=0)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
	virtual void execQuery(const String &ns, CIMInstanceResultHandlerIFC &result, const String &query, const String &queryLanguage)
	{
		THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME();
	}
};

class LoggerErrHandler : public ParserErrorHandlerIFC
{
protected:
	virtual void doProgressMessage(const char *message, const LineInfo &li)
	{
		if( logger )
		{
			OW_LOG_DEBUG(logger, Format("MOF compilation progress: %1: line %2: %3", li.filename, li.lineNum, message));
		}
		warnings.push_back(message);
	}
	virtual void doFatalError(const char *error, const LineInfo &li)
	{
		if( logger )
		{
			OW_LOG_ERROR(logger, Format("Fatal MOF compilation error: %1: line %2: %3", li.filename, li.lineNum, error));
		}
		errors.push_back(error);
	}
	virtual EParserAction doRecoverableError(const char *error, const LineInfo &li)
	{
		if( logger )
		{
			OW_LOG_ERROR(logger, Format("MOF compilation error: %1: line %2: %3", li.filename, li.lineNum, error));
		}
		errors.push_back(error);
		return ParserErrorHandlerIFC::E_ABORT_ACTION;
	}
	LoggerRef logger;

public:
	LoggerErrHandler(const LoggerRef& l):
		logger(l)
	{
	}
	StringArray errors;
	StringArray warnings;
};

CIMInstance compileInstanceFromMOF(const String& instMOF, const LoggerRef& logger)
{
	CIMInstanceArray cia = compileInstancesFromMOF(instMOF, logger);
	if (cia.size() == 1)
	{
		return cia[0];
	}
	OW_THROW(MOFCompilerException, "MOF did not contain one instance");
}

CIMInstanceArray compileInstancesFromMOF(const String& instMOF, const LoggerRef& logger)
{
	return compileInstancesFromMOF(instMOF, CIMOMHandleIFCRef(), "", logger);
}

CIMInstanceArray compileInstancesFromMOF(const String& instMOF, const CIMOMHandleIFCRef& realhdl, const String& ns, const LoggerRef& logger)
{
	IntrusiveReference<StoreLocalInstancesHandle> hdl(new StoreLocalInstancesHandle(realhdl));
	MOF::Compiler::Options opts;
	opts.m_namespace = ns;
	IntrusiveReference<LoggerErrHandler> errHandler(new LoggerErrHandler(logger));
	MOF::Compiler comp(hdl, opts, errHandler);
	long errors = comp.compileString(instMOF);
	if (errors > 0)
	{
		// just report the first message, since anything else is too complicated :-{
		OW_THROW(MOFCompilerException, errHandler->errors.size() > 0 ? errHandler->errors[0].c_str() : "");
	}
	CIMInstanceArray cia = hdl->getInstances();
	return cia;
}

} // end namespace MOF
} // end namespace OpenWBEM

