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

using std::ostream;

//////////////////////////////////////////////////////////////////////////////
OW_XMLListener::OW_XMLListener(OW_CIMListenerCallback* callback)
: OW_RequestHandlerIFCXML(),  m_callback(callback)
{
}

//////////////////////////////////////////////////////////////////////////////
int
OW_XMLListener::executeXML(OW_CIMXMLParser& parser, ostream* ostrEntity,
	ostream* ostrError, const OW_String& /*userName*/)
{
	m_hasError = false;

	OW_String messageId = parser.mustGetAttribute(OW_CIMXMLParser::A_MSG_ID);

	parser.getChild();

	if (!parser)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
	}

	makeXMLHeader(messageId, *ostrEntity);

	if (parser.tokenIs(OW_CIMXMLParser::E_MULTIEXPREQ))
	//if (node.getToken() == OW_CIMXMLParser::E_MULTIEXPREQ)
	{
		parser.getChild();
		if (!parser)
		{
			OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
		}
		while(parser)
		{
			OW_TempFileStream ostrEnt, ostrErr;
			processSimpleExpReq(parser, ostrEnt, ostrErr, messageId);
			if (m_hasError)
			{
				(*ostrEntity) << ostrErr.rdbuf();
				m_hasError = false;
			}
			else
			{
				(*ostrEntity) << ostrEnt.rdbuf();
			}

			parser.getNext(OW_CIMXMLParser::E_SIMPLEEXPREQ);
		}
	}
	else if (parser.tokenIs(OW_CIMXMLParser::E_SIMPLEEXPREQ))
	{
		processSimpleExpReq(parser, *ostrEntity, *ostrError, messageId);
	}
	else
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
	}


	(void)ostrError;

	(*ostrEntity) << "</MESSAGE></CIM>\r\n";
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLListener::doOptions(OW_CIMFeatures &cf, const OW_SortedVectorMap<OW_String, OW_String>& /*handlerVars*/)
{
	cf.extURL = "http://www.dmtf.org/cim/mapping/http/v1.0";
	cf.cimProduct = OW_CIMFeatures::LISTENER;
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
OW_XMLListener::processSimpleExpReq(OW_CIMXMLParser& parser,
	ostream& ostrEntity, ostream& ostrError, const OW_String& messageId)
{
	try
	{
		if (!parser.tokenIs(OW_CIMXMLParser::E_SIMPLEEXPREQ))
		{
			OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
		}
		parser.mustGetChild(OW_CIMXMLParser::E_EXPMETHODCALL);
//		node = node.mustChildElement(OW_CIMXMLParser::E_EXPPARAMVALUE);
		parser.mustGetChild(OW_CIMXMLParser::E_IPARAMVALUE);
		parser.mustGetChild(OW_CIMXMLParser::E_INSTANCE);
		OW_CIMInstance inst = OW_XMLCIMFactory::createInstance(parser);
		m_callback->indicationOccurred(inst, m_path);
		ostrEntity << "<SIMPLEEXPRSP>";
		ostrEntity << "<EXPMETHODRESPONSE NAME=\"ExportIndication\">";

		ostrEntity << "</EXPMETHODRESPONSE>";

		ostrEntity << "</SIMPLEEXPRSP>";
	}
	catch(OW_CIMException& ce)
	{
		m_hasError = true;
		makeXMLHeader(messageId, ostrError);
		outputError(ce.getErrNo(), ce.getMessage(), ostrError);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLListener::outputError(OW_CIMException::ErrNoType errorCode,
	OW_String msg, ostream& ostr)
{

	ostr << "<SIMPLEEXPRSP>";
	ostr << "<EXPMETHODRESPONSE NAME=\"ExportIndication\">";


	ostr << "<ERROR CODE=\"" << errorCode << "\" DESCRIPTION=\"" <<
		OW_XMLEscape(msg) <<
		"\"></ERROR>";

	ostr << "</EXPMETHODRESPONSE>";

	ostr << "</SIMPLEEXPRSP>";
	ostr << "</MESSAGE></CIM>\r\n";
}



//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_XMLListener::getSupportedContentTypes() const
{
	OW_StringArray rval;
	rval.push_back("text/xml");
	rval.push_back("application/xml");
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLListener::getContentType() const
{
	return OW_String("application/xml");
}

//////////////////////////////////////////////////////////////////////////////
