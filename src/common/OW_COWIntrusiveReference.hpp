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
#include <functional>           // for std::less

namespace OpenWBEM
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

	COWIntrusiveReference(): p_(0)
	{
	}
	COWIntrusiveReference(T * p, bool add_ref = true): p_(p)
	{
		if(p_ != 0 && add_ref) COWIntrusiveReferenceAddRef(p_);
	}
	template<class U> COWIntrusiveReference(COWIntrusiveReference<U> const & rhs): p_(rhs.get())
	{
		if(p_ != 0) COWIntrusiveReferenceAddRef(p_);
	}
	COWIntrusiveReference(COWIntrusiveReference const & rhs): p_(rhs.p_)
	{
		if(p_ != 0) COWIntrusiveReferenceAddRef(p_);
	}
	~COWIntrusiveReference()
	{
		if(p_ != 0) COWIntrusiveReferenceRelease(p_);
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
	const T * get() const
	{
		return p_;
	}
	
	const T & operator*() const
	{
		return *p_;
	}
	
	const T * operator->() const
	{
		return p_;
	}

	T & operator*()
	{
		getWriteLock();
		return *p_;
	}
	
	T * operator->()
	{
		getWriteLock();
		return p_;
	}

	typedef T * this_type::*unspecified_bool_type;
	operator unspecified_bool_type () const
	{
		return p_ == 0? 0: &this_type::p_;
	}

	bool operator! () const
	{
		return p_ == 0;
	}
	
	void swap(COWIntrusiveReference & rhs)
	{
		T * tmp = p_;
		p_ = rhs.p_;
		rhs.p_ = tmp;
	}
private:
	void getWriteLock()
	{
		if ((p_ != 0) && !COWIntrusiveReferenceUnique(p_))
		{
			p_ = COWIntrusiveReferenceClone(p_);
		}
	}


	T * p_;
};
template<class T, class U> inline bool operator==(COWIntrusiveReference<T> const & a, COWIntrusiveReference<U> const & b)
{
	return a.get() == b.get();
}
template<class T, class U> inline bool operator!=(COWIntrusiveReference<T> const & a, COWIntrusiveReference<U> const & b)
{
	return a.get() != b.get();
}
template<class T> inline bool operator==(COWIntrusiveReference<T> const & a, T * b)
{
	return a.get() == b;
}
template<class T> inline bool operator!=(COWIntrusiveReference<T> const & a, T * b)
{
	return a.get() != b;
}
template<class T> inline bool operator==(T * a, COWIntrusiveReference<T> const & b)
{
	return a == b.get();
}
template<class T> inline bool operator!=(T * a, COWIntrusiveReference<T> const & b)
{
	return a != b.get();
}
#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96
// Resolve the ambiguity between our op!= and the one in rel_ops
template<class T> inline bool operator!=(COWIntrusiveReference<T> const & a, COWIntrusiveReference<T> const & b)
{
	return a.get() != b.get();
}
#endif
template<class T> inline bool operator<(COWIntrusiveReference<T> const & a, COWIntrusiveReference<T> const & b)
{
	return std::less<T *>()(a.get(), b.get());
}
template<class T> void swap(COWIntrusiveReference<T> & lhs, COWIntrusiveReference<T> & rhs)
{
	lhs.swap(rhs);
}
template<class T, class U> COWIntrusiveReference<T> static_pointer_cast(COWIntrusiveReference<U> const & p)
{
	return static_cast<T *>(p.get());
}
template<class T, class U> COWIntrusiveReference<T> const_pointer_cast(COWIntrusiveReference<U> const & p)
{
	return const_cast<T *>(p.get());
}
template<class T, class U> COWIntrusiveReference<T> dynamic_pointer_cast(COWIntrusiveReference<U> const & p)
{
	return dynamic_cast<T *>(p.get());
}

} // end namespace OpenWBEM

#endif

