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

#ifndef __OW_MUTEX_HPP__
#define __OW_MUTEX_HPP__

#include "OW_config.h"
#include "OW_ThreadTypes.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_Bool.hpp"
#include "OW_Assertion.hpp"


/**
 * The OW_MutexImpl class represents the functionality needed by the
 * OpenWbem Mutex class (OW_Mutex). The implementation for this class
 * must be provided on all platforms that OpenWbem runs on. It is essentially
 * an abstraction layer over another mutex implementation.
 */
class OW_MutexImpl
{
public:

	/**
	 * Create a platform specific mutext handle.
	 * @param handle	The mutex handle that should be initialized by this method
	 * @return 0 on success. Otherwise -1.
	 */
	static int createMutex(OW_Mutex_t& handle);

	/**
	 * Destroy a mutex previously created with createMutex.
	 * @param handle The handle to the mutex that will be destroyed.
	 * @return The following error codes:
	 *		 0:	success
	 *		-1:	Could not be acquired for destruction because it is currently
	 *				locked.
	 *		-2:	All other error conditions
	 */
	static int destroyMutex(OW_Mutex_t& handle);

	/**
	 * Acquire the mutex specified by a given mutex handle. This method should
	 * block until the desired mutex can be acquired. The error return value is
	 * used to indicate critical errors.
	 *
	 * @param handle The mutex to acquire.
	 * @return 0 on success. -1 indicates a critical error.
	 */
	static int acquireMutex(OW_Mutex_t& handle)
	{
	#ifdef OW_USE_GNU_PTH
		pth_mutex_acquire(&handle, false, NULL);
		return 0;
	#else
		int cc = pthread_mutex_lock(&handle);
		if(cc != 0)
		{
			//cerr << "OW_MutexImpl::acquireMutex got err on lock: " << cc << endl;
			return cc;
		}
	
		return 0;
	#endif
	}


	/**
	 * Release a mutex that was previously acquired with the acquireMutex
	 * method.
	 * @param handle The handle to the mutex that is being released.
	 * @return 0 on success. -1 indicates a critical error.
	 */
	static int releaseMutex(OW_Mutex_t& handle)
	{
	#ifdef OW_USE_GNU_PTH
		(void)handle;
		return 0;
	#else
		int cc = pthread_mutex_unlock(&handle);
		if(cc != 0)
		{
			//cerr << "OW_MutexImpl::releaseMutex got err on lock: " << cc << endl;
			return -1;
		}
	
		return 0;
	#endif
	}

};


class OW_Mutex
{
public:

	/**
	 * Create a new OW_Mutex object.
	 * @param recursive	If true then this is a recursive mutext. Otherwise
	 * a thread will block indefinately if it calls acquire more than once.
	 */
	OW_Mutex(OW_Bool recursive=true);

	/**
	 * Destroy this OW_Mutex object.
	 */
	~OW_Mutex();

	/**
	 * Acquire ownership of this OW_Mutex object.
	 * If this is a recursive mutex, this call will block if another thread has
	 * ownership of this OW_Mutex. When it returns, the current thread will be
	 * the owner of this OW_Mutex object.
	 * If this is not a recursive mutex, the behaviour is similar to recursive
	 * with the exception that more than one call to acquire will block the
	 * thread indefinately.
	 */
	void acquire()
	{
		if(!m_isRecursive)
		{
			int rv = OW_MutexImpl::acquireMutex(m_mutex);
			if (rv != 0)
			{
				OW_THROW(OW_Assertion,
					"OW_MutexImpl::acquireMutex returned with error");
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


	/**
	 * Release ownership of this OW_Mutex object. If another thread is waiting
	 * to acquire the ownership of this mutex it will stop blocking and acquire
	 * ownership when this call returns.
	 * @return ?
	 */
	bool release()
	{
		if(!m_isRecursive)
		{
			if (OW_MutexImpl::releaseMutex(m_mutex) != 0)
			{
				OW_THROW(OW_Assertion, "OW_MutexImpl::releaseMutex returned with error");
			}
			return true;
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
				return true;
			}
			else
			{
				OW_THROW(OW_Exception, "attempted to release mutex "
					"owned by another thread!");
			}
		}
	
		return false;
	}


protected:

	OW_Mutex_t m_mutex;
	OW_Thread_t volatile m_owner;
	int volatile m_refCount;
	bool m_isRecursive;
private:
	OW_Mutex(const OW_Mutex&);
	OW_Mutex operator = (const OW_Mutex&);

	static OW_Thread_t zeroThread();
	static OW_Thread_t NULLTHREAD;
};


#endif // __OW_MUTEX_HPP__
