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
#include "OW_XMLListener.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMListenerCallback.hpp"
#include "blocxx/Format.hpp"
#include "OW_XMLEscape.hpp"
#include "blocxx/TempFileStream.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Assertion.hpp"
#include "OW_ServiceIFCNames.hpp"

namespace OW_NAMESPACE
{

using std::ostream;
//////////////////////////////////////////////////////////////////////////////
XMLListener::XMLListener(const CIMListenerCallbackRef& callback)
: RequestHandlerIFCXML(),  m_callback(callback)
{
}
//////////////////////////////////////////////////////////////////////////////
XMLListener::~XMLListener()
{
}
//////////////////////////////////////////////////////////////////////////////
String
XMLListener::getName() const
{
	return ServiceIFCNames::XMLListener;
}
//////////////////////////////////////////////////////////////////////////////
RequestHandlerIFC*
XMLListener::clone() const
{
	return new XMLListener(*this);
}

namespace
{
	bool currentTagMatches(const CIMXMLParser& parser, CIMXMLParser::tokenId id, XMLToken::XMLType type)
	{
		if( !parser )
		{
			return false;
		}
		return parser.tokenIsId(id) && (parser.getTokenType() == type);
	}

	bool currentTokenIsStartTag(const CIMXMLParser& parser, CIMXMLParser::tokenId id)
	{
		return currentTagMatches(parser, id, XMLToken::START_TAG);
	}

	bool currentTokenIsEndTag(const CIMXMLParser& parser, CIMXMLParser::tokenId id)
	{
		return currentTagMatches(parser, id, XMLToken::END_TAG);
	}
}

//////////////////////////////////////////////////////////////////////////////
int
XMLListener::executeXML(CIMXMLParser& parser, ostream* ostrEntity,
	ostream* ostrError, OperationContext& context)
{
	clearError();
	String messageId = parser.mustGetAttribute(CIMXMLParser::A_ID);
	parser.getChild();
	if (!parser)
	{
		OW_THROW(CIMErrorException, CIMErrorException::request_not_loosely_valid);
	}
	makeXMLHeader(messageId, *ostrEntity);

	if (currentTokenIsStartTag(parser, CIMXMLParser::E_MULTIEXPREQ))
	{
		parser.getChild();
		if (!parser || !currentTokenIsStartTag(parser, CIMXMLParser::E_SIMPLEEXPREQ))
		{
			OW_THROW(CIMErrorException, CIMErrorException::request_not_loosely_valid);
		}

		while (currentTokenIsStartTag(parser, CIMXMLParser::E_SIMPLEEXPREQ))
		{
			TempFileStream ostrEnt, ostrErr;
			processSimpleExpReq(parser, ostrEnt, ostrErr, messageId, context);
			if (hasError())
			{
				(*ostrEntity) << ostrErr.rdbuf();
				clearError();
			}
			else
			{
				(*ostrEntity) << ostrEnt.rdbuf();
			}

			// If the connection is reused, this can eat WAY more xml nodes than
			// would be intended.  It fulfills our needs for now, but will
			// probably need to be fixed properly in the future so that it only
			// eats as much as we need to and no more.
			parser.getNextId(CIMXMLParser::E_SIMPLEEXPREQ, false);
		}
	}
	else if (parser.tokenIsId(CIMXMLParser::E_SIMPLEEXPREQ))
	{
		processSimpleExpReq(parser, *ostrEntity, *ostrError, messageId, context);
	}
	else
	{
		OW_THROW(CIMErrorException, CIMErrorException::request_not_loosely_valid);
	}
	if (!hasError())
	{
		(*ostrEntity) << "</MESSAGE></CIM>\r\n";
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void
XMLListener::doOptions(CIMFeatures &cf, OperationContext&)
{
	cf.extURL = "http://www.dmtf.org/cim/mapping/http/v1.0";
	cf.cimProduct = CIMFeatures::LISTENER;
	cf.cimom = "/cimom";
	cf.protocolVersion = "1.1";
	cf.supportedGroups.clear();
	cf.supportedGroups.push_back("Indication");
	cf.supportedQueryLanguages.clear();
	cf.supportsBatch = true;
	cf.validation = "";
}
//////////////////////////////////////////////////////////////////////////////
void
XMLListener::processSimpleExpReq(CIMXMLParser& parser,
	ostream& ostrEntity, ostream& ostrError, const String& messageId, OperationContext&)
{
	try
	{
		if (!parser.tokenIsId(CIMXMLParser::E_SIMPLEEXPREQ))
		{
			OW_THROW(CIMErrorException, CIMErrorException::request_not_loosely_valid);
		}
		parser.mustGetChildId(CIMXMLParser::E_EXPMETHODCALL);
		parser.mustGetNextTag();
		while (parser.tokenIsId(CIMXMLParser::E_EXPPARAMVALUE)
			|| parser.tokenIsId(CIMXMLParser::E_IPARAMVALUE) // do this because of a bug in the CIMXML 2.1 DTD
		)
		{
			String paramName = parser.getAttribute(CIMXMLParser::A_NAME);
			if (paramName != "NewIndication")
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					paramName.c_str());
			}
			parser.mustGetChildId(CIMXMLParser::E_INSTANCE);
			CIMInstance inst = XMLCIMFactory::createInstance(parser);
			m_callback->indicationOccurred(inst, m_path);
			parser.mustGetEndTag(); // pass </EXPPARAMVALUE>
		}
		parser.mustGetEndTag(); // pass </EXPMETHODCALL>
		parser.mustGetEndTag(); // pass </SIMPLEEXPREQ>
		ostrEntity << "<SIMPLEEXPRSP>";
		ostrEntity << "<EXPMETHODRESPONSE NAME=\"ExportIndication\">";
		ostrEntity << "</EXPMETHODRESPONSE>";
		ostrEntity << "</SIMPLEEXPRSP>";
	}
	catch(CIMException& ce)
	{
		makeXMLHeader(messageId, ostrError);
		outputError(ce.getErrNo(), ce.getDescription(), ostrError);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
XMLListener::outputError(CIMException::ErrNoType errorCode,
	const String& msg, ostream& ostr)
{
	setError(errorCode, msg);
	ostr << "<SIMPLEEXPRSP>";
	ostr << "<EXPMETHODRESPONSE NAME=\"ExportIndication\">";
	ostr << "<ERROR CODE=\"" << errorCode << "\"";
	// always have to send DESCRIPTION, since there are clients that can't handle it not being there.
	//if (!msg.empty())
	//{
		ostr << " DESCRIPTION=\"" <<
		XMLEscape(msg) <<
		"\"";
	//}
	ostr << "></ERROR>";
	ostr << "</EXPMETHODRESPONSE>";
	ostr << "</SIMPLEEXPRSP>";
	ostr << "</MESSAGE></CIM>\r\n";
}
//////////////////////////////////////////////////////////////////////////////
StringArray
XMLListener::getSupportedContentTypes() const
{
	StringArray rval;
	rval.push_back("text/xml");
	rval.push_back("application/xml");
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
String
XMLListener::getContentType() const
{
	return String("application/xml");
}

//////////////////////////////////////////////////////////////////////////////
void
XMLListener::init(const ServiceEnvironmentIFCRef& env)
{
	setEnvironment(env);
}

//////////////////////////////////////////////////////////////////////////////
void
XMLListener::shutdown()
{
}

} // end namespace OW_NAMESPACE

