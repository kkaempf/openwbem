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
#include <csignal> // for sig_atomic_t
#include <cassert>
#include "OW_MemoryBarrier.hpp"
#endif

namespace OW_NAMESPACE
{

#if defined(OW_USE_PTHREAD)

struct OnceFlag
{
	volatile ::sig_atomic_t flag;
	::pthread_mutex_t mtx;
};

#define OW_ONCE_INIT {0, PTHREAD_MUTEX_INITIALIZER}

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
template <typename FuncT>
void OW_COMMON_API callOnce(OnceFlag& flag, FuncT F);



#if defined(OW_USE_PTHREAD)

class CallOnce_pthread_MutexLock
{
public:
	CallOnce_pthread_MutexLock(::pthread_mutex_t* mtx)
		: m_mtx(mtx)
	{
		int res = pthread_mutex_lock(m_mtx);
		assert(res == 0);
	}
	~CallOnce_pthread_MutexLock()
	{
		int res = pthread_mutex_unlock(m_mtx);
		assert(res == 0);
	}
private:
	::pthread_mutex_t* m_mtx;
};

template <typename FuncT>
inline void callOnce(OnceFlag& flag, FuncT f)
{
	readWriteMemoryBarrier();
	if (flag.flag == 0)
	{
		CallOnce_pthread_MutexLock lock(&flag.mtx);
		if (flag.flag == 0)
		{
			f();
			flag.flag = 1;
		}
	}
}

#endif


} // end namespace OW_NAMESPACE

#endif

