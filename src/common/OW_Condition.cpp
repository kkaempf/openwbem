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
#include "OW_Condition.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_ExceptionIds.hpp"

#include <cassert>
#include <cerrno>
#ifdef OW_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(ConditionLock);
OW_DEFINE_EXCEPTION_WITH_ID(ConditionResource);
#if defined(OW_USE_PTHREAD)
/////////////////////////////////////////////////////////////////////////////
Condition::Condition()
{
	int res = pthread_cond_init(&m_condition, 0);
	if (res != 0)
	{
		OW_THROW(ConditionResourceException, "Failed initializing condition variable");
	}
}
/////////////////////////////////////////////////////////////////////////////
Condition::~Condition()
{
	int res = pthread_cond_destroy(&m_condition);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void
Condition::notifyOne()
{
	int res = pthread_cond_signal(&m_condition);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void
Condition::notifyAll()
{
	int res = pthread_cond_broadcast(&m_condition);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void
Condition::doWait(NonRecursiveMutex& mutex)
{
	int res;
	NonRecursiveMutexLockState state;
	mutex.conditionPreWait(state);
	res = pthread_cond_wait(&m_condition, state.pmutex);
	mutex.conditionPostWait(state);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
bool
Condition::doTimedWait(NonRecursiveMutex& mutex, UInt32 sTimeout, UInt32 usTimeout)
{
	int res;
	NonRecursiveMutexLockState state;
	mutex.conditionPreWait(state);
	bool ret = false;
	timespec ts;
	struct timeval now;
	::gettimeofday(&now, NULL);
	
	ts.tv_sec = now.tv_sec + sTimeout;

	const int NANOSECONDS_PER_MICROSECOND = 1000;
	const int NANOSECONDS_PER_SECOND = 1000000000;
	int nsec = (now.tv_usec + usTimeout) * NANOSECONDS_PER_MICROSECOND;
	ts.tv_sec += nsec / NANOSECONDS_PER_SECOND;
	ts.tv_nsec = nsec % NANOSECONDS_PER_SECOND;

	res = pthread_cond_timedwait(&m_condition, state.pmutex, &ts);
	mutex.conditionPostWait(state);
	assert(res == 0 || res == ETIMEDOUT);
	ret = res != ETIMEDOUT;
	return ret;
}
#elif defined (OW_WIN32)
/////////////////////////////////////////////////////////////////////////////
Condition::Condition()
	: m_condition(new ConditionInfo_t)
{
	m_condition->waitersCount = 0;
	m_condition->wasBroadcast = false;
	m_condition->queue = ::CreateSemaphore(
		NULL,		// No security
		0,			// initially 0
		0x7fffffff,	// max count
		NULL);		// Unnamed
	::InitializeCriticalSection(&m_condition->waitersCountLock);
	m_condition->waitersDone = ::CreateEvent(
		NULL,		// No security
		false,		// auto-reset
		false,		// non-signaled initially
		NULL);		// Unnamed
}
/////////////////////////////////////////////////////////////////////////////
Condition::~Condition()
{
	::CloseHandle(m_condition->queue);
	::DeleteCriticalSection(&m_condition->waitersCountLock);
	::CloseHandle(m_condition->waitersDone);
	delete m_condition;
}
/////////////////////////////////////////////////////////////////////////////
void
Condition::notifyOne()
{
	::EnterCriticalSection(&m_condition->waitersCountLock);
	bool haveWaiters = m_condition->waitersCount > 0;
	::LeaveCriticalSection(&m_condition->waitersCountLock);

	// If no threads waiting, then this is a no-op
	if (haveWaiters)
	{
		::ReleaseSemaphore(m_condition->queue, 1, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void
Condition::notifyAll()
{
	::EnterCriticalSection(&m_condition->waitersCountLock);
	bool haveWaiters = false;
	if (m_condition->waitersCount > 0)
	{
		// It's gonna be a broadcast, even if there's only one waiting thread.
		haveWaiters = m_condition->wasBroadcast = true;
	}

	if (haveWaiters)
	{
		// Wake up all the waiting threads atomically
		::ReleaseSemaphore(m_condition->queue, m_condition->waitersCount, 0);
		::LeaveCriticalSection(&m_condition->waitersCountLock);

		// Wait for all the threads to acquire the counting semaphore
		::WaitForSingleObject(m_condition->waitersDone, INFINITE);
		m_condition->wasBroadcast = false;
	}
	else
	{
		::LeaveCriticalSection(&m_condition->waitersCountLock);
	}
}
/////////////////////////////////////////////////////////////////////////////
void
Condition::doWait(NonRecursiveMutex& mutex)
{
	doTimedWait(mutex, INFINITE, 0);
}
/////////////////////////////////////////////////////////////////////////////
bool
Condition::doTimedWait(NonRecursiveMutex& mutex, UInt32 sTimeout, UInt32 usTimeout)
{
	bool cc = true;
	NonRecursiveMutexLockState state;
	mutex.conditionPreWait(state);

	::EnterCriticalSection(&m_condition->waitersCountLock);
	m_condition->waitersCount++;
	::LeaveCriticalSection(&m_condition->waitersCountLock);

	// Calc timeout if specified
	if (sTimeout != INFINITE)
	{
		sTimeout *= 1000;		// Convert to ms
		sTimeout += usTimeout / 1000;		// Convert micro seconds to ms and add
	}

	// Atomically release the mutex and wait on the
	// queue until signal/broadcast.
	if (::SignalObjectAndWait(mutex.m_mutex, m_condition->queue, sTimeout,
		false) == WAIT_TIMEOUT)
	{
		cc = false;
	}

	::EnterCriticalSection(&m_condition->waitersCountLock);
	m_condition->waitersCount--;

	// Check to see if we're the last waiter after the broadcast
	bool isLastWaiter = (m_condition->wasBroadcast && m_condition->waitersCount == 0
		&& cc == true);

	::LeaveCriticalSection(&m_condition->waitersCountLock);

	// If this is the last thread waiting for this broadcast, then let all the
	// other threads proceed.
	if (isLastWaiter)
	{
		// Atomically signal the waitersDone event and wait to acquire
		// the external mutex. Enusres fairness
		::SignalObjectAndWait(m_condition->waitersDone, mutex.m_mutex,
			INFINITE, false);
	}
	else
	{
		// Re-gain ownership of the external mutex
		::WaitForSingleObject(mutex.m_mutex, INFINITE);
	}
	mutex.conditionPostWait(state);
	return cc;
}
#else
#error "port me!"
#endif
/////////////////////////////////////////////////////////////////////////////
void
Condition::wait(NonRecursiveMutexLock& lock)
{
	if (!lock.isLocked())
	{
		OW_THROW(ConditionLockException, "Lock must be locked");
	}
	doWait(*(lock.m_mutex));
}
/////////////////////////////////////////////////////////////////////////////
bool
Condition::timedWait(NonRecursiveMutexLock& lock, UInt32 sTimeout, UInt32 usTimeout)
{
	if (!lock.isLocked())
	{
		OW_THROW(ConditionLockException, "Lock must be locked");
	}
	return doTimedWait(*(lock.m_mutex), sTimeout, usTimeout);
}

} // end namespace OpenWBEM

