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
#ifndef _OW_HTTPSVRCONNECTION_HPP__
#define _OW_HTTPSVRCONNECTION_HPP__

#include "OW_config.h"
#include "OW_Thread.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_String.hpp"
#include "OW_HTTPServer.hpp"
#include "OW_HTTPStatusCodes.h"
#include "OW_InetSocket.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include <iosfwd>
#include <fstream>

class OW_UnnamedPipe;
class OW_StringStream;

class OW_HTTPSvrConnection: public OW_Runnable
{
public:

	/**
	 * Start a new http server connection.  This is called after
	 * OW_HTTPServer accepts the connection.
	 * @param socket the socket (just accepted)
	 * @param htin a pointer to the OW_HTTPServer
	 * @param upipe a pipe to receive a shutdown signal from
	 * @param opts The configuration options struct (see OW_HTTPServer.hpp)
	 */
	OW_HTTPSvrConnection(OW_InetSocket socket, OW_HTTPServer* htin,
		OW_Reference<OW_UnnamedPipe>& upipe,
		const OW_HTTPServer::Options& opts);
	~OW_HTTPSvrConnection();

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
	OW_Bool headerHasKey(const OW_String& key) const
	{
		return OW_HTTPUtils::headerHasKey(m_requestHeaders, key);
	}

	/**
	 * Get the value associated with a key in the request headers.
	 * @param key the key to look up
	 * @return the value associated with the key in the request headers.
	 */
	OW_String getHeaderValue(const OW_String& key) const
	{
		return OW_HTTPUtils::getHeaderValue(m_requestHeaders, key);
	}

	/**
	 * Add a header to the response headers.
	 * @param key the key of the header (left of the ':')
	 * @param value the value (right of the ':')
	 */
	void addHeader(const OW_String& key, const OW_String& value)
	{
		OW_HTTPUtils::addHeader(m_responseHeaders, key, value);
	}

	/**
	 * Get the request status line
	 *  example: POST /cimom HTTP/1.1
	 * @return a string array containing the tokenized request status line.
	 */
	OW_Array<OW_String> getRequestLine() const { return m_requestLine; }

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
	void setErrorDetails(const OW_String& details) { m_errDetails = details; }
	
	/**
	 * Get the hostname of the server.
	 */
	OW_String getHostName();

protected:

private:

	typedef enum requestMethod_t
	{
		BAD = 0,
		POST,
		M_POST,
		TRACE,
		OPTIONS
	};

	typedef enum httpVerFlag_t
	{
		HTTP_VER_BAD = 0,
		HTTP_VER_10,
		HTTP_VER_11
	};

	OW_Array<OW_String> m_requestLine;
	OW_HTTPHeaderMap m_requestHeaders;
	OW_HTTPServer* m_pHTTPServer;
	std::istream* m_pIn;
	std::ofstream m_tempFile;
	OW_InetSocket m_socket;
	std::ostream& m_ostr;
	int m_resCode;
	OW_Bool m_needSendError;
	OW_Array<OW_String> m_responseHeaders;

	httpVerFlag_t m_httpVersion;
	requestMethod_t m_method;
	std::istream& m_istr;
	OW_Bool m_isClose;
	OW_Int64 m_contentLength;
	OW_Bool m_chunkedIn;
	OW_Bool m_deflateCompressionIn;
	OW_Bool m_deflateCompressionOut;
	OW_String m_errDetails;
	OW_String m_reqHeaderPrefix;
	OW_String m_respHeaderPrefix;
	OW_Bool m_isAuthenticated;
	OW_Reference<OW_UnnamedPipe> m_upipe;
	OW_Bool m_chunkedOut;
	OW_String m_userName;

	OW_HTTPServer::Options m_options;


	int processRequestLine();
	int processHeaders();
	void trace();
	void options(std::istream& istr);
	void post(std::istream& istr);
	void sendError(int resCode);
	void beginPostResponse();
	void initRespStream(std::ostream*& ostrEntity);
	void sendPostResponse(std::ostream* ostrEntity,
		OW_StringStream& ostrError);
	int performAuthentication(const OW_String& info);
	void sendHeaders(int sc, int len = -1);
	void cleanUpIStreams(std::istream*& istrm);
	std::istream* convertToFiniteStream(std::istream& istr);
};

#endif	// _OW_HTTPSVRCONNECTION_HPP__

