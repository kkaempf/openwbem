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

/**
 * OW_TempFileBuffer is the "IntelliBuffer".
 * This buffer is for an iostream similar to a stringstream.
 * The difference is that there is a 4k static buffer.  Once the
 * buffer is full, it switches to write to a temp file.
 * Designed to optimize speed in the case of small buffers, and
 * memory in the case of large buffers.
 *
 */
#ifndef OW_TEMPFILESTREAM_HPP_INCLUDE_GUARD_
#define OW_TEMPFILESTREAM_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_String.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_CommonFwd.hpp"

#if defined(OW_HAVE_STREAMBUF)
#include <streambuf>
#elif defined(OW_HAVE_STREAMBUF_H)
#include <streambuf.h>
#endif

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace OW_NAMESPACE
{

class OW_COMMON_API TempFileBuffer : public std::streambuf
{
public:
	TempFileBuffer(size_t bufSize);
	TempFileBuffer(String const& filename, size_t bufSize);
	~TempFileBuffer();
	std::streamsize getSize();
	void rewind();
	void reset();
	String releaseFile();
	bool usingTempFile() const;
protected:
	// for input
	int underflow();
	// for output
	std::streamsize xsputn(const char* s, std::streamsize n);
	virtual int overflow(int c);
	//virtual int sync();
	void initBuffers();
	void initGetBuffer();
	void initPutBuffer();
	int buffer_to_device(const char* c, int n);
	int buffer_from_device(char* c, int n);
private:
	size_t m_bufSize;
	char* m_buffer;
	TmpFile* m_tempFile;
	std::streamsize m_readPos;
	std::streamsize m_writePos;
	bool m_isEOF;
	int buffer_in();
	int buffer_out();
	// prohibit copying and assigning
	TempFileBuffer(const TempFileBuffer& arg);
	TempFileBuffer& operator=(const TempFileBuffer& arg);
};
class OW_COMMON_API TempFileStream : public std::iostream
{
public:
	TempFileStream(size_t bufSize = 4096);
	TempFileStream(String const& filename, size_t bufSize = 4096);
	std::streamsize getSize() { return m_buffer->getSize(); }
	void rewind();
	void reset();
	String releaseFile();
	bool usingTempFile() const;
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	AutoPtr<TempFileBuffer> m_buffer;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	// disallow copying and assigning
	TempFileStream(const TempFileStream&);
	TempFileStream& operator=(const TempFileStream&);
};

} // end namespace OW_NAMESPACE

#endif
