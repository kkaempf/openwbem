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

#ifndef OW_THREAD_BARRIER_HPP_INCLUDE_GUARD_
#define OW_THREAD_BARRIER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

class ThreadBarrierImpl;
OW_DECLARE_EXCEPTION(ThreadBarrier);
/**
 * The ThreadBarrier class is used to synchronize threads.  Each thread that calls wait() will block until <i>threshold</i> number of threads has called wait()
 * This class is freely copyable.  All copies reference the same underlying implementation.
 */
class ThreadBarrier
{
public:
	/**
	 * Constructor
	 * @param threshold The number of threads that must call wait() before any of them successfully return from the call. The value specified by threshold must be greater than zero.
	 * @throw ThreadBarrierException if the underlying implementation fails.
	 */
	ThreadBarrier(UInt32 threshold);
	/**
	 * Synchronize participating threads at the barrier. The calling thread shall block until the required number of threads have called wait().
	 * @throw ThreadBarrierException if the underlying implementation fails.
	 */
	void wait();
	~ThreadBarrier();
	ThreadBarrier(const ThreadBarrier& x);
	ThreadBarrier& operator=(const ThreadBarrier& x);
private:
	IntrusiveReference<ThreadBarrierImpl> m_impl;
};

} // end namespace OpenWBEM

#endif
