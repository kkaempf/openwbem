/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
#include "OW_ThreadBarrier.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_IntrusiveCountableBase.hpp"

#if defined(OW_USE_PTHREAD) && defined(OW_HAVE_PTHREAD_BARRIER) && !defined(OW_VALGRIND_SUPPORT)
 #include <pthread.h>
#else
 // This is for the generic less-efficient version
 #include "OW_Condition.hpp"
 #include "OW_NonRecursiveMutex.hpp"
 #include "OW_NonRecursiveMutexLock.hpp"
#endif

/////////////////////////////////////////////////////////////////////////////
namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(ThreadBarrier);

#if defined(OW_USE_PTHREAD) && defined(OW_HAVE_PTHREAD_BARRIER) && !defined(OW_VALGRIND_SUPPORT) // valgrind doesn't support pthread_barrier_*()
class ThreadBarrierImpl : public IntrusiveCountableBase
{
public:
	ThreadBarrierImpl(UInt32 threshold)
	{
		OW_ASSERT(threshold != 0);
		int res = pthread_barrier_init(&barrier, NULL, threshold);
		if (res != 0)
		{
			OW_THROW(ThreadBarrierException, Format("pthread_barrier_init failed: %1(%2)", res, strerror(res)).c_str());
		}
	}
	~ThreadBarrierImpl()
	{
		int res = pthread_barrier_destroy(&barrier);
		if (res != 0)
		{
			// can't throw... just log it or something...
		}
	}
	
	void wait()
	{
		int res = pthread_barrier_wait(&barrier);
		if (res != 0 && res != PTHREAD_BARRIER_SERIAL_THREAD)
		{
			OW_THROW(ThreadBarrierException, Format("pthread_barrier_wait failed: %1(%2)", res, strerror(res)).c_str());
		}
	}
private:	
	pthread_barrier_t barrier;
};

#else

// This is the generic less-efficient version

class ThreadBarrierImpl : public IntrusiveCountableBase
{
public:
	/**
	 * This code is inspired by ACE, by Douglas C. Schmidt <schmidt@cs.wustl.edu>
	 * File Barrier.h and Barrier.cpp
	 * We keep two SubBarriers, one for the first "generation" of
	 * waiters, and one for the next "generation" of waiters.  This
	 * efficiently solves the problem of what to do if all the first
	 * generation waiters don't leave the barrier before one of the
	 * threads calls wait() again (i.e., starts up the next generation
	 * barrier).
	 */
	struct SubBarrier
	{
		SubBarrier() : m_waitingCount(0) {}
		/// The number of waiting threads
		UInt32 m_waitingCount;
		/// Condition for threads to wait on.
		Condition m_cond;
	};
	ThreadBarrierImpl(UInt32 threshold)
		: m_threshold(threshold)
    		, m_curSubBarrier(0)
	{
	}
	void wait()
	{
		NonRecursiveMutexLock l(m_mutex);
		// Select the current SubBarrier
		SubBarrier& curBarrier = m_curSubBarrier?m_subBarrier0:m_subBarrier1;
		++curBarrier.m_waitingCount;
		if (curBarrier.m_waitingCount == m_threshold)
		{
			// reset the sub barrier so it can be reused
			curBarrier.m_waitingCount = 0;
			// swap current barriers
			m_curSubBarrier = 1 - m_curSubBarrier;
			// now wake up all the threads that were stopped
			curBarrier.m_cond.notifyAll();
		}
		else
		{
			// because of spurious wake-ups we need to put this in a loop.
			// we need to wait until the count is 0, which will only happen
			// once m_threshold threads have called wait()
			while (curBarrier.m_waitingCount != 0)
			{
				curBarrier.m_cond.wait(l);
			}
		}
	}
private:
	/// The number of threads to synchronize
	UInt32 m_threshold;
	/// Either 0 or 1, depending on whether we are the first generation
	/// of waiters or the next generation of waiters.
	int m_curSubBarrier;
	NonRecursiveMutex m_mutex;
	SubBarrier m_subBarrier0;
	SubBarrier m_subBarrier1;
};

#endif

/////////////////////////////////////////////////////////////////////////////
ThreadBarrier::ThreadBarrier(UInt32 threshold)
	: m_impl(new ThreadBarrierImpl(threshold))
{
	OW_ASSERT(threshold != 0);
}
/////////////////////////////////////////////////////////////////////////////
void ThreadBarrier::wait()
{
	m_impl->wait();
}
/////////////////////////////////////////////////////////////////////////////
ThreadBarrier::~ThreadBarrier()
{
}
/////////////////////////////////////////////////////////////////////////////
ThreadBarrier::ThreadBarrier(const ThreadBarrier& x)
	: m_impl(x.m_impl)
{
}
/////////////////////////////////////////////////////////////////////////////
ThreadBarrier& ThreadBarrier::operator=(const ThreadBarrier& x)
{
	m_impl = x.m_impl;
	return *this;
}

} // end namespace OW_NAMESPACE

