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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_XMLExecute.hpp"
#include "OW_Format.hpp"
#include "OW_XMLClass.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_XMLEscape.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_SocketException.hpp"
#include "OW_Logger.hpp"
#include "OW_OperationContext.hpp"
#include "OW_ExceptionIds.hpp"

#include <algorithm>

#define OW_LOGDEBUG(msg) this->getEnvironment()->getLogger()->logDebug(msg)
#define OW_LOGINFO(msg) this->getEnvironment()->getLogger()->logInfo(msg)
#define OW_LOGERROR(msg) this->getEnvironment()->getLogger()->logError(msg)
#define OW_LOGFATALERROR(msg) this->getEnvironment()->getLogger()->logFatalError(msg)

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(BadStream)
OW_DEFINE_EXCEPTION_WITH_ID(BadStream)
using std::ostream;
using namespace WBEMFlags;

template<typename T> inline static void checkStream(T& str)
{
	if (!str.good())
	{
		OW_THROW(BadStreamException, "The stream is bad");
	}
}
XMLExecute::FuncEntry XMLExecute::g_funcs[] =
{
	// This list must be kept in alphabetical order!
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	{ "associatornames", &XMLExecute::associatorNames },
	{ "associators", &XMLExecute::associators },
#endif
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	{ "createclass", &XMLExecute::createClass },
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	{ "createinstance", &XMLExecute::createInstance },
#endif
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	{ "deleteclass", &XMLExecute::deleteClass },
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	{ "deleteinstance", &XMLExecute::deleteInstance },
#endif
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	{ "deletequalifier", &XMLExecute::deleteQualifier },
#endif
	{ "enumerateclasses", &XMLExecute::enumerateClasses },
	{ "enumerateclassnames", &XMLExecute::enumerateClassNames },
	{ "enumerateinstancenames", &XMLExecute::enumerateInstanceNames },
	{ "enumerateinstances", &XMLExecute::enumerateInstances },
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	{ "enumeratequalifiers", &XMLExecute::enumerateQualifiers },
#endif
	{ "execquery", &XMLExecute::execQuery },
	{ "getclass", &XMLExecute::getClass },
	{ "getinstance", &XMLExecute::getInstance },
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	{ "getproperty", &XMLExecute::getProperty },
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	{ "getqualifier", &XMLExecute::getQualifier },
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	{ "modifyclass", &XMLExecute::modifyClass },
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	{ "modifyinstance", &XMLExecute::modifyInstance },
#endif
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	{ "referencenames", &XMLExecute::referenceNames },
	{ "references", &XMLExecute::references },
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	{ "setproperty", &XMLExecute::setProperty },
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	{ "setqualifier", &XMLExecute::setQualifier },
#endif
	{ "garbage", 0 }
};
XMLExecute::FuncEntry* XMLExecute::g_funcsEnd = &XMLExecute::g_funcs[0] +
	(sizeof(XMLExecute::g_funcs)/sizeof(*XMLExecute::g_funcs)) - 1;
