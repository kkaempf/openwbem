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

#ifndef OW_NON_RECURSIVE_MUTEX_IMPL_INCLUDE_GUARD_HPP_
#define OW_NON_RECURSIVE_MUTEX_IMPL_INCLUDE_GUARD_HPP_

#include "OW_config.h"
#include "OW_ThreadTypes.hpp"

/**
 * The OW_NonRecursiveMutexImpl class represents the functionality needed by the
 * OpenWbem non recursive Mutex class (OW_NonRecursiveMutex). The implementation for this class
 * must be provided on all platforms that OpenWbem runs on. It is essentially
 * an abstraction layer over another mutex implementation.
 */
class OW_NonRecursiveMutexImpl
{
public:

	/**
	 * Create a platform specific mutext handle.
	 * @param handle	The mutex handle that should be initialized by this method
	 * @param isRecursive Specified whether to create a recursive mutex
	 * @return 0 on success. Otherwise -1.
	 */
	static int createMutex(OW_NonRecursiveMutex_t& handle);

	/**
	 * Destroy a mutex previously created with createMutex.
	 * @param handle The handle to the mutex that will be destroyed.
	 * @return The following error codes:
	 *		 0:	success
	 *		-1:	Could not be acquired for destruction because it is currently
	 *				locked.
	 *		-2:	All other error conditions
	 */
	static int destroyMutex(OW_NonRecursiveMutex_t& handle);

	/**
	 * Acquire the mutex specified by a given mutex handle. This method should
	 * block until the desired mutex can be acquired. The error return value is
	 * used to indicate critical errors.
	 *
	 * @param handle The mutex to acquire.
	 * @return 0 on success. -1 indicates a critical error.
	 */
	static int acquireMutex(OW_NonRecursiveMutex_t& handle);

	/**
	 * Release a mutex that was previously acquired with the acquireMutex
	 * method.
	 * @param handle The handle to the mutex that is being released.
	 * @return 0 on success. -1 indicates a critical error.
	 */
	static int releaseMutex(OW_NonRecursiveMutex_t& handle);
	
	static int conditionPreWait(OW_NonRecursiveMutex_t& handle, OW_NonRecursiveMutexLockState& state);
	static int conditionPostWait(OW_NonRecursiveMutex_t& handle, OW_NonRecursiveMutexLockState& state);
};

#endif


