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

#ifndef OW_SEMAPHORE_HPP_INCLUDE_GUARD_
#define OW_SEMAPHORE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_Condition.hpp"
#include "OW_NonRecursiveMutexLock.hpp"

class OW_Semaphore
{
public:
	OW_Semaphore()
		: m_curCount(0)
	{}
	OW_Semaphore(OW_Int32 initCount)
		: m_curCount(initCount)
	{}

	void wait()
	{
		OW_NonRecursiveMutexLock l(m_mutex);

		while (m_curCount <= 0)
		{
			m_cond.wait(l);
		}

		--m_curCount;
	}

	bool timedWait(OW_UInt32 sTimeout, OW_UInt32 usTimeout=0)
	{
		bool ret = true;

		OW_NonRecursiveMutexLock l(m_mutex);

		while (m_curCount <= 0 && ret == true)
		{
			ret = m_cond.timedWait(l, sTimeout, usTimeout);
		}

		if (ret == true)
		{
			--m_curCount;
		}

		return ret;
	}

	void signal()
	{
		OW_NonRecursiveMutexLock l(m_mutex);
		++m_curCount;
		m_cond.notifyAll();
	}

	OW_Int32 getCount()
	{
		OW_NonRecursiveMutexLock l(m_mutex);
		return m_curCount;
	}

private:
	OW_Int32 m_curCount;

	OW_Condition m_cond;
	OW_NonRecursiveMutex m_mutex;

	// noncopyable
	OW_Semaphore(const OW_Semaphore&);
	OW_Semaphore& operator=(const OW_Semaphore&);

};

#endif

