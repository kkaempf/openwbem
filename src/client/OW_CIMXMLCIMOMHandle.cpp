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
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_XMLParameters.hpp"
#include "OW_Format.hpp"
#include "OW_XMLEscape.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_HTTPDeflateIStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMParamValue.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif


using std::ostream;
using std::iostream;
using std::istream;



//////////////////////////////////////////////////////////////////////////////
OW_CIMXMLCIMOMHandle::OW_CIMXMLCIMOMHandle(OW_CIMProtocolIFCRef prot)
: OW_ClientCIMOMHandle(), m_protocol(prot)
{
	m_iMessageID = 0;
	m_protocol->setContentType("application/xml");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::sendXMLHeader( const OW_String &sMethod,
												 const OW_CIMObjectPath& path,
												 ostream& ostr, bool intrinsic)
{
	if (++m_iMessageID > 65535)
	{
		m_iMessageID = 1;
	}
	ostr << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
	ostr << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">";
	ostr << "<MESSAGE ID=\"" << m_iMessageID << "\" PROTOCOLVERSION=\"1.0\">";
	ostr << "<SIMPLEREQ>";
	if (intrinsic)
	{
		ostr << "<IMETHODCALL NAME=\"" << sMethod << "\">";
		OW_CIMtoXML(path.getFullNameSpace(), ostr, OW_CIMtoXMLFlags::doLocal );
	}
	else
	{
		ostr << "<METHODCALL NAME=\"" << sMethod << "\">";
		if (path.getKeys().size() > 0) // it's an instance
		{
			ostr << "<LOCALINSTANCEPATH>";
			OW_CIMtoXML(path.getFullNameSpace(), ostr, OW_CIMtoXMLFlags::doLocal);
			OW_CIMtoXML(path, ostr, OW_CIMtoXMLFlags::isInstanceName);
			ostr << "</LOCALINSTANCEPATH>";
		}
		else // it's a class
		{
			ostr << "<LOCALCLASSPATH>";
			OW_CIMtoXML(path.getFullNameSpace(), ostr, OW_CIMtoXMLFlags::doLocal);
			ostr << "<CLASSNAME NAME=\"" << path.getObjectName() << "\"/>";
			ostr << "</LOCALCLASSPATH>";
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::sendXMLTrailer( ostream& ostr, bool intrinsic)
{
	if (intrinsic)
	{
		ostr << "</IMETHODCALL>";
	}
	else
	{
		ostr << "</METHODCALL>";
	}
	ostr << "</SIMPLEREQ>";
	ostr << "</MESSAGE>";
	ostr << "</CIM>";
	ostr << "\r\n";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::doSendRequest(
	OW_Reference<std::iostream> ostrRef,
	const OW_String& methodName,
	const OW_CIMObjectPath& path,
	bool isIntrinsic,
	OW_ClientOperation& op)
{
	OW_Reference<OW_CIMProtocolIStreamIFC> istr = m_protocol->endRequest(
		ostrRef, methodName, path.getNameSpace());
	// Debug stuff
	/*
	OW_TempFileStream buf;
	buf << istr.rdbuf();
	ofstream ofstr("/tmp/rchXMLDump", ios::app);
	ofstr << "******* New dump ********" << endl;
	ofstr << buf.rdbuf() << endl;
	buf.rewind();
	OW_XMLParser parser(&buf);
	*/
	// end debug stuff
	OW_CIMXMLParser parser(*istr);

	try
	{
		checkNodeForCIMError(parser, methodName, isIntrinsic);
		if (isIntrinsic)
		{
			parser.mustGetNext(); // pass over <IRETURNVALUE>
		}
		op(parser);
		if (isIntrinsic)
		{
			parser.mustGetEndTag(); // pass </IRETURNVALUE>
		}
		parser.mustGetEndTag(); // pass </(I)METHODRESPONSE>
		parser.mustGetEndTag(); // pass </SIMPLERSP>
		parser.mustGetEndTag(); // pass </MESSAGE>
		parser.mustGetEndTag(); // pass </CIM>
		OW_HTTPUtils::eatEntity(*istr);
	}
	catch (OW_XMLParseException& xmlE)
	{
		OW_HTTPUtils::eatEntity(*istr);
		if (istr->getError().length() == 0)
		{
			throw xmlE;
		}
	}
	if (istr->getError().length() > 0)
	{
		// The trailer is escaped, so first unescape it
		OW_TempFileStream error(500);
		istr->getError(error);
		OW_CIMXMLParser errorParser(error);
		return checkNodeForCIMError(errorParser, methodName, isIntrinsic);
	}
}

void
OW_CIMXMLCIMOMHandle::checkNodeForCIMError(OW_CIMXMLParser& parser,
	const OW_String& operation, bool isIntrinsic)
{
	//
	// Check for <CIM> element
	//
	if (!parser || !parser.tokenIs(OW_CIMXMLParser::E_CIM))
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Invalid XML");
	}
	OW_String cimattr = parser.mustGetAttribute(OW_CIMXMLParser::A_CIMVERSION);
	if (!cimattr.equals(OW_CIMXMLParser::AV_CIMVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for CIMVERSION " + cimattr).c_str());
	}

	cimattr = parser.mustGetAttribute(OW_CIMXMLParser::A_DTDVERSION);
	if (!cimattr.equals(OW_CIMXMLParser::AV_DTDVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for DTDVERSION " + cimattr).c_str());
	}

	//
	// Find <MESSAGE>
	//
	parser.mustGetChild(OW_CIMXMLParser::E_MESSAGE);
	cimattr = parser.mustGetAttribute(OW_CIMXMLParser::A_MSG_ID);
	if (!cimattr.equals(OW_String(m_iMessageID)))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return messageid="+cimattr+", expected="
										 +OW_String(m_iMessageID)).c_str());
	}

	cimattr = parser.mustGetAttribute(OW_CIMXMLParser::A_PROTOCOLVERSION);
	if (!cimattr.equals(OW_CIMXMLParser::AV_PROTOCOLVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for PROTOCOLVERSION "+cimattr).c_str());
	}

	//
	// Find <SIMPLERSP>
	//
	parser.mustGetChild(OW_CIMXMLParser::E_SIMPLERSP);

	//
	// TODO-NICE: need to look for complex RSPs!!
	//

	//
	// <METHODRESPONSE> or <IMETHODRESPONSE>
	//
	parser.mustGetNext(isIntrinsic ?
		OW_CIMXMLParser::E_IMETHODRESPONSE :
		OW_CIMXMLParser::E_METHODRESPONSE);

	OW_String nameOfMethod = parser.mustGetAttribute("NAME");

	if (!nameOfMethod.equalsIgnoreCase(operation))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Called "+operation+" but response was for "+
										 nameOfMethod).c_str());
	}

	parser.getChild();
	if (!parser)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"No E_IRETURNVALUE");
	}

	//
	// See if there was an error, and if there was throw an equivalent
	// exception on the client
	//
	if (parser.tokenIs(OW_CIMXMLParser::E_ERROR))
	{
		OW_String errCode = parser.mustGetAttribute(
			OW_XMLParameters::paramErrorCode);
		OW_String description = parser.mustGetAttribute(
			OW_XMLParameters::paramErrorDescription);
		OW_Int32 iErrCode;
		try
		{
			iErrCode = errCode.toInt32();
		}
		catch (const OW_StringConversionException& e)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, format("Invalid xml.  %1",
				e.getMessage()).c_str());
		}
		OW_THROWCIMMSG(OW_CIMException::ErrNoType(errCode.toInt32()), description.c_str());
	}

	// For extrinsic methods, we leave the parser sitting on either
	// RETURNVALUE, PARAMVALUE or /METHODRESPONSE
	// For intrinsic methods, it's sitting on either IPARAMVALUE or
	// /IMETHODRESPONSE
}


