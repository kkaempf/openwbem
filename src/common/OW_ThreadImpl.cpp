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
#include "OW_ThreadImpl.hpp"
#include "OW_Mutex.hpp"
#include "OW_Assertion.hpp"
#include <cassert>

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

#include <cstring>

extern "C"
{
#ifdef OW_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/types.h>
#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <signal.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}

#ifdef OW_USE_PTHREAD
struct LocalThreadParm
{
	OW_ThreadFunction m_func;
	void* m_funcParm;
};

extern "C" {
static void* threadStarter(void* arg);
}

#endif

#ifdef OW_USE_GNU_PTH
static pth_attr_t g_joinable_attr;
static pth_attr_t g_nonjoinable_attr;
bool g_initialized = false;

//////////////////////////////////////////////////////////////////////////////
void
OW_ThreadImpl::initThreads()
{
    if(!g_initialized)
    {
        g_initialized = true;
        pth_init();

        g_joinable_attr = pth_attr_new();
        pth_attr_set(g_joinable_attr, PTH_ATTR_JOINABLE, true);
        g_nonjoinable_attr = pth_attr_new();
        pth_attr_set(g_nonjoinable_attr, PTH_ATTR_JOINABLE, false);
    }
}
#endif

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadImpl::sleep(OW_UInt32 milliSeconds)
{
#ifdef OW_USE_GNU_PTH
    initThreads();
    pth_usleep(milliSeconds * 1000);
#elif defined(OW_HAVE_NANOSLEEP)
	struct timespec wait;
	wait.tv_sec = milliSeconds / 1000;
	wait.tv_nsec = (milliSeconds % 1000) * 1000000;
	while (nanosleep(&wait, &wait) == -1 && errno == EINTR);
#elif OW_WIN32
	Sleep(milliSeconds);
#else
	timeval now, end;
	unsigned long microSeconds = milliSeconds * 1000;

	gettimeofday(&now, NULL);
	end = now;
	end.tv_sec  += microSeconds / 1000000;
	end.tv_usec += microSeconds % 1000000;

	while ((now.tv_sec < end.tv_sec)
			 || ((now.tv_sec == end.tv_sec) && (now.tv_usec < end.tv_usec)))
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

		select(0, NULL, NULL, NULL, &tv);
		gettimeofday(&now, NULL);
	}
#endif
}

