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

#include "OW_config.h"
#include "OW_Socket.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Assertion.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Exception.hpp"
#include "OW_IOException.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_SocketImpl.hpp"


namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(Socket);
OW_DEFINE_EXCEPTION_WITH_ID(SocketTimeout);

Socket::ShutDownMechanism_t Socket::s_shutDownMechanism = 0;

//////////////////////////////////////////////////////////////////////////////
Socket::Socket()
	: m_impl(new SocketImpl)
{
}

static bool b_gotShutDown = false;
static Mutex shutdownMutex;
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
Socket::shutdownAllSockets()
{
	MutexLock mlock(shutdownMutex);

	OW_ASSERT(s_shutDownMechanism != 0);
	if (!s_shutDownMechanism)
	{
		return;
	}

	b_gotShutDown = true;
#if defined(OW_WIN32)
	::SetEvent(s_shutDownMechanism);
#else
	if (s_shutDownMechanism->writeString("die!") == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed writing to socket shutdown pipe");
	}
#endif
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
Socket::createShutDownMechanism()
{
	MutexLock mlock(shutdownMutex);
	if (!s_shutDownMechanism)
	{
#if defined(OW_WIN32)
		s_shutDownMechanism = ::CreateEvent(0, TRUE, FALSE, 0);
		OW_ASSERT(s_shutDownMechanism != 0);
#else
		s_shutDownMechanism = UnnamedPipe::createUnnamedPipe();
		s_shutDownMechanism->setBlocking(UnnamedPipe::E_NONBLOCKING);
#endif
		b_gotShutDown = false;
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
Socket::deleteShutDownMechanism()
{
	MutexLock mlock(shutdownMutex);
	if (s_shutDownMechanism)
	{
#if defined(OW_WIN32)
		::CloseHandle(s_shutDownMechanism);
#endif
		s_shutDownMechanism = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
Socket::gotShutDown()
{
	MutexLock mlock(shutdownMutex);
	return b_gotShutDown;
}

} // end namespace OW_NAMESPACE

