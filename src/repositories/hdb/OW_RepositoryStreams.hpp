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
#ifndef OW_REPOSITORYSTREAMS_HPP_INCLUDE_GUARD_
#define OW_REPOSITORYSTREAMS_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
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
#include <vector>

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
class RepositoryIStreamBuf : public std::streambuf
{
public:
	RepositoryIStreamBuf(int dataLen, const unsigned char* data) :
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
class RepositoryIStreamBase
{
protected:
	RepositoryIStreamBase(int dataLen, const unsigned char* data) : m_strbuf(dataLen, data) {}
	RepositoryIStreamBuf m_strbuf;
};
//////////////////////////////////////////////////////////////////////////////
class RepositoryIStream : private RepositoryIStreamBase, public std::istream
{
public:
	RepositoryIStream(int dataLen, const unsigned char* data)
	: RepositoryIStreamBase(dataLen, data), std::istream(&m_strbuf)	{}
};
//////////////////////////////////////////////////////////////////////////////
class RepositoryOStreamBuf : public std::streambuf
{
public:
	RepositoryOStreamBuf(size_t initialSize = 256);
	const unsigned char* getData() const { return &m_bfr[0]; }
	int length() const { return m_bfr.size(); }
	void clear() { m_bfr.clear(); }
protected:
	virtual int overflow(int c=EOF);
	virtual std::streamsize xsputn(const char* s, std::streamsize n);
private:
	std::vector<unsigned char> m_bfr;
};
//////////////////////////////////////////////////////////////////////////////
class RepositoryOStreamBase
{
protected:
	RepositoryOStreamBase(size_t initialSize = 256)
	: m_buf(initialSize) {}

	RepositoryOStreamBuf m_buf;
};
//////////////////////////////////////////////////////////////////////////////
class RepositoryOStream : private RepositoryOStreamBase, public std::ostream
{
public:
	RepositoryOStream(size_t initialSize = 256)
		: RepositoryOStreamBase(initialSize)
		, std::ostream(&m_buf)
	{}
	const unsigned char* getData() const { return m_buf.getData(); }
	int length() const { return m_buf.length();  }
	void clearData() { m_buf.clear(); }
};

} // end namespace OpenWBEM

#endif
