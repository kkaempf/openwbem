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

	if (pthread_create(&handle, &attr, threadStarter, (void*)parg) != 0)
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
	return((pthread_join(handle, NULL) == 0) ? 0 : -1);
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

//////////////////////////////////////////////////////////////////////////////
// EVENT IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////
struct OW_PlatformThreadEvent
{
	unsigned int sig;
#ifdef OW_USE_GNU_PTH
    pth_mutex_t mutex;
    pth_cond_t cond;
#else
	pthread_mutex_t mutex;
	pthread_cond_t cond;
#endif
	OW_Bool wasPulsed;
	OW_Bool signaled;
};
#define EVENTSIG 0x0aabbccd

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_ThreadEventImpl::createThreadEvent(OW_ThreadEvent_t& handle)
{
#ifdef OW_USE_GNU_PTH
    OW_ThreadImpl::initThreads();
#endif

	OW_PlatformThreadEvent* pev = new OW_PlatformThreadEvent;
	OW_ASSERT(pev != NULL);

	::memset(pev, 0, sizeof(OW_PlatformThreadEvent));
#ifdef OW_USE_GNU_PTH
    if(!pth_mutex_init(&pev->mutex))
#else
	if (pthread_mutex_init(&pev->mutex, NULL) != 0)
#endif
	{
		//cerr << "OW_ThreadEventImpl::createThreadEvent - mutex init failed"
		//	<< endl;

		delete pev;
		return -1;
	}

#ifdef OW_USE_GNU_PTH
    if(!pth_cond_init(&pev->cond))
#else
	if (pthread_cond_init(&pev->cond, NULL) != 0)
#endif
	{
		//cerr << "OW_ThreadEventImpl::createThreadEvent - cond init failed"
		//	<< endl;

#ifndef OW_USE_GNU_PTH
		pthread_mutex_destroy(&pev->mutex);
#endif
		delete pev;
		return -1;
	}
	pev->sig = EVENTSIG;
	handle = (OW_ThreadEvent_t)pev;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_ThreadEventImpl::destroyThreadEvent(OW_ThreadEvent_t& handle)
{
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;

	assert(pev != NULL);
	assert(pev->sig == EVENTSIG);

#ifndef OW_USE_GNU_PTH
	pthread_cond_destroy(&pev->cond);
	pthread_mutex_destroy(&pev->mutex);
#endif
	pev->sig = 0;
	delete pev;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_ThreadEventImpl::waitForThreadEventSignal(OW_ThreadEvent_t& handle,
															OW_UInt32 ms)
{
#ifdef OW_USE_GNU_PTH
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

    pth_mutex_acquire(&pev->mutex, false, NULL);
    if(!pev->signaled)
	{
		if (0 == ms)
		{
            pth_cond_await(&pev->cond, &pev->mutex, NULL);
		}
		else
		{
            long sec = ms / 1000L;
            long usec = (ms % 1000) * 1000000;
            pth_event_t ev = pth_event(PTH_EVENT_TIME, pth_timeout(sec, usec));
            pth_cond_await(&pev->cond, &pev->mutex, ev);
            pth_event_free(ev, PTH_FREE_ALL);
		}
	}

	OW_Bool rval = (pev->wasPulsed) ? OW_Bool(true) : pev->signaled;
	pev->wasPulsed = false;
    pth_mutex_release(&pev->mutex);
	return rval;
#else
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

	int cc = pthread_mutex_lock(&pev->mutex);
	OW_ASSERT(cc == 0);

	if (!pev->signaled)
	{
		if (0 == ms)
		{
			cc = pthread_cond_wait(&pev->cond, &pev->mutex);
			OW_ASSERT(cc == 0);
		}
		else
		{
			timespec ts;
			ts.tv_sec = time(NULL) + (ms / 1000L);
			ts.tv_nsec = (ms % 1000) * 1000000;
			cc = pthread_cond_timedwait(&pev->cond, &pev->mutex, &ts);
			OW_ASSERT(cc == 0 || cc == ETIMEDOUT);
		}
	}

	OW_Bool rval = (pev->wasPulsed) ? OW_Bool(true) : pev->signaled;
	pev->wasPulsed = false;
	cc = pthread_mutex_unlock(&pev->mutex);
	OW_ASSERT(cc == 0);
	return rval;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadEventImpl::pulseThreadEvent(OW_ThreadEvent_t& handle)
{
#ifdef OW_USE_GNU_PTH
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

    pth_mutex_acquire(&pev->mutex, false, NULL);
	pev->wasPulsed = true;
    pth_mutex_release(&pev->mutex);
    pth_cond_notify(&pev->cond, false);
#else
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

	int cc = pthread_mutex_lock(&pev->mutex);
	OW_ASSERT(cc == 0);

	pev->wasPulsed = true;

	cc = pthread_mutex_unlock(&pev->mutex);
	OW_ASSERT(cc == 0);

	cc = pthread_cond_signal(&pev->cond);
	OW_ASSERT(cc == 0);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadEventImpl::signalThreadEvent(OW_ThreadEvent_t& handle)
{
#ifdef OW_USE_GNU_PTH
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

    pth_mutex_acquire(&pev->mutex, false, NULL);
	pev->wasPulsed = false;
	pev->signaled = true;
    pth_mutex_release(&pev->mutex);
    pth_cond_notify(&pev->cond, true);
#else
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

	int cc = pthread_mutex_lock(&pev->mutex);
	OW_ASSERT(cc == 0);

	pev->wasPulsed = false;
	pev->signaled = true;
	cc = pthread_mutex_unlock(&pev->mutex);
	OW_ASSERT(cc == 0);

	cc = pthread_cond_broadcast(&pev->cond);
	OW_ASSERT(cc == 0);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_ThreadEventImpl::resetThreadEvent(OW_ThreadEvent_t& handle)
{
#ifdef OW_USE_GNU_PTH
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

    pth_mutex_acquire(&pev->mutex, false, NULL);
	pev->wasPulsed = false;
	pev->signaled = false;
    pth_mutex_release(&pev->mutex);
#else
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

	int cc = pthread_mutex_lock(&pev->mutex);
	OW_ASSERT(cc == 0);

	pev->wasPulsed = false;
	pev->signaled = false;
	cc = pthread_mutex_unlock(&pev->mutex);
	OW_ASSERT(cc == 0);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_ThreadEventImpl::isThreadEventSignaled(OW_ThreadEvent_t& handle)
{
#ifdef OW_USE_GNU_PTH
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

    pth_mutex_acquire(&pev->mutex, false, NULL);
	OW_Bool rval = pev->signaled;
    pth_mutex_release(&pev->mutex);
	return rval;
#else
	OW_PlatformThreadEvent* pev = (OW_PlatformThreadEvent*) handle;
	OW_ASSERT(pev != NULL);
	OW_ASSERT(pev->sig == EVENTSIG);

	int cc = pthread_mutex_lock(&pev->mutex);
	OW_ASSERT(cc == 0);

	OW_Bool rval = pev->signaled;

	cc = pthread_mutex_unlock(&pev->mutex);
	OW_ASSERT(cc == 0);

	return rval;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_SemaphoreImpl::signal(OW_ConditionVar_t& cond, OW_Mutex_t& mutex,
								 OW_Int32& curCount)
{
#ifdef OW_USE_GNU_PTH
    pth_mutex_acquire(&mutex, false, NULL);
	++curCount;
	if (curCount > 0)
	{
        pth_cond_notify(&cond, true);
	}
    pth_mutex_release(&mutex);
#else
	int cc = pthread_mutex_lock(&mutex);
	OW_ASSERT(cc == 0);

	++curCount;
	if (curCount > 0)
	{
		cc = pthread_cond_broadcast(&cond);
		OW_ASSERT(cc == 0);
	}

	cc = pthread_mutex_unlock(&mutex);
	OW_ASSERT(cc == 0);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_SemaphoreImpl::wait(OW_ConditionVar_t& cond, OW_Mutex_t& mutex,
							  OW_Int32& curCount, OW_UInt32 ms)
{
#ifdef OW_USE_GNU_PTH
	int retcode = 0;

    pth_mutex_acquire(&mutex, false, NULL);
	while (curCount <= 0 && retcode != ETIMEDOUT)
	{
		if (ms != 0)
		{
            long sec = ms / 1000L;
            long usec = (ms % 1000) * 1000000;

            pth_event_t ev = pth_event(PTH_EVENT_TIME, pth_timeout(sec, usec));
            pth_cond_await(&cond, &mutex, ev);
            if(pth_event_occurred(ev))
            {
                retcode = ETIMEDOUT;
            }

            pth_event_free(ev, PTH_FREE_ALL);
		}
		else
		{
            pth_cond_await(&cond, &mutex, NULL);
		}
	}

	OW_Bool rval = true;
	if (retcode == ETIMEDOUT)
	{
		rval = false;
	}
	else
	{
		--curCount;
	}

    pth_mutex_release(&mutex);
	return rval;
#else
	int retcode;

	int cc = pthread_mutex_lock(&mutex);
	OW_ASSERT(cc == 0);

	timespec ts;
	if (ms != 0)
	{
		ts.tv_sec = time(NULL) + (ms / 1000L);
		ts.tv_nsec = (ms % 1000) * 1000000;
	}

	retcode = 0;
	while (curCount <= 0 && retcode != ETIMEDOUT)
	{
		if (ms != 0)
		{
			retcode = pthread_cond_timedwait(&cond, &mutex, &ts);
		}
		else
		{
			retcode = pthread_cond_wait(&cond, &mutex);
		}
	}

	OW_Bool rval = true;
	if (retcode == ETIMEDOUT)
	{
		rval = false;
	}
	else
	{
		--curCount;
	}

	cc = pthread_mutex_unlock(&mutex);
	OW_ASSERT(cc == 0);

	return rval;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_SemaphoreImpl::createConditionVar(OW_ConditionVar_t& cond)
{
#ifdef OW_USE_GNU_PTH
    OW_ThreadImpl::initThreads();
    if(!pth_cond_init(&cond))
    {
        OW_THROW(OW_Exception, "Failed to create condition variable");
    }
#else
	int cc = pthread_cond_init(&cond, NULL);
	(void)cc;
	OW_ASSERT(cc == 0);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Int32
OW_SemaphoreImpl::getCount(OW_Mutex_t& mutex, OW_Int32& count)
{
#ifdef OW_USE_GNU_PTH
	OW_Int32 tmp;

    pth_mutex_acquire(&mutex, false, NULL);
	tmp = count;
    pth_mutex_release(&mutex);
	return tmp;
#else
	OW_Int32 tmp;
	int cc = pthread_mutex_lock(&mutex);
	OW_ASSERT(cc == 0);

	tmp = count;

	cc = pthread_mutex_unlock(&mutex);
	OW_ASSERT(cc == 0);

	return tmp;
#endif
}
