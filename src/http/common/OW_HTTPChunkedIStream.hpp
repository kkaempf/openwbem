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

#ifndef _OW_HTTPChunkedIStream_HPP__
#define _OW_HTTPChunkedIStream_HPP__

#include "OW_config.h"
#include "OW_HTTPChunkException.hpp"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_String.hpp"
#include "OW_Map.hpp"
#include "OW_AutoPtr.hpp"
#include <iostream>


class OW_HTTPChunkedIStream;

class OW_HTTPChunkedIStreamBuffer : public OW_BaseStreamBuffer
{
	public:
		OW_HTTPChunkedIStreamBuffer(std::istream& istr,
				OW_HTTPChunkedIStream* chunkedIstr);

		void resetInput();
		~OW_HTTPChunkedIStreamBuffer();

	private:
		std::istream& m_istr;
		int m_inLen;
		int m_inPos;

		OW_Bool m_isEOF;
		virtual int buffer_from_device(char* c, int n);
			/*throw (OW_HTTPChunkException)*/

		OW_HTTPChunkedIStream* m_pChunker;

		// don't allow copying.
		OW_HTTPChunkedIStreamBuffer(const OW_HTTPChunkedIStreamBuffer&);
		OW_HTTPChunkedIStreamBuffer operator=(const OW_HTTPChunkedIStreamBuffer&);
};

class OW_HTTPChunkedIStreamBase 
{
public:
	OW_HTTPChunkedIStreamBase(std::istream& istr, 
		OW_HTTPChunkedIStream* chunkedIStr) : m_strbuf(istr, chunkedIStr) {}
	OW_HTTPChunkedIStreamBuffer m_strbuf;
};

class OW_HTTPChunkedIStream : private OW_HTTPChunkedIStreamBase, 
	public std::istream
{
	public:
		/**
		 * Convert a istream& into a OW_HTTPChunkedIStream.  The istream&
		 * passed in is wrapped to that the new input available from this
		 * is de-chunked.
		 * @param istr The istream& to be wrapped (de-chunked).
		 */
		OW_HTTPChunkedIStream(std::istream& istr);
		~OW_HTTPChunkedIStream();

		/**
		 * Get the original istream&.
		 * @return a istream& pointing to the original istream.
		 */
		std::istream& getInputStreamOrig() { return m_istr; }

		/**
		 * Get the HTTP trailers from the http response.
		 * This must be called after EOF is hit on the input stream.
		 * @return the trailers.
		 */
		OW_Map<OW_String, OW_String> getTrailers() const { return m_trailerMap; }

		/**
		 * Clear the EOF/BAD bits, so that input can continue.
		 */
		void resetInput();

	private:

		std::istream& m_istr;
		// incoming trailer info
		OW_Map<OW_String, OW_String> m_trailerMap;

		/**
		 * A callback function, invoked by the OW_HTTPChunkingStreamBuffer.
		 * After a zero length chunk is encountered, this is called to build
		 * the trailer map, if there is one
		 */
		void buildTrailerMap();

		// don't allow copying
		OW_HTTPChunkedIStream(const OW_HTTPChunkedIStream&);
		OW_HTTPChunkedIStream& operator=(const OW_HTTPChunkedIStream&);

		friend class OW_HTTPChunkedIStreamBuffer;
};


#endif // _OW_HTTPChunkedIStream_HPP__

