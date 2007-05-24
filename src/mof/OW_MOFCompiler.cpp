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
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Logger.hpp"
#include "OW_CIMFlavor.hpp"

#include <assert.h>
#include <cctype>


// forward declarations of some lex/yacc functions we need to call.
void owmof_delete_buffer(YY_BUFFER_STATE b);
YY_BUFFER_STATE owmof_scan_bytes( const char *bytes, int len );

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(MOFCompiler)
OW_DEFINE_EXCEPTION2(MOFParser, MOFCompilerException)

namespace MOF
{


namespace
{
// since flex/bison aren't re-entrant or thread-safe.
NonRecursiveMutex g_guard;

String COMPONENT_NAME("ow.mof");
}

Compiler::Compiler( const CIMOMHandleIFCRef& ch, const Options& opts, const ParserErrorHandlerIFCRef& mpeh )
	: theErrorHandler(mpeh)
	, m_ch(ch)
	, m_opts(opts)
{
}
Compiler::~Compiler()
{
}
long Compiler::compile( const String& filename )
{
	CompilerState compilerState(theErrorHandler, filename);
	try
	{
		try
		{
			if (filename != "-")
			{
				owmofin = fopen(filename.c_str(), "r");
				if (!owmofin)
				{
					theErrorHandler->fatalError("Unable to open file", LineInfo(filename, 0, 0));
					return 1;
				}
			}
			theErrorHandler->progressMessage("Starting parsing",
					LineInfo(filename, 0, 0));
			
			{
				NonRecursiveMutexLock lock(g_guard);
				#ifdef YYOW_DEBUG
				owmofdebug = 1;
				#endif
				ParseError parseError;
				int parserv = owmofparse(&compilerState, &parseError);
				if (parserv != 0)
				{
					OW_THROW(MOFParserException, Format("Parse error at: %1:%2 (%3)", parseError.column, parseError.line, parseError.message).c_str());
				}
			}

			theErrorHandler->progressMessage("Finished parsing", compilerState.getLineInfo());
			CIMOMVisitor v(m_ch, m_opts, theErrorHandler);
			compilerState.mofSpecification->Accept(&v);
		}
		catch (const ParseFatalErrorException&)
		{
			// error has already been reported.
		}
		catch (AssertionException& e)
		{
			theErrorHandler->fatalError(Format( "INTERNAL COMPILER ERROR: %1", e).c_str(), compilerState.getLineInfo());
		}
		catch (Exception& e)
		{
			theErrorHandler->fatalError(Format( "ERROR: %1", e).c_str(), compilerState.getLineInfo());
		}
		catch (std::exception& e)
		{
			theErrorHandler->fatalError(Format( "INTERNAL COMPILER ERROR: %1", e.what() ).c_str(), compilerState.getLineInfo());
		}
		catch (ThreadCancelledException&)
		{
			theErrorHandler->fatalError("INTERNAL COMPILER ERROR: Thread cancelled", compilerState.getLineInfo());
			throw;
		}
		catch(...)
		{
			theErrorHandler->fatalError( "INTERNAL COMPILER ERROR: Unknown exception", compilerState.getLineInfo());
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
	String filename = "string";
	CompilerState compilerState(theErrorHandler, filename);
	try
	{
		try
		{
			{
				NonRecursiveMutexLock lock(g_guard);
			
				YY_BUFFER_STATE buf = owmof_scan_bytes(mof.c_str(), mof.length());
				owmofBufferDeleter deleter(buf);
				theErrorHandler->progressMessage("Starting parsing",
						LineInfo(filename, 0, 0));
				#ifdef YYOW_DEBUG
				owmofdebug = 1;
				#endif
				ParseError parseError;
				int parserv = owmofparse(&compilerState, &parseError);
				if (parserv != 0)
				{
					OW_THROW(MOFParserException, Format("Parse error at: %1:%2 (%3)", parseError.column, parseError.line, parseError.message).c_str());
				}
			}

			theErrorHandler->progressMessage("Finished parsing", compilerState.getLineInfo());
			CIMOMVisitor v(m_ch, m_opts, theErrorHandler);
			compilerState.mofSpecification->Accept(&v);
		}
		catch (const ParseFatalErrorException&)
		{
			// error has already been reported.
		}
		catch (AssertionException& e)
		{
			theErrorHandler->fatalError(Format( "INTERNAL COMPILER ERROR: %1", e).c_str(), compilerState.getLineInfo());
		}
		catch (Exception& e)
		{
			theErrorHandler->fatalError(Format( "ERROR: %1", e).c_str(), compilerState.getLineInfo());
		}
		catch (std::exception& e)
		{
			theErrorHandler->fatalError(Format( "INTERNAL COMPILER ERROR: %1", e.what() ).c_str(), compilerState.getLineInfo());
		}
		catch (ThreadCancelledException&)
		{
			theErrorHandler->fatalError("INTERNAL COMPILER ERROR: Thread cancelled", compilerState.getLineInfo());
			throw;
		}
		catch(...)
		{
			theErrorHandler->fatalError( "INTERNAL COMPILER ERROR: Unknown exception", compilerState.getLineInfo());
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
	bool inString = false;
	StringBuffer unescaped;
	for (size_t i = 0; i < s.length(); ++i)
	{
		if (inString)
		{
			if (s[i] == '\\')
			{
				++i;

				/* this can never happen, unless someone messes up the lexer */
				OW_ASSERT(i < s.length());

				switch (s[i])
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
							size_t j = 1;
							for (; j <= 4; ++j)
							{
								char c = s[i+j];
								if (isdigit(c))
								{
									hex <<= 4;
									hex |= c - '0';
								}
								else if (isxdigit(c))
								{
									hex <<= 4;
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
								OW_THROW(MOFParserException, "Escape sequence larger than supported maximum");
							}
							unescaped += static_cast<char>(hex);
							i += j - 1;
						}
						break;
					default:
						// this could never happen unless someone messes up the lexer
						OW_ASSERTMSG(0, "Invalid escape sequence");
						break;
				}
			}
			else if (s[i] == '"')
			{
				inString = false;
			}
			else
			{
				unescaped += s[i];
			}
		}
		else
		{
			// not in the string
			if (s[i] == '"')
			{
				inString = true;
			}
		}
	}

	return unescaped.releaseString();
}

class StoreLocalDataHandle : public CIMOMHandleIFC
{
public:
	StoreLocalDataHandle(const CIMOMHandleIFCRef& hdl, CIMInstanceArray& instances, CIMClassArray& classes, CIMQualifierTypeArray& qualifierTypes)
		: m_realhdl(hdl)
		, m_instances(instances)
		, m_classes(classes)
		, m_qualifierTypes(qualifierTypes)
	{
	}
	virtual CIMObjectPath createInstance(const String &ns, const CIMInstance &instance)
	{
		m_instances.push_back(instance);
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, Format("StoreLocalDataHandle::createInstance returning %1", CIMObjectPath(ns, instance).toString()));
		return CIMObjectPath(ns, instance);
	}
	virtual CIMClass getClass(const String &ns, const String &className,
		WBEMFlags:: ELocalOnlyFlag localOnly=WBEMFlags:: E_NOT_LOCAL_ONLY,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers=WBEMFlags:: E_INCLUDE_QUALIFIERS,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin=WBEMFlags:: E_INCLUDE_CLASS_ORIGIN,
		const StringArray *propertyList=0)
	{
		Logger logger(COMPONENT_NAME);
		// first get back a class contained in the mof.
		for (size_t i = 0; i < m_classes.size(); ++i)
		{
			if (m_classes[i].getName() == CIMName(className))
			{
				OW_LOG_DEBUG(logger, Format("StoreLocalDataHandle::getClass found %1 in m_classes", className));
				return m_classes[i];
			}
		}
		// try getting it from the cimom handle
		if (m_realhdl)
		{
			try
			{
				OW_LOG_DEBUG(logger, Format("StoreLocalDataHandle::getClass attempting to get %1 from m_realhdl", className));
				return m_realhdl->getClass(ns, className, localOnly, includeQualifiers, includeClassOrigin, propertyList);
			}
			catch (CIMException& e)
			{
				OW_LOG_DEBUG(logger, Format("StoreLocalDataHandle::getClass failed to get %1 from m_realhdl: %2", className, e));
				// ignore it.
			}
		}

		OW_LOG_DEBUG(logger, Format("StoreLocalDataHandle::getClass returning a dummy class of %1", className));
		// just give back an empty class, with just the name set.
		return CIMClass(className);
	}

	virtual CIMQualifierType getQualifierType(const String &ns, const String &qualifierName)
	{
		// first get back a qualifier contained in the mof.
		for (size_t i = 0; i < m_qualifierTypes.size(); ++i)
		{
			if (m_qualifierTypes[i].getName() == CIMName(qualifierName))
			{
				return m_qualifierTypes[i];
			}
		}
		// try getting it from the cimom handle
		if (m_realhdl)
		{
			return m_realhdl->getQualifierType(ns, qualifierName);
		}
		OW_THROWCIM(CIMException::FAILED);
	}

	virtual void setQualifierType(const String &ns, const CIMQualifierType &qualifierType)
	{
		m_qualifierTypes.push_back(qualifierType);
	}

	virtual void createClass(const String &ns, const CIMClass &cimClass_)
	{
		CIMClass cimClass(cimClass_);
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, Format("StoreLocalDataHandle::createClass() before adjustClass cimClass = %1",  cimClass.toMOF()));
		adjustClass(ns, cimClass);
		resolveClass(ns, cimClass);
		OW_LOG_DEBUG(logger, Format("StoreLocalDataHandle::createClass() after adjustClass cimClass = %1",  cimClass.toMOF()));
		m_classes.push_back(cimClass);
	}

private:
	CIMOMHandleIFCRef m_realhdl;
	CIMInstanceArray& m_instances;
	CIMClassArray& m_classes;
	CIMQualifierTypeArray& m_qualifierTypes;

#define THROW_ERROR_NOT_IMPLEMENTED(name) OW_THROWCIMMSG(CIMException::FAILED, Format("Not implemented: %1", (name)).c_str())
#define THROW_ERROR_NOT_IMPLEMENTED_FUNCNAME() THROW_ERROR_NOT_IMPLEMENTED(OW_LOGGER_PRETTY_FUNCTION)
	// all these throw FAILED
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

	//////////////////////////////////////////////////////////////////////////////
	void adjustClass(const String& ns, CIMClass& childClass)
	{
		Logger logger(COMPONENT_NAME);
		CIMName childName = childClass.getName();
		CIMName parentName = childClass.getSuperClass();
		CIMClass parentClass(CIMNULL);
		if (parentName != CIMName())
		{
            parentClass = this->getClass(ns, parentName.toString());
		}
		if (!parentClass)
		{
			// No parent class. Must be a base class
			CIMQualifierArray qualArray = childClass.getQualifiers();
			for (size_t i = 0; i < qualArray.size(); i++)
			{
				qualArray[i].setPropagated(false);
			}
			CIMPropertyArray propArray = childClass.getAllProperties();
			for (size_t i = 0; i < propArray.size(); i++)
			{
				propArray[i].setPropagated(false);
				propArray[i].setOriginClass(childName);
			}
			childClass.setProperties(propArray);
			CIMMethodArray methArray = childClass.getAllMethods();
			for (size_t i = 0; i < methArray.size(); i++)
			{
				methArray[i].setPropagated(false);
				methArray[i].setOriginClass(childName);
			}
			childClass.setMethods(methArray);
			return;
		}
		//////////
		// At this point we know we have a parent class
		CIMQualifierArray newQuals;
		CIMQualifierArray qualArray = childClass.getQualifiers();
		for (size_t i = 0; i < qualArray.size(); i++)
		{
			CIMQualifier qual = qualArray[i];
			CIMQualifier pqual = parentClass.getQualifier(qual.getName());
			if (pqual)
			{
				if (pqual.getValue().equal(qual.getValue()))
				{
					if (pqual.hasFlavor(CIMFlavor::RESTRICTED))
					{
						// NOT PROPAGATED.  qual.setPropagated(true);
						newQuals.append(qual);
					}
				}
				else
				{
					if (pqual.hasFlavor(CIMFlavor::DISABLEOVERRIDE))
					{
						OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parent class qualifier %1 has DISABLEOVERRIDE flavor. "
									"Child cannot override it.", pqual.getName()).c_str());
					}
					newQuals.append(qual);
				}
			}
			else
			{
				newQuals.push_back(qual);
			}
		}
		childClass.setQualifiers(newQuals);
		CIMPropertyArray propArray = childClass.getAllProperties();
		for (size_t i = 0; i < propArray.size(); i++)
		{
			CIMProperty parentProp = parentClass.getProperty(propArray[i].getName());
			if (parentProp)
			{
				if (propArray[i].getQualifier(CIMQualifier::CIM_QUAL_OVERRIDE))
				{
					if (propArray[i].getOriginClass().empty())
					{
						propArray[i].setOriginClass(childName);
						propArray[i].setPropagated(false);
					}
					else
					{
						propArray[i].setPropagated(true);
					}
					// now make sure any qualifiers are properly set
					CIMQualifierArray parentQuals = parentProp.getQualifiers();
					for (size_t j = 0; j < parentQuals.size(); ++j)
					{
						CIMQualifier& qual = parentQuals[j];
						// If the qualifier has DisableOverride flavor, the
						// subclass can't change it.  (e.g. Key). It gets the
						// parent qualifier.
						if (qual.hasFlavor(CIMFlavor::DISABLEOVERRIDE))
						{
							if (!propArray[i].getQualifier(qual.getName()))
							{
								propArray[i].addQualifier(qual);
							}
							else
							{
								// TODO: look at this message, it seems the dmtf cim schema causes it quite often.
								// maybe we should only output it if the value is different?
								Logger lgr(COMPONENT_NAME);
								OW_LOG_INFO(lgr, Format("Warning: %1.%2: qualifier %3 was "
											"overridden, but the qualifier can't be "
											"overridden because it has DisableOverride flavor",
											childClass.getName(), propArray[i].getName(),
											qual.getName()));
								propArray[i].setQualifier(qual);
							}
						}
						// If the qualifier has ToSubclass (not Restricted), then
						// only propagate it down if it's not overridden in the
						// subclass.
						else if (!qual.hasFlavor(CIMFlavor::RESTRICTED))
						{
							if (!propArray[i].getQualifier(qual.getName()))
							{
								propArray[i].addQualifier(qual);
							}
						}
					}
				}
				else
				{
					propArray[i].setOriginClass(parentProp.getOriginClass());
					propArray[i].setPropagated(true);
				}
			}
			else
			{
				// According to the 2.2 spec. If the parent class has key properties,
				// the child class cannot declare additional key properties.
				if (propArray[i].isKey())
				{
					// This is a key property, so the parent class better not be a
					// keyed class.
					if (parentClass.isKeyed())
					{
						OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parent class has keys. Child cannot have additional"
									" key properties: %1: %2", childClass.getName(), propArray[i].toMOF()).c_str());
					}
				}
				propArray[i].setOriginClass(childName);
				propArray[i].setPropagated(false);
			}
		}
		childClass.setProperties(propArray);
		CIMMethodArray methArray = childClass.getAllMethods();
		for (size_t i = 0; i < methArray.size(); i++)
		{
			if (parentClass.getMethod(methArray[i].getName()) &&
					!methArray[i].getQualifier(CIMQualifier::CIM_QUAL_OVERRIDE))
			{
				methArray[i].setOriginClass(parentName);
				methArray[i].setPropagated(true);
			}
			else
			{
				methArray[i].setOriginClass(childName);
				methArray[i].setPropagated(false);
			}
		}
		childClass.setMethods(methArray);
		if (parentClass.isKeyed())
		{
			childClass.setIsKeyed();
		}
		// Don't allow the child class to be an association if the parent class isn't.
		// This shouldn't normally happen, because the association qualifier has
		// a DISABLEOVERRIDE flavor, so it will be caught in an earlier test.
		if (childClass.isAssociation() && !parentClass.isAssociation())
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					Format("Association class is derived from non-association class: %1",
						childClass.getName()).c_str());
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	void resolveClass(const String& ns, CIMClass& child)
	{
		// Determine if any properties are keys
		CIMPropertyArray propArray = child.getAllProperties();
		for (size_t i = 0; i < propArray.size(); i++)
		{
			if (propArray[i].isKey())
			{
				child.setIsKeyed(true);
				break;
			}
		}
		CIMClass parentClass(CIMNULL);
		CIMName superID = child.getSuperClass();
		// If class doesn't have a super class - don't propagate anything
		if (superID == CIMName())
		{
			return;
		}
		parentClass = this->getClass(ns, superID.toString());
		if (parentClass.isKeyed())
		{
			child.setIsKeyed(true);
		}
		// Propagate appropriate class qualifiers
		CIMQualifierArray qualArray = parentClass.getQualifiers();
		for (size_t i = 0; i < qualArray.size(); i++)
		{
			CIMQualifier qual = qualArray[i];
			if (!qual.hasFlavor(CIMFlavor::RESTRICTED))
			//if (qual.hasFlavor(CIMFlavor::TOSUBCLASS))
			{
				if (!child.hasQualifier(qual))
				{
					qual.setPropagated(true);
					child.addQualifier(qual);
				}
			}
		}
		// Propagate Properties from parent class.
		//
		// TODO: Regardless of whether there is an override
		// this will perform override like behavior - probably
		// need to add validation code...
		//
		propArray = parentClass.getAllProperties();
		for (size_t i = 0; i < propArray.size(); i++)
		{
			CIMProperty parentProp = propArray[i];
			CIMProperty childProp = child.getProperty(parentProp.getName());
			if (!childProp)
			{
				parentProp.setPropagated(true);
				child.addProperty(parentProp);
			}
			else if (!childProp.getQualifier(CIMQualifier::CIM_QUAL_OVERRIDE))
			{
				//
				// Propagate any qualifiers that have not been
				// re-defined
				//
				qualArray = parentProp.getQualifiers();
				for (size_t qi = 0; qi < qualArray.size(); qi++)
				{
					CIMQualifier parentQual = qualArray[qi];
					if (!childProp.getQualifier(parentQual.getName()))
					{
						//
						// Qualifier not defined on child property
						// so propagate it
						//
						parentQual.setPropagated(true);
						childProp.addQualifier(parentQual);
					}
				}
				child.setProperty(childProp);
			}
		}
		// Propagate methods from parent class
		CIMMethodArray methods = parentClass.getAllMethods();
		for (size_t i = 0; i < methods.size(); i++)
		{
			CIMMethod cm = methods[i];
			CIMMethod childMethod = child.getMethod(methods[i].getName());
			if (!childMethod)
			{
				cm.setPropagated(true);
				child.addMethod(cm);
			}
			else if (!childMethod.getQualifier(CIMQualifier::CIM_QUAL_OVERRIDE))
			{
				//
				// Propagate any qualifiers that have not been
				// re-defined by the method declaration
				//
				qualArray = cm.getQualifiers();
				for (size_t mi = 0; mi < qualArray.size(); mi++)
				{
					CIMQualifier methQual = qualArray[mi];
					if (!childMethod.getQualifier(methQual.getName()))
					{
						methQual.setPropagated(true);
						childMethod.addQualifier(methQual);
					}
				}
				child.setMethod(childMethod);
			}
		}
	}

};

