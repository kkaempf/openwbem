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

#ifndef OW_HTTPUTILS_HPP_
#define OW_HTTPUTILS_HPP_
#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_Map.hpp"
#include "OW_String.hpp"
#include "OW_Base64FormatException.hpp"
#include <iosfwd>

namespace OpenWBEM
{

typedef Map<String, String> HTTPHeaderMap;
namespace HTTPUtils
{
	extern const char* const Header_BypassLocker; 
	extern const char* const HeaderValue_true; 
	extern const char* const HeaderValue_false; 
	/**
	 * Read from an input stream, and fill out a <String, String> map and
	 * String Array representing the status line.
	 * @param map a <String, String> map to fill out
	 * @param array A String array to be filled out, representing the
	 * 	status line
	 * @param istr A istream& to read the headers from
	 * @return true if no errors occurred, false if an error occurred.
	 */
	bool parseHeader(HTTPHeaderMap& map, Array<String>& array,
									std::istream& istr);
	/**
	 * Read from in input stream, parse HTTP headers into a <String,
	 * String> map (this is called by parseHeader(map, array, istr))
	 * @param map A <String, String> map to be filled out.
	 * @param istr A istream& to read the headers from
	 * @return true if no errors occurred, false if an error occurred.
	 */
	bool parseHeader(HTTPHeaderMap& map, std::istream& istr);
	/**
	 * Get the date (for http headers)
	 * @return a String representing the current date/time
	 */
	String date( void );
	/**
	 * Return a descriptive string associated with a HTTP status code.
	 * @param code the status code to interpret.
	 * @return a String represetation of code
	 */
	String status2String(int code);
	/**
	 * Returns a two digit sequence for header counting.
	 * @return String containing a two digit number
	 */
	String getCounterStr();
	/**
	 * Decode Base64 encoded arg
	 * @throws (Base64FormatException)
	 */
	String base64Decode(const String& arg);
	/**
	 * Decode Base64 encoded arg
	 * @param src NULL-terminated string to be encoded
	 * @throws (Base64FormatException)
	 */
	Array<char> base64Decode(const char* src);
	/**
	 * Encode src in Base64
	 * @throws (Base64FormatException)
	 */
	String base64Encode(const String& arg);
	/**
	 * Encode src in Base64
	 * @param src NULL-terminated string to be encoded
	 * @throws (Base64FormatException)
	 */
	String base64Encode(const char* src);
	/**
	 * @param src pointer to data to be encoded.  The data may contain any
	 * value, including 0.
	 * @param len The length of the data to be encoded.
	 * @throws (Base64FormatException)
	 */
	String base64Encode(const UInt8* src, size_t len);
#ifndef OW_DISABLE_DIGEST
	/* calculate H(A1) as per HTTP Digest spec */
	void DigestCalcHA1(
					const String &sAlg,
					const String &sUserName,
					const String &sRealm,
					const String &sPassword,
					const String &sNonce,
					const String &sCNonce,
					String &sSessionKey
					  );
	/* calculate request-digest/response-digest as per HTTP Digest spec */
	void DigestCalcResponse(
					const String &sHA1,			/* H(A1) */
					const String &sNonce,		/* nonce from server */
					const String &sNonceCount,	/* 8 hex digits */
					const String &sCNonce,		/* client nonce */
					const String &sQop,			/* qop-value: "", "auth", "auth-int" */
					const String &sMethod,		/* method from the request */
					const String &sDigestUri,	/* requested URL */
					const String &sHEntity,		/* H(entity body) if qop="auth-int" */
					String &Response
						   );
#endif
	/**
	 * Do the headers have a certain key?
	 * @param headers A Map<String, String> containing the headers
	 * @param key the key to look for.
	 * @return true if the headers contain the key
	 */
	bool headerHasKey(const HTTPHeaderMap& headers,
		const String& key);
	/**
	 * Get a value (based on a key/value pair) from the http headers
	 * @param headers A Map<String, String> containing the headers.
	 * @param key The key to look for.
	 * @return the value associated with the key
	 */
	String getHeaderValue(const HTTPHeaderMap& headers,
		const String& key);
	/**
	 * Add a HTTP header
	 * @param headers a Array<String> containing the outgoing headers.
	 * @param key the key for the header (left of the ':')
	 * @param value the value for the header (right of the ':')
	 */
	void addHeader(Array<String>& headers,
		const String& key, const String& value);
	/**
	 * Read from an input stream, until the end of the entity is reached.
	 * This is usefull when using a HTTPChunkedIStream or
	 * HTTPLenLimitIStream, and the XML parser (for instance), may
	 * not have read to the end of the entity.
	 * @param istr A istream& containing the entity (presumably a
	 * 	HTTPChunkedIStream, HTTPLenLimitIStream, or TempFileStream)
	 */
	void eatEntity(std::istream& istr);
	/**
	 * Base64Decode a user name/password.
	 * @param info a base64 encoded representation of a "<name>:<password>"
	 * 	string
	 * @param name The name gets assigned here.
	 * @param password the password gets assigned here.
	 */
	void decodeBasicCreds(const String& info, String& name,
		String& password);
	
	bool buildMap(HTTPHeaderMap& map, std::istream& istr);
	/**
	 * Apply the standard URI [RFC 2396, section 2] escaping mechanism to 
	 * the char c, using the ""%" HEX HEX" convention)
	 * @param c The char to escape
	 * @return The escaped char
	 */
	String escapeCharForURL(char c);

	OW_DECLARE_EXCEPTION(UnescapeCharForURL);
	/**
	 * Apply the standard URI [RFC 2396, section 2] unescaping mechanism to 
	 * the String s, formatted in the ""%" HEX HEX" convention)
	 * @param str The string to unescape.  Only the first 3 characters are
	 *  considered.  Following characters are ignored.
	 * @return The unescaped char
	 * @throws unEscapeCharForURLException If the string doesn't contain a 
	 *  valid escape sequence
	 */
	char unescapeCharForURL(const char* str);
	/**
	 * Apply the standard URI [RFC 2396, section 2] escaping mechanism to 
	 * the string input, using the ""%" HEX HEX" convention)
	 * @param input The string to escape
	 * @return The escaped string
	 */
	String escapeForURL(const String& input);
	/**
	 * Apply the standard URI [RFC 2396, section 2] unescaping mechanism to 
	 * the string input, using the ""%" HEX HEX" convention)
	 * @param input The string to unescape
	 * @return The unescaped string
	 */
	String unescapeForURL(const String& input);
};

} // end namespace OpenWBEM

namespace OW_HTTPUtils = OpenWBEM::HTTPUtils;

#endif	// OW_HTTPUTILS_HPP_
