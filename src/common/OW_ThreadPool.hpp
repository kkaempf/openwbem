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
#include "OW_Types.h"

#include "OW_Reference.hpp"
#include "OW_Runnable.hpp"

class OW_ThreadPoolImpl;

DECLARE_EXCEPTION(ThreadPool);

/**
 * The OW_ThreadPool class is used to coordinate a group of threads.
 * This class is freely copyable.  All copies reference the same underlying implementation.
 */
class OW_ThreadPool
{
public:
	/**
	 * Constructor
	 * @throw OW_ThreadPoolException if the underlying implementation fails.
	 */
	OW_ThreadPool(OW_UInt32 numThreads, OW_UInt32 maxQueueSize);

	// will block if queue is full.  return true if added to the queue, false if not, which will only happen if the pool is shutting down.
	bool addWork(const OW_RunnableRef& work);

	// will not block if queue is full.  return true if added to the queue, false if not.
	bool tryAddWork(const OW_RunnableRef& work);

	void shutdown(bool finishWorkInQueue=true);

	~OW_ThreadPool();
	OW_ThreadPool(const OW_ThreadPool& x);
	OW_ThreadPool& operator=(const OW_ThreadPool& x);
private:
	OW_Reference<OW_ThreadPoolImpl> m_impl;
};


#endif
