/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OW_THREAD_ONCE_HPP_INCLUDE_GUARD_
#define OW_THREAD_ONCE_HPP_INCLUDE_GUARD_
#include "OW_config.h"

#if defined(OW_USE_PTHREAD)
#include <pthread.h>
#endif

namespace OW_NAMESPACE
{

#if defined(OW_USE_PTHREAD)

typedef pthread_once_t OnceFlag;
#define OW_ONCE_INIT PTHREAD_ONCE_INIT

#elif defined(OW_WIN32)

typedef long OnceFlag;
#define OW_ONCE_INIT 0

#else
#error "Port me!"
#endif

/**
 * The first time callOnce is called with a given onceFlag argument, it calls func with no argument and changes the value of flag to indicate
 * that func has been run.  Subsequent calls with the same onceFlag do nothing.
 */
void OW_COMMON_API callOnce(OnceFlag& flag, void (*func)());



#if defined(OW_USE_PTHREAD)

inline void callOnce(OnceFlag& flag, void (*func)())
{
	pthread_once(&flag, func);
}

#endif


} // end namespace OW_NAMESPACE

#endif

