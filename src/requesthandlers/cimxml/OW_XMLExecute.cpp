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

#include "OW_config.h"
#include "OW_XMLExecute.hpp"
#include "OW_Format.hpp"
#include "OW_XMLClass.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_XMLException.hpp"
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
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_CIMtoXML.hpp"

#include <algorithm>

using std::ostream;



OW_XMLExecute::FuncEntry OW_XMLExecute::g_funcs[24] =
{
	{ "associatornames", &OW_XMLExecute::associatorNames },
	{ "associators", &OW_XMLExecute::associators },
	{ "createclass", &OW_XMLExecute::createClass },
	{ "createinstance", &OW_XMLExecute::createInstance },
	{ "deleteclass", &OW_XMLExecute::deleteClass },
	{ "deleteinstance", &OW_XMLExecute::deleteInstance },
	{ "deletequalifier", &OW_XMLExecute::deleteQualifier },
	{ "enumerateclasses", &OW_XMLExecute::enumerateClasses },
	{ "enumerateclassnames", &OW_XMLExecute::enumerateClassNames },
	{ "enumerateinstancenames", &OW_XMLExecute::enumerateInstanceNames },
	{ "enumerateinstances", &OW_XMLExecute::enumerateInstances },
	{ "enumeratequalifiers", &OW_XMLExecute::enumerateQualifiers },
	{ "execquery", &OW_XMLExecute::execQuery },
	{ "getclass", &OW_XMLExecute::getClass },
	{ "getinstance", &OW_XMLExecute::getInstance },
	{ "getproperty", &OW_XMLExecute::getProperty },
	{ "getqualifier", &OW_XMLExecute::getQualifier },
	{ "modifyclass", &OW_XMLExecute::modifyClass },
	{ "modifyinstance", &OW_XMLExecute::modifyInstance },
	{ "referencenames", &OW_XMLExecute::referenceNames },
	{ "references", &OW_XMLExecute::references },
	{ "setproperty", &OW_XMLExecute::setProperty },
	{ "setqualifier", &OW_XMLExecute::setQualifier },
	{ "garbage", 0 }
};

OW_XMLExecute::FuncEntry* OW_XMLExecute::g_funcsEnd = &OW_XMLExecute::g_funcs[23];

//////////////////////////////////////////////////////////////////////////////
bool
OW_XMLExecute::funcEntryCompare(const OW_XMLExecute::FuncEntry& f1,
	const OW_XMLExecute::FuncEntry& f2)
{
	return (strcmp(f1.name, f2.name) < 0);
}

/*
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::init()
{
	g_funcMap.clear();
	g_funcMap["associatornames"] = &OW_XMLExecute::associatorNames;
	g_funcMap["associators"] = &OW_XMLExecute::associators;
	g_funcMap["createclass"] = &OW_XMLExecute::createClass;
	g_funcMap["createinstance"] = &OW_XMLExecute::createInstance;
	g_funcMap["deleteclass"] = &OW_XMLExecute::deleteClass;
	g_funcMap["deleteinstance"] = &OW_XMLExecute::deleteInstance;
	g_funcMap["deletequalifier"] = &OW_XMLExecute::deleteQualifier;
	g_funcMap["enumerateclasses"] = &OW_XMLExecute::enumerateClasses;
	g_funcMap["enumerateinstances"] = &OW_XMLExecute::enumerateInstances;
	g_funcMap["enumerateclassnames"] = &OW_XMLExecute::enumerateClassNames;
	g_funcMap["enumerateinstancenames"] = &OW_XMLExecute::enumerateInstanceNames;
	g_funcMap["enumeratequalifiers"] = &OW_XMLExecute::enumerateQualifiers;
	g_funcMap["getclass"] = &OW_XMLExecute::getClass;
	g_funcMap["getinstance"] = &OW_XMLExecute::getInstance;
	g_funcMap["getproperty"] = &OW_XMLExecute::getProperty;
	g_funcMap["getqualifier"] = &OW_XMLExecute::getQualifier;
	g_funcMap["modifyclass"] = &OW_XMLExecute::modifyClass;
	g_funcMap["modifyinstance"] = &OW_XMLExecute::modifyInstance;
	g_funcMap["referencenames"] = &OW_XMLExecute::referenceNames;
	g_funcMap["references"] = &OW_XMLExecute::references;
	g_funcMap["setqualifier"] = &OW_XMLExecute::setQualifier;
	g_funcMap["setproperty"] = &OW_XMLExecute::setProperty;
	g_funcMap["execquery"] = &OW_XMLExecute::execQuery;
}

class OW_XMLFuncLoader
{
public:
	OW_XMLFuncLoader()
	{
		OW_XMLExecute::init();
	}
};

OW_XMLFuncLoader g_funcLoader;
*/


//////////////////////////////////////////////////////////////////////////////
OW_XMLExecute::OW_XMLExecute()
	: OW_RequestHandlerIFCXML(),
	m_ostrEntity(NULL),
	m_ostrError(NULL),
	m_isIntrinsic(false),
	m_functionName()
{
}

