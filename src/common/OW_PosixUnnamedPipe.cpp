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

#include "OW_config.h"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_AutoPtr.hpp"

extern "C"
{
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}

#include <cstring>

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_UnnamedPipeRef
OW_UnnamedPipe::createUnnamedPipe(OW_Bool doOpen)
{
	return OW_UnnamedPipeRef(new OW_PosixUnnamedPipe(doOpen));
}

//////////////////////////////////////////////////////////////////////////////
OW_PosixUnnamedPipe::OW_PosixUnnamedPipe(OW_Bool doOpen)
{
	m_fds[0] = m_fds[1] = -1;
	if(doOpen)
	{
		open();
	}
}
	
//////////////////////////////////////////////////////////////////////////////
OW_PosixUnnamedPipe::~OW_PosixUnnamedPipe()
{
	close();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_PosixUnnamedPipe::setOutputBlocking(OW_Bool outputIsBlocking)
{
	// If not opened, ignore?
	if(m_fds[0] == -1)
	{
		return;
	}

	int fdflags = fcntl(m_fds[1], F_GETFL, 0);

	if(outputIsBlocking)
	{
		fdflags ^= O_NONBLOCK;
	}
	else
	{
		fdflags |= O_NONBLOCK;

	}

	fcntl(m_fds[1], F_SETFL, fdflags);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_PosixUnnamedPipe::open()
{
	if(m_fds[0] != -1)
	{
		close();
	}

	if(::pipe(m_fds) == -1)
	{
		m_fds[0] = m_fds[1] = -1;
		OW_THROW(OW_UnnamedPipeException, ::strerror(errno));
	}

	fcntl(m_fds[1], F_SETFL, O_NONBLOCK);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_PosixUnnamedPipe::close()
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
OW_PosixUnnamedPipe::closeInputHandle()
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
OW_PosixUnnamedPipe::closeOutputHandle()
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
OW_PosixUnnamedPipe::write(const void* data, int dataLen, OW_Bool errorAsException)
{
	int rc = -1;

	if(m_fds[1] != -1)
	{
        // Always use the regular write in this method, instead of the
        // pth_write. This is because the signal handler calls this method
        // indirectly when it pushes a signal that has been received.
		rc = ::write(m_fds[1], data, dataLen);
	}
	if (errorAsException && rc == -1)
	{
		OW_THROW(OW_Exception, "pipe write failed");
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_PosixUnnamedPipe::read(void* buffer, int bufferLen, OW_Bool errorAsException)
{
	int rc = -1;

	if(m_fds[0] != -1)
	{
#ifdef OW_USE_GNU_PTH
        rc = pth_read(m_fds[0], buffer, bufferLen);
#else
		rc = ::read(m_fds[0], buffer, bufferLen);
#endif
	}
	if (errorAsException && rc == -1)
	{
		OW_THROW(OW_Exception, "pipe write failed");
	}
	return rc;
}

//////////////////////////////////////////////////////////////////////////////
OW_Select_t
OW_PosixUnnamedPipe::getSelectObj() const
{
	return m_fds[0];
}

