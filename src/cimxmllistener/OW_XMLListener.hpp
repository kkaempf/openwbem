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
 * @author Dan Nuffer
 */

#ifndef OW_XMLLISTENER_HPP_INCLUDE_GUARD_
#define OW_XMLLISTENER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_RequestHandlerIFCXML.hpp"
#include "OW_CIMXMLListenerFwd.hpp"
#include <iosfwd>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OW_LISTENER_API XMLListener : public RequestHandlerIFCXML
{
public:
	/**
	 * Create a new XMLListener.
	 * @param callback a pointer to a CIMListenerCallback object whose
	 * 	method should be called when an indication occurrs.
	 *
	 */
	XMLListener(CIMListenerCallback* callback);
	virtual ~XMLListener();
	virtual RequestHandlerIFC* clone() const;
	virtual StringArray getSupportedContentTypes() const;
	virtual String getContentType() const;

	virtual String getName() const;
	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void shutdown();
protected:
	/**
	 * After doProcess is called, and the XML is parsed, executeXML
	 * is called on the root node.
	 * @param node the root node.
	 * @param ostrEntity the ostream to write to if no errors occurr
	 * @param ostrError the ostream to write to if an error occurred.
	 * @param context the operation context
	 * @return a HTTP status code, I think.
	 */
	virtual int executeXML(CIMXMLParser &parser, std::ostream *ostrEntity,
		std::ostream *ostrError, OperationContext& context);
	/**
	 * Based on a path, what options are available?
	 * @param cf a CIMFeatures object to fill out.
	 * @param context the operation context.
	 */
	virtual void doOptions(CIMFeatures &cf, OperationContext& context);
	virtual void outputError(CIMException::ErrNoType errorCode,
		const String& msg, std::ostream& ostr);
private:
	CIMListenerCallback* m_callback;
	void processSimpleExpReq(CIMXMLParser& parser, std::ostream& ostrEntity,
		std::ostream& ostrError, const String& messageId, OperationContext& context);
};

} // end namespace OW_NAMESPACE

#endif
