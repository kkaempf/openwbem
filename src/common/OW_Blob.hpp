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
#ifndef __OW_BLOB_HPP__
#define __OW_BLOB_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Bool.hpp"
#include "OW_AutoPtr.hpp"
#include <cstdlib>

#include <iostream>
#if defined(OW_HAVE_STREAMBUF)
#include <streambuf>
#elif defined(OW_HAVE_STREAMBUF_H)
#include <streambuf.h>
#endif

struct OW_Blob
{
	size_t len;			// Length of bfr
	OW_UInt8 bfr[0];	// Data buffer
};

//////////////////////////////////////////////////////////////////////////////
class OW_BlobIStreamBuf : public std::streambuf
{
public:
	OW_BlobIStreamBuf(OW_Blob* blob)
		: std::streambuf()
	{
		setg((char*)(blob->bfr+blob->len), (char*)(blob->bfr),
			(char*)(blob->bfr+blob->len));
	}

protected:
	int underflow()
	{
		return (gptr() < egptr()) ? static_cast<int>(*gptr()) : EOF;
	}
};

//////////////////////////////////////////////////////////////////////////////
class OW_BlobIStream : public std::istream
{
public:
	OW_BlobIStream(OW_Blob* blob)
		: std::istream(new OW_BlobIStreamBuf(blob))
		, m_strbuf(dynamic_cast<OW_BlobIStreamBuf*>(rdbuf())) {}

private:
		OW_AutoPtr<OW_BlobIStreamBuf> m_strbuf;
};

//////////////////////////////////////////////////////////////////////////////
class OW_BlobOStreamBuf : public std::streambuf
{
public:
	OW_BlobOStreamBuf(int incSize=-1);
	~OW_BlobOStreamBuf();
	int length() { return m_blob->len; }
	void clear();
	OW_Blob* getBlob(OW_Bool forgetMemory=false);

protected:
	int overflow(int c=EOF);

private:

	enum { DEFAULT_INCSIZE = 128, MIN_INCSIZE = 32 };

	int m_incSize;
	OW_Blob* m_blob;
	int m_size;
	OW_Bool m_forgetMemory;
};

//////////////////////////////////////////////////////////////////////////////
class OW_BlobOStream : public std::ostream
{
public:
	OW_BlobOStream() 
		: std::ostream(new OW_BlobOStreamBuf)
		, m_buf(dynamic_cast<OW_BlobOStreamBuf*>(rdbuf())) 
	{}

	OW_Blob* getBlob(OW_Bool forgetMemory)
	{
		return m_buf->getBlob(forgetMemory);
	}

	int length() { return m_buf->length();  }
	void clearData() { m_buf->clear(); }

private:
	OW_AutoPtr<OW_BlobOStreamBuf> m_buf;
};

#endif	//  __OW_BLOB_HPP__

