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

static OW_Thread_t zeroThread();
static OW_Thread_t NULLTHREAD = zeroThread();


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
void
OW_Mutex::acquire()
{
	if(!m_isRecursive)
	{
		int rv = OW_MutexImpl::acquireMutex(m_mutex);
		if (rv != 0)
		{
			OW_THROW(OW_Assertion, 
				OW_Format("OW_MutexImpl::acquireMutex returned with error: %1", 
					rv).c_str());
		}
	}
	else
	{
		OW_Thread_t volatile curThread = OW_ThreadImpl::currentThread();
		if(OW_ThreadImpl::sameThreads(m_owner, curThread))
		{
			m_refCount++;
		}
		else
		{
			if (OW_MutexImpl::acquireMutex(m_mutex) != 0)
			{
				OW_THROW(OW_Assertion, "OW_MutexImpl::acquireMutex returned with error");
			}
		  	m_owner = OW_ThreadImpl::currentThread();
		  	m_refCount = 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_Mutex::release()
{
	OW_Bool rval = false;

	if(!m_isRecursive)
	{
		if (OW_MutexImpl::releaseMutex(m_mutex) != 0)
		{
			OW_THROW(OW_Assertion, "OW_MutexImpl::releaseMutex returned with error");
		}
		rval = true;
	}
	else
	{
		OW_Thread_t curThread = OW_ThreadImpl::currentThread();
		if(OW_ThreadImpl::sameThreads(m_owner, curThread))
		{
			--m_refCount;
			if(m_refCount <= 0)
			{
				m_refCount = 0;
				m_owner = NULLTHREAD;
				if (OW_MutexImpl::releaseMutex(m_mutex) != 0)
				{
					OW_THROW(OW_Assertion, "OW_MutexImpl::releaseMutex returned with error");
				}
			}
			rval = true;
		}
		else
		{
			OW_THROW(OW_Exception, format("%1 attempted to release mutex "
				"owned by %2", OW_ThreadImpl::currentThread(), m_owner).c_str());
		}
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
static OW_Thread_t
zeroThread()
{
	OW_Thread_t zthr;
	::memset(&zthr, 0, sizeof(zthr));
	return zthr;
}

//////////////////////////////////////////////////////////////////////////////
// OW_MutexLock methods
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
OW_MutexLock::OW_MutexLock() : m_pMutex(NULL), m_locked(false)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_MutexLock::OW_MutexLock(OW_Mutex& mutex, OW_Bool doLock) :
	m_pMutex(&mutex), m_locked(false)
{
	if(doLock)
	{
		lock();
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_MutexLock::OW_MutexLock(const OW_MutexLock& arg) :
	m_pMutex(arg.m_pMutex), m_locked(arg.m_locked)
{
	arg.m_locked = false;
}

//////////////////////////////////////////////////////////////////////////////
OW_MutexLock::~OW_MutexLock()
{
	release();
}
	
//////////////////////////////////////////////////////////////////////////////
OW_MutexLock&
OW_MutexLock::operator= (const OW_MutexLock& arg)
{
	release();
	m_locked = arg.m_locked;
	m_pMutex = arg.m_pMutex;
	arg.m_locked = false;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MutexLock::lock()
{
	if(!m_locked && m_pMutex != NULL)
	{
		m_pMutex->acquire();
		m_locked = true;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MutexLock::release()
{
	if(m_locked && m_pMutex != NULL)
	{
		m_pMutex->release();
		m_locked = false;
	}
}

