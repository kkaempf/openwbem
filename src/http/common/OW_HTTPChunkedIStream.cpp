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
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_HTTPException.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMErrorException.hpp"

namespace OW_NAMESPACE
{

using std::istream;
using namespace blocxx;
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedIStreamBuffer::HTTPChunkedIStreamBuffer(istream& istr)
	: BaseStreamBuffer(BaseStreamBuffer::E_IN, HTTP_BUF_SIZE), m_istr(istr),
	m_inLen(0), m_inPos(0), m_isEOF(false)
{
}
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedIStreamBuffer::~HTTPChunkedIStreamBuffer()
{
}
//////////////////////////////////////////////////////////////////////////////
int
HTTPChunkedIStreamBuffer::buffer_from_device(char* c, int n)
{
	if (m_isEOF || n < 0)
	{
		return -1;
	}
	unsigned int un = n;
	unsigned int tmpInLen = 0;
	unsigned int offset = 0;
	unsigned int lastRead = 0;
	while (offset < un && m_istr.good())
	{
		if (m_inLen == 0)
		{
			m_istr >> std::hex >> m_inLen >> std::dec;
			if (m_istr.fail() || m_istr.bad())
			{
				return -1;
			}

			// skip past the trailing \r\n
			while (m_istr.get() != '\n' && m_istr.good())
			{
				// do nothing
			}

			m_inPos = 0;
			if (m_inLen == 0)
			{
				// reset the state
				m_isEOF = true;
				return offset;
			}
		}
		// min of (n - offset) and (m_inLen - m_inPos)
		tmpInLen = ((un - offset) < (m_inLen - m_inPos)) ? (un - offset)
			: (m_inLen - m_inPos);
		m_istr.read(c + offset, tmpInLen);
		lastRead = m_istr.gcount();
		offset += lastRead;
		m_inPos += lastRead;
		if (m_inPos == m_inLen)
		{
			m_inLen = 0;
			m_inPos = 0;
			// don't need to skip trailing \r\n, because formatted input will
			// skip it.
		}
		if (lastRead < tmpInLen) // eof from other side
		{
			break;
		}
	}
	return offset; // should be equal to n if we reached this.
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPChunkedIStreamBuffer::resetInput()
{
	initGetBuffer();
	m_inLen = 0;
	m_inPos = 0;
	m_isEOF = false;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedIStream::HTTPChunkedIStream(istream& istr)
	: HTTPChunkedIStreamBase(istr)
	, std::istream(&m_strbuf)
	, m_istr(istr)
{
}
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedIStream::~HTTPChunkedIStream()
{
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPChunkedIStream::resetInput()
{
	clear();
	m_strbuf.resetInput();
}

} // end namespace OW_NAMESPACE