#ifdef OW_USE_PTHREAD
namespace {

// The purpose of this class is to retrieve the default stack size only once at library load time and re-use it thereafter.
struct default_stack_size
{
	default_stack_size()
	{
		// if anything in this function fails, we'll just leave val == 0.
		val = 0;
		pthread_attr_t stack_size_attr;
		if (pthread_attr_init(&stack_size_attr) != 0)
			return;
		if (pthread_attr_getstacksize(&stack_size_attr, &val) != 0)
			return;
	}
	static size_t val;
};

size_t default_stack_size::val = 0;

default_stack_size g_theDefaultStackSize;
} // end unnamed namespace
#endif

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_ThreadImpl::createThread(OW_Thread_t& handle, OW_ThreadFunction func,
									 void* funcParm, OW_UInt32 threadFlags)
{
#ifdef OW_USE_GNU_PTH
    initThreads();
    int cc = 0;
    if((threadFlags & OW_THREAD_FLG_JOINABLE))
    {
        handle = pth_spawn(g_joinable_attr, func, funcParm);
    }
    else
    {
        handle = pth_spawn(g_nonjoinable_attr, func, funcParm);
    }
    if(!handle)
    {
        cc = -1;
    }
    return cc;
#elif OW_WIN32
	return 0;
#elif OW_USE_PTHREAD
	int cc = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if (!(threadFlags & OW_THREAD_FLG_JOINABLE))
	{
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	}

	// make sure we have a big enough stack.  OpenWBEM can use quite a bit, so we'll try to make sure we get at least 1 MB.
	// 1 MB is just an arbitrary number.  The default on Linux is 2 MB which has never been a problem.  However, on UnixWare
	// the default is really low (16K IIRC) and that isn't enough. It would be good to do some sort of measurement...
#ifdef _POSIX_THREAD_ATTR_STACKSIZE

// This bunch of yuckiness is to make sure we get the larger of 1 MB or PTHREAD_STACK_MIN
#define OW_MIN_REQ_STACK_SIZE 1048576 // 1 MB
#ifdef PTHREAD_STACK_MIN
	#if PTHREAD_STACK_MIN > OW_MIN_REQ_STACK_SIZE
		#define MIN_REQ_STACK_SIZE PTHREAD_STACK_MIN
	#else
		#define MIN_REQ_STACK_SIZE OW_MIN_REQ_STACK_SIZE
	#endif
#else
	#define MIN_REQ_STACK_SIZE OW_MIN_REQ_STACK_SIZE
#endif

	if (default_stack_size::val < MIN_REQ_STACK_SIZE)
	{
		pthread_attr_setstacksize(&attr, MIN_REQ_STACK_SIZE);
	}
#undef MIN_REQ_STACK_SIZE
#undef OW_MIN_REQ_STACK_SIZE

#endif

	LocalThreadParm* parg = new LocalThreadParm;
	parg->m_func = func;
	parg->m_funcParm = funcParm;

	if (pthread_create(&handle, &attr, threadStarter, parg) != 0)
	{
		cc = -1;
	}

	pthread_attr_destroy(&attr);
	return cc;
#elif
#error "port me!"
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadImpl::destroyThread(OW_Thread_t& )
{
}

//////////////////////////////////////////////////////////////////////////////
#ifdef OW_USE_PTHREAD
extern "C"
{
static void*
threadStarter(void* arg)
{
	sigset_t sigset;
	sigprocmask(SIG_BLOCK, NULL, &sigset);
	sigaddset(&sigset, SIGINT);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	LocalThreadParm* parg = (LocalThreadParm*)arg;
	OW_ThreadFunction func = parg->m_func;
	void* funcParm = parg->m_funcParm;
	delete parg;

	(*func)(funcParm);
	pthread_exit(NULL);
	return NULL;
}

} // extern "C"
#endif

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadImpl::exitThread(OW_Thread_t&)
{
#ifdef OW_USE_GNU_PTH
    pth_exit(NULL);
#elif defined(OW_USE_PTHREAD)
	pthread_exit(NULL);
#elif defined(OW_USE_WIN32_THREADS)
#else
#error "port me!"
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_ThreadImpl::setThreadDetached(OW_Thread_t& handle)
{
#ifdef OW_USE_GNU_PTH
    pth_t tid = handle;
    if(!handle)
    {
         OW_THROW(OW_Exception, "OW_ThreadImpl::setThreadDetached was passed an "
                 "invalid thread handle");
    }
    pth_attr_t attr = pth_attr_of(tid);
    if(!attr)
    {
        OW_THROW(OW_Exception, "OW_ThreadImpl::setThreadDetached failed getting "
                "thread attribute");
    }
    int cc = pth_attr_set(attr, PTH_ATTR_JOINABLE, false);
    pth_attr_destroy(attr);
    return cc;
#elif defined(OW_USE_PTHREAD)
	int cc = pthread_detach(handle);

	if (cc != 0)
	{
		if (cc != EINVAL)
		{
			cc = -1;
		}
	}

	return cc;
#elif defined (OW_USE_WIN32_THREADS)
	return 0;
#else
#error "port me!"
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_ThreadImpl::joinThread(OW_Thread_t& handle)
{
#ifdef OW_USE_GNU_PTH
    return ((pth_join(handle, NULL)) != -1 ? 0 : -1);
#elif defined(OW_USE_PTHREAD)
	return(((errno = pthread_join(handle, NULL)) == 0) ? 0 : -1);
#elif defined(OW_USE_WIN32_THREADS)
	return 0;
#else
#error "port me!"
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadImpl::yield()
{
#ifdef OW_USE_GNU_PTH
    pth_yield(NULL);
#elif defined(OW_HAVE_SCHED_YIELD)
    sched_yield();
#elif defined(OW_WIN32)
	Sleep(0);
#else
	OW_ThreadImpl::sleep(1);
#endif
}



