/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_Condition.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include <cassert>
#include <cerrno>

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION(ConditionLock);
OW_DEFINE_EXCEPTION(ConditionResource);
#if defined(OW_USE_PTHREAD)
/////////////////////////////////////////////////////////////////////////////
Condition::Condition()
{
	int res = pthread_cond_init(&m_condition, 0);
	if (res != 0)
	{
		OW_THROW(ConditionResourceException, "Failed initializing condition variable");
	}
}
/////////////////////////////////////////////////////////////////////////////
Condition::~Condition()
{
	int res = pthread_cond_destroy(&m_condition);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void 
Condition::notifyOne()
{
	int res = pthread_cond_signal(&m_condition);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void 
Condition::notifyAll()
{
	int res = pthread_cond_broadcast(&m_condition);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void 
Condition::doWait(NonRecursiveMutex& mutex)
{
	int res;
	NonRecursiveMutexLockState state;
	mutex.conditionPreWait(state);
	res = pthread_cond_wait(&m_condition, state.pmutex);
	mutex.conditionPostWait(state);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
bool 
Condition::doTimedWait(NonRecursiveMutex& mutex, UInt32 sTimeout, UInt32 usTimeout)
{
	int res;
	NonRecursiveMutexLockState state;
	mutex.conditionPreWait(state);
	bool ret = false;
	timespec ts;
	ts.tv_sec = time(NULL) + sTimeout;
	ts.tv_nsec = usTimeout * 1000;
	res = pthread_cond_timedwait(&m_condition, state.pmutex, &ts);
	mutex.conditionPostWait(state);
	assert(res == 0 || res == ETIMEDOUT);
	ret = res != ETIMEDOUT;
	return ret;
}
#elif defined (OW_USE_WIN32_THREADS)
/////////////////////////////////////////////////////////////////////////////
Condition::Condition()
{
	//if (res != 0)
	{
		OW_THROW(ConditionResourceException, "Failed initializing condition variable");
	}
}
/////////////////////////////////////////////////////////////////////////////
Condition::~Condition()
{
	//assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void 
Condition::notifyOne()
{
//	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void 
Condition::notifyAll()
{
//	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
void 
Condition::doWait(NonRecursiveMutex& mutex)
{
	int res;
	NonRecursiveMutexLockState state;
	mutex.conditionPreWait(state);
	mutex.conditionPostWait(state);
	assert(res == 0);
}
/////////////////////////////////////////////////////////////////////////////
bool 
Condition::doTimedWait(NonRecursiveMutex& mutex, UInt32 sTimeout, UInt32 usTimeout)
{
	NonRecursiveMutexLockState state;
	mutex.conditionPreWait(state);
	bool ret = false;
	mutex.conditionPostWait(state);
	return ret;
}
#else
#error "port me!"
#endif
/////////////////////////////////////////////////////////////////////////////
void 
Condition::wait(NonRecursiveMutexLock& lock)
{
	if (!lock.isLocked())
	{
		OW_THROW(ConditionLockException, "Lock must be locked");
	}
	doWait(*(lock.m_mutex));
}
/////////////////////////////////////////////////////////////////////////////
bool 
Condition::timedWait(NonRecursiveMutexLock& lock, UInt32 sTimeout, UInt32 usTimeout)
{
	if (!lock.isLocked())
	{
		OW_THROW(ConditionLockException, "Lock must be locked");
	}
	return doTimedWait(*(lock.m_mutex), sTimeout, usTimeout);
}

} // end namespace OpenWBEM

