/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_HTTPCLIENT_HPP_
#define OW_HTTPCLIENT_HPP_
#include "OW_config.h"
#include "OW_Socket.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Map.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_CIMProtocolIFC.hpp"
#include "OW_URL.hpp"

#if defined(GOOD)
#undef GOOD
#endif

namespace OpenWBEM
{

class TempFileStream;
class HTTPClient : public CIMProtocolIFC
{
	public:
		/**
		 * Creates an httpClient object.  This represents one client
		 * connection to a CIMOM.
		 * @param url The URL to the CIMOM the client should connect to.
		 * 		URLs have this form:
		 * 		http[s]://[user:passwd@]hostname[:port][/path]	
		 */
		HTTPClient( const String& url);
		virtual ~HTTPClient();
		virtual Reference<std::iostream> beginRequest(
				const String& methodName, const String& cimObject);
		/**
		 * Establishes a connection (if not already connected) to the
		 * CIMOM and sends a request.  An istream& is returned containing
		 * the response from the CIMOM, after protocol processing is done.
		 * @param request An istream& containing the request to be send to
		 * 	the CIMOM.
		 * @param methodName The CIM method that corresponds to the request.
		 * @cimObject the CIM object the request applies to.
		 *  If this is an intrinsic method, it must be a namespace.
		 *  If an extrinsic method is being invoked, it must be a class
		 *  or instance path in ObjectPath format.
		 * @return an istream& containing the response from the server
		 * @exception HTTPException
		 * @exception SocketException
		 *
		 */
		virtual Reference<CIMProtocolIStreamIFC> 
			endRequest(Reference<std::iostream> request,
				const String& methodName, const String& cimObject);
		/**
		 * Sends an OPTIONS request to the HTTP server, and reports the
		 * results.
		 * @return a CIMFeatures object listing the features of the CIMOM.
		 */
		virtual CIMFeatures getFeatures();
		/**
		 * Gets the address of the local machine
		 * @return An SocketAddress corresponding to the local machine.
		 */
		SocketAddress getLocalAddress() const;
		/**
		 * Gets the address of the peer connection
		 * @return An SocketAddress corresponding to the peer connection
		 */
		SocketAddress getPeerAddress()  const;
	private:
		void setUrl();
		void cleanUpIStreams();
		void receiveAuthentication();
		void sendAuthorization();
		bool receiveOptions( void );
		
		String m_sAuthorization;
		
#ifndef OW_DISABLE_DIGEST
		String m_sRealm;
		String m_sDigestNonce;
		String m_sDigestCNonce;
		UInt8 m_iDigestNonceCount;
		String m_sDigestSessionKey;
		String m_sDigestResponse;
#endif
		typedef enum Resp_t
		{
			FATAL,
			RETRY,
			GOOD,
			CONTINUE
		};
		SocketAddress m_serverAddress;
		URL m_url;
		HTTPHeaderMap m_responseHeaders;
		Array<String> m_requestHeadersCommon;
		Array<String> m_requestHeadersNew;
		Reference<CIMProtocolIStreamIFC> m_pIstrReturn;
		mutable Socket m_socket;
		String m_requestMethod;
		bool m_authRequired;
		mutable bool m_needsConnect;
		std::istream& m_istr;
		std::ostream& m_ostr;
		bool m_doDeflateOut;
		int m_retryCount;
		bool headerHasKey(const String& key)
		{
			return HTTPUtils::headerHasKey(m_responseHeaders, key);
		}
		String getHeaderValue(const String& key)
		{
			return HTTPUtils::getHeaderValue(m_responseHeaders, key);
		}
		void addHeaderCommon(const String& key, const String& value)
		{
			HTTPUtils::addHeader(m_requestHeadersCommon, key, value);
		}
		void addHeaderNew(const String& key, const String& value)
		{
			HTTPUtils::addHeader(m_requestHeadersNew, key, value);
		}
		void sendHeaders(const String& method,
			const String& prot);
		Resp_t processHeaders(String& statusLine);
		Reference<CIMProtocolIStreamIFC> convertToFiniteStream();
		void prepareForRetry();
		void handleAuth(); // process authorization
		void checkConnection();
		String checkResponse(Resp_t& rt);
		void prepareHeaders();
		void sendDataToServer( Reference<TempFileStream> tfs, const String& methodName, const String& cimObject );
		HTTPClient(const HTTPClient&);
		HTTPClient& operator=(const HTTPClient&);
};

} // end namespace OpenWBEM

typedef OpenWBEM::HTTPClient OW_HTTPClient;

#endif	// OW_HTTPCLIENT_HPP_
