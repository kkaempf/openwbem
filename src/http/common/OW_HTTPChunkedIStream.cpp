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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_HTTPException.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMErrorException.hpp"

namespace OpenWBEM
{

using std::istream;
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedIStreamBuffer::HTTPChunkedIStreamBuffer(istream& istr,
	HTTPChunkedIStream* chunker)
	: BaseStreamBuffer(HTTP_BUF_SIZE, "in"), m_istr(istr),
	m_inLen(-1), m_inPos(0), m_isEOF(false), m_pChunker(chunker)
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
	if (m_isEOF)
	{
		return -1;
	}
	int tmpInLen = 0;
	int offset = 0;
	int lastRead = 0;
	while (offset < n && m_istr.good())
	{
		if (m_inLen == -1)
		{
			m_istr >> std::hex >> m_inLen >> std::dec;
			if (m_istr.fail() || m_istr.bad())
			{
				return -1;
			}
				//OW_THROW(HTTPChunkException, "Invalid length in chunk header");
					// skip past the trailing \r\n
			while (m_istr.get() != '\n' && m_istr.good())
			{
				// do nothing
			}

			m_inPos = 0;
			if (m_inLen == 0)
			{
				m_inLen = -1; // reset the state
				m_isEOF = true;
				m_pChunker->buildTrailerMap(); // build the trailer map
				return offset;
			}
		}
		// min of (n - offset) and (m_inLen - m_inPos)
		tmpInLen = ((n - offset) < (m_inLen - m_inPos)) ? (n - offset)
			: (m_inLen - m_inPos);
		m_istr.read(c + offset, tmpInLen);
		lastRead = m_istr.gcount();
		offset += lastRead;
		m_inPos += lastRead;
		if (m_inPos == m_inLen)
		{
			m_inLen = -1;
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
	m_inLen = -1;
	m_inPos = 0;
	m_isEOF = false;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HTTPChunkedIStream::HTTPChunkedIStream(istream& istr)
	: HTTPChunkedIStreamBase(istr, this)
	, CIMProtocolIStreamIFC(&m_strbuf)
	, m_istr(istr)
	, m_trailerMap()
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
//////////////////////////////////////////////////////////////////////////////
void
HTTPChunkedIStream::buildTrailerMap()
{
	if (!HTTPUtils::parseHeader(m_trailerMap, m_istr))
	{
		m_trailerMap.clear();
		OW_THROW(HTTPException, "Error parsing trailers");
	}
}
//////////////////////////////////////////////////////////////////////////////
String
HTTPChunkedIStream::getTrailer(const String& key) const
{
	for (Map<String, String>::const_iterator iter = m_trailerMap.begin();
		  iter != m_trailerMap.end(); ++iter)
	{
		if (iter->first.substring(3).equalsIgnoreCase(key))
		{
			return iter->second;
		}
	}
	return String();
}
//////////////////////////////////////////////////////////////////////////////
// TODO: Move all this knowledge about CIM and specific trailers into HTTPClient
void HTTPChunkedIStream::checkForError() const
{
	String errorStr;
	errorStr = getTrailer("CIMError");
	if (!errorStr.empty())
	{
		OW_THROW(CIMErrorException, errorStr.c_str());
	}
	errorStr = getTrailer("CIMStatusCode");
	if (errorStr.empty())
	{
		// try the old OW 2.0.x pre-standard way
		errorStr = getTrailer("CIMErrorCode");
	}
	if (!errorStr.empty())
	{
		String descr;
		descr = getTrailer("CIMStatusDescription");
		if (descr.empty())
		{
			// try the old OW 2.0.x pre-standard way
			descr = getTrailer("CIMErrorDescription");
		}
		if (!descr.empty())
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(errorStr.toInt32()),
				descr.c_str());
		}
		else
		{
			OW_THROWCIM(CIMException::ErrNoType(errorStr.toInt32()));
		}
	}
}
//////////////////////////////////////////////////////////////////////////////

} // end namespace OpenWBEM

