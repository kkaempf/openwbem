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
#ifndef OW_HTTPSVRCONNECTION_HPP_INCLUDE_GUARD_
#define OW_HTTPSVRCONNECTION_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Runnable.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_String.hpp"
#include "OW_HTTPServer.hpp"
#include "OW_HTTPStatusCodes.hpp"
#include "OW_Socket.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_CIMProtocolIStreamIFC.hpp"
#include <iosfwd>
#include <fstream>

namespace OpenWBEM
{

class UnnamedPipe;
class TempFileStream;
class HTTPSvrConnection: public Runnable
{
public:
	/**
	 * Start a new http server connection.  This is called after
	 * HTTPServer accepts the connection.
	 * @param socket the socket (just accepted)
	 * @param htin a pointer to the HTTPServer
	 * @param upipe a pipe to receive a shutdown signal from
	 * @param opts The configuration options struct (see HTTPServer.hpp)
	 */
	HTTPSvrConnection(Socket socket, HTTPServer* htin,
		Reference<UnnamedPipe>& upipe,
		const HTTPServer::Options& opts);
	~HTTPSvrConnection();
	/**
	 * start processing the connection
	 */
	virtual void run();
	// these are public so that authentication modules can access them.
	/**
	 * Do the request headers contain a specific key?
	 * @param key the key to look for
	 * @return true if the key is found in the request headers.
	 */
	bool headerHasKey(const String& key) const
	{
		return HTTPUtils::headerHasKey(m_requestHeaders, key);
	}
	/**
	 * Get the value associated with a key in the request headers.
	 * @param key the key to look up
	 * @return the value associated with the key in the request headers.
	 */
	String getHeaderValue(const String& key) const
	{
		return HTTPUtils::getHeaderValue(m_requestHeaders, key);
	}
	/**
	 * Add a header to the response headers.
	 * @param key the key of the header (left of the ':')
	 * @param value the value (right of the ':')
	 */
	void addHeader(const String& key, const String& value)
	{
		HTTPUtils::addHeader(m_responseHeaders, key, value);
	}
	/**
	 * Get the request status line
	 *  example: POST /cimom HTTP/1.1
	 * @return a string array containing the tokenized request status line.
	 */
	Array<String> getRequestLine() const { return m_requestLine; }
	/**
	 * Set the details of the error to be returned.  This is usefull for
	 * auth modules to be able to describe why authentication failed.
	 * @param details the details of the error.
	 * @example
	 * 	// if we say
	 * 	setErrorDetails("foo");
	 * 	// and return 401, the response status line would look like this:
	 * 	// HTTP/1.1 401 Unauthorized: foo
	 */
	void setErrorDetails(const String& details) { m_errDetails = details; }
	
	/**
	 * Get the hostname of the server.
	 */
	String getHostName();
protected:
private:
	enum requestMethod_t
	{
		BAD = 0,
		POST,
		M_POST,
		TRACE,
		OPTIONS
	};
	enum httpVerFlag_t
	{
		HTTP_VER_BAD = 0,
		HTTP_VER_10,
		HTTP_VER_11
	};
	Array<String> m_requestLine;
	HTTPHeaderMap m_requestHeaders;
	HTTPServer* m_pHTTPServer;
	std::istream* m_pIn;
	std::ofstream m_tempFile;
	Socket m_socket;
	std::ostream& m_ostr;
	int m_resCode;
	bool m_needSendError;
	Array<String> m_responseHeaders;
	httpVerFlag_t m_httpVersion;
	requestMethod_t m_method;
	std::istream& m_istr;
	bool m_isClose;
	Int64 m_contentLength;
	bool m_chunkedIn;
	bool m_deflateCompressionIn;
	bool m_deflateCompressionOut;
	String m_errDetails;
	String m_reqHeaderPrefix;
	String m_respHeaderPrefix;
	bool m_isAuthenticated;
	Reference<UnnamedPipe> m_upipe;
	bool m_chunkedOut;
	String m_userName;
	bool m_clientIsOpenWBEM2;

	RequestHandlerIFCRef m_requestHandler;
	HTTPServer::Options m_options;
	int processRequestLine();
	int processHeaders(OperationContext& context);
	void trace();
	void options(OperationContext& context);
	void post(std::istream& istr, OperationContext& context);
	void sendError(int resCode);
	void beginPostResponse();
	void initRespStream(std::ostream*& ostrEntity);
	void sendPostResponse(std::ostream* ostrEntity,
		TempFileStream& ostrError);
	int performAuthentication(const String& info, OperationContext& context);
	void sendHeaders(int sc, int len = -1);
	void cleanUpIStreams(Reference<CIMProtocolIStreamIFC> istrm);
	Reference<CIMProtocolIStreamIFC> convertToFiniteStream(
			std::istream& istr);
};

} // end namespace OpenWBEM

#endif
