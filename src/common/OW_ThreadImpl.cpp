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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ThreadImpl.hpp"
#include "OW_Mutex.hpp"
#include "OW_Assertion.hpp"
#include "OW_Thread.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Format.hpp"
#if defined(OW_WIN32)
#include "OW_Map.hpp"
#include "OW_MutexLock.hpp"
#endif
#include <cassert>
#include <cstring>
#include <cstddef>

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

#ifdef OW_USE_PTHREAD
#include <pthread.h>
#endif

#ifdef OW_WIN32
#include <process.h>
#endif
}

namespace OpenWBEM
{

namespace ThreadImpl {

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
sleep(UInt32 milliSeconds)
{
	ThreadImpl::testCancel();
#if defined(OW_HAVE_NANOSLEEP)
	struct timespec wait;
	wait.tv_sec = milliSeconds / 1000;
	wait.tv_nsec = (milliSeconds % 1000) * 1000000;
	while (nanosleep(&wait, &wait) == -1 && errno == EINTR)
	{
		ThreadImpl::testCancel();
	}
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
		ThreadImpl::testCancel();
		select(0, NULL, NULL, NULL, &tv);
		gettimeofday(&now, NULL);
	}
#endif
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
yield()
{
#if defined(OW_HAVE_SCHED_YIELD)
	sched_yield();
#elif defined(OW_WIN32)
	ThreadImpl::testCancel();
	::SwitchToThread();
#else
	ThreadImpl::sleep(1);
#endif
}

#if defined(OW_USE_PTHREAD)
namespace {
struct LocalThreadParm
{
	ThreadFunction m_func;
	void* m_funcParm;
};
extern "C" {
static void*
threadStarter(void* arg)
{
	// set our cancellation state to asynchronous, so we can actually be
	// killed if need be.
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	LocalThreadParm* parg = (LocalThreadParm*)arg;
	ThreadFunction func = parg->m_func;
	void* funcParm = parg->m_funcParm;
	delete parg;
	Int32 rval = (*func)(funcParm);
	void* prval = reinterpret_cast<void*>(static_cast<ptrdiff_t>(rval));
	pthread_exit(prval);
	return prval;
}
}
// The purpose of this class is to retrieve the default stack size only once
// at library load time and re-use it thereafter.
struct default_stack_size
{
	default_stack_size()
	{
		// if anything in this function fails, we'll just leave val == 0.
		val = 0;
		needsSetting = false;

// make sure we have a big enough stack.  OpenWBEM can use quite a bit, so we'll try to make sure we get at least 1 MB.
// 1 MB is just an arbitrary number.  The default on Linux is 2 MB which has never been a problem.  However, on UnixWare
// the default is really low (16K IIRC) and that isn't enough. It would be good to do some sort of measurement...
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
		pthread_attr_t stack_size_attr;
		if (pthread_attr_init(&stack_size_attr) != 0)
		{
			return;
		}
		if (pthread_attr_getstacksize(&stack_size_attr, &val) != 0)
		{
			return;
		}

		if (val < 1048576) {
			val = 1048576; // 1 MB
			needsSetting = true;
		}
#ifdef PTHREAD_STACK_MIN
		if (PTHREAD_STACK_MIN > val) {
			val = PTHREAD_STACK_MIN;
			needsSetting = true;
		}
#endif
#endif
	}
	static size_t val;
	static bool needsSetting;
};
size_t default_stack_size::val = 0;
bool default_stack_size::needsSetting(false);
default_stack_size g_theDefaultStackSize;
//////////////////////////////////////////////////////////////////////
pthread_once_t once_control = PTHREAD_ONCE_INIT;
pthread_key_t theKey;
extern "C" {
//////////////////////////////////////////////////////////////////////
static void initializeTheKey()
{
	pthread_key_create(&theKey,NULL);
	// set SIGUSR1 to SIG_IGN so we can safely send it to threads when we want to cancel them.
	struct sigaction temp;
	memset(&temp, '\0', sizeof(temp));
	sigaction(SIGUSR1, 0, &temp);
	if (temp.sa_handler != SIG_IGN)
	{
		temp.sa_handler = SIG_IGN;
		sigemptyset(&temp.sa_mask);
		temp.sa_flags = 0;
		sigaction(SIGUSR1, &temp, NULL);
	}
}
} // end extern "C"
} // end unnamed namespace
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
createThread(Thread_t& handle, ThreadFunction func,
	void* funcParm, UInt32 threadFlags)
{
	// set up our TLS which will be used to store the Thread* in.
	pthread_once(&once_control, &initializeTheKey);
	
	int cc = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if (!(threadFlags & OW_THREAD_FLG_JOINABLE))
	{
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	}

#if !defined(OW_VALGRIND_SUPPORT) // valgrind doesn't like us to set the stack size
	// Won't be set to true unless _POSIX_THREAD_ATTR_STACKSIZE is defined
	if (default_stack_size::needsSetting)
	{
		pthread_attr_setstacksize(&attr, default_stack_size::val);
	}
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
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
exitThread(Thread_t&, Int32 rval)
{
	void* prval = reinterpret_cast<void*>(static_cast<ptrdiff_t>(rval));
	pthread_exit(prval);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
destroyThread(Thread_t& )
{
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
setThreadDetached(Thread_t& handle)
{
	int cc = pthread_detach(handle);
	if (cc != 0)
	{
		if (cc != EINVAL)
		{
			cc = -1;
		}
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
joinThread(Thread_t& handle, Int32& rval)
{
	void* prval;
	if ((errno = pthread_join(handle, &prval)) == 0)
	{
		rval = static_cast<Int32>(reinterpret_cast<ptrdiff_t>(prval));
		return 0;
	}
	else
	{
		return 1;
	}
}
//////////////////////////////////////////////////////////////////////
void
testCancel()
{
	pthread_once(&once_control, &initializeTheKey);
	Thread* theThread = reinterpret_cast<Thread*>(pthread_getspecific(theKey));
	if (theThread == 0)
	{
		return;
	}
	NonRecursiveMutexLock l(theThread->m_cancelLock);
	if (theThread->m_cancelRequested)
	{
		// We don't use OW_THROW here because 
		// ThreadCancelledException is special.  It's not derived
		// from Exception on purpose so it can be propagated up
		// the stack easier. This exception shouldn't be caught and not
		// re-thrown anywhere except in Thread::threadRunner()
		throw ThreadCancelledException();
	}
}
//////////////////////////////////////////////////////////////////////
void saveThreadInTLS(void* pTheThread)
{
	pthread_once(&once_control, &initializeTheKey);
	int rc;
	if ((rc = pthread_setspecific(theKey, pTheThread)) != 0)
	{
		OW_THROW(ThreadException, Format("pthread_setspecific failed.  error = %1(%2)", rc, strerror(rc)).c_str());
	}
}
//////////////////////////////////////////////////////////////////////
void sendSignalToThread(Thread_t threadID, int signo)
{
	int rc;
	if ((rc = pthread_kill(threadID, signo)) != 0)
	{
		OW_THROW(ThreadException, Format("pthread_kill failed.  error = %1(%2)", rc, strerror(rc)).c_str());
	}
}
//////////////////////////////////////////////////////////////////////
void cancel(Thread_t threadID)
{
	int rc;
	if ((rc = pthread_cancel(threadID)) != 0)
	{
		OW_THROW(ThreadException, Format("pthread_cancel failed.  error = %1(%2)", rc, strerror(rc)).c_str());
	}
}
#endif // #ifdef OW_USE_PTHREAD

#if defined(OW_WIN32)

namespace {

struct WThreadInfo
{
	HANDLE	handle;
	OpenWBEM::Thread* pTheThread;
};

typedef Map<DWORD, WThreadInfo> Win32ThreadMap;
Win32ThreadMap g_threads;
Mutex g_threadsGuard;

struct LocalThreadParm
{
	ThreadFunction m_func;
	void* m_funcParm;
};

//////////////////////////////////////////////////////////////////////////////
extern "C" {
unsigned __stdcall threadStarter(void* arg)
{
	LocalThreadParm* parg = reinterpret_cast<LocalThreadParm*>(arg);
	ThreadFunction func = parg->m_func;
	void* funcParm = parg->m_funcParm;
	delete parg;
	Int32 rval = (*func)(funcParm);
	::_endthreadex(static_cast<unsigned>(rval));
	return rval;
}
}	// End extern "C"

//////////////////////////////////////////////////////////////////////////////
void
addThreadToMap(DWORD threadId, HANDLE threadHandle)
{
	MutexLock ml(g_threadsGuard);
	WThreadInfo wi;
	wi.handle = threadHandle;
	wi.pTheThread = 0;
	g_threads[threadId] = wi;
}

//////////////////////////////////////////////////////////////////////////////
HANDLE
getThreadHandle(DWORD threadId)
{
	MutexLock ml(g_threadsGuard);
	HANDLE chdl = 0;
	Win32ThreadMap::iterator it = g_threads.find(threadId);
	if (it != g_threads.end())
	{
		chdl = it->second.handle;
	}
	return chdl;
}

//////////////////////////////////////////////////////////////////////////////
void
setThreadPointer(DWORD threadId, Thread* pTheThread)
{
	MutexLock ml(g_threadsGuard);
	Win32ThreadMap::iterator it = g_threads.find(threadId);
	if (it != g_threads.end())
	{
		it->second.pTheThread = pTheThread;
	}
}

//////////////////////////////////////////////////////////////////////////////
HANDLE
removeThreadFromMap(DWORD threadId)
{
	MutexLock ml(g_threadsGuard);
	HANDLE chdl = 0;
	Win32ThreadMap::iterator it = g_threads.find(threadId);
	if (it != g_threads.end())
	{
		chdl = it->second.handle;
		g_threads.erase(it);
	}
	return chdl;
}

//////////////////////////////////////////////////////////////////////////////
Thread*
getThreadObject(DWORD threadId)
{
	Thread* pTheThread = 0;
	MutexLock ml(g_threadsGuard);
	Win32ThreadMap::iterator it = g_threads.find(threadId);
	if (it != g_threads.end())
	{
		pTheThread = it->second.pTheThread;
	}
	return pTheThread;
}

}	// End unnamed namespace

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
createThread(Thread_t& handle, ThreadFunction func,
	void* funcParm, UInt32 threadFlags)
{
	int cc = -1;
	HANDLE hThread;
	unsigned threadId;

	LocalThreadParm* parg = new LocalThreadParm;
	parg->m_func = func;
	parg->m_funcParm = funcParm;
	hThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0, threadStarter,
		parg, 0, &threadId));
	if (hThread != 0)
	{
		addThreadToMap(threadId, hThread);
	}

	return cc;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
exitThread(Thread_t&, Int32 rval)
{
	::_endthreadex(static_cast<unsigned>(rval));
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
destroyThread(Thread_t& threadId)
{
	HANDLE thdl = removeThreadFromMap(threadId);
	if (thdl != 0)
	{
		::CloseHandle(thdl);
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
setThreadDetached(Thread_t& handle)
{
	// No need for this on Win32
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
joinThread(Thread_t& threadId, Int32& rvalArg)
{
	int cc = -1;
	DWORD rval;
	HANDLE thdl = getThreadHandle(threadId);
	if (thdl != 0)
	{
		if (::WaitForSingleObject(thdl, INFINITE) != WAIT_FAILED)
		{
			if (::GetExitCodeThread(thdl, &rval) != 0)
			{
				rvalArg = static_cast<Int32>(rval);
				cc = 0;
			}
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
void
testCancel()
{
	DWORD threadId = ThreadImpl::currentThread();
	Thread* pTheThread = getThreadObject(threadId);
	if (pTheThread)
	{
		NonRecursiveMutexLock l(pTheThread->m_cancelLock);
		if (pTheThread->m_cancelRequested)
		{
			// We don't use OW_THROW here because 
			// ThreadCancelledException is special.  It's not derived
			// from Exception on purpose so it can be propagated up
			// the stack easier. This exception shouldn't be caught and not
			// re-thrown anywhere except in Thread::threadRunner()
			throw ThreadCancelledException();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void saveThreadInTLS(void* pThreadArg)
{
	Thread* pThread = static_cast<Thread*>(pThreadArg);
	DWORD threadId = pThread->getId();
    setThreadPointer(threadId, pThread);
}
//////////////////////////////////////////////////////////////////////
void sendSignalToThread(Thread_t threadID, int signo)
{
}
//////////////////////////////////////////////////////////////////////
void cancel(Thread_t threadId)
{
	HANDLE thdl = getThreadHandle(threadId);
	if (thdl != 0)
	{
		::TerminateThread(thdl, -1);
	}
}

#endif // #ifdef OW_WIN32
} // end namespace OW_ThreadImpl

} // end namespace OpenWBEM

