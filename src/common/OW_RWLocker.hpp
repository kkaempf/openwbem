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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_RWLOCKER_HPP_INCLUDE_GUARD_
#define OW_RWLOCKER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Condition.hpp"
#include "OW_Exception.hpp"
#include "OW_Array.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(RWLocker, OW_COMMON_API);
//////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API RWLocker
{
public:
	RWLocker();
	~RWLocker();
	void getReadLock(UInt32 sTimeout, UInt32 usTimeout=0);
	void getWriteLock(UInt32 sTimeout, UInt32 usTimeout=0);
	void releaseReadLock();
	void releaseWriteLock();
private:
	void doWakeups();
	Condition   m_waiting_writers;
	Condition   m_waiting_readers;
	
	int         m_num_waiting_writers;
	int         m_num_waiting_readers;
	int			m_readers_next;
	
	NonRecursiveMutex	m_guard;
	// -1 means writer has lock.  0 means no one has the lock. 
	// > 0 means readers have the lock.
	int m_state;

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	Array<Thread_t> m_readers;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	Thread_t m_writer;
};
//////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API ReadLock
{
public:
	ReadLock(RWLocker& locker, UInt32 sTimeout, UInt32 usTimeout=0)
		: m_locker(&locker)
		, m_released(false)
	{
		m_locker->getReadLock(sTimeout, usTimeout);
	}
	~ReadLock()
	{
		release();
	}
	void lock(UInt32 sTimeout, UInt32 usTimeout=0)
	{
		if (m_released)
		{
			m_locker->getReadLock(sTimeout, usTimeout);
			m_released = false;
		}
	}
	void release()
	{
		if (!m_released)
		{
			m_locker->releaseReadLock();
			m_released = true;
		}
	}
private:
	RWLocker* m_locker;
	bool m_released;
	// noncopyable
	ReadLock(const ReadLock&);
	ReadLock& operator=(const ReadLock&);
};
//////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API WriteLock
{
public:
	WriteLock(RWLocker& locker, UInt32 sTimeout, UInt32 usTimeout=0)
		: m_locker(&locker)
		, m_released(false)
	{
		m_locker->getWriteLock(sTimeout, usTimeout);
	}
	~WriteLock()
	{
		release();
	}
	void lock(UInt32 sTimeout, UInt32 usTimeout=0)
	{
		if (m_released)
		{
			m_locker->getWriteLock(sTimeout, usTimeout);
			m_released = false;
		}
	}
	void release()
	{
		if (!m_released)
		{
			m_locker->releaseWriteLock();
			m_released = true;
		}
	}
private:
	RWLocker* m_locker;
	bool m_released;
	
	// noncopyable
	WriteLock(const WriteLock&);
	WriteLock& operator=(const WriteLock&);
};

} // end namespace OW_NAMESPACE

#endif
