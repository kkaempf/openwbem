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
#include "OW_MutexImpl.hpp"

#include <cerrno>


/**
 * Create a platform specific mutext handle.
 * @param handle	The mutex handle that should be initialized by this method
 * @return 0 on success. Otherwise -1.
 */
// static 
int
OW_MutexImpl::createMutex(OW_Mutex_t& handle)
{
#ifdef OW_USE_GNU_PTH
	OW_ThreadImpl::initThreads();
	int cc = 0;
	if(!pth_mutex_init(&handle))
	{
		cc = -1;
	}
	return cc;
#else
	pthread_mutex_init(&handle, NULL);
	return 0;
#endif

/*
    pthread_mutexattr_t attr;
    int res = 0;
    res = pthread_mutexattr_init(&attr);
    assert(res == 0);
 
#   if defined(BOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE)
    res = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    assert(res == 0);
#   endif
 
    res = pthread_mutex_init(&m_mutex, &attr);
    if (res != 0)
        throw thread_resource_error();
 
#   if !defined(BOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE)
    res = pthread_cond_init(&m_unlocked, 0);
    if (res != 0)
    {
        pthread_mutex_destroy(&m_mutex);
        throw thread_resource_error();
    }
#   endif
*/
}


/**
 * Destroy a mutex previously created with createMutex.
 * @param handle The handle to the mutex that will be destroyed.
 * @return The following error codes:
 *		 0:	success
 *		-1:	Could not be acquired for destruction because it is currently
 *				locked.
 *		-2:	All other error conditions
 */
// static
int
OW_MutexImpl::destroyMutex(OW_Mutex_t& handle)
{
#ifdef OW_USE_GNU_PTH
	(void)handle;
	return 0;
#else
	int cc = pthread_mutex_destroy(&handle);
	switch (cc)
	{
		case 0:
			break;

		case EBUSY:
			//cerr << "OW_MutexImpl::destroyMutex - got EBUSY on destroy" << endl;
			cc = -1;
			break;

		default:
			//cerr << "OW_MutexImpl::destroyMutex - Error on destroy: "
			//	<< cc << endl;
			cc = -2;
	}
	return cc;
#endif
}


/**
 * Acquire the mutex specified by a given mutex handle. This method should
 * block until the desired mutex can be acquired. The error return value is
 * used to indicate critical errors.
 *
 * @param handle The mutex to acquire.
 * @return 0 on success. -1 indicates a critical error.
 */
// static
int
OW_MutexImpl::acquireMutex(OW_Mutex_t& handle)
{
#ifdef OW_USE_GNU_PTH
	pth_mutex_acquire(&handle, false, 0);
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
// static
int
OW_MutexImpl::releaseMutex(OW_Mutex_t& handle)
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