class LoggerErrHandler : public ParserErrorHandlerIFC
{
protected:
	virtual void doProgressMessage(const char *message, const LineInfo &li)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, Format("MOF compilation progress: %1: line %2:%3: %4", li.filename, li.lineNum, li.columnNum, message));
		warnings.push_back(message);
	}
	virtual void doFatalError(const char *error, const LineInfo &li)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_ERROR(logger, Format("Fatal MOF compilation error: %1: line %2:%3: %4", li.filename, li.lineNum, li.columnNum, error));
		errors.push_back(error);
	}
	virtual EParserAction doRecoverableError(const char *error, const LineInfo &li)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_ERROR(logger, Format("MOF compilation error: %1: line %2:%3: %4", li.filename, li.lineNum, li.columnNum, error));
		errors.push_back(error);
		return ParserErrorHandlerIFC::E_ABORT_ACTION;
	}

public:
	LoggerErrHandler()
	{
	}
	StringArray errors;
	StringArray warnings;
};

void compileMOF(const String& mof, const CIMOMHandleIFCRef& realhdl, const String& ns,
	CIMInstanceArray& instances, CIMClassArray& classes, CIMQualifierTypeArray& qualifierTypes)
{
	IntrusiveReference<StoreLocalDataHandle> hdl(new StoreLocalDataHandle(realhdl, instances, classes, qualifierTypes));
	MOF::Compiler::Options opts;
	opts.m_namespace = ns;
	IntrusiveReference<LoggerErrHandler> errHandler(new LoggerErrHandler());
	MOF::Compiler comp(hdl, opts, errHandler);
	long errors = comp.compileString(mof);
	if (errors > 0)
	{
		StringBuffer errorStrs;
		for (size_t i = 0; i < errHandler->errors.size(); ++i)
		{
			if (i > 0)
			{
				errorStrs += '\n';
			}
			errorStrs += errHandler->errors[i];
		}
		OW_THROW(MOFCompilerException, errorStrs.c_str());
	}
}

