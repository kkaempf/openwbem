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
#include "OW_SocketException.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_Assertion.hpp"
#include "OW_Socket.hpp"
#include "OW_Format.hpp"
#include "OW_Thread.hpp"

#ifndef OW_HAVE_GETHOSTBYNAME_R
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#endif

extern "C"
{
#if !defined(OW_WIN32)
#ifdef OW_HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#ifdef OW_HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
}

#include <cstring>
#include <cstdio>
#include <cerrno>

namespace OpenWBEM
{

namespace SocketUtils 
{
#if defined(OW_WIN32)
//////////////////////////////////////////////////////////////////////////////
String
getLastErrorMsg()
{
	LPVOID lpMsgBuf;
	if (!::FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				::WSAGetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL ))
	{
		return OpenWBEM::String();
	}

	OpenWBEM::String rmsg((const char*)lpMsgBuf);

	// Free the buffer.
	::LocalFree(lpMsgBuf);
	return rmsg;
}
#endif

//////////////////////////////////////////////////////////////////////////////
String
inetAddrToString(UInt64 addr)
{
	struct in_addr iaddr;
	iaddr.s_addr = addr;
	String s(inet_ntoa(iaddr));
	return s;
}
#ifndef MAX
	#define MAX(A,B) (((A) > (B))? (A): (B))
#endif

#if defined(OW_WIN32)
int
waitForIO(SocketHandle_t fd, HANDLE eventArg, int timeOutSecs,
	long networkEvents)
{
	OW_ASSERT(Socket::getShutDownMechanism() != NULL);

	DWORD timeout = (timeOutSecs > 0)
		? static_cast<DWORD>(timeOutSecs * 1000)
		: INFINITE;

	if (networkEvents != -1L)
	{
		::WSAEventSelect(fd, eventArg, networkEvents);
	}

	HANDLE events[2];
	events[0] = Socket::getShutDownMechanism();
	events[1] = eventArg;

	DWORD index = ::WaitForMultipleObjects(
		2,
		events,
		FALSE,
		timeout);

	int cc;

	switch (index)
	{
		case WAIT_FAILED:
			cc = -1;
			break;
		case WAIT_TIMEOUT:
			cc = ETIMEDOUT;
			break;
		default:
			index -= WAIT_OBJECT_0;
			// If not shutdown event, then reset
			if (index != 0)
			{
				::ResetEvent(eventArg);
				cc = 0;
			}
			else
			{
				// Shutdown handle was signaled
				cc = -2;
			}
			break;
	}

	return cc;
}

#else
//////////////////////////////////////////////////////////////////////////////
int
waitForIO(SocketHandle_t fd, int timeOutSecs, SocketFlags::EWaitDirectionFlag forInput)
{
	if (fd == -1)
	{
		errno = EBADF;
		return -1;
	}

	fd_set readfds;
	fd_set writefds;
	int rc;
	struct timeval *ptimeval = 0;
	struct timeval timeout;
	PosixUnnamedPipeRef lUPipe;
	int pipefd = -1;
	if (Socket::getShutDownMechanism())
	{
  		UnnamedPipeRef foo = Socket::getShutDownMechanism();
  		lUPipe = foo.cast_to<PosixUnnamedPipe>();
		OW_ASSERT(lUPipe);
		pipefd = lUPipe->getInputHandle();
	}
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	if (timeOutSecs != -1)
	{
		timeout.tv_sec = timeOutSecs;
		timeout.tv_usec = 0;
		ptimeval = &timeout;
	}
	int maxfd = fd;
	if (pipefd != -1)
	{
		FD_SET(pipefd, &readfds);
		maxfd = MAX(fd, pipefd);
	}
	if (forInput == SocketFlags::E_WAIT_FOR_INPUT)
	{
		FD_SET(fd, &readfds);
	}
	else
	{
		FD_SET(fd, &writefds);
	}
	rc = ::select(maxfd + 1, &readfds, &writefds,
					  NULL, ptimeval);
	switch (rc)
	{
		case 0:
			rc = ETIMEDOUT;
			break;
		case -1:
			if (errno == EINTR)
			{
				Thread::testCancel();
			}
			break;
		default:
			if (pipefd != -1)
			{
				if (FD_ISSET(pipefd, &readfds))
				{
					rc = -1;
				}
				else
				{
					rc = 0;
				}
			}
			else
			{
				rc = 0;
			}
	}
	return rc;
}
#endif	// 

#ifndef OW_HAVE_GETHOSTBYNAME_R
} // end namespace SocketUtils
extern Mutex gethostbynameMutex;  // defined in SocketAddress.cpp
namespace SocketUtils {
#endif

String getFullyQualifiedHostName()
{
	char hostName [2048];
	if (gethostname (hostName, sizeof(hostName)) == 0)
	{
#ifndef OW_HAVE_GETHOSTBYNAME_R
		MutexLock lock(gethostbynameMutex);
		struct hostent *he;
		if ((he = gethostbyname (hostName)) != 0)
		{
		   return he->h_name;
		}
		else
		{
			OW_THROW(SocketException, Format("SocketUtils::getFullyQualifiedHostName: gethostbyname failed: %1", h_errno).c_str());
		}
#else
		hostent hostbuf;
		hostent* host = &hostbuf;
#if (OW_GETHOSTBYNAME_R_ARGUMENTS == 6 || OW_GETHOSTBYNAME_R_ARGUMENTS == 5)
		char buf[2048];
		int h_err = 0;
#elif (OW_GETHOSTBYNAME_R_ARGUMENTS == 3)
		hostent_data hostdata;
		int h_err = 0;		
#else
#error Not yet supported: gethostbyname_r() with other argument counts.
#endif /* OW_GETHOSTBYNAME_R_ARGUMENTS */
		// gethostbyname_r will randomly fail on some platforms/networks
		// maybe the DNS server is overloaded or something.  So we'll
		// give it a few tries to see if it can get it right.
		bool worked = false;
		for (int i = 0; i < 10 && (!worked || host == 0); ++i)
		{
#if (OW_GETHOSTBYNAME_R_ARGUMENTS == 6)		  
			if (gethostbyname_r(hostName, &hostbuf, buf, sizeof(buf),
						&host, &h_err) != -1)
			{
				worked = true;
				break;
			}
#elif (OW_GETHOSTBYNAME_R_ARGUMENTS == 5)
			// returns NULL if not successful
			if ((host = gethostbyname_r(hostName, &hostbuf, buf, sizeof(buf), &h_err))) {
				worked = true;
				break;
			}
#elif (OW_GETHOSTBYNAME_R_ARGUMENTS == 3)
			if (gethostbyname_r(hostName, &hostbuf, &hostdata) == 0)
			{
				worked = true;
				break;
			}
			else
			{
			  h_err = h_errno;
			}
#else
#error Not yet supported: gethostbyname_r() with other argument counts.
#endif /* OW_GETHOSTBYNAME_R_ARGUMENTS */
		}
		if (worked && host != 0)
		{
			return host->h_name;
		}
		else
		{
			OW_THROW(SocketException, Format("SocketUtils::getFullyQualifiedHostName: gethostbyname_r(%1) failed: %2", hostName, h_err).c_str());
		}
#endif
	}
	else
	{
		OW_THROW(SocketException, Format("SocketUtils::getFullyQualifiedHostName: gethostname failed: %1(%2)", errno, strerror(errno)).c_str());
	}
	return "";
}
} // end namespace SocketUtils

} // end namespace OpenWBEM

