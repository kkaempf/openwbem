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
#include "OW_IndicationExporter.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_Format.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMException.hpp"

namespace OW_NAMESPACE
{

// We always send the lowest possible version. If 1.0 and 1.1 are the same, we send 1.0 so that 1.0 only clients will accept the request.
// If we're using a 1.1 only feature, then we have to send 1.1.
namespace
{
const String PROTOCOL_VERSION_1_1("1.1");
}

using std::ostream;
using std::istream;
using std::iostream;
IndicationExporter::IndicationExporter( CIMProtocolIFCRef prot )
	: m_protocol(prot), m_iMessageID(0)
{
	m_protocol->setContentType("application/xml");
}
void
IndicationExporter::exportIndication( const String& ns, const CIMInstance& ci )
{
	static const char* const commandName = "ExportIndication";
	Array<Param> params;
	
	Reference<TempFileStream> iostr(new TempFileStream);
	sendXMLHeader(*iostr, PROTOCOL_VERSION_1_1);
	*iostr << "<EXPPARAMVALUE NAME=\"NewIndication\">";
	CIMInstancetoXML(ci, *iostr);
	*iostr << "</EXPPARAMVALUE>";
	sendXMLTrailer(*iostr);
	doSendRequest(iostr, commandName, ns, PROTOCOL_VERSION_1_1);
}
void
IndicationExporter::sendXMLHeader(ostream& ostr, const String& cimProtocolVersion)
{
	// TODO: merge this with the code in CIMXMLCIMOMHandle.cpp
	// TODO: WRT the versions, have a way of doing a fallback to older
	// versions for the sake of compatibility.
	if (++m_iMessageID > 65535)
	{
		m_iMessageID = 1;
	}
	ostr << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
	ostr << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">";
	ostr << "<MESSAGE ID=\"" << m_iMessageID << "\" PROTOCOLVERSION=\"" << cimProtocolVersion << "\">";
	ostr << "<SIMPLEEXPREQ>";
	ostr << "<EXPMETHODCALL NAME=\"ExportIndication\">";
}
void
IndicationExporter::sendXMLTrailer(ostream& ostr)
{
	ostr << "</EXPMETHODCALL>";
	ostr << "</SIMPLEEXPREQ>";
	ostr << "</MESSAGE>";
	ostr << "</CIM>";
	ostr << "\r\n";
}
	
void
IndicationExporter::doSendRequest(Reference<iostream> ostr, const String& methodName,
		const String& ns, const String& cimProtocolVersion)
{
	Reference<std::istream> istr = m_protocol->endRequest(ostr, methodName,
		ns, CIMProtocolIFC::E_CIM_EXPORT_REQUEST, cimProtocolVersion);
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
	return checkNodeForCIMError(parser, methodName);
}
void
IndicationExporter::checkNodeForCIMError(CIMXMLParser& parser,
		const String& operation)
{
// TODO: This code is the same as in CIMXMLCIMOMHandle.cpp.  Put it in a
// common spot.
	//
	// Check for <CIM> element
	//
	if (!parser || !parser.tokenIsId(CIMXMLParser::E_CIM))
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Invalid XML");
	}
	String cimattr;
// TODO: Decide if we really should check this or not.
#if 0
	cimattr = parser.mustGetAttribute(CIMXMLParser::A_CIMVERSION);
	if (!cimattr.equals(CIMXMLParser::AV_CIMVERSION_VALUE))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							String("Return is for CIMVERSION " + cimattr).c_str());
	}
	cimattr = parser.mustGetAttribute(CIMXMLParser::A_DTDVERSION);
	if (!cimattr.equals(CIMXMLParser::AV_DTDVERSION_VALUE))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							String("Return is for DTDVERSION " + cimattr).c_str());
	}
#endif
	//
	// Find <MESSAGE>
	//
	parser.mustGetChildId(CIMXMLParser::E_MESSAGE);
	cimattr=parser.mustGetAttribute(CIMXMLParser::A_ID);
	if (!cimattr.equals(String(m_iMessageID)))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							String("Return messageid="+cimattr+", expected="
										 +String(m_iMessageID)).c_str());
	}
// TODO: Decide if we really should check this or not.
#if 0
	cimattr = parser.mustGetAttribute(CIMXMLParser::A_PROTOCOLVERSION);
	if (!cimattr.equals(CIMXMLParser::AV_PROTOCOLVERSION_VALUE))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							String("Return is for PROTOCOLVERSION "+cimattr).c_str());
	}
#endif
	// Find <SIMPLEEXPRSP>
	//
	// TODO-NICE: need to look for complex EXPRSPs!!
	//
	parser.mustGetChildId(CIMXMLParser::E_SIMPLEEXPRSP);
	//
	// <EXPMETHODRESPONSE>
	//
	parser.mustGetChildId(CIMXMLParser::E_EXPMETHODRESPONSE);
	String nameOfMethod = parser.getAttribute(CIMXMLParser::A_NAME);
	if (nameOfMethod.empty())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							"Response had no method name");
	}
	if (!nameOfMethod.equalsIgnoreCase(operation))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							String("Called "+operation+" but response was for "+
										 nameOfMethod).c_str());
	}
	parser.mustGetNextTag();
	if (parser.tokenIsId(CIMXMLParser::E_ERROR))
	{
		String errCode = parser.mustGetAttribute(
			CIMXMLParser::A_CODE);
		String description = parser.getAttribute(
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
}
	

} // end namespace OW_NAMESPACE

