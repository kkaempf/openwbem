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

#ifdef OW_DEBUG		
#include <iostream>
#endif

/////////////////////////////////////////////////////////////////////////////

class OW_ThreadPoolImpl
{
public:
	// returns true if work is placed in the queue to be run and false if not.
	virtual bool addWork(const OW_RunnableRef& work, bool blockWhenFull) = 0;

	virtual void shutdown(bool finishWorkInQueue, int shutdownSecs) = 0;

	virtual void waitForEmptyQueue() = 0;

	virtual ~OW_ThreadPoolImpl()
	{
	}

};

namespace {

class FixedSizePoolImpl;

/////////////////////////////////////////////////////////////////////////////
class FixedSizePoolWorkerThread : public OW_Thread
{
public:
	FixedSizePoolWorkerThread(FixedSizePoolImpl* thePool)
		: OW_Thread()
		, m_thePool(thePool)
	{
	}

	virtual OW_Int32 run();

private:
	FixedSizePoolImpl* m_thePool;
};

/////////////////////////////////////////////////////////////////////////////
class CommonPoolImpl : public OW_ThreadPoolImpl
{
protected:
	CommonPoolImpl(OW_UInt32 maxQueueSize)
		: m_maxQueueSize(maxQueueSize)
		, m_queueClosed(false)
		, m_shutdown(false)
	{
	}

	// assumes that m_queueLock is locked
	bool queueIsFull() const
	{
		return ((m_maxQueueSize > 0) && (m_queue.size() == m_maxQueueSize));
	}

	// assumes that m_queueLock is locked
	bool queueClosed() const
	{
		return m_shutdown || m_queueClosed;
	}

	bool finishOffWorkInQueue(bool finishWorkInQueue, int shutdownSecs)
	{
		OW_NonRecursiveMutexLock l(m_queueLock);
		// the pool is in the process of being destroyed
		if (queueClosed())
		{
			return false;
		}

		m_queueClosed = true;
		
		if (finishWorkInQueue)
		{
			while (m_queue.size() != 0)
			{
				if (shutdownSecs < 0)
				{
					m_queueEmpty.wait(l);
				}
				else
				{
					if (!m_queueEmpty.timedWait(l, shutdownSecs))
						break; // timed out
				}
			}
		}

		m_shutdown = true;
		return true;
	}

	virtual void waitForEmptyQueue()
	{
		OW_NonRecursiveMutexLock l(m_queueLock);
		while (m_queue.size() != 0)
		{
			m_queueEmpty.wait(l);
		}
	}

	void shutdownThreads(int shutdownSecs)
	{
		if (shutdownSecs >= 0)
		{
			// Set cooperative thread cancellation flag
			for (OW_UInt32 i = 0; i < m_threads.size(); ++i)
			{
				m_threads[i]->cooperativeCancel();
			}

			// If any still haven't shut down, kill them.
			for (OW_UInt32 i = 0; i < m_threads.size(); ++i)
			{
				m_threads[i]->definitiveCancel(shutdownSecs);
			}
		}

		// Clean up after the threads and/or wait for them to exit.
		for (OW_UInt32 i = 0; i < m_threads.size(); ++i)
		{
			m_threads[i]->join();
		}
	}

	OW_RunnableRef getWorkFromQueue(bool waitForWork)
	{
		OW_NonRecursiveMutexLock l(m_queueLock);
		while ((m_queue.size() == 0) && (!m_shutdown))
		{
			if (waitForWork)
			{
				m_queueNotEmpty.wait(l);
			}
			else
			{
				return OW_RunnableRef();
			}
		}

		// check to see if a shutdown started while the thread was sleeping
		if (m_shutdown)
		{
			return OW_RunnableRef();
		}

		OW_RunnableRef work = m_queue.front();
		m_queue.pop_front();

		// handle threads waiting in addWork()
		if (m_queue.size() == (m_maxQueueSize - 1))
		{
			m_queueNotFull.notifyAll();
		}

		// handle waiting shutdown thread or callers of waitForEmptyQueue()
		if (m_queue.size() == 0)
		{
			m_queueEmpty.notifyAll();
		}

		return work;
	}

	// pool characteristics
	OW_UInt32 m_maxQueueSize;

	// pool state
	OW_Array<OW_ThreadRef> m_threads;
	std::deque<OW_RunnableRef> m_queue;
	bool m_queueClosed;

	bool m_shutdown;

	// pool synchronization
	OW_NonRecursiveMutex m_queueLock;
	OW_Condition m_queueNotFull;
	OW_Condition m_queueEmpty;
	OW_Condition m_queueNotEmpty;

};

class FixedSizePoolImpl : public CommonPoolImpl
{
public:
	FixedSizePoolImpl(OW_UInt32 numThreads, OW_UInt32 maxQueueSize)
		: CommonPoolImpl(maxQueueSize)
	{
		// create the threads and start them up.
		m_threads.reserve(numThreads);
		for (OW_UInt32 i = 0; i < numThreads; ++i)
		{
			m_threads.push_back(OW_ThreadRef(new FixedSizePoolWorkerThread(this)));
		}
		for (OW_UInt32 i = 0; i < numThreads; ++i)
		{
			m_threads[i]->start();
		}
	}

