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

#ifndef OW_THREAD_COUNTER_HPP_INCLUDE_GUARD_
#define OW_THREAD_COUNTER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_Condition.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_ThreadDoneCallback.hpp"

namespace OpenWBEM
{

// Note: Do not inline any functions in these classes, the code must
// be contained in the main library, if a loadable library contains any,
// it will cause a race-condition that may segfault the cimom.
class OW_COMMON_API ThreadCounter : public IntrusiveCountableBase
{
public:
	ThreadCounter(Int32 maxThreads);
	~ThreadCounter();
	// Throws TimeoutException in case of timeout
	void incThreadCount(UInt32 sTimeout, UInt32 usTimeout);
	void decThreadCount();
	Int32 getThreadCount();
	// Throws TimeoutException in case of timeout
	void waitForAll(UInt32 sTimeout, UInt32 usTimeout);
	void setMax(Int32 maxThreads);
private:
	Int32 m_maxThreads;
	Int32 m_runCount;
	NonRecursiveMutex m_runCountGuard;
	Condition m_runCountCondition;
	// noncopyable
	ThreadCounter(ThreadCounter const&);
	ThreadCounter& operator=(ThreadCounter const&);
};

typedef IntrusiveReference<ThreadCounter> ThreadCounterRef;

class OW_COMMON_API ThreadCountDecrementer : public ThreadDoneCallback
{
public:
	ThreadCountDecrementer(ThreadCounterRef const& x);
	virtual ~ThreadCountDecrementer();
private:
	virtual void doNotifyThreadDone(Thread *);
	ThreadCounterRef m_counter;
	// noncopyable
	ThreadCountDecrementer(ThreadCountDecrementer const&);
	ThreadCountDecrementer& operator=(ThreadCountDecrementer const&);
};

} // end namespace OpenWBEM

#endif
