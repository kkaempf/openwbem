/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
#include "OW_DateTime.hpp"
#include "OW_Assertion.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_HTTPException.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_StringBuffer.hpp"

using std::ios;
using std::istream;
using std::ostream;
using std::flush;

#ifndef SOCKET_TIMEOUT
#define SOCKET_TIMEOUT 300
#endif


#ifndef OW_LOGDEBUG
#define OW_LOGDEBUG(x) m_options.env->getLogger()->logDebug(x)
#endif

#ifndef OW_LOGERROR
#define OW_LOGERROR(x) m_options.env->getLogger()->logError(x)
#endif

#ifndef OW_LOGCUSTINFO
#define OW_LOGCUSTINFO(x) m_options.env->getLogger()->logCustInfo(x)
#endif



//////////////////////////////////////////////////////////////////////////////
OW_HTTPSvrConnection::OW_HTTPSvrConnection(OW_Socket socket,
	OW_HTTPServer* htin,
	OW_Reference<OW_UnnamedPipe>& upipe,
	const OW_HTTPServer::Options& opts)
	: OW_Runnable()
	, m_requestLine(), m_requestHeaders(), m_pHTTPServer(htin)
	, m_socket(socket), m_ostr(socket.getOutputStream())
	, m_resCode(SC_OK), m_needSendError(false)
	, m_responseHeaders(), m_httpVersion(HTTP_VER_BAD)
	, m_method(BAD), m_istr(socket.getInputStream())
	, m_isClose(false)
	, m_contentLength(-1)
	, m_chunkedIn(false)
	, m_deflateCompressionIn(false)
	, m_deflateCompressionOut(false)
	, m_errDetails(), m_reqHeaderPrefix()
	, m_respHeaderPrefix()
	, m_isAuthenticated(false)
	, m_upipe(upipe)
	, m_chunkedOut(false)
	, m_userName()
	, m_requestHandler()
	, m_options(opts)
{
	m_socket.setTimeouts(SOCKET_TIMEOUT);
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
OW_HTTPSvrConnection::~OW_HTTPSvrConnection()
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
OW_HTTPSvrConnection::run()
{
	OW_Reference<OW_CIMProtocolIStreamIFC> istrToReadFrom(0);
	OW_SelectTypeArray selArray;

	selArray.push_back(m_upipe->getSelectObj());
	selArray.push_back(m_socket.getSelectObj());

	try
	{
		m_isAuthenticated = false;
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
				OW_HTTPUtils::date());
			addHeader("Cache-Control",
				"no-cache");
			addHeader("Server",
				"OpenWBEM/" OW_VERSION " (CIMOM)");

			int selType = OW_Select::select(selArray, SOCKET_TIMEOUT * 1000);
			if(selType == OW_Select::OW_SELECT_ERROR)
			{
			   OW_THROW(OW_SocketException, "Error occurred during select()");
			}
			if(selType == OW_Select::OW_SELECT_INTERRUPTED)
			{
			   OW_THROW(OW_SocketException, "select() was interrupted.");
			}

			if(selType == OW_Select::OW_SELECT_TIMEOUT)
			{
			   m_resCode = SC_REQUEST_TIMEOUT;
			   m_errDetails = "Timeout waiting for request.";
			   sendError(m_resCode);
			   return;
			}

			if(selType == 0)	// Unnamped pipe selected
			{
			   m_resCode = SC_SERVICE_UNAVAILABLE;
			   m_errDetails = "Server is shutting down."
				   "  Please try again later.";
			   sendError(m_resCode);
			   return;
			}

			if(selType != 1)	// If this isn't the socket - something is wrong
			{
			   OW_THROW(OW_Assertion, "Unexpected return code from select");
			}

			if (!OW_HTTPUtils::parseHeader(m_requestHeaders, m_requestLine, m_istr))
			{
				if (OW_Socket::gotShutDown())
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

			m_resCode = processHeaders();
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
					if (istrToReadFrom.isNull())
					{
						OW_THROW(OW_HTTPException,
							"POST, but no content-length or chunking");
					}
					post(*istrToReadFrom);
					break;
				case OPTIONS:
					options();
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

			if(m_isClose)
			{
				break;
			}
		} // while(m_istr.good())
	} // try
	catch (OW_CIMErrorException& cee)
	{
		addHeader("CIMError", cee.getMessage());
		if (m_errDetails.empty())
		{
			m_errDetails = OW_String("CIMError: ") + cee.getMessage();
		}
		cleanUpIStreams(istrToReadFrom);
		OW_String errMsg(cee.getMessage());
		if (errMsg == OW_CIMErrorException::unsupported_protocol_version ||
			errMsg == OW_CIMErrorException::multiple_requests_unsupported ||
			errMsg == OW_CIMErrorException::unsupported_cim_version ||
			errMsg == OW_CIMErrorException::unsupported_dtd_version)
		{
			sendError(SC_NOT_IMPLEMENTED);
		}
		else
		{
			sendError(SC_BAD_REQUEST);
		}
	}
	catch (OW_Assertion& a)
	{
		OW_LOGERROR(OW_Format("Caught ASSERTION in OW_HTTPSvrConnection::run: %1",
			a));
		m_errDetails = a.getMessage();
		cleanUpIStreams(istrToReadFrom);
		sendError(SC_INTERNAL_SERVER_ERROR);
	}
	catch (OW_Exception& e)
	{
		OW_LOGERROR(format("%1", e));
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
		m_errDetails = format("Caught std::exception (%1) in OW_HTTPSvrConnection::run()", e.what());
		OW_LOGERROR(m_errDetails);
		cleanUpIStreams(istrToReadFrom);
		sendError(SC_INTERNAL_SERVER_ERROR);
	}
	catch (...)
	{
		OW_LOGERROR("Got Unknown Exception in OW_HTTPSvrConnection::run()");
		m_errDetails = "HTTP Server caught unknown exception";
		cleanUpIStreams(istrToReadFrom);
		sendError(SC_INTERNAL_SERVER_ERROR);
	}
	//m_socket.disconnect();
}

