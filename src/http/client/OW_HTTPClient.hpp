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

#ifndef OW_HTTPCLIENT_HPP_
#define OW_HTTPCLIENT_HPP_
#include "OW_config.h"
#include "blocxx/Socket.hpp"
#include "blocxx/AutoPtr.hpp"
#include "blocxx/Map.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_CIMProtocolIFC.hpp"
#include "OW_URL.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_ClientFwd.hpp"
#include "blocxx/SSLCtxMgr.hpp"
#include "OW_SPNEGOHandler.hpp"

namespace OW_NAMESPACE
{

/// @todo  Rewrite this class. It's way too big and convoluted.
// Take a look at Apache's HttpClient library: http://jakarta.apache.org/commons/httpclient/index.html

/**
 * HTTPClient encapulates the functionality of a CIM HTTP Client.  It handles the details of the HTTP protocol.
 *
 * By default the connection timeout is set to 1 minute, and the send and receive timeouts are set to 10 minutes.
 *
 * Thread safety: non-reentrant.
 * Copy semantics: Non-copyable.
 * Exception safety: Basic.
 */
class OW_HTTP_API HTTPClient : public CIMProtocolIFC
{
public:
	/**
	 * Creates an httpClient object.  This represents one client
	 * connection to a CIMOM.
	 * @param url The URL to the CIMOM the client should connect to.
	 * 		URLs have this form:
	 * 		[scheme"://"][[<principal>][":"<credential>]"@"]<host>[":"<port>]["/"<namespace name>["/:"<model path>]]
	 * The only required element is <host>
	 *
	 * Standard WBEM schemes are: cimxml.wbem, cimxml.wbems, http, https.
	 * OW specific WBEM schemes are: owbinary.wbem, owbinary.wbems
	 *
	 * A port may be a number to indicate a TCP port, or it may be the special
	 *  value owipc which indicates the Unix Domain Socket for the system.
	 *
	 * example: "https://jdd:test@myhost.com:5989/root/cimv2"
	 *
	 * If (principal == "" && (host == "localhost" || host == "127.0.0.1")) then HTTPClient will attempt to use OWLocal
	 * authentication.
	 *
	 * @throws SocketException If an SSL connection was requested, but support for SSL is not available.
	 */
	HTTPClient(const blocxx::String& url, const blocxx::SSLClientCtxRef& sslCtx = blocxx::SSLClientCtxRef());
	virtual ~HTTPClient();
	virtual blocxx::Reference<std::ostream> beginRequest(
			const blocxx::String& methodName, const blocxx::String& cimObject);

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
	 * @param requestType The type of request, currently must be one of
	 *  E_CIM_OPERATION_REQUEST, E_CIM_EXPORT_REQUEST
	 * @param cimProtocolVersion The version of the CIM Protocol in use
	 * @return an istream& containing the response from the server
	 * @exception HTTPException
	 * @exception SocketException
	 *
	 */
	virtual blocxx::Reference<std::istream> endRequest(
		const blocxx::Reference<std::ostream>& request,
		const blocxx::String& methodName,
		const blocxx::String& cimObject,
		ERequestType requestType,
		const blocxx::String& cimProtocolVersion);

	virtual void endResponse(std::istream & istr);

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
	blocxx::SocketAddress getLocalAddress() const;
	/**
	 * Gets the address of the peer connection
	 * @return An SocketAddress corresponding to the peer connection
	 */
	blocxx::SocketAddress getPeerAddress()  const;
	/**
	 * Set the HTTP path to use.  Previously this was specified in the URL,
	 * however now with the WBEM URI spec, that portion of the URL is
	 * allocated to the namespace name.  To use a different HTTP path (the
	 * path the M-POST is sent to), call this function.  An initial / will
	 * not be prepended.  The default HTTP path is "/cimom"
	 *
	 * @param newPath The new HTTP path to use.
	 */
	void setHTTPPath(const blocxx::String& newPath);

