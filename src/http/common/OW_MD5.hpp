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

/**
 * Taken from RFC 1321.  The following disclaimer is contained therein.
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */


#ifndef OW_MD5_HPP_INCLUDE_GUARD_
#define OW_MD5_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Exception.hpp"

#ifdef OW_HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

#ifdef OW_HAVE_OSTREAM
#include <ostream>
#elif defined(OW_HAVE_OSTREAM_H)
#include <ostream.h>
#else
#include <iostream>
#endif

DECLARE_EXCEPTION(MD5);

#define HASHLEN 16

class OW_String;
class OW_MD5;

//////////////////////////////////////////////////////////////////////////////
class OW_MD5StreamBuffer : public std::streambuf
{
public: 
	OW_MD5StreamBuffer(OW_MD5* md5);

protected:
	OW_MD5* _md5;

	virtual int overflow(int c = EOF);
	virtual std::streamsize xsputn(const char* s, std::streamsize num);
};

//////////////////////////////////////////////////////////////////////////////
class OW_MD5OStreamBase 
{
public:
	OW_MD5StreamBuffer _buf;
	OW_MD5OStreamBase(OW_MD5* md5);
};

//////////////////////////////////////////////////////////////////////////////
class OW_MD5 : private OW_MD5OStreamBase, public std::ostream
{
/* MD5 context. */
public:

	OW_MD5();

	/**
	 * Start a MD5 session,
	 * @param input The initial data to process.
	 */
	OW_MD5(const OW_String& input);

	void init(const OW_String& input);

	~OW_MD5() {};
	typedef struct
	{
		OW_UInt32 state[4];											  /* state (ABCD) */
		OW_UInt32 count[2];		  /* number of bits, modulo 2^64 (lsb first) */
		unsigned char buffer[64];								  /* input buffer */
	} MD5_CTX;

	/**
	 * Process more data
	 * @param input the additional data to process.
	 */
	void update(const OW_String& input);

	/**
	 * Finish the digest, and return the results.
	 * @return the results of the digest.
	 */
	OW_String toString();
	unsigned char* getDigest();

private:
	MD5_CTX m_ctx;
	unsigned char m_digest[16];
	bool m_finished;

	OW_String convertBinToHex( const unsigned char sBin[ 16 ]);
	static void MD5Init(MD5_CTX * md5ctx);
	static void MD5Update(MD5_CTX *md5ctx, const unsigned char* input,
		OW_UInt32 inputLen);
	static void MD5Final(unsigned char [16], MD5_CTX *);

	friend class OW_MD5StreamBuffer;
};


#endif

