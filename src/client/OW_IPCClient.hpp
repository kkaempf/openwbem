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

#ifndef OW_IPCCLIENT_HPP_
#define OW_IPCCLIENT_HPP_

#include "OW_config.h"
#include "OW_Bool.hpp"
#include "OW_CIMProtocol.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_IPCConnection.hpp"

class OW_IPCClient : public OW_CIMProtocol
{
	public:
		/**
		 * Creates an httpClient object.  This represents one client
		 * connection to a CIMOM.
		 * @param url The URL to the CIMOM the client should connect to.
		 * 		URLs have this form:
		 * 		http[s]://[user:passwd@]hostname[:port][/path]	
		 */
		OW_IPCClient( const OW_String& url);
		virtual ~OW_IPCClient();

		virtual OW_Reference<std::iostream> getStream();
		/**
		 * Establishes a connection (if not already connected) to the
		 * CIMOM and sends an http request.  HTTP headers are added,
		 * appropriate for the request.  An istream& is returned containing
		 * the response from the CIMOM, after http header processing is done.
		 * Adds http headers/etc. to request and sends to server.
		 * returns the response (with http stuff stripped off
		 * @param request An istream& containing the request to be send to
		 * 	the CIMOM.
		 * @param methodName The CIM method that corresponds to the request.
		 * @nameSpace the namespace the request applies to.
		 * @return an istream& containing the response from the server,
		 * 	once the headers have been processed.
		 * @exception OW_HTTPException
		 * @exception OW_SocketException
		 *
		 */
		virtual std::istream& sendRequest(std::istream& request,
				const OW_String& methodName,
				const OW_String& nameSpace);

		/**
		 * Sends an OPTIONS request to the HTTP server, and reports the
		 * results.
		 * @return a OW_CIMFeatures object listing the features of the CIMOM.
		 */
		virtual OW_CIMFeatures getFeatures();


		void setLoginCallBack(OW_ClientAuthCBIFCRef loginCB)
			{ m_loginCB = loginCB; }

		OW_Bool authenticate(const OW_String& userName,
			const OW_String& info, OW_String& details, OW_Bool throwIfFailure);

	private:

		OW_IPCConnection m_conn;
		OW_String m_url;
		OW_Bool m_authenticated;
		OW_ClientAuthCBIFCRef m_loginCB;
		
		void authenticateIfNeeded();
		void checkError();

        // unimplemented
		OW_IPCClient(const OW_IPCClient&);
		OW_IPCClient& operator=(const OW_IPCClient&);
};


#endif

