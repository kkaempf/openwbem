/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
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
#include "OW_Thread.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_ThreadBarrier.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <iostream> // for cerr
#include <csignal>

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
OW_DEFINE_EXCEPTION(Thread);
OW_DEFINE_EXCEPTION(CancellationDenied);
//////////////////////////////////////////////////////////////////////
// this is what's really passed to threadRunner
struct ThreadParam
{
	ThreadParam(Thread* t, const Reference<ThreadDoneCallback>& c, const ThreadBarrier& b)
		: thread(t)
		, cb(c)
		, barrier(b)
	{}
	Thread* thread;
	Reference<ThreadDoneCallback> cb;
	ThreadBarrier barrier;
};
static Thread_t zeroThread();
static Thread_t NULLTHREAD = zeroThread();
//////////////////////////////////////////////////////////////////////
static inline bool
sameId(const Thread_t& t1, const Thread_t& t2)
{
	return ThreadImpl::sameThreads(t1, t2);
}
//////////////////////////////////////////////////////////////////////////////
// Constructor
Thread::Thread() 
	: m_id(NULLTHREAD)
	, m_isRunning(false)
	, m_isStarting(false)
	, m_joined(false)
	, m_cancelRequested(false)
	, m_cancelled(false)
{
}
//////////////////////////////////////////////////////////////////////////////
// Destructor
Thread::~Thread()
{
	try
	{
		if (!m_joined)
		{
			join();
		}
		OW_ASSERT(m_isRunning == false);
		if(!sameId(m_id, NULLTHREAD))
		{
			ThreadImpl::destroyThread(m_id);
		}
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
// Start the thread
void
Thread::start(Reference<ThreadDoneCallback> cb)
{
	if(isRunning())
	{
		OW_THROW(ThreadException,
			"Thread::start - thread is already running");
	}
	if(!sameId(m_id, NULLTHREAD))
	{
		OW_THROW(ThreadException,
			"Thread::start - cannot start previously run thread");
	}
	m_isStarting = true;
	UInt32 flgs = OW_THREAD_FLG_JOINABLE;
	ThreadBarrier barrier(2);
	// p will be delted by threadRunner
	ThreadParam* p = new ThreadParam(this, cb, barrier);
	if(ThreadImpl::createThread(m_id, threadRunner, p, flgs) != 0)
	{
		OW_THROW(Assertion, "ThreadImpl::createThread failed");
	}
	m_isStarting = false;
	barrier.wait();
}
//////////////////////////////////////////////////////////////////////////////
// Wait for this object's thread execution (if any) to complete.
Int32
Thread::join()
{
	OW_ASSERT(!sameId(m_id, NULLTHREAD));
	Int32 rval;
	if(ThreadImpl::joinThread(m_id, rval) != 0)
	{
		OW_THROW(ThreadException,
			format("Thread::join - ThreadImpl::joinThread: %1(%2)", 
				   errno, strerror(errno)).c_str());
	}
	// need to set this to false, since the thread may have been cancelled, in which case the m_isRunning flag will be wrong.
	m_isRunning = false;
	m_joined = true;
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
// Method used for starting threads
Int32
Thread::threadRunner(void* paramPtr)
{
	Thread_t theThreadID;
	Int32 rval = -1;
	try
	{
		// scope is important so destructors will run before the thread is clobbered by exitThread
		OW_ASSERT(paramPtr != NULL);
		ThreadParam* pParam = static_cast<ThreadParam*>(paramPtr);
		Thread* pTheThread = pParam->thread;
		ThreadImpl::saveThreadInTLS(pTheThread);
		theThreadID = pTheThread->m_id;
		Reference<ThreadDoneCallback> cb = pParam->cb;
		ThreadBarrier barrier = pParam->barrier;
		delete pParam;
		pTheThread->m_isRunning = true;
		barrier.wait();

		try
		{
			rval = pTheThread->run();
		}
		// make sure we get all exceptions so the thread is cleaned up properly
		catch (ThreadCancelledException&)
		{
		}
		catch (Exception& ex)
		{
#ifdef OW_DEBUG		
			std::cerr << "!!! Exception: " << ex.type() << " caught in Thread class\n";
			std::cerr << ex << std::endl;
#endif
			pTheThread->doneRunning(cb);
			// we need to re-throw here, otherwise we'll segfault
			// if pthread_cancel() does forced stack unwinding.
			throw;
		}
		catch (...)
		{
#ifdef OW_DEBUG		
			std::cerr << "!!! Unknown Exception caught in Thread class" << std::endl;
#endif
			pTheThread->doneRunning(cb);
			// we need to re-throw here, otherwise we'll segfault
			// if pthread_cancel() does forced stack unwinding.
			throw;
		}

		pTheThread->doneRunning(cb);
		
	}
	catch (Exception& ex)
	{
#ifdef OW_DEBUG		
		std::cerr << "!!! Exception: " << ex.type() << " caught in Thread class\n";
		std::cerr << ex << std::endl;
#endif
		// end the thread.  exitThread never returns.
		ThreadImpl::exitThread(theThreadID, rval);
	}
	catch (...)
	{
#ifdef OW_DEBUG		
		std::cerr << "!!! Unknown Exception caught in Thread class" << std::endl;
#endif
		// end the thread.  exitThread never returns.
		ThreadImpl::exitThread(theThreadID, rval);
	}
	// end the thread.  exitThread never returns.
	ThreadImpl::exitThread(theThreadID, rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////
void
Thread::doneRunning(const ThreadDoneCallbackRef& cb)
{
	NonRecursiveMutexLock l(m_cancelLock);
	m_isRunning = m_isStarting = false;
	m_cancelled = true;
	m_cancelCond.notifyAll();
	if (cb)
	{
		cb->notifyThreadDone(this);
	}
}

//////////////////////////////////////////////////////////////////////
static Thread_t
zeroThread()
{
	Thread_t zthr;
	::memset(&zthr, 0, sizeof(zthr));
	return zthr;
}
//////////////////////////////////////////////////////////////////////
void
Thread::cooperativeCancel()
{
	if (!isRunning())
		return;
	// give the thread a chance to clean up a bit or abort the cancellation.
	doCooperativeCancel();
	NonRecursiveMutexLock l(m_cancelLock);
	m_cancelRequested = true;
	// send a SIGUSR1 to the thread to break it out of any blocking syscall.
	// SIGUSR1 is ignored.  It's set to SIG_IGN in ThreadImpl.cpp
	// If the thread has already exited, an ThreadException will be thrown
	// we just want to ignore that.
	try
	{
		ThreadImpl::sendSignalToThread(m_id, SIGUSR1);
	}
	catch (ThreadException&) 
	{
	}
}
//////////////////////////////////////////////////////////////////////
bool
Thread::definitiveCancel(UInt32 waitForCooperativeSecs)
{
	if (!isRunning())
		return true;
	// give the thread a chance to clean up a bit or abort the cancellation.
	doCooperativeCancel();
	NonRecursiveMutexLock l(m_cancelLock);
	m_cancelRequested = true;
	// send a SIGUSR1 to the thread to break it out of any blocking syscall.
	// SIGUSR1 is ignored.  It's set to SIG_IGN in ThreadImpl.cpp
	// If the thread has already exited, an ThreadException will be thrown
	// we just want to ignore that.
	try
	{
		ThreadImpl::sendSignalToThread(m_id, SIGUSR1);
	}
	catch (ThreadException&) 
	{
	}
	while (!m_cancelled && isRunning())
	{
		if (!m_cancelCond.timedWait(l, waitForCooperativeSecs, 0))
		{
			// give the thread a chance to clean up a bit or abort the cancellation.
			doDefinitiveCancel();
			// thread didn't (or won't) exit by itself, we'll have to really kill it.
			if (!m_cancelled && isRunning())
			{
				this->cancel();
			}
			return false;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////
void
Thread::cancel()
{
	// Ignore errors from ThreadImpl (usually caused by the fact that the thread has already exited)
	try
	{
		ThreadImpl::cancel(m_id);
	}
	catch (ThreadException&)
	{
	}
	m_cancelled = true;
}
//////////////////////////////////////////////////////////////////////
void
Thread::testCancel()
{
	ThreadImpl::testCancel();
}
//////////////////////////////////////////////////////////////////////
void 
Thread::doCooperativeCancel()
{
}
//////////////////////////////////////////////////////////////////////
void 
Thread::doDefinitiveCancel()
{
}

} // end namespace OpenWBEM

