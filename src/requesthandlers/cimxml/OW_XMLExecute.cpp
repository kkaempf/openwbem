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

DECLARE_EXCEPTION(BadStream)
DEFINE_EXCEPTION(BadStream)

#include <algorithm>

using std::ostream;


template<typename T> inline static void checkStream(T& str)
{
	if (!str.good())
	{
		OW_THROW(OW_BadStreamException, "The stream is bad");
	}
}

OW_XMLExecute::FuncEntry OW_XMLExecute::g_funcs[] =
{
	// This list must be kept in alphabetical order!
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	{ "associatornames", &OW_XMLExecute::associatorNames },
	{ "associators", &OW_XMLExecute::associators },
#endif
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	{ "createclass", &OW_XMLExecute::createClass },
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	{ "createinstance", &OW_XMLExecute::createInstance },
#endif
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	{ "deleteclass", &OW_XMLExecute::deleteClass },
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	{ "deleteinstance", &OW_XMLExecute::deleteInstance },
#endif
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	{ "deletequalifier", &OW_XMLExecute::deleteQualifier },
#endif
	{ "enumerateclasses", &OW_XMLExecute::enumerateClasses },
	{ "enumerateclassnames", &OW_XMLExecute::enumerateClassNames },
	{ "enumerateinstancenames", &OW_XMLExecute::enumerateInstanceNames },
	{ "enumerateinstances", &OW_XMLExecute::enumerateInstances },
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	{ "enumeratequalifiers", &OW_XMLExecute::enumerateQualifiers },
#endif
	{ "execquery", &OW_XMLExecute::execQuery },
	{ "getclass", &OW_XMLExecute::getClass },
	{ "getinstance", &OW_XMLExecute::getInstance },
	{ "getproperty", &OW_XMLExecute::getProperty },
	{ "getqualifier", &OW_XMLExecute::getQualifier },
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	{ "modifyclass", &OW_XMLExecute::modifyClass },
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	{ "modifyinstance", &OW_XMLExecute::modifyInstance },
#endif
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	{ "referencenames", &OW_XMLExecute::referenceNames },
	{ "references", &OW_XMLExecute::references },
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	{ "setproperty", &OW_XMLExecute::setProperty },
#endif
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	{ "setqualifier", &OW_XMLExecute::setQualifier },
#endif
	{ "garbage", 0 }
};

OW_XMLExecute::FuncEntry* OW_XMLExecute::g_funcsEnd = &OW_XMLExecute::g_funcs[0] + 
	(sizeof(OW_XMLExecute::g_funcs)/sizeof(*OW_XMLExecute::g_funcs)) - 1;

//////////////////////////////////////////////////////////////////////////////
bool
OW_XMLExecute::funcEntryCompare(const OW_XMLExecute::FuncEntry& f1,
	const OW_XMLExecute::FuncEntry& f2)
{
	return (strcmp(f1.name, f2.name) < 0);
}

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
OW_XMLExecute::~OW_XMLExecute() 
{
}

