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
#include "OW_NonRecursiveMutexImpl.hpp"

#include <cerrno>
#include <cassert>


/**
 * Create a platform specific mutext handle.
 * @param handle	The mutex handle that should be initialized by this method
 * @return 0 on success. Otherwise -1.
 */
// static 
int
OW_NonRecursiveMutexImpl::createMutex(OW_NonRecursiveMutex_t& handle)
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

    pthread_mutexattr_t attr;
    int res = pthread_mutexattr_init(&attr);
    assert(res == 0);
    if (res != 0)
        return -1;
 
    res = pthread_mutex_init(&handle.mutex, &attr);
    if (res != 0)
        return -1;
 
    handle.valid_id = false;
	return 0;
#endif
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
OW_NonRecursiveMutexImpl::destroyMutex(OW_NonRecursiveMutex_t& handle)
{
#ifdef OW_USE_GNU_PTH
	(void)handle;
	return 0;
#else
	switch (pthread_mutex_destroy(&handle.mutex))
	{
		case 0:
			break;

		case EBUSY:
			//cerr << "OW_NonRecursiveMutexImpl::destroyMutex - got EBUSY on destroy" << endl;
			return -1;
			break;

		default:
			//cerr << "OW_NonRecursiveMutexImpl::destroyMutex - Error on destroy: "
			//	<< cc << endl;
			return -2;
	}

	return 0;
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
OW_NonRecursiveMutexImpl::acquireMutex(OW_NonRecursiveMutex_t& handle)
{
#ifdef OW_USE_GNU_PTH
	pth_mutex_acquire(&handle, false, 0);
	return 0;
#else

    pthread_t tid = pthread_self();
    if (handle.valid_id && pthread_equal(handle.thread_id, tid))
        //OW_THROW(OW_DeadlockException, "Trying to lock a non-recursive mutex that this thread already has locked");
        return -1;

    int res = pthread_mutex_lock(&handle.mutex);
    assert(res == 0);

    handle.valid_id = true;
    handle.thread_id = tid;

	return res;
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
OW_NonRecursiveMutexImpl::releaseMutex(OW_NonRecursiveMutex_t& handle)
{
#ifdef OW_USE_GNU_PTH
	// TODO: ?!?!
	(void)handle;
	return 0;
#else
    pthread_t tid = pthread_self();
    if (!handle.valid_id)
    {
        return -3;
    }

    if (!pthread_equal(handle.thread_id, tid))
    {
        return -2;
    }

	int res = pthread_mutex_unlock(&handle.mutex);
	assert(res == 0);

    handle.valid_id = false;

	return res;
 
#endif
}


// static
int
OW_NonRecursiveMutexImpl::conditionPreWait(OW_NonRecursiveMutex_t& handle, OW_NonRecursiveMutexLockState& state)
{
    state.pmutex = &handle.mutex;
    state.thread_id = handle.thread_id;
    assert(handle.valid_id); // must have been locked
    handle.valid_id = false;
	return 0;
}

// static
int
OW_NonRecursiveMutexImpl::conditionPostWait(OW_NonRecursiveMutex_t& handle, OW_NonRecursiveMutexLockState& state)
{
    handle.thread_id = state.thread_id;
	handle.valid_id = true;
	return 0;
}

