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
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}

#ifndef OW_USE_GNU_PTH
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
#else
	usleep(milliSeconds * 1000);
	/*
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
	*/
#endif
}

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
#else
	int cc = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if (!(threadFlags & OW_THREAD_FLG_JOINABLE))
	{
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	}

	LocalThreadParm* parg = new LocalThreadParm;
	parg->m_func = func;
	parg->m_funcParm = funcParm;

	if (pthread_create(&handle, &attr, threadStarter, parg) != 0)
	{
		cc = -1;
	}

	pthread_attr_destroy(&attr);
	return cc;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadImpl::destroyThread(OW_Thread_t& )
{
}

//////////////////////////////////////////////////////////////////////////////
#ifndef OW_USE_GNU_PTH
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
#else
	pthread_exit(NULL);
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
#else
	int cc = pthread_detach(handle);

	if (cc != 0)
	{
		if (cc != EINVAL)
		{
			cc = -1;
		}
	}

	return cc;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_ThreadImpl::joinThread(OW_Thread_t& handle)
{
#ifdef OW_USE_GNU_PTH
    return ((pth_join(handle, NULL)) != -1 ? 0 : -1);
#else
	return(((errno = pthread_join(handle, NULL)) == 0) ? 0 : -1);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadImpl::yield()
{
#ifdef OW_USE_GNU_PTH
    pth_yield(NULL);
#else
#ifdef OW_HAVE_SCHED_YIELD
    sched_yield();
#else
	OW_ThreadImpl::sleep(1);
#endif
#endif
}



