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

#ifndef OW_HTTPUTILS_HPP_
#define OW_HTTPUTILS_HPP_

#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_Map.hpp"
#include "OW_String.hpp"
#include "OW_Base64FormatException.hpp"
#include <iosfwd>


#define HASHLEN 16
#define HASHHEXLEN 32

typedef OW_Map<OW_String, OW_String> OW_HTTPHeaderMap;

class OW_HTTPUtils
{
public:

	/**
	 * Read from an input stream, and fill out a <OW_String, OW_String> map and
	 * OW_String Array representing the status line.
	 * @param map a <OW_String, OW_String> map to fill out
	 * @param array A OW_String array to be filled out, representing the
	 * 	status line
	 * @param istr A istream& to read the headers from
	 * @return true if no errors occurred, false if an error occurred.
	 */
	static OW_Bool parseHeader(OW_HTTPHeaderMap& map, OW_Array<OW_String>& array,
									std::istream& istr);

	/**
	 * Read from in input stream, parse HTTP headers into a <OW_String,
	 * OW_String> map (this is called by parseHeader(map, array, istr))
	 * @param map A <OW_String, OW_String> map to be filled out.
	 * @param istr A istream& to read the headers from
	 * @return true if no errors occurred, false if an error occurred.
	 */
	static OW_Bool parseHeader(OW_HTTPHeaderMap& map, std::istream& istr);


	/**
	 * Get the date (for http headers)
	 * @return a String representing the current date/time
	 */
	static OW_String date( void );

	/**
	 * Return a descriptive string associated with a HTTP status code.
	 * @param code the status code to interpret.
	 * @return a String represetation of code
	 */
	static OW_String status2String(int code);

	/**
	 * Returns a two digit sequence for header counting.
	 * @return OW_String containing a two digit number
	 */
	static OW_String getCounterStr();

	// Decode Base64 encoded src into dest, returns the number of data bytes
	//		stored in dest, or -1 on error
	static OW_String base64Decode(const OW_String& arg)
	/*throw (OW_Base64FormatException)*/;
	static OW_String base64Decode(const char* src)
	/*throw (OW_Base64FormatException)*/;
	// Encode src in Base64 into dest, returns the number of data bytes
	//		stored in dest, or -1 on error
	static OW_String base64Encode(const OW_String& arg)
	/*throw (OW_Base64FormatException)*/;
	static OW_String base64Encode(const char* src)
	/*throw (OW_Base64FormatException)*/;

#ifndef OW_DISABLE_DIGEST
	/* calculate H(A1) as per HTTP Digest spec */
	static void DigestCalcHA1(
					const OW_String &sAlg,
					const OW_String &sUserName,
					const OW_String &sRealm,
					const OW_String &sPassword,
					const OW_String &sNonce,
					const OW_String &sCNonce,
					OW_String &sSessionKey
					  );

	/* calculate request-digest/response-digest as per HTTP Digest spec */
	static void DigestCalcResponse(
					const OW_String &sHA1,			/* H(A1) */
					const OW_String &sNonce,		/* nonce from server */
					const OW_String &sNonceCount,	/* 8 hex digits */
					const OW_String &sCNonce,		/* client nonce */
					const OW_String &sQop,			/* qop-value: "", "auth", "auth-int" */
					const OW_String &sMethod,		/* method from the request */
					const OW_String &sDigestUri,	/* requested URL */
					const OW_String &sHEntity,		/* H(entity body) if qop="auth-int" */
					OW_String &Response
						   );
#endif

	/**
	 * Do the headers have a certain key?
	 * @param headers A OW_Map<OW_String, OW_String> containing the headers
	 * @param key the key to look for.
	 * @return true if the headers contain the key
	 */
	static OW_Bool headerHasKey(const OW_HTTPHeaderMap& headers,
		const OW_String& key);

	/**
	 * Get a value (based on a key/value pair) from the http headers
	 * @param headers A OW_Map<OW_String, OW_String> containing the headers.
	 * @param key The key to look for.
	 * @return the value associated with the key
	 */
	static OW_String getHeaderValue(const OW_HTTPHeaderMap& headers,
		const OW_String& key);

	/**
	 * Add a HTTP header
	 * @param headers a OW_Array<OW_String> containing the outgoing headers.
	 * @param key the key for the header (left of the ':')
	 * @param value the value for the header (right of the ':')
	 */
	static void addHeader(OW_Array<OW_String>& headers,
		const OW_String& key, const OW_String& value);

	/**
	 * Read from an input stream, until the end of the entity is reached.
	 * This is usefull when using a OW_HTTPChunkedIStream or
	 * OW_HTTPLenLimitIStream, and the XML parser (for instance), may
	 * not have read to the end of the entity.
	 * @param istr A istream& containing the entity (presumably a
	 * 	OW_HTTPChunkedIStream, OW_HTTPLenLimitIStream, or OW_TempFileStream)
	 */
	static void eatEntity(std::istream& istr);

	/**
	 * Base64Decode a user name/password.
	 * @param info a base64 encoded representation of a "<name>:<password>"
	 * 	string
	 * @param name The name gets assigned here.
	 * @param password the password gets assigned here.
	 */
	static void decodeBasicCreds(const OW_String& info, OW_String& name,
		OW_String& password);
	
private:
	OW_HTTPUtils();  // Don't allow instantiation
	static OW_Bool buildMap(OW_HTTPHeaderMap& map, std::istream& istr);
};


#endif	// OW_HTTPUTILS_HPP_
