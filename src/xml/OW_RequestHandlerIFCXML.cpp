/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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
#include "OW_CIMErrorException.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_ThreadCancelledException.hpp"

using std::istream;
using std::ostream;


//////////////////////////////////////////////////////////////////////////////
void
OW_RequestHandlerIFCXML::doProcess(istream* istr, ostream* ostrEntity,
	ostream* ostrError, const OW_SortedVectorMap<OW_String, OW_String>& handlerVars)
{

	OW_ASSERT(ostrEntity);
	OW_ASSERT(ostrError);

	OW_SortedVectorMap<OW_String, OW_String>::const_iterator i
		= handlerVars.find(OW_ConfigOpts::HTTP_PATH_opt);
	if (i != handlerVars.end())
	{
		setPath((*i).second);
	}
	try
	{
		OW_CIMXMLParser parser(*istr);
		if (!parser)
		{
			OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_well_formed);
		}
		OW_XMLOperationGeneric::XMLGetCIMElement(parser);
		if (!parser)
		{
			OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
		}

		if (!parser.tokenIs(OW_CIMXMLParser::E_MESSAGE))
		{
			OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
		}

		OW_String userName;
		i = handlerVars.find(OW_ConfigOpts::USER_NAME_opt);
		if (i != handlerVars.end())
		{
			userName = (*i).second;
		}

		try
		{
			executeXML(parser, ostrEntity, ostrError, userName);
		}
		catch (OW_CIMException& ce)
		{
			OW_LOGDEBUG(format("OW_RequestHandlerIFCXML::doProcess caught CIM "
				"exception:\nCode: %1\nFile: %2\n Line: %3\nMessage: %4",
				ce.getErrNo(), ce.getFile(), ce.getLine(), ce.getMessage()));

			outputError(ce.getErrNo(), ce.getMessage(), *ostrError);
		}
		catch (OW_CIMErrorException& cee)
		{
			OW_LOGDEBUG(format("OW_RequestHandlerIFCXML::doProcess caught CIMError "
				"exception:File: %1\n Line: %2\nMessage: %3",
				cee.getFile(), cee.getLine(), cee.getMessage()));
			m_cimError = cee.getMessage();
			outputError(OW_CIMException::FAILED, cee.getMessage(), *ostrError);
		}
		catch (std::exception& e)
		{
			OW_LOGDEBUG(format("OW_RequestHandlerIFCXML::doProcess caught std exception: %1"
				, e.what()));
			outputError(OW_CIMException::FAILED, e.what(), *ostrError);
		}
		catch (OW_ThreadCancelledException&)
		{
			OW_LOGDEBUG("OW_RequestHandlerIFCXML::doProcess caught Thread Cancelled exception.");
			outputError(OW_CIMException::FAILED, "thread cancelled", *ostrError);
			throw;
		}
		catch (...)
		{
			OW_LOGDEBUG("OW_RequestHandlerIFCXML::doProcess caught unknown exception");
			outputError(OW_CIMException::FAILED, "Unknown Exception", *ostrError);
		}
		if (hasError())
		{
			(*ostrError) << "</MESSAGE></CIM>\r\n";
		}
	}
	catch (OW_XMLParseException& e)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_well_formed);
	}

}


//////////////////////////////////////////////////////////////////////////////
void
OW_RequestHandlerIFCXML::makeXMLHeader(const OW_String& messageID, ostream& ostr)
{
	ostr << XML_CIM_HEADER1;
	ostr << XML_CIM_HEADER2;
	ostr << "<MESSAGE ID=\"" << messageID << "\" PROTOCOLVERSION=\""
		<< CIM_PROTOCOL_VERSION << "\">";
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
void
OW_RequestHandlerIFCXML::setPath(const OW_String& arg)
{
	m_path = arg;
}


