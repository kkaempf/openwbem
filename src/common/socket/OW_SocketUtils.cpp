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
#include "OW_System.hpp"
#include "OW_Select.hpp"

#ifndef OW_HAVE_GETHOSTBYNAME_R
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#endif

extern "C"
{
#if !defined(OW_WIN32)
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

namespace OW_NAMESPACE
{

namespace SocketUtils 
{

//////////////////////////////////////////////////////////////////////////////
String
inetAddrToString(UInt64 addr)
{
	sockaddr_in iaddr;
	char buf[INET6_ADDRSTRLEN];
	iaddr.sin_family = AF_INET;
	iaddr.sin_addr.s_addr = addr;
	iaddr.sin_port = 0;
	String s(inet_ntop(iaddr.sin_family, &(iaddr.sin_addr), buf, sizeof(buf)));
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
	DWORD timeout = (timeOutSecs != -1)
		? static_cast<DWORD>(timeOutSecs * 1000)
		: INFINITE;

	if (networkEvents != -1L)
	{
		if(::WSAEventSelect(fd, eventArg, networkEvents) != 0)
		{
			OW_THROW(SocketException, 
				Format("WSAEventSelect failed in waitForIO: %1",
				System::lastErrorMsg(true)).c_str());
		}
	}

	int cc;
	if(Socket::getShutDownMechanism() != NULL)
	{
		HANDLE events[2];
		events[0] = Socket::getShutDownMechanism();
		events[1] = eventArg;

		DWORD index = ::WaitForMultipleObjects(
			2,
			events,
			FALSE,
			timeout);

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
	}
	else
	{
		switch(::WaitForSingleObject(eventArg, timeout))
		{
			case WAIT_OBJECT_0:
				::ResetEvent(eventArg);
				cc = 0;
				break;
			case WAIT_TIMEOUT:
				cc = ETIMEDOUT;
				break;
			default:
				cc = -1;
				break;
		}
	}

	// Set socket back to blocking
	if(::WSAEventSelect(fd, eventArg, 0) != 0)
	{
		OW_THROW(SocketException, 
			Format("Resetting socket with WSAEventSelect failed: %1",
			System::lastErrorMsg(true)).c_str());
	}
	u_long ioctlarg = 0;
	::ioctlsocket(fd, FIONBIO, &ioctlarg);
	return cc;
}

#else
//////////////////////////////////////////////////////////////////////////////
int
waitForIO(SocketHandle_t fd, int timeOutSecs, SocketFlags::EWaitDirectionFlag waitFlag)
{
	if (fd == -1)
	{
		errno = EBADF;
		return -1;
	}

	Select::SelectObject so(fd); 
	if (waitFlag == SocketFlags::E_WAIT_FOR_INPUT)
	{
		so.waitForRead = true; 
	}
	else if (waitFlag == SocketFlags::E_WAIT_FOR_OUTPUT)
	{
		so.waitForWrite = true; 
	}
	else
	{
		so.waitForRead = true; 
		so.waitForWrite = true; 
	}
	Select::SelectObjectArray selarray; 
	selarray.push_back(so); 

	PosixUnnamedPipeRef lUPipe;
	int pipefd = -1;
	if (Socket::getShutDownMechanism())
	{
  		UnnamedPipeRef foo = Socket::getShutDownMechanism();
  		lUPipe = foo.cast_to<PosixUnnamedPipe>();
		OW_ASSERT(lUPipe);
		pipefd = lUPipe->getInputHandle();
	}
	if (pipefd != -1)
	{
		so = Select::SelectObject(pipefd); 
		so.waitForRead = true; 
		selarray.push_back(so); 
	}

	int rc = Select::selectRW(selarray, timeOutSecs*1000); 
	switch (rc)
	{
	case Select::SELECT_TIMEOUT:
		rc = ETIMEDOUT; 
		break; 
	case 2:
		rc = -1; // pipe was signalled
		break; 
	case 1: 
		if (pipefd != -1)
		{
			if (selarray[1].readAvailable)
			{
				rc = -1; 
			}
		}
		if (selarray[0].writeAvailable || selarray[0].readAvailable)
		{
			rc = 0; 
		}
		break; 
	default: 
		rc = -1; 
	}
	return rc; 

}
#endif	// 

#ifndef OW_HAVE_GETHOSTBYNAME_R
} // end namespace SocketUtils
extern Mutex gethostbynameMutex;  // defined in SocketAddress.cpp
namespace SocketUtils {
#endif

#ifndef OW_WIN32
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
#else
// WIN32 defined
String getFullyQualifiedHostName()
{
	String rv;
	struct hostent *hostentp;
	char bfr[1024], ipaddrstr[128];
	struct in_addr iaHost;

	if(gethostname(bfr, sizeof(bfr)-1) == SOCKET_ERROR)
	{
		OW_THROW(SocketException, 
			Format("SocketUtils::getFullyQualifiedHostName: gethostname failed: %1(%2)", 
				WSAGetLastError(), System::lastErrorMsg(true)).c_str());
	}

	if(strchr(bfr, '.'))
	{
		// Guess we already have the DNS name
		return String(bfr);
	}

	if((hostentp = gethostbyname(bfr)) == NULL)
	{
		OW_THROW(SocketException, 
			Format("SocketUtils::getFullyQualifiedHostName: gethostbyname"
				" failed: %1(%2)", WSAGetLastError(),
				System::lastErrorMsg(true)).c_str());
	}

	if(strchr(hostentp->h_name, '.'))
	{
		rv = hostentp->h_name;
	}
	else
	{
		char buf[INET6_ADDRSTRLEN];
		sockaddr_in     addr;
		addr.sin_family = AF_INET;
		addr.sin_port   = 0;
		memcpy(&addr.sin_addr, hostentp->h_addr_list[0], sizeof(addr.sin_addr));
		rv = inet_ntop(address->sin_family, &(address->sin_addr), buf, sizeof(buf));
		iaHost.s_addr = inet_addr(rv.c_str());
		if(iaHost.s_addr != INADDR_NONE)
		{
			hostentp = gethostbyaddr((const char*)&iaHost,
				sizeof(struct in_addr), AF_INET);
			if(hostentp)
			{
				if(strchr(hostentp->h_name, '.'))
				{
					// GOT IT
					rv = hostentp->h_name;
				}
			}
		}
	}

	return rv;
}
#endif


} // end namespace SocketUtils

} // end namespace OW_NAMESPACE

