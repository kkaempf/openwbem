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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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
#include "OW_MutexLock.hpp"

#include <cassert>
#include <cerrno>

DEFINE_EXCEPTION(ConditionLock);
DEFINE_EXCEPTION(ConditionResource);

/////////////////////////////////////////////////////////////////////////////
OW_Condition::OW_Condition()
{
	#ifdef OW_USE_GNU_PTH
	int res = pth_cond_init(&m_condition, 0);
	#else
	int res = pthread_cond_init(&m_condition, 0);
	#endif
	if (res != 0)
	{
		OW_THROW(OW_ConditionResourceException, "Failed initializing condition variable");
	}
}

/////////////////////////////////////////////////////////////////////////////
OW_Condition::~OW_Condition()
{
	#ifdef OW_USE_GNU_PTH
	int res = pth_cond_destroy(&m_condition);
	#else
	int res = pthread_cond_destroy(&m_condition);
	#endif
	assert(res == 0);
}

/////////////////////////////////////////////////////////////////////////////
void 
OW_Condition::notifyOne()
{
	#ifdef OW_USE_GNU_PTH
	int res = pth_cond_signal(&m_condition);
	#else
	int res = pthread_cond_signal(&m_condition);
	#endif
	assert(res == 0);
}

/////////////////////////////////////////////////////////////////////////////
void 
OW_Condition::notifyAll()
{
	#ifdef OW_USE_GNU_PTH
	int res = pth_cond_broadcast(&m_condition);
	#else
	int res = pthread_cond_broadcast(&m_condition);
	#endif
	assert(res == 0);
}

/////////////////////////////////////////////////////////////////////////////
void 
OW_Condition::wait(OW_MutexLock& lock)
{
	if (!lock.isLocked())
	{
		OW_THROW(OW_ConditionLockException, "Lock must be locked");
	}
	doWait(*(lock.m_mutex));
}

/////////////////////////////////////////////////////////////////////////////
bool 
OW_Condition::timedWait(OW_MutexLock& lock, OW_UInt32 sTimeout, OW_UInt32 usTimeout)
{
	if (!lock.isLocked())
	{
		OW_THROW(OW_ConditionLockException, "Lock must be locked");
	}
	return doTimedWait(*(lock.m_mutex), sTimeout, usTimeout);
}

/////////////////////////////////////////////////////////////////////////////
void 
OW_Condition::doWait(OW_Mutex& mutex)
{
	int res;
	OW_MutexLockState state;
	mutex.conditionPreWait(state);
	#ifdef OW_USE_GNU_PTH
	res = pth_cond_wait(&m_condition, state.pmutex);
	#else
	res = pthread_cond_wait(&m_condition, state.pmutex);
	#endif
	mutex.conditionPostWait(state);
	assert(res == 0);
}

/////////////////////////////////////////////////////////////////////////////
bool 
OW_Condition::doTimedWait(OW_Mutex& mutex, OW_UInt32 sTimeout, OW_UInt32 usTimeout)
{
	int res;
	OW_MutexLockState state;
	mutex.conditionPreWait(state);
	bool ret = false;
	timespec ts;
	ts.tv_sec = time(NULL) + sTimeout;
	ts.tv_nsec = usTimeout;
	#ifdef OW_USE_GNU_PTH
	res = pth_cond_timedwait(&m_condition, state.pmutex);
	#else
	res = pthread_cond_timedwait(&m_condition, state.pmutex, &ts);
	#endif
	mutex.conditionPostWait(state);
	assert(res == 0 || res == ETIMEDOUT);
	ret = res != ETIMEDOUT;
	return ret;
}



