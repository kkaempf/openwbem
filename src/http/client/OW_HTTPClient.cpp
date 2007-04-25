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
#include "OW_HTTPClient.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_HTTPDeflateOStream.hpp"
#include "OW_HTTPDeflateIStream.hpp"
#include "OW_HTTPLenLimitIStream.hpp"
#include "OW_Format.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_Assertion.hpp"
#include "OW_SecureRand.hpp"
#include "OW_HTTPException.hpp"
#include "OW_UserUtils.hpp"
#include "OW_Select.hpp"
#include "OW_SSLCtxMgr.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMException.hpp"

#include <fstream>
#include <cerrno>

// stupid AIX has a macro named GOOD.
#ifdef GOOD
#undef GOOD
#endif

using namespace std;

namespace OW_NAMESPACE
{

using std::flush;
using std::istream;
//////////////////////////////////////////////////////////////////////////////
HTTPClient::HTTPClient( const String &sURL, const SSLClientCtxRef& sslCtx)
#ifndef OW_DISABLE_DIGEST
	: m_iDigestNonceCount(1) ,
#else
	:
#endif
	 m_url(sURL)
	, m_pIstrReturn(0)
	, m_sslCtx(sslCtx)
	, m_socket(
		m_url.scheme.endsWith('s') ?
		(
			m_sslCtx ?
			(
				m_sslCtx
			)
			:
			(
				m_sslCtx = SSLClientCtxRef(new SSLClientCtx()), m_sslCtx
			)
		)
		:
		(
			SSLClientCtxRef(0)
		)
	)
	, m_requestMethod("M-POST"), m_authRequired(false)
	, m_istr(m_socket.getInputStream()), m_ostr(m_socket.getOutputStream())
	, m_doDeflateOut(false)
	, m_retryCount(0)
	, m_httpPath("/cimom")
	, m_uselocalAuthentication(false)
	, m_closeConnection(false)
	, m_statusCode(-1)
{
	// turn off exceptions, since we're not coded to handle them.
	m_istr.exceptions(std::ios::goodbit);
	m_ostr.exceptions(std::ios::goodbit);

#ifndef OW_WIN32
	// TODO: figure out a better way to do this.
	signal(SIGPIPE, SIG_IGN);
#endif

	setUrl();
	addHeaderPersistent("Host", m_url.host);
	addHeaderPersistent("User-Agent", OW_PACKAGE"/"OW_VERSION);

	m_socket.setConnectTimeout(Timeout::relative(60));
	m_socket.setReceiveTimeout(Timeout::relative(600));
	m_socket.setSendTimeout(Timeout::relative(600));
}
//////////////////////////////////////////////////////////////////////////////
HTTPClient::~HTTPClient()
{
	try
	{
		cleanUpIStreams();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::cleanUpIStreams()
{
	// if there remains bytes from last response, eat them.
	if (m_pIstrReturn)
	{
		HTTPUtils::eatEntity(*m_pIstrReturn);
		m_pIstrReturn = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	inline bool isUInt16(const String& s)
	{
		try
		{
			s.toUInt16();
			return true;
		}
		catch (StringConversionException& e)
		{
			return false;
		}
	}

	String getAuthParam(const String& paramName, const String& authInfo)
	{
		String rval;
		size_t pos = authInfo.indexOf(paramName);
		if (pos == String::npos)
		{
			return rval;
		}

		pos += paramName.length();
		if ((pos = authInfo.indexOf('=', pos)) == String::npos)
		{
			return rval;
		}

		if (pos + 1 >= authInfo.length())
		{
			return rval;
		}
		++pos; // move past =

		if (authInfo[pos] == '"')
		{
			size_t endPos = authInfo.indexOf('"', pos + 1);
			if (endPos != String::npos)
			{
				rval = authInfo.substring(pos + 1, endPos - pos - 1); // don't get the quotes.
			}
		}
		else
		{
			size_t endPos = authInfo.indexOf(',', pos);
			if (endPos != String::npos)
			{
				rval = authInfo.substring(pos, endPos - pos);
			}
			else
			{
				rval = authInfo.substring(pos);
			}
		}
		return rval;
	}
}
//////////////////////////////////////////////////////////////////////////////
void HTTPClient::setUrl()
{
	if (m_url.scheme.empty())
	{
		m_url.scheme = "http";
	}
	if (m_url.port.empty())
	{
		if ( m_url.scheme.endsWith('s') ) // https, cimxml.wbems, owbinary.wbems
		{
			m_url.port = "5989";
		}
		else // http, cimxml.wbem, owbinary.wbem
		{
			m_url.port = "5988";
		}
	}
	if (m_url.scheme.endsWith('s'))
	{
#ifdef OW_NO_SSL
		OW_THROW(SocketException, "SSL not available");
#endif // #ifdef OW_NO_SSL
	}
	if (m_url.port.equalsIgnoreCase(URL::OWIPC)
		|| m_url.scheme.equals("ipc")) // the ipc:// scheme is deprecated in 3.0.0 and will be removed!
	{
#ifdef OW_WIN32
		OW_THROW(SocketException, "IPC Method not currently available on Win32");
#else
		m_serverAddress = SocketAddress::getUDS(OW_DOMAIN_SOCKET_NAME);
#endif
	}
	else if (isUInt16(m_url.port))
	{
		m_serverAddress = SocketAddress::getByName(HTTPUtils::unescapeForURL(m_url.host),
			m_url.port.toUInt16());
	}
	else // port may be a path to the UDS
	{
#ifdef OW_WIN32
		OW_THROW(SocketException, "IPC Method not currently available on Win32");
#else
		m_serverAddress = SocketAddress::getUDS(HTTPUtils::unescapeForURL(m_url.port));
#endif
	}

	bool hostIsLocal = false;
	if (m_url.host == "localhost" || m_url.host == "127.0.0.1")
	{
		hostIsLocal = true;
	}
#ifdef OW_HAVE_IPV6
	// check local address IPV6 format
	if (m_url.host == "::1" || m_url.host == "::ffff:127.0.0.1" || m_url.host == "::127.0.0.1" )
	{
		hostIsLocal = true;
	}
#endif

	if (hostIsLocal && m_url.principal.empty() && m_url.credential.empty())
	{
		m_uselocalAuthentication = true;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::receiveAuthentication()
{
	String authInfo = getHeaderValue("www-authenticate");
	String scheme; 
	if (!authInfo.empty())
	{
		scheme = authInfo.tokenize()[0]; 
		scheme.toLowerCase(); 
	}
	m_sRealm = getAuthParam("realm", authInfo);
#ifndef OW_DISABLE_DIGEST
	m_sDigestCNonce.format(
		"%08x%08x%08x%08x%08x",
		Secure::rand_uint<UInt32>(), Secure::rand_uint<UInt32>(),
		Secure::rand_uint<UInt32>(), Secure::rand_uint<UInt32>(),
		Secure::rand_uint<UInt32>()
	);
	
	if (headerHasKey("authentication-info") && m_sAuthorization=="Digest" )
	{
		String authInfo = getHeaderValue("authentication-info");
		m_sDigestNonce = getAuthParam("nextnonce", authInfo);
		getCredentialsIfNecessary();
		HTTPUtils::DigestCalcHA1( "md5", m_url.principal, m_sRealm,
			m_url.credential, m_sDigestNonce, m_sDigestCNonce, m_sDigestSessionKey );
		m_iDigestNonceCount = 1;
	}
	else if ( scheme.equals("digest"))
	{
		m_sAuthorization = "Digest";
		m_uselocalAuthentication = false;
		m_sDigestNonce = getAuthParam("nonce", authInfo);
		getCredentialsIfNecessary();
		HTTPUtils::DigestCalcHA1( "md5", m_url.principal, m_sRealm,
			m_url.credential, m_sDigestNonce, m_sDigestCNonce, m_sDigestSessionKey );
	}
	else
#endif
	if ( scheme.equals("basic"))
	{
		m_sAuthorization = "Basic";
		m_uselocalAuthentication = false;
	}
	else if (m_spnegoHandler && authInfo.indexOf("Negotiate") != String::npos)
	{
		OW_ASSERT(m_spnegoData != "");
		m_sAuthorization = "Negotiate";
		m_uselocalAuthentication = false;
	}
	// note this else has to be last, because it will fallback to using OWLocal if m_uselocalAuthentication is set.
	else if ( scheme.equals( "owlocal" ) || m_uselocalAuthentication)
	{
		m_sAuthorization = "OWLocal";
		m_uselocalAuthentication = true;

		m_localNonce = getAuthParam("nonce", authInfo);
		m_localCookieFile = getAuthParam("cookiefile", authInfo);

	}

	if (m_sAuthorization.empty())
	{
		OW_THROW_ERR(HTTPException, "No known authentication schemes", m_statusCode);
	}


}

//////////////////////////////////////////////////////////////////////////////
void HTTPClient::getCredentialsIfNecessary()
{
	if (m_url.principal.empty())
	{
		if (!m_loginCB)
		{
			OW_THROW_ERR(HTTPException, "No login/password to send", m_statusCode);
		}
		else
		{
			String realm;
			if (m_sRealm.empty())
			{
				realm = m_url.toString();
			}
			else
			{
				realm = m_sRealm;
			}
			String name, passwd;
			if (m_loginCB->getCredentials(realm, name, passwd, "") && !name.empty())
			{
				m_url.principal = name;
				m_url.credential = passwd;
			}
			else
			{
				OW_THROW_ERR(HTTPException, "No login/password to send", m_statusCode);
			}
		}
	}

}

//////////////////////////////////////////////////////////////////////////////
void HTTPClient::addCustomHeader(const String& name, const String& value)
{
	this->addHeaderPersistent(name, value);
}

//////////////////////////////////////////////////////////////////////////////
bool HTTPClient::getResponseHeader(const String& hdrName,
	String& valueOut) const
{
	if (HTTPUtils::headerHasKey(m_responseHeaders, hdrName))
	{
		valueOut = HTTPUtils::getHeaderValue(m_responseHeaders, hdrName);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
void HTTPClient::sendAuthorization()
{
	if ( !m_sAuthorization.empty())
	{
		OStringStream ostr;
		ostr << m_sAuthorization << " ";
		if ( m_sAuthorization == "Basic" )
		{
			getCredentialsIfNecessary();
			ostr << HTTPUtils::base64Encode( m_url.principal + ":" +
				m_url.credential );
		}
#ifndef OW_DISABLE_DIGEST
		else if ( m_sAuthorization == "Digest" )
		{
			String sNonceCount;
			sNonceCount.format( "%08x", m_iDigestNonceCount );
			HTTPUtils::DigestCalcResponse( m_sDigestSessionKey, m_sDigestNonce, sNonceCount,
				m_sDigestCNonce, "auth", m_requestMethod, m_httpPath, "", m_sDigestResponse );
			ostr << "username=\"" << m_url.principal << "\", ";
			ostr << "realm=\"" << m_sRealm << "\", ";
			ostr << "nonce=\"" << m_sDigestNonce << "\", ";
			ostr << "uri=\"" + m_httpPath + ", ";
			ostr << "qop=\"auth\", ";	
			ostr << "nc=" << sNonceCount << ", ";
			ostr << "cnonce=\"" << m_sDigestCNonce << "\", ";
			ostr << "response=\"" << m_sDigestResponse << "\"";
			m_iDigestNonceCount++;
		}
#endif
		else if (m_sAuthorization == "OWLocal")
		{
			if (m_localNonce.empty())
			{
				// first round - we just send our euid

				ostr << "uid=\"" << UserUtils::getEffectiveUserId() << "\"";
			}
			else
			{
				// second round - send the nonce and the cookie

				// first try to read the cookie
				std::ifstream cookieFile(m_localCookieFile.c_str());
				if (!cookieFile)
				{
					OW_THROW_ERR(HTTPException, Format("Unable to open local authentication file: %1", strerror(errno)).c_str(), m_statusCode);
				}
				String cookie = String::getLine(cookieFile);
				ostr << "nonce=\"" << m_localNonce << "\", ";
				ostr << "cookie=\"" << cookie << "\"";
			}
		}
		else if (m_spnegoHandler && m_sAuthorization == "Negotiate")
		{
			if (m_spnegoData.empty())
			{
				String errDetails;
				SPNEGOHandler::EResult result = m_spnegoHandler->handshake("", m_spnegoData, errDetails);
				if (result == SPNEGOHandler::E_FAILURE)
				{
					OW_THROW_ERR(HTTPException, Format("SPNEGO authentication failed: %1", errDetails).c_str(), m_statusCode);
				}
				else if (result == SPNEGOHandler::E_CONTINUE)
				{
					// continue. Nothing to do. m_spnegoData was updated by handshake().
				}
				else
				{
					// something has gone horribly wrong. This shouldn't ever happen unless there is a bug.
					OW_THROW_ERR(HTTPException, "SPNEGOAuthentication received success on first round. The handler is buggy.", m_statusCode);
				}
			}
			ostr << m_spnegoData;
		}
		addHeaderNew("Authorization", ostr.toString());
	}
}

//////////////////////////////////////////////////////////////////////////////
HTTPClient::EStatusLineSummary
HTTPClient::checkAndExamineStatusLine()
{
	// If there's some input, we need to read the header. If we got anything
	// besides a 100 or 2xx, we need to stop sending!
	// It's a pretty safe assumption that if the server sent us anything not
	// in response, but just out of the blue, it's terminating the connection
	// for some reason.
	if (m_socket.isConnected() && !m_socket.waitForInput(Timeout::relative(0)))
	{
		getStatusLine();
		StringArray statusLine(m_statusLine.tokenize(" "));
		if (statusLine.size() < 2)
		{
			return E_STATUS_ERROR;
		}
		int statusCode = 500;
		try
		{
			statusCode = statusLine[1].toInt32();
		}
		catch (const StringConversionException&)
		{
			return E_STATUS_ERROR;
		}
		if (statusCode >= 300 && statusCode != 401) // ignore 401 for purposes of authentication
		{
			return E_STATUS_ERROR;
		}
		return E_STATUS_GOOD;
	}
	return E_STATUS_GOOD;
}

//////////////////////////////////////////////////////////////////////////////
void HTTPClient::copyStreams(std::ostream& ostr, std::istream& istr)
{
	std::streambuf* outbuf(ostr.rdbuf());
	std::streambuf* inbuf(istr.rdbuf());
	std::streamsize rv = 0;
	std::streamsize curbufsize = inbuf->in_avail();
	std::streamsize bytesWritten;

	std::vector<char> buffer(curbufsize);
	while (curbufsize != -1)
	{
// From RFC 2616:
//8.2.2 Monitoring Connections for Error Status Messages

//   An HTTP/1.1 (or later) client sending a message-body SHOULD monitor
//   the network connection for an error status while it is transmitting
//   the request. If the client sees an error status, it SHOULD
//   immediately cease transmitting the body. If the body is being sent
//   using a "chunked" encoding (section 3.6), a zero length chunk and
//   empty trailer MAY be used to prematurely mark the end of the message.
//   If the body was preceded by a Content-Length header, the client MUST
//   close the connection.

		if (checkAndExamineStatusLine() == E_STATUS_ERROR)
		{
			// can't close() here, because we still need to read the rest of the response.
			m_closeConnection = true;
			break;
		}

		streamsize bytesToRead(curbufsize > 0 ? curbufsize : 1);

		// reserve() is guaranteed to allocate the appropriate number of bytes.
		buffer.reserve(bytesToRead);
		buffer.push_back(0); // gcc-4.0.2 won't let us access buffer[0] below if the vector is empty

		streamsize charsRead = inbuf->sgetn(&buffer[0], bytesToRead);
		bytesWritten = outbuf->sputn(&buffer[0], charsRead);
		
		rv += bytesWritten;
		if (bytesWritten != charsRead)
			break;

		if (std::istream::traits_type::eq_int_type(inbuf->sgetc(), std::istream::traits_type::eof()))
			break;

		curbufsize = inbuf->in_avail();
	}

}

//////////////////////////////////////////////////////////////////////////////
void HTTPClient::sendDataToServer( const Reference<TempFileStream>& tfs,
	const String& methodName, const String& cimObject, ERequestType requestType )
{
	// Make sure our connection is good
	checkConnection();
	handleAuth();
	String hp;
	if (m_requestMethod.equals("M-POST"))
	{
		hp = HTTPUtils::getCounterStr();
		addHeaderNew("Man", "http://www.dmtf.org/cim/mapping/http/v1.0; ns="
			+ hp);
		hp += "-";
	}
	else
	{
		hp.erase();
	}

	if (requestType == E_CIM_OPERATION_REQUEST || requestType == E_CIM_BATCH_OPERATION_REQUEST)
	{
		addHeaderNew(hp + "CIMOperation", "MethodCall");

		if (requestType == E_CIM_BATCH_OPERATION_REQUEST || methodName.equals("CIMBatch"))
		{
			addHeaderNew(hp + "CIMBatch", "");
		}
		else // normal operations
		{
			addHeaderNew(hp + "CIMMethod", HTTPUtils::escapeForURL(methodName));
			addHeaderNew(hp + "CIMObject", HTTPUtils::escapeForURL(cimObject));
		}
	}

	if (requestType == E_CIM_EXPORT_REQUEST || requestType == E_CIM_BATCH_EXPORT_REQUEST)
	{
		addHeaderNew(hp + "CIMExport", "MethodRequest");

		if (requestType == E_CIM_BATCH_EXPORT_REQUEST || methodName.equals("CIMBatch"))
		{
			addHeaderNew(hp + "CIMExportBatch", "");
		}
		else // normal operations
		{
			addHeaderNew(hp + "CIMExportMethod", HTTPUtils::escapeForURL(methodName));
		}
	}

	if (m_doDeflateOut)
	{
		// deflate test items
		addHeaderNew("Transfer-Encoding", "chunked");
		addHeaderNew("Content-Encoding", "deflate");
	}
	
	// clear out previous status, this may be set if the server sends us something while we're sending.
	m_statusLine.erase();

	// send headers
	sendHeaders(m_requestMethod, "HTTP/1.1");

	// send entity
	tfs->rewind();				
	if (m_doDeflateOut)
	{
#ifdef OW_HAVE_ZLIB_H
		HTTPChunkedOStream chunkostr(m_ostr);
		HTTPDeflateOStream deflateostr(chunkostr);
		copyStreams(deflateostr, *tfs);
		deflateostr.termOutput();
		chunkostr.termOutput();
#else
		OW_THROW_ERR(HTTPException, "Attempted to deflate output but not "
			"compiled with zlib!", m_statusCode);
#endif
	}
	else
	{
		copyStreams(m_ostr, *tfs);
		m_ostr.flush();
	}
	m_requestHeadersNew.clear();
	m_responseHeaders.clear();
}
//////////////////////////////////////////////////////////////////////////////
Reference<std::ostream>
HTTPClient::beginRequest(const String& methodName,
	const String& cimObject)
{
	return Reference<std::ostream>(new TempFileStream());
}
//////////////////////////////////////////////////////////////////////////////
Reference<std::istream>
HTTPClient::endRequest(const Reference<std::ostream>& request, const String& methodName,
			const String& cimObject, ERequestType requestType, const String& cimProtocolVersion)
{
	Reference<TempFileStream> tfs = request.cast_to<TempFileStream>();
	OW_ASSERT(tfs);
	if (!tfs->good())
	{
		OW_THROW_ERR(HTTPException, "HTTPClient: TempFileStream is bad. Temp file creation failed.",m_statusCode);
	}

	int len = tfs->getSize();
	// add common headers
	prepareHeaders();
	OW_ASSERT(!m_contentType.empty());
	addHeaderCommon("Content-Type", m_contentType + "; charset=\"utf-8\"");
	if (!m_doDeflateOut)
	{
		addHeaderCommon("Content-Length", String(len));
	}
	// else we're chunked and don't need the Content-Length header
	addHeaderCommon("TE", "trailers");
#ifdef OW_HAVE_ZLIB_H
	addHeaderCommon("Accept-Encoding", "deflate");
#endif

	// default of 1.0 if we leave it out, so don't bother sending it if that's the case.
	if (!cimProtocolVersion.empty() && cimProtocolVersion != "1.0")
	{
		addHeaderCommon("CIMProtocolVersion", cimProtocolVersion);
	}

	// if there remains bytes from last response, eat them.
	cleanUpIStreams();


	String reasonPhrase;
	Resp_t rt = E_RESPONSE_RETRY;
	do
	{
		if (checkAndExamineStatusLine() == E_STATUS_GOOD)
		{
			sendDataToServer(tfs, methodName, cimObject, requestType);
		}
		reasonPhrase = checkResponse(rt);
	} while (rt == E_RESPONSE_RETRY);
	if (rt == E_RESPONSE_FATAL)
	{
		String CIMError = getHeaderValue("CIMError");
		if (CIMError.empty())
		{
			OW_THROW_ERR(HTTPException, Format("Unable to process request: %1",
				reasonPhrase).c_str(), m_statusCode);
		}
		else
		{
			OW_THROW(CIMErrorException, Format("Unable to process request: %1:%2",
				reasonPhrase, CIMError).c_str());
		}
	}
	m_pIstrReturn = convertToFiniteStream();
	if (!m_pIstrReturn)
	{
		OW_THROW_ERR(HTTPException, "HTTPClient: unable to understand server response. There may be no content in the reply.", m_statusCode);
	}
	return m_pIstrReturn;
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::endResponse(std::istream & /*istr*/)
{
	// process the trailers
	if (m_pIstrReturn)
	{
		// Before we can get at the trailers we need to empty out any data from the layer streams.
		// This also needs to be done to reset the http stream for the next exchange.
		HTTPUtils::eatEntity(*m_pIstrReturn);

		Reference<std::istream> tmp = m_pIstrReturn;
#ifdef OW_HAVE_ZLIB_H
		tmp = m_pIstrReturn.cast_to<HTTPDeflateIStream>();
		if (!tmp)
		{
			tmp = m_pIstrReturn;
		}
#endif
		tmp = tmp.cast_to<HTTPChunkedIStream>();
		if (tmp)
		{
			// the response was chunked, so there could be trailers.  We need to process them.
			HTTPHeaderMap tmpMap;
			if (!HTTPUtils::parseHeader(tmpMap, m_istr))
			{
				OW_THROW_ERR(HTTPException, "Error parsing trailers",m_statusCode);
			}
			m_responseHeaders.insert(tmpMap.begin(), tmpMap.end());
		}
	}

    String errorStr;
    errorStr = getHeaderValue("CIMError");
    if (!errorStr.empty())
    {
        OW_THROW(CIMErrorException, errorStr.c_str());
    }
    errorStr = getHeaderValue("CIMStatusCode");
    if (errorStr.empty())
    {
        // try the old OW 2.0.x pre-standard way
        errorStr = getHeaderValue("CIMErrorCode");
    }
    if (!errorStr.empty())
    {
        String descr;
        descr = getHeaderValue("CIMStatusDescription");
        if (descr.empty())
        {
            // try the old OW 2.0.x pre-standard way
            descr = getHeaderValue("CIMErrorDescription");
        }
        if (!descr.empty())
        {
            OW_THROWCIMMSG(CIMException::ErrNoType(errorStr.toInt32()),
                descr.c_str());
        }
        else
        {
            OW_THROWCIM(CIMException::ErrNoType(errorStr.toInt32()));
        }
    }

	cleanUpIStreams();
}
//////////////////////////////////////////////////////////////////////////////
CIMFeatures
HTTPClient::getFeatures()
{
	m_statusCode = -1; 
	String methodOrig = m_requestMethod;
	m_requestMethod = "OPTIONS";
	prepareHeaders();
	String reasonPhrase;
	Resp_t rt = E_RESPONSE_RETRY;
	do
	{
		checkConnection();
		handleAuth();
		sendHeaders(m_requestMethod,  "HTTP/1.1");
		m_ostr.flush();
		m_requestHeadersNew.clear();
		m_responseHeaders.clear();
		m_statusLine.erase();
		reasonPhrase = checkResponse(rt);
	} while (rt == E_RESPONSE_RETRY);
	m_requestMethod = methodOrig;
	if (rt == E_RESPONSE_FATAL)
	{
		OW_THROW_ERR(HTTPException, Format("Unable to process request: %1",
			reasonPhrase).c_str(), m_statusCode);
	}
	if (getHeaderValue("allow").indexOf("M-POST") == String::npos)
	{
		m_requestMethod = "POST";
	}
	if (getHeaderValue("Accept-Encoding").indexOf("deflate") != String::npos)
	{
		m_doDeflateOut = true;
	}
	String extURL = getHeaderValue("Opt");
	size_t idx = extURL.indexOf(';');
	if (idx < 1 || idx == String::npos)
	{
		OW_THROW_ERR(HTTPException, "No \"Opt\" header in OPTIONS response", m_statusCode);
	}
	CIMFeatures rval;
	rval.extURL = extURL.substring(0, idx);
	rval.extURL.trim();
	String hp = extURL.substring(idx + 1);
	idx = hp.indexOf("=");
	hp = hp.substring(idx + 1);
	hp.trim();
	if (hp.length() != 2)
	{
		OW_THROW_ERR(HTTPException, "HTTP Ext header prefix is not a two digit "
			"number", m_statusCode);
	}
	hp += "-";
	rval.protocolVersion = getHeaderValue(hp + "CIMProtocolVersion");
	String supportedGroups;
	rval.supportsBatch = false;
	if (headerHasKey(hp + "CIMSupportedFunctionalGroups"))
	{
		rval.cimProduct = CIMFeatures::SERVER;
		supportedGroups = getHeaderValue(hp + "CIMSupportedFunctionalGroups");
		if (headerHasKey(hp + "CIMSupportsMultipleOperations"))
		{
			rval.supportsBatch = true;
		}
	}
	else if (headerHasKey(hp + "CIMSupportedExportGroups"))
	{
		rval.cimProduct = CIMFeatures::LISTENER;
		supportedGroups = getHeaderValue(hp + "CIMSupportedExportGroups");
		if (headerHasKey(hp + "CIMSupportsMultipleExports"))
		{
			rval.supportsBatch = true;
		}
	}
	else
	{
		OW_THROW_ERR(HTTPException, "No CIMSupportedFunctionalGroups or "
			"CIMSupportedExportGroups header", m_statusCode);
	}
	rval.supportedGroups = supportedGroups.tokenize(",");
	for (size_t i = 0; i < rval.supportedGroups.size(); i++)
	{
		rval.supportedGroups[i].trim();
	}
	rval.supportedQueryLanguages =
		getHeaderValue(hp + "CIMSupportedQueryLanguages").tokenize(",");
	for (size_t i = 0; i < rval.supportedQueryLanguages.size(); i++)
	{
		rval.supportedQueryLanguages[i].trim();
	}
	rval.cimom = getHeaderValue(hp + "CIMOM");
	rval.validation = getHeaderValue(hp + "CIMValidation");
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::prepareForRetry()
{
	Reference<std::istream> tmpIstr = convertToFiniteStream();
	if (tmpIstr)
	{
		HTTPUtils::eatEntity(*tmpIstr);
	}
	if (m_closeConnection)
	{
		close();
	}
	m_closeConnection = false;
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::sendHeaders(const String& method,
	const String& prot)
{
	m_ostr << method << ' ' << m_httpPath << ' ' << prot << "\r\n";
	for (size_t i = 0; i < m_requestHeadersPersistent.size(); i++)
	{
		m_ostr << m_requestHeadersPersistent[i] << "\r\n";
	}
	for (size_t i = 0; i < m_requestHeadersCommon.size(); i++)
	{
		m_ostr << m_requestHeadersCommon[i] << "\r\n";
	}
	for (size_t i = 0; i < m_requestHeadersNew.size(); i++)
	{
		m_ostr << m_requestHeadersNew[i] << "\r\n";
	}
	m_ostr << "\r\n";
}
//////////////////////////////////////////////////////////////////////////////
HTTPClient::Resp_t
HTTPClient::processHeaders(String& reasonPhrase)
{
	if (getHeaderValue("Connection").equalsIgnoreCase("close"))
	{
		m_closeConnection = true;
		//close();
	}

	Resp_t rt = E_RESPONSE_RETRY;
	String statusLine(m_statusLine);
	size_t idx = statusLine.indexOf(' ');
	String sc; // http status code
	m_statusCode = 500; // status code (int)
	if (idx > 0 && idx != String::npos)
	{
		statusLine = statusLine.substring(idx + 1);
	}
	idx = statusLine.indexOf(' ');
	if (idx > 0 && idx != String::npos)
	{
		sc = statusLine.substring(0,idx);
		reasonPhrase = statusLine.substring(idx + 1);
		try
		{
			m_statusCode = sc.toInt32();
		}
		catch (const StringConversionException&)
		{
			return E_RESPONSE_RETRY;
		}
	}
	if (sc.length() != 3)
	{
		return E_RESPONSE_RETRY;
	}
	switch (sc[0])
	{
		case '1':
			if (m_statusCode == 100)
			{
				rt = E_RESPONSE_CONTINUE;
			}
			else
			{
				rt = E_RESPONSE_FATAL; // support protocol upgrades?  nope.
				reasonPhrase = "No support for protocol upgrades";
			}
			break;
		case '2':
			rt = E_RESPONSE_GOOD;
			m_authRequired = false;
			break;
		case '3':
			rt = E_RESPONSE_FATAL; // support redirects?  I think not...
			reasonPhrase = "No support for redirects";
			break;
		case '4':
//			m_closeConnection = true;
//			close();
			switch (m_statusCode)
			{
				case SC_REQUEST_TIMEOUT:
					rt = E_RESPONSE_RETRY;
					break;
				case SC_UNAUTHORIZED:
					// add authentication info, if available
					if (!m_authRequired)
					{
						m_authRequired = true;
						m_retryCount = 0;
						rt = E_RESPONSE_RETRY;
					}
					else
					{
						rt = E_RESPONSE_FATAL; // already tried authorization once.
						reasonPhrase = "Authentication failure";
						
					}
					break;
				case SC_METHOD_NOT_ALLOWED:
					// switch from M-POST to POST
					if (m_requestMethod.equals("M-POST"))
					{
						m_requestMethod = "POST";
						rt = E_RESPONSE_RETRY;
					}
					else
					{
						rt = E_RESPONSE_FATAL;
						reasonPhrase = "Request method not supported";
					}
					break;
				default:
					m_closeConnection = true;
					//close();
					rt = E_RESPONSE_FATAL;
					reasonPhrase = "Unknown status code";
					break;
			} // switch (m_statusCode)
			break;
		case '5':
			switch (m_statusCode)
			{
				case SC_NOT_IMPLEMENTED:
				case SC_NOT_EXTENDED:
					// switch from M-POST to POST
					if (m_requestMethod.equals("M-POST"))
					{
						m_requestMethod = "POST";
						rt = E_RESPONSE_RETRY;
						// only do this for JWS, since it doesn't eat the entity.
						m_closeConnection = true;
						//close();
					}
					else
					{
						rt = E_RESPONSE_FATAL;
						reasonPhrase = "Request method not supported";
					}
					break;
				case SC_SERVICE_UNAVAILABLE:
					rt = E_RESPONSE_RETRY;
					break;
				default:
					rt = E_RESPONSE_FATAL;
					reasonPhrase = "Unknown status code";
					break;
			} // switch (m_statusCode)
			break;
		default:
			rt = E_RESPONSE_RETRY; // shouldn't happen
			break;
	} // switch (sc[0])

	// Get the Man header and save the ns
	String man = getHeaderValue("Man");
	if (!man.empty())
	{
		StringArray manVals = man.tokenize(";");
		if (manVals.size() >= 2)
		{
			StringArray nsVals = manVals[1].tokenize("=");
			if (nsVals.size() == 2)
			{
				m_ns = nsVals[1];
			}
		}
	}
	// now check for headers which indicate an error
	String CIMError = getHeaderValue("CIMError");
	if (!CIMError.empty())
	{
		rt = E_RESPONSE_FATAL;
		reasonPhrase = "CIMError";
	}

	// have to process WWW-Authenticate: Negotiate <blah> here, because we have to validate it on the response.
	String authInfo = getHeaderValue("www-authenticate");
	String negStr("Negotiate");
	if (m_spnegoHandler && authInfo.indexOf(negStr) != String::npos)
	{
		size_t idx = authInfo.indexOf(negStr);
		if (idx != String::npos)
		{
			String data = authInfo.substring(idx + negStr.length() + 1);
			String errDetails;
			SPNEGOHandler::EResult result = m_spnegoHandler->handshake(data, m_spnegoData, errDetails);
			if (result == SPNEGOHandler::E_SUCCESS)
			{
				// success. Do nothing.
			}
			else if (result == SPNEGOHandler::E_FAILURE)
			{
				reasonPhrase = Format("SPNEGO handshake failed: %1", errDetails);
				rt = E_RESPONSE_FATAL;
			}
			else if (result == SPNEGOHandler::E_CONTINUE)
			{
				// continue, so handshake() should have saved off the token for the next round in m_spnegoData.
			}
			else
			{
				// something has gone horribly wrong. This shouldn't ever happen unless there is a bug.
				OW_THROW_ERR(HTTPException, Format("SPNEGOAuthentication received unknown response (%1) from spnego handler.", result).c_str(), m_statusCode);
			}
		}
		else
		{
			m_spnegoData.erase();
		}
	}

	return rt;
}
/////////////////////////////////////////////////////////////////////////////
Reference<std::istream>
HTTPClient::convertToFiniteStream()
{
	Reference<std::istream> rval(0);
	if (getHeaderValue("Transfer-Encoding").equalsIgnoreCase("chunked"))
	{
		rval = new HTTPChunkedIStream(m_istr);
	}
	else if (headerHasKey("Content-Length"))
	{
		UInt64 clen = getHeaderValue("Content-Length").toUInt64(); 
		rval = new HTTPLenLimitIStream(m_istr,clen); 
	}
	if (getHeaderValue("Content-Encoding").equalsIgnoreCase("deflate"))
	{
#ifdef OW_HAVE_ZLIB_H
		rval = new HTTPDeflateIStream(rval);
#else
		OW_THROW_ERR(HTTPException, "Response is deflated but we're not "
			"compiled with zlib!", m_statusCode);
#endif // #ifdef OW_HAVE_ZLIB_H
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::handleAuth()
{
	// add new headers.
	if (m_authRequired || m_uselocalAuthentication)
	{
		receiveAuthentication();
		sendAuthorization();
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::checkConnection()
{
	if (!m_istr || !m_ostr || !m_socket.isConnected())
	{
		m_socket.disconnect();
		m_socket.connect(m_serverAddress);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::getStatusLine()
{
	// RFC 2616 says to skip leading blank lines...
	while (m_statusLine.trim().empty() && m_istr)
	{
		m_statusLine = String::getLine(m_istr);
	}
	m_statusCode = -1; 
}
//////////////////////////////////////////////////////////////////////////////
String
HTTPClient::checkResponse(Resp_t& rt)
{
	String reasonPhrase;
	do
	{
		getStatusLine();
		
		if (!m_istr)
		{
			if (m_socket.receiveTimeOutExpired())
			{
				reasonPhrase = "Client receive timeout expired.";
				rt = E_RESPONSE_FATAL;
			}
			else
			{
				reasonPhrase = "Lost connection with server";
				rt = E_RESPONSE_RETRY;
			}
			m_closeConnection = true;
			//close();
			break;
		}
		if (!HTTPUtils::parseHeader(m_responseHeaders, m_istr))
		{
			OW_THROW_ERR(HTTPException, Format("Received junk from server statusline = %1", m_statusLine).c_str(), m_statusCode);
		}
		rt = processHeaders(reasonPhrase);
		if (rt == E_RESPONSE_CONTINUE)
		{
			prepareForRetry();
		}
	} while (rt == E_RESPONSE_CONTINUE);
	if (rt == E_RESPONSE_RETRY)
	{
		++m_retryCount;
		if (m_retryCount > 3)
		{
			rt = E_RESPONSE_FATAL;
			reasonPhrase = "Tried maximum number of times";
		}
		else
		{
			prepareForRetry();
		}
	}
	else if (rt == E_RESPONSE_GOOD)
	{
		m_retryCount = 0;
	}
	return reasonPhrase;
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::prepareHeaders()
{
	m_requestHeadersCommon.clear();
	m_responseHeaders.clear();
}
//////////////////////////////////////////////////////////////////////////////
SocketAddress
HTTPClient::getLocalAddress() const
{
	if (!m_socket.isConnected())
	{
		m_socket.connect(m_serverAddress);
	}
	return m_socket.getLocalAddress();
}
//////////////////////////////////////////////////////////////////////////////
SocketAddress
HTTPClient::getPeerAddress() const
{
	if (!m_socket.isConnected())
	{
		m_socket.connect(m_serverAddress);
	}
	return m_socket.getPeerAddress();
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::setHTTPPath(const String& newPath)
{
	m_httpPath = newPath;
}

//////////////////////////////////////////////////////////////////////////////
void HTTPClient::assumeBasicAuth()
{
	close();
	m_authRequired = true;
	m_sAuthorization = "Basic";
	m_uselocalAuthentication = false;
}

//////////////////////////////////////////////////////////////////////////////
void HTTPClient::close()
{
	m_socket.disconnect();
}


//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::setReceiveTimeout(const Timeout& timeout)
{
	m_socket.setReceiveTimeout(timeout);
}

//////////////////////////////////////////////////////////////////////////////
Timeout
HTTPClient::getReceiveTimeout() const
{
	return m_socket.getReceiveTimeout();
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::setSendTimeout(const Timeout& timeout)
{
	m_socket.setSendTimeout(timeout);
}

//////////////////////////////////////////////////////////////////////////////
Timeout
HTTPClient::getSendTimeout() const
{
	return m_socket.getSendTimeout();
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::setConnectTimeout(const Timeout& timeout)
{
	m_socket.setConnectTimeout(timeout);
}

//////////////////////////////////////////////////////////////////////////////
Timeout
HTTPClient::getConnectTimeout() const
{
	return m_socket.getConnectTimeout();
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPClient::setTimeouts(const Timeout& timeout)
{
	m_socket.setTimeouts(timeout);
}

//////////////////////////////////////////////////////////////////////////////
void 
HTTPClient::assumeSPNEGOAuth()
{
	close();
	m_authRequired = true;
	m_sAuthorization = "Negotiate";
	m_uselocalAuthentication = false;
}

void
HTTPClient::setSPNEGOHandler(const SPNEGOHandlerRef& spnegoHandler)
{
	m_spnegoHandler = spnegoHandler;
}

} // end namespace OW_NAMESPACE

