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
#include "OW_RWLocker.hpp"
#include "OW_Assertion.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_TimeoutException.hpp"

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION(RWLocker);
//////////////////////////////////////////////////////////////////////////////
RWLocker::RWLocker()
	: m_waiting_writers()
	, m_waiting_readers()
	, m_num_waiting_writers(0)
	, m_num_waiting_readers(0)
	, m_readers_next(0)
	, m_guard()
	, m_state(0)
{
}
//////////////////////////////////////////////////////////////////////////////
RWLocker::~RWLocker()
{
	// ???
	//try
	//{
	//	m_cond.notifyAll();
	//}
	//catch (...)
	//{
		// don't let exceptions escape
	//}
}
//////////////////////////////////////////////////////////////////////////////
void
RWLocker::getReadLock(UInt32 sTimeout, UInt32 usTimeout)
{
	NonRecursiveMutexLock l(m_guard);
	
	// Wait until no exclusive lock is held.
	//
	// Note:  Scheduling priorities are enforced in the unlock()
	//   call.  unlock will wake the proper thread.
	// if we will lock, then make sure we won't deadlock
	Thread_t tid = ThreadImpl::currentThread();
	if (m_state < 0)
	{
		// see if we already have the write lock
		if (ThreadImpl::sameThreads(m_writer, tid))
		{
			OW_THROW(DeadlockException, "A thread that has a write lock is trying to acquire a read lock.");
		}
	}
	while(m_state < 0)
	{
		++m_num_waiting_readers;
		//m_waiting_readers.wait(l);
		if (!m_waiting_readers.timedWait(l, sTimeout, usTimeout))
		{
			--m_num_waiting_readers;
			OW_THROW(TimeoutException, "Timeout while waiting for read lock.");
		}
		--m_num_waiting_readers;
	}
	
	// Increase the reader count
	m_state++;
	m_readers.push_back(tid);
}
//////////////////////////////////////////////////////////////////////////////
void
RWLocker::getWriteLock(UInt32 sTimeout, UInt32 usTimeout)
{
	NonRecursiveMutexLock l(m_guard);
	// Wait until no exclusive lock is held.
	//
	// Note:  Scheduling priorities are enforced in the unlock()
	//   call.  unlock will wake the proper thread.
	// if we will lock, then make sure we won't deadlock
	Thread_t tid = ThreadImpl::currentThread();
	if (m_state != 0)
	{
		// see if we already have a read lock
		for (size_t i = 0; i < m_readers.size(); ++i)
		{
			if (ThreadImpl::sameThreads(m_readers[i], tid))
			{
				OW_THROW(DeadlockException, "A thread that has a read lock is trying to acquire a write lock.");
			}
		}
	}
	while(m_state != 0)
	{
		++m_num_waiting_writers;
		if (!m_waiting_writers.timedWait(l, sTimeout, usTimeout))
		{
			--m_num_waiting_writers;
			OW_THROW(TimeoutException, "Timeout while waiting for write lock.");
		}
		--m_num_waiting_writers;
	}
	m_state = -1;
	m_writer = tid;
}
//////////////////////////////////////////////////////////////////////////////
void
RWLocker::releaseReadLock()
{
	NonRecursiveMutexLock l(m_guard);
	if(m_state > 0)        // Release a reader.
		--m_state;
	else
		OW_THROW(RWLockerException, "A writer is releasing a read lock");
	if(m_state == 0)
	{
		doWakeups();
	}
	Thread_t tid = ThreadImpl::currentThread();
	for (size_t i = 0; i < m_readers.size(); ++i)
	{
		if (ThreadImpl::sameThreads(m_readers[i], tid))
		{
			m_readers.remove(i);
			i--;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
RWLocker::releaseWriteLock()
{
	NonRecursiveMutexLock l(m_guard);
	if(m_state == -1)
		m_state = 0;
	else
		OW_THROW(RWLockerException, "A reader is releasing a write lock");
	// After a writer is unlocked, we are always back in the unlocked state.
	//
	doWakeups();
}
//////////////////////////////////////////////////////////////////////////////
void
RWLocker::doWakeups()
{
	if( m_num_waiting_writers > 0 && 
		m_num_waiting_readers > 0)
	{
		if(m_readers_next == 1)
		{
			m_readers_next = 0;
			m_waiting_readers.notifyAll();
		}
		else
		{
			m_waiting_writers.notifyOne();
			m_readers_next = 1;
		}
	}
	else if(m_num_waiting_writers > 0)
	{
		// Only writers - scheduling doesn't matter
		m_waiting_writers.notifyOne();
	}
	else if(m_num_waiting_readers > 0)
	{
		// Only readers - scheduling doesn't matter
		m_waiting_readers.notifyAll();
	}
}

} // end namespace OpenWBEM

