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
#include "OW_XMLOperationGeneric.hpp"
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
#include "OW_XMLParser.hpp"
#include "OW_XMLException.hpp"

using std::ostream;

//////////////////////////////////////////////////////////////////////////////
OW_XMLListener::OW_XMLListener(OW_CIMListenerCallback* callback)
: OW_RequestHandlerIFC(), m_hasError(false), m_callback(callback)
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLListener::doProcess(std::istream* istr, std::ostream* ostrEntity,
        std::ostream* ostrError, const OW_SortedVector<OW_String, OW_String>& handlerVars)
{

	OW_ASSERT(ostrEntity);
	OW_ASSERT(ostrError);

	OW_SortedVector<OW_String, OW_String>::const_iterator i = handlerVars.find(OW_ConfigOpts::HTTP_PATH_opt);
	if (i != handlerVars.end())
	{
		setPath((*i).second);
	}

	OW_XMLNode node;
	try
	{
		OW_XMLParser parser(istr);
		node = parser.parse();
		if (!node)
		{
			OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_well_formed);
		}
	}
	catch (OW_XMLException& e)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_well_formed);
	}

	node = OW_XMLOperationGeneric::XMLGetCIMElement(node);
	if (!node)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
	}

	node = node.mustFindElement(OW_XMLNode::XML_ELEMENT_MESSAGE);
	if (!node)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
	}

	OW_String userName;
	i = handlerVars.find(OW_ConfigOpts::USER_NAME_opt);
	if (i != handlerVars.end())
	{
		userName = (*i).second;
	}

	executeXML(node, ostrEntity, ostrError, userName);
}


//////////////////////////////////////////////////////////////////////////////
static void
makeXMLHeader(const OW_String& messageID, ostream& ostr)
{
	ostr << "<?xml version=\"1.0\" ?>\r\n";
	ostr << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">\r\n";
	ostr << "<MESSAGE ID=\"" << messageID << "\" PROTOCOLVERSION=\"1.0\">\r\n";
}
//////////////////////////////////////////////////////////////////////////////
int
OW_XMLListener::executeXML(OW_XMLNode& node, ostream* ostrEntity,
	ostream* ostrError, const OW_String& /*userName*/)
{
	m_hasError = false;

	OW_String messageId = node.mustGetAttribute(OW_XMLOperationGeneric::MSG_ID);

	node = node.getChild();

	if (!node)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
	}

	makeXMLHeader(messageId, *ostrEntity);

	if (node.getToken() == OW_XMLNode::XML_ELEMENT_MULTIEXPREQ)
	{
		node = node.getChild();
		if (!node)
		{
			OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
		}
		while(node)
		{
			OW_TempFileStream ostrEnt, ostrErr;
			processSimpleExpReq(node, ostrEnt, ostrErr, messageId);
			if (m_hasError)
			{
				(*ostrEntity) << ostrErr.rdbuf();
				m_hasError = false;
			}
			else
			{
				(*ostrEntity) << ostrEnt.rdbuf();
			}

			node = node.getNext();
		}
	}
	else if (node.getToken() == OW_XMLNode::XML_ELEMENT_SIMPLEEXPREQ)
	{
		processSimpleExpReq(node, *ostrEntity, *ostrError, messageId);
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
OW_XMLListener::doOptions(OW_CIMFeatures &cf, const OW_SortedVector<OW_String, OW_String>& /*handlerVars*/)
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
OW_XMLListener::processSimpleExpReq(const OW_XMLNode& startNode,
	ostream& ostrEntity, ostream& ostrError, const OW_String& messageId)
{
	try
	{
		OW_XMLNode node = startNode;
		if (node.getToken() != OW_XMLNode::XML_ELEMENT_SIMPLEEXPREQ)
		{
			OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
		}
		node = node.mustChildElement(OW_XMLNode::XML_ELEMENT_EXPMETHODCALL);
//		node = node.mustChildElement(OW_XMLNode::XML_ELEMENT_EXPPARAMVALUE);
		node = node.mustChildElement(OW_XMLNode::XML_ELEMENT_IPARAMVALUE);
		node = node.mustChildElement(OW_XMLNode::XML_ELEMENT_INSTANCE);
		OW_CIMInstance inst = OW_XMLCIMFactory::createInstance(node);
		m_callback->indicationOccurred(inst, m_path);
		ostrEntity << "<SIMPLEEXPRSP>\r\n";
		ostrEntity << "<EXPMETHODRESPONSE NAME=\"ExportIndication\">\r\n";

		ostrEntity << "</EXPMETHODRESPONSE>\r\n";

		ostrEntity << "</SIMPLEEXPRSP>\r\n";
	}
	catch(OW_CIMException& ce)
	{
		m_hasError = true;
		outputError(ce, ostrError, messageId);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLListener::outputError(const OW_CIMException& ce, ostream& ostr, const OW_String& messageId)
{
	int errorCode;
	makeXMLHeader(messageId, ostr);

	ostr << "<SIMPLEEXPRSP>\r\n";
	ostr << "<EXPMETHODRESPONSE NAME=\"ExportIndication\">\r\n";

	errorCode = ce.getErrNo();

	ostr << "<ERROR CODE=\"" << errorCode << "\" DESCRIPTION=\"" <<
		OW_XMLEscape(ce.getMessage()) <<
		"\"></ERROR>\r\n";

	ostr << "</EXPMETHODRESPONSE>\r\n";

	ostr << "</SIMPLEEXPRSP>\r\n";
	ostr << "</MESSAGE></CIM>\r\n";
}


//////////////////////////////////////////////////////////////////////////////
void
OW_XMLListener::setPath(const OW_String& id)
{
	m_path = id;
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
