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
#include "OW_config.h"
#include "OW_XMLListener.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMListenerCallback.hpp"
#include "OW_Format.hpp"
#include "OW_XMLEscape.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Assertion.hpp"

namespace OpenWBEM
{

using std::ostream;
//////////////////////////////////////////////////////////////////////////////
XMLListener::XMLListener(CIMListenerCallback* callback)
: RequestHandlerIFCXML(),  m_callback(callback)
{
}
//////////////////////////////////////////////////////////////////////////////
XMLListener::~XMLListener()
{
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
	if (parser.tokenIsId(CIMXMLParser::E_MULTIEXPREQ))
	{
		parser.getChild();
		if (!parser)
		{
			OW_THROW(CIMErrorException, CIMErrorException::request_not_loosely_valid);
		}
		while(parser)
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
			parser.getNext(CIMXMLParser::E_SIMPLEEXPREQ);
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
	(void)ostrError;
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
		outputError(ce.getErrNo(), ce.getMessage(), ostrError);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
XMLListener::outputError(CIMException::ErrNoType errorCode,
	String msg, ostream& ostr)
{
	setError(errorCode, msg);
	ostr << "<SIMPLEEXPRSP>";
	ostr << "<EXPMETHODRESPONSE NAME=\"ExportIndication\">";
	ostr << "<ERROR CODE=\"" << errorCode << "\" DESCRIPTION=\"" <<
		XMLEscape(msg) <<
		"\"></ERROR>";
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

} // end namespace OpenWBEM

