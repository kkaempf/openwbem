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

#ifndef OW_HTTPUTILS_HPP_
#define OW_HTTPUTILS_HPP_
#include "OW_config.h"
#include "blocxx/Array.hpp"
#include "blocxx/Map.hpp"
#include "blocxx/String.hpp"
#include "OW_Base64FormatException.hpp"
#include <iosfwd>

namespace OW_NAMESPACE
{

typedef blocxx::Map<blocxx::String, blocxx::String> HTTPHeaderMap;
namespace HTTPUtils
{
	extern OW_HTTP_API const char* const HeaderValue_true;
	extern OW_HTTP_API const char* const HeaderValue_false;
	/**
	 * Read from an input stream, and fill out a <String, String> map and
	 * String Array representing the status line.
	 * @param map a <String, String> map to fill out
	 * @param array A String array to be filled out, representing the
	 * 	status line
	 * @param istr A istream& to read the headers from
	 * @return true if no errors occurred, false if an error occurred.
	 */
	OW_HTTP_API bool parseHeader(HTTPHeaderMap& map, blocxx::Array<blocxx::String>& array,
									std::istream& istr);
	/**
	 * Read from in input stream, parse HTTP headers into a <String,
	 * String> map (this is called by parseHeader(map, array, istr))
	 * @param map A <String, String> map to be filled out.
	 * @param istr A istream& to read the headers from
	 * @return true if no errors occurred, false if an error occurred.
	 */
	OW_HTTP_API bool parseHeader(HTTPHeaderMap& map, std::istream& istr);
	/**
	 * Get the date (for http headers)
	 * @return a String representing the current date/time
	 */
	OW_HTTP_API blocxx::String date( void );
	/**
	 * Return a descriptive string associated with a HTTP status code.
	 * @param code the status code to interpret.
	 * @return a String represetation of code
	 */
	OW_HTTP_API blocxx::String status2String(int code);
	/**
	 * Returns a two digit sequence for header counting.
	 * @return String containing a two digit number
	 */
	OW_HTTP_API blocxx::String getCounterStr();
	/**
	 * Decode Base64 encoded arg
	 * @throws (Base64FormatException)
	 */
	OW_HTTP_API blocxx::String base64Decode(const blocxx::String& arg);
	/**
	 * Decode Base64 encoded arg
	 * @param src NULL-terminated string to be encoded
	 * @throws (Base64FormatException)
	 */
	OW_HTTP_API blocxx::Array<char> base64Decode(const char* src);
	/**
	 * Encode src in Base64
	 */
	OW_HTTP_API blocxx::String base64Encode(const blocxx::String& arg);
	/**
	 * Encode src in Base64
	 * @param src NULL-terminated string to be encoded
	 */
	OW_HTTP_API blocxx::String base64Encode(const char* src);
	/**
	 * @param src pointer to data to be encoded.  The data may contain any
	 * value, including 0.
	 * @param len The length of the data to be encoded.
	 */
	OW_HTTP_API blocxx::String base64Encode(const blocxx::UInt8* src, size_t len);
#ifndef OW_DISABLE_DIGEST
	/* calculate H(A1) as per HTTP Digest spec */
	OW_HTTP_API void DigestCalcHA1(
					const blocxx::String &sAlg,
					const blocxx::String &sUserName,
					const blocxx::String &sRealm,
					const blocxx::String &sPassword,
					const blocxx::String &sNonce,
					const blocxx::String &sCNonce,
					blocxx::String &sSessionKey
					  );
	/* calculate request-digest/response-digest as per HTTP Digest spec */
	OW_HTTP_API void DigestCalcResponse(
					const blocxx::String &sHA1,			/* H(A1) */
					const blocxx::String &sNonce,		/* nonce from server */
					const blocxx::String &sNonceCount,	/* 8 hex digits */
					const blocxx::String &sCNonce,		/* client nonce */
					const blocxx::String &sQop,			/* qop-value: "", "auth", "auth-int" */
					const blocxx::String &sMethod,		/* method from the request */
					const blocxx::String &sDigestUri,	/* requested URL */
					const blocxx::String &sHEntity,		/* H(entity body) if qop="auth-int" */
					blocxx::String &Response
						   );
#endif
	/**
	 * Do the headers have a certain key?
	 * @param headers A Map<String, String> containing the headers
	 * @param key the key to look for.
	 * @return true if the headers contain the key
	 */
	OW_HTTP_API bool headerHasKey(const HTTPHeaderMap& headers,
		const blocxx::String& key);
	/**
	 * Get a value (based on a key/value pair) from the http headers
	 * @param headers A Map<String, String> containing the headers.
	 * @param key The key to look for.
	 * @return the value associated with the key
	 */
	OW_HTTP_API blocxx::String getHeaderValue(const HTTPHeaderMap& headers,
		const blocxx::String& key);
	/**
	 * Add a HTTP header
	 * @param headers a Array<String> containing the outgoing headers.
	 * @param key the key for the header (left of the ':')
	 * @param value the value for the header (right of the ':')
	 */
	OW_HTTP_API void addHeader(blocxx::Array<blocxx::String>& headers,
		const blocxx::String& key, const blocxx::String& value);
	/**
	 * Read from an input stream, until the end of the entity is reached.
	 * This is usefull when using a HTTPChunkedIStream or
	 * HTTPLenLimitIStream, and the XML parser (for instance), may
	 * not have read to the end of the entity.
	 * @param istr A istream& containing the entity (presumably a
	 * 	HTTPChunkedIStream, HTTPLenLimitIStream, or TempFileStream)
	 */
	OW_HTTP_API void eatEntity(std::istream& istr);
	/**
	 * Read from an input buffer, until the end of the entity is reached.
	 * This is usefull when using a HTTPChunkedIStream or
	 * HTTPLenLimitIStream, and the XML parser (for instance), may
	 * not have read to the end of the entity.
	 * @param istr A istream& containing the entity (presumably a
	 * 	HTTPChunkedIStream, HTTPLenLimitIStream, or TempFileStream)
	 */
	OW_HTTP_API void eatEntity(std::streambuf& buf);
	/**
	 * Base64Decode a user name/password.
	 * @param info a base64 encoded representation of a "<name>:<password>"
	 * 	string
	 * @param name The name gets assigned here.
	 * @param password the password gets assigned here.
	 */
	OW_HTTP_API void decodeBasicCreds(const blocxx::String& info, blocxx::String& name,
		blocxx::String& password);

