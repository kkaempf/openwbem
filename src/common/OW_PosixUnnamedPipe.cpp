/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_IOException.hpp"
#include "OW_Format.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_Assertion.hpp"

extern "C"
{
#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <errno.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
#ifdef OW_WIN32
#include <io.h>
#endif
}
#include <cstring>

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
// STATIC
UnnamedPipeRef
UnnamedPipe::createUnnamedPipe(EOpen doOpen)
{
	return UnnamedPipeRef(new PosixUnnamedPipe(doOpen));
}
//////////////////////////////////////////////////////////////////////////////
PosixUnnamedPipe::PosixUnnamedPipe(EOpen doOpen)
	: m_blocking(false)
{
	m_fds[0] = m_fds[1] = -1;
	if(doOpen)
	{
		open();
	}
	setTimeouts(60 * 10); // 10 minutes. This helps break deadlocks when using safePopen()
	setBlocking(true);
}
	
//////////////////////////////////////////////////////////////////////////////
PosixUnnamedPipe::~PosixUnnamedPipe()
{
	close();
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::setBlocking(bool outputIsBlocking)
{
	// precondition
	OW_ASSERT(m_fds[0] != -1 && m_fds[1] != -1);

#ifdef OW_WIN32
	unsigned long argp = !outputIsBlocking;
	if (ioctlsocket(m_fds[0], FIONBIO, &argp) != 0)
		OW_THROW(IOException, "Failed to set pipe to non-blocking");
	if (ioctlsocket(m_fds[1], FIONBIO, &argp) != 0)
		OW_THROW(IOException, "Failed to set pipe to non-blocking");
#else
	for (size_t i = 0; i <= 1; ++i)
	{
		int fdflags = fcntl(m_fds[i], F_GETFL, 0);
		if (fdflags == -1)
		{
			OW_THROW(IOException, "Failed to set pipe to non-blocking");
		}
		if(outputIsBlocking)
		{
			fdflags &= !O_NONBLOCK;
		}
		else
		{
			fdflags |= O_NONBLOCK;
		}
		if (fcntl(m_fds[i], F_SETFL, fdflags) == -1)
		{
			OW_THROW(IOException, "Failed to set pipe to non-blocking");
		}
	}

#endif
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::setOutputBlocking(bool outputIsBlocking)
{
	// precondition
	OW_ASSERT(m_fds[1] != -1);

#ifdef OW_WIN32
	unsigned long argp = !outputIsBlocking;
	if (ioctlsocket(m_fds[1], FIONBIO, &argp) != 0)
		OW_THROW(IOException, "Failed to set pipe to non-blocking");
#else
	int fdflags = fcntl(m_fds[1], F_GETFL, 0);
	if (fdflags == -1)
	{
		OW_THROW(IOException, "Failed to set pipe to non-blocking");
	}
	if(outputIsBlocking)
	{
		fdflags ^= O_NONBLOCK;
	}
	else
	{
		fdflags |= O_NONBLOCK;
	}
	if (fcntl(m_fds[1], F_SETFL, fdflags) == -1)
	{
		OW_THROW(IOException, "Failed to set pipe to non-blocking");
	}
#endif
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::open()
{
	if(m_fds[0] != -1)
	{
		close();
	}
#ifdef OW_WIN32
	if(::_pipe(m_fds, 2560, _O_BINARY) == -1)
#else
	if(::pipe(m_fds) == -1)
#endif
	{
		m_fds[0] = m_fds[1] = -1;
		OW_THROW(UnnamedPipeException, ::strerror(errno));
	}
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::close()
{
	int rc = -1;
	if(m_fds[0] != -1)
	{
		rc = ::close(m_fds[0]);
		m_fds[0] = -1;
	}
	if(m_fds[1] != -1)
	{
		rc = ::close(m_fds[1]);
		m_fds[1] = -1;
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::closeInputHandle()
{
	int rc = -1;
	if(m_fds[0] != -1)
	{
		rc = ::close(m_fds[0]);
		m_fds[0] = -1;
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::closeOutputHandle()
{
	int rc = -1;
	if(m_fds[1] != -1)
	{
		rc = ::close(m_fds[1]);
		m_fds[1] = -1;
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::write(const void* data, int dataLen, bool errorAsException)
{
	int rc = -1;
	if(m_fds[1] != -1)
	{
		if (m_blocking)
		{
			rc = SocketUtils::waitForIO(m_fds[1], m_writeTimeout, SocketFlags::E_WAIT_FOR_OUTPUT);
			if (rc != 0)
			{
				if (errorAsException)
				{
					OW_THROW(IOException, Format("SocketUtils::waitForIO failed. errno = %1(%2)", errno, strerror(errno)).c_str());
				}
				else
				{
					return rc;
				}
			}
		}

		// Always use the regular write in this method, instead of the
		// pth_write. This is because the signal handler calls this method
		// indirectly when it pushes a signal that has been received.
		rc = ::write(m_fds[1], data, dataLen);
	}
	if (errorAsException && rc == -1)
	{
		OW_THROW(IOException, Format("pipe write failed. errno = %1(%2)", errno, strerror(errno)).c_str());
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::read(void* buffer, int bufferLen, bool errorAsException)
{
	int rc = -1;
	if(m_fds[0] != -1)
	{
		if (m_blocking)
		{
			rc = SocketUtils::waitForIO(m_fds[0], m_readTimeout, SocketFlags::E_WAIT_FOR_INPUT);
			if (rc != 0)
			{
				if (errorAsException)
				{
					OW_THROW(IOException, Format("SocketUtils::waitForIO failed. errno = %1(%2)", errno, strerror(errno)).c_str());
				}
				else
				{
					return rc;
				}
			}
		}

		rc = ::read(m_fds[0], buffer, bufferLen);
	}
	if (errorAsException && rc == -1)
	{
		OW_THROW(IOException, Format("pipe read failed. errno = %1(%2)", errno, strerror(errno)).c_str());
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
Select_t
PosixUnnamedPipe::getSelectObj() const
{
	return m_fds[0];
}

} // end namespace OpenWBEM

