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
#include "OW_HTTPClient.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_HTTPDeflateOStream.hpp"
#include "OW_HTTPDeflateIStream.hpp"
#include "OW_HTTPLenLimitIStream.hpp"
#include "OW_Format.hpp"
#include "OW_DateTime.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_Assertion.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_HTTPException.hpp"


using std::flush;
using std::istream;

//////////////////////////////////////////////////////////////////////////////
OW_HTTPClient::OW_HTTPClient( const OW_String &sURL )
#ifndef OW_DISABLE_DIGEST
	: m_sRealm()
  	, m_sDigestNonce()
	, m_sDigestCNonce()
	, m_iDigestNonceCount(1)
	, m_sDigestSessionKey()
	, m_sDigestResponse() ,
#else
	:
#endif
	 m_serverAddress()
	, m_url(sURL)
	, m_responseHeaders(), m_requestHeadersCommon()
	, m_requestHeadersNew(), m_pIstrReturn(0)
	, m_socket(m_url.protocol.equalsIgnoreCase("https"))
	, m_requestMethod("M-POST"), m_authRequired(false)
	, m_needsConnect(true)
	, m_istr(m_socket.getInputStream()), m_ostr(m_socket.getOutputStream())
	, m_doDeflateOut(false)
	, m_retryCount(0)
{
	signal(SIGPIPE, SIG_IGN);
	setUrl();
	//m_socket.setReceiveTimeout(300);
	//m_socket.setSendTimeout(300);
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPClient::~OW_HTTPClient()
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
OW_HTTPClient::cleanUpIStreams()
{
	// if there remains bytes from last response, eat them.
	if (m_pIstrReturn)
	{
		OW_HTTPUtils::eatEntity(*m_pIstrReturn);
		m_pIstrReturn = 0;
	}

}

//////////////////////////////////////////////////////////////////////////////
void OW_HTTPClient::setUrl()
{
	if (m_url.protocol.empty())
	{
		m_url.protocol = "http";
	}

	if (m_url.port == 0)
	{
		if( m_url.protocol.equalsIgnoreCase("https") )
		{
			m_url.port = 5989;
		}
		else if (m_url.protocol.equalsIgnoreCase("http"))
		{
			m_url.port = 5988;
		}
	}

	if (m_url.protocol.equalsIgnoreCase("https"))
	{
#ifndef OW_NO_SSL
		OW_SSLCtxMgr::initClient();
#else
		OW_THROW(OW_SocketException, "SSL not available");
#endif // #ifndef OW_NO_SSL
	}

	if (m_url.protocol.equalsIgnoreCase("ipc"))
	{
		m_serverAddress = OW_SocketAddress::getUDS(OW_DOMAIN_SOCKET_NAME);
	}
	else
	{
		m_serverAddress = OW_SocketAddress::getByName(m_url.host,
			m_url.port);
	}

	if( m_url.path.empty())
	{
		m_url.path = "/cimom";
	}
}


//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPClient::receiveAuthentication()
{
#ifndef OW_DISABLE_DIGEST
	OW_RandomNumber rn(0, 0x7FFFFFFF);
	m_sDigestCNonce.format( "%08x", rn.getNextNumber() );
	
	OW_String authInfo = getHeaderValue("www-authenticate");
	size_t iBeginIndex = authInfo.indexOf( "realm" ) + 7;
	if( iBeginIndex >= 7 )
	{
		size_t iEndIndex = authInfo.indexOf( '"', iBeginIndex );
		if( iEndIndex != OW_String::npos )
		{
			m_sRealm = authInfo.substring( iBeginIndex, iEndIndex -
				iBeginIndex );
		}
		else
		{
			m_sRealm = "";
		}
	}
	else
	{
		m_sRealm = "";
	}
#endif // #ifndef OW_DISABLE_DIGEST

	if (m_url.username.empty())
	{
		if (!m_loginCB)
		{
			OW_THROW(OW_HTTPException, "No login/password to send");
		}
		else
		{
			OW_String realm;
#ifndef OW_DISABLE_DIGEST
			if (m_sRealm.empty())
			{
				realm = m_url.toString();
			}
			else
			{
				realm = m_sRealm;
			}
#else
			realm = m_url.toString();
#endif
			OW_String name, passwd;
			if (m_loginCB->getCredentials(realm, name, passwd, ""))
			{
				m_url.username = name;
				m_url.password = passwd;
			}
			else
			{
				OW_THROW(OW_HTTPException, "No login/password to send");
			}
		}
	}


#ifndef OW_DISABLE_DIGEST
	if(headerHasKey("authentication-info") && m_sAuthorization=="Digest" )
	{
		OW_String authInfo = getHeaderValue("authentication-info");
		size_t iBeginIndex = authInfo.indexOf( "nextnonce" ) + 11;
		if( iBeginIndex >= 11 )
		{
			size_t iEndIndex = authInfo.indexOf( '"', iBeginIndex );
			if( iEndIndex != OW_String::npos )
			{
				m_sDigestNonce = authInfo.substring( iBeginIndex,
					iEndIndex - iBeginIndex );
			}
		}
		OW_HTTPUtils::DigestCalcHA1( "md5", m_url.username, m_sRealm,
			m_url.password, m_sDigestNonce, m_sDigestCNonce, m_sDigestSessionKey );
		m_iDigestNonceCount = 1;

	}
	else if( getHeaderValue("www-authenticate").indexOf( "Digest" ) != OW_String::npos )
	{
		m_sAuthorization = "Digest";
		OW_String authInfo = getHeaderValue("www-authenticate");
		size_t iBeginIndex = authInfo.indexOf( "nonce" ) + 7;
		if( iBeginIndex >= 7 )
		{
			size_t iEndIndex = authInfo.indexOf( '"', iBeginIndex );
			if( iEndIndex != OW_String::npos )
			{
				m_sDigestNonce = authInfo.substring( iBeginIndex, iEndIndex -
					iBeginIndex );
			}
		}

		OW_HTTPUtils::DigestCalcHA1( "md5", m_url.username, m_sRealm,
			m_url.password, m_sDigestNonce, m_sDigestCNonce, m_sDigestSessionKey );
	}
	else 
#endif
	if( getHeaderValue("www-authenticate").indexOf( "Basic" ) != OW_String::npos )
	{
		m_sAuthorization = "Basic";
	}
	else
	{
		OW_THROW(OW_HTTPException, "No known authentication schemes");
	}
}
				
//////////////////////////////////////////////////////////////////////////////
void OW_HTTPClient::sendAuthorization()
{
	if( !m_sAuthorization.empty())
	{
		OW_StringStream ostr;
		ostr << m_sAuthorization << " ";
		if( m_sAuthorization == "Basic" )
		{
			ostr << OW_HTTPUtils::base64Encode( m_url.username + ":" +
				m_url.password );
		}
#ifndef OW_DISABLE_DIGEST
		else if( m_sAuthorization == "Digest" )
		{
			OW_String sNonceCount;
			sNonceCount.format( "%08x", m_iDigestNonceCount );
			OW_HTTPUtils::DigestCalcResponse( m_sDigestSessionKey, m_sDigestNonce, sNonceCount,
				m_sDigestCNonce, "auth", m_requestMethod, m_url.path, "", m_sDigestResponse );
			ostr << "username=\"" << m_url.username << "\", ";
			ostr << "realm=\"" << m_sRealm << "\", ";
			ostr << "nonce=\"" << m_sDigestNonce << "\", ";
			ostr << "uri=\"" << m_url.path << "\", ";
			ostr << "qop=\"auth\", ";	
			ostr << "nc=" << sNonceCount << ", ";
			ostr << "cnonce=\"" << m_sDigestCNonce << "\", ";
			ostr << "response=\"" << m_sDigestResponse << "\"";
			m_iDigestNonceCount++;
		}
#endif
		addHeaderNew("Authorization", ostr.toString());
	}
}


//////////////////////////////////////////////////////////////////////////////
void OW_HTTPClient::sendDataToServer( OW_Reference<OW_TempFileStream> tfs,
	const OW_String& methodName, const OW_String& nameSpace )
{
	// Make sure our connection is good

	checkConnection();


	OW_String hp;
	if (m_requestMethod.equals("M-POST"))
	{
		hp = OW_HTTPUtils::getCounterStr();
		addHeaderNew("Man", "http://www.dmtf.org/cim/mapping/http/v1.0; ns="
			+ hp);
		hp += "-";
	}
	else
	{
		hp.erase();
	}
	addHeaderNew(hp + "CIMOperation", "MethodCall");
	if (methodName.equals("CIMBatch"))
	{
		addHeaderNew(hp + "CIMBatch", "");
	}
	else
	{
		addHeaderNew(hp + "CIMMethod", methodName);
		addHeaderNew(hp + "CIMObject", nameSpace);
	}

	if (m_doDeflateOut)
	{
		// deflate test items
		addHeaderNew("Transfer-Encoding", "chunked");
		addHeaderNew("Content-Encoding", "deflate");
	}

	// send headers
	sendHeaders(m_requestMethod, "HTTP/1.1");
	
	// send entity
	tfs->rewind();				

	if (m_doDeflateOut)
	{
#ifdef OW_HAVE_ZLIB_H
		// test deflate stuff
		OW_HTTPChunkedOStream chunkostr(m_ostr);
		OW_HTTPDeflateOStream deflateostr(chunkostr);
		deflateostr << tfs->rdbuf();
		deflateostr.termOutput();
		chunkostr.termOutput();
		// end deflate test stuff
#else
		OW_THROW(OW_HTTPException, "Attempted to deflate output but not "
			"compiled with zlib!");
#endif
	}
	else
	{
		m_ostr << tfs->rdbuf() << "\r\n" << flush;
	}

	m_requestHeadersNew.clear();
	m_responseHeaders.clear();
}


//////////////////////////////////////////////////////////////////////////////
OW_Reference<std::iostream>
OW_HTTPClient::beginRequest(const OW_String& methodName,
	const OW_String& nameSpace)
{
	(void)methodName; (void)nameSpace;
    return OW_Reference<std::iostream>(new OW_TempFileStream());
}

//////////////////////////////////////////////////////////////////////////////
OW_Reference<OW_CIMProtocolIStreamIFC>
OW_HTTPClient::endRequest(OW_Reference<std::iostream> request, const OW_String& methodName,
			const OW_String& nameSpace)
{
	OW_Reference<OW_TempFileStream> tfs = request.cast_to<OW_TempFileStream>();
	OW_ASSERT(tfs);
	int len = tfs->getSize();

	// add common headers
	prepareHeaders();
	OW_ASSERT(!m_contentType.empty());
	addHeaderCommon("Content-Type", m_contentType);
	addHeaderCommon("Content-Length", OW_String(len));

	addHeaderCommon("TE", "trailers");

#ifdef OW_HAVE_ZLIB_H
	addHeaderCommon("Accept-Encoding", "deflate");
#endif

	// if there remains bytes from last response, eat them.
	cleanUpIStreams();

	OW_String statusLine;
	Resp_t rt = RETRY;
	do
	{
		sendDataToServer(tfs, methodName, nameSpace);

		statusLine = checkResponse(rt);

	} while(rt == RETRY);

	if (rt == FATAL)
	{

		/*
		OW_String sNonceCount;
		sNonceCount.format( "%08x", m_iDigestNonceCount );
		OW_String errDetails = "Bailing out of sendRequest: ";
		errDetails += "sHA1: >" + m_sDigestSessionKey + "< sNonce: >" +
			m_sDigestNonce + "< Nonce Count >" + sNonceCount + "< sCNonce: >" +
			m_sDigestCNonce + "< Method >" + m_requestMethod +
			"< url >" + m_url.path + "<";
		*/
		OW_THROW(OW_HTTPException, format("Unable to process request: %1",
			statusLine).c_str());
	}

	m_pIstrReturn = convertToFiniteStream();
	OW_ASSERT(m_pIstrReturn);
	return m_pIstrReturn;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_HTTPClient::getFeatures()
{
	OW_String methodOrig = m_requestMethod;
	m_requestMethod = "OPTIONS";
	prepareHeaders();
	OW_String statusLine;
	Resp_t rt = RETRY;
	do
	{
		checkConnection();
		sendHeaders(m_requestMethod,  "HTTP/1.1");
		m_ostr.flush();

		m_requestHeadersNew.clear();
		m_responseHeaders.clear();

		statusLine = checkResponse(rt);
	} while(rt == RETRY);

	m_requestMethod = methodOrig;
	if (rt == FATAL)
	{
		OW_THROW(OW_HTTPException, format("Unable to process request: %1",
			statusLine).c_str());
	}

	if (getHeaderValue("allow").indexOf("M-POST") == OW_String::npos)
	{
		m_requestMethod = "POST";
	}

	OW_String extURL = getHeaderValue("Opt");
	size_t idx = extURL.indexOf(';');
	if (idx < 1 || idx == OW_String::npos)
	{
		OW_THROW(OW_HTTPException, "No \"Opt\" header in OPTIONS response");
	}
	OW_CIMFeatures rval;
	rval.extURL = extURL.substring(0, idx);
	rval.extURL.trim();

	OW_String hp = extURL.substring(idx + 1);
	idx = hp.indexOf("=");
	hp = hp.substring(idx + 1);
	hp.trim();

	if (hp.length() != 2)
	{
		OW_THROW(OW_HTTPException, "HTTP Ext header prefix is not a two digit "
			"number");
	}

	hp += "-";
	rval.protocolVersion = getHeaderValue(hp + "CIMProtocolVersion");
	OW_String supportedGroups;

	rval.supportsBatch = false;
	if (headerHasKey(hp + "CIMSupportedFunctionalGroups"))
	{
		rval.cimProduct = OW_CIMFeatures::SERVER;
		supportedGroups = getHeaderValue(hp + "CIMSupportedFunctionalGroups");
		if (headerHasKey(hp + "CIMSupportsMultipleOperations"))
		{
			rval.supportsBatch = true;
		}


	}
	else if (headerHasKey(hp + "CIMSupportedExportGroups"))
	{
		rval.cimProduct = OW_CIMFeatures::LISTENER;
		supportedGroups = getHeaderValue(hp + "CIMSupportedExportGroups");
		if (headerHasKey(hp + "CIMSupportsMultipleExports"))
		{
			rval.supportsBatch = true;
		}
	}
	else
	{
		OW_THROW(OW_HTTPException, "No CIMSupportedFunctionalGroups or "
			"CIMSupportedExportGroups header");
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
OW_HTTPClient::prepareForRetry()
{
	OW_Reference<OW_CIMProtocolIStreamIFC> tmpIstr = convertToFiniteStream();
	if (tmpIstr)
	{
		OW_HTTPUtils::eatEntity(*tmpIstr);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPClient::sendHeaders(const OW_String& method,
	const OW_String& prot)
{
	m_ostr << "\r\n";
	m_ostr << method << " " << m_url.path << " " << prot << "\r\n";
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

OW_HTTPClient::Resp_t
OW_HTTPClient::processHeaders(OW_String& statusLine)
{

	Resp_t rt = FATAL;
	size_t idx = statusLine.indexOf(' ');
	OW_String respProt;
	OW_String sc; // http status code
	int isc = 500; // status code (int)
	if (idx > 0 && idx != OW_String::npos)
	{
		respProt = statusLine.substring(0, idx);
		statusLine = statusLine.substring(idx + 1);
	}
	idx = statusLine.indexOf(' ');
	if (idx > 0 && idx != OW_String::npos)
	{
		sc = statusLine.substring(0,idx);
		statusLine = statusLine.substring(idx + 1);
		try
		{
			isc = sc.toInt32();
		}
		catch (const OW_StringConversionException&)
		{
			return FATAL;
		}
	}
	if (sc.length() != 3)
	{
		return FATAL;
	}

	switch (sc[0])
	{
		case '1':
			if (isc == 100)
			{
				rt = CONTINUE;
			}
			else
			{
				rt = FATAL; // support protocol upgrades?  nope.
			}
			break;
		case '2':
			rt = GOOD;
			m_retryCount = 0;
			m_authRequired = false;
			break;
		case '3':
			rt = FATAL; // support redirects?  I think not...
			break;
		case '4':
			m_needsConnect = true;
			switch (isc)
			{
				case SC_UNAUTHORIZED:
					// add authentication info, if available
					if (!m_authRequired)
					{
						m_authRequired = true;
						rt = RETRY;
					}
					else
					{
						rt = FATAL; // already tried authorization once.
					}
					break;
				case SC_METHOD_NOT_ALLOWED:
					// switch from M-POST to POST
					if (m_requestMethod.equals("M-POST"))
					{
						m_requestMethod = "POST";
						rt = RETRY;
					}
					else
					{
						rt = FATAL;
					}
					break;
			default:
	  				m_needsConnect = true;
					rt = FATAL;
					break;
			} // switch (isc)
			break;
		case '5':
			switch (isc)
			{
				case SC_NOT_IMPLEMENTED:
				case SC_NOT_EXTENDED:
					// switch from M-POST to POST
					if (m_requestMethod.equals("M-POST"))
					{
						m_requestMethod = "POST";
						rt = RETRY;
					}
					else
					{
						rt = FATAL;
					}
					break;
				default:
					rt = FATAL;
					break;
			} // switch (isc)
			break;
		default:
			rt = FATAL; // shouln't happen
			break;
	} // switch (sc[0])
	return rt;
}

/////////////////////////////////////////////////////////////////////////////
OW_Reference<OW_CIMProtocolIStreamIFC>
OW_HTTPClient::convertToFiniteStream()
{
	OW_Reference<OW_CIMProtocolIStreamIFC> rval(0);
	if (getHeaderValue("Transfer-Encoding").equalsIgnoreCase("chunked"))
	{
		rval = new OW_HTTPChunkedIStream(m_istr);
	}
	else if (headerHasKey("Content-Length"))
	{
		rval = new OW_HTTPLenLimitIStream(m_istr,
			getHeaderValue("Content-Length").toInt32());
	}
	if (getHeaderValue("Content-Encoding").equalsIgnoreCase("deflate"))
	{
#ifdef OW_HAVE_ZLIB_H
		rval = new OW_HTTPDeflateIStream(rval);
#else
		OW_THROW(OW_HTTPException, "Response is deflated but we're not "
			"compiled with zlib!");
#endif // #ifdef OW_HAVE_ZLIB_H
	}
	if (!rval)
	{
		OW_THROW(OW_HTTPException, "OW_HTTPClient::convertToFiniteStream: unable to understand server response!");
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPClient::handleAuth()
{
	receiveAuthentication();
	sendAuthorization();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPClient::checkConnection()
{
	if (!m_istr || !m_ostr)
	{
		m_needsConnect = true;
	}
	if (m_needsConnect)
	{
		m_socket.disconnect();
		m_socket.connect(m_serverAddress);
		m_needsConnect = false;
	}

	// add new headers.

	if (m_authRequired)
	{
		handleAuth();
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_HTTPClient::checkResponse(Resp_t& rt)
{
	OW_String statusLine;
	do
	{
		// RFC 2616 says to skip leading blank lines...
		do
		{
			statusLine = OW_String::getLine(m_istr);
		}
		while(statusLine.trim().empty() && m_istr);
		if (!m_istr)
		{
			m_needsConnect = true;
			// ATTN:
			// used to be RETRY, but this caused problems if
			// attempting a http connection to a https port
			// (infinite loop).
			// not sure why it was retry before, maybe FATAL
			// will break something else?
			if (++m_retryCount > 3)
			{
				rt = FATAL;
			}
			else
			{
				rt = RETRY;
			}
			break;
		}

		if (!OW_HTTPUtils::parseHeader(m_responseHeaders, m_istr))
		{
			OW_THROW(OW_HTTPException, "Received junk from server");
		}
		if (getHeaderValue("Connection").equalsIgnoreCase("close"))
		{
			m_needsConnect = true;
		}
		rt = processHeaders(statusLine);
		if (rt == CONTINUE)
		{
			prepareForRetry();
		}
	} while (rt == CONTINUE);

	if (rt == RETRY)
	{
		prepareForRetry();
	}

	return statusLine;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPClient::prepareHeaders()
{
	m_requestHeadersCommon.clear();
	addHeaderCommon("Host", m_url.host);
	addHeaderCommon("User-Agent", OW_PACKAGE"/"OW_VERSION);
	m_responseHeaders.clear();
}


//////////////////////////////////////////////////////////////////////////////
OW_SocketAddress
OW_HTTPClient::getLocalAddress() const
{
	if (m_needsConnect)
	{
		m_socket.connect(m_serverAddress);
		m_needsConnect = false;
	}
	return m_socket.getLocalAddress();
}


//////////////////////////////////////////////////////////////////////////////
OW_SocketAddress
OW_HTTPClient::getPeerAddress() const
{
	if (m_needsConnect)
	{
		m_socket.connect(m_serverAddress);
		m_needsConnect = false;
	}
	return m_socket.getPeerAddress();
}