	OW_HTTP_API bool buildMap(HTTPHeaderMap& map, std::istream& istr);
	/**
	 * Apply the standard URI [RFC 2396, section 2] escaping mechanism to
	 * the char c, using the ""%" HEX HEX" convention)
	 * @param c The char to escape
	 * @return The escaped char
	 */
	OW_HTTP_API blocxx::String escapeCharForURL(char c);

	OW_DECLARE_APIEXCEPTION(UnescapeCharForURL, OW_HTTP_API);
	/**
	 * Apply the standard URI [RFC 2396, section 2] unescaping mechanism to
	 * the String s, formatted in the ""%" HEX HEX" convention)
	 * @param str The string to unescape.  Only the first 3 characters are
	 *  considered.  Following characters are ignored.
	 * @return The unescaped char
	 * @throws unEscapeCharForURLException If the string doesn't contain a
	 *  valid escape sequence
	 */
	OW_HTTP_API char unescapeCharForURL(const char* str);
	/**
	 * Apply the standard URI [RFC 2396, section 2] escaping mechanism to
	 * the string input, using the ""%" HEX HEX" convention)
	 * @param input The string to escape
	 * @return The escaped string
	 */
	OW_HTTP_API blocxx::String escapeForURL(const blocxx::String& input);
	/**
	 * Apply the standard URI [RFC 2396, section 2] unescaping mechanism to
	 * the string input, using the ""%" HEX HEX" convention)
	 * @param input The string to unescape
	 * @return The unescaped string
	 */
	OW_HTTP_API blocxx::String unescapeForURL(const blocxx::String& input);
};

} // end namespace OW_NAMESPACE

namespace OW_HTTPUtils = OW_NAMESPACE::HTTPUtils;

#endif	// OW_HTTPUTILS_HPP_
