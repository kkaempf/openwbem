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

#ifndef OW_HTTPCHUNKEDOSTREAM_HPP_INCLUDE_GUARD_
#define OW_HTTPCHUNKEDOSTREAM_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_Array.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_String.hpp"
#ifdef OW_HAVE_OSTREAM
#include <ostream>
#elif defined(OW_HAVE_OSTREAM_H)
#include <ostream.h>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

class OW_HTTP_API HTTPChunkedOStreamBuffer : public BaseStreamBuffer
{
public: 
	HTTPChunkedOStreamBuffer(std::ostream& ostr);
	virtual ~HTTPChunkedOStreamBuffer();

protected:
	virtual int sync();
private:
	std::ostream& m_ostr;
	virtual int buffer_to_device(const char* c, int n);
	// disallow copying and assigning
	HTTPChunkedOStreamBuffer(const HTTPChunkedOStreamBuffer&);
	HTTPChunkedOStreamBuffer& operator=(const HTTPChunkedOStreamBuffer&);

	friend class HTTPChunkedOStream; // so it can call initPutBuffer()
};
//////////////////////////////////////////////////////////////////////////////
class OW_HTTP_API HTTPChunkedOStreamBase 
{
public:
	HTTPChunkedOStreamBase(std::ostream& ostr)
		: m_strbuf(ostr) {}
	HTTPChunkedOStreamBuffer m_strbuf;
};
//////////////////////////////////////////////////////////////////////////////
class OW_HTTP_API HTTPChunkedOStream : private HTTPChunkedOStreamBase, 
	public std::ostream
{
public:
	/**
	 * Convert a ostream to a HTTPChunkedOStream.  The ostream 
	 * that is passed in is wrapped.  All output sent to the new 
	 * stream is chunked, and then passed to the original stream.
	 * @param ostr the ostream& to wrap.
	 */
	HTTPChunkedOStream(std::ostream& ostr);
	
	~HTTPChunkedOStream();

	enum ESendLastChunkFlag
	{
		E_DISCARD_LAST_CHUNK,
		E_SEND_LAST_CHUNK
	};
	/**
	 * Call this when the entity has been completely sent.
	 * This flushes the remaining output, and sends a zero length
	 * chunk, signalling the end of the entity.
	 */
	void termOutput(ESendLastChunkFlag sendLastChunk = E_SEND_LAST_CHUNK);
	/** Get the original ostream&
	 * @return the original ostream
	 */
	std::ostream& getOutputStreamOrig() { return m_ostr; };
	/**
	 * Add a HTTP trailer (header at the end of a chunked entity)
	 * @param key the name of the trailer (left of the ':')
	 * @param value the value of the trailer (right of the ':')
	 */
	void addTrailer(const String& key, const String& value);
private:
	std::ostream& m_ostr;
	Array<String> m_trailers;
	// disallow copying and assigning
	HTTPChunkedOStream(const HTTPChunkedOStream&);
	HTTPChunkedOStream& operator=(const HTTPChunkedOStream&);
};

} // end namespace OpenWBEM

#endif
