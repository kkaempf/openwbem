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
 ******************************************************************************/
#include "OW_config.h"
#include "OW_AtomicOps.hpp"

#if defined(__i386__) && defined(__GNUC__)
// inline in the header

#elif defined(OW_HAVE_PTHREAD_SPIN_LOCK) && !defined(OW_USE_GNU_PTH)
OW_Atomic_t::OW_Atomic_t()
	: val(0)
{
	pthread_spin_init(&spinlock);
}

OW_Atomic_t::OW_Atomic_t(int i)
	: val(i)
{
	pthread_spin_init(&spinlock);
}

void OW_AtomicInc(OW_Atomic_t &v)
{
	pthread_spin_lock(&v.spinlock);
	++v.val;
	pthread_spin_unlock(&v.spinlock);
}


bool OW_AtomicDecAndTest(OW_Atomic_t &v)
{
	pthread_spin_lock(&v.spinlock);
	--v.val;
	bool b = ((v.val == 0) ? true : false) ;
	pthread_spin_unlock(&v.spinlock);
	return b;
}

int OW_AtomicGet(OW_Atomic_t const &v)
{
	return v.val;
}

void OW_AtomicDec(OW_Atomic_t &v)
{
	pthread_spin_lock(&v.spinlock);
	--v.val;
	pthread_spin_unlock(&v.spinlock);
}

#else

#if defined(OW_USE_DEFAULT_ATOMIC_OPS)


// pth doesn't have pth_spin_lock
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"

static OW_Mutex guard;
void OW_AtomicInc(OW_Atomic_t &v)
{
    OW_MutexLock lock(guard);
    ++v.val;
}

bool OW_AtomicDecAndTest(OW_Atomic_t &v)
{
    OW_MutexLock lock(guard);
    return --v.val == 0;
}

int OW_AtomicGet(OW_Atomic_t const &v)
{
    OW_MutexLock lock(guard);
	return v.val;
}

void OW_AtomicDec(OW_Atomic_t &v)
{
    OW_MutexLock lock(guard);
    --v.val;
}

#endif

#endif

