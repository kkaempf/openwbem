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

#ifndef OW_CONDITION_HPP_INCLUDE_GUARD_
#define OW_CONDITION_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ThreadTypes.hpp"
#include "OW_Exception.hpp"
#include "OW_Types.hpp"

namespace OpenWBEM
{

class NonRecursiveMutexLock;
class NonRecursiveMutex;
OW_DECLARE_EXCEPTION(ConditionLock);
OW_DECLARE_EXCEPTION(ConditionResource);
class Condition
{
public:
	Condition();
	~Condition();
	void notifyOne();
	void notifyAll();
	/**
	 * Atomically unlocks the mutex and waits for the condition variable 
	 * to be notified. The thread execution is suspended and does not 
	 * consume any CPU time until the condition variable is notified. 
	 * The mutex lock must  be locked  by the calling thread on entrance 
	 * to wait. Before returning to the calling thread, wait re-acquires  
	 * the mutex lock.
	 * This function should always be called within a while loop that
	 * checks the condition.
	 */
	void wait(NonRecursiveMutexLock& lock);
	/**
	 * Acquire ownership of this Mutex object.
	 * This call will block if another thread has ownership of 
	 * this Mutex. When it returns, the current thread will be
	 * the owner of this Mutex object.
	 * @param sTimeout The number of seconds to wait for the mutex.
	 * @param usTimeout The number of micro seconds (1/1000000th) to wait
	 * The total wait time is sTimeout * 1000000 + usTimeout micro seconds.
	 * This function should always be called within a while loop that
	 * checks the condition.
	 * @returns true if the lock was acquired, false if timeout occurred.
	 */
	bool timedWait(NonRecursiveMutexLock& lock, UInt32 sTimeout, UInt32 usTimeout=0);
private:
	// unimplemented
	Condition(const Condition&);
	Condition& operator=(const Condition&);
	void doWait(NonRecursiveMutex& mutex);
	bool doTimedWait(NonRecursiveMutex& mutex, UInt32 sTimeout, UInt32 usTimeout);
	ConditionVar_t m_condition;
};

} // end namespace OpenWBEM

#endif
