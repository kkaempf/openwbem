/*******************************************************************************
* Copyright (C) 2007, Quest Software All rights reserved.
* Copyright (C) 2006, Novell, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of
*       Quest Software,
*       nor Novell, Inc.,
*       nor the names of its contributors or employees may be used to
*       endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
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

#ifndef OW_OPERATION_RWLOCK_HPP_INCLUDE_GUARD_
#define OW_OPERATION_RWLOCK_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "blocxx/GenericRWLockImpl.hpp"
#include <functional> // for std::less

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(OperationRWLock, OW_COMMON_API);
//////////////////////////////////////////////////////////////////////////////
// The locker is recursive and also supports upgrading a read-lock to a write lock
class OW_COMMON_API OperationRWLock
{
public:
	OperationRWLock();
	~OperationRWLock();

	/**
	 * @throws TimeoutException if the lock isn't acquired within the timeout.
	 */
	void acquireReadLock(UInt64 operationId, const Timeout& timeout);

	/**
	 * @throws TimeoutException if the lock isn't acquired within the timeout.
	 * @throws DeadlockException if this call would upgrade a read lock to a write lock
	 *   and another thread is already waiting to upgrade. If this happens, the calling
	 *   thread must release it's read lock in order for forward progress to be made.
	 */
	void acquireWriteLock(UInt64 operationId, const Timeout& timeout);

	/**
	 * @throws OperationRWLockException if a read lock hasn't been acquired.
	 */
	void releaseReadLock(UInt64 operationId);

	/**
	 * @throws OperationRWLockException if a write lock hasn't been acquired.
	 */
	void releaseWriteLock(UInt64 operationId);

private:

	GenericRWLockImpl<UInt64, std::less<UInt64> > m_impl;

	// unimplemented
	OperationRWLock(const OperationRWLock&);
	OperationRWLock& operator=(const OperationRWLock&);
};
//////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API OperationReadLock
{
public:
	OperationReadLock(OperationRWLock& locker, UInt64 operationId, const Timeout& timeout)
		: m_locker(&locker)
		, m_released(false)
		, m_operationId(operationId)
	{
		m_locker->acquireReadLock(m_operationId, timeout);
	}
	~OperationReadLock()
	{
		release();
	}
	void lock(UInt64 operationId, const Timeout& timeout)
	{
		if (m_released)
		{
			m_locker->acquireReadLock(operationId, timeout);
			m_released = false;
		}
	}
	void release()
	{
		if (!m_released)
		{
			m_locker->releaseReadLock(m_operationId);
			m_released = true;
		}
	}
private:
	OperationRWLock* m_locker;
	bool m_released;
	UInt64 m_operationId;

	// noncopyable
	OperationReadLock(const OperationReadLock&);
	OperationReadLock& operator=(const OperationReadLock&);
};

//////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API OperationWriteLock
{
public:
	OperationWriteLock(OperationRWLock& locker, UInt64 operationId, const Timeout& timeout)
		: m_locker(&locker)
		, m_released(false)
		, m_operationId(operationId)
	{
		m_locker->acquireWriteLock(m_operationId, timeout);
	}
	~OperationWriteLock()
	{
		release();
	}
	void lock(UInt64 operationId, const Timeout& timeout)
	{
		if (m_released)
		{
			m_locker->acquireWriteLock(operationId, timeout);
			m_released = false;
		}
	}
	void release()
	{
		if (!m_released)
		{
			m_locker->releaseWriteLock(m_operationId);
			m_released = true;
		}
	}
private:
	OperationRWLock* m_locker;
	bool m_released;
	UInt64 m_operationId;

	// noncopyable
	OperationWriteLock(const OperationWriteLock&);
	OperationWriteLock& operator=(const OperationWriteLock&);
};

} // end namespace OW_NAMESPACE

#endif
