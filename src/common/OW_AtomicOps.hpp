/*******************************************************************************
 * Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 ******************************************************************************/

/**
 * @author Dan Nuffer
 */

#ifndef OW_ATOMIC_OPS_HPP_
#define OW_ATOMIC_OPS_HPP_
#include "OW_config.h"

#if defined(OW_AIX)
extern "C"
{
#include <sys/atomic_op.h>
}
#endif

// TODO: PA-RISC and itanium.
// See http://h21007.www2.hp.com/dspp/files/unprotected/Itanium/spinlocks.pdf
// http://www.hpl.hp.com/research/linux/atomic_ops/

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

// x86 and x86-64 asm is identical
#if (defined(OW_ARCH_X86) || defined(__i386__) || defined(OW_ARCH_X86_64) || defined(__x86_64__)) && defined(__GNUC__)

namespace OW_NAMESPACE
{

// use fast inline assembly versions
struct Atomic_t
{ 
	Atomic_t() : val(0) {}
	Atomic_t(int i) : val(i) {}
	volatile int val; 
};
inline void AtomicInc(Atomic_t &v)
{
	__asm__ __volatile__(
		"lock ; " "incl %0"
		:"=m" (v.val)
		:"m" (v.val));
}
inline bool AtomicDecAndTest(Atomic_t &v)
{
	unsigned char c;
	__asm__ __volatile__(
		"lock ; " "decl %0; sete %1"
		:"=m" (v.val), "=qm" (c)
		:"m" (v.val) : "memory");
	return c != 0;
}
inline int AtomicGet(Atomic_t const &v)
{
	return v.val;
}
inline void AtomicDec(Atomic_t &v)
{
	__asm__ __volatile__(
		"lock ; " "decl %0"
		:"=m" (v.val)
		:"m" (v.val));
}

} // end namespace OW_NAMESPACE

#elif defined(OW_AIX)
namespace OW_NAMESPACE
{
// This comment was stolen from the libstdc++ implementation of atomicity.h
// (and modified). 
// We cannot use the inline assembly for powerpc, since definitions for
// these operations since they depend on operations that are not available on
// the original POWER architecture.  AIX still runs on the POWER architecture,
// so it would be incorrect to assume the existence of these instructions. 
//
// The definition of Atomic_t.val must match the type pointed to by atomic_p in
// <sys/atomic_op.h>. 
struct Atomic_t
{ 
	Atomic_t() : val(0) {}
	Atomic_t(int i) : val(i) {}
	volatile int val; 
};

inline void AtomicInc(Atomic_t &v)
{
	::fetch_and_add(const_cast<atomic_p>(&v.val), 1);
}
inline bool AtomicDecAndTest(Atomic_t &v)
{
	// fetch_and_add returns the original value before the add operation.  Thus,
	// we must subtract one from the returned value before comparing.
	int c = ::fetch_and_add(const_cast<atomic_p>(&v.val), -1);
	--c;
	return c == 0;
}
inline int AtomicGet(Atomic_t const &v)
{
	int c = ::fetch_and_add(const_cast<atomic_p>(&v.val), 0);
	return c;
}
inline void AtomicDec(Atomic_t &v)
{
	::fetch_and_add(const_cast<atomic_p>(&v.val), -1);
}

} // end namespace OW_NAMESPACE

#elif (defined(OW_ARCH_PPC) || defined(__ppc__)) && defined(__GNUC__)

namespace OW_NAMESPACE
{

// use fast inline assembly versions
struct Atomic_t
{ 
	Atomic_t() : val(0) {}
	Atomic_t(int i) : val(i) {}
	volatile int val; 
};

inline void AtomicInc(Atomic_t &v)
{
	int t;
	__asm__ __volatile__(
		"1:	lwarx   %0,0,%2\n"
		"	addic   %0,%0,1\n"
		"	stwcx.  %0,0,%2\n"
		"	bne-    1b"
		: "=&r" (t), "=m" (v.val)
		: "r" (&v.val), "m" (v.val)
		: "cc");
}
inline bool AtomicDecAndTest(Atomic_t &v)
{
	int c;
	__asm__ __volatile__(
		"1:	lwarx   %0,0,%1\n"
		"	addic   %0,%0,-1\n"
		"	stwcx.  %0,0,%1\n"
		"	bne-    1b\n"
		"	isync"
		: "=&r" (c)
		: "r" (&v.val)
		: "cc", "memory");
	return c == 0;
}
inline int AtomicGet(Atomic_t const &v)
{
	return v.val;
}
inline void AtomicDec(Atomic_t &v)
{
	int c;
	__asm__ __volatile__(
		"1:	lwarx   %0,0,%2\n"
		"	addic   %0,%0,-1\n"
		"	stwcx.  %0,0,%2\n"
		"	bne-    1b"
		: "=&r" (c), "=m" (v.val)
		: "r" (&v.val), "m" (v.val)
		: "cc");
}

} // end namespace OW_NAMESPACE

#elif defined(OW_WIN32)

namespace OW_NAMESPACE
{

// use fast inline assembly versions
struct OW_COMMON_API Atomic_t
{ 
	Atomic_t() : val(0) {}
	Atomic_t(int i) : val(i) {}
	volatile LONG val; 
};
inline void AtomicInc(Atomic_t &v)
{
	InterlockedIncrement(&v.val); 
}
inline bool AtomicDecAndTest(Atomic_t &v)
{
	return InterlockedDecrement(&v.val) == 0;
}
inline int AtomicGet(Atomic_t const &v)
{
	return v.val;
}
inline void AtomicDec(Atomic_t &v)
{
	InterlockedDecrement(&v.val);
}

} // end namespace OW_NAMESPACE

#elif defined(OW_HAVE_PTHREAD_SPIN_LOCK)
#include <pthread.h>

#define OW_USE_PTHREAD_SPIN_LOCK_ATOMIC_OPS // used in OW_AtomicOps.cpp

namespace OW_NAMESPACE
{

struct Atomic_t
{
	Atomic_t();
	Atomic_t(int i);
	int val;
	pthread_spinlock_t spinlock;
};
void AtomicInc(Atomic_t &v);
bool AtomicDecAndTest(Atomic_t &v);
int AtomicGet(Atomic_t const &v);
void AtomicDec(Atomic_t &v);

} // end namespace OW_NAMESPACE

#else
// use slow mutex protected versions
#define OW_USE_OW_DEFAULT_ATOMIC_OPS // used in OW_AtomicOps.cpp

namespace OW_NAMESPACE
{

struct Atomic_t
{ 
	Atomic_t() : val(0) {}
	Atomic_t(int i) : val(i) {}
	volatile int val; 
};
void AtomicInc(Atomic_t &v);
bool AtomicDecAndTest(Atomic_t &v);
int AtomicGet(Atomic_t const &v);
void AtomicDec(Atomic_t &v);

} // end namespace OW_NAMESPACE

#endif
#endif
