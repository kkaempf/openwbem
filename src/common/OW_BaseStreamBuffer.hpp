/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_BASESTREAMBUFFER_HPP_INCLUDE_GUARD_
#define OW_BASESTREAMBUFFER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#ifdef OW_HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

namespace OpenWBEM
{

const size_t BASE_BUF_SIZE = 256;
const size_t HTTP_BUF_SIZE = 4096;
class BaseStreamBuffer : public std::streambuf
{
public:
	/**
	 * Create a base stream buffer.
	 * @param bufSize size of buffer
	 * @param direction "in", "out", or "io"
	 */
	BaseStreamBuffer(size_t bufSize = BASE_BUF_SIZE,
			const char* direction = "io");
	~BaseStreamBuffer();
protected:
	// for input
	int underflow();
	//int pbackfail(int c);
	// for output
	std::streamsize xsputn(const char* s, std::streamsize n);
	int overflow(int c = EOF);
	virtual int sync();
	virtual void initBuffers();
	virtual void initGetBuffer();
	virtual void initPutBuffer();
	// meant to be overwritten (at least one of them)
	/**
	 * Writes the buffer to the "device"
	 * @param c A pointer to the start of the buffer
	 * @param n the number of bytes to write
	 *
	 * @return -1 if error, 0 if the entire buffer was written.
	 */
	virtual int buffer_to_device(const char* c, int n);
	
	/**
	 * Fill the buffer from the "device"
	 * @param c A pointer to the beginning of the buffer
	 * @param n The number of bytes to be read into the buffer.
	 *
	 * @return -1 if no bytes are able to be read from the "device"
	 *             (for instance, end of input stream).  Otherwise,
	 *             return the number of bytes read into the buffer.
	 */
	virtual int buffer_from_device(char* c, int n);
private:
	std::streamsize m_bufSize;
	char* m_inputBuffer;
	char* m_outputBuffer;
	int buffer_in();
	int buffer_out();
	// prohibit copying and assigning
	// NO IMPLEMENTATION
	BaseStreamBuffer(const BaseStreamBuffer& arg);
	BaseStreamBuffer& operator=(const BaseStreamBuffer& arg);
};

} // end namespace OpenWBEM

#endif
