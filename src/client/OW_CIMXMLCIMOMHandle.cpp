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
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_Format.hpp"
#include "OW_XMLEscape.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMException.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_HTTPDeflateIStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_XMLParseException.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_ResultHandlerIFC.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

using std::ostream;
using std::iostream;
using std::istream;
using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
CIMXMLCIMOMHandle::ClientOperation::~ClientOperation()
{
}

//////////////////////////////////////////////////////////////////////////////
CIMXMLCIMOMHandle::CIMXMLCIMOMHandle(const CIMProtocolIFCRef& prot)
	: ClientCIMOMHandle()
	, m_protocol(prot)
	, m_performStrictChecks(true) // TODO: Make a way to set this to false.
{
	m_iMessageID = 0;
	m_protocol->setContentType("application/xml");
}
//////////////////////////////////////////////////////////////////////////////
CIMProtocolIFCRef
CIMXMLCIMOMHandle::getWBEMProtocolHandler() const
{
	return m_protocol;
}

//////////////////////////////////////////////////////////////////////////////
void CIMXMLCIMOMHandle::close()
{
	m_protocol->close();
}

//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::sendCommonXMLHeader(ostream& ostr)
{
	if (++m_iMessageID > 65535)
	{
		m_iMessageID = 1;
	}
	ostr << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
	ostr << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">";
	ostr << "<MESSAGE ID=\"" << m_iMessageID << "\" PROTOCOLVERSION=\"1.0\">";
	ostr << "<SIMPLEREQ>";
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::sendIntrinsicXMLHeader( const String &sMethod,
	const String& ns,
	ostream& ostr)
{
	sendCommonXMLHeader(ostr);
	CIMNameSpace nameSpace(ns);
	ostr << "<IMETHODCALL NAME=\"" << sMethod << "\">";
	LocalCIMNameSpacetoXML(nameSpace, ostr);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::sendExtrinsicXMLHeader( const String &sMethod,
	const String& ns,
	const CIMObjectPath& path,
	ostream& ostr)
{
	sendCommonXMLHeader(ostr);
	CIMNameSpace nameSpace(ns);
	ostr << "<METHODCALL NAME=\"" << sMethod << "\">";
	if (path.isInstancePath())
	{
		CIMObjectPath pathWithNs(path);
		pathWithNs.setNameSpace(ns);
		CIMLocalInstancePathtoXML(pathWithNs,ostr);
	}
	else // it's a class
	{
		ostr << "<LOCALCLASSPATH>";
		LocalCIMNameSpacetoXML(nameSpace, ostr);
		ostr << "<CLASSNAME NAME=\"" << path.getClassName() << "\"/>";
		ostr << "</LOCALCLASSPATH>";
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::sendXMLTrailer( ostream& ostr, bool intrinsic)
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
CIMXMLCIMOMHandle::doSendRequest(
	const Reference<std::iostream>& ostrRef,
	const String& methodName,
	const String& cimObject,
	bool isIntrinsic,
	ClientOperation& op)
{
	CIMProtocolIStreamIFCRef istr = m_protocol->endRequest(
		ostrRef, methodName, cimObject, CIMProtocolIFC::E_CIM_OPERATION_REQUEST);
	// Debug stuff
	/*
	TempFileStream buf;
	buf << istr.rdbuf();
	ofstream ofstr("/tmp/rchXMLDump", ios::app);
	ofstr << "******* New dump ********" << endl;
	ofstr << buf.rdbuf() << endl;
	buf.rewind();
	XMLParser parser(&buf);
	*/
	// end debug stuff
	CIMXMLParser parser(*istr);
	try
	{
		checkNodeForCIMError(parser, methodName, isIntrinsic);
		if (isIntrinsic)
		{
			if (parser.tokenIsId(CIMXMLParser::E_IRETURNVALUE))
			{
				parser.mustGetNextTag(); // pass over <IRETURNVALUE>
				op(parser);
				parser.mustGetEndTag(); // pass </IRETURNVALUE>
			}
		}
		else
		{
			op(parser);
		}
		parser.mustGetEndTag(); // pass </(I)METHODRESPONSE>
		parser.mustGetEndTag(); // pass </SIMPLERSP>
		parser.mustGetEndTag(); // pass </MESSAGE>
		parser.mustGetEndTag(); // pass </CIM>
		HTTPUtils::eatEntity(*istr);
		getHTTPTrailers(istr);
		istr->checkForError();
	}
	catch (Exception&)
	{
		HTTPUtils::eatEntity(*istr);
		getHTTPTrailers(istr);
		istr->checkForError();
		throw;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::checkNodeForCIMError(CIMXMLParser& parser,
	const String& operation, bool isIntrinsic)
{
	//
	// Check for <CIM> element
	//
	if (!parser || !parser.tokenIsId(CIMXMLParser::E_CIM))
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Invalid XML. <CIM> not found.");
	}
	if (m_performStrictChecks)
	{
		String cimattr = parser.mustGetAttribute(CIMXMLParser::A_CIMVERSION);
		if (!cimattr.equals(CIMXMLParser::AV_CIMVERSION20_VALUE) &&
			!cimattr.equals(CIMXMLParser::AV_CIMVERSION21_VALUE) &&
			!cimattr.equals(CIMXMLParser::AV_CIMVERSION22_VALUE))
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								String("Return is for CIMVERSION " + cimattr).c_str());
		}
		cimattr = parser.mustGetAttribute(CIMXMLParser::A_DTDVERSION);
		if (!cimattr.equals(CIMXMLParser::AV_DTDVERSION20_VALUE) &&
			!cimattr.equals(CIMXMLParser::AV_DTDVERSION21_VALUE))
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								String("Return is for DTDVERSION " + cimattr).c_str());
		}
	}
	//
	// Find <MESSAGE>
	//
	parser.mustGetChildId(CIMXMLParser::E_MESSAGE);
	String cimattr = parser.mustGetAttribute(CIMXMLParser::A_ID);
	if (!cimattr.equals(String(m_iMessageID)))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							String("Return messageid="+cimattr+", expected="
										 +String(m_iMessageID)).c_str());
	}
	if (m_performStrictChecks)
	{
		cimattr = parser.mustGetAttribute(CIMXMLParser::A_PROTOCOLVERSION);
		if (!cimattr.equals(CIMXMLParser::AV_PROTOCOLVERSION10_VALUE) &&
			!cimattr.equals(CIMXMLParser::AV_PROTOCOLVERSION11_VALUE))
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								String("Return is for PROTOCOLVERSION "+cimattr).c_str());
		}
	}
	//
	// Find <SIMPLERSP>
	//
	parser.mustGetChildId(CIMXMLParser::E_SIMPLERSP);
	//
	// TODO-NICE: need to look for complex RSPs!!
	//
	//
	// <METHODRESPONSE> or <IMETHODRESPONSE>
	//
	parser.mustGetNextId(isIntrinsic ?
		CIMXMLParser::E_IMETHODRESPONSE :
		CIMXMLParser::E_METHODRESPONSE);
	String nameOfMethod = parser.mustGetAttribute("NAME");
	if (!nameOfMethod.equalsIgnoreCase(operation))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							String("Called "+operation+" but response was for "+
										 nameOfMethod).c_str());
	}
	parser.mustGetNextTag();
	//
	// See if there was an error, and if there was throw an equivalent
	// exception on the client
	//
	if (parser.tokenIsId(CIMXMLParser::E_ERROR))
	{
		String errCode = parser.mustGetAttribute(
			CIMXMLParser::A_CODE);
		String description = parser.mustGetAttribute(
			CIMXMLParser::A_DESCRIPTION);
		Int32 iErrCode;
		try
		{
			iErrCode = errCode.toInt32();
		}
		catch (const StringConversionException& e)
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Invalid xml. Error code \"%1\" is not an integer",
				e.getMessage()).c_str());
		}
		OW_THROWCIMMSG(CIMException::ErrNoType(errCode.toInt32()), description.c_str());
	}
	// For extrinsic methods, we leave the parser sitting on either
	// RETURNVALUE, PARAMVALUE or /METHODRESPONSE
	// For intrinsic methods, it's sitting on either IPARAMVALUE or
	// /IMETHODRESPONSE
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class voidRetValOp : public CIMXMLCIMOMHandle::ClientOperation
	{
	public:
		virtual void operator ()(CIMXMLParser &parser)
		{
			return;
		}
	};
}
//////////////////////////////////////////////////////////////////////////////
static String
instanceNameToKey(const CIMObjectPath& path,
	const String& parameterName)
{
	StringBuffer text = "<IPARAMVALUE NAME=\"" + parameterName + "\">";
	
	OStringStream ss;
	CIMInstanceNametoXML(path, ss);
	text += ss.toString();
	text += "</IPARAMVALUE>";
	return text.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct enumClassNamesOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		enumClassNamesOp(StringResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			while (parser.tokenIsId(CIMXMLParser::E_CLASSNAME))
			{
				result.handle(XMLCIMFactory::createObjectPath(parser).getClassName());
			}
		}
		StringResultHandlerIFC& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::enumClassNames(
		const String& ns,
		const String& className,
		StringResultHandlerIFC& result,
		EDeepFlag deep)
{
	static const char* const commandName = "EnumerateClassNames";
	Array<Param> params;
	if (!className.empty())
	{
		params.push_back(Param(CIMXMLParser::P_ClassName,Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	if (deep != E_SHALLOW)
	{
		params.push_back(Param(CIMXMLParser::P_DeepInheritance,deep));
	}
	enumClassNamesOp op(result);
	intrinsicMethod(ns, commandName, op, params);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct enumClassOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		enumClassOp(CIMClassResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			while (parser.tokenIsId(CIMXMLParser::E_CLASS))
			{
				result.handle(XMLCIMFactory::createClass(parser));
			}
		}
		CIMClassResultHandlerIFC& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::enumClass(const String& ns,
	const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin)
{
	static const char* const commandName = "EnumerateClasses";
	Array<Param> params;
	if (!className.empty())
	{
		params.push_back(Param(CIMXMLParser::P_ClassName,Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	if (localOnly != E_LOCAL_ONLY)
	{
		params.push_back(Param(CIMXMLParser::P_LocalOnly,localOnly));
	}
	if (deep != E_SHALLOW)
	{
		params.push_back(Param(CIMXMLParser::P_DeepInheritance,deep));
	}
	if (includeQualifiers != E_INCLUDE_QUALIFIERS)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeQualifiers, includeQualifiers));
	}
	if (includeClassOrigin != E_EXCLUDE_CLASS_ORIGIN)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeClassOrigin, includeClassOrigin));
	}
	enumClassOp op(result);
	intrinsicMethod(ns, commandName, op, params);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct enumInstanceNamesOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		enumInstanceNamesOp(CIMObjectPathResultHandlerIFC& result_,
			const String& ns_)
			: result(result_)
			, ns(ns_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			while (parser.tokenIsId(CIMXMLParser::E_INSTANCENAME))
			{
				CIMObjectPath p = XMLCIMFactory::createObjectPath(parser);
				p.setNameSpace(ns);
				result.handle(p);
			}
		}
		CIMObjectPathResultHandlerIFC& result;
		const String& ns;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result)
{
	static const char* const commandName = "EnumerateInstanceNames";
	Array<Param> params;
	if (!className.empty())
	{
		params.push_back(Param(CIMXMLParser::P_ClassName, Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(CIMException::NOT_FOUND, "Class was empty in "
							"EnumerateInstanceNames");
	}
	enumInstanceNamesOp op(result, ns);
	intrinsicMethod(ns, commandName, op, params);
}
//////////////////////////////////////////////////////////////////////////////
static inline void generatePropertyListXML(std::ostream& ostr,
	const StringArray* propertyList)
{
	if (propertyList)
	{
		ostr << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_PropertyList <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			ostr << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		ostr << "</VALUE.ARRAY></IPARAMVALUE>";
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct enumInstancesOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		enumInstancesOp(CIMInstanceResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			while (parser.tokenIsId(CIMXMLParser::E_VALUE_NAMEDINSTANCE))
			{
				parser.mustGetChildId(CIMXMLParser::E_INSTANCENAME);
				CIMObjectPath iop(XMLCIMFactory::createObjectPath(parser));
				CIMInstance ci = XMLCIMFactory::createInstance(parser);
				ci.setKeys(iop.getKeys());
				result.handle(ci);
				parser.mustGetEndTag(); // pass </VALUE.NAMEDINSTANCE>
			}
		}
		CIMInstanceResultHandlerIFC& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result, EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	static const char* const commandName = "EnumerateInstances";
	OStringStream extra(1000);
	Array<Param> params;
	if (className.empty())
	{
		OW_THROWCIMMSG(CIMException::NOT_FOUND, "Class Name was empty in "
			"EnumerateInstances");
	}
	params.push_back(Param(CIMXMLParser::P_ClassName, Param::VALUESET,
		"<CLASSNAME NAME=\""+className+"\"/>"));
	if (deep != E_DEEP)
	{
		params.push_back(Param(CIMXMLParser::P_DeepInheritance, deep));
	}
	if (localOnly != E_LOCAL_ONLY)
	{
		params.push_back(Param(CIMXMLParser::P_LocalOnly, localOnly));
	}
	if (includeQualifiers != E_EXCLUDE_QUALIFIERS)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeQualifiers, includeQualifiers));
	}
	if (includeClassOrigin != E_EXCLUDE_CLASS_ORIGIN)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeClassOrigin, includeClassOrigin));
	}
	generatePropertyListXML(extra,propertyList);
	enumInstancesOp op(result);
	intrinsicMethod(ns, commandName, op, params, extra.toString());
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct getClassOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		getClassOp(CIMClass& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			result = XMLCIMFactory::createClass(parser);
		}
		CIMClass& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMXMLCIMOMHandle::getClass(
	const String& ns,
	const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	static const char* const commandName = "GetClass";
	Array<Param> params;
	if (!className.empty())
	{
		params.push_back(Param(CIMXMLParser::P_ClassName,Param::VALUESET,
			"<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(CIMException::INVALID_CLASS, "no class given for "
			"GetClass()");
	}
	if (localOnly != E_LOCAL_ONLY)
	{
		params.push_back(Param(CIMXMLParser::P_LocalOnly, localOnly));
	}
	if (includeQualifiers != E_INCLUDE_QUALIFIERS)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeQualifiers, includeQualifiers));
	}
	if (includeClassOrigin != E_EXCLUDE_CLASS_ORIGIN)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeClassOrigin, includeClassOrigin));
	}
	OStringStream extra;
	generatePropertyListXML(extra,propertyList);
	CIMClass rval(CIMNULL);
	getClassOp op(rval);
	intrinsicMethod(ns, commandName, op, params, extra.toString());
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct getInstanceOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		getInstanceOp(CIMInstance& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			result = XMLCIMFactory::createInstance(parser);
		}
		CIMInstance& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMXMLCIMOMHandle::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	static const char* const commandName = "GetInstance";
	OStringStream extra(1000);
	Array<Param> params;
	if (localOnly != E_LOCAL_ONLY)
	{
		params.push_back(Param(CIMXMLParser::P_LocalOnly, localOnly));
	}
	if (includeQualifiers != E_EXCLUDE_QUALIFIERS)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeQualifiers, includeQualifiers));
	}
	if (includeClassOrigin != E_EXCLUDE_CLASS_ORIGIN)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeClassOrigin, includeClassOrigin));
	}
	CIMObjectPath path(instanceName);
	path.setNameSpace(ns);
	extra << instanceNameToKey(path, "InstanceName");
	generatePropertyListXML(extra,propertyList);
	CIMInstance rval(CIMNULL);
	getInstanceOp op(rval);
	intrinsicMethod(ns, commandName, op, params, extra.toString());
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct invokeMethodOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		invokeMethodOp(CIMValue& result_, CIMParamValueArray& outParams_)
			: result(result_)
			, outParams(outParams_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			// For extrinsic methods, the parser is sitting on either
			// RETURNVALUE, PARAMVALUE or /METHODRESPONSE
			// handle RETURNVALUE, which is optional
			if (parser.tokenIsId(CIMXMLParser::E_RETURNVALUE))
			{
				String returnType = parser.getAttribute(CIMXMLParser::A_PARAMTYPE);
				if (returnType.empty())
				{
					returnType = "string";
				}
				parser.mustGetChild();
				if (!parser.tokenIsId(CIMXMLParser::E_VALUE) &&
					!parser.tokenIsId(CIMXMLParser::E_VALUE_REFERENCE))
				{
					OW_THROWCIMMSG(CIMException::FAILED,
						"<RETURNVALUE> did not contain a <VALUE> or "
						"<VALUE.REFERENCE> element");
				}
				result = XMLCIMFactory::createValue(parser, returnType);
				parser.mustGetEndTag(); // pass /RETURNVALUE
			}
			// handle PARAMVALUE*
			for (size_t outParamCount = 0;
				  parser && parser.tokenIsId(CIMXMLParser::E_PARAMVALUE);
				  ++outParamCount)
			{
				String name = parser.mustGetAttribute(CIMXMLParser::A_NAME);
				String type = parser.getAttribute(CIMXMLParser::A_PARAMTYPE);
				if (type.empty())
				{
					type = "string";
				}
				parser.getNextTag();
				
				if (outParams.size() <= outParamCount)
				{
					// make sure there's enough space in the vector
					outParams.resize(outParamCount + 1);
				}
				int token = parser.getToken();
				if (token != CIMXMLParser::E_VALUE
					&& token != CIMXMLParser::E_VALUE_REFERENCE
					&& token != CIMXMLParser::E_VALUE_ARRAY
					&& token != CIMXMLParser::E_VALUE_REFARRAY
					)
				{
					outParams[outParamCount] = CIMParamValue(name,
						CIMValue(CIMNULL));
				}
				else
				{
					outParams[outParamCount] = CIMParamValue(name,
						XMLCIMFactory::createValue(parser, type));
				}
				parser.mustGetEndTag(); // pass /PARAMVALUE
			}
		}
		CIMValue& result;
		CIMParamValueArray& outParams;
	};
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMXMLCIMOMHandle::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName,
	const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams)
{
	Reference<std::iostream> iostrRef =
		m_protocol->beginRequest(methodName, ns);
	std::iostream& tfs = *iostrRef;
	sendExtrinsicXMLHeader(methodName, ns, path, tfs);
	for (size_t i = 0; i < inParams.size(); ++i)
	{
		tfs << "<PARAMVALUE NAME=\"" << inParams[i].getName() << "\"";
		CIMValue v = inParams[i].getValue();
		if (v)
		{
			String type = v.getCIMDataType().toString();
			if (type == "REF")
			{
				type = "reference";
			}
			tfs << " PARAMTYPE=\"" << type << "\">";
			CIMtoXML(inParams[i].getValue(), tfs);
		}
		else
		{
			tfs << '>';
		}
		tfs << "</PARAMVALUE>";
	}
	sendXMLTrailer(tfs, false);
	CIMValue rval(CIMNULL);
	invokeMethodOp op(rval, outParams);
	doSendRequest(iostrRef, methodName,
		ns + ":" + path.modelPath(),
		false, op);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct getQualifierTypeOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		getQualifierTypeOp(CIMQualifierType& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			XMLQualifier::processQualifierDecl(parser, result);
		}
		CIMQualifierType& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
CIMXMLCIMOMHandle::getQualifierType(const String& ns,
		const String& qualifierName)
{
	static const char* const commandName = "GetQualifier";
	Array<Param> params;
	params.push_back(Param(CIMXMLParser::P_QualifierName, qualifierName));
	CIMQualifierType rval;
	getQualifierTypeOp op(rval);
	intrinsicMethod(ns, commandName, op, params);
	return rval;
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::setQualifierType(const String& ns,
		const CIMQualifierType& qt)
{
	static const char* const commandName = "SetQualifier";
	OStringStream extra;
	extra << "<IPARAMVALUE NAME=\"QualifierDeclaration\">";
	CIMtoXML(qt, extra);
	extra << "</IPARAMVALUE>";
	voidRetValOp op;
	intrinsicMethod(ns, commandName, op, Array<Param>(),
						 extra.toString());
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::deleteQualifierType(const String& ns, const String& qualName)
{
	static const char* const commandName = "DeleteQualifier";
	Array<Param> params;
	params.push_back(Param(CIMXMLParser::P_QualifierName, qualName));
	voidRetValOp op;
	intrinsicMethod(ns, commandName, op, params);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct enumQualifierTypesOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		enumQualifierTypesOp(CIMQualifierTypeResultHandlerIFC& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			while (parser.tokenIsId(CIMXMLParser::E_QUALIFIER_DECLARATION))
			{
				CIMQualifierType cqt;
				XMLQualifier::processQualifierDecl(parser, cqt);
				result.handle(cqt);
			}
		}
		CIMQualifierTypeResultHandlerIFC& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result)
{
	static const char* const commandName = "EnumerateQualifiers";
	enumQualifierTypesOp op(result);
	intrinsicMethod(ns, commandName, op);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::modifyClass(const String &ns,
		const CIMClass& cc)
{
	static const char* const commandName = "ModifyClass";
	OStringStream extra(1024);
	extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ModifiedClass << "\">";
	CIMtoXML(cc, extra);
	extra << "</IPARAMVALUE>";
	voidRetValOp op;
	intrinsicMethod(ns, commandName, op, Array<Param>(),
		extra.toString());
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::createClass(const String& ns,
		const CIMClass& cc)
{
	static const char* const commandName = "CreateClass";
	OStringStream ostr;
	ostr << "<IPARAMVALUE NAME=\"NewClass\">";
	CIMtoXML(cc, ostr);
	ostr << "</IPARAMVALUE>";
	voidRetValOp op;
	intrinsicMethod(ns, commandName, op, Array<Param>(),
		ostr.toString());
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::deleteClass(const String& nameSpace, const String& className)
{
	static const char* const commandName = "DeleteClass";
	Array<Param> params;
	if (!className.empty())
	{
		params.push_back(Param(CIMXMLParser::P_ClassName,Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(CIMException::INVALID_CLASS, "No ClassName passed"
							" to deleteClass()");
	}
	voidRetValOp op;
	intrinsicMethod(nameSpace, commandName, op, params);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList)
{
	static const char* const commandName = "ModifyInstance";
	// This check is wrong, because instances w/out keys are singletons.
	//if (modifiedInstance.getKeyValuePairs().empty())
	//{
	//	OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Instance must have keys");
	//}
	OStringStream ostr(1000);
	ostr << "<IPARAMVALUE NAME=\"ModifiedInstance\">";
	ostr << "<VALUE.NAMEDINSTANCE>";
	CIMInstanceNameAndInstancetoXML(modifiedInstance, ostr,
		CIMObjectPath(ns, modifiedInstance));
	ostr << "</VALUE.NAMEDINSTANCE></IPARAMVALUE>";
	
	Array<Param> params;
	if (includeQualifiers != E_INCLUDE_QUALIFIERS)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeQualifiers, includeQualifiers));
	}
	
	generatePropertyListXML(ostr, propertyList);
	
	voidRetValOp op;
	intrinsicMethod(ns, commandName, op, params,
		ostr.toString());
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct createInstanceOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		createInstanceOp(CIMObjectPath& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			if (!parser.tokenIsId(CIMXMLParser::E_INSTANCENAME))
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Expected but did not get <INSTANCENAME>");
			}
			result = XMLCIMFactory::createObjectPath(parser);
		}
		CIMObjectPath& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
CIMXMLCIMOMHandle::createInstance(const String& ns,
	const CIMInstance& ci)
{
	static const char* const commandName = "CreateInstance";
	// This check isn't necessary, because of singleton classes/instances
	// that don't have any keys
	//if (ci.getKeyValuePairs().empty())
	//{
	//	OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
	//		"The instance does not have any keys");
	//}
	OStringStream ostr;
	ostr << "<IPARAMVALUE NAME=\"NewInstance\">";
	CIMInstancetoXML(ci, ostr);
	ostr << "</IPARAMVALUE>";
	CIMObjectPath rval(CIMNULL);
	createInstanceOp op(rval);
	intrinsicMethod(ns, commandName, op, Array<Param>(), ostr.toString());
	rval.setNameSpace(ns);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::deleteInstance(const String& ns, const CIMObjectPath& inst)
{
	static const char* const commandName = "DeleteInstance";
	Array<Param> params;
	voidRetValOp op;
	intrinsicMethod(ns, commandName, op, params,
						 instanceNameToKey(inst, "InstanceName"));
}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::setProperty(
	const String& ns,
	const CIMObjectPath& path,
	const String& propName,
	const CIMValue& cv)
{
	static const char* const commandName = "SetProperty";
	Array<Param> params;
	params.push_back(Param(CIMXMLParser::P_PropertyName, propName));
	if (cv)
	{
		OStringStream ostr;
		CIMtoXML(cv, ostr);
		params.push_back(Param(CIMXMLParser::P_NewValue, Param::VALUESET, ostr.toString()));
	}
	voidRetValOp op;
	intrinsicMethod(ns, commandName, op, params,
		instanceNameToKey(path,"InstanceName"));
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct getPropertyOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		getPropertyOp(CIMValue& result_)
			: result(result_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			if (!parser.tokenIsId(CIMXMLParser::E_IRETURNVALUE))
			{
				// "string" because we don't know the type--defect in the spec.
				result = XMLCIMFactory::createValue(parser, "string");
			}
			// else it was a NULL value
		}
		CIMValue& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMXMLCIMOMHandle::getProperty(
	const String& ns,
	const CIMObjectPath& path,
	const String& propName)
{
	static const char* const commandName = "GetProperty";
	Array<Param> params;
	params.push_back(Param(CIMXMLParser::P_PropertyName, propName));
	CIMValue rval(CIMNULL);
	getPropertyOp op(rval);
	intrinsicMethod(ns, commandName, op, params,
		instanceNameToKey(path,"InstanceName"));
	return rval;
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
namespace
{
	struct objectPathOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		objectPathOp(CIMObjectPathResultHandlerIFC& result_,
			const String& ns_)
			: result(result_)
			, ns(ns_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			while (!parser.tokenIsId(CIMXMLParser::E_IRETURNVALUE))
			{
				CIMXMLParser::tokenId token = parser.getToken();
				CIMObjectPath cop = XMLCIMFactory::createObjectPath(parser);
				switch (token)
				{
					case CIMXMLParser::E_CLASSNAME:
						cop.setNameSpace(ns);
						break;
					case CIMXMLParser::E_INSTANCENAME:
						cop.setNameSpace(ns);
						break;
					default:
						break;
				} // switch
				result.handle(cop);
			}
		}
		CIMObjectPathResultHandlerIFC& result;
		String ns;
	};
//////////////////////////////////////////////////////////////////////////////
	struct objectWithPathOp : public CIMXMLCIMOMHandle::ClientOperation
	{
		objectWithPathOp(
			CIMInstanceResultHandlerIFC* iresult_,
			CIMClassResultHandlerIFC* cresult_,
			const String& ns_)
			: iresult(iresult_)
			, cresult(cresult_)
			, ns(ns_)
		{}
		virtual void operator ()(CIMXMLParser &parser)
		{
			while (!parser.tokenIsId(CIMXMLParser::E_IRETURNVALUE))
			{
				CIMInstance ci(CIMNULL);
				CIMClass cc(CIMNULL);
				CIMObjectPath cop = XMLClass::getObjectWithPath(parser, cc, ci);
				if (cop)
				{
					if (iresult)
					{
						if (!ci)
						{
							OW_THROWCIMMSG(CIMException::FAILED, "Server did not send an instance.");
						}
						ci.setKeys(cop.getKeys());
						iresult->handle(ci);
					}
					if (cresult)
					{
						if (!cc)
						{
							OW_THROWCIMMSG(CIMException::FAILED, "Server did not send an class.");
						}
						cresult->handle(cc);
					}
				}
			}
		}
		CIMInstanceResultHandlerIFC* iresult;
		CIMClassResultHandlerIFC* cresult;
		String ns;
	};
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::associatorNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole)
{
	static const char* const commandName = "AssociatorNames";
	Array<Param> params;
	OStringStream extra(1000);
	if (!role.empty())
	{
		params.push_back(Param(CIMXMLParser::P_Role, role));
	}
	if (!resultRole.empty())
	{
		params.push_back(Param(CIMXMLParser::P_ResultRole, resultRole));
	}
	if (path.isInstancePath())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ObjectName << "\">";
		CIMInstanceNametoXML(path, extra);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ObjectName << "\">" <<
		"<CLASSNAME NAME=\"" << path.getClassName() <<
		"\"/></IPARAMVALUE>";
	}
	if (!assocClass.empty())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_AssocClass << "\">" <<
		"<CLASSNAME NAME=\"" << assocClass <<
		"\"/></IPARAMVALUE>";
	}
	if (!resultClass.empty())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ResultClass << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"/></IPARAMVALUE>";
	}
	objectPathOp op(result, ns);
	intrinsicMethod(ns, commandName, op, params,
		extra.toString());
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::associators(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	if (!path.isInstancePath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"associators requires an instance path not a class path");
	}
	associatorsCommon(ns, path, &result, 0, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::associatorsClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	if (!path.isClassPath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"associatorsClasses requires a class path not an instance path");
	}
	associatorsCommon(ns, path, 0, &result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::associatorsCommon(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC* iresult,
	CIMClassResultHandlerIFC* cresult,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	static const char* const commandName = "Associators";
	Array<Param> params;
	OStringStream extra(1000);
	if (!role.empty())
	{
		params.push_back(Param(CIMXMLParser::P_Role, role));
	}
	if (!resultRole.empty())
	{
		params.push_back(Param(CIMXMLParser::P_ResultRole, resultRole));
	}
	if (includeQualifiers != E_EXCLUDE_QUALIFIERS)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeQualifiers, includeQualifiers));
	}
	if (includeClassOrigin != E_EXCLUDE_CLASS_ORIGIN)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeClassOrigin, includeClassOrigin));
	}
	generatePropertyListXML(extra,propertyList);
	if (path.isInstancePath())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ObjectName << "\">";
		CIMInstanceNametoXML(path, extra);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ObjectName << "\">" <<
		"<CLASSNAME NAME=\"" << path.getClassName() <<
		"\"/></IPARAMVALUE>";
	}
	if (!assocClass.empty())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_AssocClass << "\">" <<
		"<CLASSNAME NAME=\"" << assocClass <<
		"\"/></IPARAMVALUE>";
	}
	if (!resultClass.empty())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ResultClass << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"/></IPARAMVALUE>";
	}
	objectWithPathOp op(iresult, cresult, ns);
	intrinsicMethod(ns, commandName, op, params, extra.toString());
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::referenceNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass,
	const String& role)
{
	static const char* const commandName = "ReferenceNames";
	Array<Param> params;
	OStringStream extra(1000);
	if (!role.empty())
	{
		params.push_back(Param(CIMXMLParser::P_Role, role));
	}
	if (path.isInstancePath())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ObjectName << "\">";
		CIMInstanceNametoXML(path, extra);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ObjectName << "\">" <<
		"<CLASSNAME NAME=\"" << path.getClassName() <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}
	if (!resultClass.empty())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ResultClass << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}
	objectPathOp op(result, ns);
	intrinsicMethod(ns, commandName, op, params, extra.toString());
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::references(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	if (!path.isInstancePath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"references requires an instance path not a class path");
	}
	referencesCommon(ns, path, &result, 0, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::referencesClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	if (!path.isClassPath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"referencesClasses requires a class path not an instance path");
	}
	referencesCommon(ns, path, 0, &result, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::referencesCommon(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC* iresult,
	CIMClassResultHandlerIFC* cresult,
	const String& resultClass,
	const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	static const char* const commandName = "References";
	Array<Param> params;
	OStringStream extra(1000);
	if (!role.empty())
	{
		params.push_back(Param(CIMXMLParser::P_Role, role));
	}
	if (includeQualifiers != E_EXCLUDE_QUALIFIERS)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeQualifiers, includeQualifiers));
	}
	if (includeClassOrigin != E_EXCLUDE_CLASS_ORIGIN)
	{
		params.push_back(Param(CIMXMLParser::P_IncludeClassOrigin, includeClassOrigin));
	}
	generatePropertyListXML(extra,propertyList);
	if (path.isInstancePath())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ObjectName << "\">";
		CIMInstanceNametoXML(path, extra);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ObjectName << "\">"
		<< "<CLASSNAME NAME=\"" << path.getClassName()
		<< "\"></CLASSNAME></IPARAMVALUE>";
	}
	if (!resultClass.empty())
	{
		extra << "<IPARAMVALUE NAME=\"" << CIMXMLParser::P_ResultClass << "\">"
		<< "<CLASSNAME NAME=\"" << resultClass <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}
	objectWithPathOp op(iresult,cresult,ns);
	intrinsicMethod(ns, commandName, op, params, extra.toString());
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration
CIMXMLCIMOMHandle::execQuery(
	const String& ns,
	const String& query, int wqlLevel)
{
	return CIMOMHandleIFC::execQueryE(ns, query,
		String("WQL") + String(wqlLevel));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::execQuery(
	const String& ns,
	CIMInstanceResultHandlerIFC& result,
	const String& query, const String& queryLanguage)
{
	static const char* const commandName = "ExecQuery";
	Array<Param> params;
	params.push_back(Param(CIMXMLParser::P_QueryLanguage, XMLEscape(queryLanguage)));
	params.push_back(Param(CIMXMLParser::P_Query, XMLEscape(query)));
	objectWithPathOp op(&result, 0, ns);
	intrinsicMethod(ns, commandName, op, params);
}
//////////////////////////////////////////////////////////////////////////////
CIMFeatures
CIMXMLCIMOMHandle::getServerFeatures()
{
	return m_protocol->getFeatures();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLCIMOMHandle::intrinsicMethod(
	const String& ns, const String& operation,
	ClientOperation& op,
	const Array<Param>& params, const String& extra)
{
	Reference<std::iostream> iostrRef = m_protocol->beginRequest(operation, ns);
	std::iostream& iostr = *iostrRef;
	sendIntrinsicXMLHeader(operation, ns, iostr);
	for (size_t i = 0; i < params.size(); i++)
	{
		iostr << "<IPARAMVALUE NAME=\"" << params[i].getArgName()
		<< "\">" << params[i].getArgValue() << "</IPARAMVALUE>";
	}
	if (!extra.empty())
	{
		iostr << extra;
	}
	sendXMLTrailer(iostr);
	doSendRequest(iostrRef, operation, ns, true, op);
}

//////////////////////////////////////////////////////////////////////////////
bool
CIMXMLCIMOMHandle::setHTTPRequestHeader(const String& hdrName,
	const String& hdrValue)
{
	bool cc = false;
	IntrusiveReference<HTTPClient> httpClient =
		m_protocol.cast_to<HTTPClient>();
	if (httpClient)
	{
		httpClient->addCustomHeader(hdrName, hdrValue);
		cc = true;
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
bool
CIMXMLCIMOMHandle::getHTTPResponseHeader(const String& hdrName,
	String& valueOut) const
{
	bool cc = false;
	IntrusiveReference<HTTPClient> httpClient =
		m_protocol.cast_to<HTTPClient>();
	if (httpClient)
	{
		if (!(cc = httpClient->getResponseHeader(hdrName, valueOut)))
		{
			if (HTTPUtils::headerHasKey(m_trailers, hdrName))
			{
				cc = true;
				valueOut = HTTPUtils::getHeaderValue(m_trailers, hdrName);
			}
		}
	}
	return cc;
}

} // end namespace OpenWBEM

