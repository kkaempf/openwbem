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

#ifndef OW_REPOSITORYSTREAMS_HPP_INCLUDE_GUARD_
#define OW_REPOSITORYSTREAMS_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_AutoPtr.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

#if defined(OW_HAVE_STREAMBUF)
#include <streambuf>
#elif defined(OW_HAVE_STREAMBUF_H)
#include <streambuf.h>
#endif

//////////////////////////////////////////////////////////////////////////////
class OW_RepositoryIStreamBuf : public std::streambuf
{
public:
	OW_RepositoryIStreamBuf(int dataLen, const unsigned char* data) :
		std::streambuf()
	{
		setg(const_cast<char*>(reinterpret_cast<const char*>(data+dataLen)), 
			const_cast<char*>(reinterpret_cast<const char*>(data)), 
			const_cast<char*>(reinterpret_cast<const char*>(data+dataLen)));
	}

protected:
	int underflow()
	{
		return (gptr() < egptr()) ? static_cast<int>(*gptr()) : EOF;
	}
};

//////////////////////////////////////////////////////////////////////////////
class OW_RepositoryIStream : public std::istream
{
public:
	OW_RepositoryIStream(int dataLen, const unsigned char* data) :
		std::istream(new OW_RepositoryIStreamBuf(dataLen, data))
		, m_strbuf(dynamic_cast<OW_RepositoryIStreamBuf*>(rdbuf())) {}

private:
		OW_AutoPtr<OW_RepositoryIStreamBuf> m_strbuf;
};


//////////////////////////////////////////////////////////////////////////////
class OW_RepositoryOStreamBuf : public std::streambuf
{
public:
	OW_RepositoryOStreamBuf(int incSize=-1);
	~OW_RepositoryOStreamBuf();
	unsigned char* getData() { return m_bfr; }
	const int length() { return m_count; }
	void clear();

protected:
	int overflow(int c=EOF);

private:

	enum { DEFAULT_INCSIZE = 64, MIN_INCSIZE = 16 };

	int m_incSize;
	unsigned char* m_bfr;
	int m_size;
	int m_count;
};

//////////////////////////////////////////////////////////////////////////////
class OW_RepositoryOStream : public std::ostream
{
public:
	OW_RepositoryOStream()
		: std::ostream(new OW_RepositoryOStreamBuf)
		, m_buf(dynamic_cast<OW_RepositoryOStreamBuf*>(rdbuf()))
	{}
	unsigned char* getData() { return m_buf->getData(); }
	int length() { return m_buf->length();  }
	void clearData() { m_buf->clear(); }

private:
	OW_AutoPtr<OW_RepositoryOStreamBuf> m_buf;
};

#endif