//////////////////////////////////////////////////////////////////////////////
// Private
int
OW_XMLExecute::executeXML(OW_CIMXMLParser& parser, ostream* ostrEntity,
	ostream* ostrError, const OW_String& userName)
{
	clearError();
	m_ostrEntity = ostrEntity;
	m_ostrError = ostrError;
	m_isIntrinsic = false;
	OW_String messageId = parser.mustGetAttribute(OW_CIMXMLParser::A_MSG_ID);

	parser.getChild();

	if (!parser)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
	}

	makeXMLHeader(messageId, *m_ostrEntity);

	if (parser.getToken() == OW_CIMXMLParser::E_MULTIREQ)
	{
		(*m_ostrEntity) << "<MULTIRSP>";
		parser.getChild();

		while (parser.tokenIs(OW_CIMXMLParser::E_SIMPLEREQ))
		{
			OW_TempFileStream ostrEnt, ostrErr(500);
			processSimpleReq(parser, ostrEnt, ostrErr, userName);
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
	else if (parser.getToken() == OW_CIMXMLParser::E_SIMPLEREQ)
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
	OW_CIMXMLParser& parser, OW_CIMOMHandleIFC& hdl,
	const OW_String& ns)
{

	OW_String functionNameLC = m_functionName;
	functionNameLC.toLowerCase();

	OW_LOGDEBUG(format("Got function name. calling function %1",
		functionNameLC));

	FuncEntry fe = { 0, 0 };
	fe.name = functionNameLC.c_str();
	FuncEntry* i = std::lower_bound(g_funcs, g_funcsEnd, fe, funcEntryCompare);

	if(i == g_funcsEnd || strcmp((*i).name, fe.name) != 0)
	{
		// they sent over an intrinsic method call we don't know about
		OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
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
OW_XMLExecute::executeExtrinsic(ostream& ostr, OW_CIMXMLParser& parser,
	OW_CIMOMHandleIFC& lch)
{
	ostr << "<METHODRESPONSE NAME=\"" << m_functionName <<
		"\">";

	doInvokeMethod(ostr, parser, m_functionName, lch);

	ostr << "</METHODRESPONSE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doInvokeMethod(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& methodName, OW_CIMOMHandleIFC& hdl)
{
	OW_CIMParamValueArray inParams;
	OW_CIMParamValueArray outParams;

	OW_CIMObjectPath path = OW_XMLCIMFactory::createObjectPath(parser);
	
	getParameters(parser, inParams);
	OW_CIMValue cv = hdl.invokeMethod(path.getNameSpace(), path, methodName,
		inParams, outParams);

	if(cv)
	{
		ostr << "<RETURNVALUE>";
		OW_CIMtoXML(cv, ostr);
		ostr << "</RETURNVALUE>";
	}

	for (size_t i=0; i < outParams.size(); i++)
	{
		OW_CIMParamValueToXML(outParams[i], ostr);
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
OW_XMLExecute::getParameters(OW_CIMXMLParser& parser,
	OW_CIMParamValueArray& params)
{
	//
	// Process parameters
	//
	while (parser.tokenIs(OW_CIMXMLParser::E_PARAMVALUE))
	{
		OW_String parameterName = parser.mustGetAttribute(OW_CIMXMLParser::A_NAME);
		OW_String parameterType = parser.getAttribute(OW_CIMXMLParser::A_PARAMTYPE);
		if (parameterType.empty())
		{
			parameterType = "string";
		}

		parser.getNextTag();
		int token = parser.getToken();
		if (token != OW_CIMXMLParser::E_VALUE
			&& token != OW_CIMXMLParser::E_VALUE_REFERENCE
			&& token != OW_CIMXMLParser::E_VALUE_ARRAY
			&& token != OW_CIMXMLParser::E_VALUE_REFARRAY
			)
		{
			params.push_back(OW_CIMParamValue(parameterName,
				OW_CIMValue(OW_CIMNULL)));
		}
		else
		{
			params.push_back(OW_CIMParamValue(parameterName,
				OW_XMLCIMFactory::createValue(parser, parameterType)));
		}

		parser.mustGetEndTag(); // pass </PARAMVALUE>
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
		virtual void doHandle(const OW_CIMObjectPath &cop_)
		{
			ostr << "<OBJECTPATH>";

			// Make sure all outgoing object paths have our host name, instead of 127.0.0.1
			OW_CIMObjectPath cop(cop_);
			if (cop.getFullNameSpace().isLocal())
			{
				try
				{
					// TODO: Cache this value?
					cop.setHost(OW_SocketUtils::getFullyQualifiedHostName());
				}
				catch (const OW_SocketException& e)
				{
				}
			}

			if (cop.isClassPath())
			{
				OW_CIMClassPathtoXML(cop, ostr);
			}
			else
			{
				OW_CIMInstancePathtoXML(cop, ostr);
			}
			ostr << "</OBJECTPATH>";
			checkStream(ostr);
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

		OW_String name;
		bool optional;
		Type type;
		OW_CIMValue defaultVal;
		bool isSet;
		OW_CIMValue val;

		param(const OW_String& name_,
			bool optional_ = true,
			Type type_ = STRING,
			OW_CIMValue defaultVal_ = OW_CIMValue(OW_CIMNULL))
			: name(name_)
			, optional(optional_)
			, type(type_)
			, defaultVal(defaultVal_)
			, isSet(false)
			, val(OW_CIMNULL)
		{}
	};

	struct name_comparer
	{
		name_comparer(const OW_String& s_)
		: s(s_)
		{}

		bool operator()(const param& p)
		{
			return p.name.equalsIgnoreCase(s);
		}

		OW_String s;
	};

	//////////////////////////////////////////////////////////////////////////////
	void getParameterValues(OW_CIMXMLParser& parser,
		OW_Array<param>& params)
	{
		// scan all the parameters and set them
		while (parser.tokenIs(OW_CIMXMLParser::E_IPARAMVALUE))
		{
			OW_String name = parser.mustGetAttribute(OW_CIMXMLParser::A_NAME);
			OW_Array<param>::iterator i = std::find_if(params.begin(), params.end(),
				name_comparer(name));
			if (i == params.end())
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("Parameter %1 is not a valid parameter", name).c_str());
			}
			
			parser.getNextTag();

			if (parser.tokenIs(OW_CIMXMLParser::E_IPARAMVALUE))
			{
				// pointing at </IPARAMVALUE>, thus
				// IPARAMVALUE was empty, so the value is NULL
				i->isSet = true;
				// should already be this: i->val = OW_CIMValue();
			}
			else
			{
				switch (i->type)
				{
					case param::CLASSNAME:
						if (!parser.tokenIs(OW_CIMXMLParser::E_CLASSNAME))
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Parameter %1 is the wrong type.  Expected <CLASSNAME> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = OW_CIMValue(OW_XMLCIMFactory::createObjectPath(parser).getObjectName());
						break;

					case param::BOOLEAN:
						if (!parser.tokenIs(OW_CIMXMLParser::E_VALUE))
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Parameter %1 is the wrong type.  Expected <VALUE> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = OW_XMLCIMFactory::createValue(parser, "boolean");
						break;

					case param::STRINGARRAY:
						if (!parser.tokenIs(OW_CIMXMLParser::E_VALUE_ARRAY))
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Parameter %1 is the wrong type.  Expected <VALUE.ARRAY> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = OW_XMLCIMFactory::createValue(parser, "string");
						break;

					case param::INSTANCENAME:
						if (!parser.tokenIs(OW_CIMXMLParser::E_INSTANCENAME))
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Parameter %1 is the wrong type.  Expected <INSTANCENAME> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = OW_CIMValue(OW_XMLCIMFactory::createObjectPath(parser));
						break;

					case param::NAMEDINSTANCE:
					{
						if (!parser.tokenIs(OW_CIMXMLParser::E_VALUE_NAMEDINSTANCE))
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Parameter %1 is the wrong type.  Expected <VALUE.NAMEDINSTANCE> tag. %2",
									i->name, parser).c_str());
						}
						i->isSet = true;
						parser.mustGetChild(OW_CIMXMLParser::E_INSTANCENAME);
						OW_CIMObjectPath ipath(OW_XMLCIMFactory::createObjectPath(parser));
						OW_CIMInstance inst(OW_XMLCIMFactory::createInstance(parser));
						parser.mustGetEndTag(); // pass </VALUE.NAMEDINSTANCE>
						inst.setKeys(ipath.getKeys());
						i->val = OW_CIMValue(inst);
						break;
					}

					case param::STRING:
						if (!parser.tokenIs(OW_CIMXMLParser::E_VALUE))
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Parameter %1 is the wrong type.  Expected <VALUE> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = OW_XMLCIMFactory::createValue(parser, "string");
						break;

					case param::OBJECTNAME:
						if (!parser.tokenIs(OW_CIMXMLParser::E_INSTANCENAME)
							&& !parser.tokenIs(OW_CIMXMLParser::E_CLASSNAME))
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Parameter %1 is the wrong type.  Expected <INSTANCENAME> or <CLASSNAME> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = OW_CIMValue(OW_XMLCIMFactory::createObjectPath(parser));
						break;

					case param::PROPERTYVALUE:
						if (!parser.tokenIs(OW_CIMXMLParser::E_VALUE)
							&& !parser.tokenIs(OW_CIMXMLParser::E_VALUE_ARRAY)
							&& !parser.tokenIs(OW_CIMXMLParser::E_VALUE_REFERENCE))
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Parameter %1 is the wrong type.  Expected <VALUE> or <VALUE.ARRAY> or <VALUE.REFERENCE> tag.",
									i->name).c_str());
						}
						i->isSet = true;
						i->val = OW_XMLCIMFactory::createValue(parser, "string");
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
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("Non-optional parameter %1 was not given",
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
						params[i].val = OW_CIMValue(false);
						break;

					case param::STRING:
					case param::CLASSNAME:
						params[i].val = OW_CIMValue("");
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
OW_XMLExecute::associatorNames(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_OBJECTNAME, false, param::OBJECTNAME));
	params.push_back(param(XMLP_ASSOCCLASS, true, param::CLASSNAME));
	params.push_back(param(XMLP_RESULTCLASS, true, param::CLASSNAME));
	params.push_back(param(XMLP_ROLE, true, param::STRING, OW_CIMValue("")));
	params.push_back(param(XMLP_RESULTROLE, true, param::STRING, OW_CIMValue("")));

	getParameterValues(parser, params);

	OW_CIMObjectPath objectName = params[0].val.toCIMObjectPath();

	OW_String assocClass;
	if (params[1].isSet)
	{
		assocClass = params[1].val.toString();
	}

	OW_String resultClass;
	if (params[2].isSet)
	{
		resultClass = params[2].val.toString();
	}

	ostr << "<IRETURNVALUE>";
	CIMObjectPathXMLOutputter handler(ostr);
	hdl.associatorNames(ns, objectName, handler, assocClass, resultClass,
		params[3].val.toString(), params[4].val.toString());
	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class AssocCIMInstanceXMLOutputter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		AssocCIMInstanceXMLOutputter(
			std::ostream& ostr_,
			const OW_String& ns_,
			bool includeQualifiers_, bool includeClassOrigin_, bool isPropertyList_,
			OW_StringArray& propertyList_)
		: ostr(ostr_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, isPropertyList(isPropertyList_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &ci)
		{
			ostr <<  "<VALUE.OBJECTWITHPATH>";

			OW_CIMObjectPath cop( ci );
			cop.setNameSpace( ns );
			// Make sure all outgoing object paths have our host name, instead of 127.0.0.1
			try
			{
				// TODO: Cache this value?
				cop.setHost(OW_SocketUtils::getFullyQualifiedHostName());
			}
			catch (const OW_SocketException& e)
			{
			}

			OW_CIMtoXML(ci, ostr, cop,
				OW_CIMtoXMLFlags::isNotInstanceName,
				OW_CIMtoXMLFlags::notLocalOnly,
				includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
				includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
				propertyList, (isPropertyList && propertyList.size() == 0));

			ostr << "</VALUE.OBJECTWITHPATH>\n";
			checkStream(ostr);
		
		}
		std::ostream& ostr;
		OW_String ns;
		bool includeQualifiers, includeClassOrigin, isPropertyList;
		OW_StringArray& propertyList;

	};
	class AssocCIMClassXMLOutputter : public OW_CIMClassResultHandlerIFC
	{
	public:
		AssocCIMClassXMLOutputter(
			std::ostream& ostr_,
			bool includeQualifiers_, bool includeClassOrigin_, bool isPropertyList_,
			OW_StringArray& propertyList_,
			const OW_String& ns_)
		: ostr(ostr_)
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
			checkStream(ostr);
		
		}
		std::ostream& ostr;
		bool includeQualifiers, includeClassOrigin, isPropertyList;
		OW_StringArray& propertyList;
		const OW_String& ns;

	};
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::associators(ostream& ostr,
	OW_CIMXMLParser& parser, const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_OBJECTNAME, false, param::OBJECTNAME));
	params.push_back(param(XMLP_ASSOCCLASS, true, param::CLASSNAME));
	params.push_back(param(XMLP_RESULTCLASS, true, param::CLASSNAME));
	params.push_back(param(XMLP_ROLE, true, param::STRING, OW_CIMValue("")));
	params.push_back(param(XMLP_RESULTROLE, true, param::STRING, OW_CIMValue("")));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_PROPERTYLIST, true, param::STRINGARRAY, OW_CIMValue(OW_CIMNULL)));

	getParameterValues(parser, params);

	OW_CIMObjectPath objectName = params[0].val.toCIMObjectPath();

	OW_String assocClass;
	if (params[1].isSet)
	{
		assocClass = params[1].val.toString();
	}

	OW_String resultClass;
	if (params[2].isSet)
	{
		resultClass = params[2].val.toString();
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

	ostr << "<IRETURNVALUE>";
	if (objectName.isClassPath())
	{
		// class path
		AssocCIMClassXMLOutputter handler(ostr, includeQualifiers,
			includeClassOrigin, isPropertyList, propertyList, ns);

		hdl.associatorsClasses(ns, objectName, handler,
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, pPropList);
	}
	else
	{
		// instance path
		AssocCIMInstanceXMLOutputter handler(ostr, ns, includeQualifiers,
			includeClassOrigin, isPropertyList, propertyList);

		hdl.associators(ns, objectName, handler,
			assocClass, resultClass, role, resultRole, includeQualifiers,
			includeClassOrigin, pPropList);
	}
	ostr << "</IRETURNVALUE>";
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::createClass(ostream& /*ostr*/, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	parser.mustGetChild();
	hdl.createClass( ns, OW_XMLCIMFactory::createClass(parser) );
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::modifyClass(ostream&	/*ostr*/, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_String name = parser.mustGetAttribute(paramName);
	if (!name.equalsIgnoreCase(XMLP_MODIFIED_CLASS))
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Parameter name was %1", name).c_str());

	parser.mustGetChild();

	//
	// Process <CLASS> element
	//
	OW_CIMClass cimClass = OW_XMLCIMFactory::createClass(parser);

	hdl.modifyClass(ns, cimClass);
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::deleteClass(ostream& /*ostr*/, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, false, param::CLASSNAME));

	getParameterValues(parser, params);

	OW_String className = params[0].val.toString();

	hdl.deleteClass(ns, className);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::createInstance(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	parser.mustGetChild();		// Point parser to <INSTANCE> tag

	OW_CIMInstance cimInstance = OW_XMLCIMFactory::createInstance(parser);
	OW_String className = cimInstance.getClassName();
	//OW_CIMObjectPath realPath = OW_CIMObjectPath(className, path.getNameSpace());

	// Special treatment for __Namespace class
	if(className.equalsIgnoreCase(OW_CIMClass::NAMESPACECLASS))
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

		// If the name property didn't come across as a key, then
		// set the name property as the key
		if (!prop.isKey())
		{
			prop.addQualifier(OW_CIMQualifier::createKeyQualifier());
		}

		cimInstance.setProperty(prop);
	}


	ostr << "<IRETURNVALUE>";
	OW_CIMObjectPath newPath = hdl.createInstance(ns, cimInstance);
	OW_CIMInstanceNametoXML(newPath, ostr);
	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::modifyInstance(ostream&	/*ostr*/, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_MODIFIED_INSTANCE, false, param::NAMEDINSTANCE));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, param::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_PROPERTYLIST, true, param::STRINGARRAY, OW_CIMValue(OW_CIMNULL)));
	
	getParameterValues(parser, params);
	
	OW_StringArray propertyList;
	OW_StringArray* pPropList = 0;
	if (params[2].isSet)
	{
		propertyList = params[2].val.toStringArray();
		pPropList = &propertyList;
	}
	
	bool includeQualifiers = params[1].val.toBool();

	OW_CIMInstance modifiedInstance(OW_CIMNULL);
	params[0].val.get(modifiedInstance);

	hdl.modifyInstance(ns, modifiedInstance, includeQualifiers, pPropList);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::deleteInstance(ostream&	/*ostr*/, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_String name = parser.getAttribute( paramName );
	if ( !name.equalsIgnoreCase( "InstanceName" ) )
		OW_THROWCIMMSG( OW_CIMException::INVALID_PARAMETER,
			OW_String( "Parameter name was " + name ).c_str() );

	parser.mustGetChild();

	OW_CIMObjectPath instPath = OW_XMLCIMFactory::createObjectPath(parser);
	hdl.deleteInstance( ns, instPath );
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::setProperty(ostream&	/*ostr*/, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_INSTANCENAME, false, param::INSTANCENAME));
	params.push_back(param(XMLP_PROPERTYNAME, false, param::STRING, OW_CIMValue("")));
	params.push_back(param(XMLP_NEWVALUE, true, param::PROPERTYVALUE));

	getParameterValues(parser, params);

	OW_CIMObjectPath instpath = params[0].val.toCIMObjectPath();

	hdl.setProperty(ns, instpath, params[1].val.toString(), params[2].val);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::setQualifier(ostream& /*ostr*/, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_String argName = parser.mustGetAttribute(paramName);

	if (!argName.equalsIgnoreCase(XMLP_QUALIFIERDECL))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"invalid qualifier xml");
	}

	parser.mustGetChild(
		OW_CIMXMLParser::E_QUALIFIER_DECLARATION);
	
	OW_CIMQualifierType cimQualifier;
	OW_XMLQualifier::processQualifierDecl(parser, cimQualifier);

	hdl.setQualifierType(ns, cimQualifier);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::deleteQualifier(ostream& /*ostr*/, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_String qualName = getQualifierName(parser);
	hdl.deleteQualifierType(ns, qualName);
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
			checkStream(ostr);
		}
		std::ostream& ostr;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateQualifiers(ostream& ostr, OW_CIMXMLParser& /*parser*/,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
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
	class ClassNameXMLWriter : public OW_StringResultHandlerIFC
	{
	public:
		ClassNameXMLWriter(std::ostream& ostr_) : ostr(ostr_) {}
	protected:
		virtual void doHandle(const OW_String &name)
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
OW_XMLExecute::enumerateClassNames(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, true, param::CLASSNAME, OW_CIMValue("")));
	params.push_back(param(XMLP_DEEP, true, param::BOOLEAN, OW_CIMValue(false)));

	getParameterValues(parser, params);

	OW_String className = params[0].val.toString();
	OW_Bool deepInheritance = params[1].val.toBool();

	ostr << "<IRETURNVALUE>";
	ClassNameXMLWriter handler(ostr);
	hdl.enumClassNames(ns, className, handler, deepInheritance);
	ostr << "</IRETURNVALUE>";
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
			checkStream(ostr);
		}
	private:
		ostream& ostr;
		bool localOnly, includeQualifiers, includeClassOrigin;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateClasses( ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, true, param::CLASSNAME));
	params.push_back(param(XMLP_DEEP, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_LOCAL, true, param::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, param::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, param::BOOLEAN, OW_CIMValue(false)));

	getParameterValues(parser, params);

	OW_String className;
	if (params[0].isSet)
	{
		className = params[0].val.toString();
	}

	ostr << "<IRETURNVALUE>";
	CIMClassXMLOutputter handler(ostr, params[2].val.toBool(), params[3].val.toBool(),
		params[4].val.toBool());
	hdl.enumClass(ns, className, handler, params[1].val.toBool(), false);

	// TODO: Switch to this.  It doesn't seem to work though (long make check fails.)
	//hdl.enumClass(path, deep, localOnly,
		//includeQualifiers, includeClassOrigin);

	ostr << "</IRETURNVALUE>";
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
			OW_CIMInstanceNametoXML(cop, ostr);
			checkStream(ostr);
		}
	private:
		ostream& ostr;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateInstanceNames(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, false, param::CLASSNAME));

	getParameterValues(parser, params);
	OW_String className = params[0].val.toString();
	
	ostr << "<IRETURNVALUE>";
	CIMInstanceNameXMLOutputter handler(ostr);
	hdl.enumInstanceNames(ns, className, handler);
	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMInstanceXMLOutputter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceXMLOutputter(
			std::ostream& ostr_,
			const OW_String& ns_,
			bool localOnly_, bool includeQualifiers_, bool includeClassOrigin_, bool isPropertyList_,
			OW_StringArray& propertyList_)
		: ostr(ostr_)
		, ns(ns_)
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
			OW_CIMObjectPath cop(cimInstance);

			cop.setNameSpace(ns);

			ostr << "<VALUE.NAMEDINSTANCE>";
			OW_CIMtoXML(cimInstance, ostr, cop,
				OW_CIMtoXMLFlags::isInstanceName,
				OW_CIMtoXMLFlags::notLocalOnly,
				includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
				includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
				propertyList,
				(isPropertyList && propertyList.size() == 0));
			ostr << "</VALUE.NAMEDINSTANCE>";
			checkStream(ostr);
		}
		std::ostream& ostr;
		OW_String ns;
		bool localOnly, includeQualifiers, includeClassOrigin, isPropertyList;
		OW_StringArray& propertyList;

	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateInstances(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, false, param::CLASSNAME));
	params.push_back(param(XMLP_LOCAL, true, param::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_DEEP, true, param::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_PROPERTYLIST, true, param::STRINGARRAY, OW_CIMValue(OW_CIMNULL)));

	getParameterValues(parser, params);

	OW_String className = params[0].val.toString();

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

	ostr << "<IRETURNVALUE>";
	// TODO: remove as many of these flags from handler as possible
	CIMInstanceXMLOutputter handler(ostr, ns, localOnly, includeQualifiers,
		includeClassOrigin, params[5].isSet, propertyList);
	hdl.enumInstances(ns, className, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin, pPropList);
	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getClass(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_CLASSNAME, false, param::CLASSNAME));
	params.push_back(param(XMLP_LOCAL, true, param::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, param::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_PROPERTYLIST, true, param::STRINGARRAY, OW_CIMValue(OW_CIMNULL)));

	getParameterValues(parser, params);

	OW_String className = params[0].val.toString();

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


	ostr << "<IRETURNVALUE>";
	OW_CIMClass cimClass = hdl.getClass(ns, className, localOnly, includeQualifiers,
		includeClassOrigin,
		pPropList);
	
	OW_CIMtoXML(cimClass, ostr,
		localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
		includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
		includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
		propertyList,
		(params[4].isSet && propertyList.size() == 0));
	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getInstance(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_INSTANCENAME, false, param::INSTANCENAME));
	params.push_back(param(XMLP_LOCAL, true, param::BOOLEAN, OW_CIMValue(true)));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_PROPERTYLIST, true, param::STRINGARRAY, OW_CIMValue(OW_CIMNULL)));

	getParameterValues(parser, params);

	OW_CIMObjectPath instancePath = params[0].val.toCIMObjectPath();

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

	ostr << "<IRETURNVALUE>";
	OW_CIMInstance cimInstance = hdl.getInstance(ns, instancePath, localOnly,
		includeQualifiers, includeClassOrigin,
		pPropList);

	OW_CIMtoXML(cimInstance, ostr, OW_CIMObjectPath(OW_CIMNULL),
		OW_CIMtoXMLFlags::isNotInstanceName,
		localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
		includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
		includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
		propertyList,
		(pPropList && propertyList.size() == 0));
	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getProperty(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_INSTANCENAME, false, param::INSTANCENAME));
	params.push_back(param(XMLP_PROPERTYNAME, false, param::STRING, OW_CIMValue("")));

	getParameterValues(parser, params);

	OW_CIMObjectPath instpath = params[0].val.toCIMObjectPath();

	ostr << "<IRETURNVALUE>";
	OW_CIMValue cv = hdl.getProperty(ns, instpath, params[1].val.toString());

	if (cv)
		OW_CIMtoXML(cv, ostr);

	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getQualifier(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_String qualifierName = getQualifierName(parser);

	ostr << "<IRETURNVALUE>";
	OW_CIMQualifierType qual = hdl.getQualifierType(ns, qualifierName);
	OW_CIMtoXML(qual, ostr);
	ostr << "</IRETURNVALUE>";
}


