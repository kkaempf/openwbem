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

#include <iostream> // for cerr
#include <cstring> // for memcpy

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
	// NOTE: If an exception escapes this function, __terminate will be called
	// for gcc 2.95.2
	try
	{
		int cnt = pptr() - pbase();
		int retval = buffer_to_device(m_outputBuffer, cnt);
		pbump(-cnt);
		return retval;
	}
	catch (const OW_Exception& e)
	{
		std::cerr << "Caught OW_Exception in OW_BaseStreamBuffer::buffer_out(): " << e << std::endl;
		return EOF;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Caught exception in OW_BaseStreamBuffer::buffer_out(): " << e.what() << std::endl;
		return EOF;
	}
	catch (...)
	{
		std::cerr << "Caught unknown exception in OW_BaseStreamBuffer::buffer_out()" << std::endl;
		return EOF;
	}
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
	// NOTE: If an exception escapes this function, __terminate will be called
	// for gcc 2.95.2
	try
	{
		if (gptr() < egptr())
			return static_cast<unsigned char>(*gptr());

		if (buffer_in() < 0)
			return EOF;
		else
			return static_cast<unsigned char>(*gptr());
	}
	catch (const OW_Exception& e)
	{
		std::cerr << "Caught OW_Exception in OW_BaseStreamBuffer::underflow(): " << e << std::endl;
		return EOF;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Caught exception in OW_BaseStreamBuffer::underflow(): " << e.what() << std::endl;
		return EOF;
	}
	catch (...)
	{
		std::cerr << "Caught unknown exception in OW_BaseStreamBuffer::underflow()" << std::endl;
		return EOF;
	}
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


