/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Quest Software, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_IPCIO.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_String.hpp"
#include "OW_DescriptorUtils.hpp"
#include "OW_Timeout.hpp"
#include "OW_IOException.hpp"
#include "OW_Process.hpp"

#include <unistd.h>

#define RETHROW_IOEXCEPTION(msg) \
	catch (IOException & e) \
	{ \
		OW_THROW_SUBEX(IPCIOException, (msg), e); \
	}

namespace
{
	std::size_t const IO_BUFFER_SIZE = 4 * 1024;
}

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION(IPCIO);

IPCIO::IPCIO(AutoDescriptor peer_descriptor, Timeout const & timeout)
: m_pipe(UnnamedPipe::createUnnamedPipeFromDescriptor(peer_descriptor)),
  m_streambuf()
{
	m_pipe->setTimeouts(timeout);
	m_streambuf = new IOIFCStreamBuffer(m_pipe.getPtr(), IOIFCStreamBuffer::E_IN_OUT, IO_BUFFER_SIZE);
	m_streambuf->setErrorAction(IOIFC::E_THROW_ON_ERROR);
}

IPCIO::~IPCIO()
{
	try
	{
		this->put_sync();
		this->close();
	}
	catch (...)
	{
	}
}

void IPCIO::close()
{
	m_pipe->close();
}

bool IPCIO::sgetn(
	char * buf, std::streamsize count, EOFAction eof_action, EBuffering eb)
{
	std::streamsize nr;
	if (eb == E_BUFFERED)
	{
		try
		{
			nr = m_streambuf->sgetn(buf, count);
			OW_ASSERT(nr <= count);
		}
		RETHROW_IOEXCEPTION("IPCIO::sgetn")
	}
	else
	{
		this->get_sync();
		ssize_t n;
		for (nr = 0; nr < count; nr += n)
		{
#ifdef BLOCXX_WIN32
#pragma message(Reminder "TODO: It's necessary to implement for Win correctly to remove 'reinterpret_cast'!")
			n = ::read(reinterpret_cast<int>(m_pipe->getInputDescriptor()), buf + nr, count - nr);
#else
			n = ::read(m_pipe->getInputDescriptor(), buf + nr, count - nr);
#endif
			if (n == 0)
			{
				break;
			}
			if (n < 0)
			{
				if (errno == EINTR)
				{
					n = 0;
					continue;
				}
				OW_THROW_ERRNO_MSG(IPCIOException, "Read failed");
			}
		}
	}
	if (nr == count)
	{
		return true;
	}
	else if (nr == 0 && eof_action == E_RETURN_FALSE_ON_EOF)
	{
		return false;
	}
	else
	{
		OW_THROW(IPCIOException, Format("Incomplete read, %1 of %2 bytes", nr, count).c_str());
	}
}

void IPCIO::sputn(char const * buf, std::streamsize count)
{
	try
	{
		if (m_streambuf->sputn(buf, count) < count)
		{
			OW_THROW(IPCIOException, "Could not complete write");
		}
	}
	RETHROW_IOEXCEPTION("IPCIO::sputn")
}

// For Unix, FileHandle and int should be the same type.  The argument is
// declared as an int here because the code assumes the argument is an int.
//
void IPCIO::put_handle(FileHandle descr)
{
	this->put_sync();

	try
	{
		m_pipe->passDescriptor(descr, m_pipe);
	}
	catch(IOException& e)
	{
		OW_THROW_SUBEX(IPCIOException, "m_pipe.passDescriptor failed", e);
	}
}

// For Unix, FileHandle and int should be the same type.  The return type is
// declared as an int here because the code assumes it is an int.
//
AutoDescriptor IPCIO::get_handle()
{
	this->get_sync();

	try
	{
		return m_pipe->receiveDescriptor(m_pipe);
	}
	RETHROW_IOEXCEPTION("IPCIO::get_handle")
}

void IPCIO::put_sync()
{
	try
	{
		errno = 0;
		if (m_streambuf->pubsync() < 0)
		{
			OW_THROW_ERRNO_MSG(IPCIOException, "IPCIO::put_sync() failed");
		}
	}
	RETHROW_IOEXCEPTION("IPCIO::put_sync")
}

void IPCIO::get_sync()
{
	try
	{
		if (m_streambuf->in_avail() > 0)
		{
			OW_THROW(IPCIOException, "message longer than expected");
		}
	}
	RETHROW_IOEXCEPTION("IPCIO::get_sync")
}

void ipcio_put(IPCIO & io, String const & s)
{
	ipcio_put(io, s.c_str(), s.length());
}

void ipcio_put(IPCIO & io, char const * s)
{
	if (!s)
	{
		s = "";
	}
	ipcio_put(io, s, std::strlen(s));
}

void ipcio_put(IPCIO & io, char const * s, std::size_t len)
{
	OW_ASSERT(s != 0);
	ipcio_put(io, len);
	io.sputn(s, len);
}

bool ipcio_get(
	IPCIO & io, String & s, std::size_t max_len,
	IPCIO::EOFAction eof_action, IPCIO::EBuffering eb
)
{
	std::size_t len;
	if (!ipcio_get(io, len, eof_action, eb))
	{
		return false;
	}
	std::size_t read_len = (std::min)(len, max_len);
	AutoPtrVec<char> buf(new char[read_len + 1]);
	io.sgetn(buf.get(), read_len, IPCIO::E_THROW_ON_EOF, eb);
	buf[read_len] = '\0';

	// consume truncated portion of string
	for (len -= read_len; len > 0; len -= read_len)
	{
		std::size_t const SKIPBUFSZ = 4096;
		char skipbuf[SKIPBUFSZ];
		read_len = (std::min)(len, SKIPBUFSZ);
		io.sgetn(skipbuf, read_len, IPCIO::E_THROW_ON_EOF, eb);
	}

	String(String::E_TAKE_OWNERSHIP, buf.release(), read_len).swap(s);
	return true;
}

} // namespace OW_NAMESPACE

