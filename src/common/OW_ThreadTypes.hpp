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

#ifndef OW_THREAD_TYPES_HPP_
#define OW_THREAD_TYPES_HPP_

#include "OW_config.h"

#ifdef OW_USE_GNU_PTH

extern "C"
{
#include <pth.h>
}

// Platform specific thread type
typedef pth_t			OW_Thread_t;

// Platform specific mutex type
typedef pth_mutex_t	OW_Mutex_t;

// Platform specific event type
typedef void*			OW_ThreadEvent_t;

// Platform specific conditional variable type
typedef pth_cond_t	OW_ConditionVar_t;

#else

#ifdef OW_HAVE_PTHREAD_H

extern "C"
{
#include <pthread.h>
}

// Platform specific thread type
typedef pthread_t					OW_Thread_t;

#if defined(OW_HAVE_PTHREAD_MUTEXATTR_SETTYPE)
// Platform specific mutex type
struct OW_Mutex_t
{
	pthread_mutex_t mutex;
};
//typedef pthread_mutex_t 		OW_Mutex_t;
#else
struct OW_Mutex_t
{
	pthread_mutex_t mutex;
};
#endif

// Platform specific event type
typedef void* 						OW_ThreadEvent_t;

// Platform specific conditional variable type
typedef pthread_cond_t 			OW_ConditionVar_t;

#endif // #ifdef OW_HAVE_PTHREAD_H

#endif	// #ifdef OW_USE_GNU_PTH

				
#endif	// #ifndef OW_THREAD_TYPES_HPP_
				
