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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_URL.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_ExceptionIds.hpp"

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(MalformedURL);

//////////////////////////////////////////////////////////////////////////////
String
URL::toString() const
{
	StringBuffer retval;
	if (!scheme.empty())
	{
		retval += scheme;
		retval += "://";
	}
	if (!principal.empty() || !credential.empty())
	{
		retval += principal;
		if (!credential.empty())
		{
			retval += ':';
			retval += credential;
		}
		retval += '@';
	}
	retval += host;
	if (!port.empty())
	{
		retval += ":";
		retval += port;
	}
	if (!namespaceName.empty())
	{
		retval += '/';
		retval += namespaceName;
		// can only have a modelPath if we have namespaceName
		if (!modelPath.empty())
		{
			retval += "/:";
			retval += modelPath;
		}
	}
	return retval.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
URL::URL() // default for all data members is okay.
{
}

//////////////////////////////////////////////////////////////////////////////
URL::URL(const String& sUrl) // default for all data members is okay.
{
	String sURL = sUrl;
	sURL.trim();

	// get the scheme
	size_t iBeginIndex = 0;
	size_t iEndIndex = sURL.indexOf( "://" );
	if ( iEndIndex != String::npos )
	{
		if ( iEndIndex > 0 )
			scheme = sURL.substring( 0, iEndIndex ).toLowerCase();
		iBeginIndex = iEndIndex + 3;
	}
	// get the userinfo
	iEndIndex = sURL.indexOf( '@', iBeginIndex );
	if ( iEndIndex != String::npos )
	{
		String sNamePass = sURL.substring( iBeginIndex, iEndIndex - iBeginIndex );
		iBeginIndex = sNamePass.indexOf( ':' );
		if ( iBeginIndex != String::npos )
		{
			if ( iBeginIndex > 0 )
				principal = sNamePass.substring( 0, iBeginIndex );
			if ( iBeginIndex < iEndIndex-1 )
				credential = sNamePass.substring( iBeginIndex + 1 );
		}
		else if ( !sNamePass.empty())
			principal = sNamePass;
		iBeginIndex = iEndIndex + 1;
	}
	// get host[:port]
	iEndIndex = sURL.indexOf( '/', iBeginIndex );
	if (iEndIndex == String::npos)
	{
		iEndIndex = sURL.length();
	}

	String hostPort = sURL.substring(iBeginIndex, iEndIndex - iBeginIndex);
	if (hostPort.empty())
	{
		OW_THROW(MalformedURLException, String("Invalid URL: " + sUrl).c_str());
	}
	size_t colonIdx = hostPort.indexOf( ':' );
	if ( colonIdx != String::npos )
	{
		host = hostPort.substring( 0, colonIdx );
		port = hostPort.substring( colonIdx + 1 );
	}
	else
	{
		host = hostPort;
	}
	if (host.empty())
	{
		OW_THROW(MalformedURLException, String("Invalid URL: " + sUrl).c_str());
	}

	// get namespaceName
	iBeginIndex = sURL.indexOf('/', iBeginIndex);
	if ( iBeginIndex != String::npos )
	{
		iEndIndex = sURL.indexOf("/:",  iBeginIndex);
		if (iEndIndex == String::npos)
		{
			iEndIndex = sURL.length();
		}
		namespaceName = sURL.substring( iBeginIndex+1, iEndIndex - (iBeginIndex+1) );

		// get modelPath
		iBeginIndex = sURL.indexOf("/:", iBeginIndex);
		if (iBeginIndex != String::npos)
		{
			modelPath = sURL.substring(iBeginIndex + 2); // this is the last piece, so grab everything to the end.
		}
	}
}

// known schemes
const char* const URL::CIMXML_WBEM = "cimxml.wbem";
const char* const URL::CIMXML_WBEMS = "cimxml.wbems";
const char* const URL::HTTP = "http";
const char* const URL::HTTPS = "https";
const char* const URL::OWBINARY_WBEM = "owbinary.wbem";
const char* const URL::OWBINARY_WBEMS = "owbinary.wbems";
const char* const URL::OWBINARY = "owbinary";

// special port
const char* const URL::OWIPC = "owipc";


} // end namespace OpenWBEM

