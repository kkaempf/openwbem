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

#include "OW_config.h"
#include "OW_RWLocker.hpp"
#include "OW_Assertion.hpp"

using std::vector;

int OW_RWLocker::m_readLockCount = 0;
int OW_RWLocker::m_writeLockCount = 0;
OW_Mutex OW_RWLocker::m_countGuard;

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::incReadLockCount()
{
	OW_MutexLock ml(m_countGuard);
	OW_ASSERT(m_writeLockCount == 0);
	m_readLockCount++;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::decReadLockCount()
{
	OW_MutexLock ml(m_countGuard);
	m_readLockCount--;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::incWriteLockCount()
{
	OW_MutexLock ml(m_countGuard);
	OW_ASSERT(m_readLockCount == 0);
	m_writeLockCount++;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::decWriteLockCount()
{
	OW_MutexLock ml(m_countGuard);
	m_writeLockCount--;
}

//////////////////////////////////////////////////////////////////////////////
OW_RWLocker::OW_RWLocker() : m_guard(), m_queue(), m_state(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_RWLocker::~OW_RWLocker()
{
	try
	{
		OW_RWQEntryVect::iterator it = m_queue.begin();
		while(it != m_queue.end())
		{
			it->m_event.signal();
			it++;
		}
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_ReadLock
OW_RWLocker::getReadLock()
{
	OW_MutexLock lock(m_guard);

	if(m_state >= 0)		// True if a reader currently has the lock/queue empty
	{
		if(m_queue.empty())
		{
			m_state++;
			incReadLockCount();
			return OW_ReadLock(this);
		}
	}

	int tryCount = 0;
	while(true)
	{
		OW_RWQEntry qentry(false);

		if(tryCount == 0)
		{
			m_queue.push_back(qentry);		// Put reader in the queue
			tryCount++;
		}
		else
		{
			m_queue.insert(m_queue.begin(), qentry); // Put in front (someone got it first)
		}

		lock.release();
		qentry.m_event.waitForSignal(); 	// Will stop blocking when pulled from queue
		lock.lock();
		if(m_state >= 0)
		{
			break;
		}
	}

	m_state++;
	incReadLockCount();
	return OW_ReadLock(this);
}

//////////////////////////////////////////////////////////////////////////////
OW_WriteLock
OW_RWLocker::getWriteLock()
{
	int tryCount = 0;
	OW_MutexLock lock(m_guard);
	while(m_state != 0)
	{
		OW_RWQEntry qentry(true);

		if(tryCount > 0)
			m_queue.insert(m_queue.begin(), qentry); // Put in front (someone got it first)
		else
		{
			m_queue.push_back(qentry);		// Put writer in the queue
			tryCount++;
		}

		lock.release();
		qentry.m_event.waitForSignal(); 	// Will stop blocking when pulled from queue
		lock.lock();
	}

	m_state = -1;
	incWriteLockCount();
	return OW_WriteLock(this);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::releaseReadLock()
{
	OW_MutexLock lock(m_guard);
	--m_state;
	if(m_queue.size() > 0 && m_state == 0)
	{
		OW_RWQEntry qentry = m_queue.front();
		m_queue.erase(m_queue.begin());
		qentry.m_event.signal();
	}
	decReadLockCount();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::releaseWriteLock()
{
	OW_MutexLock lock(m_guard);
	m_state = 0;

	if(m_queue.size() > 0)
	{
		OW_RWQEntry qentry = m_queue.front();
		while(m_queue.size() > 0)
		{
			m_queue.erase(m_queue.begin());
			qentry.m_event.signal();

			if(qentry.m_isWriter)
				break;

			qentry = m_queue.front();
			if(qentry.m_isWriter)
				break;
		}
	}

	decWriteLockCount();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ReadLock::release()
{
	if(!m_pdata.isNull())
	{
		if(!m_pdata->m_released)
		{
			m_pdata->m_locker->releaseReadLock();
			m_pdata->m_released = true;
		}
	}
}

OW_ReadLock::RLData::~RLData()
{
	try
	{
		if(!m_released)
			m_locker->releaseReadLock();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

OW_ReadLock::RLData&
OW_ReadLock::RLData::operator= (const OW_ReadLock::RLData& x)
{
	m_locker = x.m_locker;
	m_released = x.m_released;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WriteLock::release()
{
	if(!m_pdata.isNull())
	{
		if(!m_pdata->m_released)
		{
			m_pdata->m_locker->releaseWriteLock();
			m_pdata->m_released = true;
		}
	}
}

OW_WriteLock::WLData::~WLData()
{
	try
	{
		if(!m_released)
			m_locker->releaseWriteLock();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

OW_WriteLock::WLData&
OW_WriteLock::WLData::operator= (const OW_WriteLock::WLData& x)
{
	m_locker = x.m_locker;
	m_released = x.m_released;
	return *this;
}





