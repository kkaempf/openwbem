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

// gptr() && !pptr() == input mode
// !gptr() && pptr() == output mode
// pptr() && gptr() should never happen
// !gptr() && !pptr() should never happen
// pptr() should never be 0 unless we're gone to file.

#include "OW_config.h"
#include "OW_TempFileStream.hpp"
#include "OW_TmpFile.hpp"
#include "OW_Assertion.hpp"
#include <cstring>

using std::iostream;

//////////////////////////////////////////////////////////////////////////////
OW_TempFileBuffer::OW_TempFileBuffer(size_t bufSize)
	: m_bufSize(bufSize)
	, m_buffer(new char[m_bufSize])
	, m_tempFile(NULL)
	, m_readPos(0)
	, m_writePos(0)
	, m_isEOF(false)
{
	setg(0,0,0); // start out in output mode.
	initPutBuffer();
}

//////////////////////////////////////////////////////////////////////////////
OW_TempFileBuffer::OW_TempFileBuffer(OW_String const& filename, size_t bufSize)
	: m_bufSize(bufSize)
	, m_buffer(new char[m_bufSize])
	, m_tempFile(new OW_TmpFile(filename))
	, m_readPos(0)
	, m_writePos(0)
	, m_isEOF(false)
{
	m_tempFile->seek(0, SEEK_END);
	m_writePos = m_tempFile->tell();
	m_tempFile->rewind();

	setp(0,0); // start out in input mode.
	initGetBuffer();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TempFileBuffer::initBuffers()
{
	initPutBuffer();
	initGetBuffer();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TempFileBuffer::initPutBuffer()
{
	setp(m_buffer, m_buffer + m_bufSize);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TempFileBuffer::initGetBuffer()
{
	setg(m_buffer, m_buffer, m_buffer);
}

//////////////////////////////////////////////////////////////////////////////
OW_TempFileBuffer::~OW_TempFileBuffer()
{
	delete [] m_buffer;
	delete m_tempFile;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_TempFileBuffer::buffer_out()
{
	int cnt = pptr() - pbase();
	int retval = buffer_to_device(m_buffer, cnt);

	initPutBuffer();
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_TempFileBuffer::overflow(int c)
{
	if (pptr()) // buffer is full
	{
		if (buffer_out() < 0)
		{
			return EOF;
		}
	}
	else  // switching from input to output
	{
		if (!m_tempFile)
		{
			initPutBuffer();
			pbump(m_writePos);
			m_readPos = gptr() - eback();
		}
		else
		{
			m_readPos = m_tempFile->tell() - (egptr() - gptr());
			m_tempFile->seek(m_writePos);
			initPutBuffer();
		}
		setg(0,0,0);
	}
	if (c != EOF)
	{
		return sputc(c);
	}
	else
	{
		return c;
	}
}

//////////////////////////////////////////////////////////////////////////////
std::streamsize
OW_TempFileBuffer::xsputn(const char* s, std::streamsize n)
{
	if (n < epptr() - pptr())
	{
		memcpy(pptr(), s, n * sizeof(char));
		pbump(n);
		return n;
	}
	else
	{
		for (std::streamsize i = 0; i < n; i++)
		{
			if (sputc(s[i]) == EOF)
				return i;
		}
		return n;
	}
}


//////////////////////////////////////////////////////////////////////////////
int
OW_TempFileBuffer::underflow()
{
	if (m_isEOF)
	{
		return EOF;
	}
	if (gptr()) // need to fill buffer
	{
		if (buffer_in() < 0)
		{
			return EOF;
		}
	}
	else // we're in output mode; switch to input mode
	{
		if (m_tempFile)
		{
			buffer_out();
			m_writePos = m_tempFile->tell();
			m_tempFile->seek(m_readPos);
			if (buffer_in() < 0)
			{
				return EOF;
			}
		}
		else
		{
			m_writePos = pptr() - pbase();
			setg(m_buffer, m_buffer + m_readPos, pptr());
		}
		setp(0,0);
	}
	return static_cast<unsigned char>(*gptr());
}

//////////////////////////////////////////////////////////////////////////////
int
OW_TempFileBuffer::buffer_in()
{
	int retval = buffer_from_device(m_buffer, m_bufSize);

	if (retval <= 0)
	{
		setg(0,0,0);
		m_isEOF = true;
		return -1;
	}
	else
	{
		setg(m_buffer, m_buffer, m_buffer + retval);
		return retval;
	}
}

//////////////////////////////////////////////////////////////////////////////
int
OW_TempFileBuffer::buffer_to_device(const char* c, int n)
{
	if (!m_tempFile)
	{
		m_tempFile = new OW_TmpFile;
	}
	return m_tempFile->write(c, n);
}


//////////////////////////////////////////////////////////////////////////////
int
OW_TempFileBuffer::buffer_from_device(char* c, int n)
{
	if (!m_tempFile)
	{
		return -1;
	}
	else
	{
		return m_tempFile->read(c, n);
	}
}


//////////////////////////////////////////////////////////////////////////////
std::streamsize
OW_TempFileBuffer::getSize()
{
	if (gptr() && !m_tempFile)
	{
		return egptr() - eback();
	}
	std::streamsize rval = m_writePos;
	if (m_tempFile)
	{
		rval = m_tempFile->getSize();
	}
	if (pptr())
	{
		rval += pptr() - pbase();
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TempFileBuffer::rewind()
{
	m_readPos = 0;
	if (m_tempFile)
	{
		if (pptr())
		{
			m_writePos += pptr() - pbase();
			buffer_out();
		}
		m_tempFile->seek(0);
		initGetBuffer();
	}
	else
	{
		if (pptr())
		{
			m_writePos = pptr() - pbase();
		}
		else if (gptr())
		{
			m_writePos = egptr() - eback();
		}
		setg(m_buffer, m_buffer, m_buffer + m_writePos);
	}
	setp(0,0);
	m_isEOF = false;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TempFileBuffer::reset()
{
	delete m_tempFile;
	m_tempFile = NULL;
	m_writePos = m_readPos = 0;
	setg(0,0,0);
	initPutBuffer();
	m_isEOF = false;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_TempFileBuffer::releaseFile()
{
	buffer_out(); 	// Flush the buffer and cause the temp file to be written
					// if it's not already being used.
	OW_String rval = m_tempFile->releaseFile();
	reset();
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_TempFileBuffer::usingTempFile() const
{
	return m_tempFile != 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_TempFileStream::OW_TempFileStream(size_t bufSize)
: iostream(new OW_TempFileBuffer(bufSize))
, m_buffer(dynamic_cast<OW_TempFileBuffer*>(rdbuf()))
{
}

//////////////////////////////////////////////////////////////////////////////
OW_TempFileStream::OW_TempFileStream(OW_String const& filename, size_t bufSize)
: iostream(new OW_TempFileBuffer(filename, bufSize))
, m_buffer(dynamic_cast<OW_TempFileBuffer*>(rdbuf()))
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TempFileStream::rewind()
{
	m_buffer->rewind();
	clear();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TempFileStream::reset()
{
	m_buffer->reset();
	clear();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_TempFileStream::releaseFile()
{
	OW_String rval = m_buffer->releaseFile();
	clear();
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_TempFileStream::usingTempFile() const
{
	return m_buffer->usingTempFile();
}

