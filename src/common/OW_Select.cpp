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
#include "OW_AutoPtr.hpp"
#include "OW_Assertion.hpp"
#include "OW_Thread.hpp" // for testCancel()

#if defined(OW_WIN32)
#include <cassert>
#endif

extern "C"
{

// something goofy with epoll at the moment. 
#undef OW_HAVE_SYS_EPOLL_H

#ifndef OW_WIN32
 #ifdef OW_HAVE_SYS_EPOLL_H
  #include <sys/epoll.h>
 #elif defined (OW_HAVE_SYS_POLL_H)
  #include <sys/poll.h>
 #elif defined (OW_HAVE_SYS_SELECT_H)
  #include <sys/select.h>
 #else
  #error "port me!"
 #endif
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

namespace OW_NAMESPACE
{

namespace Select
{
#if defined(OW_WIN32)
#error "Port/Convert selectRW over to new format (see OW_Select.hpp)"
//////////////////////////////////////////////////////////////////////////////
int
selectRW(SelectObjectArray& input, SelectObjectArray& output, UInt32 ms)
{
	int rc;
	size_t hcount = static_cast<DWORD>(input.size() + output.size());
	AutoPtrVec<HANDLE> hdls(new HANDLE[hcount]);

	size_t handleidx = 0;
	for (size_t i = 0; i < input.size(); i++, handleidx++)
	{
		if(input[i].s.sockfd != INVALID_SOCKET
			&& input[i].s.networkevents)
		{
			::WSAEventSelect(input[i].s.sockfd, 
				input[i].s.event, input[i].s.networkevents);
		}
				
		hdls[handleidx] = input[i].s.event;
	}
	for (size_t i = 0; i < output.size(); i++, handleidx++)
	{
		if(output[i].s.sockfd != INVALID_SOCKET
			&& output[i].s.networkevents)
		{
			::WSAEventSelect(output[i].s.sockfd, 
				output[i].s.event, output[i].s.networkevents);
		}
				
		hdls[handleidx] = output[i].s.event;
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
			if( rc < input.size() )
			{
				if(input[rc].s.sockfd != INVALID_SOCKET)
				{
					if(input[rc].s.networkevents
						&& input[rc].s.doreset == false)
					{
						::WSAEventSelect(input[rc].s.sockfd, 
							input[rc].s.event, input[rc].s.networkevents);
					}
					else
					{
						// Set socket back to blocking
						::WSAEventSelect(input[rc].s.sockfd, 
							input[rc].s.event, 0);
						u_long ioctlarg = 0;
						::ioctlsocket(input[rc].s.sockfd, FIONBIO, &ioctlarg);
					}
				}
			}
			else
			{
				rc -= input.size();
				if(output[rc].s.sockfd != INVALID_SOCKET)
				{
					if(output[rc].s.networkevents
						&& output[rc].s.doreset == false)
					{
						::WSAEventSelect(output[rc].s.sockfd, 
							output[rc].s.event, output[rc].s.networkevents);
					}
					else
					{
						// Set socket back to blocking
						::WSAEventSelect(output[rc].s.sockfd, 
							output[rc].s.event, 0);
						u_long ioctlarg = 0;
						::ioctlsocket(output[rc].s.sockfd, FIONBIO, &ioctlarg);
					}
				}
				rc += input.size();
			}
			break;
	}

	if( rc < 0 )
		return rc;

	int availableCount = 0;
	for (size_t i = 0; i < input.size(); i++)
	{
		if( WaitForSingleObject(input[i].s.event, 0) == WAIT_OBJECT_0 )
		{
			input[i].available = true;
			++availableCount;
		}
		else
		{
			input[i].available = false;
		}
	}
	for (size_t i = 0; i < output.size(); i++)
	{
		if( WaitForSingleObject(output[i].s.event, 0) == WAIT_OBJECT_0 )
		{
			output[i].available = true;
			++availableCount;
		}
		else
		{
			output[i].available = false;
		}
	}
	return availableCount;
}

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

	return rc;
}

#else




#ifdef OW_HAVE_SYS_EPOLL_H
int
select(const SelectTypeArray& selarray, UInt32 ms)
{
	int rc = 0;
	int timeout;

	struct epoll_event event;
	event.events = EPOLLIN | EPOLLPRI;
	int epfd = epoll_create(selarray.size());
	if(epfd == -1)
	{
		// Need to return something else?
		return Select::SELECT_ERROR;
	}

	for (size_t i = 0; i < selarray.size(); i++)
	{
		OW_ASSERT(selarray[i] >= 0);
		event.data.fd = selarray[i];
		if(epoll_ctl(epfd, EPOLL_CTL_ADD, selarray[i], &event) != 0)
		{
			::close(epfd);
			// Need to return something else?
			return Select::SELECT_ERROR;
		}
	}

	// here we spin checking for thread cancellation every so often.
	const Int32 loopMicroSeconds = 100 * 1000; // 1/10 of a second
	timeval now, end;
	gettimeofday(&now, NULL);
	end = now;
	end.tv_sec  += ms / 1000;
	end.tv_usec += (ms % 1000) * 1000;

	while ((rc == 0) && ((ms == INFINITE_TIMEOUT) || (now.tv_sec < end.tv_sec)
		 || ((now.tv_sec == end.tv_sec) && (now.tv_usec < end.tv_usec))))
	{
		timeval tv;
		tv.tv_sec = end.tv_sec - now.tv_sec;
		if (end.tv_usec >= now.tv_usec)
		{
			tv.tv_usec = end.tv_usec - now.tv_usec;
		}
		else
		{
			tv.tv_sec--;
			tv.tv_usec = 1000000 + end.tv_usec - now.tv_usec;
		}

		if ((tv.tv_sec != 0) || (tv.tv_usec > loopMicroSeconds) || (ms == INFINITE_TIMEOUT))
		{
			tv.tv_sec = 0;
			tv.tv_usec = loopMicroSeconds;
		}

		timeout = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);

		Thread::testCancel();
		rc = epoll_wait(epfd, &event, 1, timeout);
		Thread::testCancel();
		gettimeofday(&now, NULL);
	}

	::close(epfd);
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
		if(event.data.fd == selarray[i])
		{
			return static_cast<int>(i);
		}
	}

	OW_THROW(AssertionException, "Logic error in Select. Didn't find file handle");
	return Select::SELECT_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// epoll version
int
selectRW(SelectObjectArray& selarray, UInt32 ms)
{
	int ecc = 0;
	int timeout;
	AutoPtrVec<epoll_event> events(new epoll_event[selarray.size()]);
	int epfd = epoll_create(selarray.size());
	if(epfd == -1)
	{
		// Need to return something else?
		return Select::SELECT_ERROR;
	}

	for (size_t i = 0; i < selarray.size(); i++)
	{
		OW_ASSERT(selarray[i].s >= 0);
		selarray[i].rAvailable = false;
		selarray[i].wAvailable = false;
		selarray[i].wasError = false;
		events[i].data.fd = selarray[i].s;
		events[i].data.u32 = i;
		events[i].events = 0;
		if(selarray[i].rEvents)
		{
			events[i].events |= (EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP);
		}
		if(selarray[i].wEvents)
		{
			events[i].events |= EPOLLOUT;
		}

		if(epoll_ctl(epfd, EPOLL_CTL_ADD, selarray[i].s, &events[i]) != 0)
		{
			::close(epfd);
			// Need to return something else?
			return Select::SELECT_ERROR;
		}
	}

	// here we spin checking for thread cancellation every so often.
	const Int32 loopMicroSeconds = 100 * 1000; // 1/10 of a second
	timeval now, end;
	gettimeofday(&now, NULL);
	end = now;
	end.tv_sec  += ms / 1000;
	end.tv_usec += (ms % 1000) * 1000;

	while ((ecc == 0) && ((ms == INFINITE_TIMEOUT) || (now.tv_sec < end.tv_sec)
		 || ((now.tv_sec == end.tv_sec) && (now.tv_usec < end.tv_usec))))
	{
		timeval tv;
		tv.tv_sec = end.tv_sec - now.tv_sec;
		if (end.tv_usec >= now.tv_usec)
		{
			tv.tv_usec = end.tv_usec - now.tv_usec;
		}
		else
		{
			tv.tv_sec--;
			tv.tv_usec = 1000000 + end.tv_usec - now.tv_usec;
		}

		if ((tv.tv_sec != 0) 
			|| (tv.tv_usec > loopMicroSeconds) || (ms == INFINITE_TIMEOUT))
		{
			tv.tv_sec = 0;
			tv.tv_usec = loopMicroSeconds;
		}

		timeout = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		Thread::testCancel();
		ecc = epoll_wait(epfd, events.get(), selarray.size(), timeout);
		Thread::testCancel();
		gettimeofday(&now, NULL);
	}

	::close(epfd);
	if (ecc < 0)
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
	if (ecc == 0)
	{
		return Select::SELECT_TIMEOUT;
	}

	for(int i = 0; i < ecc; i++)
	{
		int ndx = events[i].data.u32;
		selarray[ndx].wasError = ((events[i].events 
			& (EPOLLERR)) != 0);
		if(!selarray[ndx].wasError)
		{
			selarray[ndx].rAvailable = ((events[i].events 
				& (EPOLLIN | EPOLLPRI | EPOLLHUP)) != 0);
			selarray[ndx].wAvailable = ((events[i].events & EPOLLOUT) != 0);
		}
	}

	return ecc;
}

#elif defined (OW_HAVE_SYS_POLL_H)
int
select(const SelectTypeArray& selarray, UInt32 ms)
{
	int rc = 0;
	pollfd pfds[selarray.size()]; 
	// here we spin checking for thread cancellation every so often.
	timeval now, end;
	const Int32 loopMicroSeconds = 100 * 1000; // 1/10 of a second
	gettimeofday(&now, NULL);
	end = now;
	end.tv_sec  += ms / 1000;
	end.tv_usec += (ms % 1000) * 1000;
	while ((rc == 0) && ((ms == INFINITE_TIMEOUT) || (now.tv_sec < end.tv_sec)
		 || ((now.tv_sec == end.tv_sec) && (now.tv_usec < end.tv_usec))))
	{
		for (size_t i = 0; i < selarray.size(); i++)
		{
			OW_ASSERT(selarray[i] >= 0);
			pfds[i].fd = selarray[i]; 
			pfds[i].events = POLLIN | POLLPRI; 
		}

		timeval tv;
		tv.tv_sec = end.tv_sec - now.tv_sec;
		if (end.tv_usec >= now.tv_usec)
		{
			tv.tv_usec = end.tv_usec - now.tv_usec;
		}
		else
		{
			tv.tv_sec--;
			tv.tv_usec = 1000000 + end.tv_usec - now.tv_usec;
		}

		if ((tv.tv_sec != 0) || (tv.tv_usec > loopMicroSeconds) || (ms == INFINITE_TIMEOUT))
		{
			tv.tv_sec = 0;
			tv.tv_usec = loopMicroSeconds;
		}

		// TODO optimize this. 
		int loopMSecs = tv.tv_sec * 1000 + tv.tv_usec / 1000; 

		Thread::testCancel();
		rc = ::poll(pfds, selarray.size(), loopMSecs); 
		Thread::testCancel();

		gettimeofday(&now, NULL);
	}
	
	if (rc < 0)
	{
		if (errno == EINTR)
		{
#ifdef OW_NETWARE
			// When the NetWare server is shutting down, select will
			// set errno to EINTR on return. If this thread does not
			// yield control (cooperative multitasking) then we end
			// up in a very tight loop and get a CPUHog server abbend.
			pthread_yield();
#endif
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
		if (pfds[i].revents & (POLLIN | POLLPRI | POLLHUP))
		{
			return static_cast<int>(i);
		}
	}
	OW_THROW(AssertionException, "Logic error in Select. Didn't find file handle");
	return Select::SELECT_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// poll() version
int
selectRW(SelectObjectArray& selarray, UInt32 ms)
{
	int rc = 0;

	AutoPtrVec<pollfd> pfds(new pollfd[selarray.size()]);

	// here we spin checking for thread cancellation every so often.
	timeval now, end;
	const Int32 loopMicroSeconds = 100 * 1000; // 1/10 of a second
	gettimeofday(&now, NULL);
	end = now;
	end.tv_sec  += ms / 1000;
	end.tv_usec += (ms % 1000) * 1000;
	while ((rc == 0) && ((ms == INFINITE_TIMEOUT) || (now.tv_sec < end.tv_sec)
		 || ((now.tv_sec == end.tv_sec) && (now.tv_usec < end.tv_usec))))
	{
		for (size_t i = 0; i < selarray.size(); i++)
		{
			OW_ASSERT(selarray[i].s >= 0);
			selarray[i].rAvailable = false;
			selarray[i].wAvailable = false;
			selarray[i].wasError = false;
			pfds[i].revents = 0;
			pfds[i].fd = selarray[i].s;
			pfds[i].events = selarray[i].rEvents ? (POLLIN | POLLPRI) : 0;
			if(selarray[i].wEvents)
				pfds[i].events |= POLLOUT;
		}

		timeval tv;
		tv.tv_sec = end.tv_sec - now.tv_sec;
		if (end.tv_usec >= now.tv_usec)
		{
			tv.tv_usec = end.tv_usec - now.tv_usec;
		}
		else
		{
			tv.tv_sec--;
			tv.tv_usec = 1000000 + end.tv_usec - now.tv_usec;
		}

		if ((tv.tv_sec != 0) || (tv.tv_usec > loopMicroSeconds) || (ms == INFINITE_TIMEOUT))
		{
			tv.tv_sec = 0;
			tv.tv_usec = loopMicroSeconds;
		}

		// TODO optimize this. 
		int loopMSecs = tv.tv_sec * 1000 + tv.tv_usec / 1000; 

		Thread::testCancel();
		rc = ::poll(pfds.get(), selarray.size(), loopMSecs); 
		Thread::testCancel();

		gettimeofday(&now, NULL);
	}
	
	if (rc < 0)
	{
		if (errno == EINTR)
		{
#ifdef OW_NETWARE
			// When the NetWare server is shutting down, select will
			// set errno to EINTR on return. If this thread does not
			// yield control (cooperative multitasking) then we end
			// up in a very tight loop and get a CPUHog server abbend.
			pthread_yield();
#endif
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
		if(pfds[i].events & (POLLERR | POLLNVAL))
		{
			selarray[i].wasError = true;
		}
		else
		{
			if(selarray[i].rEvents)
			{
				selarray[i].rAvailable = (pfds[i].revents & 
					(POLLIN | POLLPRI | POLLHUP));
			}

			if(selarray[i].wEvents)
			{
				selarray[i].wAvailable = (pfds[i].revents &
					(POLLOUT | POLLHUP));
			}
		}
	}

	return rc;
}
#elif defined (OW_HAVE_SYS_SELECT_H)
int
select(const SelectTypeArray& selarray, UInt32 ms)
{
	int rc = 0;
	fd_set rfds;
	// here we spin checking for thread cancellation every so often.
	timeval now, end;
	const Int32 loopMicroSeconds = 100 * 1000; // 1/10 of a second
	gettimeofday(&now, NULL);
	end = now;
	end.tv_sec  += ms / 1000;
	end.tv_usec += (ms % 1000) * 1000;
	while ((rc == 0) && ((ms == INFINITE_TIMEOUT) || (now.tv_sec < end.tv_sec)
		 || ((now.tv_sec == end.tv_sec) && (now.tv_usec < end.tv_usec))))
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
			if (selarray[i] < 0 || selarray[i] >= FD_SETSIZE)
			{
				return Select::SELECT_ERROR;
			}
			FD_SET(selarray[i], &rfds);
		}

		timeval tv;
		tv.tv_sec = end.tv_sec - now.tv_sec;
		if (end.tv_usec >= now.tv_usec)
		{
			tv.tv_usec = end.tv_usec - now.tv_usec;
		}
		else
		{
			tv.tv_sec--;
			tv.tv_usec = 1000000 + end.tv_usec - now.tv_usec;
		}

		if ((tv.tv_sec != 0) || (tv.tv_usec > loopMicroSeconds) || (ms == INFINITE_TIMEOUT))
		{
			tv.tv_sec = 0;
			tv.tv_usec = loopMicroSeconds;
		}

		Thread::testCancel();
		rc = ::select(maxfd+1, &rfds, NULL, NULL, &tv);
		Thread::testCancel();

		gettimeofday(&now, NULL);
	}
	
	if (rc < 0)
	{
		if (errno == EINTR)
		{
#ifdef OW_NETWARE
			// When the NetWare server is shutting down, select will
			// set errno to EINTR on return. If this thread does not
			// yield control (cooperative multitasking) then we end
			// up in a very tight loop and get a CPUHog server abbend.
			pthread_yield();
#endif
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

//////////////////////////////////////////////////////////////////////////////
// ::select() version
int
selectRW(SelectObjectArray& selarray, UInt32 ms)
{
	int rc = 0;
	fd_set ifds;
	fd_set ofds;

	// here we spin checking for thread cancellation every so often.
	timeval now, end;
	const Int32 loopMicroSeconds = 100 * 1000; // 1/10 of a second
	gettimeofday(&now, NULL);
	end = now;
	end.tv_sec  += ms / 1000;
	end.tv_usec += (ms % 1000) * 1000;
	while ((rc == 0) && ((ms == INFINITE_TIMEOUT) || (now.tv_sec < end.tv_sec)
		 || ((now.tv_sec == end.tv_sec) && (now.tv_usec < end.tv_usec))))
	{
		int maxfd = 0;
		FD_ZERO(&ifds);
		FD_ZERO(&ofds);
		for (size_t i = 0; i < selarray.size(); ++i)
		{
			int fd = selarray[i].s;
			OW_ASSERT(fd >= 0);
			if (maxfd < fd)
			{
				maxfd = fd;
			}
			if (fd < 0 || fd >= FD_SETSIZE)
			{
				return Select::SELECT_ERROR;
			}
			if (selarray[i].rEvents)
			{
				FD_SET(fd, &ifds);
			}
			if (selarray[i].wEvents)
			{
				FD_SET(fd, &ofds);
			}
		}

		timeval tv;
		tv.tv_sec = end.tv_sec - now.tv_sec;
		if (end.tv_usec >= now.tv_usec)
		{
			tv.tv_usec = end.tv_usec - now.tv_usec;
		}
		else
		{
			tv.tv_sec--;
			tv.tv_usec = 1000000 + end.tv_usec - now.tv_usec;
		}

		if ((tv.tv_sec != 0) || (tv.tv_usec > loopMicroSeconds) || (ms == INFINITE_TIMEOUT))
		{
			tv.tv_sec = 0;
			tv.tv_usec = loopMicroSeconds;
		}

		Thread::testCancel();
		rc = ::select(maxfd+1, &ifds, &ofds, NULL, &tv);
		Thread::testCancel();

		gettimeofday(&now, NULL);
	}
	
	if (rc < 0)
	{
		if (errno == EINTR)
		{
#ifdef OW_NETWARE
			// When the NetWare server is shutting down, select will
			// set errno to EINTR on return. If this thread does not
			// yield control (cooperative multitasking) then we end
			// up in a very tight loop and get a CPUHog server abbend.
			pthread_yield();
#endif
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
	int availableCount = 0;
	int cval;
	for (size_t i = 0; i < selarray.size(); i++)
	{
		selarray[i].wasError = false;
		cval = 0;
		if (FD_ISSET(selarray[i].s, &ifds))
		{
			selarray[i].rAvailable = true;
			cval = 1;
		}
		else
		{
			selarray[i].rAvailable = false;
		}

		if (FD_ISSET(selarray[i].s, &ofds))
		{
			selarray[i].wAvailable = true;
			cval = 1;
		}
		else
		{
			selarray[i].wAvailable = false;
		}

		availableCount += cval;

	}
		
	return availableCount;
}

#else
  #error "port me!"
#endif // # epoll/poll/select
//////////////////////////////////////////////////////////////////////////////
#endif	// #else OW_WIN32

} // end namespace Select

} // end namespace OW_NAMESPACE