void
OW_HTTPSvrConnection::cleanUpIStreams(OW_Reference<OW_CIMProtocolIStreamIFC> istr)
{
	if (!istr.isNull())
	{
		OW_HTTPUtils::eatEntity(*istr);
	}
}

void
OW_HTTPSvrConnection::beginPostResponse()
{
	m_respHeaderPrefix = OW_HTTPUtils::getCounterStr();
	addHeader(
		"Content-Type","application/xml; charset=\"utf-8\"");
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

	if (m_chunkedOut)
	{
		addHeader( "Transfer-Encoding", "chunked");
		addHeader(m_respHeaderPrefix + "CIMOperation", "MethodResponse");
		addHeader("Trailers",
			m_respHeaderPrefix + "CIMError, "
			+ m_respHeaderPrefix + "CIMErrorCode, "
			+ m_respHeaderPrefix + "CIMErrorDescription");
		sendHeaders(m_resCode);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPSvrConnection::initRespStream(ostream*& ostrEntity)
{
	if (m_chunkedOut)
	{
		ostrEntity = new OW_HTTPChunkedOStream(m_ostr);
		ostrEntity->exceptions(std::ios::badbit);
		if (m_deflateCompressionOut)
		{
#ifdef OW_HAVE_ZLIB_H
			ostrEntity = new OW_HTTPDeflateOStream(*ostrEntity);
			ostrEntity->exceptions(std::ios::badbit);
#else
			OW_THROW(OW_HTTPException, "Trying to deflate output, but no zlib!");
#endif
		}
	}
	else
	{
		ostrEntity = new OW_TempFileStream;
		ostrEntity->exceptions(std::ios::badbit);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPSvrConnection::sendPostResponse(ostream* ostrEntity,
	OW_TempFileStream& ostrError)
{
	int clen = -1;
	OW_Int32 errCode = 0;
	OW_String errDescr = "";
	if (!m_chunkedOut)
	{
		ostream* ostrToSend = ostrEntity;
		if (m_requestHandler && m_requestHandler->hasError(errCode, errDescr))
		{
			ostrToSend = &ostrError;
		}

		addHeader(m_respHeaderPrefix + "CIMOperation", "MethodResponse");

		OW_TempFileStream* tfs = NULL;
		if ((tfs = dynamic_cast<OW_TempFileStream*>(ostrToSend)))
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
					OW_HTTPChunkedOStream costr(m_ostr);
					OW_HTTPDeflateOStream deflateostr(costr);
					deflateostr << tfs->rdbuf();
					deflateostr.termOutput();
					costr.termOutput();
#else
					OW_THROW(OW_HTTPException, "Attempting to deflate response "
						" but we're not compiled with zlib!  (shouldn't happen)");
#endif // #ifdef OW_HAVE_ZLIB_H
				}
				else
				{
					m_ostr << tfs->rdbuf();
					if (!m_ostr)
					{
						OW_THROW(OW_IOException, "Failed writing");
					}
				}
			}
		}
		m_ostr.flush();
	} // if !m_chunkedOut
	else // m_chunkedOut
	{
		OW_HTTPChunkedOStream* ostrChunk = NULL;
		if (m_deflateCompressionOut)
		{
#ifdef OW_HAVE_ZLIB_H
			OW_HTTPDeflateOStream* deflateostr = dynamic_cast<OW_HTTPDeflateOStream*>
				(ostrEntity);
			deflateostr->termOutput();
			ostrChunk = dynamic_cast<OW_HTTPChunkedOStream*>
				(&deflateostr->getOutputStreamOrig());
#else
			OW_THROW(OW_HTTPException, "Attempting to deflate response "
				" but we're not compiled with zlib!  (shouldn't happen)");
#endif
		}
		else
		{
			ostrChunk = dynamic_cast<OW_HTTPChunkedOStream*>
				(ostrEntity);
		}

		OW_ASSERT(ostrChunk);
		if (m_requestHandler && m_requestHandler->hasError(errCode, errDescr))
		{
			ostrChunk->addTrailer(m_respHeaderPrefix + "CIMErrorCode",
				OW_String(errCode));
			if (!errDescr.empty())
			{
				OW_StringArray lines = errDescr.tokenize("\n");
				errDescr.erase();
				for (size_t i = 0; i < lines.size(); ++i)
				{
					errDescr += lines[i] + " ";
				}
				ostrChunk->addTrailer(m_respHeaderPrefix + "CIMErrorDescription",
					errDescr);
			}
			if (!m_requestHandler->getCIMError().empty())
			{
				ostrChunk->addTrailer(m_respHeaderPrefix + "CIMError",
					m_requestHandler->getCIMError());
			}
		}
		ostrChunk->termOutput();
	} // else m_chunkedOut
}

//////////////////////////////////////////////////////////////////////////////
int
OW_HTTPSvrConnection::processRequestLine()
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
				OW_String(static_cast<unsigned int>(m_requestLine.size()));
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
OW_HTTPSvrConnection::processHeaders()
{
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
				"(see RFC2068 section 9, and 14.23";
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
			OW_String cLen = getHeaderValue("Content-Length");
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
		OW_String cc = getHeaderValue("Content-Encoding");
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
			OW_String ac = getHeaderValue("Accept");
			if (ac.indexOf("text/xml") == OW_String::npos 
				&& ac.indexOf("application/xml") == OW_String::npos)
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
			if (getHeaderValue("Accept-Charset").indexOf("utf-8") == OW_String::npos)
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

			if (getHeaderValue("Accept-Encoding").indexOf("deflate") != OW_String::npos)
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

	if (getHeaderValue("TE").indexOf("trailers") != OW_String::npos)
	{
		// Trailers not standardized yet, so only do it we're talking to
		// ourselves.
		if (getHeaderValue("User-Agent").indexOf(OW_PACKAGE) != OW_String::npos)
		{
			m_chunkedOut = true;
		}
	}

//
// Check for Accept-Languages
//
	// TODO

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
			OW_String ct = getHeaderValue("Content-Type");
			// strip off the parameters from the content type
			ct = ct.substring(0, ct.indexOf(';'));
			
			// TODO: parse and handle the parameters we may possibly care about.

			m_requestHandler = m_options.env->getRequestHandler(ct);
			if (m_requestHandler.isNull())
			{
				m_errDetails = format("Content-Type \"%1\" is not supported.", ct);
				return SC_UNSUPPORTED_MEDIA_TYPE;
			}

		}
		else
		{
			m_errDetails = "A Content-Type must be specified";
			return SC_NOT_ACCEPTABLE;
		}
	}

	/*
	// set the path for the handler
	if (m_requestLine[1].equalsIgnoreCase("/" OW_BINARY_ID))
	{
		m_options.env->getLogger()->logDebug("Using binary request handler");
		m_requestHandler = m_options.env->getRequestHandler(OW_BINARY_ID);
	}
	else
	{
		m_options.env->getLogger()->logDebug("Using CIM/XML request handler");
		m_requestHandler = m_options.env->getRequestHandler(OW_CIMXML_ID);
	}
	if (!m_requestHandler)
	{
		OW_HTTP_THROW(OW_HTTPException, "Invalid Path", SC_NOT_FOUND);
	}
	*/

