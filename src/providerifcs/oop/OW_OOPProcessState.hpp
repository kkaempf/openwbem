/*******************************************************************************
* Copyright (C) 2007 Quest Software All rights reserved.
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
*  - Neither the name of Quest Software nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software OR THE CONTRIBUTORS
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

#ifndef OW_OOP_PROCESS_STATE_HPP_INCLUDE_GUARD_
#define OW_OOP_PROCESS_STATE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "blocxx/String.hpp"
#include "blocxx/RWLocker.hpp"
#include "blocxx/IntrusiveReference.hpp"
#include "blocxx/ThreadSafeProcess.hpp"
#include "blocxx/NonRecursiveMutex.hpp"
#include "blocxx/NonRecursiveMutexLock.hpp"

namespace OW_NAMESPACE
{

class OOPProcessState
{
private:
	struct Data : public IntrusiveCountableBase
	{
		RWLocker m_guard;
		ThreadSafeProcessRef m_persistentProcess;
		String m_persistentProcessUserName;

		mutable NonRecursiveMutex m_objectLock;
	};

public:
	OOPProcessState()
	: m_data(new Data)
	{
	}

	enum PSNULL
	{
		E_PSNULL
	};

	OOPProcessState(PSNULL)
	{
	}

	bool isNull() const
	{
		return !m_data;
	}

	ThreadSafeProcessRef getProcess() const
	{
		NonRecursiveMutexLock l(m_data->m_objectLock);
		return m_data->m_persistentProcess;
	}

	void getProcessAndUserName(ThreadSafeProcessRef& proc, String& userName) const
	{
		NonRecursiveMutexLock l(m_data->m_objectLock);
		proc = m_data->m_persistentProcess;
		userName = m_data->m_persistentProcessUserName;
	}

	void setProcessAndUserName(const ThreadSafeProcessRef& persistentProcess, const String& userName)
	{
		NonRecursiveMutexLock l(m_data->m_objectLock);
		m_data->m_persistentProcess = persistentProcess;
		m_data->m_persistentProcessUserName = userName;
	}

	void clearProcess()
	{
		NonRecursiveMutexLock l(m_data->m_objectLock);
		m_data->m_persistentProcess = ThreadSafeProcessRef();
		m_data->m_persistentProcessUserName = String();
	}

	RWLocker& getGuard()
	{
		NonRecursiveMutexLock l(m_data->m_objectLock);
		return m_data->m_guard;
	}

private:
	IntrusiveReference<Data> m_data;
};


} // end namespace OW_NAMESPACE


#endif


