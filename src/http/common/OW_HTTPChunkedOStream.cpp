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
 */

#include "OW_config.h"
#include "OW_HTTPChunkedOStream.hpp"

using std::ostream;

//////////////////////////////////////////////////////////////////////////////
OW_HTTPChunkedOStreamBuffer::OW_HTTPChunkedOStreamBuffer(ostream& ostr)
	: OW_BaseStreamBuffer(HTTP_BUF_SIZE, "out"), m_ostr(ostr)
{
}

//////////////////////////////////////////////////////////////////////////////

OW_HTTPChunkedOStreamBuffer::~OW_HTTPChunkedOStreamBuffer()
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
OW_HTTPChunkedOStreamBuffer::sync()
{
	int rval = OW_BaseStreamBuffer::sync();
	m_ostr.flush();
	return rval;
}


//////////////////////////////////////////////////////////////////////////////
int
OW_HTTPChunkedOStreamBuffer::buffer_to_device(const char* c, int n)
{
	if (n <= 0) // we don't ever want to write a 0 length here.
		return 0;
	m_ostr << std::hex << n << std::dec << "\r\n";
	m_ostr.write(c, n);
	m_ostr.write("\r\n", 2);
	if (m_ostr.good())
		return 0;
	else
		return -1;
}


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
OW_HTTPChunkedOStream::OW_HTTPChunkedOStream(ostream& ostr) :
	OW_HTTPChunkedOStreamBase(ostr),
	ostream(&m_strbuf),
	m_ostr(ostr)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPChunkedOStream::~OW_HTTPChunkedOStream()
{
}


//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPChunkedOStream::termOutput()
{
	flush();
	m_ostr << "0\r\n";
	for (size_t i = 0; i < m_trailers.size(); i++)
	{
		m_ostr << m_trailers[i] << "\r\n";
	}
	//m_trailers.clear();  // TODO should this be uncommented?
	m_ostr << "\r\n";
	m_ostr.flush();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPChunkedOStream::addTrailer(const OW_String& key, const OW_String& value)
{
	OW_String tmpKey = key;
	tmpKey.trim();
	if (!tmpKey.empty())
	{
		m_trailers.push_back(key + ": " + value);
	}
	else // A "folded" continuation line from previous key
	{
		m_trailers.push_back(" " + value);
	}
}

