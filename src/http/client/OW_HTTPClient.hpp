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

#ifndef OW_HTTPCLIENT_HPP_
#define OW_HTTPCLIENT_HPP_

#include "OW_config.h"
#include "OW_InetSocket.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Map.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_Bool.hpp"
#include "OW_CIMProtocol.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_URL.hpp"


class OW_TempFileStream;

class OW_HTTPClient : public OW_CIMProtocol
{
	public:
		/**
		 * Creates an httpClient object.  This represents one client
		 * connection to a CIMOM.
		 * @param url The URL to the CIMOM the client should connect to.
		 * 		URLs have this form:
		 * 		http[s]://[user:passwd@]hostname[:port][/path]	
		 */
		OW_HTTPClient( const OW_String& url);
		virtual ~OW_HTTPClient();

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


		/**
		 * Gets the address of the local machine
		 * @return An OW_InetAddress corresponding to the local machine.
		 */
		OW_InetAddress getLocalAddress() const;

		/**
		 * Gets the address of the peer connection
		 * @return An OW_InetAddress corresponding to the peer connection
		 */
		OW_InetAddress getPeerAddress()  const;

		/**
		 * Assigns a login callback object.
		 * @param loginCB A OW_Reference to a OW_ClientAuthCB object
		 * 	containing the callback method.
		 */
		void setLoginCallBack(OW_ClientAuthCBIFCRef loginCB)
			{ m_loginCB = loginCB; }


	private:

		void setUrl();
		void cleanUpIStreams();
		void receiveAuthentication();
		void sendAuthorization();
		OW_Bool receiveOptions( void );
		
		OW_String m_sAuthorization;
		

		OW_String m_sRealm;
		OW_String m_sDigestNonce;
		OW_String m_sDigestCNonce;
		OW_UInt8 m_iDigestNonceCount;
		OW_String m_sDigestSessionKey;
		OW_String m_sDigestResponse;

		// new stuff

		typedef enum Resp_t
		{
			FATAL,
			RETRY,
			GOOD,
			CONTINUE
		};

		OW_InetAddress m_serverAddress;
		OW_URL m_url;
		OW_HTTPHeaderMap m_responseHeaders;
		OW_Array<OW_String> m_requestHeadersCommon;
		OW_Array<OW_String> m_requestHeadersNew;
		std::istream* m_pIstrReturn;
		mutable OW_InetSocket m_socket;
		OW_String m_requestMethod;
		bool m_authRequired;
		mutable bool m_needsConnect;
		std::istream& m_istr;
		std::ostream& m_ostr;
		OW_Bool m_doDeflateOut;
		OW_ClientAuthCBIFCRef m_loginCB;
		int m_retryCount;


		OW_Bool headerHasKey(const OW_String& key)
		{
			return OW_HTTPUtils::headerHasKey(m_responseHeaders, key);
		}
		OW_String getHeaderValue(const OW_String& key)
		{
			return OW_HTTPUtils::getHeaderValue(m_responseHeaders, key);
		}
		void addHeaderCommon(const OW_String& key, const OW_String& value)
		{
			OW_HTTPUtils::addHeader(m_requestHeadersCommon, key, value);
		}
		void addHeaderNew(const OW_String& key, const OW_String& value)
		{
			OW_HTTPUtils::addHeader(m_requestHeadersNew, key, value);
		}
		void sendHeaders(const OW_String& method,
			const OW_String& prot);
		Resp_t processHeaders(OW_String& statusLine);

		std::istream* convertToFiniteStream();
		void prepareForRetry();

		void handleAuth(); // process authorization
		void checkConnection();
		OW_String checkResponse(Resp_t& rt);
		void prepareHeaders();

		void sendDataToServer( OW_TempFileStream* tfs, const OW_String& methodName, const OW_String& nameSpace );

		OW_HTTPClient(const OW_HTTPClient&);
		OW_HTTPClient& operator=(const OW_HTTPClient&);
};


#endif	// OW_HTTPCLIENT_HPP_
