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
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_Format.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMException.hpp"

using std::ostream;
using std::istream;
using std::iostream;

OW_IndicationExporter::OW_IndicationExporter( OW_CIMProtocolIFCRef prot )
	: m_protocol(prot), m_iMessageID(0)
{
	m_protocol->setContentType("application/xml");
}

void
OW_IndicationExporter::exportIndication( const OW_String& ns, const OW_CIMInstance& ci )
{
	static const char* const commandName = "ExportIndication";
	OW_Array<OW_Param> params;
	
	OW_Reference<OW_TempFileStream> iostr(new OW_TempFileStream);
	sendXMLHeader(*iostr);
	*iostr << "<EXPPARAMVALUE NAME=\"NewIndication\">";
	OW_CIMtoXML(ci, *iostr, OW_CIMObjectPath(),
		OW_CIMtoXMLFlags::isNotInstanceName,
		OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers,
		OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray());
	*iostr << "</EXPPARAMVALUE>";
	sendXMLTrailer(*iostr);
	doSendRequest(iostr, commandName, ns);
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
	
void
OW_IndicationExporter::doSendRequest(OW_Reference<iostream> ostr, const OW_String& methodName,
		const OW_String& ns)
{
	OW_CIMProtocolIStreamIFCRef istr = m_protocol->endRequest(ostr, methodName,
		ns);

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

	return checkNodeForCIMError(parser, methodName);
}

void
OW_IndicationExporter::checkNodeForCIMError(OW_CIMXMLParser& parser,
		const OW_String& operation)
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
	cimattr=parser.mustGetAttribute(OW_CIMXMLParser::A_MSG_ID);
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

	// Find <SIMPLEEXPRSP>
	//
	// TODO-NICE: need to look for complex EXPRSPs!!
	//
	parser.mustGetChild(OW_CIMXMLParser::E_SIMPLEEXPRSP);

	//
	// <EXPMETHODRESPONSE>
	//
	parser.mustGetChild(OW_CIMXMLParser::E_EXPMETHODRESPONSE);

	OW_String nameOfMethod = parser.getAttribute(OW_CIMXMLParser::A_NAME);
	if (nameOfMethod.empty())
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

	parser.mustGetNextTag();
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
}
	

