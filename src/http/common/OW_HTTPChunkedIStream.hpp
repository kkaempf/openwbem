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

#ifndef OW_HTTPChunkedIStream_HPP_INCLUDE_GUARD_
#define OW_HTTPChunkedIStream_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_HTTPChunkException.hpp"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_String.hpp"
#include "OW_Map.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_CIMProtocolIStreamIFC.hpp"
#include "OW_HttpCommonFwd.hpp"

namespace OW_NAMESPACE
{

class OW_HTTP_API HTTPChunkedIStreamBuffer : public BaseStreamBuffer
{
	public:
		HTTPChunkedIStreamBuffer(std::istream& istr,
				HTTPChunkedIStream* chunkedIstr);
		void resetInput();
		~HTTPChunkedIStreamBuffer();
	private:
		std::istream& m_istr;
		int m_inLen;
		int m_inPos;
		bool m_isEOF;
		virtual int buffer_from_device(char* c, int n);
		HTTPChunkedIStream* m_pChunker;
		// don't allow copying.
		HTTPChunkedIStreamBuffer(const HTTPChunkedIStreamBuffer&);
		HTTPChunkedIStreamBuffer& operator=(const HTTPChunkedIStreamBuffer&);
};
class OW_HTTP_API HTTPChunkedIStreamBase
{
public:
	HTTPChunkedIStreamBase(std::istream& istr,
		HTTPChunkedIStream* chunkedIStr) : m_strbuf(istr, chunkedIStr) {}
	HTTPChunkedIStreamBuffer m_strbuf;
};
class OW_HTTP_API HTTPChunkedIStream : private HTTPChunkedIStreamBase,
	public CIMProtocolIStreamIFC
{
	public:
		/**
		 * Convert a istream& into a HTTPChunkedIStream.  The istream&
		 * passed in is wrapped to that the new input available from this
		 * is de-chunked.
		 * @param istr The istream& to be wrapped (de-chunked).
		 */
		HTTPChunkedIStream(std::istream& istr);
		~HTTPChunkedIStream();
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
		Map<String, String> getTrailers() const { return m_trailerMap; }
		/**
		 * Clear the EOF/BAD bits, so that input can continue.
		 */
		void resetInput();
		// TODO: Move all this knowledge about CIM and specific trailers into HTTPClient
		virtual void checkForError() const;
		virtual String getTrailer(const String& key) const;
	private:
		std::istream& m_istr;
		// incoming trailer info

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

		Map<String, String> m_trailerMap;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

		/**
		 * A callback function, invoked by the HTTPChunkingStreamBuffer.
		 * After a zero length chunk is encountered, this is called to build
		 * the trailer map, if there is one
		 */
		void buildTrailerMap();
		// don't allow copying
		HTTPChunkedIStream(const HTTPChunkedIStream&);
		HTTPChunkedIStream& operator=(const HTTPChunkedIStream&);
		friend class HTTPChunkedIStreamBuffer;
};

} // end namespace OW_NAMESPACE

#endif
