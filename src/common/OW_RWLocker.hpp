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

#ifndef OW_RWLOCKER_HPP_INCLUDE_GUARD_
#define OW_RWLOCKER_HPP_INCLUDE_GUARD_

#include "OW_config.h"

#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Condition.hpp"
#include "OW_Exception.hpp"
#include "OW_Array.hpp"

DECLARE_EXCEPTION(RWLocker);

//////////////////////////////////////////////////////////////////////////////
class OW_RWLocker
{
public:
	OW_RWLocker();
	~OW_RWLocker();

	void getReadLock(OW_UInt32 sTimeout, OW_UInt32 usTimeout=0);
	void getWriteLock(OW_UInt32 sTimeout, OW_UInt32 usTimeout=0);

	void releaseReadLock();
	void releaseWriteLock();

private:

	void doWakeups();

	OW_Condition   m_waiting_writers;
	OW_Condition   m_waiting_readers;
	
	int         m_num_waiting_writers;
	int         m_num_waiting_readers;
	int			m_readers_next;
	
	OW_NonRecursiveMutex	m_guard;

	// -1 means writer has lock.  0 means no one has the lock. 
	// > 0 means readers have the lock.
	int m_state;

	OW_Array<OW_Thread_t> m_readers;
	OW_Thread_t m_writer;
};

//////////////////////////////////////////////////////////////////////////////
class OW_ReadLock
{
public:
	OW_ReadLock(OW_RWLocker& locker, OW_UInt32 sTimeout, OW_UInt32 usTimeout=0)
		: m_locker(&locker)
		, m_released(false)
	{
		m_locker->getReadLock(sTimeout, usTimeout);
	}

	~OW_ReadLock()
	{
		release();
	}

	void lock(OW_UInt32 sTimeout, OW_UInt32 usTimeout=0)
	{
		if(m_released)
		{
			m_locker->getReadLock(sTimeout, usTimeout);
			m_released = false;
		}
	}

	void release()
	{
		if(!m_released)
		{
			m_locker->releaseReadLock();
			m_released = true;
		}
	}

private:
	OW_RWLocker* m_locker;
	bool m_released;

	// noncopyable
	OW_ReadLock(const OW_ReadLock&);
	OW_ReadLock& operator=(const OW_ReadLock&);
};

//////////////////////////////////////////////////////////////////////////////
class OW_WriteLock
{
public:
	OW_WriteLock(OW_RWLocker& locker, OW_UInt32 sTimeout, OW_UInt32 usTimeout=0)
		: m_locker(&locker)
		, m_released(false)
	{
		m_locker->getWriteLock(sTimeout, usTimeout);
	}

	~OW_WriteLock()
	{
		release();
	}

	void lock(OW_UInt32 sTimeout, OW_UInt32 usTimeout=0)
	{
		if(m_released)
		{
			m_locker->getWriteLock(sTimeout, usTimeout);
			m_released = false;
		}
	}

	void release()
	{
		if(!m_released)
		{
			m_locker->releaseWriteLock();
			m_released = true;
		}
	}

private:
	OW_RWLocker* m_locker;
	bool m_released;
	
	// noncopyable
	OW_WriteLock(const OW_WriteLock&);
	OW_WriteLock& operator=(const OW_WriteLock&);
};

#endif
