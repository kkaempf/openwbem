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
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_Assertion.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"

#include <cstring>

OW_Thread_t OW_Mutex::NULLTHREAD = zeroThread();


//////////////////////////////////////////////////////////////////////////////
OW_Mutex::OW_Mutex(OW_Bool recursive) :
	m_owner(NULLTHREAD), m_refCount(0), m_isRecursive(recursive)
{
	if(OW_MutexImpl::createMutex(m_mutex) != 0)
	{
		OW_THROW(OW_Assertion, "OW_MutexImpl::createMutex failed");
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Mutex::~OW_Mutex()
{
	if(OW_MutexImpl::destroyMutex(m_mutex) == -1)
	{
		OW_MutexImpl::releaseMutex(m_mutex);
		OW_MutexImpl::destroyMutex(m_mutex);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Thread_t
OW_Mutex::zeroThread()
{
	OW_Thread_t zthr;
	::memset(&zthr, 0, sizeof(zthr));
	return zthr;
}