CompilerState::CompilerState(const ParserErrorHandlerIFCRef& mpeh, const String& initialFilename)
	: m_firstColumn(0)
	, m_firstLine(0)
	, m_lastColumn(0)
	, m_lastLine(0)
	, m_nextColumn(1)
	, m_nextLine(1)
	, m_fileName(initialFilename)
	, m_errorHandler(mpeh)
{
	size_t i = initialFilename.lastIndexOf(OW_FILENAME_SEPARATOR);
	if (i != String::npos)
	{
		m_basepath = initialFilename.substring(0,i);
	}
}

CompilerState::~CompilerState()
{
}

void 
CompilerState::updateLocation(const char* yytext, owmofltype* yylocp)
{
	m_firstLine = m_lastLine = m_nextLine;
	m_firstColumn = m_nextColumn;
	char c;
	for (std::size_t i = 0; (c = yytext[i]) != '\0'; ++i)
	{
		m_lastColumn = m_nextColumn;
		++m_nextColumn;
		if (c == '\n')
		{
			m_lastLine = m_nextLine;
			++m_nextLine;
			m_nextColumn = 1;
		}
	}
	yylocp->first_column = m_firstColumn;
	yylocp->first_line = m_firstLine;
	yylocp->last_column = m_lastColumn;
	yylocp->last_line = m_lastLine;
}

LineInfo 
CompilerState::getLineInfo() const
{
	return LineInfo(m_fileName, m_firstLine, m_firstColumn);
}

ParserErrorHandlerIFCRef 
CompilerState::getErrorHandler() const
{
	return m_errorHandler;
}

void 
CompilerState::startNewFile(const String& filenameWithPath)
{
	m_firstColumn = 0;
	m_firstLine = 0;
	m_lastColumn = 0;
	m_lastLine = 0;
	m_nextColumn = 1;
	m_nextLine = 1;
	m_fileName = filenameWithPath;
}

String
CompilerState::getBasePath() const
{
	return m_basepath;
}

void
CompilerState::setLineInfo(const CompilerState::include_t& x)
{
	m_firstColumn = x.m_firstColumn;
	m_firstLine = x.m_firstLine;
	m_lastColumn = x.m_lastColumn;
	m_lastLine = x.m_lastLine;
	m_nextColumn = x.m_nextColumn;
	m_nextLine = x.m_nextLine;
	m_fileName = x.m_fileName;
}

} // end namespace MOF
} // end namespace OW_NAMESPACE

