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
#include "OW_RWLocker.hpp"
#include "OW_Assertion.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_TimeoutException.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_Timeout.hpp"
#include "OW_TimeoutTimer.hpp"

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(RWLocker);

inline bool RWLocker::ThreadComparer::operator()(Thread_t x, Thread_t y) const
{
	return !ThreadImpl::sameThreads(x, y) && x < y;
}

//////////////////////////////////////////////////////////////////////////////
RWLocker::RWLocker()
	: m_canRead(true)
	, m_numReaderThreads(0)
	, m_numWriterThreads(0)
{
}
//////////////////////////////////////////////////////////////////////////////
RWLocker::~RWLocker()
{
}
//////////////////////////////////////////////////////////////////////////////
void
RWLocker::getReadLock(UInt32 sTimeout, UInt32 usTimeout)
{
	getReadLock(Timeout::relative(sTimeout + static_cast<float>(usTimeout) * 1000000.0));
}

//////////////////////////////////////////////////////////////////////////////
void
RWLocker::getReadLock(const Timeout& timeout)
{
	TimeoutTimer timer(timeout);
	Thread_t tid = ThreadImpl::currentThread();

	NonRecursiveMutexLock l(m_guard);
	ThreadMap::iterator info = m_threads.find(tid);

	if (info != m_threads.end())
	{
		ThreadInfo& ti(info->second);
		// The thread already have a read or write lock, so just increment.
		OW_ASSERT(ti.isReader() || ti.isWriter());
		++ti.readCount;
		return;
	}

	// The thread is a new reader

	while (!m_canRead || m_numWriterThreads > 0)
	{
		if (!m_waiting_readers.timedWait(l, timer.asAbsoluteTimeout()))
		{
			OW_THROW(TimeoutException, "Timeout while waiting for read lock.");
		}
	}
	
	// Increase the reader count
	ThreadInfo threadInfo;
	threadInfo.readCount = 1;
	threadInfo.writeCount = 0;
	m_threads.insert(ThreadMap::value_type(tid, threadInfo));

	++m_numReaderThreads;
}

//////////////////////////////////////////////////////////////////////////////
void
RWLocker::releaseReadLock()
{
	Thread_t tid = ThreadImpl::currentThread();
	NonRecursiveMutexLock l(m_guard);

	ThreadMap::iterator pInfo = m_threads.find(tid);

	if (pInfo == m_threads.end() || !pInfo->second.isReader())
	{
		OW_THROW(RWLockerException, "Cannot release a read lock when no read lock is held");
	}

	ThreadInfo& info(pInfo->second);
	--info.readCount;

	if (!info.isWriter() && !info.isReader())
	{
		--m_numReaderThreads;
		if (m_numReaderThreads == 0)
		{
			// This needs to wake them all up. In the case where one thread is waiting to upgrade a read to a write lock
			// and others are waiting to get a write lock, we have to wake up the thread trying to upgrade.
			m_waiting_writers.notifyAll();
		}
		m_threads.erase(pInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
RWLocker::getWriteLock(UInt32 sTimeout, UInt32 usTimeout)
{
	getWriteLock(Timeout::relative(sTimeout + static_cast<float>(usTimeout) * 1000000.0));
}
//////////////////////////////////////////////////////////////////////////////
void
RWLocker::getWriteLock(const Timeout& timeout)
{
	// 7 cases:
	// 1. No thread has the lock
	//   Get the lock
	// 2. This thread has the write lock
	//   Increment the lock count
	// 3. Another thread has the write lock & other threads may be waiting for read and/or write locks.
	//   Block until the lock is acquired.
	// 4. Only this thread has a read lock
	//   Increment the write lock count .
	// 5. >0 other threads have the read lock & other threads may be waiting for write locks.
	//   Block until the write lock is acquired.
	// 6. This thread and other threads have the read lock
	//   Block new readers and writers and wait until existing readers finish.
	// 7. This thread and other threads have the read lock and one of the other threads has requested a write lock.
	//   Throw an exception.

	Thread_t tid = ThreadImpl::currentThread();
	TimeoutTimer timer(timeout);

	NonRecursiveMutexLock l(m_guard);

	ThreadMap::iterator pInfo = m_threads.find(tid);
	if (pInfo != m_threads.end())
	{
		// This thread already has some sort of lock
		ThreadInfo& ti(pInfo->second);
		OW_ASSERT(ti.isReader() || ti.isWriter());

		if (!ti.isWriter())
		{
			// The thread is upgrading

			OW_ASSERT(m_numWriterThreads == 0 || m_numWriterThreads == 1);
			if (m_numWriterThreads == 1)
			{
				// another thread beat us to upgrading the write lock.  Throw an exception.
				OW_THROW(DeadlockException, "Upgrading read lock to a write lock failed, another thread is already upgrading.");
			}

			// switch from being a reader to a writer
			--m_numReaderThreads;
			// mark us as a writer, this will prevent other threads from becoming a writer
			++m_numWriterThreads;

			// This thread isn't the only reader. Wait for others to finish.
			while (m_numReaderThreads != 0)
			{
				// stop new readers - inside while loop, because it may get reset by other threads releasing locks.
				m_canRead = false;

				if (!m_waiting_writers.timedWait(l, timer.asAbsoluteTimeout()))
				{
					// undo changes
					++m_numReaderThreads;
					--m_numWriterThreads;
					m_canRead = true;
					if (m_numWriterThreads == 0)
					{
						m_waiting_readers.notifyAll();
					}
					OW_THROW(TimeoutException, "Timeout while waiting for write lock.");
				}
			}
		}
		++ti.writeCount;

	}
	else
	{
		// This thread doesn't have any lock

		while (m_numReaderThreads != 0 || m_numWriterThreads != 0)
		{
			// stop new readers
			m_canRead = false;

			if (!m_waiting_writers.timedWait(l, timer.asAbsoluteTimeout()))
			{
				m_canRead = true;
				if (m_numWriterThreads == 0)
				{
					m_waiting_readers.notifyAll();
				}
				OW_THROW(TimeoutException, "Timeout while waiting for write lock.");
			}
		}

		ThreadInfo ti;
		ti.readCount = 0;
		ti.writeCount = 1;
		m_threads.insert(ThreadMap::value_type(tid, ti));
		++m_numWriterThreads;
		m_canRead = false;
	}

}

//////////////////////////////////////////////////////////////////////////////
void
RWLocker::releaseWriteLock()
{
	Thread_t tid = ThreadImpl::currentThread();
	NonRecursiveMutexLock l(m_guard);
	
	ThreadMap::iterator pInfo = m_threads.find(tid);
	
	if (pInfo == m_threads.end() || !pInfo->second.isWriter())
	{
		OW_THROW(RWLockerException, "Cannot release a write lock when no write lock is held");
	}

	ThreadInfo& ti(pInfo->second);

	OW_ASSERT(ti.isWriter());

	--ti.writeCount;

	if (!ti.isWriter())
	{
		--m_numWriterThreads;

		OW_ASSERT(m_numWriterThreads == 0);

		m_canRead = true;
		if (ti.isReader())
		{
			// restore reader status
			++m_numReaderThreads;
		}
		else
		{
			// This thread no longer holds locks.
			m_waiting_writers.notifyOne();
			m_threads.erase(pInfo);
		}
		m_waiting_readers.notifyAll();
	}
}

} // end namespace OW_NAMESPACE