#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::referenceNames(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns,OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_OBJECTNAME, false, param::OBJECTNAME));
	params.push_back(param(XMLP_RESULTCLASS, true, param::CLASSNAME));
	params.push_back(param(XMLP_ROLE, true, param::STRING, OW_CIMValue("")));

	getParameterValues(parser, params);

	OW_CIMObjectPath path = params[0].val.toCIMObjectPath();

	OW_String resultClass;
	if (params[1].isSet)
	{
		resultClass = params[1].val.toString();
	}

	ostr << "<IRETURNVALUE>";
	CIMObjectPathXMLOutputter handler(ostr);
	hdl.referenceNames(ns, path, handler, resultClass, params[2].val.toString());
	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::references(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_OBJECTNAME, false, param::OBJECTNAME));
	params.push_back(param(XMLP_RESULTCLASS, true, param::CLASSNAME));
	params.push_back(param(XMLP_ROLE, true, param::STRING, OW_CIMValue("")));
	params.push_back(param(XMLP_INCLUDEQUALIFIERS, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_INCLUDECLASSORIGIN, true, param::BOOLEAN, OW_CIMValue(false)));
	params.push_back(param(XMLP_PROPERTYLIST, true, param::STRINGARRAY, OW_CIMValue(OW_CIMNULL)));

	getParameterValues(parser, params);

	OW_CIMObjectPath path = params[0].val.toCIMObjectPath();

	OW_String resultClass;
	if (params[1].isSet)
	{
		resultClass = params[1].val.toString();
	}

	OW_StringArray propertyList;
	OW_StringArray* pPropList = 0;
	if (params[5].isSet)
	{
		propertyList = params[5].val.toStringArray();
		pPropList = &propertyList;
	}
	

	bool includeQualifiers = params[3].val.toBool();
	bool includeClassOrigin = params[4].val.toBool();
	bool isPropertyList = params[5].isSet;
	OW_String role = params[2].val.toString();
	
	ostr << "<IRETURNVALUE>";
	if (path.isClassPath())
	{
		// It's a class
		AssocCIMClassXMLOutputter handler(ostr, includeQualifiers,
			includeClassOrigin, isPropertyList, propertyList, ns);

		hdl.referencesClasses(ns, path, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, pPropList);
	}
	else
	{
		AssocCIMInstanceXMLOutputter handler(ostr, ns, includeQualifiers,
			includeClassOrigin, isPropertyList, propertyList);

		hdl.references(ns, path, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, pPropList);
	}
	ostr << "</IRETURNVALUE>";
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

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
			OW_CIMObjectPath cop(i);

			cop.setNameSpace(ns);

			ostr << "<VALUE.OBJECTWITHPATH>";
			OW_CIMtoXML(i, ostr, cop,
				OW_CIMtoXMLFlags::isNotInstanceName,
				OW_CIMtoXMLFlags::notLocalOnly,
				OW_CIMtoXMLFlags::includeQualifiers,
				OW_CIMtoXMLFlags::includeClassOrigin,
				OW_StringArray());
			ostr << "</VALUE.OBJECTWITHPATH>";
			checkStream(ostr);
		}
	private:
		std::ostream& ostr;
		const OW_String& ns;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::execQuery(ostream& ostr, OW_CIMXMLParser& parser,
	const OW_String& ns, OW_CIMOMHandleIFC& hdl)
{
	OW_Array<param> params;
	params.push_back(param(XMLP_QUERYLANGUAGE, false, param::STRING));
	params.push_back(param(XMLP_QUERY, false, param::STRING));

	getParameterValues(parser, params);

	ostr << "<IRETURNVALUE>";
	execQueryXMLOutputter handler(ostr, ns);
	hdl.execQuery(ns, handler, params[1].val.toString(), params[0].val.toString());
	ostr << "</IRETURNVALUE>";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::processSimpleReq(OW_CIMXMLParser& parser, ostream& ostrEntity,
	ostream& ostrError, const OW_String& userName)
{
	try
	{
		ostrEntity << "<SIMPLERSP>";
		
		// start out pointing to SIMPLEREQ
		OW_ASSERT(parser.tokenIs(OW_CIMXMLParser::E_SIMPLEREQ));

		// <!ELEMENT SIMPLEREQ (IMETHODCALL|METHODCALL)>
		parser.mustGetChild();

		if (parser.getToken() == OW_CIMXMLParser::E_METHODCALL)
		{
			m_isIntrinsic = false;
		}
		else if (parser.getToken() == OW_CIMXMLParser::E_IMETHODCALL)
		{
			m_isIntrinsic = true;
		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"No <METHODCALL> or <IMETHODCALL> element");
		}

		// <!ELEMENT IMETHODCALL (LOCALNAMESPACEPATH, IPARAMVALUE*)>
		// <!ATTLIST IMETHODCALL %CIMName;>
		// or
		// <!ELEMENT METHODCALL ((LOCALINSTANCEPATH|LOCALCLASSPATH),PARAMVALUE*)>
		// <!ATTLIST METHODCALL %CIMName;>

		m_functionName = parser.mustGetAttribute(OW_CIMXMLParser::A_NAME);

		parser.mustGetChild();

		OW_CIMOMHandleIFCRef hdl = this->getEnvironment()->getCIMOMHandle(userName, true);

		if (m_isIntrinsic)
		{
			// <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
			parser.mustGetChild(OW_CIMXMLParser::E_NAMESPACE);
			OW_String nameSpace = OW_XMLClass::getNameSpace(parser);
			
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
	catch (OW_CIMException& ce)
	{
		OW_LOGDEBUG(format("XMLExecute::processSimpleReq caught CIM "
			"exception:\nCode: %1\nFile: %2\n Line: %3\nMessage: %4",
			ce.getErrNo(), ce.getFile(), ce.getLine(), ce.getMessage()));

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
	setError(errorCode, msg);

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


