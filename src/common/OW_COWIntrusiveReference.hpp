/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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

//
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

/**
 * @author Peter Dimov
 * @author Dan Nuffer
 */

#ifndef OW_COW_INTRUSIVE_REFERENCE_HPP_INCLUDE_GUARD_
#define OW_COW_INTRUSIVE_REFERENCE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ReferenceHelpers.hpp"

namespace OW_NAMESPACE
{

/**
 *
 *  COWIntrusiveReference
 *
 *  A smart pointer that uses intrusive reference counting.
 *
 *  Relies on unqualified calls to
 *
 *      void COWIntrusiveReferenceAddRef(T* p);
 *      void COWIntrusiveReferenceRelease(T* p);
 *      bool COWIntrusiveReferenceUnique(T* p);
 *      T* COWIntrusiveReferenceClone(T* p);
 *
 *          (p != 0)
 *
 *  The object is responsible for destroying itself.
 *
 * If you want your class to be managed by COWIntrusiveReference, you can
 * derive it from COWIntrusiveCountableBase, or write your own set of
 * functions.
 */

template<class T> class COWIntrusiveReference
{
private:
	typedef COWIntrusiveReference this_type;
public:
	typedef T element_type;

	COWIntrusiveReference(): m_pObj(0)
	{
	}
	COWIntrusiveReference(T * p, bool addRef = true): m_pObj(p)
	{
		if (m_pObj != 0 && addRef) COWIntrusiveReferenceAddRef(m_pObj);
	}
	template<class U> COWIntrusiveReference(COWIntrusiveReference<U> const & rhs): m_pObj(rhs.m_pObj)
	{
		if (m_pObj != 0) COWIntrusiveReferenceAddRef(m_pObj);
	}
	COWIntrusiveReference(COWIntrusiveReference const & rhs): m_pObj(rhs.m_pObj)
	{
		if (m_pObj != 0) COWIntrusiveReferenceAddRef(m_pObj);
	}
	~COWIntrusiveReference()
	{
		if (m_pObj != 0) COWIntrusiveReferenceRelease(m_pObj);
	}
	template<class U> COWIntrusiveReference & operator=(COWIntrusiveReference<U> const & rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}
	COWIntrusiveReference & operator=(COWIntrusiveReference const & rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}
	COWIntrusiveReference & operator=(T * rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}
	const T * getPtr() const
	{
		return m_pObj;
	}
	
	const T & operator*() const
	{
#ifdef OW_CHECK_NULL_REFERENCES
		ReferenceHelpers::checkNull(this);
		ReferenceHelpers::checkNull(m_pObj);
#endif
		return *m_pObj;
	}
	
	const T * operator->() const
	{
#ifdef OW_CHECK_NULL_REFERENCES
		ReferenceHelpers::checkNull(this);
		ReferenceHelpers::checkNull(m_pObj);
#endif
		return m_pObj;
	}

	T & operator*()
	{
#ifdef OW_CHECK_NULL_REFERENCES
		ReferenceHelpers::checkNull(this);
		ReferenceHelpers::checkNull(m_pObj);
#endif
		getWriteLock();
		return *m_pObj;
	}
	
	T * operator->()
	{
#ifdef OW_CHECK_NULL_REFERENCES
		ReferenceHelpers::checkNull(this);
		ReferenceHelpers::checkNull(m_pObj);
#endif
		getWriteLock();
		return m_pObj;
	}

	typedef T * this_type::*unspecified_bool_type;
	operator unspecified_bool_type () const
	{
		return m_pObj == 0? 0: &this_type::m_pObj;
	}

	bool operator! () const
	{
		return m_pObj == 0;
	}
	
	void swap(COWIntrusiveReference & rhs)
	{
		T * tmp = m_pObj;
		m_pObj = rhs.m_pObj;
		rhs.m_pObj = tmp;
	}

#if !defined(__GNUC__) || __GNUC__ > 2 // causes gcc 2.95 to ICE
	/* This is so the templated constructor will work */
	template <class U> friend class COWIntrusiveReference;
private:
#endif

	void getWriteLock()
	{
		if ((m_pObj != 0) && !COWIntrusiveReferenceUnique(m_pObj))
		{
			m_pObj = COWIntrusiveReferenceClone(m_pObj);
		}
	}


	T * m_pObj;
};
template<class T, class U> inline bool operator==(COWIntrusiveReference<T> const & a, COWIntrusiveReference<U> const & b)
{
	return a.getPtr() == b.getPtr();
}
template<class T, class U> inline bool operator!=(COWIntrusiveReference<T> const & a, COWIntrusiveReference<U> const & b)
{
	return a.getPtr() != b.getPtr();
}
template<class T> inline bool operator==(COWIntrusiveReference<T> const & a, const T * b)
{
	return a.getPtr() == b;
}
template<class T> inline bool operator!=(COWIntrusiveReference<T> const & a, const T * b)
{
	return a.getPtr() != b;
}
template<class T> inline bool operator==(const T * a, COWIntrusiveReference<T> const & b)
{
	return a == b.getPtr();
}
template<class T> inline bool operator!=(const T * a, COWIntrusiveReference<T> const & b)
{
	return a != b.getPtr();
}
#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96
// Resolve the ambiguity between our op!= and the one in rel_ops
template<class T> inline bool operator!=(COWIntrusiveReference<T> const & a, COWIntrusiveReference<T> const & b)
{
	return a.getPtr() != b.getPtr();
}
#endif
template<class T> inline bool operator<(COWIntrusiveReference<T> const & a, COWIntrusiveReference<T> const & b)
{
	return a.getPtr() < b.getPtr();
}
template<class T> void swap(COWIntrusiveReference<T> & lhs, COWIntrusiveReference<T> & rhs)
{
	lhs.swap(rhs);
}

} // end namespace OW_NAMESPACE

#endif

