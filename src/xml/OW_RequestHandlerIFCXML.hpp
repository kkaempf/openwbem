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
#ifndef OW_REQUESTHANDLERIFCXML_HPP_INCLUDE_GUARD_
#define OW_REQUESTHANDLERIFCXML_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMException.hpp"
#include <iosfwd>

namespace OpenWBEM
{

const char *const CIM_PROTOCOL_VERSION = "1.0";
const char *const XML_CIM_HEADER1 = "<?xml version=\"1.0\" ?>";
const char *const XML_CIM_HEADER2 = "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">";
#define OW_LOGDEBUG(msg) this->getEnvironment()->getLogger()->logDebug(msg)
#define OW_LOGCUSTINFO(msg) this->getEnvironment()->getLogger()->logInfo(msg)
#define OW_LOGERROR(msg) this->getEnvironment()->getLogger()->logError(msg)
#define OW_LOGFATALERROR(msg) this->getEnvironment()->getLogger()->logFatalError(msg)
class RequestHandlerIFCXML : public RequestHandlerIFC
{
public:
	RequestHandlerIFCXML(): m_path() {}
	virtual ~RequestHandlerIFCXML()
	{
	}
	virtual StringArray getSupportedContentTypes() const;
	virtual String getContentType() const;
protected:
	/**
	 * Process the input stream and generate appropriate output.
	 * @param istr the istream containing the input.
	 * @param ostrEntity write to this ostream if no errors occur.
	 * @param ostrError write to this ostream if an error occurs.
	 * @param userName the name of the user performing the action.
	 */
	virtual void doProcess(std::istream *istr, std::ostream *ostrEntity,
		std::ostream *ostrError, const SortedVectorMap<String, String>& handlerVars);
	/**
	 * After doProcess is called, and the XML is parsed, executeXML
	 * is called on the root node.
	 * @param node the root node.
	 * @param ostrEntity the ostream to write to if no errors occurr
	 * @param ostrError the ostream to write to if an error occurred.
	 * @param userName the user performing the action
	 * @return a HTTP status code, I think.
	 */
	virtual int executeXML(CIMXMLParser& parser, std::ostream* ostrEntity,
		std::ostream* ostrError, const String& userName) = 0;
	/**
	 * Write a XML header based on the messageId, and write it to ostr
	 * @param messageId the message ID
	 * @param ostr the ostream to write the header to.
	 */
	void makeXMLHeader(const String& messageId, std::ostream& ostr);
	void setPath(const String& id);
	virtual void outputError(CIMException::ErrNoType errorCode,
		String, std::ostream& ostr) = 0;
	String m_path;
};

} // end namespace OpenWBEM

typedef OpenWBEM::RequestHandlerIFCXML OW_RequestHandlerIFCXML;

#endif
