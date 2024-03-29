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
 * @author Dan Nuffer
 */

#ifndef OW_CLIENTCIMOMHANDLE_HPP_
#define OW_CLIENTCIMOMHANDLE_HPP_
#include "OW_config.h"
#include "OW_IfcsFwd.hpp"
#include "OW_ClientFwd.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "blocxx/Map.hpp"
#include "OW_ClientFwd.hpp"
#include "blocxx/SSLCtxMgr.hpp"

namespace OW_NAMESPACE
{

// This class is meant to hold common functionality in the client-side CIMOM
// handles
class OW_CLIENT_API ClientCIMOMHandle : public CIMOMHandleIFC
{
public:

	virtual ~ClientCIMOMHandle();

	/**
	 * Get a Reference to the WBEM protocol handler (HTTPClient)
	 */
	virtual CIMProtocolIFCRef getWBEMProtocolHandler() const = 0;

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
	 * @param sslCtx A SSLClientCtx can optionally be provided.  This is useful
	 * in the case where a client SSL certificate is used for client SSL
	 * authentication.  It is not necessary to provide this parameter to use
	 * SSL.  If it is not provided, a default SSLClientCtx will be used.
	 *
	 * @return a ClientCIMOMHandleRef suitable for connecting to the given url.
	 *
	 * @throws MalformedURLException If the url is bad
	 * @throws std::bad_alloc
	 * @throws SocketException If an SSL connection was requested, but support for SSL is not available.
	 */
	static ClientCIMOMHandleRef createFromURL(const blocxx::String& url,
											  const ClientAuthCBIFCRef& authCb = ClientAuthCBIFCRef(),
											  const blocxx::SSLClientCtxRef& sslCtx = blocxx::SSLClientCtxRef());

};

} // end namespace OW_NAMESPACE

#endif
