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

#include "OW_ThreadPool.hpp"
#include "OW_Array.hpp"
#include "OW_Thread.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Condition.hpp"

#include <deque>

/////////////////////////////////////////////////////////////////////////////

class OW_ThreadPoolImpl;

namespace {

class workerThread : public OW_Thread
{
public:
	workerThread(OW_ThreadPoolImpl* thePool)
		: OW_Thread(true) // joinable
		, m_thePool(thePool)
	{
	}

	virtual OW_Int32 run();

private:
	OW_ThreadPoolImpl* m_thePool;
};

} // end anonymous namespace

class OW_ThreadPoolImpl
{
public:
	OW_ThreadPoolImpl(OW_UInt32 numThreads, OW_UInt32 maxQueueSize)
		: m_maxQueueSize(maxQueueSize)
		, m_queueClosed(false)
		, m_shutdown(false)
	{
		m_threads.reserve(numThreads);
		for (OW_UInt32 i = 0; i < numThreads; ++i)
		{
			m_threads.push_back(OW_ThreadRef(new workerThread(this)));
		}
		for (OW_UInt32 i = 0; i < numThreads; ++i)
		{
			m_threads[i]->start();
		}
	}

	// returns true if work is placed in the queue to be run and false if not.
	bool addWork(const OW_RunnableRef& work, bool blockWhenFull)
	{
		// check precondition: work != NULL
		if (!work)
		{
			return false;
		}

		OW_NonRecursiveMutexLock l(m_queueLock);
		if ((m_maxQueueSize > 0) && (m_queue.size() == m_maxQueueSize) && !blockWhenFull)
		{
			return false;
		}
		while( (m_maxQueueSize > 0) && (m_queue.size() == m_maxQueueSize) && (!(m_shutdown || m_queueClosed)) )
		{
			m_queueNotFull.wait(l);
		}

		// the pool is in the process of being destroyed
		if (m_shutdown || m_queueClosed) 
		{
			return false;
		}

		m_queue.push_back(work);
		
		// if the queue was empty, there may be workers just sitting around, so we need to wake them up!
		if (m_queue.size() == 1)
		{
			m_queueNotEmpty.notifyAll();
		}
		return true;
	}

	void shutdown(bool finishWorkInQueue)
	{
		{ // scope for l
			OW_NonRecursiveMutexLock l(m_queueLock);
			// the pool is in the process of being destroyed
			if (m_shutdown || m_queueClosed) 
			{
				return;
			}
			
			m_queueClosed = true;
			
			if (finishWorkInQueue)
			{
				while (m_queue.size() != 0)
				{
					m_queueEmpty.wait(l);
				}
			}

			m_shutdown = true;

		}
		// Wake up any workers so they recheck shutdown flag
		m_queueNotEmpty.notifyAll();
		m_queueNotFull.notifyAll();

		// TODO: Set cooperative thread cancellation flag (once it's implemented)

		// Wait for workers to exit
		for (OW_UInt32 i = 0; i < m_threads.size(); ++i)
		{
			m_threads[i]->join();
		}
		
	}

	void waitForEmptyQueue()
	{
		OW_NonRecursiveMutexLock l(m_queueLock);
		while (m_queue.size() != 0)
		{
			m_queueEmpty.wait(l);
		}
	}

	~OW_ThreadPoolImpl()
	{
		// can't let exception escape the destructor
		try
		{
			// Make sure the pool is shutdown.
			shutdown(false);
		}
		catch (...)
		{
		}
	}

private:
	// pool characteristics
	OW_UInt32 m_maxQueueSize;

	// pool state
	OW_Array<OW_ThreadRef> m_threads;
	std::deque<OW_RunnableRef> m_queue;
	bool m_queueClosed;

	bool m_shutdown;

	// pool synchronization
	OW_NonRecursiveMutex m_queueLock;
	OW_Condition m_queueNotEmpty;
	OW_Condition m_queueNotFull;
	OW_Condition m_queueEmpty;

	friend class workerThread;
};

namespace {

OW_Int32 workerThread::run()
{
	while (true)
	{
		// check queue for work
		OW_NonRecursiveMutexLock l(m_thePool->m_queueLock);
		while ((m_thePool->m_queue.size() == 0) && (!m_thePool->m_shutdown))
		{
			m_thePool->m_queueNotEmpty.wait(l);
		}

		// check to see if a shutdown started while I was sleeping
		if (m_thePool->m_shutdown)
		{
			return 0;
		}

		OW_RunnableRef work = m_thePool->m_queue.front();
		m_thePool->m_queue.pop_front();

		// handle threads waiting in addWork()
		if (m_thePool->m_queue.size() == (m_thePool->m_maxQueueSize - 1))
		{
			m_thePool->m_queueNotFull.notifyAll();
		}

		// handle waiting shutdown thread or callers of waitForEmptyQueue()
		if (m_thePool->m_queue.size() == 0)
		{
			m_thePool->m_queueEmpty.notifyAll();
		}

		l.release();

		// don't let exceptions escape, we need to keep going
		try
		{
			work->run();
		}
		catch (...)
		{
		}
	}
	return 0;
}


} // end anonymous namespace

/////////////////////////////////////////////////////////////////////////////
OW_ThreadPool::OW_ThreadPool(OW_UInt32 numThreads, OW_UInt32 maxQueueSize)
	: m_impl(new OW_ThreadPoolImpl(numThreads, maxQueueSize))
{
}

/////////////////////////////////////////////////////////////////////////////
bool OW_ThreadPool::addWork(const OW_RunnableRef& work)
{
	return m_impl->addWork(work, true);
}

/////////////////////////////////////////////////////////////////////////////
bool OW_ThreadPool::tryAddWork(const OW_RunnableRef& work)
{
	return m_impl->addWork(work, false);
}

/////////////////////////////////////////////////////////////////////////////
void OW_ThreadPool::shutdown(bool finishWorkInQueue)
{
	m_impl->shutdown(finishWorkInQueue);
}

/////////////////////////////////////////////////////////////////////////////
void OW_ThreadPool::waitForEmptyQueue()
{
	m_impl->waitForEmptyQueue();
}

/////////////////////////////////////////////////////////////////////////////
OW_ThreadPool::~OW_ThreadPool()
{
}

/////////////////////////////////////////////////////////////////////////////
OW_ThreadPool::OW_ThreadPool(const OW_ThreadPool& x)
	: m_impl(x.m_impl)
{
}


/////////////////////////////////////////////////////////////////////////////
OW_ThreadPool& OW_ThreadPool::operator=(const OW_ThreadPool& x)
{
	m_impl = x.m_impl;
	return *this;
}