//////////////////////////////////////////////////////////////////////////////
bool
XMLExecute::funcEntryCompare(const XMLExecute::FuncEntry& f1,
	const XMLExecute::FuncEntry& f2)
{
	return (strcmp(f1.name, f2.name) < 0);
}
//////////////////////////////////////////////////////////////////////////////
XMLExecute::XMLExecute()
	: RequestHandlerIFCXML(),
	m_ostrEntity(NULL),
	m_ostrError(NULL),
	m_isIntrinsic(false),
	m_functionName()
{
}
//////////////////////////////////////////////////////////////////////////////
XMLExecute::~XMLExecute()
{
}
//////////////////////////////////////////////////////////////////////////////
// Private
int
XMLExecute::executeXML(CIMXMLParser& parser, ostream* ostrEntity,
	ostream* ostrError, OperationContext& context)
{
	clearError();
	m_ostrEntity = ostrEntity;
	m_ostrError = ostrError;
	m_isIntrinsic = false;
	String messageId = parser.mustGetAttribute(CIMXMLParser::A_ID);
	parser.getChild();
	if (!parser)
	{
		OW_THROW(CIMErrorException, CIMErrorException::request_not_loosely_valid);
	}
	makeXMLHeader(messageId, *m_ostrEntity);
	if (parser.getToken() == CIMXMLParser::E_MULTIREQ)
	{
		(*m_ostrEntity) << "<MULTIRSP>";
		parser.getChild();
		while (parser.tokenIsId(CIMXMLParser::E_SIMPLEREQ))
		{
			TempFileStream ostrEnt, ostrErr(500);
			processSimpleReq(parser, ostrEnt, ostrErr, context);
			if (hasError())
			{
				(*m_ostrEntity) << ostrErr.rdbuf();
				clearError();
			}
			else
			{
				(*m_ostrEntity) << ostrEnt.rdbuf();
			}
			parser.getNextTag();
			parser.mustGetEndTag();
		} // while
		(*m_ostrEntity) << "</MULTIRSP>";
	} // if MULTIRSP
	else if (parser.getToken() == CIMXMLParser::E_SIMPLEREQ)
	{
		makeXMLHeader(messageId, *m_ostrError);
		processSimpleReq(parser, *m_ostrEntity, *m_ostrError, context);
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED,
			"No <SIMPLEREQ> or <MULTIREQ> tag");
	}
	if (!hasError())
	{
		(*m_ostrEntity) << "</MESSAGE></CIM>\r\n";
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::executeIntrinsic(ostream& ostr,
	CIMXMLParser& parser, CIMOMHandleIFC& hdl,
	const String& ns)
{
	String functionNameLC = m_functionName;
	functionNameLC.toLowerCase();
	OW_LOGDEBUG(Format("Got function name. calling function %1",
		functionNameLC));
	FuncEntry fe = { 0, 0 };
	fe.name = functionNameLC.c_str();
	FuncEntry* i = std::lower_bound(g_funcs, g_funcsEnd, fe, funcEntryCompare);
	if (i == g_funcsEnd || strcmp((*i).name, fe.name) != 0)
	{
		// they sent over an intrinsic method call we don't know about
		OW_THROWCIM(CIMException::NOT_SUPPORTED);
	}
	else
	{
		ostr << "<IMETHODRESPONSE NAME=\"" << m_functionName <<
			"\">";
		// call the member function that was found
		(this->*((*i).func))(ostr, parser, ns, hdl);
		ostr << "</IMETHODRESPONSE>";
	}
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::executeExtrinsic(ostream& ostr, CIMXMLParser& parser,
	CIMOMHandleIFC& lch)
{
	ostr << "<METHODRESPONSE NAME=\"" << m_functionName <<
		"\">";
	doInvokeMethod(ostr, parser, m_functionName, lch);
	ostr << "</METHODRESPONSE>";
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::doInvokeMethod(ostream& ostr, CIMXMLParser& parser,
	const String& methodName, CIMOMHandleIFC& hdl)
{
	CIMParamValueArray inParams;
	CIMParamValueArray outParams;
	CIMObjectPath path = XMLCIMFactory::createObjectPath(parser);
	
	getParameters(parser, inParams);
	CIMValue cv = hdl.invokeMethod(path.getNameSpace(), path, methodName,
		inParams, outParams);
	if (cv)
	{
		ostr << "<RETURNVALUE PARAMTYPE=\"";
		CIMtoXML(cv.getCIMDataType(), ostr);
		ostr << "\">";
		CIMtoXML(cv, ostr);
		ostr << "</RETURNVALUE>";
	}
	for (size_t i=0; i < outParams.size(); i++)
	{
		CIMParamValueToXML(outParams[i], ostr);
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
//
// Reads paramters for extrinsinc method calls
//
// Paramlist is the list of parameters (in) for this method, params will
// be returned with input parameters in the same order as IN's were found in the
// paramlist.
//
// paramlist is a vector of all the IN CIMParameters
// params is a vector of CIMValues
//
void
XMLExecute::getParameters(CIMXMLParser& parser,
	CIMParamValueArray& params)
{
	//
	// Process parameters
	//
	while (parser.tokenIsId(CIMXMLParser::E_PARAMVALUE))
	{
		String parameterName = parser.mustGetAttribute(CIMXMLParser::A_NAME);
		String parameterType = parser.getAttribute(CIMXMLParser::A_PARAMTYPE);
		if (parameterType.empty())
		{
			parameterType = "string";
		}
		parser.getNextTag();
		int token = parser.getToken();
		if (token != CIMXMLParser::E_VALUE
			&& token != CIMXMLParser::E_VALUE_REFERENCE
			&& token != CIMXMLParser::E_VALUE_ARRAY
			&& token != CIMXMLParser::E_VALUE_REFARRAY
			)
		{
			params.push_back(CIMParamValue(parameterName,
				CIMValue(CIMNULL)));
		}
		else
		{
			params.push_back(CIMParamValue(parameterName,
				XMLCIMFactory::createValue(parser, parameterType)));
		}
		parser.mustGetEndTag(); // pass </PARAMVALUE>
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMObjectPathXMLOutputter : public CIMObjectPathResultHandlerIFC
	{
	public:
		CIMObjectPathXMLOutputter(ostream& ostr_, const String& host)
		: ostr(ostr_)
		, m_host(host)
		{}
	protected:
		virtual void doHandle(const CIMObjectPath &cop_)
		{
			ostr << "<OBJECTPATH>";
			// Make sure all outgoing object paths have our host name, instead of 127.0.0.1
			CIMObjectPath cop(cop_);
			if (cop.getFullNameSpace().isLocal())
			{
				try
				{
					cop.setHost(m_host);
				}
				catch (const SocketException& e)
				{
				}
			}
			if (cop.isClassPath())
			{
				CIMClassPathtoXML(cop, ostr);
			}
			else
			{
				CIMInstancePathtoXML(cop, ostr);
			}
			ostr << "</OBJECTPATH>";
			checkStream(ostr);
		}
	private:
		ostream& ostr;
		String m_host;
	};
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct param
	{
		enum Type
		{
			CLASSNAME,
			BOOLEAN,
			STRINGARRAY,
			INSTANCENAME,
			NAMEDINSTANCE,
			STRING,
			OBJECTNAME,
			PROPERTYVALUE
		};
		String name;
		bool optional;
		Type type;
		CIMValue defaultVal;
		bool isSet;
		CIMValue val;
		param(const String& name_,
			bool optional_ = true,
			Type type_ = STRING,
			const CIMValue& defaultVal_ = CIMValue(CIMNULL))
			: name(name_)
			, optional(optional_)
			, type(type_)
			, defaultVal(defaultVal_)
			, isSet(false)
			, val(CIMNULL)
		{}
	};
	struct name_comparer
	{
		name_comparer(const String& s_)
		: s(s_)
		{}
		bool operator()(const param& p)
		{
			return p.name.equalsIgnoreCase(s);
		}
		String s;
	};
	//////////////////////////////////////////////////////////////////////////////
	void getParameterValues(CIMXMLParser& parser,
		Array<param>& params)
	{
		// scan all the parameters and set them
		while (parser.tokenIsId(CIMXMLParser::E_IPARAMVALUE))
		{
			String name = parser.mustGetAttribute(CIMXMLParser::A_NAME);
			Array<param>::iterator i = std::find_if (params.begin(), params.end(),
				name_comparer(name));
			if (i == params.end())
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					Format("Parameter %1 is not a valid parameter", name).c_str());
			}
			
			parser.getNextTag();
			if (parser.tokenIsId(CIMXMLParser::E_IPARAMVALUE))
			{
				// pointing at </IPARAMVALUE>, thus
				// IPARAMVALUE was empty, so the value is NULL
				i->isSet = true;
				// should already be this: i->val = CIMValue();
			}
			else
			{
				switch (i->type)
				{
					case param::CLASSNAME:
						if (!parser.tokenIsId(CIMXMLParser::E_CLASSNAME))
						{
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parameter %1 is the wrong type.  Expected <CLASSNAME> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = CIMValue(XMLCIMFactory::createObjectPath(parser).getClassName());
						break;
					case param::BOOLEAN:
						if (!parser.tokenIsId(CIMXMLParser::E_VALUE))
						{
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parameter %1 is the wrong type.  Expected <VALUE> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = XMLCIMFactory::createValue(parser, "boolean");
						break;
					case param::STRINGARRAY:
						if (!parser.tokenIsId(CIMXMLParser::E_VALUE_ARRAY))
						{
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parameter %1 is the wrong type.  Expected <VALUE.ARRAY> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = XMLCIMFactory::createValue(parser, "string");
						break;
					case param::INSTANCENAME:
						if (!parser.tokenIsId(CIMXMLParser::E_INSTANCENAME))
						{
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parameter %1 is the wrong type.  Expected <INSTANCENAME> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = CIMValue(XMLCIMFactory::createObjectPath(parser));
						break;
					case param::NAMEDINSTANCE:
					{
						if (!parser.tokenIsId(CIMXMLParser::E_VALUE_NAMEDINSTANCE))
						{
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parameter %1 is the wrong type.  Expected <VALUE.NAMEDINSTANCE> tag. %2",
									i->name, parser).c_str());
						}
						i->isSet = true;
						parser.mustGetChildId(CIMXMLParser::E_INSTANCENAME);
						CIMObjectPath ipath(XMLCIMFactory::createObjectPath(parser));
						CIMInstance inst(XMLCIMFactory::createInstance(parser));
						parser.mustGetEndTag(); // pass </VALUE.NAMEDINSTANCE>
						inst.setKeys(ipath.getKeys());
						i->val = CIMValue(inst);
						break;
					}
					case param::STRING:
						if (!parser.tokenIsId(CIMXMLParser::E_VALUE))
						{
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parameter %1 is the wrong type.  Expected <VALUE> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = XMLCIMFactory::createValue(parser, "string");
						break;
					case param::OBJECTNAME:
						if (!parser.tokenIsId(CIMXMLParser::E_INSTANCENAME)
							&& !parser.tokenIsId(CIMXMLParser::E_CLASSNAME))
						{
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parameter %1 is the wrong type.  Expected <INSTANCENAME> or <CLASSNAME> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = CIMValue(XMLCIMFactory::createObjectPath(parser));
						break;
					case param::PROPERTYVALUE:
						if (!parser.tokenIsId(CIMXMLParser::E_VALUE)
							&& !parser.tokenIsId(CIMXMLParser::E_VALUE_ARRAY)
							&& !parser.tokenIsId(CIMXMLParser::E_VALUE_REFERENCE))
						{
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								Format("Parameter %1 is the wrong type.  Expected <VALUE> or <VALUE.ARRAY> or <VALUE.REFERENCE> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = XMLCIMFactory::createValue(parser, "string");
						break;
					default:
						OW_ASSERT(0);
						break;
				}
			}
			parser.mustGetEndTag(); // pass </IPARAMVALUE>
		}
		// make sure all the parameters were set that don't have defaults
		// and assign the defaults to those that weren't set.
		for (size_t i = 0; i < params.size(); ++i)
		{
			if (params[i].optional == false && params[i].isSet == false)
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					Format("Non-optional parameter %1 was not given",
						params[i].name).c_str());
			}
			if (params[i].isSet == false)
			{
				params[i].val = params[i].defaultVal;
			}
			// make sure that booleans and strings always have a valid value
			if (!params[i].val)
			{
				switch (params[i].type)
				{
					case param::BOOLEAN:
						params[i].val = CIMValue(false);
						break;
					case param::STRING:
					case param::CLASSNAME:
						params[i].val = CIMValue("");
						break;
					
					default:
						break;
				}
			}
		}
	}
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::associatorNames(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ObjectName, false, param::OBJECTNAME));
	params.push_back(param(CIMXMLParser::P_AssocClass, true, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_ResultClass, true, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_Role, true, param::STRING, CIMValue("")));
	params.push_back(param(CIMXMLParser::P_ResultRole, true, param::STRING, CIMValue("")));
	getParameterValues(parser, params);
	CIMObjectPath objectName = params[0].val.toCIMObjectPath();
	String assocClass;
	if (params[1].isSet)
	{
		assocClass = params[1].val.toString();
	}
	String resultClass;
	if (params[2].isSet)
	{
		resultClass = params[2].val.toString();
	}
	ostr << "<IRETURNVALUE>";
	CIMObjectPathXMLOutputter handler(ostr, getHost());
	hdl.associatorNames(ns, objectName, handler, assocClass, resultClass,
		params[3].val.toString(), params[4].val.toString());
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class AssocCIMInstanceXMLOutputter : public CIMInstanceResultHandlerIFC
	{
	public:
		AssocCIMInstanceXMLOutputter(
			std::ostream& ostr_,
			const String& ns_,
			const String& host_)
		: ostr(ostr_)
		, ns(ns_)
		, m_host(host_)
		{}
	protected:
		virtual void doHandle(const CIMInstance &ci)
		{
			ostr <<  "<VALUE.OBJECTWITHPATH>";
			CIMObjectPath cop( ns, ci );
			// Make sure all outgoing object paths have our host name, instead of 127.0.0.1
			cop.setHost(m_host);
			CIMInstancePathAndInstancetoXML(ci, ostr, cop);
			ostr << "</VALUE.OBJECTWITHPATH>\n";
			checkStream(ostr);
		
		}
		std::ostream& ostr;
		String ns;
		String m_host;
	};
	class AssocCIMClassXMLOutputter : public CIMClassResultHandlerIFC
	{
	public:
		AssocCIMClassXMLOutputter(
			std::ostream& ostr_,
			const String& ns_)
		: ostr(ostr_)
		, ns(ns_)
		{}
	protected:
		virtual void doHandle(const CIMClass &cc)
		{
			ostr <<  "<VALUE.OBJECTWITHPATH>";
			CIMObjectPath cop(cc.getName(), ns);
			CIMClassPathtoXML(cop,ostr);
			CIMtoXML(cc, ostr);
			ostr << "</VALUE.OBJECTWITHPATH>\n";
			checkStream(ostr);
		
		}
		std::ostream& ostr;
		const String& ns;
	};
}
//////////////////////////////////////////////////////////////////////////////
void XMLExecute::associators(ostream& ostr,
	CIMXMLParser& parser, const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ObjectName, false, param::OBJECTNAME));
	params.push_back(param(CIMXMLParser::P_AssocClass, true, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_ResultClass, true, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_Role, true, param::STRING, CIMValue("")));
	params.push_back(param(CIMXMLParser::P_ResultRole, true, param::STRING, CIMValue("")));
	params.push_back(param(CIMXMLParser::P_IncludeQualifiers, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_IncludeClassOrigin, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_PropertyList, true, param::STRINGARRAY, CIMValue(CIMNULL)));
	getParameterValues(parser, params);
	CIMObjectPath objectName = params[0].val.toCIMObjectPath();
	String assocClass;
	if (params[1].isSet)
	{
		assocClass = params[1].val.toString();
	}
	String resultClass;
	if (params[2].isSet)
	{
		resultClass = params[2].val.toString();
	}
	StringArray propertyList;
	StringArray* pPropList = 0;
	if (params[7].isSet)
	{
		propertyList = params[7].val.toStringArray();
		pPropList = &propertyList;
	}
	
	bool includeQualifiers = params[5].val.toBool();
	bool includeClassOrigin = params[6].val.toBool();
	String role = params[3].val.toString();
	String resultRole = params[4].val.toString();
	ostr << "<IRETURNVALUE>";
	if (objectName.isClassPath())
	{
		// class path
		AssocCIMClassXMLOutputter handler(ostr, ns);
		hdl.associatorsClasses(ns, objectName, handler,
			assocClass, resultClass, role, resultRole,
			includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			includeClassOrigin ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			pPropList);
	}
	else
	{
		// instance path
		AssocCIMInstanceXMLOutputter handler(ostr, ns,
			getHost());
		hdl.associators(ns, objectName, handler,
			assocClass, resultClass, role, resultRole,
			includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			includeClassOrigin ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			pPropList);
	}
	ostr << "</IRETURNVALUE>";
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void XMLExecute::createClass(ostream& /*ostr*/, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	parser.mustGetChild();
	hdl.createClass( ns, XMLCIMFactory::createClass(parser) );
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::modifyClass(ostream&	/*ostr*/, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	String name = parser.mustGetAttribute(CIMXMLParser::A_NAME);
	if (!name.equalsIgnoreCase(CIMXMLParser::P_ModifiedClass))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			Format("Parameter name was %1", name).c_str());
	}
	parser.mustGetChild();
	//
	// Process <CLASS> element
	//
	CIMClass cimClass = XMLCIMFactory::createClass(parser);
	hdl.modifyClass(ns, cimClass);
}
//////////////////////////////////////////////////////////////////////////////
void XMLExecute::deleteClass(ostream& /*ostr*/, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ClassName, false, param::CLASSNAME));
	getParameterValues(parser, params);
	String className = params[0].val.toString();
	hdl.deleteClass(ns, className);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void XMLExecute::createInstance(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	parser.mustGetChild();		// Point parser to <INSTANCE> tag
	CIMInstance cimInstance = XMLCIMFactory::createInstance(parser);
	String className = cimInstance.getClassName();
	//CIMObjectPath realPath = CIMObjectPath(className, path.getNameSpace());
	// Special treatment for __Namespace class
	if (className.equalsIgnoreCase(CIMClass::NAMESPACECLASS))
	{
		CIMProperty prop = cimInstance.getProperty(
			CIMProperty::NAME_PROPERTY);
		// Need the name property since it contains the name of the new
		// name space
		if (!prop)
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				"Name property not specified for new namespace");
		}
		// If the name property didn't come across as a key, then
		// set the name property as the key
		if (!prop.isKey())
		{
			prop.addQualifier(CIMQualifier::createKeyQualifier());
		}
		cimInstance.setProperty(prop);
	}
	ostr << "<IRETURNVALUE>";
	CIMObjectPath newPath = hdl.createInstance(ns, cimInstance);
	CIMInstanceNametoXML(newPath, ostr);
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::modifyInstance(ostream&	/*ostr*/, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ModifiedInstance, false, param::NAMEDINSTANCE));
	params.push_back(param(CIMXMLParser::P_IncludeQualifiers, true, param::BOOLEAN, CIMValue(true)));
	params.push_back(param(CIMXMLParser::P_PropertyList, true, param::STRINGARRAY, CIMValue(CIMNULL)));
	
	getParameterValues(parser, params);
	
	StringArray propertyList;
	StringArray* pPropList = 0;
	if (params[2].isSet)
	{
		propertyList = params[2].val.toStringArray();
		pPropList = &propertyList;
	}
	
	bool includeQualifiers = params[1].val.toBool();
	CIMInstance modifiedInstance(CIMNULL);
	params[0].val.get(modifiedInstance);
	hdl.modifyInstance(ns, modifiedInstance,
		includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
		pPropList);
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::deleteInstance(ostream&	/*ostr*/, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	String name = parser.getAttribute( CIMXMLParser::A_NAME );
	if ( !name.equalsIgnoreCase( "InstanceName" ) )
	{
		OW_THROWCIMMSG( CIMException::INVALID_PARAMETER,
			String( "Parameter name was " + name ).c_str() );
	}
	parser.mustGetChild();
	CIMObjectPath instPath = XMLCIMFactory::createObjectPath(parser);
	hdl.deleteInstance( ns, instPath );
}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::setProperty(ostream&	/*ostr*/, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_InstanceName, false, param::INSTANCENAME));
	params.push_back(param(CIMXMLParser::P_PropertyName, false, param::STRING, CIMValue("")));
	params.push_back(param(CIMXMLParser::P_NewValue, true, param::PROPERTYVALUE));
	getParameterValues(parser, params);
	CIMObjectPath instpath = params[0].val.toCIMObjectPath();
	hdl.setProperty(ns, instpath, params[1].val.toString(), params[2].val);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::setQualifier(ostream& /*ostr*/, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	String argName = parser.mustGetAttribute(CIMXMLParser::A_NAME);
	if (!argName.equalsIgnoreCase(CIMXMLParser::P_QualifierDeclaration))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"invalid qualifier xml");
	}
	parser.mustGetChildId(
		CIMXMLParser::E_QUALIFIER_DECLARATION);
	
	CIMQualifierType cimQualifier;
	XMLQualifier::processQualifierDecl(parser, cimQualifier);
	hdl.setQualifierType(ns, cimQualifier);
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::deleteQualifier(ostream& /*ostr*/, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	String qualName = XMLQualifier::getQualifierName(parser);
	hdl.deleteQualifierType(ns, qualName);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMQualifierTypeXMLOutputter : public CIMQualifierTypeResultHandlerIFC
	{
	public:
		CIMQualifierTypeXMLOutputter(
			std::ostream& ostr_)
		: ostr(ostr_)
		{}
	protected:
		virtual void doHandle(const CIMQualifierType &i)
		{
			CIMtoXML(i, ostr);
			checkStream(ostr);
		}
		std::ostream& ostr;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::enumerateQualifiers(ostream& ostr, CIMXMLParser& /*parser*/,
	const String& ns, CIMOMHandleIFC& hdl)
{
	ostr << "<IRETURNVALUE>";
	CIMQualifierTypeXMLOutputter handler(ostr);
	hdl.enumQualifierTypes(ns, handler);
	ostr << "</IRETURNVALUE>";
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class ClassNameXMLWriter : public StringResultHandlerIFC
	{
	public:
		ClassNameXMLWriter(std::ostream& ostr_) : ostr(ostr_) {}
	protected:
		virtual void doHandle(const String &name)
		{
			ostr << "<CLASSNAME NAME=\"" << name <<
				"\"/>";
			checkStream(ostr);
		}
	private:
		std::ostream& ostr;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::enumerateClassNames(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ClassName, true, param::CLASSNAME, CIMValue("")));
	params.push_back(param(CIMXMLParser::P_DeepInheritance, true, param::BOOLEAN, CIMValue(false)));
	getParameterValues(parser, params);
	String className = params[0].val.toString();
	EDeepFlag deepInheritance(
		params[1].val.toBool() ? E_DEEP : E_SHALLOW);
	ostr << "<IRETURNVALUE>";
	ClassNameXMLWriter handler(ostr);
	hdl.enumClassNames(ns, className, handler,
		deepInheritance);
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMClassXMLOutputter : public CIMClassResultHandlerIFC
	{
	public:
		CIMClassXMLOutputter(ostream& ostr_)
		: ostr(ostr_)
		{}
	protected:
		virtual void doHandle(const CIMClass &c)
		{
			CIMtoXML(c, ostr);
			checkStream(ostr);
		}
	private:
		ostream& ostr;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::enumerateClasses( ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ClassName, true, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_DeepInheritance, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_LocalOnly, true, param::BOOLEAN, CIMValue(true)));
	params.push_back(param(CIMXMLParser::P_IncludeQualifiers, true, param::BOOLEAN, CIMValue(true)));
	params.push_back(param(CIMXMLParser::P_IncludeClassOrigin, true, param::BOOLEAN, CIMValue(false)));
	getParameterValues(parser, params);
	String className;
	if (params[0].isSet)
	{
		className = params[0].val.toString();
	}
	ostr << "<IRETURNVALUE>";
	bool deep = params[1].val.toBool();
	bool localOnly = params[2].val.toBool();
	bool includeQualifiers = params[3].val.toBool();
	bool includeClassOrigin = params[4].val.toBool();
	CIMClassXMLOutputter handler(ostr);
	hdl.enumClass(ns, className, handler,
		deep ? E_DEEP : E_SHALLOW,
		localOnly ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
		includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
		includeClassOrigin ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMInstanceNameXMLOutputter : public CIMObjectPathResultHandlerIFC
	{
	public:
		CIMInstanceNameXMLOutputter(ostream& ostr_)
		: ostr(ostr_)
		{}
	protected:
		virtual void doHandle(const CIMObjectPath &cop)
		{
			CIMInstanceNametoXML(cop, ostr);
			checkStream(ostr);
		}
	private:
		ostream& ostr;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::enumerateInstanceNames(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ClassName, false, param::CLASSNAME));
	getParameterValues(parser, params);
	String className = params[0].val.toString();
	
	ostr << "<IRETURNVALUE>";
	CIMInstanceNameXMLOutputter handler(ostr);
	hdl.enumInstanceNames(ns, className, handler);
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMInstanceXMLOutputter : public CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceXMLOutputter(
			std::ostream& ostr_,
			const String& ns_)
		: ostr(ostr_)
		, ns(ns_)
		{}
	protected:
		virtual void doHandle(const CIMInstance &i)
		{
			const CIMInstance& cimInstance = i;
			CIMObjectPath cop(ns, cimInstance);
			ostr << "<VALUE.NAMEDINSTANCE>";
			CIMInstanceNameAndInstancetoXML(cimInstance, ostr, cop);
			ostr << "</VALUE.NAMEDINSTANCE>";
			checkStream(ostr);
		}
		std::ostream& ostr;
		String ns;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::enumerateInstances(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ClassName, false, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_LocalOnly, true, param::BOOLEAN, CIMValue(true)));
	params.push_back(param(CIMXMLParser::P_DeepInheritance, true, param::BOOLEAN, CIMValue(true)));
	params.push_back(param(CIMXMLParser::P_IncludeQualifiers, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_IncludeClassOrigin, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_PropertyList, true, param::STRINGARRAY, CIMValue(CIMNULL)));
	getParameterValues(parser, params);
	String className = params[0].val.toString();
	StringArray propertyList;
	StringArray* pPropList = 0;
	if (params[5].isSet)
	{
		propertyList = params[5].val.toStringArray();
		pPropList = &propertyList;
	}
	bool localOnly = params[1].val.toBool();
	bool deep = params[2].val.toBool();
	bool includeQualifiers = params[3].val.toBool();
	bool includeClassOrigin = params[4].val.toBool();
	ostr << "<IRETURNVALUE>";
	CIMInstanceXMLOutputter handler(ostr, ns);
	hdl.enumInstances(ns, className, handler,
		deep ? E_DEEP : E_SHALLOW,
		localOnly ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
		includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
		includeClassOrigin ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
		pPropList);
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::getClass(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ClassName, false, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_LocalOnly, true, param::BOOLEAN, CIMValue(true)));
	params.push_back(param(CIMXMLParser::P_IncludeQualifiers, true, param::BOOLEAN, CIMValue(true)));
	params.push_back(param(CIMXMLParser::P_IncludeClassOrigin, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_PropertyList, true, param::STRINGARRAY, CIMValue(CIMNULL)));
	getParameterValues(parser, params);
	String className = params[0].val.toString();
	StringArray propertyList;
	StringArray* pPropList = 0;
	if (params[4].isSet)
	{
		propertyList = params[4].val.toStringArray();
		pPropList = &propertyList;
	}
	bool localOnly = params[1].val.toBool();
	bool includeQualifiers = params[2].val.toBool();
	bool includeClassOrigin = params[3].val.toBool();
	ostr << "<IRETURNVALUE>";
	CIMClass cimClass = hdl.getClass(ns, className,
		localOnly ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
		includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
		includeClassOrigin ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
		pPropList);
	
	CIMtoXML(cimClass, ostr);
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::getInstance(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_InstanceName, false, param::INSTANCENAME));
	params.push_back(param(CIMXMLParser::P_LocalOnly, true, param::BOOLEAN, CIMValue(true)));
	params.push_back(param(CIMXMLParser::P_IncludeQualifiers, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_IncludeClassOrigin, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_PropertyList, true, param::STRINGARRAY, CIMValue(CIMNULL)));
	getParameterValues(parser, params);
	CIMObjectPath instancePath = params[0].val.toCIMObjectPath();
	StringArray propertyList;
	StringArray* pPropList = 0;
	if (params[4].isSet)
	{
		propertyList = params[4].val.toStringArray();
		pPropList = &propertyList;
	}
	bool localOnly = params[1].val.toBool();
	bool includeQualifiers = params[2].val.toBool();
	bool includeClassOrigin = params[3].val.toBool();
	ostr << "<IRETURNVALUE>";
	CIMInstance cimInstance = hdl.getInstance(ns, instancePath,
		localOnly ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
		includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
		includeClassOrigin ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
		pPropList);
	CIMInstancetoXML(cimInstance, ostr);
	ostr << "</IRETURNVALUE>";
}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::getProperty(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_InstanceName, false, param::INSTANCENAME));
	params.push_back(param(CIMXMLParser::P_PropertyName, false, param::STRING, CIMValue("")));
	getParameterValues(parser, params);
	CIMObjectPath instpath = params[0].val.toCIMObjectPath();
	ostr << "<IRETURNVALUE>";
	CIMValue cv = hdl.getProperty(ns, instpath, params[1].val.toString());
	if (cv)
	{
		CIMtoXML(cv, ostr);
	}
	ostr << "</IRETURNVALUE>";
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::getQualifier(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	String qualifierName = XMLQualifier::getQualifierName(parser);
	ostr << "<IRETURNVALUE>";
	CIMQualifierType qual = hdl.getQualifierType(ns, qualifierName);
	CIMtoXML(qual, ostr);
	ostr << "</IRETURNVALUE>";
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::referenceNames(ostream& ostr, CIMXMLParser& parser,
	const String& ns,CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ObjectName, false, param::OBJECTNAME));
	params.push_back(param(CIMXMLParser::P_ResultClass, true, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_Role, true, param::STRING, CIMValue("")));
	getParameterValues(parser, params);
	CIMObjectPath path = params[0].val.toCIMObjectPath();
	String resultClass;
	if (params[1].isSet)
	{
		resultClass = params[1].val.toString();
	}
	ostr << "<IRETURNVALUE>";
	CIMObjectPathXMLOutputter handler(ostr, getHost());
	hdl.referenceNames(ns, path, handler, resultClass, params[2].val.toString());
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::references(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_ObjectName, false, param::OBJECTNAME));
	params.push_back(param(CIMXMLParser::P_ResultClass, true, param::CLASSNAME));
	params.push_back(param(CIMXMLParser::P_Role, true, param::STRING, CIMValue("")));
	params.push_back(param(CIMXMLParser::P_IncludeQualifiers, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_IncludeClassOrigin, true, param::BOOLEAN, CIMValue(false)));
	params.push_back(param(CIMXMLParser::P_PropertyList, true, param::STRINGARRAY, CIMValue(CIMNULL)));
	getParameterValues(parser, params);
	CIMObjectPath path = params[0].val.toCIMObjectPath();
	String resultClass;
	if (params[1].isSet)
	{
		resultClass = params[1].val.toString();
	}
	StringArray propertyList;
	StringArray* pPropList = 0;
	if (params[5].isSet)
	{
		propertyList = params[5].val.toStringArray();
		pPropList = &propertyList;
	}
	
	bool includeQualifiers = params[3].val.toBool();
	bool includeClassOrigin = params[4].val.toBool();
	String role = params[2].val.toString();
	
	ostr << "<IRETURNVALUE>";
	if (path.isClassPath())
	{
		// It's a class
		AssocCIMClassXMLOutputter handler(ostr, ns);
		hdl.referencesClasses(ns, path, handler, resultClass,
			role,
			includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			includeClassOrigin ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			pPropList);
	}
	else
	{
		AssocCIMInstanceXMLOutputter handler(ostr, ns,
			getHost());
		hdl.references(ns, path, handler, resultClass,
			role,
			includeQualifiers ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			includeClassOrigin ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			pPropList);
	}
	ostr << "</IRETURNVALUE>";
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class execQueryXMLOutputter : public CIMInstanceResultHandlerIFC
	{
	public:
		execQueryXMLOutputter(std::ostream& ostr_, const String& ns_)
		: ostr(ostr_)
		, ns(ns_)
		{}
	protected:
		virtual void doHandle(const CIMInstance &i)
		{
			CIMObjectPath cop(ns, i);
			ostr << "<VALUE.OBJECTWITHPATH>";
			CIMInstancePathAndInstancetoXML(i, ostr, cop);
			ostr << "</VALUE.OBJECTWITHPATH>";
			checkStream(ostr);
		}
	private:
		std::ostream& ostr;
		const String& ns;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::execQuery(ostream& ostr, CIMXMLParser& parser,
	const String& ns, CIMOMHandleIFC& hdl)
{
	Array<param> params;
	params.push_back(param(CIMXMLParser::P_QueryLanguage, false, param::STRING));
	params.push_back(param(CIMXMLParser::P_Query, false, param::STRING));
	getParameterValues(parser, params);
	ostr << "<IRETURNVALUE>";
	execQueryXMLOutputter handler(ostr, ns);
	hdl.execQuery(ns, handler, params[1].val.toString(), params[0].val.toString());
	ostr << "</IRETURNVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::processSimpleReq(CIMXMLParser& parser, ostream& ostrEntity,
	ostream& ostrError, OperationContext& context)
{
	try
	{
		ostrEntity << "<SIMPLERSP>";
		
		// start out pointing to SIMPLEREQ
		OW_ASSERT(parser.tokenIsId(CIMXMLParser::E_SIMPLEREQ));
		// <!ELEMENT SIMPLEREQ (IMETHODCALL|METHODCALL)>
		parser.mustGetChild();
		if (parser.getToken() == CIMXMLParser::E_METHODCALL)
		{
			m_isIntrinsic = false;
		}
		else if (parser.getToken() == CIMXMLParser::E_IMETHODCALL)
		{
			m_isIntrinsic = true;
		}
		else
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				"No <METHODCALL> or <IMETHODCALL> element");
		}
		// <!ELEMENT IMETHODCALL (LOCALNAMESPACEPATH, IPARAMVALUE*)>
		// <!ATTLIST IMETHODCALL %CIMName;>
		// or
		// <!ELEMENT METHODCALL ((LOCALINSTANCEPATH|LOCALCLASSPATH),PARAMVALUE*)>
		// <!ATTLIST METHODCALL %CIMName;>
		m_functionName = parser.mustGetAttribute(CIMXMLParser::A_NAME);
		parser.mustGetChild();
		CIMOMHandleIFCRef hdl = this->getEnvironment()->getCIMOMHandle(context);
		if (m_isIntrinsic)
		{
			// <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
			parser.mustGetChildId(CIMXMLParser::E_NAMESPACE);
			String nameSpace = XMLClass::getNameSpace(parser);
			
			// move past LOCALNAMESPACEPATH to IPARAMVALUE*
			parser.mustGetEndTag();
			executeIntrinsic(ostrEntity, parser, *hdl, nameSpace);
		}
		else
		{
			executeExtrinsic(ostrEntity, parser, *hdl);
		}
		ostrEntity << "</SIMPLERSP>";
	}
	catch (CIMException& ce)
	{
		OW_LOGDEBUG(Format("XMLExecute::processSimpleReq caught CIM "
			"exception:\nCode: %1\nFile: %2\n Line: %3\nMessage: %4",
			ce.getErrNo(), ce.getFile(), ce.getLine(), ce.getMessage()));
		outputError(ce.getErrNo(), ce.getMessage(), ostrError);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::doLogDebug(const String& message)
{
	OW_LOGDEBUG(message);
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::doLogInfo(const String& message)
{
	OW_LOGINFO(message);
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::doLogError(const String& message)
{
	OW_LOGERROR(message);
}
//////////////////////////////////////////////////////////////////////////////
void
XMLExecute::doOptions(CIMFeatures& cf,
	OperationContext& context)
{
	cf = this->getEnvironment()->getCIMOMHandle(context, ServiceEnvironmentIFC::E_DONT_SEND_INDICATIONS)->getServerFeatures();
}
//////////////////////////////////////////////////////////////////////////////
RequestHandlerIFC*
XMLExecute::clone() const
{
	OW_ASSERT(!m_ostrEntity);
	OW_ASSERT(!m_ostrError);
	return new XMLExecute(*this);
}
//////////////////////////////////////////////////////////////////////////////
// Protected
void
XMLExecute::outputError(CIMException::ErrNoType errorCode,
	const String& msg, ostream& ostr)
{
	setError(errorCode, msg);
	ostr << "<SIMPLERSP>\r\n";
	if (m_isIntrinsic)
	{
		ostr << "<IMETHODRESPONSE NAME=\"" << m_functionName << "\">\r\n";
	}
	else
	{
		ostr << "<METHODRESPONSE NAME=\"" << m_functionName << "\">\r\n";
	}

	ostr << "<ERROR CODE=\"" << errorCode << "\" DESCRIPTION=\"" <<
		XMLEscape(msg) <<
		"\"></ERROR>\r\n";

	if (m_isIntrinsic)
	{
		ostr << "</IMETHODRESPONSE>\r\n";
	}
	else
	{
		ostr << "</METHODRESPONSE>\r\n";
	}
	ostr << "</SIMPLERSP>\r\n";
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
OW_REQUEST_HANDLER_FACTORY(OpenWBEM::XMLExecute,XMLExecute);

