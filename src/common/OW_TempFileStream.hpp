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
#include "OW_Types.h"
#include "OW_String.hpp"
#include "OW_AutoPtr.hpp"

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


class OW_TmpFile;

class OW_TempFileBuffer : public std::streambuf
{
public:
	OW_TempFileBuffer(size_t bufSize);
	OW_TempFileBuffer(OW_String const& filename, size_t bufSize);
	~OW_TempFileBuffer();

	std::streamsize getSize();
	void rewind();
	void reset();
	OW_String releaseFile();
	OW_Bool usingTempFile() const;

protected:
	// for input
	int underflow();

	// for output
	std::streamsize xsputn(const char* s, std::streamsize n);
	int overflow(int c = EOF);
	//virtual int sync();

	void initBuffers();
	void initGetBuffer();
	void initPutBuffer();

	int buffer_to_device(const char* c, int n);
	int buffer_from_device(char* c, int n);

private:

	size_t m_bufSize;
	char* m_buffer;
	OW_TmpFile* m_tempFile;

	std::streamsize m_readPos;
	std::streamsize m_writePos;
	bool m_isEOF;

	int buffer_in();
	int buffer_out();

	// prohibit copying and assigning
	OW_TempFileBuffer(const OW_TempFileBuffer& arg);
	OW_TempFileBuffer& operator=(const OW_TempFileBuffer& arg);

};

class OW_TempFileStream : public std::iostream
{
public:
	OW_TempFileStream(size_t bufSize = 4096);
	OW_TempFileStream(OW_String const& filename, size_t bufSize = 4096);
	std::streamsize getSize() { return m_buffer->getSize(); }
	void rewind();
	void reset();
	OW_String releaseFile();
	OW_Bool usingTempFile() const;

private:
	OW_AutoPtr<OW_TempFileBuffer> m_buffer;

	// disallow copying and assigning
	OW_TempFileStream(const OW_TempFileStream&);
	OW_TempFileStream& operator=(const OW_TempFileStream&);

};

#endif

