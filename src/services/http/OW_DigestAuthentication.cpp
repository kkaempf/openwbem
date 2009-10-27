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

#include "OW_config.h"
#include "OW_DigestAuthentication.hpp"
#include "OW_AuthenticationException.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/Map.hpp"
#include "OW_HTTPUtils.hpp"
#include "blocxx/DateTime.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/MD5.hpp"
#include "OW_HTTPSvrConnection.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/SecureRand.hpp"
#include <fstream>

namespace OW_NAMESPACE
{

using std::endl;
using std::ifstream;
using namespace blocxx;
//////////////////////////////////////////////////////////////////////////////
DigestAuthentication::DigestAuthentication(const String& passwdFile)
	: m_asNonces()
	, m_aDateTimes()
	, m_passwdMap()
{
	if (passwdFile.empty())
	{
		OW_THROW(AuthenticationException, "No password file given for "
			"digest authentication.");
	}
	ifstream infile(passwdFile.c_str());
	if (!infile)
	{
		OW_THROW(AuthenticationException, Format("Unable to open password file %1",
			passwdFile).c_str());
	}
	while (infile)
	{
		String line;
		line = String::getLine(infile);
		size_t idx = line.lastIndexOf(':');
		m_passwdMap[line.substring(0, idx)] = line.substring(idx + 1);
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
String
DigestAuthentication::getHash( const String &sUserName,
	const String &sRealm )
{
	String key = sUserName + ":" + sRealm;
	return m_passwdMap[key];
}
//////////////////////////////////////////////////////////////////////////////
String
DigestAuthentication::generateNewNonce( void )
{
	DateTime DateTime;
	DateTime.setToCurrent();
	String sDateTime( static_cast<Int64>(DateTime.get()) );
	String sPrivateData;
	sPrivateData.format("%08x%08x%08x%08x%08x",
		Secure::rand_uint<UInt32>(), Secure::rand_uint<UInt32>(),
		Secure::rand_uint<UInt32>(), Secure::rand_uint<UInt32>(),
		Secure::rand_uint<UInt32>()
	);

	blocxx::MD5 md5;
	md5.update(sDateTime);
	md5.update(":");
	md5.update("ETag");	/// @todo  This should be a real ETag
	md5.update(":");
	md5.update(sPrivateData);
	String sNonce = md5.toString();
	sNonce = sDateTime + sNonce;
	sNonce = HTTPUtils::base64Encode( sNonce );
	for ( Int16 iNonce=m_asNonces.size()-1; iNonce>=0; iNonce-- )
	{
		if ( m_aDateTimes[ iNonce ] < (DateTime.get()-60) ) // Only valid for 60 seconds.
		{
			m_asNonces.remove( 0, iNonce+1 );	// Every nonce before this one
			m_aDateTimes.remove( 0, iNonce+1 );	// Came before this one.
			break;
		}
	}
	m_asNonces.append( sNonce );
	m_aDateTimes.append( DateTime.get() );
	return sNonce;
}
//////////////////////////////////////////////////////////////////////////////
static void
parseInfo(const String& pinfo, Map<String, String>& infoMap)
{
	size_t idx = pinfo.indexOf("Digest");
	String info;
	if (idx != String::npos)
	{
		info = pinfo.substring(7);
	}
	else
	{
		OW_THROW(AuthenticationException, "Error parsing Digest Response");
	}
	Array<String> infoAr = info.tokenize(",");
	for (size_t i = 0; i < infoAr.size(); ++i)
	{
		String lhs, rhs;
		idx = infoAr[i].indexOf('=');
		if (idx != String::npos)
		{
			lhs = infoAr[i].substring(0, idx);
			lhs.trim();
			if (idx + 1 < infoAr[i].length())
			{
				rhs = infoAr[i].substring(idx + 1);
				rhs.trim();
				if (rhs[0] == '\"')
				{
					rhs = rhs.substring(1);
					rhs = rhs.substring(0, rhs.length() - 1);
				}
				infoMap[lhs] = rhs;
				continue;
			}
		}
		OW_THROW(AuthenticationException, "Error parsing Digest Response");
	}
}
//////////////////////////////////////////////////////////////////////////////
EAuthenticateResult
DigestAuthentication::authenticate(String& userName,
		const String& info, HTTPSvrConnection* htcon)
{
	String hostname = htcon->getHostName();
	if (info.empty())
	{
		htcon->setErrorDetails("You must authenticate to access this resource");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return E_AUTHENTICATE_CONTINUE;
	}
	Map<String, String> infoMap;
	parseInfo(info, infoMap);
	String sNonce = infoMap["nonce"];
	bool nonceFound = false;
	if ( !sNonce.empty())
	{
		for (size_t iNonce = 0; iNonce < m_asNonces.size(); ++iNonce)
		{
			if ( sNonce == m_asNonces[ iNonce ] )
			{
				m_asNonces.remove( iNonce );
				m_aDateTimes.remove( iNonce );
				nonceFound = true;
				break;
			}
		}
	}
	else
	{
		htcon->setErrorDetails("No nonce value was provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return E_AUTHENTICATE_FAIL;
	}
	userName = infoMap["username"];
	if ( userName.empty() )
	{
		htcon->setErrorDetails("No user name was provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return E_AUTHENTICATE_FAIL;
	}
	String sRealm = infoMap["realm"];
	if ( sRealm.empty() )
	{
		htcon->setErrorDetails("No realm was provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return E_AUTHENTICATE_FAIL;
	}
	String sNonceCount = infoMap["nc"];
	if ( sNonceCount.empty() )
	{
		htcon->setErrorDetails("No Nonce Count was provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return E_AUTHENTICATE_FAIL;
	}
	/// @todo isn't cnonce optional?
	String sCNonce = infoMap["cnonce"];
	if ( sCNonce.empty() )
	{
		htcon->setErrorDetails("No cnonce value provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return E_AUTHENTICATE_FAIL;
	}
	String sResponse = infoMap["response"];
	if ( sResponse.empty() )
	{
		htcon->setErrorDetails("The response was zero length");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return E_AUTHENTICATE_FAIL;
	}
	String sHA1 = getHash( userName, sRealm );
	String sTestResponse;
	HTTPUtils::DigestCalcResponse( sHA1, sNonce, sNonceCount, sCNonce,
		"auth", htcon->getRequestLine()[ 0 ], htcon->getRequestLine()[1],
		"", sTestResponse );
	if ( sTestResponse == sResponse )
	{
		if (nonceFound)
		{
			htcon->addHeader("Authentication-Info",
				"qop=\"auth\", nextnonce=\"" + generateNewNonce() + "\"");
			return E_AUTHENTICATE_SUCCESS;
		}
		else
		{
			htcon->setErrorDetails("Nonce not found.");
			htcon->addHeader("WWW-Authenticate", getChallenge(hostname)
				+ ", stale=true");
			return E_AUTHENTICATE_FAIL;
		}
	}
	/*
	String errDetails = "At end of doAuthenticate(): ";
	errDetails += "sHA1: >" + sHA1 + "< sNonce: >" + sNonce +
		"< Nonce Count >" + sNonceCount + "< sCNonce: >" +
		sCNonce + "< Method >" + htcon->getRequestLine()[0] +
		"< url >" + htcon->getRequestLine()[1] + "< testResponse >" + sTestResponse
		+ "< client Response >" + sResponse + "<";
	*/
	String errDetails = "User name or password not valid";
	htcon->setErrorDetails(errDetails);
	htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
	return E_AUTHENTICATE_FAIL;
}
//////////////////////////////////////////////////////////////////////////////
String
DigestAuthentication::getChallenge(const String& realm)
{
	return String("Digest realm=\"" + realm + "\", "
			"qop=\"auth\", nonce=\"" + generateNewNonce() + "\"");
}

} // end namespace OW_NAMESPACE

