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
#ifndef OW_ATOMIC_OPS_HPP_
#define OW_ATOMIC_OPS_HPP_

#include "OW_config.h"

#if defined(__i386__) && defined(__GNUC__)
// use fast inline assembly versions
typedef struct { volatile int val; } OW_Atomic_t;

// OW_ATOMIC() is weird; you can't use it for member variables.  Here's how to do 
// it correctly.
// In constructor body:
//	OW_Atomic_t c = OW_ATOMIC(0);
//	m_myAtomicVar = c;

#define OW_ATOMIC(i)	{ (i) }

inline void OW_AtomicInc(OW_Atomic_t &v)
{
	__asm__ __volatile__(
		"lock ; " "incl %0"
		:"=m" (v.val)
		:"m" (v.val));
}

inline bool OW_AtomicDecAndTest(OW_Atomic_t &v)
{
	unsigned char c;

	__asm__ __volatile__(
		"lock ; " "decl %0; sete %1"
		:"=m" (v.val), "=qm" (c)
		:"m" (v.val) : "memory");
	return c != 0;
}

inline int OW_AtomicGet(OW_Atomic_t const &v)
{
	return v.val;
}

#else
// use slow mutex protected versions
#define OW_USE_DEFAULT_ATOMIC_OPS // used in OW_AtomicOps.cpp
typedef int OW_Atomic_t;

#define OW_ATOMIC(i)	(i)

void OW_AtomicInc(OW_Atomic_t &v);
bool OW_AtomicDecAndTest(OW_Atomic_t &v);
int OW_AtomicGet(OW_Atomic_t const &v)

#endif

#endif
