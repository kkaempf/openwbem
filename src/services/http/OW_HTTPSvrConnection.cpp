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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_HTTPSvrConnection.hpp"
#include "OW_IOException.hpp"
#include "OW_HTTPStatusCodes.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_HTTPDeflateIStream.hpp"
#include "OW_HTTPDeflateOStream.hpp"
#include "OW_HTTPLenLimitIStream.hpp"
#include "OW_Select.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Assertion.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_HTTPException.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_ThreadCancelledException.hpp"
#include "OW_OperationContext.hpp"
#include "OW_SessionLanguage.hpp"
#include "OW_AuthenticationException.hpp"

#include <ios> // for std::ios::badbit

#if defined(BAD)
#undef BAD
#endif

namespace OpenWBEM
{

using std::ios;
using std::istream;
using std::ostream;
using std::flush;
#ifndef OW_LOGDEBUG
#define OW_LOGDEBUG(x) m_options.env->getLogger()->logDebug(x)
#endif
#ifndef OW_LOGERROR
#define OW_LOGERROR(x) m_options.env->getLogger()->logError(x)
#endif
#ifndef OW_LOGCUSTINFO
#define OW_LOGCUSTINFO(x) m_options.env->getLogger()->logInfo(x)
#endif
#ifndef OW_LOGFATALERROR
#define OW_LOGFATALERROR(x) m_options.env->getLogger()->logFatalError(x)
#endif
//////////////////////////////////////////////////////////////////////////////
HTTPSvrConnection::HTTPSvrConnection(const Socket& socket,
	HTTPServer* htin,
	IntrusiveReference<UnnamedPipe>& upipe,
	const HTTPServer::Options& opts)
	: Runnable()
	, m_requestLine()
	, m_requestHeaders()
	, m_pHTTPServer(htin)
	, m_socket(socket)
	, m_ostr(m_socket.getOutputStream())
	, m_resCode(SC_OK)
	, m_needSendError(false)
	, m_responseHeaders()
	, m_httpVersion(HTTP_VER_BAD)
	, m_method(BAD)
	, m_istr(m_socket.getInputStream())
	, m_isClose(false)
	, m_contentLength(-1)
	, m_chunkedIn(false)
	, m_deflateCompressionIn(false)
	, m_deflateCompressionOut(false)
	, m_errDetails()
	, m_reqHeaderPrefix()
	, m_respHeaderPrefix()
	, m_isAuthenticated(false)
	, m_upipe(upipe)
	, m_chunkedOut(false)
	, m_userName()
	, m_clientIsOpenWBEM2(false)
	, m_requestHandler()
	, m_options(opts)
{
	m_socket.setTimeouts(m_options.timeout);
}
//////////////////////////////////////////////////////////////////////////////
// Destructor
HTTPSvrConnection::~HTTPSvrConnection()
{
	try
	{
		m_socket.disconnect();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::run()
{
	CIMProtocolIStreamIFCRef istrToReadFrom(0);
	SelectTypeArray selArray;
	selArray.push_back(m_upipe->getSelectObj());
	selArray.push_back(m_socket.getSelectObj());
	try
	{
		m_isAuthenticated = false;
		OperationContext context;
		while (m_istr.good())
		{
			//m_isAuthenticated = false;
			m_errDetails.erase();
			m_requestLine.clear();
			m_requestHeaders.clear();
			m_reqHeaderPrefix.erase();
			m_responseHeaders.clear();
			m_needSendError = false;
			m_resCode = SC_OK;
			m_contentLength = -1;
			m_chunkedIn = false;
			//
			// Add response headers common to all responses
			//
			addHeader("Date",
				HTTPUtils::date());
			addHeader("Cache-Control",
				"no-cache");
			addHeader("Server",
				OW_PACKAGE "/" OW_VERSION " (CIMOM)");
			int selType = Select::select(selArray, m_options.timeout * 1000); // *1000 to convert seconds to milliseconds
			if (selType == Select::SELECT_ERROR)
			{
			   OW_THROW(SocketException, "Error occurred during select()");
			}
			if (selType == Select::SELECT_INTERRUPTED)
			{
			   OW_THROW(SocketException, "select() was interrupted.");
			}
			if (selType == Select::SELECT_TIMEOUT)
			{
			   m_resCode = SC_REQUEST_TIMEOUT;
			   m_errDetails = "Timeout waiting for request.";
			   sendError(m_resCode);
			   return;
			}
			if (selType == 0)	// Unnamped pipe selected
			{
			   m_resCode = SC_SERVICE_UNAVAILABLE;
			   m_errDetails = "Server is shutting down."
				   "  Please try again later.";
			   sendError(m_resCode);
			   return;
			}
			if (!HTTPUtils::parseHeader(m_requestHeaders, m_requestLine, m_istr))
			{
				if (Socket::gotShutDown())
				{
					m_errDetails = "Server is shutting down!";
					m_resCode = SC_INTERNAL_SERVER_ERROR;
				}
				else if (!m_istr)
				{
					// client closed the socket
					return;
				}
				else
				{
					m_errDetails = "There was a problem parsing the request Header";
					m_resCode = SC_BAD_REQUEST;
				}
				sendError(m_resCode);
				return;
			}
			//
			// Process request line
			//
			m_resCode = processRequestLine();
			if (m_resCode >= 300)	// problem with request detected in request line.
			{
				sendError(m_resCode);
				return;
			}
			//
			// Process Headers
			//
			context.setStringData(OperationContext::HTTP_PATH, m_requestLine[1]);

			m_resCode = processHeaders(context);
			istrToReadFrom = convertToFiniteStream(m_istr);
			if (m_resCode >= 300)	// problem with request detected in headers.
			{
				cleanUpIStreams(istrToReadFrom);
				sendError(m_resCode);
				return;
			}
			//
			// Set up input stream to read entity
			//
			switch (m_method)
			{
				case TRACE:
					trace();
					break;
				case M_POST:
				case POST:
					if (!istrToReadFrom)
					{
						OW_THROW(HTTPException,
							"POST, but no content-length or chunking");
					}
					post(*istrToReadFrom, context);
					break;
				case OPTIONS:
					options(context);
					break;
				default:
					// should never happen.
					m_errDetails = "This should never happen.";
					m_resCode = SC_INTERNAL_SERVER_ERROR;
					cleanUpIStreams(istrToReadFrom);
					sendError(m_resCode);
					return;
			} // switch (m_method)
			m_ostr.flush();
			cleanUpIStreams(istrToReadFrom);
			if (m_isClose)
			{
				break;
			}
		} // while (m_istr.good())
	} // try
	catch (CIMErrorException& cee)
	{
		addHeader("CIMError", cee.getMessage());
		if (m_errDetails.empty())
		{
			m_errDetails = String("CIMError: ") + cee.getMessage();
		}
		cleanUpIStreams(istrToReadFrom);
		String errMsg(cee.getMessage());
		if (errMsg == CIMErrorException::unsupported_protocol_version ||
			errMsg == CIMErrorException::multiple_requests_unsupported ||
			errMsg == CIMErrorException::unsupported_cim_version ||
			errMsg == CIMErrorException::unsupported_dtd_version)
		{
			sendError(SC_NOT_IMPLEMENTED);
		}
		else
		{
			sendError(SC_BAD_REQUEST);
		}
	}
	catch (Exception& e)
	{
		OW_LOGERROR(Format("%1", e));
		m_errDetails = e.getMessage();
		cleanUpIStreams(istrToReadFrom);
		sendError(SC_INTERNAL_SERVER_ERROR);
	}
// gcc 2.x doesn't have ios_base::failure
#if !defined(__GNUC__) || __GNUC__ > 2
	catch (std::ios_base::failure& e)
	{
		// This happens if the socket is closed, so we don't have to do anything.
		OW_LOGDEBUG("Caught std::ios_base::failure, client has closed the connection");
	}
#endif
	catch (std::exception& e)
	{
		m_errDetails = Format("Caught std::exception (%1) in HTTPSvrConnection::run()", e.what());
		OW_LOGERROR(m_errDetails);
		cleanUpIStreams(istrToReadFrom);
		sendError(SC_INTERNAL_SERVER_ERROR);
	}
	catch (ThreadCancelledException&)
	{
		OW_LOGERROR("Got Thread Cancelled Exception in HTTPSvrConnection::run()");
		m_errDetails = "HTTP Server thread cancelled";
		cleanUpIStreams(istrToReadFrom);
		sendError(SC_INTERNAL_SERVER_ERROR);
		throw;
	}
	catch (...)
	{
		OW_LOGERROR("Got Unknown Exception in HTTPSvrConnection::run()");
		m_errDetails = "HTTP Server caught unknown exception";
		cleanUpIStreams(istrToReadFrom);
		sendError(SC_INTERNAL_SERVER_ERROR);
	}
	//m_socket.disconnect();
}
void
HTTPSvrConnection::cleanUpIStreams(const CIMProtocolIStreamIFCRef& istr)
{
	if (istr)
	{
		HTTPUtils::eatEntity(*istr);
	}
}
void
HTTPSvrConnection::beginPostResponse()
{
	m_respHeaderPrefix = HTTPUtils::getCounterStr();
	addHeader(
		"Content-Type", m_requestHandler->getContentType() + "; charset=\"utf-8\"");
	if (m_method == M_POST)
	{
		addHeader("Ext","");
	}
	addHeader("Man",
		"http://www.dmtf.org/cim/mapping/http/v1.0 ; ns=" + m_respHeaderPrefix);
	m_respHeaderPrefix += "-";
	if (m_deflateCompressionOut && m_chunkedOut)
	{
		addHeader("Content-Encoding", "deflate");
	}
	if (m_chunkedOut) // we only do chunked output is the client says they can handle trailers.
	{
		addHeader( "Transfer-Encoding", "chunked");
		addHeader(m_respHeaderPrefix + "CIMOperation", "MethodResponse");
		if (m_clientIsOpenWBEM2) // it uses different pre-standard trailer names
		{
			addHeader("Trailer",
				m_respHeaderPrefix + "CIMError, "
				+ m_respHeaderPrefix + "CIMErrorCode, "
				+ m_respHeaderPrefix + "CIMErrorDescription");
		}
		else // talking to someone who can understand new WBEM Ops 1.2 trailers
		{
			addHeader("Trailer",
				m_respHeaderPrefix + "CIMError, "
				+ m_respHeaderPrefix + "CIMStatusCode, "
				+ m_respHeaderPrefix + "CIMStatusDescription");
		}
		sendHeaders(m_resCode);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::initRespStream(ostream*& ostrEntity)
{
	OW_ASSERT(ostrEntity == 0);

	// these get deleted in post().  If this logic is changed, make sure you
	// update post() as well.

	if (m_chunkedOut)
	{
		ostrEntity = new HTTPChunkedOStream(m_ostr);
		ostrEntity->exceptions(std::ios::badbit);
		if (m_deflateCompressionOut)
		{
#ifdef OW_HAVE_ZLIB_H
			ostrEntity = new HTTPDeflateOStream(*ostrEntity);
			ostrEntity->exceptions(std::ios::badbit);
#else
			OW_THROW(HTTPException, "Trying to deflate output, but no zlib!");
#endif
		}
	}
	else
	{
		ostrEntity = new TempFileStream;
		ostrEntity->exceptions(std::ios::badbit);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::sendPostResponse(ostream* ostrEntity,
	TempFileStream& ostrError, OperationContext& context)
{
	int clen = -1;
	Int32 errCode = 0;
	String errDescr = "";
	if (!m_chunkedOut)
	{
		ostream* ostrToSend = ostrEntity;

		//
		// Set content-langage header here
		//
		bool clientSpecified, setByProvider;
		String clang = getContentLanguage(context, setByProvider,
			clientSpecified);
		if (setByProvider || clientSpecified)
		{
			addHeader("Content-Language", clang);
		}

		if (m_requestHandler && m_requestHandler->hasError(errCode, errDescr))
		{
			ostrToSend = &ostrError;
		}
		addHeader(m_respHeaderPrefix + "CIMOperation", "MethodResponse");
		TempFileStream* tfs = NULL;
		if ((tfs = dynamic_cast<TempFileStream*>(ostrToSend)))
		{
			clen = tfs->getSize();
		}
		if (m_deflateCompressionOut && tfs)
		{
			addHeader("Transfer-Encoding", "chunked");
			addHeader("Content-Encoding", "deflate");
			sendHeaders(m_resCode, -1);
		}
		else if (!m_requestHandler->getCIMError().empty())
		{
			addHeader(m_respHeaderPrefix + "CIMError",
				m_requestHandler->getCIMError());
		}
		else
		{
			sendHeaders(m_resCode, clen);
		}
		if (tfs)
		{
			if (clen > 0)
			{
				if (m_deflateCompressionOut)
				{
#ifdef OW_HAVE_ZLIB_H
					// gzip test
					HTTPChunkedOStream costr(m_ostr);
					HTTPDeflateOStream deflateostr(costr);
					deflateostr << tfs->rdbuf();
					deflateostr.termOutput();
					costr.termOutput(HTTPChunkedOStream::E_SEND_LAST_CHUNK);
#else
					OW_THROW(HTTPException, "Attempting to deflate response "
						" but we're not compiled with zlib!  (shouldn't happen)");
#endif // #ifdef OW_HAVE_ZLIB_H
				}
				else
				{
					m_ostr << tfs->rdbuf();
					if (!m_ostr)
					{
						OW_THROW(IOException, "Failed writing");
					}
				}
			}
		}
		m_ostr.flush();
	} // if !m_chunkedOut
	else // m_chunkedOut
	{
		HTTPChunkedOStream* ostrChunk = NULL;
		if (m_deflateCompressionOut)
		{
#ifdef OW_HAVE_ZLIB_H
			HTTPDeflateOStream* deflateostr = dynamic_cast<HTTPDeflateOStream*>
				(ostrEntity);
			deflateostr->termOutput();
			ostrChunk = dynamic_cast<HTTPChunkedOStream*>
				(&deflateostr->getOutputStreamOrig());
#else
			OW_THROW(HTTPException, "Attempting to deflate response "
				" but we're not compiled with zlib!  (shouldn't happen)");
#endif
		}
		else
		{
			ostrChunk = dynamic_cast<HTTPChunkedOStream*>
				(ostrEntity);
		}
		OW_ASSERT(ostrChunk);

		//
		// Add trailer for content-language
		//
		bool clientSpecified, setByProvider;
		String clang = getContentLanguage(context, setByProvider,
			clientSpecified);
		if (setByProvider || clientSpecified)
		{
			OW_LOGDEBUG(Format("HTTPSvrConnection::sendPostResponse (chunk)"
				" setting Content-Language to %1", clang).c_str());

			ostrChunk->addTrailer("Content-Language", clang);
		}

		if (m_requestHandler && m_requestHandler->hasError(errCode, errDescr))
		{
			const char* CIMStatusCodeTrailer = "CIMStatusCode";
			if (m_clientIsOpenWBEM2)
			{
				CIMStatusCodeTrailer = "CIMErrorCode";
			}
			const char* CIMStatusDescriptionTrailer = "CIMStatusDescription";
			if (m_clientIsOpenWBEM2)
			{
				CIMStatusDescriptionTrailer = "CIMErrorDescription";
			}

			ostrChunk->addTrailer(m_respHeaderPrefix + CIMStatusCodeTrailer,
				String(errCode));
			if (!errDescr.empty())
			{
				StringArray lines = errDescr.tokenize("\n");
				errDescr.erase();
				for (size_t i = 0; i < lines.size(); ++i)
				{
					errDescr += lines[i] + " ";
				}
				ostrChunk->addTrailer(m_respHeaderPrefix + CIMStatusDescriptionTrailer,
					errDescr);
			}
			if (!m_requestHandler->getCIMError().empty())
			{
				ostrChunk->addTrailer(m_respHeaderPrefix + "CIMError",
					m_requestHandler->getCIMError());
			}
			ostrChunk->termOutput(HTTPChunkedOStream::E_DISCARD_LAST_CHUNK);
		}
		else
		{
			ostrChunk->termOutput(HTTPChunkedOStream::E_SEND_LAST_CHUNK);
		}
	} // else m_chunkedOut
}
//////////////////////////////////////////////////////////////////////////////
int
HTTPSvrConnection::processRequestLine()
{
	switch (m_requestLine.size())
	{ // first check the request line to determine HTTP version
		case 2:
			m_httpVersion = HTTP_VER_10;
			break;
		case 3:
			if (m_requestLine[2].equalsIgnoreCase("HTTP/1.1"))
			{
				m_httpVersion = HTTP_VER_11;
			}
			else if (m_requestLine[2].equalsIgnoreCase("HTTP/1.0"))
			{
				m_httpVersion = HTTP_VER_10;
			}
			else
			{
				m_httpVersion = HTTP_VER_BAD;
				m_errDetails = "The HTTP protocol version " +
					m_requestLine[2] + " is not supported by this server.";
				return SC_HTTP_VERSION_NOT_SUPPORTED;
			}
			break;
		default:
			m_errDetails = "Invalid number of tokens on request line: " +
				String(static_cast<unsigned int>(m_requestLine.size()));
			return SC_BAD_REQUEST;
	}
	// check for a supported method.
	if (m_requestLine[0].equals("M-POST"))
	{
		m_method = M_POST;
	}
	else if (m_requestLine[0].equals("POST"))
	{
		m_method = POST;
	}
	else if (m_requestLine[0].equals("TRACE"))
	{
		m_method = TRACE;
	}
	else if (m_requestLine[0].equals("OPTIONS"))
	{
		m_method = OPTIONS;
	}
	else
	{
		m_method = BAD;
		m_errDetails = "Method not allowed by server: " + m_requestLine[0];
		return SC_METHOD_NOT_ALLOWED;
	}
	// make sure they're trying to hit the right resource
	/* TODO: Fix this with respect to listeners
	if (!m_requestLine[1].equalsIgnoreCase("/cimom") && m_method != OPTIONS)
	{
		m_errDetails = "Access to " + m_requestLine[1] +
			" is not allowed on this server.";
		return SC_FORBIDDEN;
	}
	*/
	return SC_OK;
}
//////////////////////////////////////////////////////////////////////////////
// This function may seem large and complex, but it is composed of many
// small, independent blocks.
int
HTTPSvrConnection::processHeaders(OperationContext& context)
{
//
// Check for Authentication
//
	// if m_options.allowAnonymous is true, we don't check.
	if (m_options.allowAnonymous == false)
	{
		if (!m_isAuthenticated)
		{
			m_isAuthenticated = false;
			try
			{
				if (performAuthentication(getHeaderValue("Authorization"), context) < 300 )
				{
					m_isAuthenticated = true;
				}
			}
			catch (AuthenticationException& e)
			{
				m_errDetails = e.getMessage();
				m_isAuthenticated = false;
				return SC_INTERNAL_SERVER_ERROR;
			}
			if (m_isAuthenticated == false)
			{
				return SC_UNAUTHORIZED;
			}
		}
		context.setStringData(OperationContext::USER_NAME, m_userName);
	}
//
// check for required headers with HTTP/1.1
//
	if (m_httpVersion == HTTP_VER_11)
	{
		if ( ! headerHasKey("Host"))
		{
			m_errDetails = "Your browser sent a request that this server could"
				"not understand.  "
				"Client sent HTTP/1.1 request without hostname "
				"(see RFC2068 section 9, and 14.23)";
			return SC_BAD_REQUEST;
		}
	}
//
// determine if connection is persistent.
//
	if (m_httpVersion != HTTP_VER_11)
	{
		m_isClose = true;	 // pre 1.1 version, no persistent connections.
		// TODO what's up with Keep-Alive in 1.0?
	}
	else
	{
		if (headerHasKey("Connection"))
		{
			if (getHeaderValue("Connection").equals("close"))
			{
				m_isClose = true;
			}
		}
	}
//
// determine content length or transfer-encoding.
//
	m_contentLength = -1;
	m_chunkedIn = false;
	if (headerHasKey("Transfer-Encoding"))
	{
		// If a Transfer-Encoding header field (section 14.41) is present and
		// has any value other than "identity", then the transfer-length is
		// defined by use of the "chunked" transfer-coding (section 3.6),
		// unless the message is terminated by closing the connection
		if (!getHeaderValue("Transfer-Encoding").equals("identity"))
		{
			m_contentLength = -1;
			m_chunkedIn = true;
		}
	}
	if (!m_chunkedIn)
	{
		// No chunking.  get the content-length.
		if (headerHasKey("Content-Length"))
		{
			String cLen = getHeaderValue("Content-Length");
			if (!cLen.empty())
			{
				m_contentLength = cLen.toInt64();
			}
		}
		// POST or M_POST, no chunking: test for content length
		// and send 411 if none.
		if (m_method == M_POST || m_method == POST)
		{
			if (m_contentLength < 0 && m_httpVersion == HTTP_VER_11)
			{
				m_errDetails = "No Content-Length or Transfer-Encoding"
					" was specified.";
				return SC_LENGTH_REQUIRED;
			}
		}
		// no entity allowed with a trace.
		else if (m_method == TRACE)
		{
			if (m_contentLength > 0 || m_chunkedIn)
			{
				m_errDetails = "An entity cannot be supplied with the TRACE "
					"method.";
				return SC_BAD_REQUEST;
			}
		}
	} // if (!m_chunkedIn)
//
// Check for content-encoding
//
	m_deflateCompressionIn = false;
	if (headerHasKey("Content-Encoding"))
	{
		String cc = getHeaderValue("Content-Encoding");
		if (cc.equalsIgnoreCase("deflate"))
		{
#ifdef OW_HAVE_ZLIB_H
			m_deflateCompressionIn = true;
			m_deflateCompressionOut = m_options.enableDeflate;
#else
			m_errDetails = "Content-Encoding \"deflate\" is not supported.  "
				"(CIMOM not compiled with zlib)";
			return SC_NOT_ACCEPTABLE;
#endif // #ifdef OW_HAVE_ZLIB_H
		}
		else if (!cc.equals("identity"))
		{
			m_errDetails = "Invalid Content-Encoding: " + cc
#ifdef OW_HAVE_ZLIB_H
				+ "  Only \"deflate\" is supported."
#endif
				;
			return SC_NOT_ACCEPTABLE;
		}
	}
//
// Check for correct Accept value
//
	if (m_method == POST || m_method == M_POST)
	{
		if (headerHasKey("Accept"))
		{
			String ac = getHeaderValue("Accept");
			if (ac.indexOf("text/xml") == String::npos
				&& ac.indexOf("application/xml") == String::npos)
			{
				m_errDetails = "Only entities of type \"text/xml\" or "
					"\"application/xml\" are supported.";
				return SC_NOT_ACCEPTABLE;
			}
		}
	}
//
// Check for Accept charset
//
	if (m_method == POST || m_method == M_POST)
	{
		if (headerHasKey("Accept-Charset"))
		{
			if (getHeaderValue("Accept-Charset").indexOf("utf-8") == String::npos)
			{
				m_errDetails = "Only the utf-8 charset is acceptable.";
				return SC_NOT_ACCEPTABLE;
			}
		}
	}
//
// Check for Accept-Encoding
//
	if (m_method == POST || m_method == M_POST)
	{
		if (headerHasKey("Accept-Encoding"))
		{
			if (getHeaderValue("Accept-Encoding").indexOf("deflate") != String::npos)
			{
#ifdef OW_HAVE_ZLIB_H
				m_deflateCompressionOut = m_options.enableDeflate;
#endif
			}
			
			// TODO I should really look to q != 0 after deflate as well...
			/*   // SNIA has Accept-Encoding with no "identity", so this is commented out...
			if (getHeaderValue("Accept-Encoding").indexOf("identity") < 0)
			{
				m_errDetails = "The \"identity\" encoding must be accepted.";
				return SC_NOT_ACCEPTABLE;
			}
			*/
		}
	}
//
// Check for TE header
//
	if (getHeaderValue("TE").indexOf("trailers") != String::npos)
	{
		// Now that trailers are standardized, we'll just reverse this test to
		// disable chunking/trailers for broken clients.

		// Trailers not standardized yet, so only do it we're talking to
		// ourselves.
		//if (getHeaderValue("User-Agent").indexOf(OW_PACKAGE) != String::npos)
		//{
		//	m_chunkedOut = true;
		//}
		if (getHeaderValue("User-Agent") == "RPT-HTTPClient/0.3-2") // SNIA browser says it can handle trailers when it really can't.
		{
			m_chunkedOut = false;
		}
		else
		{
			m_chunkedOut = true;
		}


		// now we need to see if the client is an 2.0.x version of OW which
		// supported a different (pre-standard) version of the trailers.
		if (getHeaderValue("User-Agent").startsWith("openwbem/2"))
		{
			m_clientIsOpenWBEM2 = true;
		}
	}
//
// Check for Accept-Language
//
	SessionLanguageRef psl(new SessionLanguage);
	if (headerHasKey("Accept-Language"))
	{
		String al = getHeaderValue("Accept-Language");
		if (al.length())
		{
			psl->assign(al.c_str());
		}
	}
	context.setData(SESSION_LANGUAGE_KEY, psl);

//
// Check for forbidden header keys.
//
	if (
		headerHasKey("Accept-Ranges")
		|| headerHasKey("Content-Range")
		|| headerHasKey("If-Range")
		|| headerHasKey("Range")
		|| headerHasKey("Accept-Ranges")
		)
	{
		m_errDetails = "Illegal header in request.  See: "
			"http://www.dmtf.org/cim/mapping/http/v1.0";
		return SC_NOT_ACCEPTABLE;
	}
//
// Content-Language
//
	// TODO
//
// Content-Type
//	
	if (m_method == M_POST || m_method == POST)
	{
		if (headerHasKey("Content-Type"))
		{
			String ct = getHeaderValue("Content-Type");
			// strip off the parameters from the content type
			ct = ct.substring(0, ct.indexOf(';'));
			
			// TODO: parse and handle the parameters we may possibly care about.
			m_requestHandler = m_options.env->getRequestHandler(ct);
			if (!m_requestHandler)
			{
				m_errDetails = Format("Content-Type \"%1\" is not supported.", ct);
				return SC_UNSUPPORTED_MEDIA_TYPE;
			}
		}
		else
		{
			m_errDetails = "A Content-Type must be specified";
			return SC_NOT_ACCEPTABLE;
		}
	}
//
// Check for "Man: " header and get ns value.
//
	if (m_method == M_POST)
	{
		if (headerHasKey("Man"))
		{
			String manLine = getHeaderValue("Man");
			if (manLine.indexOf("http://www.dmtf.org/cim/mapping/http/v1.0") == String::npos)
			{
				m_errDetails = "Unknown extension URI";
				return SC_NOT_EXTENDED;
			}
			size_t idx = manLine.indexOf(';');
			if (idx > 0 && idx != String::npos)
			{
				manLine = manLine.substring(idx + 0);
				idx = manLine.indexOf("ns");
				if (idx != String::npos)
				{
					idx = manLine.indexOf('=');
					if (idx > 0 && idx != String::npos)
					{
						m_reqHeaderPrefix = manLine.substring(idx + 1).trim();
					}
				}
			}
		} // if (m_requestHeaders.count("Man") > 0)
		else
		{
			m_errDetails = "Cannot use M-POST method with no Man: header.  See: "
				"http://www.ietf.org/rfc/rfc2774.txt";
			return SC_NOT_EXTENDED;
		}
	} // if (m_method == M_POST)
//
// Check for Custom OW_BypassLocker header
//
	if (headerHasKey(HTTPUtils::Header_BypassLocker))
	{
		if (getHeaderValue(HTTPUtils::Header_BypassLocker) == HTTPUtils::HeaderValue_true)
		{
			context.setStringData(OperationContext::BYPASS_LOCKERKEY, "true"); 
		}
	}

//
//
//
	return SC_OK;
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::trace()
{
	addHeader("TransferEncoding", "chunked");
	sendHeaders(m_resCode);
	HTTPChunkedOStream ostr(m_ostr);
	for (size_t i = 0; i < m_requestLine.size(); i++)
	{
		ostr << m_requestLine[i] << " ";
	}
	ostr << "\r\n";
	Map<String, String>::iterator iter;
	for (iter = m_requestHeaders.begin(); iter != m_requestHeaders.end(); iter++)
	{
		ostr << iter->first << ": " << iter->second << "\r\n" ;
	}
	ostr.termOutput(HTTPChunkedOStream::E_SEND_LAST_CHUNK);
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::post(istream& istr, OperationContext& context)
{
	ostream* ostrEntity = NULL;
	initRespStream(ostrEntity);
	try
	{
		OW_ASSERT(ostrEntity);
		TempFileStream ostrError(400);

	/*
		TempFileStream ltfs;
		ltfs << istr.rdbuf();
		ofstream ofstr("/tmp/HTTPSvrConnectionXMLDump", ios::app);
		ofstr << "************* New entity **** Size: " << ltfs.getSize() << " Should be: " << getHeaderValue("Content-Length") << endl;
		ofstr << ltfs.rdbuf();
		ltfs.rewind();
	*/
		m_requestHandler->setEnvironment(m_options.env);
		beginPostResponse();
		// process the request

		m_requestHandler->process(&istr, ostrEntity, &ostrError, context);
		sendPostResponse(ostrEntity, ostrError, context);
	}
	catch (...)
	{
#ifdef OW_HAVE_ZLIB_H
		HTTPDeflateOStream* deflateostr = dynamic_cast<HTTPDeflateOStream*>(ostrEntity);
		if (deflateostr)
		{
			ostrEntity = &deflateostr->getOutputStreamOrig();
			delete deflateostr;
		}
#endif // #ifdef OW_HAVE_ZLIB_H
		delete ostrEntity;
		throw;
	}

#ifdef OW_HAVE_ZLIB_H
	HTTPDeflateOStream* deflateostr = dynamic_cast<HTTPDeflateOStream*>(ostrEntity);
	if (deflateostr)
	{
		ostrEntity = &deflateostr->getOutputStreamOrig();
		delete deflateostr;
	}
#endif // #ifdef OW_HAVE_ZLIB_H
	delete ostrEntity;
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::options(OperationContext& context)
{
	addHeader("Allow","POST, M-POST, OPTIONS, TRACE");
#ifdef OW_HAVE_ZLIB_H
	if (m_options.enableDeflate)
	{
		addHeader("Accept-Encoding", "deflate");
	}
#endif
	String hp = HTTPUtils::getCounterStr();
	CIMFeatures cf;
	
	m_requestHandler = m_options.env->getRequestHandler("application/xml");
	if (!m_requestHandler)
	{
		OW_HTTP_THROW(HTTPException, "OPTIONS is only implemented for XML requests", SC_NOT_IMPLEMENTED);
	}
	m_requestHandler->setEnvironment(m_options.env);
	
	m_requestHandler->options(cf, context);
	
	addHeader("Opt", cf.extURL + " ; ns=" + hp);
	hp += "-";
	addHeader(hp + "CIMProtocolVersion", cf.protocolVersion);
	String headerKey;
	switch (cf.cimProduct)
	{
		case CIMFeatures::SERVER:
			if (cf.supportsBatch)
			{
				addHeader(hp + "CIMSupportsMultipleOperations", "");
			}
			headerKey = hp + "CIMSupportedFunctionalGroups";
			break;
		case CIMFeatures::LISTENER:
			if (cf.supportsBatch)
			{
				addHeader(hp + "CIMSupportsMultipleExports", "");
			}
			headerKey = hp + "CIMSupportedExportGroups";
			break;
		default:
			OW_ASSERT( "Attempting OPTIONS on a CIMProductIFC "
				"that is not a LISTENER or SERVER" == 0);
	}
	String headerVal;
	for (size_t i = 0; i < cf.supportedGroups.size(); i++)
	{
		headerVal += cf.supportedGroups[i];
		if (i < cf.supportedGroups.size() - 1)
		{
			headerVal += ", ";
		}
	}
	addHeader(headerKey, headerVal);
	if (!cf.cimom.empty())
	{
		addHeader(hp + "CIMOM", cf.cimom);
	}
	if (!cf.validation.empty())
	{
		addHeader(hp + "CIMValidation", cf.validation);
	}
	if (cf.supportedQueryLanguages.size() > 0)
	{
		headerVal.erase();
		for (size_t i = 0; i < cf.supportedQueryLanguages.size(); i++)
		{
			headerVal += cf.supportedQueryLanguages[i];
			if (i < cf.supportedQueryLanguages.size() - 1)
			{
				headerVal += ", ";
			}
		}
		addHeader(hp + "CIMSupportedQueryLanguages", headerVal);
	}
	sendHeaders(m_resCode);
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::sendError(int resCode)
{
	if (!m_ostr)
	{
		// connection closed, bail out
		return;
	}
	if (m_socket.receiveTimeOutExpired())
	{
		resCode = SC_REQUEST_TIMEOUT;
		m_errDetails = "Timeout waiting for request.";
	}
	else if (Socket::gotShutDown())
	{
		resCode = SC_SERVICE_UNAVAILABLE;
		m_errDetails = "The server is shutting down.  Please try "
			"again later.";
	}
	String resMessage = HTTPUtils::status2String(resCode) +
		": " + m_errDetails;
	String reqProtocol;
	if (m_httpVersion == HTTP_VER_11)
	{
		reqProtocol = "HTTP/1.1";
	}
	else
	{
		reqProtocol = "HTTP/1.0";
	}
	m_ostr << reqProtocol << " " << resCode << " " << resMessage << "\r\n";
	// TODO more headers (date and such)
	addHeader("Connection", "close");
	addHeader("Content-Length", "0");
	//addHeader("Content-Length",
	//	String(tmpOstr.length()));
	//addHeader("Content-Type", "text/html");
	for (size_t i = 0; i < m_responseHeaders.size(); i++)
	{
		m_ostr << m_responseHeaders[i] << "\r\n";
	}
	m_ostr << "\r\n";
	m_ostr.flush();
}
//////////////////////////////////////////////////////////////////////////////
int
HTTPSvrConnection::performAuthentication(const String& info, OperationContext& context)
{
	if (m_pHTTPServer->authenticate(this, m_userName, info, context))
	{
		return SC_OK;
	}
	else
	{
		return SC_UNAUTHORIZED;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::sendHeaders(int sc, int len)
{
	if (len >= 0)
	{
		addHeader("Content-Length",
			String(len));
	}
	m_ostr << "HTTP/1.1 " << sc << " " << HTTPUtils::status2String(sc) <<
		"\r\n";
	for (size_t i = 0; i < m_responseHeaders.size(); i++)
	{
		m_ostr << m_responseHeaders[i] << "\r\n";
	}
	m_ostr << "\r\n";
}
//////////////////////////////////////////////////////////////////////////////
String
HTTPSvrConnection::getHostName()
{
	//return m_socket.getLocalAddress().getName();
	return SocketAddress::getAnyLocalHost().getName();
}
//////////////////////////////////////////////////////////////////////////////
CIMProtocolIStreamIFCRef
HTTPSvrConnection::convertToFiniteStream(istream& istr)
{
	CIMProtocolIStreamIFCRef rval(0);
	if (m_chunkedIn)
	{
		rval = new HTTPChunkedIStream(istr);
	}
	else if (m_contentLength > 0)
	{
		rval = new HTTPLenLimitIStream(istr, m_contentLength);
	}
	else
	{
		return rval;
	}
	if (m_deflateCompressionIn)
	{
#ifdef OW_HAVE_ZLIB_H
		rval = new HTTPDeflateIStream(rval);
#else
		OW_THROW(HTTPException, "Attempting to deflate request, but "
				"we're not linked with zlib!  (shouldn't happen)");
#endif // #ifdef OW_HAVE_ZLIB_H
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
String 
HTTPSvrConnection::getContentLanguage(OperationContext& context,
	bool& setByProvider, bool& clientSpecified)
{
	setByProvider = false;
	clientSpecified = false;
	String contentLang = m_options.defaultContentLanguage;

	OperationContext::DataRef dataref = context.getData(
		SESSION_LANGUAGE_KEY);
	if (!dataref)
	{
		return contentLang;
	}

	SessionLanguageRef slref = dataref.cast_to<SessionLanguage>();
	if (!slref)
	{
		return contentLang;
	}

	if (slref->langCount() > 0)
	{
		clientSpecified = true;	// Client specified accept-language
	}
	String pcl = slref->getContentLanguage();
	if (pcl.length())
	{
		contentLang = pcl;
		setByProvider = true;
	}

	return contentLang;
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPSvrConnection::doCooperativeCancel()
{
	m_socket.disconnect();
}

} // end namespace OpenWBEM

