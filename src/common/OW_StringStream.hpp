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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_STRINGSTREAM_HPP_INCLUDE_GUARD_
#define OW_STRINGSTREAM_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_StringBuffer.hpp"
#include "OW_BaseStreamBuffer.hpp"
#if defined(OW_HAVE_OSTREAM) && defined(OW_HAVE_ISTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif
#ifdef OW_HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

namespace OpenWBEM
{

///////////////////////////////////////////////////////////////////////////////
class IStringStreamBuf : public std::streambuf
{
public:
	IStringStreamBuf(const String& s)
		: std::streambuf()
		, m_buf(s)
	{
		setg(const_cast<char*>(reinterpret_cast<const char*>(m_buf.c_str())),
			const_cast<char*>(reinterpret_cast<const char*>(m_buf.c_str())),
			const_cast<char*>(reinterpret_cast<const char*>(m_buf.c_str()+m_buf.length())));
	}
protected:
	int underflow()
	{
		return (gptr() < egptr()) ? static_cast<unsigned char>(*gptr()) : EOF; 	// need a static_cast so a -1 doesn't turn into an EOF
	}
private:
	String m_buf;
};
///////////////////////////////////////////////////////////////////////////////
class IStringStreamBase
{
public:
	IStringStreamBase(const String& s) : m_buf(s) {}
	mutable IStringStreamBuf m_buf;
};

///////////////////////////////////////////////////////////////////////////////
class IStringStream : private IStringStreamBase, public std::istream
{
public:
	IStringStream(const String& s);
	~IStringStream();
	void reset();
private:
	// not implemented
	IStringStream(const IStringStream&);
	IStringStream& operator=(const IStringStream&);
};

///////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API OStringStreamBuf : public BaseStreamBuffer
{
public:
	OStringStreamBuf(size_t size);
	virtual ~OStringStreamBuf();
	String toString() const;
	// After calling releaseString(), this OStringStream is unusable
	String releaseString();
	size_t length() const;
	const char* c_str() const;
	void reset();
protected:
	virtual int buffer_to_device(const char *c, int n);
private:
	StringBuffer m_buf;
	friend class OStringStream;
};
///////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API OStringStreamBase
{
public:
	OStringStreamBase(size_t sz);
	mutable OStringStreamBuf m_buf;
};
///////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API OStringStream : private OStringStreamBase, public std::ostream
{
public:
	OStringStream(size_t size = 256);
	~OStringStream();
	OStringStream(const OStringStream&);
	OStringStream& operator=(const OStringStream&);  
	String toString() const;
	// After calling releaseString(), this OStringStream is unusable
	String releaseString();
	size_t length() const;
	const char* c_str() const;
	void reset();
};

} // end namespace OpenWBEM

#endif
