/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_THREAD_TYPES_HPP_
#define OW_THREAD_TYPES_HPP_
#include "OW_config.h"

#if defined(OW_USE_PTHREAD)

	#include <pthread.h>

	namespace OpenWBEM
	{

		// Platform specific thread type
		typedef pthread_t			Thread_t;
		typedef pthread_mutex_t NativeMutex_t;
		struct NonRecursiveMutex_t
		{
			pthread_mutex_t mutex;
		};

		#if defined(OW_HAVE_PTHREAD_MUTEXATTR_SETTYPE)
		// Platform specific mutex type
		// we have native recursive mutexes.
		struct Mutex_t
		{
			pthread_mutex_t mutex;
		};

		#else

		// we have to emulate recursive mutexes.
		struct Mutex_t
		{
			pthread_mutex_t mutex;
			pthread_cond_t unlocked;
			bool valid_id;
			unsigned count;
			pthread_t thread_id;
		};
		#endif

		// Platform specific conditional variable type
		typedef pthread_cond_t 			ConditionVar_t;
		struct NonRecursiveMutexLockState
		{
			pthread_t thread_id;
			NativeMutex_t* pmutex;
		};

	} // end namespace OpenWBEM

#elif defined(OW_WIN32)

	#include <Windows.h>

	namespace OpenWBEM
	{

		// Platform specific thread type
		typedef DWORD Thread_t;
		typedef HANDLE NativeMutex_t;
		struct NonRecursiveMutex_t
		{
			HANDLE mutex;
			HANDLE thread_id;
			bool valid_id;
		};
		struct Mutex_t
		{
			void* mutex;
			unsigned long count;
		};
		// Platform specific conditional variable type
		typedef void* 			ConditionVar_t;
		struct NonRecursiveMutexLockState
		{
			void* thread_id;
			NativeMutex_t* pmutex;
		};

	} // end namespace OpenWBEM

#endif

#endif	// #ifndef OW_THREAD_TYPES_HPP_
				