//////////////////////////////////////////////////////////////////////////////
// Private
int
OW_XMLExecute::executeXML(OW_CIMXMLParser& parser, ostream* ostrEntity,
	ostream* ostrError, const OW_String& userName)
{
	m_hasError = false;
	m_ostrEntity = ostrEntity;
	m_ostrError = ostrError;
	m_isIntrinsic = false;
	OW_String messageId = parser.mustGetAttribute(OW_XMLOperationGeneric::MSG_ID);

	parser.getChild();

	if (!parser)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
	}

	makeXMLHeader(messageId, *m_ostrEntity);

	if (parser.getToken() == OW_CIMXMLParser::XML_ELEMENT_MULTIREQ)
	{
		(*m_ostrEntity) << "<MULTIRSP>";
		parser.getChild();

		while (parser.tokenIs(OW_CIMXMLParser::XML_ELEMENT_SIMPLEREQ))
		{
			OW_TempFileStream ostrEnt, ostrErr(500);
			processSimpleReq(parser, ostrEnt, ostrErr, userName);
			if (m_hasError)
			{
				(*m_ostrEntity) << ostrErr.rdbuf();
				m_hasError = false;
			}
			else
			{
				(*m_ostrEntity) << ostrEnt.rdbuf();
			}
			parser.getNext();
			parser.mustGetEndTag();
		} // while

		(*m_ostrEntity) << "</MULTIRSP>";
	} // if MULTIRSP
	else if (parser.getToken() == OW_CIMXMLParser::XML_ELEMENT_SIMPLEREQ)
	{
		makeXMLHeader(messageId, *m_ostrError);
		processSimpleReq(parser, *m_ostrEntity, *m_ostrError, userName);
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"No <SIMPLEREQ> or <MULTIREQ> tag");
	}

	(*m_ostrEntity) << "</MESSAGE></CIM>\r\n";


	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::executeIntrinsic(ostream& ostr,
	OW_CIMXMLParser parser, OW_CIMOMHandleIFC& hdl,
	OW_CIMObjectPath& path)
{

	OW_String functionNameLC = m_functionName;
	functionNameLC.toLowerCase();

	OW_LOGDEBUG(format("Got function name. calling function %1",
		functionNameLC));

	FuncEntry fe = { 0, 0 };
	fe.name = functionNameLC.c_str();
	FuncEntry* i = std::lower_bound(g_funcs, g_funcsEnd, fe, funcEntryCompare);

	if(i == g_funcsEnd)
	{
		// they sent over an intrinsic method call we don't know about
		OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
	}
	else
	{
		ostr << "<IMETHODRESPONSE NAME=\"" << m_functionName <<
			"\"><IRETURNVALUE>";

		// call the member function that was found
		(this->*((*i).func))(ostr, parser, path, hdl);
		ostr << "</IRETURNVALUE></IMETHODRESPONSE>";
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::executeExtrinsic(ostream& ostr, OW_CIMXMLParser parser,
	OW_CIMOMHandleIFC& lch)
{
	ostr << "<METHODRESPONSE NAME=\"" << m_functionName <<
		"\"><RETURNVALUE>";

	doInvokeMethod(ostr, parser, m_functionName, lch);

	ostr << "</RETURNVALUE></METHODRESPONSE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doInvokeMethod(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& methodName, OW_CIMOMHandleIFC& hdl)
{
	OW_CIMValueArray inParams;
	OW_CIMValueArray outParams;

	OW_CIMObjectPath instancePath = OW_XMLCIMFactory::createObjectPath(parser);
	OW_CIMClass cc = hdl.getClass(instancePath, false);

	if(!cc)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,"Class was null");
	}

	OW_CIMMethod method = cc.getMethod(methodName);

	if(!method)
	{
		OW_THROWCIMMSG(OW_CIMException::METHOD_NOT_FOUND,
			format("Looking for method %1", methodName).c_str() );
	}

	OW_CIMParameterArray parameters = method.getParameters();

	getParameters(parser, parameters, inParams);

	OW_CIMValue cv = hdl.invokeMethod(instancePath, methodName, inParams,
		outParams);

	if(cv)
	{
		OW_CIMtoXML(cv, ostr);
	}

	if(parameters.size() == 0)
	{
		return;	// Assume no paramters
	}

	size_t outParamsIdx = 0;
	for (size_t i=0; i < parameters.size(); i++)
	{
		OW_CIMParameter cp = parameters[i];
		if (cp.getQualifier("out"))
		{
			ostr << "<PARAMVALUE NAME=\"" << cp.getName() << "\">";
			OW_CIMtoXML(outParams[outParamsIdx++], ostr);
			ostr << "</PARAMVALUE";
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
//
// Reads paramters for extrinsinc method calls
//
// Paramlist is the list of parameters (in and out) for this method, params will
// be returned with input parameters in the same order as IN's were found in the
// paramlist.
//
// paramlist is a vector of CIMParameter
// params is a vector of CIMValues
//
void
OW_XMLExecute::getParameters(OW_CIMXMLParser& parser,
	const OW_Array<OW_CIMParameter>& paramlist, OW_Array<OW_CIMValue>& params)
{
	//
	// Process parameters
	//
	size_t paramIdx = 0;
	while (parser.tokenIs(OW_CIMXMLParser::XML_ELEMENT_PARAMVALUE))
	{
		OW_String parameterName = parser.mustGetAttribute(paramName);

		parser.getNext();
		int token = parser.getToken();
		if (token != OW_CIMXMLParser::XML_ELEMENT_VALUE
			&& token != OW_CIMXMLParser::XML_ELEMENT_VALUE_REFERENCE
			&& token != OW_CIMXMLParser::XML_ELEMENT_VALUE_ARRAY
			&& token != OW_CIMXMLParser::XML_ELEMENT_VALUE_REFARRAY
			)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
				"Parameter %1, has no value", paramName).c_str());
		}

		for (; paramIdx < paramlist.size(); paramIdx++)
		{
			if (paramlist[paramIdx].getName().equalsIgnoreCase(parameterName))
			{
				break;
			}
			if (paramlist[paramIdx].getQualifier("IN"))
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
					"Parameter %1 is out of order.  Next expected parameter "
					"is %2", parameterName, paramlist[paramIdx].getName()).c_str());
			}
			params.push_back(OW_CIMValue());	// insert an invalid cimValue
			paramIdx++;
		} // for

		if (!paramlist[paramIdx].getQualifier("IN"))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
				"Parameter %1 is not an \"IN\" parameter.", parameterName).c_str());
		}

		params.push_back(OW_XMLCIMFactory::createValue(parser,
			paramlist[paramIdx].getType().toString()));
		++paramIdx;

	} // for (; parser; parser = parser.getNext()
	for (; paramIdx < paramlist.size(); ++paramIdx)
	{
		if (paramlist[paramIdx].getQualifier("IN"))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
				"No in parameter for %1", paramlist[paramIdx].getName()).c_str());
		}
		else
		{
			params.push_back(OW_CIMValue());	// fill in remainder of inParams.
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMObjectPathXMLOutputter : public OW_CIMObjectPathResultHandlerIFC
	{
	public:
		CIMObjectPathXMLOutputter(ostream& ostr_)
		: ostr(ostr_)
		{}
	protected:
		virtual void doHandle(const OW_CIMObjectPath &cop)
		{
			OW_CIMtoXML(cop, ostr, OW_CIMtoXMLFlags::isNotInstanceName);
		}
	private:
		ostream& ostr;
	};
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct param
	{
		OW_String name;
		bool optional;
		OW_CIMDataType type;
		OW_CIMValue defaultVal;
		bool isSet;
		OW_CIMValue val;

		param(const OW_String& name_,
			bool optional_ = true,
			OW_CIMDataType type_ = OW_CIMDataType(OW_CIMDataType::STRING),
			OW_CIMValue defaultVal_ = OW_CIMValue())
			: name(name_)
			, optional(optional_)
			, type(type_)
			, defaultVal(defaultVal_)
			, isSet(false)
			, val()
		{}

		param(const OW_String& name_,
			bool optional_,
			OW_CIMDataType::Type type_,
			OW_CIMValue defaultVal_ = OW_CIMValue())
			: name(name_)
			, optional(optional_)
			, type(type_)
			, defaultVal(defaultVal_)
			, isSet(false)
			, val()
		{}
	};

	//////////////////////////////////////////////////////////////////////////////
	void getParameterValues(OW_CIMXMLParser& parser,
		OW_Array<param>& params)
	{
		(void)parser;
		(void)params;
	}
}


