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


#include "OW_config.h"
#include "OW_DigestAuthentication.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include "OW_Array.hpp"
#include "OW_Map.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_DateTime.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Format.hpp"
#include "OW_MD5.hpp"
#include "OW_HTTPSvrConnection.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_RandomNumber.hpp"


#include <fstream>

using std::endl;
using std::ifstream;

//////////////////////////////////////////////////////////////////////////////
OW_DigestAuthentication::OW_DigestAuthentication(const OW_String& passwdFile)
	: m_asNonces()
	, m_aDateTimes()
	, m_passwdMap()
{
	if(passwdFile.empty())
	{
		OW_THROW(OW_AuthenticationException, "No password file given for "
			"digest authentication.");
	}

	ifstream infile(passwdFile.c_str());
	if(!infile)
	{
		OW_THROW(OW_AuthenticationException, format("Unable to open password file %1",
			passwdFile).c_str());
	}

	while(infile)
	{
		OW_String line;
		line = OW_String::getLine(infile);
		int idx = line.lastIndexOf(':');
		m_passwdMap[line.substring(0, idx)] = line.substring(idx + 1);
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
OW_String
OW_DigestAuthentication::getHash( const OW_String &sUserName,
	const OW_String &sRealm )
{
	OW_String key = sUserName + ":" + sRealm;
	return m_passwdMap[key];
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_DigestAuthentication::generateNewNonce( void )
{
	OW_DateTime DateTime;
	DateTime.setToCurrent();
	OW_String sDateTime( static_cast<OW_Int64>(DateTime.get()) );

	OW_String sPrivateData;

	OW_RandomNumber rn(0, 0x7FFFFFFF);
	sPrivateData.format( "%08x", rn.getNextNumber() );
	
	OW_MD5 md5;
	md5.update(sDateTime);
	md5.update(":");
	md5.update("ETag");	// TODO: This should be a real ETag
	md5.update(":");
	md5.update(sPrivateData);


	OW_String sNonce = md5.toString();

	sNonce = sDateTime + sNonce;

	sNonce = OW_HTTPUtils::base64Encode( sNonce );

	for ( OW_Int16 iNonce=m_asNonces.size()-1; iNonce>=0; iNonce-- )
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
parseInfo(const OW_String& pinfo, OW_Map<OW_String, OW_String>& infoMap)
{
	int idx = pinfo.indexOf("Digest");
	OW_String info;
	if (idx >= 0)
	{
		info = pinfo.substring(7);
	}
	else
	{
		OW_THROW(OW_AuthenticationException, "Error parsing Digest Response");
	}
	OW_Array<OW_String> infoAr = info.tokenize(",");
	for (size_t i = 0; i < infoAr.size(); ++i)
	{
		OW_String lhs, rhs;
		idx = infoAr[i].indexOf('=');
		if (idx > 0)
		{
			lhs = infoAr[i].substring(0, idx);
			lhs.trim();
			if (static_cast<size_t>(idx + 1) < infoAr[i].length())
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
		OW_THROW(OW_AuthenticationException, "Error parsing Digest Response");
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_DigestAuthentication::authorize(OW_String& userName,
		const OW_String& info, OW_HTTPSvrConnection* htcon)
{
	OW_String hostname = htcon->getHostName();
	if (info.empty())
	{
		htcon->setErrorDetails("You must authenticate to access this resource");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return OW_Bool(false);
	}

	OW_Map<OW_String, OW_String> infoMap;
	parseInfo(info, infoMap);

	OW_String sNonce = infoMap["nonce"];

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
		return OW_Bool(false);
	}

	userName = infoMap["username"];
	if ( userName.empty() )
	{
		htcon->setErrorDetails("No user name was provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return OW_Bool(false);
	}

	OW_String sRealm = infoMap["realm"];
	if ( sRealm.empty() )
	{
		htcon->setErrorDetails("No realm was provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return OW_Bool(false);
	}

	OW_String sNonceCount = infoMap["nc"];
	if ( sNonceCount.empty() )
	{
		htcon->setErrorDetails("No Nonce Count was provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return OW_Bool(false);
	}

	// TODO isn't cnonce optional?
	OW_String sCNonce = infoMap["cnonce"];
	if ( sCNonce.empty() )
	{
		htcon->setErrorDetails("No cnonce value provided");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return OW_Bool(false);
	}

	OW_String sResponse = infoMap["response"];
	if ( sResponse.empty() )
	{
		htcon->setErrorDetails("The response was zero length");
		htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
		return OW_Bool(false);
	}


	OW_String sHA1 = getHash( userName, sRealm );

	OW_String sTestResponse;
	OW_HTTPUtils::DigestCalcResponse( sHA1, sNonce, sNonceCount, sCNonce,
		"auth", htcon->getRequestLine()[ 0 ], htcon->getRequestLine()[1],
		"", sTestResponse );

	if ( sTestResponse == sResponse )
	{
		if (nonceFound)
		{
			htcon->addHeader("Authentication-Info",
				"qop=\"auth\", nextnonce=\"" + generateNewNonce() + "\"");
			return OW_Bool(true);
		}
		else
		{
			htcon->setErrorDetails("Nonce not found.");
			htcon->addHeader("WWW-Authenticate", getChallenge(hostname)
				+ ", stale=true");
			return OW_Bool(false);
		}
	}

	/*
	OW_String errDetails = "At end of doAuthenticate(): ";
	errDetails += "sHA1: >" + sHA1 + "< sNonce: >" + sNonce +
		"< Nonce Count >" + sNonceCount + "< sCNonce: >" +
		sCNonce + "< Method >" + htcon->getRequestLine()[0] +
		"< url >" + htcon->getRequestLine()[1] + "< testResponse >" + sTestResponse
		+ "< client Response >" + sResponse + "<";

	*/
	OW_String errDetails = "User name or password not valid";
	htcon->setErrorDetails(errDetails);
	htcon->addHeader("WWW-Authenticate", getChallenge(hostname));
	return OW_Bool(false);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_DigestAuthentication::getChallenge(const OW_String& hostname)
{
	return OW_String("Digest realm=\"" + hostname + "\", "
			"qop=\"auth\", nonce=\"" + generateNewNonce() + "\"");
}



