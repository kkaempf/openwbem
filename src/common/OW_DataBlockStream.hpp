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

#ifndef OW_DATABLOCKSTREAM_HPP_
#define OW_DATABLOCKSTREAM_HPP_

#include "OW_config.h"
#include "OW_BaseStreamBuffer.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
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

#include <vector>

class OW_DataBlockStreamBuf : public OW_BaseStreamBuffer
{
public:
	OW_DataBlockStreamBuf(size_t size)
		: OW_BaseStreamBuffer(size, "out"), m_buf() {m_buf.reserve(size);}
	virtual ~OW_DataBlockStreamBuf() {}
	size_t size() const { return m_buf.size(); }
	void reset() { m_buf.clear(); }
	const char* data() const
	{
		return &m_buf[0];
	}

protected:
	virtual int buffer_to_device(const char *c, int n)
	{
		m_buf.insert(m_buf.end(), c, c + n);
		return n;
	}

private:
	std::vector<char> m_buf;

	friend class OW_DataBlockStream;
};

///////////////////////////////////////////////////////////////////////////////
class OW_DataBlockStreamBase
{
public:
	OW_DataBlockStreamBase(size_t sz) : m_buf(sz) {}

	mutable OW_DataBlockStreamBuf m_buf;
};

///////////////////////////////////////////////////////////////////////////////
class OW_DataBlockStream : private OW_DataBlockStreamBase, public std::ostream
{
public:
	OW_DataBlockStream(size_t size = 256)
		: OW_DataBlockStreamBase(size), std::ostream(&m_buf)
	{}
	
	size_t size() const { m_buf.sync(); return m_buf.size(); }
	const char* data() const { m_buf.sync(); return m_buf.data(); }
	void reset() { m_buf.reset(); }

private:

	// not implemented
	OW_DataBlockStream(const OW_DataBlockStream&);
	OW_DataBlockStream& operator=(const OW_DataBlockStream&);
};


#endif

