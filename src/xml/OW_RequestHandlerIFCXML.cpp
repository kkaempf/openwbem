/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "OW_RequestHandlerIFCXML.hpp"
#include "blocxx/Assertion.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "blocxx/Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "blocxx/ThreadCancelledException.hpp"
#include "OW_XMLParseException.hpp"
#include "blocxx/Logger.hpp"
#include "OW_OperationContext.hpp"
#include "blocxx/Array.hpp"

#define LOG_DEBUG(msg) BLOCXX_LOG_DEBUG(logger, msg)
#define LOG_INFO(msg) BLOCXX_LOG_INFO(logger, msg)
#define LOG_ERROR(msg) BLOCXX_LOG_ERROR(logger, msg)
#define LOG_FATAL_ERROR(msg) BLOCXX_LOG_FATAL_ERROR(logger, msg)

namespace OW_NAMESPACE
{

using namespace blocxx;
namespace
{
	const String COMPONENT_NAME("ow.requesthandler.cimxml");
}
using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////
RequestHandlerIFCXML::~RequestHandlerIFCXML()
{
}
//////////////////////////////////////////////////////////////////////////////
void
RequestHandlerIFCXML::doProcess(istream* istr, ostream* ostrEntity,
	ostream* ostrError, OperationContext& context)
{
	BLOCXX_ASSERT(ostrEntity);
	BLOCXX_ASSERT(ostrError);
	try
	{
		setPath(context.getStringData(OperationContext::HTTP_PATH));
	}
	catch (ContextDataNotFoundException& e)
	{
		// not found: okay, setPath() won't be called.
	}

	try
	{
		Logger logger(COMPONENT_NAME);
		CIMXMLParser parser(*istr);
		if (!parser)
		{
			OW_THROW(CIMErrorException, CIMErrorException::request_not_well_formed);
		}
		XMLOperationGeneric::XMLGetCIMElement(parser);
		if (!parser)
		{
			OW_THROW(CIMErrorException, CIMErrorException::request_not_loosely_valid);
		}
		if (!parser.tokenIsId(CIMXMLParser::E_MESSAGE))
		{
			OW_THROW(CIMErrorException, CIMErrorException::request_not_loosely_valid);
		}
		try
		{
			executeXML(parser, ostrEntity, ostrError, context);
		}
		catch (CIMException& ce)
		{
			LOG_INFO(Format("RequestHandlerIFCXML::doProcess caught CIM "
				"exception:\nCode: %1\nFile: %2\n Line: %3\nMessage: %4",
				ce.getErrNo(), ce.getFile(), ce.getLine(), ce.getMessage()));
			outputError(ce.getErrNo(), ce.getDescription(), *ostrError);
		}
		catch (CIMErrorException& cee)
		{
			LOG_INFO(Format("RequestHandlerIFCXML::doProcess caught CIMError "
				"exception:File: %1\n Line: %2\nMessage: %3",
				cee.getFile(), cee.getLine(), cee.getMessage()));
			m_cimError = cee.getMessage();
			outputError(CIMException::FAILED, cee.getMessage(), *ostrError);
		}
		catch (Exception& e)
		{
			LOG_INFO(Format("RequestHandlerIFCXML::doProcess caught "
				"Exception:%1",	e));
			m_cimError = e.getMessage();
			outputError(CIMException::FAILED, e.getMessage(), *ostrError);
		}
		catch (std::exception& e)
		{
			LOG_ERROR(Format("RequestHandlerIFCXML::doProcess caught std exception: %1"
				, e.what()));
			outputError(CIMException::FAILED, e.what(), *ostrError);
		}
		catch (ThreadCancelledException&)
		{
			LOG_DEBUG("RequestHandlerIFCXML::doProcess caught Thread Cancelled exception.");
			outputError(CIMException::FAILED, "thread cancelled", *ostrError);
			throw;
		}
		catch (...)
		{
			LOG_ERROR("RequestHandlerIFCXML::doProcess caught unknown exception");
			outputError(CIMException::FAILED, "Unknown Exception", *ostrError);
		}
		if (hasError())
		{
			(*ostrError) << "</MESSAGE></CIM>\r\n";
		}
	}
	catch (XMLParseException& e)
	{
		OW_THROW(CIMErrorException, CIMErrorException::request_not_well_formed);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
RequestHandlerIFCXML::makeXMLHeader(const String& messageID, ostream& ostr)
{
	ostr << XML_CIM_HEADER1;
	ostr << XML_CIM_HEADER2;
	ostr << "<MESSAGE ID=\"" << messageID << "\" PROTOCOLVERSION=\""
		<< CIM_PROTOCOL_VERSION << "\">";
}
//////////////////////////////////////////////////////////////////////////////
StringArray
RequestHandlerIFCXML::getSupportedContentTypes() const
{
	StringArray rval;
	rval.push_back("text/xml");
	rval.push_back("application/xml");
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
String
RequestHandlerIFCXML::getContentType() const
{
	return String("application/xml");
}
//////////////////////////////////////////////////////////////////////////////
void
RequestHandlerIFCXML::setPath(const String& arg)
{
	m_path = arg;
}

} // end namespace OW_NAMESPACE

