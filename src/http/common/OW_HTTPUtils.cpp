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
#include "OW_HTTPUtils.hpp"
#include "OW_DateTime.hpp"
#include "OW_HTTPStatusCodes.hpp"
#include "OW_HTTPCounter.hpp"
#include "OW_HTTPException.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_AuthenticatorIFC.hpp" // for OW_AuthenticationException
#include "OW_MD5.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Format.hpp"
#include "OW_ExceptionIds.hpp"

#include <algorithm> // for std::find
#include <cctype>
#include <cstring>
#include <cstdio>
#ifdef OW_HAVE_ISTREAM
#include <istream>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(Base64Format);

namespace HTTPUtils
{
using std::istream;

///////////////////////////////////////////////////////////////////////////////
const char* const Header_BypassLocker = "OW_BypassLocker"; 
const char* const HeaderValue_true = "true"; 
const char* const HeaderValue_false = "false"; 
///////////////////////////////////////////////////////////////////////////////
bool
parseHeader(HTTPHeaderMap& map, Array<String>& array, istream& istr)
{
	String line;
	do
	{ // leading empty lines should be ignored.
		line = String::getLine(istr);
	} while ( line.isSpaces() && istr );

	if ( !istr )
	{
		return false;
	}
	
	array = line.tokenize();
	return buildMap(map, istr);
}
//////////////////////////////////////////////////////////////////////////////
// static
bool
parseHeader(HTTPHeaderMap& map, istream& istr)
{
	return buildMap(map, istr);
}
//////////////////////////////////////////////////////////////////////////////
bool
buildMap(HTTPHeaderMap& map, istream& istr)
{
	String line;
	String key;
	while ( istr )
	{
		line = String::getLine(istr);
		if ( line.isSpaces() )
		{
			break; // blank line; end of header.
		}
		size_t len = line.length();
		if ( len > line.ltrim().length() ) // continuation "folded" line.
		{
			if ( key.length() > 1 )	// make sure there is a previous key.
			{
				map[key].concat(" ");
				map[key].concat(line.rtrim());
				continue;  // "folding" is probably rare, so this should be sufficient
			}
			else
			{
				return false; // continuation expected, but no previous key.
				// TODO maybe we should silently ignore bad header
				// data instead
			}
		}
		size_t idx = line.indexOf(':');
		if ( idx == String::npos )	// no ':' found, and not a continuation line.
		{
			return false;
		}
		key = line.substring(0, idx).toLowerCase();
		if ( map.count(key) == 0 )
		{
			map[key] = line.substring(idx + 1).trim();
		}
		else
		{
			// multiple headers with the same name (key) MAY be
			// present if subsequent headers represent the continuation
			// of a comma-seperated list of values.
			map[key].concat(", ");
			map[key].concat(line.substring(idx + 1).trim());
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////
String date( void )
{
	DateTime DateTime;
	DateTime.setToCurrent();
	Array< String > DateTimeArray = DateTime.toString(DateTime::E_UTC_TIME).tokenize();
	if ( DateTimeArray.size() < 5 )
	{
		OW_THROW(HTTPException, "DateTimeArray has less than 5 elements.");
	}
	String HTTPDateTime = DateTimeArray[ 0 ] + ", " + DateTimeArray[ 2 ] + " " +
		DateTimeArray[ 1 ] + " " + DateTimeArray[ 4 ] + " " + DateTimeArray[ 3 ] + " GMT";
	return HTTPDateTime;
}
//////////////////////////////////////////////////////////////////////////////
String
status2String(int code)
{
	switch ( code )
	{
	case SC_CONTINUE:
		return String("Continue");
	case SC_SWITCHING_PROTOCOLS:
		return String("Switching protocols");
	case SC_OK:
		return String("Ok");
	case SC_CREATED:
		return String("Created");
	case SC_ACCEPTED:
		return String("Accepted");
	case SC_NON_AUTHORITATIVE_INFORMATION:
		return String("Non-authoritative");
	case SC_NO_CONTENT:
		return String("No content");
	case SC_RESET_CONTENT:
		return String("Reset content");
	case SC_PARTIAL_CONTENT:
		return String("Partial content");
	case SC_MULTIPLE_CHOICES:
		return String("Multiple choices");
	case SC_MOVED_PERMANENTLY:
		return String("Moved permanentently");
	case SC_MOVED_TEMPORARILY:
		return String("Moved temporarily");
	case SC_SEE_OTHER:
		return String("See other");
	case SC_NOT_MODIFIED:
		return String("Not modified");
	case SC_USE_PROXY:
		return String("Use proxy");
	case SC_BAD_REQUEST:
		return String("Bad request");
	case SC_UNAUTHORIZED:
		return String("Unauthorized");
	case SC_PAYMENT_REQUIRED:
		return String("Payment required");
	case SC_FORBIDDEN:
		return String("Forbidden");
	case SC_NOT_FOUND:
		return String("Not found");
	case SC_METHOD_NOT_ALLOWED:
		return String("Method not allowed");
	case SC_NOT_ACCEPTABLE:
		return String("Not acceptable");
	case SC_PROXY_AUTHENTICATION_REQUIRED:
		return String("Proxy auth required");
	case SC_REQUEST_TIMEOUT:
		return String("Request timeout");
	case SC_CONFLICT:
		return String("Conflict");
	case SC_GONE:
		return String("Gone");
	case SC_LENGTH_REQUIRED:
		return String("Length required");
	case SC_PRECONDITION_FAILED:
		return String("Precondition failed");
	case SC_REQUEST_ENTITY_TOO_LARGE:
		return String("Request entity too large");
	case SC_REQUEST_URI_TOO_LONG:
		return String("Request URI too large");
	case SC_UNSUPPORTED_MEDIA_TYPE:
		return String("Unsupported media type");
	case SC_INTERNAL_SERVER_ERROR:
		return String("Internal server error");
	case SC_NOT_IMPLEMENTED:
		return String("Not implemented");
	case SC_BAD_GATEWAY:
		return String("Bad gateway");
	case SC_SERVICE_UNAVAILABLE:
		return String("Service unavailable");
	case SC_GATEWAY_TIMEOUT:
		return String("Gateway timeout");
	case SC_HTTP_VERSION_NOT_SUPPORTED:
		return  String("HTTP version not supported");
	case SC_NOT_EXTENDED:
		return String("Not Extended");
	default:
		return String() ;
	}
}
//////////////////////////////////////////////////////////////////////////////
static HTTPCounter theCounter;
//////////////////////////////////////////////////////////////////////////////
String
getCounterStr()
{
	int count = theCounter.getNextCounter();
	// string should always be two digits.
	if ( count < 10 )
	{
		return("0" + String(count));
	}
	else
	{
		return String(count);
	}
}

//////////////////////////////////////////////////////////////////////////////
static const char* const Base64 =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';
//////////////////////////////////////////////////////////////////////////////
String base64Decode(const String& arg)
{
	// only up to the first '\0' will be returned.
	return String(&base64Decode(arg.c_str())[0]);
}
static int char2val(char c)
{
	switch (c)
	{
		case 'A': return 0;
		case 'B': return 1;
		case 'C': return 2;
		case 'D': return 3;
		case 'E': return 4;
		case 'F': return 5;
		case 'G': return 6;
		case 'H': return 7;
		case 'I': return 8;
		case 'J': return 9;
		case 'K': return 10;
		case 'L': return 11;
		case 'M': return 12;
		case 'N': return 13;
		case 'O': return 14;
		case 'P': return 15;
		case 'Q': return 16;
		case 'R': return 17;
		case 'S': return 18;
		case 'T': return 19;
		case 'U': return 20;
		case 'V': return 21;
		case 'W': return 22;
		case 'X': return 23;
		case 'Y': return 24;
		case 'Z': return 25;
		case 'a': return 26;
		case 'b': return 27;
		case 'c': return 28;
		case 'd': return 29;
		case 'e': return 30;
		case 'f': return 31;
		case 'g': return 32;
		case 'h': return 33;
		case 'i': return 34;
		case 'j': return 35;
		case 'k': return 36;
		case 'l': return 37;
		case 'm': return 38;
		case 'n': return 39;
		case 'o': return 40;
		case 'p': return 41;
		case 'q': return 42;
		case 'r': return 43;
		case 's': return 44;
		case 't': return 45;
		case 'u': return 46;
		case 'v': return 47;
		case 'w': return 48;
		case 'x': return 49;
		case 'y': return 50;
		case 'z': return 51;
		case '0': return 52;
		case '1': return 53;
		case '2': return 54;
		case '3': return 55;
		case '4': return 56;
		case '5': return 57;
		case '6': return 58;
		case '7': return 59;
		case '8': return 60;
		case '9': return 61;
		case '+': return 62;
		case '/': return 63;
		default: return -1;
	}
}
//////////////////////////////////////////////////////////////////////////////
Array<char> base64Decode(const char* src)
{
	int szdest = strlen(src) * 2;
	// TODO this is likely too big, but safe.  figure out correct minimal size.
	AutoPtrVec<char> dest(new char[szdest]);
	memset(dest.get(), '\0', szdest);
	int destidx, state, ch;
	int b64val;
	state = 0;
	destidx = 0;
	while ( (ch = *src++) != '\0' )
	{
		if ( isspace(ch) ) // Skip whitespace
		{
			continue;
		}
		if ( ch == Pad64 )
		{
			break;
		}
		b64val = char2val(ch);
		if ( b64val == -1 ) // A non-base64 character
		{
			OW_THROW(Base64FormatException, "non-base64 char");
		}
		switch ( state )
		{
		case 0:
			if ( destidx >= szdest )
			{
				OW_THROW(Base64FormatException, "non-base64 char");
			}
			dest[destidx] = b64val << 2;
			state = 1;
			break;
		case 1:
			if ( destidx + 1 >= szdest )
			{
				OW_THROW(Base64FormatException, "non-base64 char");
			}
			dest[destidx]   |=  b64val >> 4;
			dest[destidx+1]  = (b64val & 0x0f) << 4 ;
			destidx++;
			state = 2;
			break;
		case 2:
			if ( destidx + 1 >= szdest )
			{
				OW_THROW(Base64FormatException, "non-base64 char");
			}
			dest[destidx]   |=  b64val >> 2;
			dest[destidx+1]  = (b64val & 0x03) << 6;
			destidx++;
			state = 3;
			break;
		case 3:
			if ( destidx >= szdest )
			{
				OW_THROW(Base64FormatException, "non-base64 char");
			}
			dest[destidx] |= b64val;
			destidx++;
			state = 0;
			break;
		}
	}
	// We are done decoding Base-64 chars.  Let's see if we ended
	//	on a byte boundary, and/or with erroneous trailing characters.
	if ( ch == Pad64 )	  // We got a pad char
	{
		ch = *src++;		// Skip it, get next
		switch ( state )
		{
		case 0:		// Invalid = in first position
		case 1:		// Invalid = in second position
			OW_THROW(Base64FormatException, "non-base64 char");
		case 2:		// Valid, means one byte of info
			// Skip any number of spaces
			for ( ; ch != '\0'; ch = *src++ )
			{
				if ( !isspace(ch) )
				{
					break;
				}
			}
				// Make sure there is another trailing = sign
			if ( ch != Pad64 )
			{
				OW_THROW(Base64FormatException, "non-base64 char");
			}
			ch = *src++;		// Skip the =
			// Fall through to "single trailing =" case
			// FALLTHROUGH
		case 3:		// Valid, means two bytes of info
			// We know this char is an =.  Is there anything but
			//	whitespace after it?
			for ( ; ch != '\0'; ch = *src++ )
			{
				if ( !isspace(ch) )
				{
					OW_THROW(Base64FormatException, "non-base64 char");
				}
			}
				// Now make sure for cases 2 and 3 that the "extra"
				//	bits that slopped past the last full byte were
				//	zeros.  If we don't check them, they become a
				//	subliminal channel.
			if ( dest[destidx] != 0 )
			{
				OW_THROW(Base64FormatException, "non-base64 char");
			}
		}
	}
	else
	{
		// We ended by seeing the end of the string.  Make sure we
		//	have no partial bytes lying around.
		if ( state != 0 )
		{
			OW_THROW(Base64FormatException, "non-base64 char");
		}
	}
	Array<char> rval(dest.get(), dest.get()+destidx+1);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
String base64Encode(const String& arg)
{
	return base64Encode(reinterpret_cast<const UInt8*>(arg.c_str()), arg.length());
}
//////////////////////////////////////////////////////////////////////////////
String base64Encode(const char* src)
{
	return base64Encode(reinterpret_cast<const UInt8*>(src), ::strlen(src));
}

//////////////////////////////////////////////////////////////////////////////
String base64Encode(const UInt8* src, size_t len)
{
	int szdest = len * 3 + 4;
	// TODO this is likely too big, but safe.  figure out correct minimal size.
	AutoPtrVec<char> dest(new char[szdest]);
	dest[0] = '\0'; // null terminate in case input is empty
	char a, b, c, d, *dst;
	const UInt8* cp;
	int i, srclen, enclen, remlen;
	cp = src;
	dst = dest.get();
	srclen = len;			// length of source
	enclen = srclen / 3;			  // number of 4 byte encodings (source DIV 3)
	remlen = srclen - 3 * enclen;	// remainder if srclen not divisible by 3 (source MOD 3)
	for ( i = 0; i < enclen; i++ )
	{
		a = (cp[0] >> 2);
		b = (cp[0] << 4) & 0x30;
		b |= (cp[1] >> 4);
		c = (cp[1] << 2) & 0x3c;
		c |= (cp[2] >> 6);
		d = cp[2] & 0x3f;
		cp +=3;
		if ( dst + 6 - dest.get() > szdest )
		{
			OW_THROW(Base64FormatException, "buffer too small");
		}
		sprintf(dst, "%c%c%c%c",Base64[a],Base64[b],Base64[c],Base64[d]);
		dst+=4;
	}
	if ( remlen == 1 )
	{
		a = (cp[0] >> 2);
		b = (cp[0] << 4) & 0x30;
		if ( dst + 6 - dest.get() > szdest )
		{
			OW_THROW(Base64FormatException, "buffer too small");
		}
		sprintf(dst, "%c%c==",Base64[a],Base64[b]);
		dst+=4;
	}
	else if ( remlen == 2 )
	{
		a = (cp[0] >> 2);
		b = (cp[0] << 4) & 0x30 ;
		b |= (cp[1] >> 4);
		c = (cp[1] << 2) & 0x3c;
		if ( dst + 6 - dest.get() > szdest )
		{
			OW_THROW(Base64FormatException, "non-base64 char");
		}
		sprintf(dst, "%c%c%c=",Base64[a],Base64[b],Base64[c]);
		dst+=4;
	}
	String rval(String::E_TAKE_OWNERSHIP, dest.release(), dst-dest.get());
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
#ifndef OW_DISABLE_DIGEST
/* calculate H(A1) as per spec */
void DigestCalcHA1(
	const String &sAlg,
	const String &sUserName,
	const String &sRealm,
	const String &sPassword,
	const String &sNonce,
	const String &sCNonce,
	String &sSessionKey
	)
{
	MD5 md5;
	md5.update(sUserName);
	md5.update(":");
	md5.update(sRealm);
	md5.update(":");
	md5.update(sPassword);
	sSessionKey = md5.toString();
	if ( sAlg.equalsIgnoreCase( "md5-sess" ))
	{
		unsigned char sHA1[MD5HASHLEN];
		memcpy(sHA1, md5.getDigest(), MD5HASHLEN);
		MD5 md5_2;
		md5_2.update(reinterpret_cast<const char*>(sHA1));
		md5_2.update(":");
		md5_2.update(sNonce);
		md5_2.update(":");
		md5_2.update(sCNonce);
		sSessionKey = md5_2.toString();
	};
};
/* calculate request-digest/response-digest as per HTTP Digest spec */
void DigestCalcResponse(
	 const String& sHA1,			 /* H(A1) */
	 const String& sNonce,		 /* nonce from server */
	 const String& sNonceCount, /* 8 hex digits */
	 const String& sCNonce,		 /* client nonce */
	 const String& sQop,			 /* qop-value: "", "auth", "auth-int" */
	 const String& sMethod,		 /* method from the request */
	 const String& sDigestUri,	 /* requested URL */
	 const String& sHEntity,	 /* H(entity body) if qop="auth-int" */
	 String& sResponse
	 )
{
	String sHA2Hex;
	// calculate H(A2)
	MD5 md5;
	md5.update(sMethod);
	md5.update(":");
	md5.update(sDigestUri);
	if ( sQop.equalsIgnoreCase( "auth-int" ))
	{
		md5.update(":");
		md5.update(sHEntity);
	};
	sHA2Hex = md5.toString();
	// calculate response
	MD5 md5new;
	md5new.update(sHA1);
	md5new.update(":");
	md5new.update(sNonce);
	md5new.update(":");
	if ( !sQop.empty())
	{
		md5new.update(sNonceCount);
		md5new.update(":");
		md5new.update(sCNonce);
		md5new.update(":");
		md5new.update(sQop);
		md5new.update(":");
	};
	md5new.update(sHA2Hex);
	sResponse = md5new.toString();
};
#endif
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
headerHasKey(const HTTPHeaderMap& headers, const String& key)
{
	HTTPHeaderMap::const_iterator i =
	headers.find(key.toString().toLowerCase());
	if ( i != headers.end() )
	{
		return true;
	}
	else
	{
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
String
getHeaderValue(const HTTPHeaderMap& headers, const String& key)
{
	HTTPHeaderMap::const_iterator i =
	headers.find(key.toString().toLowerCase());
	if ( i != headers.end() )
	{
		return(*i).second;
	}
	else
	{
		return String();
	}
}
//////////////////////////////////////////////////////////////////////////////
//STATIC
void
addHeader(Array<String>& headers, const String& key, const String& value)
{
	String tmpKey = key;
	tmpKey.trim();
	if ( !tmpKey.empty())
	{
		String newHeader = key + ": " + value;
		if (std::find(headers.begin(), headers.end(), newHeader) == headers.end())
		{
			headers.push_back(newHeader); 
		}
	}
	else
	{ // a "folded" continuation line
		headers.push_back(" " + value);
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
eatEntity(istream& istr)
{
	while ( istr )	
	{
		istr.get();
	}
}
void
decodeBasicCreds(const String& info, String& name, String& password)
{
	String decoded = info;
	size_t idx = decoded.indexOf("Basic");
	if (idx == String::npos)
	{
		OW_THROW(AuthenticationException, "Authentication info is not type "
			"\"Basic\"");
	}
	decoded = decoded.substring(idx + 6);
	try
	{
		decoded = base64Decode(decoded);
	}
	catch (Base64FormatException &e)
	{
		OW_THROW(AuthenticationException, "invalid BASE64 encoding of "
			"credentials");
	}
	size_t icolon = decoded.indexOf(':');
	if (icolon == String::npos)
	{
		OW_THROW(AuthenticationException, "invalid credentials syntax");
	}
	else
	{
		name = decoded.substring(0,icolon);
		password = decoded.substring(icolon + 1);
	}
}

/////////////////////////////////////////////////////////////////////////////
String escapeCharForURL(char c)
{
	char rval[4];
	rval[0] = '%';
	UInt8 hi = UInt8(c) >> 4;
	rval[1] = hi >= 10 ? hi - 10 + 'A' : hi + '0';
	UInt8 low = UInt8(c) & 0xF;
	rval[2] = low >= 10 ? low - 10 + 'A' : low + '0';
	rval[3] = '\0';
	return String(rval);
}

OW_DEFINE_EXCEPTION_WITH_ID(UnescapeCharForURL);

namespace {
	inline char digitToVal(char c)
	{
		return isdigit(c) ? c - '0' : toupper(c) - 'A' + 10;
	}
}

/////////////////////////////////////////////////////////////////////////////
char unescapeCharForURL(const char* str)
{
	if (strlen(str) < 3 || str[0] != '%' || !isxdigit(str[1]) || !isxdigit(str[2]))
	{
		OW_THROW(UnescapeCharForURLException, Format("Invalid escape: %1", str).c_str());
	}
	return (digitToVal(str[1]) << 4 ) | digitToVal(str[2]);
}

/////////////////////////////////////////////////////////////////////////////
String escapeForURL(const String& input)
{
	StringBuffer rval;
	for (size_t i = 0; i < input.length(); ++i)
	{
		switch (input[i])
		{
		// see rfc 2396 section 2
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
		case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
		case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
		case 'Y': case 'Z': case 'a': case 'b': case 'c': case 'd':
		case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
		case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
		case 'q': case 'r': case 's': case 't': case 'u': case 'v':
		case 'w': case 'x': case 'y': case 'z': 
		
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
		
		case '-': case '_': case '.': case '!': case '~': 
		case '*': case '\'': case '(': case ')':
			rval += input[i];
			break;
		default:
			rval += escapeCharForURL(input[i]);
			break;
		}
	}
	return rval.releaseString();
}

/////////////////////////////////////////////////////////////////////////////
String unescapeForURL(const String& input)
{
	StringBuffer rval(input.length());
	const char* pos = input.c_str();
	while (*pos != '\0')
	{
		if (*pos == '%')
		{
			rval += unescapeCharForURL(pos);
			pos += 3;
		}
		else
		{
			rval += *pos;
			++pos;
		}
	}
	return rval.releaseString();
}
	

} // end namespace HTTPUtils
} // end namespace OpenWBEM

