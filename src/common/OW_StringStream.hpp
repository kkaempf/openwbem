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

#ifndef OW_STRINGSTREAM_HPP_INCLUDE_GUARD_
#define OW_STRINGSTREAM_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_StringBuffer.hpp"
#include "OW_BaseStreamBuffer.hpp"

#if defined(OW_HAVE_OSTREAM)
#include <ostream>
#else
#include <iostream>
#endif

#ifdef OW_HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

class OW_StringStreamBuf : public OW_BaseStreamBuffer
{
public:
	OW_StringStreamBuf(size_t size)
		: OW_BaseStreamBuffer(size, "out"), m_buf(size) {}
	virtual ~OW_StringStreamBuf() {}
	OW_String toString() const { return m_buf.toString(); }
	size_t length() const { return m_buf.length(); }
	const char* c_str() const { return m_buf.c_str(); }
	void reset() { m_buf.reset(); }

protected:
	virtual int buffer_to_device(const char *c, int n)
	{
		m_buf.append(c, n);
		return 0;
	}

private:
	OW_StringBuffer m_buf;

	friend class OW_StringStream;
};

///////////////////////////////////////////////////////////////////////////////
class OW_StringStreamBase
{
public:
	OW_StringStreamBase(size_t sz) : m_buf(sz) {}

	mutable OW_StringStreamBuf m_buf;
};

///////////////////////////////////////////////////////////////////////////////
class OW_StringStream : private OW_StringStreamBase, public std::ostream
{
public:
	OW_StringStream(size_t size = 256)
		: OW_StringStreamBase(size), std::ostream(&m_buf)
	{}
	
	OW_String toString() const { m_buf.sync(); return m_buf.toString(); }
	size_t length() const { m_buf.sync(); return m_buf.length(); }
	const char* c_str() const { m_buf.sync(); return m_buf.c_str(); }
	void reset() { m_buf.reset(); }

private:

	// not implemented
	OW_StringStream(const OW_StringStream&);
	OW_StringStream& operator=(const OW_StringStream&);
};


#endif


