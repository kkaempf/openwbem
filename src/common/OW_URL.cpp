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
#include "OW_URL.hpp"
#include "OW_Exception.hpp"

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_URL::toString() const
{
	OW_String retval;
	if (this->protocol.length() > 0)
	{
		retval = this->protocol + "://";
	}
	if ( this->username.length() > 0 )
	{
		retval += this->username + ":" + this->password + "@";
	}
	retval += this->host;
	if ( this->port > 0 )
	{
		retval += ":" + OW_String((OW_UInt32)this->port);
	}
	if ( this->path.length() > 0 )
	{
		retval += this->path;
	}
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
OW_URL::OW_URL()
: protocol(), username(), password(), host(), port(0), path()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_URL::OW_URL(const OW_String& sUrl): port(0)
{
	OW_String sURL = sUrl;
	sURL.trim();

	OW_Int16 iBeginIndex = 0;
	OW_Int16 iEndIndex = sURL.indexOf( "://" );

	if ( iEndIndex >= 0 )
	{
		if ( iEndIndex > 0 )
			protocol = sURL.substring( 0, iEndIndex ).toLowerCase();
		iBeginIndex = iEndIndex + 3;
	}

	iEndIndex = sURL.indexOf( "@", iBeginIndex );
	if ( iEndIndex >= 0 )
	{
		OW_String sNamePass = sURL.substring( iBeginIndex, iEndIndex - iBeginIndex );
		iBeginIndex = sNamePass.indexOf( ":" );
		if ( iBeginIndex >= 0 )
		{
			if ( iBeginIndex > 0 )
				username = sNamePass.substring( 0, iBeginIndex );
			if ( iBeginIndex < iEndIndex-1 )
				password = sNamePass.substring( iBeginIndex + 1 );
		}
		else if ( sNamePass.length() )
			username = sNamePass;
		iBeginIndex = iEndIndex + 1;
	}

	iEndIndex = sURL.indexOf( "/", iBeginIndex );
	if ( iEndIndex >= 0 )
	{
		path = sURL.substring( iEndIndex );
		sURL = sURL.substring( iBeginIndex, iEndIndex - iBeginIndex );
	}
	else
		sURL = sURL.substring( iBeginIndex );

	iBeginIndex = sURL.indexOf( ":" );
	if ( iBeginIndex > 0 )
	{
		host = sURL.substring( 0, iBeginIndex );
		if ( sURL.length() > unsigned(iBeginIndex)+1 )
			port = sURL.substring( iBeginIndex + 1 ).toUInt16();
	}
	else if ( iBeginIndex && sURL.length() )
		host = sURL;
	else
	{
		OW_THROW(OW_Exception, "Invalid URL");
	}
}


