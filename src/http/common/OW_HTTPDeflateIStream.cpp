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


#include "OW_config.h"

#ifdef OW_HAVE_ZLIB_H

#include "OW_HTTPDeflateIStream.hpp"
#include "OW_HTTPException.hpp"

using std::istream;

OW_HTTPDeflateIStreamBuffer::OW_HTTPDeflateIStreamBuffer(istream& istr)
	: OW_BaseStreamBuffer(HTTP_BUF_SIZE, "in")
	, m_istr(istr)
{
	m_zstr.opaque = Z_NULL;
	m_zstr.zfree = Z_NULL;
	m_zstr.zalloc = Z_NULL;
	m_zstr.avail_in = 0;
	int rval = inflateInit(&m_zstr);
	if (rval != Z_OK)
	{
		OW_String msg = "Error: inflateInit returned " + OW_String(rval);
		if (m_zstr.msg)
		{
			msg += OW_String(": ") + OW_String(m_zstr.msg);
		}
		OW_THROW(OW_HTTPException, msg.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPDeflateIStreamBuffer::~OW_HTTPDeflateIStreamBuffer()
{
	inflateEnd(&m_zstr);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_HTTPDeflateIStreamBuffer::buffer_from_device(char* c, int n)
{
	if (n < 1)
	{
		return 0;
	}
	m_zstr.avail_out = n;
	m_zstr.next_out = reinterpret_cast<Bytef*>(c);
	int bytesRead = 0;
	while (m_zstr.avail_out > 0)
	{
		if (m_zstr.avail_in == 0)
		{
			if (m_istr)
			{
				m_istr.read(reinterpret_cast<char*>(m_inBuf), m_inBufSize);
				m_zstr.avail_in = m_istr.gcount();
				m_zstr.next_in = m_inBuf;
			}
		}
		int rv = inflate(&m_zstr, Z_SYNC_FLUSH);
		bytesRead = n - m_zstr.avail_out;
		if (rv != Z_OK)
		{
			break;
		}
	}
	if (bytesRead > 0)
	{
		return bytesRead;
	}
	else
	{
		return -1;
	}


	/*
	if (!m_istr)
	{
		return -1;
	}
	m_istr.read(m_inBuf, n);
	int bytesRead = m_istr.gcount();
	m_zstr.next_in = m_inBuf;
	m_zstr.avail_in = bytesRead;
	m_zstr.next_out = (Bytef*)c;	
	m_zstr.avail_out = n;
	int rv = inflate(&m_zstr, Z_SYNC_FLUSH);
	if (rv != Z_OK
		 && rv != Z_STREAM_END)
	{
		return -1;
	}

	return (n - m_zstr.avail_out);
	*/
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
OW_HTTPDeflateIStream::OW_HTTPDeflateIStream(
			OW_Reference<OW_CIMProtocolIStreamIFC> istr)
	: OW_HTTPDeflateIStreamBase(*istr)
	, OW_CIMProtocolIStreamIFC(&m_strbuf)
	, m_istr(istr)
{
}


#endif // #ifdef OW_HAVE_ZLIB_H
