/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#ifndef OW_THREAD_POOL_HPP_INCLUDE_GUARD_
#define OW_THREAD_POOL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_Reference.hpp"
#include "OW_Runnable.hpp"
#include "OW_Exception.hpp"
#include "OW_Logger.hpp"

namespace OpenWBEM
{

class ThreadPoolImpl;
OW_DECLARE_EXCEPTION(ThreadPool);
/**
 * The ThreadPool class is used to coordinate a group of threads.  There is
 * a queue maintained of work to do.  As each thread in the group is available
 * it will get it's next task from the head of the queue.  The queue can have
 * a maximum size limit or can be of unlimited size.
 *
 * This class is freely copyable.  All copies reference the same underlying implementation.
 * This class is thread safe.
 */
class ThreadPool
{
public:
	enum PoolType
	{
		FIXED_SIZE,
		DYNAMIC_SIZE
	};
	/**
	 * Constructor
	 *
	 * @param poolType The type of pool, either FIXED_SIZE or DYNAMIC_SIZE
	 * FIXED_SIZE - numThreads threads will be created at instantiation time.
	 *  No threads are created or destroyed until the pool is shutdown.
	 * DYNAMIC_SIZE - Threads will be created as work is added.  The number
	 *  of threads will always be less than numThreads. Threads exit when no
	 *  more work is available in the queue.
	 *
	 * @param numThreads The number of threads in the pool.
	 *
	 * @param maxQueueSize The upper bound on the size of the queue.  0 means
	 *  no limit.
	 *
	 * @param logger used to log messages if non-NULL.
	 *
	 * @param poolName incorporated into log messages to identify the pool.
	 *
	 * @throw ThreadPoolException if the underlying implementation fails.
	 */
	ThreadPool(PoolType poolType, UInt32 numThreads, UInt32 maxQueueSize, const LoggerRef& logger=LoggerRef(), const String& poolName="");
	/**
	 * Add an RunnableRef for the pool to execute.
	 * If the queue is full, this call will block until there is space in the queue.
	 * @return true if added to the queue, false if not, which will only happen if the pool is shutting down.
	 */
	bool addWork(const RunnableRef& work);
	/**
	 * Add an RunnableRef for the pool to execute.
	 * If the queue is full, this call will *not* block.
	 * @return true if added to the queue, false if not.
	 */
	bool tryAddWork(const RunnableRef& work);
	enum EShutdownQueueFlag
	{
		E_DISCARD_WORK_IN_QUEUE,
		E_FINISH_WORK_IN_QUEUE
	};
	/**
	 * Instruct all threads to exit and stop working.  After shutdown() is
	 * called, addWork() and tryAddWork() will return false.
	 *
	 * @param finishWorkInQueue If E_FINISH_WORK_IN_QUEUE, threads will continue to process the
	 *  current work in the queue, before shutting down.  If E_DISCARD_WORK_IN_QUEUE, the work in
	 *  the queue will be discarded.
	 *
	 * @param timeoutSecs The number of seconds to wait for the threads to finish
	 *  their work (and possibly the work in the queue) before attempting to
	 *  cancel the threads.  If timeoutSecs < 0, the timeout will be unlimited,
	 *  the threads will not be cancelled, and shutdown() will return once
	 *  all the threads have exited (which may never happen if one or more
	 *  of the threads are deadlocked).
	 */
	void shutdown(EShutdownQueueFlag finishWorkInQueue = E_FINISH_WORK_IN_QUEUE, int timeoutSecs=-1);
	/**
	 * Wait for the queue to empty out.
	 */
	void waitForEmptyQueue();
	/**
	 * Destructor.  This will call shutdown(E_DISCARD_WORK_IN_QUEUE, 1) when the last copy is
	 * destroyed
	 */
	~ThreadPool();
	ThreadPool(const ThreadPool& x);
	ThreadPool& operator=(const ThreadPool& x);
private:
	Reference<ThreadPoolImpl> m_impl;
};
typedef Reference<ThreadPool> ThreadPoolRef;

} // end namespace OpenWBEM

#endif
