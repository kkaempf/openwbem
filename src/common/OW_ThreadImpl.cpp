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
#include "OW_Thread.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Format.hpp"
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
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
#ifdef OW_USE_PTHREAD
#include <pthread.h>
#include <limits.h> // for PTHREAD_KEYS_MAX
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
		ThreadImpl::testCancel();
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
	Sleep(0);
#else
	ThreadImpl::sleep(1);
#endif
}
#ifdef OW_USE_PTHREAD
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
	// set our cancellation state to asynchronous, so we can actually be killed if need be.
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
//////////////////////////////////////////////////////////////////////
pthread_once_t once_control = PTHREAD_ONCE_INIT;
const pthread_key_t NOKEY = PTHREAD_KEYS_MAX+1;
pthread_key_t theKey=NOKEY;
extern "C" {
//////////////////////////////////////////////////////////////////////
static void initializeTheKey()
{
	pthread_key_create(&theKey,NULL);
	// set SIGUSR1 to SIG_IGN so we can safely send it to threads when we want to cancel them.
	struct sigaction temp;
	memset(&temp, '\0', sizeof(temp));
	sigaction(SIGUSR1, 0, &temp);
	if(temp.sa_handler != SIG_IGN)
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
	int rc;
	if ((rc = pthread_setspecific(theKey, pTheThread)) != 0)
	{
		OW_THROW(ThreadException, format("pthread_setspecific failed.  error = %1(%2)", rc, strerror(rc)).c_str());
	}
}
//////////////////////////////////////////////////////////////////////
void sendSignalToThread(Thread_t threadID, int signo)
{
	int rc;
	if ((rc = pthread_kill(threadID, signo)) != 0)
	{
		OW_THROW(ThreadException, format("pthread_kill failed.  error = %1(%2)", rc, strerror(rc)).c_str());
	}
}
//////////////////////////////////////////////////////////////////////
void cancel(Thread_t threadID)
{
	int rc;
	if ((rc = pthread_cancel(threadID)) != 0)
	{
		OW_THROW(ThreadException, format("pthread_cancel failed.  error = %1(%2)", rc, strerror(rc)).c_str());
	}
}
#endif // #ifdef OW_USE_PTHREAD
#ifdef OW_WIN32
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
createThread(Thread_t& handle, ThreadFunction func,
	void* funcParm, UInt32 threadFlags)
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
exitThread(Thread_t&, Int32 rval)
{
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
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
joinThread(Thread_t& handle, Int32& rval)
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
void
testCancel()
{
}
//////////////////////////////////////////////////////////////////////
void saveThreadInTLS(void* pTheThread)
{
}
//////////////////////////////////////////////////////////////////////
void sendSignalToThread(Thread_t threadID, int signo)
{
}
//////////////////////////////////////////////////////////////////////
void cancel(Thread_t threadID)
{
}
#endif // #ifdef OW_WIN32
} // end namespace OW_ThreadImpl

} // end namespace OpenWBEM

