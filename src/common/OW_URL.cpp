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
#include "OW_URL.hpp"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
String
URL::toString() const
{
	String retval;
	if (!this->protocol.empty())
	{
		retval = this->protocol + "://";
	}
	if ( !this->username.empty())
	{
		retval += this->username + ":" + this->password + "@";
	}
	retval += this->host;
	if ( this->port > 0 )
	{
		retval += ":" + String(static_cast<UInt32>(this->port));
	}
	if ( !this->path.empty())
	{
		retval += this->path;
	}
	return retval;
}
//////////////////////////////////////////////////////////////////////////////
URL::URL()
: protocol(), username(), password(), host(), port(0), path()
{
}
//////////////////////////////////////////////////////////////////////////////
URL::URL(const String& sUrl): port(0)
{
	String sURL = sUrl;
	sURL.trim();
	size_t iBeginIndex = 0;
	size_t iEndIndex = sURL.indexOf( "://" );
	if ( iEndIndex != String::npos )
	{
		if ( iEndIndex > 0 )
			protocol = sURL.substring( 0, iEndIndex ).toLowerCase();
		iBeginIndex = iEndIndex + 3;
	}
	iEndIndex = sURL.indexOf( "@", iBeginIndex );
	if ( iEndIndex != String::npos )
	{
		String sNamePass = sURL.substring( iBeginIndex, iEndIndex - iBeginIndex );
		iBeginIndex = sNamePass.indexOf( ":" );
		if ( iBeginIndex != String::npos )
		{
			if ( iBeginIndex > 0 )
				username = sNamePass.substring( 0, iBeginIndex );
			if ( iBeginIndex < iEndIndex-1 )
				password = sNamePass.substring( iBeginIndex + 1 );
		}
		else if ( !sNamePass.empty())
			username = sNamePass;
		iBeginIndex = iEndIndex + 1;
	}
	iEndIndex = sURL.indexOf( "/", iBeginIndex );
	if ( iEndIndex != String::npos )
	{
		path = sURL.substring( iEndIndex );
		sURL = sURL.substring( iBeginIndex, iEndIndex - iBeginIndex );
	}
	else
		sURL = sURL.substring( iBeginIndex );
	iBeginIndex = sURL.indexOf( ":" );
	if ( iBeginIndex != String::npos )
	{
		host = sURL.substring( 0, iBeginIndex );
		if ( sURL.length() > iBeginIndex+1 )
		{
			try
			{
				port = sURL.substring( iBeginIndex + 1 ).toUInt16();
			}
			catch (const StringConversionException&)
			{
				OW_THROW(Exception, String("Invalid URL: " + sUrl).c_str());
			}
		}
	}
	else if ( iBeginIndex && !sURL.empty() )
		host = sURL;
	else
	{
		OW_THROW(Exception, String("Invalid URL: " + sUrl).c_str());
	}
}

} // end namespace OpenWBEM

