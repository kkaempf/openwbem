/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
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

#ifndef _OW_SEMAPHORE_HPP__
#define _OW_SEMAPHORE_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Mutex.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_Bool.hpp"

class OW_Semaphore
{
public:
	OW_Semaphore(OW_Int32 initCount)
		: m_curCount(initCount)
	{
			OW_MutexImpl::createMutex(m_mutex);
			OW_SemaphoreImpl::createConditionVar(m_cond);
	}


	~OW_Semaphore()
	{
	}

	OW_Bool wait(OW_UInt32 ms=0)
	{
		return OW_SemaphoreImpl::wait(m_cond, m_mutex, m_curCount, ms);
	}

	void signal()
	{
		OW_SemaphoreImpl::signal(m_cond, m_mutex, m_curCount);
	}

	OW_Int32 getCount()
	{
		return OW_SemaphoreImpl::getCount(m_mutex, m_curCount);
	}

private:
	OW_Int32 m_curCount;

	OW_ConditionVar_t m_cond;
	OW_Mutex_t m_mutex;
};

#endif  // _OW_SEMAPHORE_HPP__