	/**
	 * After calling this function, the next request will (re)connect and
	 * send credentials using basic authentication.  This could be a
	 * security risk, and should only be used if you understand the
	 * risks.  Using this avoids the extra round-trip that typically
	 * happens with http authentication.
	 */
	void assumeBasicAuth();

	void assumeSPNEGOAuth();

	/**
	 * Close the connetion to the CIMOM. This will free resources used for the
	 * client session.
	 */
	virtual void close();

	/**
	 * Add a custom header to add to requests.  "<name>: <value>"
	 * will be added to each request.
	 * @param name The header name
	 * @param value The header value
	 */
	void addCustomHeader(const blocxx::String& name, const blocxx::String& value);

	/**
	 * Retrieve a given header value from the server response.
	 * @param The name of the header to retrieve the value of
	 * 		(e.g. "Content-Language")
	 * @param Output param that will contain the value of the header.
	 * @return true if the header exists. Otherwise false.
	 */
	bool getResponseHeader(const blocxx::String& hdrName, blocxx::String& valueOut) const;

	/**
	 * Set the receive timeout on the socket
	 * @param timeout The timeout to use when waiting for data
	 */
	virtual void setReceiveTimeout(const blocxx::Timeout& timeout);
	/**
	 * Get the receive timeout
	 * @return The receive timeout
	 */
	virtual blocxx::Timeout getReceiveTimeout() const;
	/**
	 * Set the send timeout on the socket
	 * @param timeout The timeout to use when waiting to send data
	 */
	virtual void setSendTimeout(const blocxx::Timeout& timeout);
	/**
	 * Get the send timeout
	 * @return The number of seconds of the send timeout
	 */
	virtual blocxx::Timeout getSendTimeout() const;
	/**
	 * Set the connect timeout on the socket
	 * @param timeout The connect timeout
	 */
	virtual void setConnectTimeout(const blocxx::Timeout& timeout);
	/**
	 * Get the connect timeout
	 * @return The number of seconds of the connect timeout
	 */
	virtual blocxx::Timeout getConnectTimeout() const;
	/**
	 * Set all timeouts (send, receive, connect)
	 * @param timeout The timeouts.
	 */
	virtual void setTimeouts(const blocxx::Timeout& timeout);

	/**
	 * Set the SPNEGO handler.
	 */
	void setSPNEGOHandler(const SPNEGOHandlerRef& spnegoHandler);

private:
	/*
	 * @throws SocketException If an SSL connection was requested, but support for SSL is not available.
	 */
	void setUrl();

