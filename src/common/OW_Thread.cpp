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
#include <iostream>
#include <csignal>

//////////////////////////////////////////////////////////////////////////////
DEFINE_EXCEPTION(Thread);
DEFINE_EXCEPTION(CancellationDenied);

//////////////////////////////////////////////////////////////////////
// this is what's really passed to threadRunner
struct OW_ThreadParam
{
	OW_ThreadParam(OW_Thread* t, const OW_Reference<OW_ThreadDoneCallback>& c, const OW_ThreadBarrier& b)
		: thread(t)
		, cb(c)
		, barrier(b)
	{}

	OW_Thread* thread;
	OW_Reference<OW_ThreadDoneCallback> cb;
	OW_ThreadBarrier barrier;
};


static OW_Thread_t zeroThread();
static OW_Thread_t NULLTHREAD = zeroThread();

//////////////////////////////////////////////////////////////////////
static inline bool
sameId(const OW_Thread_t& t1, const OW_Thread_t& t2)
{
	return OW_ThreadImpl::sameThreads(t1, t2);
}

//////////////////////////////////////////////////////////////////////////////
// Constructor
OW_Thread::OW_Thread() 
	: m_id(NULLTHREAD)
	, m_isRunning(false)
	, m_isStarting(false)
	, m_cancelRequested(false)
	, m_cancelled(false)
{
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
OW_Thread::~OW_Thread()
{
	try
	{
		try
		{
			join();
		}
		catch (OW_ThreadException& e)
		{
			// this will happen if join has already been called
		}

		OW_ASSERT(m_isRunning == false);
		if(!sameId(m_id, NULLTHREAD))
		{
			OW_ThreadImpl::destroyThread(m_id);
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
OW_Thread::start(OW_Reference<OW_ThreadDoneCallback> cb)
{
	if(isRunning())
	{
		OW_THROW(OW_ThreadException,
			"OW_Thread::start - thread is already running");
	}

	if(!sameId(m_id, NULLTHREAD))
	{
		OW_THROW(OW_ThreadException,
			"OW_Thread::start - cannot start previously run thread");
	}

	m_isStarting = true;

	OW_UInt32 flgs = OW_THREAD_FLG_JOINABLE;

	OW_ThreadBarrier barrier(2);
	// p will be delted by threadRunner
	OW_ThreadParam* p = new OW_ThreadParam(this, cb, barrier);
	if(OW_ThreadImpl::createThread(m_id, threadRunner, p, flgs) != 0)
	{
		OW_THROW(OW_Assertion, "OW_ThreadImpl::createThread failed");
	}

	m_isStarting = false;

	barrier.wait();

	// Note that you can't do *anything* with the this pointer after barrier.wait()
	// returns, because it may have been deleted already by the thread itself.
}

//////////////////////////////////////////////////////////////////////////////
// Wait for this object's thread execution (if any) to complete.
OW_Int32
OW_Thread::join()
{
	OW_ASSERT(!sameId(m_id, NULLTHREAD));

	OW_Int32 rval;
	if(OW_ThreadImpl::joinThread(m_id, rval) != 0)
	{
		OW_THROW(OW_ThreadException,
			format("OW_Thread::join - OW_ThreadImpl::joinThread: %1(%2)", 
				   errno, strerror(errno)).c_str());
	}

	// need to set this to false, since the thread may have been cancelled, in which case the m_isRunning flag will be wrong.
	m_isRunning = false;

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
// Method used for starting threads
OW_Int32
OW_Thread::threadRunner(void* paramPtr)
{
	OW_Thread_t theThreadID;
	OW_Int32 rval = -1;
	try
	{
		// scope is important so destructors will run before the thread is clobbered by exitThread
		OW_ASSERT(paramPtr != NULL);

		OW_ThreadParam* pParam = static_cast<OW_ThreadParam*>(paramPtr);
		OW_Thread* pTheThread = pParam->thread;
		OW_ThreadImpl::saveThreadInTLS(pTheThread);
		theThreadID = pTheThread->m_id;
		OW_Reference<OW_ThreadDoneCallback> cb = pParam->cb;
		OW_ThreadBarrier barrier = pParam->barrier;

		pTheThread->m_isRunning = true;

		barrier.wait();

		try
		{
			rval = pTheThread->run();
		}
		catch (OW_ThreadCancelledException&)
		{
		}


		{
			OW_NonRecursiveMutexLock l(pTheThread->m_cancelLock);
			pTheThread->m_isRunning = pTheThread->m_isStarting = false;
			pTheThread->m_cancelled = true;
			pTheThread->m_cancelCond.notifyAll();
		}

		delete pParam;

		// Note that this *has* to come after the 'delete pTheThread' statement.
		// This is because during shutdown the notifyThreadDone will decrement
		// the # of threads running, which will allow shutdown to proceed once
		// the # == 0, and then subsequently the library that contains the 
		// thread's code will be unloaded.  If the destructor of pTheThread 
		// runs after the library is unloaded, then *boom*, segfault.
		if (cb)
		{
			cb->notifyThreadDone(pTheThread);
		}

	}
	catch (OW_Exception& ex)
	{
#ifdef OW_DEBUG		
		std::cerr << "!!! Exception: " << ex.type() << " caught in OW_Thread class\n";
		std::cerr << ex << std::endl;
#endif
	}
	catch (...)
	{
#ifdef OW_DEBUG		
		std::cerr << "!!! Unknown Exception caught in OW_Thread class" << std::endl;
#endif
	}

	// end the thread.  exitThread never returns.
	OW_ThreadImpl::exitThread(theThreadID, rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////
static OW_Thread_t
zeroThread()
{
	OW_Thread_t zthr;
	::memset(&zthr, 0, sizeof(zthr));
	return zthr;
}

//////////////////////////////////////////////////////////////////////
void
OW_Thread::cooperativeCancel()
{
	if (!isRunning())
		return;

	// give the thread a chance to clean up a bit or abort the cancellation.
	doCooperativeCancel();

	OW_NonRecursiveMutexLock l(m_cancelLock);
	m_cancelRequested = true;

	// send a SIGUSR1 to the thread to break it out of any blocking syscall.
	// SIGUSR1 is ignored.  It's set to SIG_IGN in OW_ThreadImpl.cpp
	// If the thread has already exited, an OW_ThreadException will be thrown
	// we just want to ignore that.
	try
	{
		OW_ThreadImpl::sendSignalToThread(m_id, SIGUSR1);
	}
	catch (OW_ThreadException&) 
	{
	}
}

//////////////////////////////////////////////////////////////////////
bool
OW_Thread::definitiveCancel(OW_UInt32 waitForCooperativeSecs)
{
	if (!isRunning())
		return true;

	// give the thread a chance to clean up a bit or abort the cancellation.
	doCooperativeCancel();

	OW_NonRecursiveMutexLock l(m_cancelLock);
	m_cancelRequested = true;

	// send a SIGUSR1 to the thread to break it out of any blocking syscall.
	// SIGUSR1 is ignored.  It's set to SIG_IGN in OW_ThreadImpl.cpp
	// If the thread has already exited, an OW_ThreadException will be thrown
	// we just want to ignore that.
	try
	{
		OW_ThreadImpl::sendSignalToThread(m_id, SIGUSR1);
	}
	catch (OW_ThreadException&) 
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
OW_Thread::cancel()
{
	// Ignore errors from OW_ThreadImpl (usually caused by the fact that the thread has already exited)
	try
	{
		OW_ThreadImpl::cancel(m_id);
	}
	catch (OW_ThreadException&)
	{
	}
}

//////////////////////////////////////////////////////////////////////
void
OW_Thread::testCancel()
{
	OW_ThreadImpl::testCancel();
}


//////////////////////////////////////////////////////////////////////
void 
OW_Thread::doCooperativeCancel()
{
}

//////////////////////////////////////////////////////////////////////
void 
OW_Thread::doDefinitiveCancel()
{
}


