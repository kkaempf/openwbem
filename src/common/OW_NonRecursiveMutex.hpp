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

#ifndef OW_NON_RECURSIVE_MUTEX_INCLUDE_GUARD_HPP_
#define OW_NON_RECURSIVE_MUTEX_INCLUDE_GUARD_HPP_
#include "OW_config.h"
#include "OW_ThreadTypes.hpp"
#include "OW_Exception.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(Deadlock, OW_COMMON_API)
class OW_COMMON_API NonRecursiveMutex
{
public:
	/**
	 * Create a new NonRecursiveMutex object.
	 */
	NonRecursiveMutex();
	/**
	 * Destroy this NonRecursiveMutex object.
	 */
	~NonRecursiveMutex();
	/**
	 * Acquire ownership of this NonRecursiveMutex object.
	 * This call will block if another thread has
	 * ownership of this NonRecursiveMutex. When it returns, the current thread will be
	 * the owner of this NonRecursiveMutex object.
	 * If this thread is the owner of the mutex, then an Deadlock 
	 * exception will be thrown.
	 */
	void acquire();
	/**
	 * Release ownership of this NonRecursiveMutex object. If another thread is waiting
	 * to acquire the ownership of this mutex it will stop blocking and acquire
	 * ownership when this call returns.
	 */
	bool release();
private:
	NonRecursiveMutex_t m_mutex;
	// noncopyable
	NonRecursiveMutex(const NonRecursiveMutex&);
	NonRecursiveMutex& operator = (const NonRecursiveMutex&);
	friend class Condition;
	void conditionPreWait(NonRecursiveMutexLockState& state);
	void conditionPostWait(NonRecursiveMutexLockState& state);
};

} // end namespace OW_NAMESPACE

#endif
