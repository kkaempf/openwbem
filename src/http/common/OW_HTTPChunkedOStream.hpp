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

#ifndef _OW_HTTPCHUNKEDOSTREAM_HPP__
#define _OW_HTTPCHUNKEDOSTREAM_HPP__

#include "OW_config.h"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_Array.hpp"
#include "OW_AutoPtr.hpp"

#ifdef OW_HAVE_OSTREAM
#include <ostream>
#elif defined(OW_HAVE_OSTREAM_H)
#include <ostream.h>
#else
#include <iostream>
#endif

class OW_HTTPChunkedOStreamBuffer : public OW_BaseStreamBuffer
{
public: 
	OW_HTTPChunkedOStreamBuffer(std::ostream& ostr);

	virtual ~OW_HTTPChunkedOStreamBuffer();

protected:
	virtual int sync();

private:
	std::ostream& m_ostr;

	virtual int buffer_to_device(const char* c, int n);

	// disallow copying and assigning
	OW_HTTPChunkedOStreamBuffer(const OW_HTTPChunkedOStreamBuffer&);
	OW_HTTPChunkedOStreamBuffer& operator=(const OW_HTTPChunkedOStreamBuffer&);
};

//////////////////////////////////////////////////////////////////////////////
class OW_HTTPChunkedOStreamBase 
{
public:
	OW_HTTPChunkedOStreamBase(std::ostream& ostr)
		: m_strbuf(ostr) {}
	OW_HTTPChunkedOStreamBuffer m_strbuf;
};

//////////////////////////////////////////////////////////////////////////////
class OW_HTTPChunkedOStream : private OW_HTTPChunkedOStreamBase, 
	public std::ostream
{
public:
	/**
	 * Convert a ostream to a OW_HTTPChunkedOStream.  The ostream 
	 * that is passed in is wrapped.  All output sent to the new 
	 * stream is chunked, and then passed to the original stream.
	 * @param ostr the ostream& to wrap.
	 */
	OW_HTTPChunkedOStream(std::ostream& ostr);
	
	~OW_HTTPChunkedOStream();

	/**
	 * Call this when the entity has been completely sent.
	 * This flushes the remaining output, and sends a zero length
	 * chunk, signalling the end of the entity.
	 */
	void termOutput();

	/** Get the original ostream&
	 * @return the original ostream
	 */
	std::ostream& getOutputStreamOrig() { return m_ostr; };

	/**
	 * Add a HTTP trailer (header at the end of a chunked entity)
	 * @param key the name of the trailer (left of the ':')
	 * @param value the value of the trailer (right of the ':')
	 */
	void addTrailer(const OW_String& key, const OW_String& value);
private:
	std::ostream& m_ostr;

	OW_Array<OW_String> m_trailers;

	// disallow copying and assigning
	OW_HTTPChunkedOStream(const OW_HTTPChunkedOStream&);
	OW_HTTPChunkedOStream& operator=(const OW_HTTPChunkedOStream&);
};


#endif // _OW_HTTPCHUNKEDOSTREAM_HPP__
