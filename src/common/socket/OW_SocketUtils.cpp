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
#include "OW_SocketException.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_Assertion.hpp"
#include "OW_Socket.hpp"

extern "C"
{
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_SocketUtils::inetAddrToString(OW_UInt64 addr)
{
	struct in_addr iaddr;
	iaddr.s_addr = addr;
	OW_String s(inet_ntoa(iaddr));
	return s;
}

#ifndef OW_MAX
	#define OW_MAX(A,B) (((A) > (B))? (A): (B))
#endif
//////////////////////////////////////////////////////////////////////////////
typedef OW_Reference<OW_PosixUnnamedPipe> OW_PosixUnnamedPipeRef;

int
OW_SocketUtils::waitForIO(OW_SocketHandle_t fd, int timeOutSecs, OW_Bool forInput)
/*throw (OW_SocketException)*/
{
	fd_set readfds;
	fd_set writefds;
	int rc;
	struct timeval *ptimeval = 0;
	struct timeval timeout;

	OW_PosixUnnamedPipeRef lUPipe;

	int pipefd = 0;

	if (OW_Socket::m_pUpipe)
	{
		lUPipe = OW_Socket::m_pUpipe.cast_to<OW_PosixUnnamedPipe>();
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

	if (lUPipe)
	{
		FD_SET(pipefd, &readfds);
		maxfd = OW_MAX(fd, pipefd);
	}
	if (forInput)
	{
		FD_SET(fd, &readfds);
	}
	else
	{
		FD_SET(fd, &writefds);
	}

#ifdef OW_USE_GNU_PTH
	rc = ::pth_select(maxfd + 1, &readfds, &writefds,
					  NULL, ptimeval);
#else
	rc = ::select(maxfd + 1, &readfds, &writefds,
					  NULL, ptimeval);
#endif

	switch (rc)
	{
		case 0:
			rc = ETIMEDOUT;
			break;

		case -1:
			OW_THROW(OW_SocketException, "::waitForIO: select");
			rc = errno;
			break;

		default:
			if (lUPipe)
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

