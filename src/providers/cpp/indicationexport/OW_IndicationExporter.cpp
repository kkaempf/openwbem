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
#include "OW_IndicationExporter.hpp"
#include "OW_XMLParser.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_Format.hpp"

using std::ostream;
using std::istream;
using std::iostream;

OW_IndicationExporter::OW_IndicationExporter( OW_CIMProtocolIFCRef prot )
	: m_protocol(prot), m_iMessageID(0)
{
	m_protocol->setContentType("application/xml");
}

void
OW_IndicationExporter::exportIndication( const OW_CIMInstance& ci )
{
	OW_CIMObjectPath path(ci.getName(), ci.getKeyValuePairs());
	static const char* const commandName = "ExportIndication";
	OW_Array<OW_Param> params;
	
	OW_Reference<OW_TempFileStream> iostr(new OW_TempFileStream);
	sendXMLHeader(*iostr);
	*iostr << "<IPARAMVALUE NAME=\"NewIndication\">";
//	*iostr << "<EXPPARAMVALUE NAME=\"NewIndication\">";
	OW_CIMtoXML(ci, *iostr, OW_CIMObjectPath(),
		OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers,
		OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray());
	*iostr << "</IPARAMVALUE>";
//	*iostr << "</EXPPARAMVALUE>";
	sendXMLTrailer(*iostr);
	doSendRequest(iostr, commandName, path);
}

void
OW_IndicationExporter::sendXMLHeader(ostream& ostr)
{
	if (++m_iMessageID > 65535)
	{
		m_iMessageID = 1;
	}
	ostr << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
	ostr << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">";
	ostr << "<MESSAGE ID=\"" << m_iMessageID << "\" PROTOCOLVERSION=\"1.0\">";
	ostr << "<SIMPLEEXPREQ>";
	ostr << "<EXPMETHODCALL NAME=\"ExportIndication\">";
}

void
OW_IndicationExporter::sendXMLTrailer(ostream& ostr)
{
	ostr << "</EXPMETHODCALL>";
	ostr << "</SIMPLEEXPREQ>";
	ostr << "</MESSAGE>";
	ostr << "</CIM>";
	ostr << "\r\n";
}
	
OW_XMLNode
OW_IndicationExporter::doSendRequest(OW_Reference<iostream> ostr, const OW_String& methodName,
		const OW_CIMObjectPath& path)
{
	OW_CIMProtocolIStreamIFCRef istr = m_protocol->endRequest(ostr, methodName,
		path.getNameSpace());

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

	OW_XMLParser parser(istr.getPtr());

	OW_XMLNode retval = parser.parse();
	return checkNodeForCIMError(retval, methodName);
}

OW_XMLNode
OW_IndicationExporter::checkNodeForCIMError(OW_XMLNode reply,
		const OW_String& operation)
{
	//
	// Find <CIM> element
	//
	reply = reply.mustFindElement(OW_XMLNode::XML_ELEMENT_CIM);
	OW_String cimattr = reply.mustGetAttribute(OW_XMLOperationGeneric::CIMVERSION);
	if (!cimattr.equals(OW_XMLOperationGeneric::CIMVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for CIMVERSION " + cimattr).c_str());
	}

	cimattr = reply.mustGetAttribute(OW_XMLOperationGeneric::DTDVERSION);
	if (!cimattr.equals(OW_XMLOperationGeneric::DTDVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for DTDVERSION " + cimattr).c_str());
	}

	//
	// Find <MESSAGE>
	//
	reply = reply.mustChildFindElement(OW_XMLNode::XML_ELEMENT_MESSAGE);
	cimattr=reply.mustGetAttribute(OW_XMLOperationGeneric::MSG_ID);
	if (!cimattr.equals(OW_String(m_iMessageID)))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return messageid="+cimattr+", expected="
										 +OW_String(m_iMessageID)).c_str());
	}

	cimattr = reply.mustGetAttribute(OW_XMLOperationGeneric::PROTOCOLVERSION);
	if (!cimattr.equals(OW_XMLOperationGeneric::PROTOCOLVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for PROTOCOLVERSION "+cimattr).c_str());
	}

	reply = reply.getChild();
	if (!reply)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"No XML_ELEMENT_IRETURNVALUE (22)");
	}

	//
	// Find <SIMPLEEXPRSP>
	//
	reply = reply.findElementChild(OW_XMLNode::XML_ELEMENT_SIMPLEEXPRSP);

	//
	// TODO-NICE: need to look for complex EXPRSPs!!
	//
	if (!reply)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"No XML_ELEMENT_SIMPLEEXPRSP");
	}

	//
	// <EXPMETHODRESPONSE>
	//
	OW_XMLNode tempReply =
		reply.findElement(OW_XMLNode::XML_ELEMENT_EXPMETHODRESPONSE);

	OW_String nameOfMethod = reply.getAttribute(OW_XMLAttribute::NAME);
	if (nameOfMethod.length() < 1)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"Response had no method name");
	}

	if (!nameOfMethod.equalsIgnoreCase(operation))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Called "+operation+" but response was for "+
										 nameOfMethod).c_str());
	}

	reply = reply.getChild();
	if (reply)
	{
		//
		// See if there was an error, and if there was throw an equivalent
		// exception on the client
		//
		OW_XMLNode errorNode = reply.findElement(OW_XMLNode::XML_ELEMENT_ERROR);
		if (errorNode)
		{
			OW_String errCode=errorNode.mustGetAttribute(
				OW_XMLParameters::paramErrorCode);
			OW_String description=errorNode.mustGetAttribute(
				OW_XMLParameters::paramErrorDescription);
			OW_Int32 iErrCode;
			try
			{
				iErrCode = errCode.toInt32();
			}
			catch (const OW_StringConversionException& e)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					format("Invalid XML error code. %1", e.getMessage()).c_str());
			}
			OW_THROWCIMMSG(OW_CIMException::ErrNoType(errCode.toInt32()),
				description.c_str());
		}
	}


	return reply;
}
	

