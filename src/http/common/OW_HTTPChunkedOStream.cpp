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

#include "OW_config.h"
#include "OW_HTTPChunkedOStream.hpp"

namespace OW_NAMESPACE
{

using std::ostream;
using namespace blocxx;
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedOStreamBuffer::HTTPChunkedOStreamBuffer(ostream& ostr)
	: BaseStreamBuffer(BaseStreamBuffer::E_OUT, HTTP_BUF_SIZE), m_ostr(ostr)
{
}
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedOStreamBuffer::~HTTPChunkedOStreamBuffer()
{
	try
	{
		sync();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
int
HTTPChunkedOStreamBuffer::sync()
{
	int rval = BaseStreamBuffer::sync();
	if (!m_ostr.flush())
	{
		// ERROR
		return -1;
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
int
HTTPChunkedOStreamBuffer::buffer_to_device(const char* c, int n)
{
	if (n <= 0) // we don't ever want to write a 0 length here.
	{
		return 0;
	}
	m_ostr << std::hex << n << std::dec << "\r\n";
	m_ostr.write(c, n);
	m_ostr.write("\r\n", 2);
	if (m_ostr.good())
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedOStream::HTTPChunkedOStream(ostream& ostr) :
	HTTPChunkedOStreamBase(ostr),
	std::basic_ostream<char, std::char_traits<char> >(&m_strbuf),
	m_ostr(ostr)
{
}
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedOStream::~HTTPChunkedOStream()
{
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPChunkedOStream::termOutput(ESendLastChunkFlag sendLastChunk)
{
	if (sendLastChunk == E_DISCARD_LAST_CHUNK)
	{
		// this will just erase everything in the stream buffer
		m_strbuf.initPutBuffer();
	}
	else
	{
		// Don't call the derived one because it flushes to the stream, which isn't necessary at this point.
		// This call is necessary to cause buffer_to_device() to write the chunk to m_ostr.
		m_strbuf.BaseStreamBuffer::sync();
	}

	m_ostr << "0\r\n";
	if (!m_ostr.flush())
	{
		clear(m_ostr.rdstate());
	}

	// once the output is terminated, we have to reset the stream
	m_strbuf.initPutBuffer();
}

} // end namespace OW_NAMESPACE

