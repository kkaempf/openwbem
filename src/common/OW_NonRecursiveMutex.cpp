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
#include "OW_NonRecursiveMutex.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_Assertion.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_NonRecursiveMutexImpl.hpp"

#include <cstring>


DEFINE_EXCEPTION(Deadlock)

OW_NonRecursiveMutex::OW_NonRecursiveMutex()
{
	if(OW_NonRecursiveMutexImpl::createMutex(m_mutex) != 0)
	{
		OW_THROW(OW_Assertion, "OW_NonRecursiveMutexImpl::createMutex failed");
	}
}

OW_NonRecursiveMutex::~OW_NonRecursiveMutex()
{
	if(OW_NonRecursiveMutexImpl::destroyMutex(m_mutex) == -1)
	{
		OW_NonRecursiveMutexImpl::releaseMutex(m_mutex);
		OW_NonRecursiveMutexImpl::destroyMutex(m_mutex);
	}
}


void 
OW_NonRecursiveMutex::acquire()
{
	int rv = OW_NonRecursiveMutexImpl::acquireMutex(m_mutex);
	if (rv != 0)
	{
		OW_THROW(OW_Assertion,
			"OW_NonRecursiveMutexImpl::acquireMutex returned with error");
	}
}


bool
OW_NonRecursiveMutex::release()
{
    int rc = OW_NonRecursiveMutexImpl::releaseMutex(m_mutex);
	if (rc != 0)
	{
		OW_THROW(OW_Assertion, format("OW_NonRecursiveMutexImpl::releaseMutex returned with error %1", rc).c_str());
	}
	return true;
}

void 
OW_NonRecursiveMutex::conditionPreWait(OW_NonRecursiveMutexLockState& state)
{
	if (OW_NonRecursiveMutexImpl::conditionPreWait(m_mutex, state) != 0)
	{
		OW_THROW(OW_Assertion, "OW_NonRecursiveMutexImpl::releaseMutex returned with error");
	}
}

void
OW_NonRecursiveMutex::conditionPostWait(OW_NonRecursiveMutexLockState& state)
{
	if (OW_NonRecursiveMutexImpl::conditionPostWait(m_mutex, state) != 0)
	{
		OW_THROW(OW_Assertion, "OW_NonRecursiveMutexImpl::releaseMutex returned with error");
	}
}

