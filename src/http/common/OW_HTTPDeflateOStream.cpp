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

#include "OW_HTTPDeflateOStream.hpp"
#include "OW_HTTPException.hpp"

using std::ostream;

OW_HTTPDeflateOStreamBuffer::OW_HTTPDeflateOStreamBuffer(ostream& ostr)
	: OW_BaseStreamBuffer(HTTP_BUF_SIZE, "out")
	, m_ostr(ostr)
{
	m_zstr.opaque = Z_NULL;
	m_zstr.zfree = Z_NULL;
	m_zstr.zalloc = Z_NULL;
	m_zstr.next_out = m_outBuf;
	m_zstr.avail_out = m_outBufSize;
	int rval = deflateInit(&m_zstr, Z_DEFAULT_COMPRESSION);
	if (rval != Z_OK)
	{
		OW_String msg = "Error: deflateInit returned " + OW_String(rval);
		if (m_zstr.msg)
		{
			msg += OW_String(": ") + OW_String(m_zstr.msg);
		}
		OW_THROW(OW_HTTPException, msg.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPDeflateOStreamBuffer::~OW_HTTPDeflateOStreamBuffer()
{
	sync();
	deflateEnd(&m_zstr);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_HTTPDeflateOStreamBuffer::sync()
{
	int rval = OW_BaseStreamBuffer::sync();
	m_zstr.avail_in = 0;
	flushOutBuf(Z_SYNC_FLUSH);
	m_ostr.flush();
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_HTTPDeflateOStreamBuffer::buffer_to_device(const char* c, int n)
{
	m_zstr.next_in = (Bytef*)c;
	m_zstr.avail_in = n;
	int rval = 0;
	while (m_zstr.avail_in > 0)
	{
		int rv = flushOutBuf();
		if (rv == -1)
		{
			rval = rv;
			break;
		}
		else
		{
			rval += rv;
		}
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_HTTPDeflateOStreamBuffer::flushOutBuf(int flush)
{
	int rval = 0;

	switch (flush)
	{
		case Z_SYNC_FLUSH:
		case Z_FINISH:
			int rv;
			do
			{
				rv = deflate(&m_zstr, flush);
				int bytesToWrite = writeToStream();
				rval += bytesToWrite;
			} while(rv == Z_OK);
			break;
		default:
			if (deflate(&m_zstr, flush) != Z_OK)
			{
				rval = -1;
			}
			else if (m_zstr.avail_out < m_outBufSize)
			{
				int bytesToWrite = writeToStream();
				if (!m_ostr)
				{
					rval = -1;
				}
				else
				{
					rval = bytesToWrite;
				}
			}
			break;
	}
	return rval;
}
	

//////////////////////////////////////////////////////////////////////////////
int
OW_HTTPDeflateOStreamBuffer::writeToStream()
{
	int bytesToWrite = m_outBufSize - m_zstr.avail_out;
	m_ostr.write(reinterpret_cast<char*>(m_outBuf), bytesToWrite);
	m_zstr.next_out = m_outBuf;
	m_zstr.avail_out = m_outBufSize;
	return bytesToWrite;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPDeflateOStreamBuffer::termOutput()
{
	sync();
	flushOutBuf(Z_FINISH);
	m_ostr.flush();
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPDeflateOStream::OW_HTTPDeflateOStream(ostream& ostr)
	: OW_HTTPDeflateOStreamBase(ostr)
	, ostream(&m_strbuf)
	, m_ostr(ostr)
{
}

//////////////////////////////////////////////////////////////////////////////

#endif // #ifdef OW_HAVE_ZLIB_H