//
// Check for "Man: " header and get ns value.
//
	if (m_method == M_POST)
	{
		if (headerHasKey("Man"))
		{
			OW_String manLine = getHeaderValue("Man");
			if (manLine.indexOf("http://www.dmtf.org/cim/mapping/http/v1.0") == OW_String::npos)
			{
				m_errDetails = "Unknown extension URI";
				return SC_NOT_EXTENDED;
			}
			size_t idx = manLine.indexOf(';');
			if (idx > 0 && idx != OW_String::npos)
			{
				manLine = manLine.substring(idx + 0);
				idx = manLine.indexOf("ns");
				if (idx != OW_String::npos)
				{
					idx = manLine.indexOf('=');
					if (idx > 0 && idx != OW_String::npos)
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
				if (performAuthentication(getHeaderValue("Authorization")) < 300 )
						m_isAuthenticated = true;

			}
			catch (OW_AuthenticationException& e)
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
	}

//
//
//

	return SC_OK;
}


//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPSvrConnection::trace()
{
	addHeader("TransferEncoding", "chunked");
	sendHeaders(m_resCode);
	OW_HTTPChunkedOStream ostr(m_ostr);
	for (size_t i = 0; i < m_requestLine.size(); i++)
	{
		ostr << m_requestLine[i] << " ";
	}
	ostr << "\r\n";
	OW_Map<OW_String, OW_String>::iterator iter;
	for (iter = m_requestHeaders.begin(); iter != m_requestHeaders.end(); iter++)
	{
		ostr << iter->first << ": " << iter->second << "\r\n" ;
	}
	ostr.termOutput();
}

//////////////////////////////////////////////////////////////////////////////
/*
namespace
{

class PathWrapperEnv : public OW_ServiceEnvironmentIFC
{
public:
	PathWrapperEnv(const OW_ServiceEnvironmentIFCRef& toWrap)
	{
		m_wrapped = toWrap;
	}
	
	virtual OW_CIMOMHandleIFCRef getCIMOMHandle(const OW_String &username, const OW_Bool doIndications)
	{
		return m_wrapped->getCIMOMHandle(username, doIndications);
	}
	
	virtual OW_LoggerRef getLogger() const
	{
		return m_wrapped->getLogger();
	}
	
	virtual OW_String getConfigItem(const OW_String &name) const
	{
		if (name == OW_ConfigOpts::HTTP_PATH_opt)
		{
			return m_path;
		}
		else
		{
			return m_wrapped->getConfigItem(name);
		}
	}
	
	virtual void setConfigItem( const OW_String& item,
				const OW_String& value, OW_Bool overwritePrevious = true )
	{
		if (item == OW_ConfigOpts::HTTP_PATH_opt)
		{
			m_path = value;
		}
		else
		{
			m_wrapped->setConfigItem(item, value, overwritePrevious);
		}
	}

	virtual void addSelectable(OW_SelectableIFCRef obj, OW_SelectableCallbackIFCRef cb)
	{
		m_wrapped->addSelectable(obj, cb);
	}
	
	virtual void removeSelectable(OW_SelectableIFCRef obj, OW_SelectableCallbackIFCRef cb)
	{
		m_wrapped->removeSelectable(obj,cb);
	}
	
	virtual OW_RequestHandlerIFCRef getRequestHandler(const OW_String &id) const
	{
		return m_wrapped->getRequestHandler(id);
	}
	
	virtual OW_Bool authenticate(OW_String &userName, const OW_String &info, OW_String &details)
	{
		return m_wrapped->authenticate(userName,info,details);
	}

private:
	OW_ServiceEnvironmentIFCRef m_wrapped;
	OW_String m_path;
};

}
*/
//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPSvrConnection::post(istream& istr)
{
	ostream* ostrEntity = NULL;
	OW_TempFileStream ostrError(400);

	initRespStream(ostrEntity);
	OW_ASSERT(ostrEntity);

					
/*
	OW_TempFileStream ltfs;
	ltfs << istr.rdbuf();
	ofstream ofstr("/tmp/HTTPSvrConnectionXMLDump", ios::app);
	ofstr << "************* New entity **** Size: " << ltfs.getSize() << " Should be: " << getHeaderValue("Content-Length") << endl;
	ofstr << ltfs.rdbuf();
	ltfs.rewind();
*/


	m_requestHandler->setEnvironment(m_options.env);

	beginPostResponse();

	// process the request
	OW_SortedVectorMap<OW_String, OW_String> handlerVars;
	handlerVars[OW_ConfigOpts::HTTP_PATH_opt] = m_requestLine[1];
	handlerVars[OW_ConfigOpts::USER_NAME_opt] = m_userName;
	m_requestHandler->process(&istr, ostrEntity, &ostrError, handlerVars);

	sendPostResponse(ostrEntity, ostrError);


#ifdef OW_HAVE_ZLIB_H
	OW_HTTPDeflateOStream* deflateostr = dynamic_cast<OW_HTTPDeflateOStream*>(ostrEntity);
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
OW_HTTPSvrConnection::options()
{

	addHeader("Allow","POST, M-POST, OPTIONS, TRACE");
	OW_String hp = OW_HTTPUtils::getCounterStr();
	OW_CIMFeatures cf;
	
	m_requestHandler = m_options.env->getRequestHandler("application/xml");
	if (!m_requestHandler)
	{
		OW_HTTP_THROW(OW_HTTPException, "OPTIONS is only implemented for XML requests", SC_NOT_IMPLEMENTED);
	}

	m_requestHandler->setEnvironment(m_options.env);

	OW_SortedVectorMap<OW_String, OW_String> handlerVars;
	handlerVars[OW_ConfigOpts::HTTP_PATH_opt] = m_requestLine[1];
	handlerVars[OW_ConfigOpts::USER_NAME_opt] = m_userName;
	
	m_requestHandler->options(cf, handlerVars);
	
	addHeader("Opt", cf.extURL + " ; ns=" + hp);
	hp += "-";
	addHeader(hp + "CIMProtocolVersion", cf.protocolVersion);
	OW_String headerKey;

	switch (cf.cimProduct)
	{
		case OW_CIMFeatures::SERVER:
			if (cf.supportsBatch)
			{
				addHeader(hp + "CIMSupportsMultipleOperations", "");
			}
			headerKey = hp + "CIMSupportedFunctionalGroups";
			break;
		case OW_CIMFeatures::LISTENER:
			if (cf.supportsBatch)
			{
				addHeader(hp + "CIMSupportsMultipleExports", "");
			}
			headerKey = hp + "CIMSupportedExportGroups";
			break;
		default:
			OW_THROW(OW_Exception, "Attempting OPTIONS on a CIMProductIFC "
				"that is not a LISTENER or SERVER");
	}
	OW_String headerVal;
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
OW_HTTPSvrConnection::sendError(int resCode)	/*throw (OW_IOException)*/
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
	else if (OW_Socket::gotShutDown())
	{
		resCode = SC_SERVICE_UNAVAILABLE;
		m_errDetails = "The server is shutting down.  Please try "
			"again later.";
	}
	OW_String resMessage = OW_HTTPUtils::status2String(resCode) +
		": " + m_errDetails;
	OW_String reqProtocol;
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
	//	OW_String(tmpOstr.length()));
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
OW_HTTPSvrConnection::performAuthentication(const OW_String& info)
{
	if(m_pHTTPServer->authenticate(this, m_userName, info))
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
OW_HTTPSvrConnection::sendHeaders(int sc, int len)
{
	if (len >= 0)
	{
		addHeader("Content-Length",
			OW_String(len));
	}
	m_ostr << "HTTP/1.1 " << sc << " " << OW_HTTPUtils::status2String(sc) <<
		"\r\n";
	for (size_t i = 0; i < m_responseHeaders.size(); i++)
	{
		m_ostr << m_responseHeaders[i] << "\r\n";
	}
	m_ostr << "\r\n";
}


//////////////////////////////////////////////////////////////////////////////
OW_String
OW_HTTPSvrConnection::getHostName()
{
	//return m_socket.getLocalAddress().getName();
	return OW_SocketAddress::getAnyLocalHost().getName();
}

//////////////////////////////////////////////////////////////////////////////
OW_Reference<OW_CIMProtocolIStreamIFC>
OW_HTTPSvrConnection::convertToFiniteStream(istream& istr)
{
	OW_Reference<OW_CIMProtocolIStreamIFC> rval(0);
	if (m_chunkedIn)
	{
		rval = new OW_HTTPChunkedIStream(istr);
	}
	else if (m_contentLength > 0)
	{
		rval = new OW_HTTPLenLimitIStream(istr, m_contentLength);
	}
	else
	{
		return rval;
	}

	if (m_deflateCompressionIn)
	{
#ifdef OW_HAVE_ZLIB_H
		rval = new OW_HTTPDeflateIStream(rval);
#else
		OW_THROW(OW_HTTPException, "Attempting to deflate request, but "
				"we're not linked with zlib!  (shouldn't happen)");
#endif // #ifdef OW_HAVE_ZLIB_H
	}
	return rval;
}
