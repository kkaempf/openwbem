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
#include "OW_config.h"
#include "OW_ThreadCounter.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Assertion.hpp"
#include "OW_TimeoutException.hpp"

OW_ThreadCounter::OW_ThreadCounter(OW_Int32 maxThreads)
	: m_maxThreads(maxThreads)
	, m_runCount(0)
{}

OW_ThreadCounter::~OW_ThreadCounter()
{}

void
OW_ThreadCounter::incThreadCount(OW_UInt32 sTimeout, OW_UInt32 usTimeout)
{
	OW_NonRecursiveMutexLock l(m_runCountGuard);
	while (m_runCount >= m_maxThreads)
	{
		if (!m_runCountCondition.timedWait(l, sTimeout, usTimeout))
			OW_THROW(OW_TimeoutException, "OW_ThreadCounter::incThreadCount timedout");
	}
	++m_runCount;
}

void
OW_ThreadCounter::decThreadCount()
{
	OW_NonRecursiveMutexLock l(m_runCountGuard);
	OW_ASSERT(m_runCount > 0);
	--m_runCount;
	m_runCountCondition.notifyAll();
}

OW_Int32
OW_ThreadCounter::getThreadCount()
{
	OW_NonRecursiveMutexLock l(m_runCountGuard);
	return m_runCount;
}

void
OW_ThreadCounter::waitForAll(OW_UInt32 sTimeout, OW_UInt32 usTimeout)
{
	OW_NonRecursiveMutexLock runCountLock(m_runCountGuard);
	while(m_runCount > 0)
	{
		if (!m_runCountCondition.timedWait(runCountLock, sTimeout, usTimeout))
			OW_THROW(OW_TimeoutException, "OW_ThreadCounter::waitForAll timedout");
	}
}

void
OW_ThreadCounter::setMax(OW_Int32 maxThreads)
{
	OW_NonRecursiveMutexLock runCountLock(m_runCountGuard);
	m_maxThreads = maxThreads;
	m_runCountCondition.notifyAll();
}

OW_ThreadCountDecrementer::OW_ThreadCountDecrementer(OW_ThreadCounterRef const& x)
	: m_counter(x)
{}

OW_ThreadCountDecrementer::~OW_ThreadCountDecrementer()
{}

void
OW_ThreadCountDecrementer::doNotifyThreadDone(OW_Thread *)
{
	m_counter->decThreadCount();
}

