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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_Select.hpp"
#include "OW_Assertion.hpp"
#include "OW_Thread.hpp" // for testCancel()

#if defined(OW_WIN32)
#include "OW_AutoPtr.hpp"
#include <cassert>
#endif

extern "C"
{
#ifdef OW_HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef OW_HAVE_SYS_TIME_H
 #include <sys/time.h>
#endif

#include <sys/types.h>

#ifdef OW_HAVE_UNISTD_H
 #include <unistd.h>
#endif

#include <errno.h>
}

namespace OpenWBEM
{

namespace Select
{
#if defined(OW_WIN32)
int
select(const SelectTypeArray& selarray, UInt32 ms)
{
	int rc;
	size_t hcount = static_cast<DWORD>(selarray.size());
	AutoPtrVec<HANDLE> hdls(new HANDLE[hcount]);

	for (size_t i = 0; i < hcount; i++)
	{
		if(selarray[i].sockfd != INVALID_SOCKET
			&& selarray[i].networkevents)
		{
			::WSAEventSelect(selarray[i].sockfd, 
				selarray[i].event, selarray[i].networkevents);
		}
				
		hdls[i] = selarray[i].event;
	}

	DWORD timeout = (ms != ~0U) ? ms : INFINITE;
	DWORD cc = ::WaitForMultipleObjects(hcount, hdls.get(), FALSE, timeout);

	assert(cc != WAIT_ABANDONED);

	switch (cc)
	{
		case WAIT_FAILED:
			rc = Select::SELECT_ERROR;
			break;
		case WAIT_TIMEOUT:
			rc = Select::SELECT_TIMEOUT;
			break;
		default:
			rc = cc - WAIT_OBJECT_0;
			
			// If this is a socket, set it back to 
			// blocking mode
			if(selarray[rc].sockfd != INVALID_SOCKET)
			{
				if(selarray[rc].networkevents
					&& selarray[rc].doreset == false)
				{
					::WSAEventSelect(selarray[rc].sockfd, 
						selarray[rc].event, selarray[rc].networkevents);
				}
				else
				{
					// Set socket back to blocking
					::WSAEventSelect(selarray[rc].sockfd, 
						selarray[rc].event, 0);
					u_long ioctlarg = 0;
					::ioctlsocket(selarray[rc].sockfd, FIONBIO, &ioctlarg);
				}
			}
			break;
	}

	return cc;
}

#else
//////////////////////////////////////////////////////////////////////////////
int
select(const SelectTypeArray& selarray, UInt32 ms)
{
	int rc;
	fd_set rfds;
	// here we spin checking for thread cancellation every so often.
	UInt32 remainingWait = ms;
	do
	{
		int maxfd = 0;
		FD_ZERO(&rfds);
		for (size_t i = 0; i < selarray.size(); i++)
		{
			OW_ASSERT(selarray[i] >= 0);
			if (maxfd < selarray[i])
			{
				maxfd = selarray[i];
			}
			FD_SET(selarray[i], &rfds);
		}

		const UInt32 waitMs = 100; // 1/10 of a second
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = std::min((waitMs % 1000), remainingWait) * 1000;

		rc = ::select(maxfd+1, &rfds, NULL, NULL, &tv);

		Thread::testCancel();
		if (ms != ~0U)
		{
			remainingWait -= std::min(waitMs, remainingWait);
		}
	} while (rc == 0 && remainingWait > 0);
	
	if (rc < 0)
	{
		if (errno == EINTR)
		{
			return Select::SELECT_INTERRUPTED;
		}
		else
		{
			return Select::SELECT_ERROR;
		}
	}
	if (rc == 0)
	{
		return Select::SELECT_TIMEOUT;
	}
	for (size_t i = 0; i < selarray.size(); i++)
	{
		if (FD_ISSET(selarray[i], &rfds))
		{
			return static_cast<int>(i);
		}
	}
	OW_THROW(AssertionException, "Logic error in Select. Didn't find file handle");
	return Select::SELECT_ERROR;
}
#endif	// #else OW_WIN32

} // end namespace Select

} // end namespace OpenWBEM

