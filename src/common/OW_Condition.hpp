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
#include "OW_CommonFwd.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(ConditionLock, OW_COMMON_API);
OW_DECLARE_APIEXCEPTION(ConditionResource, OW_COMMON_API);
class OW_COMMON_API Condition
{
public:
	Condition();
	~Condition();
	/**
	 * Wake up one thread that is blocked waiting on the condition.
	 */
	void notifyOne();
	/**
	 * Wake up all threads that are blocked waiting on the condition.
	 */
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
	 * Atomically unlocks the mutex and waits for the condition variable
	 * to be notified. The thread execution is suspended and does not
	 * consume any CPU time until the condition variable is notified
	 * or the timeout has expired.
	 * The mutex lock must  be locked  by the calling thread on entrance
	 * to wait. Before returning to the calling thread, wait re-acquires
	 * the mutex lock.
	 * This function should always be called within a while loop that
	 * checks the condition.
	 * @param timeout The time to wait for the condition to be signalled. 
	 *  Because of spurious wakeups, it is recommended that an absolute 
	 *  time timeout is used.
	 * @returns true if condition was signaled within the duration, 
	 *  false if timeout occurred.
	 */
	bool timedWait(NonRecursiveMutexLock& lock, const Timeout& timeout);
	bool timedWait(NonRecursiveMutexLock& lock, UInt32 sTimeout, UInt32 usTimeout=0) OW_DEPRECATED;
private:
	// unimplemented
	Condition(const Condition&);
	Condition& operator=(const Condition&);
	void doWait(NonRecursiveMutex& mutex);
	bool doTimedWait(NonRecursiveMutex& mutex, const Timeout& timeout);
	ConditionVar_t m_condition;
};

} // end namespace OW_NAMESPACE

#endif