//////////////////////////////////////////////////////////////////////////////
namespace
{
	class voidRetValOp : public OW_ClientOperation
	{
	public:
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			(void)parser;
			return;
		}
	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::deleteClass(const OW_CIMObjectPath& path)
{

	static const char* const commandName = "DeleteClass";

	OW_String nameSpace=path.getNameSpace();
	OW_String className=path.getObjectName();
	OW_Array<OW_Param> params;

	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME,OW_Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, "No ClassName passed"
							" to deleteClass()");
	}

	voidRetValOp op;
	intrinsicMethod(path, commandName, op, params);
}


//////////////////////////////////////////////////////////////////////////////
static OW_String
instanceNameToKey(const OW_CIMObjectPath& path,
	const OW_String& parameterName)
{
	OW_StringBuffer text = "<IPARAMVALUE NAME=\"" + parameterName + "\">";
	
	OW_StringStream ss;
	OW_CIMtoXML(path, ss, OW_CIMtoXMLFlags::isInstanceName);
	text += ss.toString();

    text += "</IPARAMVALUE>";
	return text.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::deleteInstance(const OW_CIMObjectPath& path)
{
	static const char* const commandName = "DeleteInstance";
	OW_Array<OW_Param> params;

	voidRetValOp op;
	intrinsicMethod(path, commandName, op, params,
						 instanceNameToKey(path, "InstanceName"));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::deleteQualifierType(const OW_CIMObjectPath& path)
{
	static const char* const commandName = "DeleteQualifier";
	OW_Array<OW_Param> params;

	OW_String qualName = path.getObjectName();

	params.push_back(OW_Param(XMLP_QUALIFIERNAME, qualName));

	voidRetValOp op;
	intrinsicMethod(path, commandName, op, params);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class enumClassNamesOp : public OW_ClientOperation
	{
	public:
		enumClassNamesOp(OW_CIMObjectPathResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			while (parser.tokenIs(OW_CIMXMLParser::E_CLASSNAME))
			{
				result.handle(OW_XMLCIMFactory::createObjectPath(parser));
			}
		}

		OW_CIMObjectPathResultHandlerIFC& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::enumClassNames(const OW_CIMObjectPath& path,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep)
{
	static const char* const commandName = "EnumerateClassNames";

	OW_String className = path.getObjectName();
	OW_Array<OW_Param> params;
	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME,OW_Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	params.push_back(OW_Param(XMLP_DEEP,deep));

	enumClassNamesOp op(result);
	intrinsicMethod(path, commandName, op, params);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class enumClassOp : public OW_ClientOperation
	{
	public:
		enumClassOp(OW_CIMClassResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			while (parser.tokenIs(OW_CIMXMLParser::E_CLASS))
			{
				result.handle(OW_XMLCIMFactory::createClass(parser));
			}
		}

		OW_CIMClassResultHandlerIFC& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::enumClass(const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	static const char* const commandName = "EnumerateClasses";

	OW_Array<OW_Param> params;
	OW_String className = path.getObjectName();

	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME,OW_Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	params.push_back(OW_Param(XMLP_LOCAL,localOnly));
	params.push_back(OW_Param(XMLP_DEEP,deep));
	params.push_back(OW_Param(XMLP_INCLUDEQUALIFIERS, includeQualifiers));
	params.push_back(OW_Param(XMLP_INCLUDECLASSORIGIN, includeClassOrigin));

	enumClassOp op(result);
	intrinsicMethod(path, commandName, op, params);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class enumInstanceNamesOp : public OW_ClientOperation
	{
	public:
		enumInstanceNamesOp(OW_CIMObjectPathResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			while (parser.tokenIs(OW_CIMXMLParser::E_INSTANCENAME))
			{
				result.handle(OW_XMLCIMFactory::createObjectPath(parser));
			}
		}

		OW_CIMObjectPathResultHandlerIFC& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::enumInstanceNames(const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result)
{
	static const char* const commandName = "EnumerateInstanceNames";

	OW_String className = path.getObjectName();
	OW_Array<OW_Param> params;

	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME, OW_Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, "Class was empty in "
							"EnumerateInstanceNames");
	}

	enumInstanceNamesOp op(result);
	intrinsicMethod(path, commandName, op, params);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class enumInstancesOp : public OW_ClientOperation
	{
	public:
		enumInstancesOp(OW_CIMInstanceResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			while (parser.tokenIs(OW_CIMXMLParser::E_VALUE_NAMEDINSTANCE))
			{
				parser.mustGetChild(OW_CIMXMLParser::E_INSTANCENAME);
				OW_CIMObjectPath iop(OW_XMLCIMFactory::createObjectPath(parser));

				OW_CIMInstance ci = OW_XMLCIMFactory::createInstance(parser);
				ci.setKeys(iop.getKeys());
				result.handle(ci);
				parser.mustGetEndTag(); // pass </VALUE.NAMEDINSTANCE>
			}
		}

		OW_CIMInstanceResultHandlerIFC& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::enumInstances(const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	static const char* const commandName = "EnumerateInstances";
	OW_StringStream extra(1000);
	OW_String className = path.getObjectName();
	OW_Array<OW_Param> params;

	if(className.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, "Class was empty in "
			"EnumerateInstances");
	}

	params.push_back(OW_Param(XMLP_CLASSNAME, OW_Param::VALUESET,
		"<CLASSNAME NAME=\""+className+"\"/>"));

	params.push_back(OW_Param(XMLP_DEEP, deep));
	params.push_back(OW_Param(XMLP_LOCAL, localOnly));
	params.push_back(OW_Param(XMLP_INCLUDEQUALIFIERS, includeQualifiers));
	params.push_back(OW_Param(XMLP_INCLUDECLASSORIGIN, includeClassOrigin));

	if(propertyList)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";
	}

	enumInstancesOp op(result);
	intrinsicMethod(path, commandName, op, params, extra.toString());
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class enumQualifierTypesOp : public OW_ClientOperation
	{
	public:
		enumQualifierTypesOp(OW_CIMQualifierTypeResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			while (parser.tokenIs(OW_CIMXMLParser::E_QUALIFIER_DECLARATION))
			{
				OW_CIMQualifierType cqt(OW_Bool(true));
				OW_XMLQualifier::processQualifierDecl(parser, cqt);
				result.handle(cqt);
			}
		}

		OW_CIMQualifierTypeResultHandlerIFC& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::enumQualifierTypes(const OW_CIMObjectPath& path,
		OW_CIMQualifierTypeResultHandlerIFC& result)
{
	static const char* const commandName = "EnumerateQualifiers";
	OW_String qualName = path.getObjectName();

	enumQualifierTypesOp op(result);
	intrinsicMethod(path, commandName, op);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class getClassOp : public OW_ClientOperation
	{
	public:
		getClassOp(OW_CIMClass& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			result = OW_XMLCIMFactory::createClass(parser);
		}

		OW_CIMClass& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMXMLCIMOMHandle::getClass(const OW_CIMObjectPath& path, OW_Bool localOnly,
    OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	static const char* const commandName = "GetClass";
	OW_Array<OW_Param> params;
	OW_String className = path.getObjectName();

	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME,OW_Param::VALUESET,
			"<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, "no class given for "
			"GetClass()");
	}

	params.push_back(OW_Param(XMLP_LOCAL, localOnly));
	params.push_back(OW_Param(XMLP_INCLUDEQUALIFIERS, includeQualifiers));
	params.push_back(OW_Param(XMLP_INCLUDECLASSORIGIN, includeClassOrigin));

	OW_String extraStr;
	if(propertyList)
	{
		OW_StringStream extra(1000);
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";

		extraStr = extra.toString();
	}

	OW_CIMClass rval;
	getClassOp op(rval);
	intrinsicMethod(path, commandName, op, params, extraStr);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class getInstanceOp : public OW_ClientOperation
	{
	public:
		getInstanceOp(OW_CIMInstance& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			result = OW_XMLCIMFactory::createInstance(parser);
		}

		OW_CIMInstance& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMXMLCIMOMHandle::getInstance(const OW_CIMObjectPath& path,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	static const char* const commandName = "GetInstance";
	OW_StringStream extra(1000);
	OW_Array<OW_Param> params;

	params.push_back(OW_Param(XMLP_LOCAL, localOnly));
	params.push_back(OW_Param(XMLP_INCLUDEQUALIFIERS, includeQualifiers));
	params.push_back(OW_Param(XMLP_INCLUDECLASSORIGIN, includeClassOrigin));

	extra << instanceNameToKey(path, "InstanceName");

	if (propertyList)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";
	}

	OW_CIMInstance rval;
	getInstanceOp op(rval);
	intrinsicMethod(path, commandName, op, params, extra.toString());
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class invokeMethodOp : public OW_ClientOperation
	{
	public:
		invokeMethodOp(OW_CIMValue& result_, OW_CIMParamValueArray& outParams_)
			: result(result_)
			, outParams(outParams_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			// For extrinsic methods, the parser is sitting on either
			// RETURNVALUE, PARAMVALUE or /METHODRESPONSE

			// handle RETURNVALUE, which is optional
			if (parser.tokenIs(OW_CIMXMLParser::E_RETURNVALUE))
			{
				OW_String returnType = parser.getAttribute(OW_CIMXMLParser::A_PARAMTYPE);
				if (returnType.length() == 0)
				{
					returnType = "string";
				}
				parser.mustGetChild();
				if (!parser.tokenIs(OW_CIMXMLParser::E_VALUE) &&
					!parser.tokenIs(OW_CIMXMLParser::E_VALUE_REFERENCE))
				{
					OW_THROWCIMMSG(OW_CIMException::FAILED,
						"<RETURNVALUE> did not contain a <VALUE> or "
						"<VALUE.REFERENCE> element");
				}
				result = OW_XMLCIMFactory::createValue(parser, returnType);
				parser.mustGetEndTag(); // pass /RETURNVALUE
			}

			// handle PARAMVALUE*
			for (size_t outParamCount = 0;
				  parser && parser.tokenIs(OW_CIMXMLParser::E_PARAMVALUE);
				  ++outParamCount)
			{
				OW_String name = parser.mustGetAttribute(OW_CIMXMLParser::A_NAME);
				OW_String type = parser.getAttribute(OW_CIMXMLParser::A_PARAMTYPE);
				if (type.length() == 0)
				{
					type = "string";
				}
				parser.getNext();
				
				if (outParams.size() <= outParamCount)
				{
					// make sure there's enough space in the vector
					outParams.resize(outParamCount + 1);
				}
				int token = parser.getToken();
				if (token != OW_CIMXMLParser::E_VALUE
					&& token != OW_CIMXMLParser::E_VALUE_REFERENCE
					&& token != OW_CIMXMLParser::E_VALUE_ARRAY
					&& token != OW_CIMXMLParser::E_VALUE_REFARRAY
					)
				{
					outParams[outParamCount] = OW_CIMParamValue(name,
						OW_CIMValue());
				}
				else
				{
					outParams[outParamCount] = OW_CIMParamValue(name,
						OW_XMLCIMFactory::createValue(parser, type));
				}
				parser.mustGetEndTag(); // pass /PARAMVALUE
			}

		}

		OW_CIMValue& result;
		OW_CIMParamValueArray& outParams;
	};
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMXMLCIMOMHandle::invokeMethod(const OW_CIMObjectPath& name,
											  const OW_String& methodName,
											  const OW_CIMParamValueArray& inParams,
											  OW_CIMParamValueArray& outParams)
{
	OW_Reference<std::iostream> iostrRef =
		m_protocol->beginRequest(methodName, name.getNameSpace());
	std::iostream& tfs = *iostrRef;

	sendXMLHeader(methodName, name, tfs, false);

	for (size_t i = 0; i < inParams.size(); ++i)
	{
		tfs << "<PARAMVALUE NAME=\"" << inParams[i].getName() << "\"";
		OW_CIMValue v = inParams[i].getValue();
		if (v)
		{
			OW_String type = v.getCIMDataType().toString();
			if (type == "REF")
			{
				type = "reference";
			}
			tfs << " PARAMTYPE=\"" << type << "\">";
			OW_CIMtoXML(inParams[i].getValue(), tfs);
		}
		else
		{
			tfs << '>';
		}
		tfs << "</PARAMVALUE>";
	}


	sendXMLTrailer(tfs, false);

	OW_CIMValue rval;
	invokeMethodOp op(rval, outParams);
	doSendRequest(iostrRef, methodName, name, false, op);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class getQualifierTypeOp : public OW_ClientOperation
	{
	public:
		getQualifierTypeOp(OW_CIMQualifierType& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			OW_XMLQualifier::processQualifierDecl(parser, result);
		}

		OW_CIMQualifierType& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMXMLCIMOMHandle::getQualifierType(const OW_CIMObjectPath& path)
{
	static const char* const commandName = "GetQualifier";

	OW_String qualName = path.getObjectName();
	OW_Array<OW_Param> params;

	params.push_back(OW_Param(XMLP_QUALIFIERNAME, qualName));

	OW_CIMQualifierType rval(OW_Bool(true));
	getQualifierTypeOp op(rval);
	intrinsicMethod(path, commandName, op, params);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::setQualifierType(const OW_CIMObjectPath& path,
													const OW_CIMQualifierType& qt)
{
	static const char* const commandName = "SetQualifier";
	OW_StringStream extra;
	extra << "<IPARAMVALUE NAME=\"QualifierDeclaration\">";
	OW_CIMtoXML(qt, extra);
	extra << "</IPARAMVALUE>";

	voidRetValOp op;
	intrinsicMethod(path, commandName, op, OW_Array<OW_Param>(),
						 extra.toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::modifyClass(const OW_CIMObjectPath& path,
										 const OW_CIMClass& cc)
{
	static const char* const commandName = "ModifyClass";
	OW_String className = path.getObjectName();

	OW_StringStream extra(1024);
	extra << "<IPARAMVALUE NAME=\"" << XMLP_MODIFIED_CLASS << "\">";
	OW_CIMtoXML(cc, extra ,OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers, OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray(), false);
	extra << "</IPARAMVALUE>";
	voidRetValOp op;
	intrinsicMethod(path, commandName, op, OW_Array<OW_Param>(),
		extra.toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::createClass(const OW_CIMObjectPath& path,
											 const OW_CIMClass& cc)
{
	static const char* const commandName = "CreateClass";

	OW_StringStream ostr;
	ostr << "<IPARAMVALUE NAME=\"NewClass\">";
	OW_CIMtoXML(cc, ostr, OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers , OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray(), false);
	ostr << "</IPARAMVALUE>";

	voidRetValOp op;
	intrinsicMethod(path, commandName, op, OW_Array<OW_Param>(),
		ostr.toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::modifyInstance(const OW_CIMObjectPath& path,
											 const OW_CIMInstance& ci)
{
	static const char* const commandName = "ModifyInstance";
	OW_String nameSpace = path.getNameSpace();
	OW_String className = path.getObjectName();

	OW_StringStream ostr(1000);
	ostr << "<IPARAMVALUE NAME=\"ModifiedInstance\">";
	ostr << "<VALUE.NAMEDINSTANCE>";
	OW_CIMtoXML(path, ostr, OW_CIMtoXMLFlags::isInstanceName);
	OW_CIMtoXML(ci, ostr, OW_CIMObjectPath(),
		OW_CIMtoXMLFlags::isInstanceName,
		OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers,
		OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray());
	ostr << "</VALUE.NAMEDINSTANCE></IPARAMVALUE>";
	voidRetValOp op;
	intrinsicMethod(path, commandName, op, OW_Array<OW_Param>(),
		ostr.toString());
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class createInstanceOp : public OW_ClientOperation
	{
	public:
		createInstanceOp(OW_CIMObjectPath& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			if (!parser.tokenIs(OW_CIMXMLParser::E_INSTANCENAME))
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Expected but did not get <INSTANCENAME>");
			}

			result = OW_XMLCIMFactory::createObjectPath(parser);
		}

		OW_CIMObjectPath& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_CIMXMLCIMOMHandle::createInstance(const OW_CIMObjectPath& path,
	const OW_CIMInstance& ci)
{
	static const char* const commandName = "CreateInstance";
	OW_String pathName = path.getObjectName();

	//
	// Check that the path has specified at least one key (can't have
	// an instance without such keys)
	//
	OW_CIMPropertyArray validKeys = path.getKeys();
	if (validKeys.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format("The "
						   "path (%1) for the instance does not specify any keys",
							 pathName).c_str());
	}

	OW_StringStream ostr;
	ostr << "<IPARAMVALUE NAME=\"NewInstance\">";
	OW_CIMtoXML(ci, ostr, OW_CIMObjectPath(),
		OW_CIMtoXMLFlags::isInstanceName,
		OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers,
		OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray());

	ostr << "</IPARAMVALUE>";

	OW_CIMObjectPath rval;
	createInstanceOp op(rval);
	intrinsicMethod(path, commandName, op, OW_Array<OW_Param>(),
												 ostr.toString());
	rval.setNameSpace(path.getNameSpace());
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class getPropertyOp : public OW_ClientOperation
	{
	public:
		getPropertyOp(OW_CIMValue& result_)
			: result(result_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			if (!parser.tokenIs(OW_CIMXMLParser::E_IRETURNVALUE))
			{
				// "string" because we don't know the type--defect in the spec.
				result = OW_XMLCIMFactory::createValue(parser, "string");
			}
			// else it was a NULL value
		}

		OW_CIMValue& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMXMLCIMOMHandle::getProperty(const OW_CIMObjectPath& path,
											 const OW_String& propName)
{
	static const char* const commandName = "GetProperty";
	OW_Array<OW_Param> params;

	params.push_back(OW_Param(XMLP_PROPERTYNAME, propName));

	OW_CIMValue rval;
	getPropertyOp op(rval);
	intrinsicMethod(path, commandName, op, params,
		instanceNameToKey(path,"InstanceName"));
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::setProperty(const OW_CIMObjectPath& path,
											 const OW_String& propName,
											 const OW_CIMValue& cv)
{
	static const char* const commandName = "SetProperty";
	OW_Array<OW_Param> params;
	params.push_back(OW_Param(XMLP_PROPERTYNAME, propName));
	OW_StringStream ostr;
	OW_CIMtoXML(cv, ostr);
	params.push_back(OW_Param(XMLP_NEWVALUE, OW_Param::VALUESET, ostr.toString()));

	voidRetValOp op;
	intrinsicMethod(path, commandName, op, params,
		instanceNameToKey(path,"InstanceName"));
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class objectPathOp : public OW_ClientOperation
	{
	public:
		objectPathOp(OW_CIMObjectPathResultHandlerIFC& result_,
			OW_String ns_)
			: result(result_)
			, ns(ns_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			while (!parser.tokenIs(OW_CIMXMLParser::E_IRETURNVALUE))
			{
				OW_CIMXMLParser::tokenId token = parser.getToken();
				OW_CIMObjectPath cop = OW_XMLCIMFactory::createObjectPath(parser);
				switch (token)
				{
					case OW_CIMXMLParser::E_CLASSNAME:
						cop.setNameSpace(ns);
						break;
					case OW_CIMXMLParser::E_INSTANCENAME:
						cop.setNameSpace(ns);
						break;
					default:
						break;
				} // switch
				result.handle(cop);
			}
		}

		OW_CIMObjectPathResultHandlerIFC& result;
		OW_String ns;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::associatorNames(const OW_CIMObjectPath& path,
				OW_CIMObjectPathResultHandlerIFC& result,
				const OW_String& assocClass, const OW_String& resultClass,
				const OW_String& role, const OW_String& resultRole)
{
	static const char* const commandName = "AssociatorNames";

	OW_Array<OW_Param> params;
	OW_StringStream extra(1000);
	if (role.length() > 0)
	{
		params.push_back(OW_Param(XMLP_ROLE, role));
	}
	if (resultRole.length() > 0)
	{
		params.push_back(OW_Param(XMLP_RESULTROLE, resultRole));
	}

	if (path.getKeys().size() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">";
		OW_CIMtoXML(path, extra, OW_CIMtoXMLFlags::isInstanceName);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">" <<
		"<CLASSNAME NAME=\"" << path.getObjectName() <<
		"\"/></IPARAMVALUE>";
	}

	if (assocClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_ASSOCCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << assocClass <<
		"\"/></IPARAMVALUE>";
	}

	if (resultClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_RESULTCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"/></IPARAMVALUE>";
	}

	objectPathOp op(result, path.getNameSpace());
	intrinsicMethod(path, commandName, op, params,
		extra.toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::associators(const OW_CIMObjectPath& path,
						OW_CIMInstanceResultHandlerIFC& result,
		 	 			 const OW_String& assocClass, const OW_String& resultClass,
		 	 			 const OW_String& role, const OW_String& resultRole,
		 	 			 OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		 				 const OW_StringArray* propertyList)
{
	if (path.getKeys().size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"associators requires an instance path not a class path");
	}

	associatorsCommon(path, &result, 0, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::associatorsClasses(const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	if (path.getKeys().size() > 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"associatorsClasses requires a class path not an instance path");
	}

	associatorsCommon(path, 0, &result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class objectWithPathOp : public OW_ClientOperation
	{
	public:
		objectWithPathOp(
			OW_CIMInstanceResultHandlerIFC* iresult_,
			OW_CIMClassResultHandlerIFC* cresult_,
			OW_String ns_)
			: iresult(iresult_)
			, cresult(cresult_)
			, ns(ns_)
		{}
		virtual void operator ()(OW_CIMXMLParser &parser)
		{
			while (!parser.tokenIs(OW_CIMXMLParser::E_IRETURNVALUE))
			{
				OW_CIMInstance ci;
				OW_CIMClass cc;
				OW_CIMObjectPath cop = OW_XMLClass::getObjectWithPath(parser, cc, ci);
				if (cop)
				{
					if (iresult)
					{
						if (!ci)
						{
							OW_THROWCIMMSG(OW_CIMException::FAILED, "Server did not send an instance.");
						}
						iresult->handle(ci);
					}
					if (cresult)
					{
						if (!cc)
						{
							OW_THROWCIMMSG(OW_CIMException::FAILED, "Server did not send an class.");
						}
						cresult->handle(cc);
					}
				}
			}
		}

		OW_CIMInstanceResultHandlerIFC* iresult;
		OW_CIMClassResultHandlerIFC* cresult;
		OW_String ns;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::associatorsCommon(const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC* iresult,
	OW_CIMClassResultHandlerIFC* cresult,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	static const char* const commandName = "Associators";

	OW_Array<OW_Param> params;
	OW_StringStream extra(1000);

	if (role.length() > 0)
	{
		params.push_back(OW_Param(XMLP_ROLE, role));
	}
	if (resultRole.length() > 0)
	{
		params.push_back(OW_Param(XMLP_RESULTROLE, resultRole));
	}
	params.push_back(OW_Param(XMLP_INCLUDEQUALIFIERS, includeQualifiers));
	params.push_back(OW_Param(XMLP_INCLUDECLASSORIGIN, includeClassOrigin));

	if (propertyList)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";
	}

	if (path.getKeys().size() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">";
		OW_CIMtoXML(path, extra, OW_CIMtoXMLFlags::isInstanceName);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">" <<
		"<CLASSNAME NAME=\"" << path.getObjectName() <<
		"\"/></IPARAMVALUE>";
	}

	if (assocClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_ASSOCCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << assocClass <<
		"\"/></IPARAMVALUE>";
	}

	if (resultClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_RESULTCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"/></IPARAMVALUE>";
	}

	objectWithPathOp op(iresult,cresult,path.getNameSpace());
	intrinsicMethod(path, commandName, op, params,
												 extra.toString());

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::referenceNames(const OW_CIMObjectPath& path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& resultClass,
		const OW_String& role)
{
	static const char* const commandName = "ReferenceNames";
	OW_Array<OW_Param> params;
	OW_StringStream extra(1000);
	if (role.length() > 0)
	{
		params.push_back(OW_Param(XMLP_ROLE, role));
	}

	if (path.getKeys().size() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">";
		OW_CIMtoXML(path, extra, OW_CIMtoXMLFlags::isInstanceName);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">" <<
		"<CLASSNAME NAME=\"" << path.getObjectName() <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}

	if (resultClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_RESULTCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}

	objectPathOp op(result, path.getNameSpace());
	intrinsicMethod(path, commandName, op, params,
												 extra.toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::references(const OW_CIMObjectPath& path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& resultClass, const OW_String& role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	if (path.getKeys().size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"references requires an instance path not a class path");
	}

	referencesCommon(path, &result, 0, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::referencesClasses(const OW_CIMObjectPath& path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String& resultClass, const OW_String& role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	if (path.getKeys().size() > 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"referencesClasses requires a class path not an instance path");
	}

	referencesCommon(path, 0, &result, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::referencesCommon(const OW_CIMObjectPath& path,
		OW_CIMInstanceResultHandlerIFC* iresult,
		OW_CIMClassResultHandlerIFC* cresult,
		const OW_String& resultClass,
		const OW_String& role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	static const char* const commandName = "References";

	OW_Array<OW_Param> params;
	OW_StringStream extra(1000);

	if (role.length() > 0)
	{
		params.push_back(OW_Param(XMLP_ROLE, role));
	}
	params.push_back(OW_Param(XMLP_INCLUDEQUALIFIERS, includeQualifiers));
	params.push_back(OW_Param(XMLP_INCLUDECLASSORIGIN, includeClassOrigin));

	if (propertyList)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0; i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";

	}

	if (path.getKeys().size() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">";
		OW_CIMtoXML(path, extra, OW_CIMtoXMLFlags::isInstanceName);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">"
		<< "<CLASSNAME NAME=\"" << path.getObjectName()
		<< "\"></CLASSNAME></IPARAMVALUE>";
	}

	if (resultClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_RESULTCLASS << "\">"
		<< "<CLASSNAME NAME=\"" << resultClass <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}

	objectWithPathOp op(iresult,cresult,path.getNameSpace());
	intrinsicMethod(path, commandName, op, params, extra.toString());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMXMLCIMOMHandle::execQuery(const OW_CIMNameSpace& path,
	const OW_String& query, int wqlLevel)
{
	return execQueryE(path, query, OW_String("WQL") + OW_String(wqlLevel));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::execQuery(const OW_CIMNameSpace& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& query, const OW_String& queryLanguage)
{
	static const char* const commandName = "ExecQuery";

	OW_Array<OW_Param> params;

	params.push_back(OW_Param(XMLP_QUERYLANGUAGE, OW_XMLEscape(queryLanguage)));
	params.push_back(OW_Param(XMLP_QUERY, OW_XMLEscape(query)));

	OW_CIMObjectPath cop("", path.getNameSpace());
	objectWithPathOp op(&result, 0, path.getNameSpace());
	intrinsicMethod(cop, commandName, op, params);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_CIMXMLCIMOMHandle::getServerFeatures()
{
	return m_protocol->getFeatures();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::intrinsicMethod(
	const OW_CIMObjectPath& path, const OW_String& operation,
	OW_ClientOperation& op,
	const OW_Array<OW_Param>& params, const OW_String& extra)
{
	OW_Reference<std::iostream> iostrRef = m_protocol->beginRequest(operation, path.getNameSpace());
	std::iostream& iostr = *iostrRef;

	sendXMLHeader( operation, path, iostr);
	for (size_t i = 0; i < params.size(); i++)
	{
		iostr << "<IPARAMVALUE NAME=\"" << params[i].getArgName()
		<< "\">" << params[i].getArgValue() << "</IPARAMVALUE>";
	}
	if (extra.length() > 0)
	{
		iostr << extra;
	}
	sendXMLTrailer(iostr);
	doSendRequest(iostrRef, operation, path, true, op);
}