	void cleanUpIStreams();
	void receiveAuthentication();
	void sendAuthorization();
	bool receiveOptions( void );

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	blocxx::String m_sAuthorization;
	blocxx::String m_sRealm;

#ifndef OW_DISABLE_DIGEST
	blocxx::String m_sDigestNonce;
	blocxx::String m_sDigestCNonce;
	blocxx::UInt8 m_iDigestNonceCount;
	blocxx::String m_sDigestSessionKey;
	blocxx::String m_sDigestResponse;
#endif
	enum Resp_t
	{
		E_RESPONSE_FATAL,
		E_RESPONSE_RETRY,
		E_RESPONSE_GOOD,
		E_RESPONSE_CONTINUE
	};
	blocxx::SocketAddress m_serverAddress;
	URL m_url;
	HTTPHeaderMap m_responseHeaders;
	// Persistant headers remain for the life of the
	// HTTPClient.  They are included in each request
	blocxx::Array<blocxx::String> m_requestHeadersPersistent;
	// Common headers are used for only a single request,
	// but are reused if the request must be repeated (with
	// new authentication credentials, for instance).
	blocxx::Array<blocxx::String> m_requestHeadersCommon;
	// New headers are replaced each time the client repeats
	// a request (with new auth credentials, for instance).
	blocxx::Array<blocxx::String> m_requestHeadersNew;
	blocxx::Reference<std::istream> m_pIstrReturn;
	blocxx::SSLClientCtxRef m_sslCtx;
	mutable blocxx::Socket m_socket;
	blocxx::String m_requestMethod;
	bool m_authRequired;
	std::istream& m_istr;
	std::ostream& m_ostr;
	bool m_doDeflateOut;
	int m_retryCount;
	blocxx::String m_httpPath;
	bool m_uselocalAuthentication;
	blocxx::String m_localNonce;
	blocxx::String m_localCookieFile;
	blocxx::String m_statusLine;
	// stores the ns value of the Man header
	blocxx::String m_ns;
	bool m_closeConnection;
	blocxx::String m_spnegoData;
	SPNEGOHandlerRef m_spnegoHandler;
	int m_statusCode;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	bool headerHasKey(const blocxx::String& key)
	{
		if (!HTTPUtils::headerHasKey(m_responseHeaders, m_ns + "-" + key))
		{
			return HTTPUtils::headerHasKey(m_responseHeaders, key);
		}
		return true;
	}
	blocxx::String getHeaderValue(const blocxx::String& key)
	{
		blocxx::String rval = HTTPUtils::getHeaderValue(m_responseHeaders, m_ns + "-" + key);
		if (rval.empty())
		{
			return HTTPUtils::getHeaderValue(m_responseHeaders, key);
		}
		return rval;
	}
	void addHeaderCommon(const blocxx::String& key, const blocxx::String& value)
	{
		HTTPUtils::addHeader(m_requestHeadersCommon, key, value);
	}
	void addHeaderPersistent(const blocxx::String& key, const blocxx::String& value)
	{
		HTTPUtils::addHeader(m_requestHeadersPersistent, key, value);
	}
	void addHeaderNew(const blocxx::String& key, const blocxx::String& value)
	{
		HTTPUtils::addHeader(m_requestHeadersNew, key, value);
	}
	void sendHeaders(const blocxx::String& method,
		const blocxx::String& prot);
	/**
	 * precondition: m_statusLine is set to the status line, and the headers have been read
	 * @param reasonPhrase - out parameter that will contain the reason phrase portion of the HTTP status line.
	 */
	Resp_t processHeaders(blocxx::String& reasonPhrase);
	blocxx::Reference<std::istream> convertToFiniteStream();
	void prepareForRetry();
	void handleAuth(); // process authorization
	void checkConnection();
	/**
	 * @return The reason phrase from the HTTP status line of the response
	 */
	blocxx::String checkResponse(Resp_t& rt);
	void prepareHeaders();
	void sendDataToServer( const blocxx::Reference<blocxx::TempFileStream>& tfs, const blocxx::String& methodName, const blocxx::String& cimObject, ERequestType requestType );
	/**
	 * If no credentials were provided in the url to the constructor, but an authentication callback was given, then call the callback to get credentials.
	 * @postcondition m_url.principal != ""
	 * @throws HTTPException if the postcondition cannot be satisfied.
	 */
	void getCredentialsIfNecessary();
	void copyStreams(std::ostream& ostr, std::istream& istr);
	/**
	 * sets m_statusLine if there is a status line to be read, this function doesn't block.
	 */
	void getStatusLine();

	/**
	 * if the server has disconnected or sent us something since our last request
	 * (remember connections are persistent), we need to process it *before* we
	 * send another request, or if we get something while we are sending, we need
	 * to look at the header to know if we should keep sending data.
	 * This function doesn't block, it just looks to see if there is a http status
	 * line on the incoming socket, and if so it reads it and examines the status
	 * code.  If there is no input or If the status code < 300, E_STATUS_GOOD is
	 * returned, else E_STATUS_ERROR.
	 */
	enum EStatusLineSummary
	{
		E_STATUS_GOOD,
		E_STATUS_ERROR
	};
	EStatusLineSummary checkAndExamineStatusLine();


	// unimplemented
	HTTPClient(const HTTPClient&);
	HTTPClient& operator=(const HTTPClient&);
};

} // end namespace OW_NAMESPACE

#endif
