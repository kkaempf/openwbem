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

/**
 *
 *
 *
 *
 */


#include "OW_config.h"
#include "OW_HTTPLenLimitIStream.hpp"

using std::istream;

OW_HTTPLengthLimitStreamBuffer::OW_HTTPLengthLimitStreamBuffer(
		istream& istr, OW_Int64 length)
	: OW_BaseStreamBuffer(HTTPLL_STREAM_BUF_SIZE, "in"), m_istr(istr),
	  m_length(length), m_pos(0), m_isEnd(false)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPLengthLimitStreamBuffer::~OW_HTTPLengthLimitStreamBuffer()
{
}


//////////////////////////////////////////////////////////////////////////////
int
OW_HTTPLengthLimitStreamBuffer::buffer_from_device(char* c, int n)
{
	if (m_isEnd)
	{
		return -1;
	}
	// min of n and (length - pos)
	int tmpInLen = (n < (m_length - m_pos)) ? n : (m_length - m_pos);
	m_istr.read(c, tmpInLen);
	int lastRead = m_istr.gcount();
	m_pos += lastRead;
	if (m_pos == m_length)
	{
		//c[lastRead] = EOF; TODO remove it you see this and make check works.  :)
		m_isEnd = true;
	}
	return lastRead;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPLengthLimitStreamBuffer::resetLen(OW_Int64 len)
{
	initGetBuffer();
	m_length = len;
  	m_pos = 0;
	m_isEnd = false;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPLenLimitIStream::resetLen(OW_Int64 len)
{
	clear();
	m_strbuf.resetLen(len);
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPLenLimitIStream::OW_HTTPLenLimitIStream(istream& istr, OW_Int64 len)
	: OW_HTTPLenLimitIStreamBase(istr, len)
	, OW_CIMProtocolIStreamIFC(&m_strbuf)
	, m_istr(istr)
{
}

//////////////////////////////////////////////////////////////////////////////



