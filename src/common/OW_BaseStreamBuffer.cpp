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

#include "OW_config.h"
#include "OW_BaseStreamBuffer.hpp"
#include <iostream>
#include <string.h>

OW_BaseStreamBuffer::OW_BaseStreamBuffer(int bufSize, 
		const OW_String& direction)
	: m_bufSize(bufSize), m_inputBuffer(NULL), m_outputBuffer(NULL)
{
	if (direction.equals("in") || direction.equals("io"))
	{
		m_inputBuffer = new char[m_bufSize];
		initGetBuffer();
	}
	if (direction.equals("out") || direction.equals("io"))
	{
		m_outputBuffer = new char[m_bufSize];
		initPutBuffer();
	}
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_BaseStreamBuffer::initBuffers()
{
	initPutBuffer();
	initGetBuffer();
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_BaseStreamBuffer::initPutBuffer()
{
	setp(m_outputBuffer, m_outputBuffer + m_bufSize);
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_BaseStreamBuffer::initGetBuffer()
{
	setg(m_inputBuffer, m_inputBuffer, m_inputBuffer);
}

//////////////////////////////////////////////////////////////////////////////
OW_BaseStreamBuffer::~OW_BaseStreamBuffer()
{
	delete [] m_inputBuffer;
	delete [] m_outputBuffer;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_BaseStreamBuffer::sync()
{
	return buffer_out();
}

//////////////////////////////////////////////////////////////////////////////
int
OW_BaseStreamBuffer::buffer_out()
{
	int cnt = pptr() - pbase();
	int retval = buffer_to_device(m_outputBuffer, cnt);
	pbump(-cnt);
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_BaseStreamBuffer::overflow(int c)
{
	if (buffer_out() < 0)
	{
		return EOF;
	}
	else
	{
		if (c != EOF)
			return sputc(c);
		else
			return c;
	}
}

//////////////////////////////////////////////////////////////////////////////
std::streamsize
OW_BaseStreamBuffer::xsputn(const char* s, std::streamsize n)
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
			{
				return i;
			}
		}

		return n;
	}
}


//////////////////////////////////////////////////////////////////////////////
int 
OW_BaseStreamBuffer::underflow()
{
	if (gptr() < egptr())
		return static_cast<unsigned char>(*gptr());

	if (buffer_in() < 0)
		return EOF;
	else
		return static_cast<unsigned char>(*gptr());
}

//////////////////////////////////////////////////////////////////////////////
int
OW_BaseStreamBuffer::buffer_in()
{
	int retval = buffer_from_device(m_inputBuffer,
			m_bufSize);

	if (retval <= 0)
	{
		setg(0,0,0);
		return -1;
	}
	else
	{
		setg(m_inputBuffer, m_inputBuffer, m_inputBuffer + retval);
		return retval;
	}
}

//////////////////////////////////////////////////////////////////////////////
int
OW_BaseStreamBufferNonBuffered::overflow(int c)
{
	if (c != EOF)
	{
		char ch = (char)c;
		if (buffer_to_device(&ch, 1) < 0)
		{
			return EOF;
		}
		else
		{
			return c;
		}
	}
	return 0; // will this work? instead of traits_type::not_eof(c)
}

//////////////////////////////////////////////////////////////////////////////
int
OW_BaseStreamBufferNonBuffered::underflow()
{
	if (m_takeFromBuf)
	{
		return (unsigned char)m_charBuf;
	}
	else
	{
		char c;
		if (buffer_from_device(&c, 1) < 0)
		{
			return EOF;
		}
		else
		{
			m_takeFromBuf = true;
			m_charBuf = c;
			return (unsigned char)c;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
int
OW_BaseStreamBufferNonBuffered::uflow()
{
	if (m_takeFromBuf)
	{
		m_takeFromBuf = false;
		return (unsigned char)m_charBuf;
	}
	else
	{
		char c;
		if (buffer_from_device(&c, 1) < 0)
		{
			return EOF;
		}
		else
		{
			m_charBuf = c;
			return (unsigned char)c;
		}
	}
}

std::streamsize 
OW_BaseStreamBufferNonBuffered::xsputn(const char* s, std::streamsize n)
{
	if (buffer_to_device(s, n) < 0)
	{
		return EOF;
	}
	else
	{
		return n;
	}
}


std::streamsize 
OW_BaseStreamBufferNonBuffered::xsgetn(char* s, std::streamsize n)
{
	if (n == 0)
	{
		return 0;
	}

	if (n == 1)
	{
		int ufl = uflow();
		if (ufl != EOF)
		{
			*s = (char)ufl;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	char* bufToWrite = s;
	std::streamsize numToRead = n;
	int numToAdd = 0; // number to adjust rval
	if (m_takeFromBuf)
	{
		m_takeFromBuf = false;
		*bufToWrite = m_charBuf;
		++bufToWrite;
		numToRead = n - 1;
		numToAdd = 1;
	}

	std::streamsize rval = buffer_from_device(bufToWrite, numToRead);
	if (rval < 0)
	{
		return EOF;
	}
	else return (rval + numToAdd);
}


OW_BaseStreamBufferNonBuffered::OW_BaseStreamBufferNonBuffered()
	: std::streambuf(), m_charBuf(0), m_takeFromBuf(false)
{
}

