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

#ifndef OW_XMLLISTENER_HPP_INCLUDE_GUARD_
#define OW_XMLLISTENER_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_RequestHandlerIFCXML.hpp"
#include <iosfwd>

class OW_CIMListenerCallback;

class OW_XMLListener : public OW_RequestHandlerIFCXML
{
public:

	/**
	 * Create a new OW_XMLListener.
	 * @param callback a pointer to a OW_CIMListenerCallback object whose
	 * 	method should be called when an indication occurrs.
	 *
	 */
	OW_XMLListener(OW_CIMListenerCallback* callback);
	virtual ~OW_XMLListener() {}

	virtual OW_RequestHandlerIFC* clone() const
	{
		return new OW_XMLListener(*this);
	}

	virtual OW_StringArray getSupportedContentTypes() const;

	virtual OW_String getContentType() const;

protected:


	/**
	 * After doProcess is called, and the XML is parsed, executeXML
	 * is called on the root node.
	 * @param node the root node.
	 * @param ostrEntity the ostream to write to if no errors occurr
	 * @param ostrError the ostream to write to if an error occurred.
	 * @param userName the user performing the action
	 * @return a HTTP status code, I think.
	 */
	virtual int executeXML(OW_CIMXMLParser &parser, std::ostream *ostrEntity,
		std::ostream *ostrError, const OW_String& userName);


	/**
	 * Based on a path, what options are available?
	 * @param cf a OW_CIMFeatures object to fill out.
	 * @param path the path of inquiry.
	 */
	virtual void doOptions(OW_CIMFeatures &cf, const OW_SortedVectorMap<OW_String, OW_String>& handlerVars);

	virtual void outputError(OW_CIMException::ErrNoType errorCode,
		OW_String msg, std::ostream& ostr);
private:
	OW_CIMListenerCallback* m_callback;

	void processSimpleExpReq(OW_CIMXMLParser& parser, std::ostream& ostrEntity,
		std::ostream& ostrError, const OW_String& messageId);
};


#endif


