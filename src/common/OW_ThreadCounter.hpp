/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
#ifndef OW_THREAD_COUNTER_HPP_INCLUDE_GUARD_
#define OW_THREAD_COUNTER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Mutex.hpp"
#include "OW_Condition.hpp"
#include "OW_Reference.hpp"
#include "OW_Thread.hpp"

// Note: Do not inline any functions in these classes, the code must
// be contained in the main library, if a loadable library contains any,
// it will cause a race-condition that may segfault the cimom.
class OW_ThreadCounter
{
public:
	OW_ThreadCounter(OW_Int32 maxThreads);
	~OW_ThreadCounter();

	void incThreadCount();
	void decThreadCount();
	OW_Int32 getThreadCount();
	void waitForAll();
	void setMax(OW_Int32 maxThreads);

private:
	OW_Int32 m_maxThreads;
	OW_Int32 m_runCount;
	OW_Mutex m_runCountGuard;
	OW_Condition m_runCountCondition;

	// noncopyable
	OW_ThreadCounter(OW_ThreadCounter const&);
	OW_ThreadCounter& operator=(OW_ThreadCounter const&);
};

typedef OW_Reference<OW_ThreadCounter> OW_ThreadCounterRef;

class OW_ThreadCountDecrementer : public OW_ThreadDoneCallback
{
public:
	OW_ThreadCountDecrementer(OW_ThreadCounterRef const& x);
	virtual ~OW_ThreadCountDecrementer();

private:
	virtual void doNotifyThreadDone(OW_Thread *);

	OW_ThreadCounterRef m_counter;

	// noncopyable
	OW_ThreadCountDecrementer(OW_ThreadCountDecrementer const&);
	OW_ThreadCountDecrementer& operator=(OW_ThreadCountDecrementer const&);
};

#endif