//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::associatorNames(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_OBJECTNAME, false, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_ASSOCCLASS, true, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_RESULTCLASS, true, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_ROLE, true, OW_CIMDataType::STRING, OW_CIMValue("")));
	params.push_back(param(XMLP_RESULTROLE, true, OW_CIMDataType::STRING, OW_CIMValue("")));

	getParameterValues(parser, params);

	OW_String ns = path.getNameSpace();

	path = params[0].val.toCIMObjectPath();
	path.setNameSpace(ns);

	OW_String assocClass;
	if (params[1].isSet)
	{
		assocClass = params[1].val.toCIMObjectPath().getObjectName();
	}

	OW_String resultClass;
	if (params[2].isSet)
	{
		resultClass = params[2].val.toCIMObjectPath().getObjectName();
	}

	CIMObjectPathXMLOutputter handler(ostr);
	hdl.associatorNames(path, handler, assocClass, resultClass,
		params[3].val.toString(), params[4].val.toString());
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class AssocCIMInstanceXMLOutputter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		AssocCIMInstanceXMLOutputter(
			std::ostream& ostr_,
			OW_CIMObjectPath& path_,
			bool includeQualifiers_, bool includeClassOrigin_, bool isPropertyList_,
			OW_StringArray& propertyList_)
		: ostr(ostr_)
		, path(path_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, isPropertyList(isPropertyList_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &ci)
		{
			ostr <<  "<VALUE.OBJECTWITHPATH>";

			OW_CIMObjectPath cop( ci.getClassName(), ci.getKeyValuePairs() );
			cop.setNameSpace( path.getNameSpace() );

			OW_CIMtoXML(ci, ostr, cop, OW_CIMtoXMLFlags::notLocalOnly,
				includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
				includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
				propertyList, (isPropertyList && propertyList.size() == 0));

			ostr << "</VALUE.OBJECTWITHPATH>\n";
		
		}
		std::ostream& ostr;
		OW_CIMObjectPath& path;
		bool includeQualifiers, includeClassOrigin, isPropertyList;
		OW_StringArray& propertyList;

	};
	class AssocCIMClassXMLOutputter : public OW_CIMClassResultHandlerIFC
	{
	public:
		AssocCIMClassXMLOutputter(
			std::ostream& ostr_,
			OW_CIMObjectPath& path_,
			bool includeQualifiers_, bool includeClassOrigin_, bool isPropertyList_,
			OW_StringArray& propertyList_,
			OW_String& ns_)
		: ostr(ostr_)
		, path(path_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, isPropertyList(isPropertyList_)
		, propertyList(propertyList_)
		, ns(ns_)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &cc)
		{
			ostr <<  "<VALUE.OBJECTWITHPATH>";

			OW_CIMObjectPath cop(cc.getName(), ns);
			OW_CIMClassPathtoXML(cop,ostr);
			OW_CIMtoXML(cc, ostr, OW_CIMtoXMLFlags::notLocalOnly,
				includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
				includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
				propertyList, (isPropertyList && propertyList.size() == 0));

			ostr << "</VALUE.OBJECTWITHPATH>\n";
		
		}
		std::ostream& ostr;
		OW_CIMObjectPath& path;
		bool includeQualifiers, includeClassOrigin, isPropertyList;
		OW_StringArray& propertyList;
		OW_String& ns;

	};
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::associators(ostream& ostr,
	OW_CIMXMLParser& parser, OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_OBJECTNAME, false, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_ASSOCCLASS, true, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_RESULTCLASS, true, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_ROLE, true, OW_CIMDataType::STRING, OW_CIMValue("")));
	params.push_back(param(XMLP_RESULTROLE, true, OW_CIMDataType::STRING, OW_CIMValue("")));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));
	OW_CIMDataType dt(OW_CIMDataType::STRING, 0);
	params.push_back(param(XMLP_PROPERTYLIST, true, dt, OW_CIMValue()));

	getParameterValues(parser, params);

	OW_String ns = path.getNameSpace();

	path = params[0].val.toCIMObjectPath();
	path.setNameSpace(ns);

	OW_String assocClass;
	if (params[1].isSet)
	{
		assocClass = params[1].val.toCIMObjectPath().getObjectName();
	}

	OW_String resultClass;
	if (params[2].isSet)
	{
		resultClass = params[2].val.toCIMObjectPath().getObjectName();
	}


	OW_StringArray propertyList;
	OW_StringArray* pPropList = 0;
	if (params[7].isSet)
	{
		propertyList = params[7].val.toStringArray();
		pPropList = &propertyList;
	}
	

	bool includeQualifiers = params[5].val.toBool();
	bool includeClassOrigin = params[6].val.toBool();
	bool isPropertyList = params[7].isSet;
	OW_String role = params[3].val.toString();
	OW_String resultRole = params[4].val.toString();

	if (path.getKeys().size() == 0)
	{
		// class path
		AssocCIMClassXMLOutputter handler(ostr, path, includeQualifiers,
			includeClassOrigin, isPropertyList, propertyList, ns);

		hdl.associatorsClasses(path, handler,
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, pPropList);
	}
	else
	{
		// instance path
		AssocCIMInstanceXMLOutputter handler(ostr, path, includeQualifiers,
			includeClassOrigin, isPropertyList, propertyList);

		hdl.associators(path, handler,
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, pPropList);
	}
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::createClass(ostream& /*ostr*/, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	parser.mustGetChild();
	hdl.createClass( path, OW_XMLCIMFactory::createClass(parser) );
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::createInstance(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	parser.mustGetChild();		// Point parser to <INSTANCE> tag

	OW_CIMInstance cimInstance = OW_XMLCIMFactory::createInstance(parser);
	OW_String className = cimInstance.getClassName();
	OW_CIMObjectPath realPath = OW_CIMObjectPath(className, path.getNameSpace());

	// Special treatment for __Namespace class
	if(className.equals(OW_CIMClass::NAMESPACECLASS))
	{
		OW_CIMProperty prop = cimInstance.getProperty(
			OW_CIMProperty::NAME_PROPERTY);

		// Need the name property since it contains the name of the new
		// name space
		if (!prop)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Name property not specified for new namespace");
		}

		// If the name property didn't come acrossed as a key, then
		// set the name property as the key
		if (!prop.isKey())
		{
			prop.addQualifier(OW_CIMQualifier::createKeyQualifier());
		}

		cimInstance.setProperty(prop);
	}

	OW_CIMPropertyArray keys = cimInstance.getKeyValuePairs();
	if(keys.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,"Instance doesn't have keys");
	}

	for (size_t i = 0; i < keys.size(); ++i)
	{
		OW_CIMProperty key = keys[i];
		if (!key.getValue())
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Key must be provided!  Property \"%1\" does not have a "
					"valid value.", key.getName()).c_str());
		}
	}

	realPath.setKeys(keys);

	OW_CIMObjectPath newPath = hdl.createInstance(realPath, cimInstance);
	// do we still need to do this below?
	newPath.setNameSpace(path.getNameSpace());
	OW_CIMtoXML(newPath, ostr, OW_CIMtoXMLFlags::isInstanceName);
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::deleteClass(ostream& /*ostr*/, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, false, OW_CIMDataType::REFERENCE));

	getParameterValues(parser, params);

	OW_String className = params[0].val.toString();

	path.setObjectName( className );
	hdl.deleteClass( path );
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::deleteInstance(ostream&	/*ostr*/, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String name = parser.getAttribute( paramName );
	if ( !name.equalsIgnoreCase( "InstanceName" ) )
		OW_THROWCIMMSG( OW_CIMException::INVALID_PARAMETER,
			OW_String( "Parameter name was " + name ).c_str() );

	parser.mustGetChild();

	OW_CIMObjectPath completePath = OW_XMLCIMFactory::createObjectPath(parser);
	completePath.setNameSpace(path.getNameSpace());
	hdl.deleteInstance( completePath );
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::deleteQualifier(ostream& /*ostr*/, OW_CIMXMLParser& qualparser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String qname = getQualifierName(qualparser);
	path.setObjectName(qname);
	hdl.deleteQualifierType(path);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class ClassNameXMLWriter : public OW_CIMObjectPathResultHandlerIFC
	{
	public:
		ClassNameXMLWriter(std::ostream& ostr_) : ostr(ostr_) {}
	protected:
		virtual void doHandle(const OW_CIMObjectPath &cop)
		{
			ostr << "<CLASSNAME NAME=\"" << cop.getObjectName() <<
				"\"/>";
		}
	private:
		std::ostream& ostr;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateClassNames(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, true, OW_CIMDataType::REFERENCE, OW_CIMValue("")));
	params.push_back(param(XMLP_DEEP, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));

	getParameterValues(parser, params);

	OW_String className = params[0].val.toString();
	OW_Bool deepInheritance = params[1].val.toBool();

	path.setObjectName(className);

	ClassNameXMLWriter handler(ostr);
	hdl.enumClassNames(path, handler, deepInheritance);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMClassXMLOutputter : public OW_CIMClassResultHandlerIFC
	{
	public:
		CIMClassXMLOutputter(ostream& ostr_, bool localOnly_,
			bool includeQualifiers_, bool includeClassOrigin_)
		: ostr(ostr_)
		, localOnly(localOnly_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &c)
		{
			OW_CIMtoXML(c, ostr,
				localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
				includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
				includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
				OW_StringArray());
		}
	private:
		ostream& ostr;
		bool localOnly, includeQualifiers, includeClassOrigin;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateClasses( ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, true, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_DEEP, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_LOCAL, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));

	getParameterValues(parser, params);

	if (params[0].isSet)
	{
		path.setObjectName(params[0].val.toCIMObjectPath().getObjectName());
	}

	CIMClassXMLOutputter handler(ostr, params[2].val.toBool(), params[3].val.toBool(),
		params[4].val.toBool());
	hdl.enumClass(path, handler, params[1].val.toBool(), false);

	// TODO: Switch to this.  It doesn't seem to work though (long make check fails.)
	//hdl.enumClass(path, deep, localOnly,
		//includeQualifiers, includeClassOrigin);

}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMInstanceNameXMLOutputter : public OW_CIMObjectPathResultHandlerIFC
	{
	public:
		CIMInstanceNameXMLOutputter(ostream& ostr_)
		: ostr(ostr_)
		{}
	protected:
		virtual void doHandle(const OW_CIMObjectPath &cop)
		{
			OW_CIMtoXML(cop, ostr, OW_CIMtoXMLFlags::isInstanceName);
		}
	private:
		ostream& ostr;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateInstanceNames(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, false, OW_CIMDataType::REFERENCE));

	getParameterValues(parser, params);
	path.setObjectName(params[0].val.toCIMObjectPath().getObjectName());
	
	CIMInstanceNameXMLOutputter handler(ostr);
	hdl.enumInstanceNames(path, handler);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMInstanceXMLOutputter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceXMLOutputter(
			std::ostream& ostr_,
			OW_CIMObjectPath& path_,
			bool localOnly_, bool includeQualifiers_, bool includeClassOrigin_, bool isPropertyList_,
			OW_StringArray& propertyList_)
		: ostr(ostr_)
		, path(path_)
		, localOnly(localOnly_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, isPropertyList(isPropertyList_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &i)
		{
			const OW_CIMInstance& cimInstance = i;
			OW_CIMObjectPath cop(cimInstance.getClassName(),
				cimInstance.getKeyValuePairs());

			cop.setNameSpace(path.getNameSpace());

			OW_CIMtoXML(cimInstance, ostr, cop,
				OW_CIMtoXMLFlags::notLocalOnly,
				includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
				includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
				propertyList,
				(isPropertyList && propertyList.size() == 0));
		}
		std::ostream& ostr;
		OW_CIMObjectPath& path;
		bool localOnly, includeQualifiers, includeClassOrigin, isPropertyList;
		OW_StringArray& propertyList;

	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateInstances(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, false, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_LOCAL, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_DEEP, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));
	OW_CIMDataType dt(OW_CIMDataType::STRING, 0);
	params.push_back(param(XMLP_PROPERTYLIST, true, dt, OW_CIMValue()));

	getParameterValues(parser, params);

	path.setObjectName(params[0].val.toCIMObjectPath().getObjectName());

	OW_StringArray propertyList;
	OW_StringArray* pPropList = 0;
	if (params[5].isSet)
	{
		propertyList = params[5].val.toStringArray();
		pPropList = &propertyList;
	}


	bool localOnly = params[1].val.toBool();
	bool deep = params[2].val.toBool();
	bool includeQualifiers = params[3].val.toBool();
	bool includeClassOrigin = params[4].val.toBool();

	// TODO: remove as many of these flags from handler as possible
	CIMInstanceXMLOutputter handler(ostr, path, localOnly, includeQualifiers,
		includeClassOrigin, params[5].isSet, propertyList);
	hdl.enumInstances(path, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin, pPropList);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMQualifierTypeXMLOutputter : public OW_CIMQualifierTypeResultHandlerIFC
	{
	public:
		CIMQualifierTypeXMLOutputter(
			std::ostream& ostr_)
		: ostr(ostr_)
		{}
	protected:
		virtual void doHandle(const OW_CIMQualifierType &i)
		{
			OW_CIMtoXML(i, ostr);
		}
		std::ostream& ostr;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateQualifiers(ostream& ostr, OW_CIMXMLParser& /*parser*/,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	CIMQualifierTypeXMLOutputter handler(ostr);
	hdl.enumQualifierTypes(path, handler);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getClass(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, false, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_LOCAL, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));
	OW_CIMDataType dt(OW_CIMDataType::STRING, 0);
	params.push_back(param(XMLP_PROPERTYLIST, true, dt, OW_CIMValue()));

	getParameterValues(parser, params);

	path.setObjectName(params[0].val.toCIMObjectPath().getObjectName());

	OW_StringArray propertyList;
	OW_StringArray* pPropList = 0;
	if (params[4].isSet)
	{
		propertyList = params[4].val.toStringArray();
		pPropList = &propertyList;
	}


	bool localOnly = params[1].val.toBool();
	bool includeQualifiers = params[2].val.toBool();
	bool includeClassOrigin = params[3].val.toBool();


	path.setObjectName(params[0].val.toCIMObjectPath().getObjectName());

	OW_CIMClass cimClass = hdl.getClass(path, localOnly, includeQualifiers,
		includeClassOrigin,
		pPropList);
	
	OW_CIMtoXML(cimClass, ostr,
		localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
		includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
		includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
		propertyList,
		(params[4].isSet && propertyList.size() == 0));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getInstance(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_INSTANCENAME, false, OW_CIMDataType::REFERENCE));
	params.push_back(param(XMLP_LOCAL, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, OW_CIMDataType::BOOLEAN, OW_CIMValue(false)));
	OW_CIMDataType dt(OW_CIMDataType::STRING, 0);
	params.push_back(param(XMLP_PROPERTYLIST, true, dt, OW_CIMValue()));

	getParameterValues(parser, params);

	path.setObjectName(params[0].val.toCIMObjectPath().getObjectName());

	OW_StringArray propertyList;
	OW_StringArray* pPropList = 0;
	if (params[4].isSet)
	{
		propertyList = params[4].val.toStringArray();
		pPropList = &propertyList;
	}


	bool localOnly = params[1].val.toBool();
	bool includeQualifiers = params[2].val.toBool();
	bool includeClassOrigin = params[3].val.toBool();

	OW_String ns = path.getNameSpace();

	OW_CIMInstance cimInstance = hdl.getInstance(path, localOnly,
		includeQualifiers, includeClassOrigin,
		isPropertyList ? &propertyList : 0);

	OW_CIMtoXML(cimInstance, ostr, OW_CIMObjectPath(),
		localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
		includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
		includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
		propertyList,
		(isPropertyList && propertyList.size() == 0));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getProperty(ostream& ostr, OW_CIMXMLParser& propparser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String propertyName=propparser.mustExtractParameterValue(XMLP_PROPERTYNAME);

	propparser=propparser.findElementAndParameter("InstanceName");
	if (!propparser)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Could not find a <IPARAMVALUE> with an InstanceName argument");
	}

	OW_String ns = path.getNameSpace();
	path = OW_XMLCIMFactory::createObjectPath(propparser);
	path.setNameSpace(ns);

	OW_CIMValue cv = hdl.getProperty(path,propertyName);

	if (cv)
		OW_CIMtoXML(cv, ostr);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getQualifier(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{

	path.setObjectName(getQualifierName(parser));

	OW_CIMQualifierType qual = hdl.getQualifierType(path);
	if (qual)
	{
		OW_CIMtoXML(qual, ostr);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::modifyClass(ostream&	/*ostr*/, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String name=parser.mustGetAttribute(paramName);
	if (!name.equalsIgnoreCase(XMLP_MODIFIED_CLASS))
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Parameter name was %1", name).c_str());

	parser=parser.getChild();
	if (!parser)
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"No element within <IPARAMVALUE>");

	//
	// Process <CLASS> element
	//
	OW_CIMClass cimClass = OW_XMLCIMFactory::createClass(parser);
	path.setObjectName(cimClass.getName());

	hdl.modifyClass(path,cimClass);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::modifyInstance(ostream&	/*ostr*/, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	//
	// Check parameter name is "ModifiedInstance"
	//
	OW_String name=parser.getAttribute(paramName);
	if (!name.equalsIgnoreCase("modifiedinstance"))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Parameter name was %1", name).c_str() );
	}

	parser=parser.mustChildElementChild(OW_CIMXMLParser::XML_ELEMENT_VALUE_NAMEDINSTANCE);
	//
	// Fetch <INSTANCENAME> element
	//
	OW_String ns = path.getNameSpace();
	path = OW_XMLCIMFactory::createObjectPath(parser);
	path.setNameSpace(ns);
	parser=parser.getNext();

	OW_CIMInstance cimInstance = OW_XMLCIMFactory::createInstance(parser);
	hdl.modifyInstance(path,cimInstance);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::referenceNames(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path,OW_CIMOMHandleIFC& hdl)
{
	OW_String className;

	OW_String role = parser.extractParameterValue(XMLP_ROLE, OW_String());
	OW_String ns = path.getNameSpace();
	//
	// Look for ObjectName
	//
	OW_CIMXMLParser tmpparser=parser;
	for (;tmpparser; tmpparser=parser.getNext())
	{
		if (tmpparser.getToken()!=OW_CIMXMLParser::XML_ELEMENT_IPARAMVALUE)
			continue;
		OW_String attr = tmpparser.getAttribute(paramName);
		if (!attr.equalsIgnoreCase(XMLP_OBJECTNAME))
			continue;
		tmpparser=tmpparser.getChild();
		if (!tmpparser)
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Needed a child for <IPARAMVALUE> when handling "
				"AssociatorNames");

		path = OW_XMLCIMFactory::createObjectPath(tmpparser);
		path.setNameSpace(ns);
		break;
	}
	OW_String resultClass = parser.extractParameterValueAttr(XMLP_RESULTCLASS,
		OW_CIMXMLParser::XML_ELEMENT_CLASSNAME, paramName);

	CIMObjectPathXMLOutputter handler(ostr);
	hdl.referenceNames(path, handler, resultClass, role);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::references(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String className;
	OW_Bool isPropertyList;
	OW_Array<OW_String> propertyList;
	OW_Bool includeQualifiers;
	OW_Bool includeClassOrigin;

	OW_String role = OW_String(parser.extractParameterValue(
		OW_String(XMLP_ROLE), OW_String()));

	propertyList = parser.extractParameterStringArray(XMLP_PROPERTYLIST,
		isPropertyList);

	includeQualifiers = parser.extractParameterValue(XMLP_QUAL, OW_Bool(false));
	includeClassOrigin = parser.extractParameterValue(XMLP_ORIGIN, OW_Bool(false));

	OW_String ns = path.getNameSpace();

	OW_CIMXMLParser tmpparser = parser;
	for (; tmpparser; tmpparser = tmpparser.getNext())
	{
		if (tmpparser.getToken() != OW_CIMXMLParser::XML_ELEMENT_IPARAMVALUE)
		{
			continue;
		}
		OW_String attr = tmpparser.getAttribute(paramName);
		if (!attr.equalsIgnoreCase(XMLP_OBJECTNAME))
		{
			continue;
		}
		tmpparser = tmpparser.getChild();
		if (!tmpparser)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Needed a child for <IPARAMVALUE> when handling associatorNames");
		}
		path = OW_XMLCIMFactory::createObjectPath(tmpparser);
		path.setNameSpace(ns);
		break;
	}

	OW_String resultClass = parser.extractParameterValueAttr(
		XMLP_RESULTCLASS, OW_CIMXMLParser::XML_ELEMENT_CLASSNAME, paramName);

	OW_StringArray* pPropList = (isPropertyList) ? &propertyList : NULL;
	
	if (path.getKeys().size() == 0)
	{
		// It's a class
		AssocCIMClassXMLOutputter handler(ostr, path, includeQualifiers,
			includeClassOrigin, isPropertyList, propertyList, ns);

		hdl.referencesClasses(path, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, pPropList);
	}
	else
	{
		AssocCIMInstanceXMLOutputter handler(ostr, path, includeQualifiers,
			includeClassOrigin, isPropertyList, propertyList);

		hdl.references(path, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, pPropList);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::setProperty(ostream&	/*ostr*/, OW_CIMXMLParser& propparser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String instanceName;
	OW_CIMInstance cimInstance;

	OW_String propertyName = propparser.extractParameterValue(XMLP_PROPERTYNAME,
		OW_String());
	if (propertyName.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Cannot find property name");
	}

	/* IPARAMVALUE cannot have a type, so assume the default is a string
	 * OW_String valueType = propparser.mustGetAttribute(OW_XMLAttribute::TYPE);
	 */
	OW_String valueType = "string";

	OW_CIMValue propValue;
	OW_CIMXMLParser valueparser = propparser.findElementAndParameter(XMLP_NewValue);
	if(valueparser)
	{
		propValue = OW_XMLCIMFactory::createValue(valueparser, valueType);
	}

	propparser = propparser.findElementAndParameter("InstanceName");
	if(!propparser)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Instance name not specified");
	}

	OW_String ns = path.getNameSpace();
	path = OW_XMLCIMFactory::createObjectPath(propparser);
	path.setNameSpace(ns);
	hdl.setProperty(path, propertyName, propValue);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class execQueryXMLOutputter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		execQueryXMLOutputter(std::ostream& ostr_, const OW_String& ns_)
		: ostr(ostr_)
		, ns(ns_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &i)
		{
			OW_CIMObjectPath cop(i.getClassName(),
				i.getKeyValuePairs() );

			cop.setNameSpace(ns);

			OW_CIMtoXML(i, ostr, cop,
				OW_CIMtoXMLFlags::notLocalOnly,
				OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,
				OW_StringArray());
		}
	private:
		std::ostream& ostr;
		const OW_String& ns;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::execQuery(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String queryLanguage =
		parser.extractParameterValue(XMLP_QUERYLANGUAGE, OW_String());
	OW_String query = parser.extractParameterValue(XMLP_QUERY, OW_String());

	if (queryLanguage.length() == 0 || query.length() == 0)
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	OW_String nameSpace = path.getNameSpace();
	execQueryXMLOutputter handler(ostr, nameSpace);
	hdl.execQuery(path.getFullNameSpace(), handler, query, queryLanguage);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::setQualifier(ostream& /*ostr*/, OW_CIMXMLParser& qualparser,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String argName = qualparser.mustGetAttribute(paramName);

	if (!argName.equalsIgnoreCase(XMLP_QUALIFIERDECL))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"invalid qualifier xml");
	}

	qualparser = qualparser.mustChildFindElement(
		OW_CIMXMLParser::XML_ELEMENT_QUALIFIER_DECLARATION);

	if (!qualparser)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Cannot find qualifier declaration argument");
	}

	OW_CIMQualifierType cimQualifier(OW_Bool(true));
	OW_XMLQualifier::processQualifierDecl(qualparser, cimQualifier);

	path.setObjectName(cimQualifier.getName());
	hdl.setQualifierType(path, cimQualifier);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::processSimpleReq(OW_CIMXMLParser& parser, ostream& ostrEntity,
	ostream& ostrError, const OW_String& userName)
{
	try
	{
		ostrEntity << "<SIMPLERSP>";
		OW_CIMXMLParser newparser = parser.getChild();
		if (!newparser)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"No <METHODCALL> or <IMETHODCALL> element");
		}

		if (newparser.getToken() == OW_CIMXMLParser::XML_ELEMENT_METHODCALL)
		{
			m_isIntrinsic = false;
		}
		else if (newparser.getToken() == OW_CIMXMLParser::XML_ELEMENT_IMETHODCALL)
		{
			m_isIntrinsic = true;
		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"No <METHODCALL> or <IMETHODCALL> element");
		}

		m_functionName = newparser.mustGetAttribute(OW_XMLAttribute::NAME);

		newparser = newparser.getChild();

		OW_CIMOMHandleIFCRef hdl = this->getEnvironment()->getCIMOMHandle(userName, true);

		if (m_isIntrinsic)
		{
			if (!newparser)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					"Missing <LOCALNAMESPACEPATH> in <IMETHODCALL>");
			}

			OW_String nameSpace = OW_XMLClass::getNameSpace(newparser.mustElementChild(
				OW_CIMXMLParser::XML_ELEMENT_LOCALNAMESPACEPATH));

			OW_CIMNameSpace ns(OW_Bool(true));
			ns.setNameSpace(nameSpace);
			OW_CIMObjectPath path = OW_CIMObjectPath(OW_String(), nameSpace);

			newparser = newparser.getNext();
			executeIntrinsic(ostrEntity, newparser, *hdl, path);
		}
		else
		{
			if (!newparser)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					"Missing <LOCALINSTANCEPATH> or <LOCALCLASSPATH> in <METHODCALL>");
			}

			executeExtrinsic(ostrEntity, newparser, *hdl);
		}
		ostrEntity << "</SIMPLERSP>";
	}
	catch (OW_CIMException& ce)
	{
		OW_LOGDEBUG(format("XMLExecute::processSimpleReq caught CIM "
			"exception:\nCode: %1\nFile: %2\n Line: %3\nMessage: %4",
			ce.getErrNo(), ce.getFile(), ce.getLine(), ce.getMessage()));

		m_hasError = true;
		outputError(ce.getErrNo(), ce.getMessage(), ostrError);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doLogDebug(const OW_String& message)
{
	OW_LOGDEBUG(message);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doLogCustInfo(const OW_String& message)
{
	OW_LOGCUSTINFO(message);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doLogError(const OW_String& message)
{
	OW_LOGERROR(message);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doOptions(OW_CIMFeatures& cf,
	const OW_SortedVectorMap<OW_String, OW_String>& /*handlerVars*/)
{
	cf = this->getEnvironment()->getCIMOMHandle(OW_String(), false)->getServerFeatures();
}

//////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFC*
OW_XMLExecute::clone() const
{
	OW_ASSERT(!m_ostrEntity);
	OW_ASSERT(!m_ostrError);
	return new OW_XMLExecute(*this);
}

//////////////////////////////////////////////////////////////////////////////
// Protected
void
OW_XMLExecute::outputError(OW_CIMException::ErrNoType errorCode,
	OW_String msg, ostream& ostr)
{
	ostr << "<SIMPLERSP>\r\n";
	if (m_isIntrinsic)
		ostr << "<IMETHODRESPONSE NAME=\"" << m_functionName << "\">\r\n";
	else
		ostr << "<METHODRESPONSE NAME=\"" << m_functionName << "\">\r\n";

	ostr << "<ERROR CODE=\"" << errorCode << "\" DESCRIPTION=\"" <<
		OW_XMLEscape(msg) <<
		"\"></ERROR>\r\n";

	if (m_isIntrinsic)
		ostr << "</IMETHODRESPONSE>\r\n";
	else
		ostr << "</METHODRESPONSE>\r\n";

	ostr << "</SIMPLERSP>\r\n";
}

//////////////////////////////////////////////////////////////////////////////
OW_REQUEST_HANDLER_FACTORY(OW_XMLExecute);


