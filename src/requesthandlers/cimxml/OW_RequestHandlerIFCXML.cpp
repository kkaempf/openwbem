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
#include "OW_RequestHandlerIFCXML.hpp"
#include "OW_XMLParser.hpp"
#include "OW_Assertion.hpp"
#include "OW_XMLException.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFCXML::OW_RequestHandlerIFCXML()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RequestHandlerIFCXML::doProcess(istream* istr, ostream* ostrEntity,
	ostream* ostrError, const OW_SortedVector<OW_String, OW_String>& handlerVars)
{

	OW_ASSERT(ostrEntity);
	OW_ASSERT(ostrError);

	OW_XMLNode node;
	try
	{
		OW_XMLParser parser(istr);
		node = parser.parse();
		if (!node)
		{
			OW_THROW(OW_XMLException, "Invalid XML node returned from parser");
		}
	}
	catch (OW_XMLException& e)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_valid);
		// TODO outputerror here.
	}

	node = OW_XMLOperationGeneric::XMLGetCIMElement(node);
	if (!node)
	{
		OW_THROW(OW_CIMErrorException, "failed to find <CIM> tag");
	}

	// TODO if debugXML log "cim element obtained"
	node = node.mustFindElement(OW_XMLNode::XML_ELEMENT_MESSAGE);
	if (!node)
	{
		OW_THROW(OW_CIMErrorException, "failed to find <MESSAGE> tag");
	}

	OW_String userName;
	OW_SortedVector<OW_String, OW_String>::const_iterator i = handlerVars.find(OW_ConfigOpts::USER_NAME_opt);
	if (i != handlerVars.end())
	{
		userName = (*i).second;
	}

	executeXML(node, ostrEntity, ostrError, userName);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_RequestHandlerIFCXML::makeXMLHeader(const OW_String& messageID, ostream& ostr)
{
	ostr << XML_CIM_HEADER1 << "\r\n";
	ostr << XML_CIM_HEADER2 << "\r\n";
	ostr << "<MESSAGE ID=\"" << messageID << "\" PROTOCOLVERSION=\""
		<< CIM_PROTOCOL_VERSION << "\">" << "\r\n";
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_RequestHandlerIFCXML::getSupportedContentTypes() const
{
	OW_StringArray rval;
	rval.push_back("text/xml");
	rval.push_back("application/xml");
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_RequestHandlerIFCXML::getContentType() const
{
	return OW_String("application/xml");
}

//////////////////////////////////////////////////////////////////////////////
