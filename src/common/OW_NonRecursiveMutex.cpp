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

#include "OW_config.h"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_Assertion.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_NonRecursiveMutexImpl.hpp"
#include "OW_ExceptionIds.hpp"

#include <cstring>

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(Deadlock);
NonRecursiveMutex::NonRecursiveMutex()
{
	if (NonRecursiveMutexImpl::createMutex(m_mutex) != 0)
	{
		OW_THROW(Assertion, "NonRecursiveMutexImpl::createMutex failed");
	}
}
NonRecursiveMutex::~NonRecursiveMutex()
{
	if (NonRecursiveMutexImpl::destroyMutex(m_mutex) == -1)
	{
		NonRecursiveMutexImpl::releaseMutex(m_mutex);
		NonRecursiveMutexImpl::destroyMutex(m_mutex);
	}
}
void 
NonRecursiveMutex::acquire()
{
	int rv = NonRecursiveMutexImpl::acquireMutex(m_mutex);
	if (rv != 0)
	{
		OW_THROW(Assertion,
			"NonRecursiveMutexImpl::acquireMutex returned with error");
	}
}
bool
NonRecursiveMutex::release()
{
	int rc = NonRecursiveMutexImpl::releaseMutex(m_mutex);
	if (rc != 0)
	{
		OW_THROW(Assertion, Format("NonRecursiveMutexImpl::releaseMutex returned with error %1", rc).c_str());
	}
	return true;
}
void 
NonRecursiveMutex::conditionPreWait(NonRecursiveMutexLockState& state)
{
	if (NonRecursiveMutexImpl::conditionPreWait(m_mutex, state) != 0)
	{
		OW_THROW(Assertion, "NonRecursiveMutexImpl::releaseMutex returned with error");
	}
}
void
NonRecursiveMutex::conditionPostWait(NonRecursiveMutexLockState& state)
{
	if (NonRecursiveMutexImpl::conditionPostWait(m_mutex, state) != 0)
	{
		OW_THROW(Assertion, "NonRecursiveMutexImpl::releaseMutex returned with error");
	}
}

} // end namespace OpenWBEM

