/*******************************************************************************
 * Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#ifndef OW_REFERENCE_BASE_HPP_INCLUDE_GUARD_
#define OW_REFERENCE_BASE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_RefCount.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline void RefSwap(T& x, T&y)
{
	T t = x;
	x = y;
	y = t;
}
//////////////////////////////////////////////////////////////////////////////
// This class contains the non-templated code for Reference, to help 
// minimize code bloat.
class OW_COMMON_API ReferenceBase
{
#if !defined(__GNUC__) || __GNUC__ > 2 // because of a gcc 2.95 ICE
protected:
#else
public:
#endif
	ReferenceBase()
		: m_pRefCount(new RefCount) {}
	ReferenceBase(const ReferenceBase& arg)
		: m_pRefCount(0)
	{
		m_pRefCount = arg.m_pRefCount;
		m_pRefCount->inc();
	}
	void incRef()
	{
		m_pRefCount->inc();
	}
	
	bool decRef()
	{
		if (m_pRefCount->decAndTest())
		{
			delete m_pRefCount;
			return true;
		}
		return false;
	}
	void swap(ReferenceBase& arg)
	{
		RefSwap(m_pRefCount, arg.m_pRefCount);
	}
	void useRefCountOf(const ReferenceBase& arg)
	{
		decRef();
		m_pRefCount = arg.m_pRefCount;
		incRef();
	}
	
#ifdef OW_CHECK_NULL_REFERENCES
	static void throwNULLException();
	static void checkNull(const void *p)
	{
		if (p == 0)
		{
			throwNULLException();
		}
	}
#endif

#if !defined(__GNUC__) || __GNUC__ > 2 // because of a gcc 2.95 ICE
protected:
#else
public:
#endif
	RefCount* volatile m_pRefCount;
};

} // end namespace OW_NAMESPACE

#endif

