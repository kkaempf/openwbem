/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
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
*       Quest Software, Inc.,
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_OperationRWLock.hpp"
#include "blocxx/ThreadImpl.hpp"
#include "OW_ExceptionIds.hpp"

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(OperationRWLock);

//////////////////////////////////////////////////////////////////////////////
OperationRWLock::OperationRWLock()
{
}
//////////////////////////////////////////////////////////////////////////////
OperationRWLock::~OperationRWLock()
{
}
//////////////////////////////////////////////////////////////////////////////
void
OperationRWLock::acquireReadLock(UInt64 operationId, const Timeout& timeout)
{
	m_impl.acquireReadLock(operationId, timeout);
}

//////////////////////////////////////////////////////////////////////////////
void
OperationRWLock::releaseReadLock(UInt64 operationId)
{
	try
	{
		m_impl.releaseReadLock(operationId);
	}
	catch (GenericRWLockImplException& e)
	{
		OW_THROW_SUBEX(OperationRWLockException, "Cannot release a read lock when no read lock is held", e);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OperationRWLock::acquireWriteLock(UInt64 operationId, const Timeout& timeout)
{
	m_impl.acquireWriteLock(operationId, timeout);
}

//////////////////////////////////////////////////////////////////////////////
void
OperationRWLock::releaseWriteLock(UInt64 operationId)
{
	try
	{
		m_impl.releaseWriteLock(operationId);
	}
	catch (GenericRWLockImplException& e)
	{
		OW_THROW_SUBEX(OperationRWLockException, "Cannot release a write lock when no write lock is held", e);
	}
}

} // end namespace OW_NAMESPACE

