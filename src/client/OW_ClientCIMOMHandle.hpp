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

#ifndef OW_CLIENTCIMOMHANDLE_HPP_
#define OW_CLIENTCIMOMHANDLE_HPP_
#include "OW_config.h"										
#include "OW_IfcsFwd.hpp"
#include "OW_ClientFwd.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_Map.hpp"

namespace OpenWBEM
{

// This class is meant to hold common functionality in the client-side CIMOM
// handles	
class OW_CLIENT_API ClientCIMOMHandle : public CIMOMHandleIFC
{
public:
	ClientCIMOMHandle() : CIMOMHandleIFC(), m_trailers() {}

#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	/** This method is deprecated.  Use CIMNameSpaceUtils::createCIM_Namespace() instead. */
	virtual void createNameSpace(const String& ns) OW_DEPRECATED; // in 3.0.0
	/** This method is deprecated.  Use CIMNameSpaceUtils::deleteCIM_Namespace() instead. */
	virtual void deleteNameSpace(const String& ns) OW_DEPRECATED; // in 3.0.0
#endif
	/** This method is deprecated.  Use CIMNameSpaceUtils::createCIM_Namespace() instead. */
	virtual void enumNameSpace(const String& ns,
		StringResultHandlerIFC &result, WBEMFlags::EDeepFlag deep) OW_DEPRECATED; // in 3.0.0

	/**
	 * Get a Reference to the WBEM protocol handler (HTTPClient)
	 */
	virtual CIMProtocolIFCRef getWBEMProtocolHandler() const = 0;

	/**
	 * Set/Add an HTTP header and its associated value. This header will be
	 * sent to the CIMOM on every request.
	 * @param hdrName The name of the HTTP Header (e.g. "Accept-Language")
	 * @param hdrValue The value of the HTTP Header (e.g. "en-US, en")
	 * @return true if successful. Otherwise false.
	 */
	virtual bool setHTTPRequestHeader(const String& hdrName,
		const String& hdrValue) = 0;

	/**
	 * Get the value of an HTTP header that was returned in the CIMOM's
	 * response.
	 * @param hdrName The of the HTTP Header value to retrieve.
	 * 		(e.g. "Content-Language")
	 * @param valueOut An output param that will hold the value of the header
	 * 		on return.
	 * @return true if the header exists. Otherwise false.
	 */
	virtual bool getHTTPResponseHeader(const String& hdrName,
		String& valueOut) const = 0;

	/**
	 * Factory function.  Parses url and creates either a CIMXMLCIMOMHandle or
	 * a BinaryCIMOMHandle along with a HTTPClient.
	 *
	 * @param url If the url begins with "owbinary"
	 * (e.g. owbinary.wbem://test1:pass1@localhost:30926/), then the openwbem
	 * binary protocol will be used.  Otherwise CIM/XML is the default.
	 * If the url's port is an integer, TCP will be used.  If the url's port is
	 * owipc, then a local ipc connection will be attempted to the predefined
	 * (OW_DOMAIN_SOCKET_NAME) domain socket.  If the port is anything else,
	 * it will be used as the identifier for the ipc mechanism.  On Unix this
	 * is the filename of the domain socket.  Note that to represent a filename
	 * in the port, the url escape mechanism must be used, since a / (among
	 * other chars) isn't allowed in the port.
	 *
	 * @param authCb If authentication is necessary, and authCb != NULL, then
	 * authCb->getCredentials() will be called to obtain credentials.
	 *
	 * @return a ClientCIMOMHandleRef suitable for connecting to the given url.
	 *
	 * @throws MalformedURLException If the url is bad
	 * @throws std::bad_alloc
	 * @throws SocketException If an SSL connection was requested, but support for SSL is not available.
	 */
	static ClientCIMOMHandleRef createFromURL(const String& url, const ClientAuthCBIFCRef& authCb = ClientAuthCBIFCRef());

protected:
	void getHTTPTrailers(const CIMProtocolIStreamIFCRef& istr);

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	Map<String, String> m_trailers;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

};

} // end namespace OpenWBEM

#endif
	