	// returns true if work is placed in the queue to be run and false if not.
	virtual bool addWork(const OW_RunnableRef& work, bool blockWhenFull)
	{
		// check precondition: work != NULL
		if (!work)
		{
			return false;
		}

		OW_NonRecursiveMutexLock l(m_queueLock);
		if (!blockWhenFull && queueIsFull())
		{
			return false;
		}

		while( queueIsFull() && !queueClosed() )
		{
			m_queueNotFull.wait(l);
		}

		// the pool is in the process of being destroyed
		if (queueClosed()) 
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

	virtual void shutdown(bool finishWorkInQueue, int shutdownSecs)
	{
		if (!finishOffWorkInQueue(finishWorkInQueue, shutdownSecs))
		{
			return;
		}

		// Wake up any workers so they recheck shutdown flag
		m_queueNotEmpty.notifyAll();
		m_queueNotFull.notifyAll();

		shutdownThreads(shutdownSecs);
	}

	virtual ~FixedSizePoolImpl()
	{
		// can't let exception escape the destructor
		try
		{
			// Make sure the pool is shutdown.
			shutdown(false, 1);
		}
		catch (...)
		{
		}
	}

private:

	friend class FixedSizePoolWorkerThread;
};

void runRunnable(const OW_RunnableRef& work)
{
	// don't let exceptions escape, we need to keep going, except for OW_ThreadCancelledException, in which case we need to stop.
	try
	{
		work->run();
	}
	catch (OW_ThreadCancelledException&)
	{
		throw;
	}
	catch (OW_Exception& ex)
	{
#ifdef OW_DEBUG		
		std::cerr << "!!! Exception: " << ex.type() << " caught in OW_ThreadPool worker: " << ex << std::endl;
#endif
	}
	catch (...)
	{
#ifdef OW_DEBUG		
		std::cerr << "!!! Unknown Exception caught in OW_ThreadPool worker" << std::endl;
#endif
	}
}

OW_Int32 FixedSizePoolWorkerThread::run()
{
	while (true)
	{
		// check queue for work
		OW_RunnableRef work = m_thePool->getWorkFromQueue(true);
		if (!work)
		{
			return 0;
		}

		runRunnable(work);
	}
	return 0;
}

class DynamicSizePoolImpl;

/////////////////////////////////////////////////////////////////////////////
class DynamicSizePoolWorkerThread : public OW_Thread
{
public:
	DynamicSizePoolWorkerThread(DynamicSizePoolImpl* thePool)
		: OW_Thread()
		, m_thePool(thePool)
	{
	}

	virtual OW_Int32 run();

private:
	DynamicSizePoolImpl* m_thePool;
};

/////////////////////////////////////////////////////////////////////////////
class DynamicSizePoolImpl : public CommonPoolImpl
{
public:
	DynamicSizePoolImpl(OW_UInt32 maxThreads, OW_UInt32 maxQueueSize)
		: CommonPoolImpl(maxQueueSize)
		, m_maxThreads(maxThreads)
	{
	}

	// returns true if work is placed in the queue to be run and false if not.
	virtual bool addWork(const OW_RunnableRef& work, bool blockWhenFull)
	{
		// check precondition: work != NULL
		if (!work)
		{
			return false;
		}

		OW_NonRecursiveMutexLock l(m_queueLock);
		if (!blockWhenFull && queueIsFull())
		{
			return false;
		}

		while( queueIsFull() && !queueClosed() )
		{
			m_queueNotFull.wait(l);
		}

		// the pool is in the process of being destroyed
		if (queueClosed())
		{
			return false;
		}

		m_queue.push_back(work);
		
		// clean up dead threads (before we add the new one, so we don't need to check it)
		for (size_t i = 0; i < m_threads.size(); ++i)
		{
			if (!m_threads[i]->isRunning())
			{
				m_threads[i]->join();
				m_threads.remove(i--);
			}
		}

		// Start up a new thread to handle the work in the queue.
		if (m_threads.size() < m_maxThreads)
		{
			OW_ThreadRef theThread(new DynamicSizePoolWorkerThread(this));
			m_threads.push_back(theThread);
			theThread->start();
		}

		return true;
	}

	virtual void shutdown(bool finishWorkInQueue, int shutdownSecs)
	{
		if (!finishOffWorkInQueue(finishWorkInQueue, shutdownSecs))
		{
			return;
		}

		// Wake up any workers so they recheck shutdown flag
		m_queueNotFull.notifyAll();

		shutdownThreads(shutdownSecs);
	}

	virtual ~DynamicSizePoolImpl()
	{
		// can't let exception escape the destructor
		try
		{
			// Make sure the pool is shutdown.
			shutdown(false, 1);
		}
		catch (...)
		{
		}
	}

private:
	// pool characteristics
	OW_UInt32 m_maxThreads;

	friend class DynamicSizePoolWorkerThread;
};

OW_Int32 DynamicSizePoolWorkerThread::run()
{
	while (true)
	{
		// check queue for work
		OW_RunnableRef work = m_thePool->getWorkFromQueue(false);
		if (!work)
		{
			return 0;
		}

		runRunnable(work);
	}
	return 0;
}


} // end anonymous namespace

/////////////////////////////////////////////////////////////////////////////
OW_ThreadPool::OW_ThreadPool(PoolType poolType, OW_UInt32 numThreads, OW_UInt32 maxQueueSize)
{
	switch (poolType)
	{
		case FIXED_SIZE:
			m_impl = new FixedSizePoolImpl(numThreads, maxQueueSize);
			break;
		case DYNAMIC_SIZE:
			m_impl = new DynamicSizePoolImpl(numThreads, maxQueueSize);
			break;
	}
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
void OW_ThreadPool::shutdown(bool finishWorkInQueue, int shutdownSecs)
{
	m_impl->shutdown(finishWorkInQueue, shutdownSecs);
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

