/*******************************************************************************
* Copyright (C) 2006, Quest Software, Inc. All rights reserved.
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

#include "OW_IPCIO_noexcept.hpp"
#include "Cifc_DescriptorUtils_noexcept.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>

namespace OW_NAMESPACE
{
	class IPCIO_noexcept::Buffer
	{
	public:
		Buffer(int descriptor);
		~Buffer();

		// These member functions are meant to mimic the corresponding
		// std::streambuf member functions.

		std::streamsize sgetn(char * buf, std::streamsize count);
		std::streamsize sputn(char const * buf, std::streamsize count);
		int pubsync();
		std::streamsize in_avail();

	private:
		static size_t const IN_BUFSZ = 4 * 1024;
		static size_t const OUT_BUFSZ = 4 * 1024;

		size_t flush();

		int descr;
		char * inp_beg; // first unread character
		char * inp_end; // one past last unread character in inp_buf
		char inp_buf[IN_BUFSZ];
		char * out_beg; // one past last character written to out_buf
		char out_buf[OUT_BUFSZ];
	};

	IPCIO_noexcept::IPCIO_noexcept()
	: m_descriptor(0),
	  m_connected(false),
	  m_ok(false),
	  m_buffer(0)
	{
		// To be on the safe side, the initializations of all data members
		// correspond to zero-initialization.
	}

	IPCIO_noexcept::~IPCIO_noexcept()
	{
		if (m_descriptor > 0)
		{
			::close(m_descriptor);
		}
	}

	void IPCIO_noexcept::connect(int descr)
	{
		if (descr >= 0)
		{
			m_descriptor = descr;
			m_connected = true;
			m_ok = true;
			m_buffer.reset(new Buffer(descr));
		}
		else
		{
			m_connected = true;
			m_ok = false;
		}
	}

	void ipcio_put_str(IPCIO_noexcept & io, char const * s)
	{
		if (s == 0)
		{
			s = "";
		}
		std::size_t len = std::strlen(s);
		ipcio_put(io, len);
		io.sputn(s, len);
	}

	void ipcio_get_str(IPCIO_noexcept & io, std::vector<char> & v)
	{
		std::size_t len;
		ipcio_get(io, len);
		if (!io.ok())
		{
			return;
		}
		std::vector<char> result(len+1);
		io.sgetn(&result[0], len);
		result[len] = '\0';
		if (io.ok())
		{
			result.swap(v);
		}
	}

	void IPCIO_noexcept::sgetn(char * buf, std::streamsize count, EBuffering eb)
	{
		if (!m_ok)
		{
			return;
		}
		std::streamsize nr;
		if (eb == E_BUFFERED)
		{
			nr = m_buffer->sgetn(buf, count);
		}
		else
		{
			this->get_sync();
			ssize_t n;
			for (nr = 0; nr < count; nr += n)
			{
				n = ::read(m_descriptor, buf + nr, count - nr);
				if (n == 0)
				{
					break;
				}
				if (n < 0)
				{
					if (errno == EINTR)
					{
						n = 0;
					}
					else
					{
						m_ok = false;
						return;
					}
				}
			}
		}
		if (nr != count)
		{
			m_ok = false;
		}
	}

	void IPCIO_noexcept::sputn(char const * buf, std::streamsize count)
	{
		if (m_ok && m_buffer->sputn(buf, count) < count)
		{
			m_ok = false;
		}
	}

	void IPCIO_noexcept::put_sync()
	{
		if (m_ok && m_buffer->pubsync() < 0)
		{
			m_ok = false;
		}
	}

	void IPCIO_noexcept::get_sync()
	{
		if (m_ok && m_buffer->in_avail() > 0)
		{
			m_ok = false;
		}
	}

	int IPCIO_noexcept::get_handle()
	{
		if (!m_ok)
		{
			return -1;
		}
		this->get_sync();
		size_t const BUFSZ = 64;
		char errbuf[BUFSZ];
		AutoDescriptor fd = receiveDescriptor(m_descriptor, errbuf, BUFSZ);
		if (fd.get() < 0)
		{
			m_ok = false;
		}
		return fd.release();
	}


	// Buffer member functions

	IPCIO_noexcept::Buffer::Buffer(int descriptor)
	: descr(descriptor),
	  inp_beg(inp_buf),
	  inp_end(inp_buf),
	  out_beg(out_buf)
	{
	}

	IPCIO_noexcept::Buffer::~Buffer()
	{
		this->flush();
	}

	std::streamsize
	IPCIO_noexcept::Buffer::sgetn(char * buf, std::streamsize count)
	{
		char * bufend = buf + count;
		size_t ncopy;
		char * pos;
		for (pos = buf; pos < bufend; inp_beg += ncopy, pos += ncopy)
		{
			if (inp_beg == inp_end)
			{
				inp_beg = inp_buf;
				ssize_t nr;
				do
				{
					nr = ::read(descr, inp_beg, IN_BUFSZ);
				} while (nr < 0 && errno == EINTR);
				if (nr <= 0)
				{
					inp_end = inp_beg;
					return pos - buf;
				}
				inp_end = inp_beg + nr;
			}
			ncopy = std::min(inp_end - inp_beg, bufend - pos);
			std::memcpy(pos, inp_beg, ncopy);
		}
		return pos - buf;
	}

	size_t IPCIO_noexcept::Buffer::flush()
	{
		ssize_t nw;
		char * pos;
		for (pos = out_buf; pos < out_beg; pos += nw)
		{
			do
			{
				nw = ::write(descr, pos, out_beg - pos);
			} while (nw < 0 && errno == EINTR);
			if (nw <= 0)
			{
				break;
			}
		}
		out_beg = out_buf;
		return pos - out_buf;
	}

	std::streamsize
	IPCIO_noexcept::Buffer::sputn(char const * buf, std::streamsize count)
	{
		char * out_end = out_buf + OUT_BUFSZ;
		char const * bufend = buf + count;
		size_t ncopy;
		char const * pos;
		for (pos = buf; pos < bufend; out_beg += ncopy, pos += ncopy)
		{
			if (out_beg == out_end)
			{
				if (this->flush() < OUT_BUFSZ)
				{
					return pos - buf;
				}
			}
			ncopy = std::min(out_end - out_beg, bufend - pos);
			std::memcpy(out_beg, pos, ncopy);
		}
		return pos - buf;
	}

	int IPCIO_noexcept::Buffer::pubsync()
	{
		size_t n = out_beg - out_buf;
		return this->flush() < n ? -1 : 0;
	}

	std::streamsize IPCIO_noexcept::Buffer::in_avail()
	{
		return static_cast<std::streamsize>(inp_end - inp_beg);
	}
}
