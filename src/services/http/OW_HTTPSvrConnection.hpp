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

#ifndef OW_HTTPSVRCONNECTION_HPP_INCLUDE_GUARD_
#define OW_HTTPSVRCONNECTION_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/Runnable.hpp"
#include "blocxx/Mutex.hpp"
#include "blocxx/MutexLock.hpp"
#include "blocxx/String.hpp"
#include "OW_HTTPServer.hpp"
#include "OW_HTTPStatusCodes.hpp"
#include "blocxx/Socket.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_HttpCommonFwd.hpp"
#include "blocxx/Reference.hpp"
#include <iosfwd>
#include <fstream>

namespace OW_NAMESPACE
{

class OW_HTTPSVC_API HTTPSvrConnection: public blocxx::Runnable
{
public:
#ifdef OW_WIN32
	/**
	 * Start a new http server connection.  This is called after
	 * HTTPServer accepts the connection.
	 * @param socket the socket (just accepted)
	 * @param htin a pointer to the HTTPServer
	 # @param eventArg Handle to an event that gets signaled on shutdown
	 * @param opts The configuration options struct (see HTTPServer.hpp)
	 */
	HTTPSvrConnection(const blocxx::Socket& socket, HTTPServer* htin,
		HANDLE eventArg, const HTTPServer::Options& opts);
#else
	/**
	 * Start a new http server connection.  This is called after
	 * HTTPServer accepts the connection.
	 * @param socket the socket (just accepted)
	 * @param htin a pointer to the HTTPServer
	 * @param upipe a pipe to receive a shutdown signal from
	 * @param opts The configuration options struct (see HTTPServer.hpp)
	 */
	HTTPSvrConnection(const blocxx::Socket& socket, HTTPServer* htin,
		blocxx::UnnamedPipeRef& upipe,
		const HTTPServer::Options& opts);
#endif

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
	bool headerHasKey(const blocxx::String& key) const
	{
		return HTTPUtils::headerHasKey(m_requestHeaders, key);
	}
	/**
	 * Get the value associated with a key in the request headers.
	 * @param key the key to look up
	 * @return the value associated with the key in the request headers.
	 */
	blocxx::String getHeaderValue(const blocxx::String& key) const
	{
		return HTTPUtils::getHeaderValue(m_requestHeaders, key);
	}
	/**
	 * Add a header to the response headers.
	 * @param key the key of the header (left of the ':')
	 * @param value the value (right of the ':')
	 */
	void addHeader(const blocxx::String& key, const blocxx::String& value)
	{
		HTTPUtils::addHeader(m_responseHeaders, key, value);
	}
	/**
	 * Get the request status line
	 *  example: POST /cimom HTTP/1.1
	 * @return a string array containing the tokenized request status line.
	 */
	blocxx::StringArray getRequestLine() const { return m_requestLine; }
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
	void setErrorDetails(const blocxx::String& details) { m_errDetails = details; }

	/**
	 * Get the hostname of the server.
	 */
	blocxx::String getHostName() const;

	/**
	 * Get a unique id for this connection.
	 */
	blocxx::UInt64 getConnectionId() const;

protected:
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

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
	blocxx::StringArray m_requestLine;
	HTTPHeaderMap m_requestHeaders;
	HTTPServer* m_pHTTPServer;
	std::istream* m_pIn;
	std::ofstream m_tempFile;
	blocxx::Socket m_socket;
	std::ostream& m_ostr;
	int m_resCode;
	bool m_needSendError;
	blocxx::Array<blocxx::String> m_responseHeaders;
	blocxx::Array<blocxx::String> m_trailers;
	httpVerFlag_t m_httpVersion;
	requestMethod_t m_method;
	std::istream& m_istr;
	bool m_isClose;
	blocxx::Int64 m_contentLength;
	bool m_chunkedIn;
	bool m_deflateCompressionIn;
	bool m_deflateCompressionOut;
	blocxx::String m_errDetails;
	blocxx::String m_reqHeaderPrefix;
	blocxx::String m_respHeaderPrefix;
	bool m_isAuthenticated;
#ifdef OW_WIN32
	HANDLE m_event;
#else
	blocxx::IntrusiveReference<blocxx::UnnamedPipe> m_upipe;
#endif
	bool m_chunkedOut;
	blocxx::String m_userName;
	bool m_clientIsOpenWBEM2;

	RequestHandlerIFCRef m_requestHandler;
	HTTPServer::Options m_options;
	bool m_shutdown;

	// Don't switch the order of the next 2 member vars. The order is important, since Deflate may hold a pointer to Chunked,
	// and it's destructor may call functions on Chunked. Yeah, this is a BAD design!
	blocxx::Reference<HTTPChunkedOStream> m_HTTPChunkedOStreamRef;
#ifdef OW_HAVE_ZLIB_H
	blocxx::Reference<HTTPDeflateOStream> m_HTTPDeflateOStreamRef;
#endif

	blocxx::Reference<blocxx::TempFileStream> m_TempFileStreamRef;

	blocxx::UInt64 m_connectionId;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	int processRequestLine();
	int processHeaders(OperationContext& context);
	void trace();
	void options(OperationContext& context);
	void post(std::istream& istr, OperationContext& context);
	void sendError(int resCode);
	void beginPostResponse();
	void initRespStream(std::ostream*& ostrEntity);
	void sendPostResponse(std::ostream* ostrEntity,
		blocxx::TempFileStream& ostrError, OperationContext& context);
	int performAuthentication(const blocxx::String& info, OperationContext& context);
	void sendHeaders(int sc, int len = -1);
	void cleanUpIStreams(const blocxx::Reference<std::istream>& istrm);
	blocxx::Reference<std::istream> convertToFiniteStream(
			std::istream& istr);
	blocxx::String getContentLanguage(OperationContext& context, bool& setByProvider,
		bool& clientSpecified);

	void doShutdown();

	/**
	 * Add a HTTP trailer (header at the end of a chunked entity)
	 * @param key the name of the trailer (left of the ':')
	 * @param value the value of the trailer (right of the ':')
	 */
	void addTrailer(const blocxx::String& key, const blocxx::String& value);
	void outputTrailers();
};

} // end namespace OW_NAMESPACE

#endif
