/*******************************************************************************
* Copyright (C) 2001-2008 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
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
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_IndicationExporter.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstance.hpp"
#include "blocxx/TempFileStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "blocxx/Format.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_HTTPClient.hpp"
#include "blocxx/Thread.hpp"

namespace OW_NAMESPACE
{

// We always send the lowest possible version. If 1.0 and 1.1 are the same, we send 1.0 so that 1.0 only clients will accept the request.
// If we're using a 1.1 only feature, then we have to send 1.1.
namespace
{
const String PROTOCOL_VERSION_1_1("1.1");
static const char* const commandName = "ExportIndication";
}

using std::ostream;
using std::istream;
using std::iostream;

IndicationExporter::~IndicationExporter()
{
}

IndicationExporterImpl::~IndicationExporterImpl()
{
}

IndicationExporterImpl::IndicationExporterImpl( CIMProtocolIFCRef prot )
	: m_protocol(prot), m_iMessageID(0)
{
	m_protocol->setContentType("application/xml");
}

void
IndicationExporterImpl::beginExport()
{
	m_ostrm = m_protocol->beginRequest(commandName, "");
	sendXMLHeader(PROTOCOL_VERSION_1_1);
	*m_ostrm << "<MULTIEXPREQ>";
}

void
IndicationExporterImpl::endExport()
{
	*m_ostrm << "</MULTIEXPREQ>";
	sendXMLTrailer();
	doSendRequest(commandName, PROTOCOL_VERSION_1_1);
}

void
IndicationExporterImpl::sendIndication(CIMInstance const & ci)
{
	Thread::testCancel();
	ostream & ostrm = *m_ostrm;
	ostrm << "<SIMPLEEXPREQ>"
	         "<EXPMETHODCALL NAME=\"ExportIndication\">"
	         "<EXPPARAMVALUE NAME=\"NewIndication\">";
	CIMInstancetoXML(ci, ostrm);
	ostrm << "</EXPPARAMVALUE>"
	         "</EXPMETHODCALL>" 
	         "</SIMPLEEXPREQ>";
}

void
IndicationExporterImpl::sendXMLHeader(const String& cimProtocolVersion)
{
	ostream & ostr = *m_ostrm;
	// TODO: merge this with the code in CIMXMLCIMOMHandle.cpp
	// TODO: WRT the versions, have a way of doing a fallback to older
	// versions for the sake of compatibility.
	if (++m_iMessageID > 65535)
	{
		m_iMessageID = 1;
	}
	ostr << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" 
	        "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">" 
	        "<MESSAGE ID=\"" << m_iMessageID << "\" PROTOCOLVERSION=\"" << cimProtocolVersion << "\">";
}

void
IndicationExporterImpl::sendXMLTrailer()
{
	ostream & ostr = *m_ostrm;
	ostr << "</MESSAGE>" 
	        "</CIM>" 
	        "\r\n";
}
	
void
IndicationExporterImpl::doSendRequest(
	const String& methodName, const String& cimProtocolVersion)
{
	Reference<ostream> ostr = m_ostrm;
	Reference<std::istream> istr = m_protocol->endRequest(ostr, methodName,
		"", CIMProtocolIFC::E_CIM_BATCH_EXPORT_REQUEST, cimProtocolVersion);
	CIMXMLParser parser(*istr);
	return checkNodeForCIMError(parser, methodName);
}

void
IndicationExporterImpl::checkNodeForCIMError(CIMXMLParser& parser,
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

namespace
{

HTTPClientRef createClient(const CIMInstance & indHandlerInst)
{
	String listenerUrl;
	indHandlerInst.getProperty("Destination").getValue().get(listenerUrl);

	// this guy parses it out.
	URL url(listenerUrl);

	if (indHandlerInst.getClassName().equalsIgnoreCase("CIM_IndicationHandlerXMLHTTPS"))
	{
		if (!url.scheme.equals(URL::CIMXML_WBEMS))
		{
			url.scheme = URL::CIMXML_WBEMS;
			listenerUrl = url.toString();
		}
	}

	HTTPClientRef httpClient = new HTTPClient(listenerUrl);

	// the OW 2.0 HTTPXMLCIMListener uses the HTTP path to differentiate different
	// subscriptions.  This is stored in namespace name of the URL.
	if (!url.namespaceName.empty())
	{
		httpClient->setHTTPPath('/' + url.namespaceName);
	}

	return httpClient;
}

} // anon namespace

IndicationExporterRef
IndicationExporterImpl::create(const CIMInstance & indHandlerInst)
{
	HTTPClientRef httpClient(createClient(indHandlerInst));
	return IndicationExporterRef(new IndicationExporterImpl(httpClient));
}

} // end namespace OW_NAMESPACE

