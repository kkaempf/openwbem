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

#ifndef OWBI1_COWREFERENCE_HPP_INCLUDE_GUARD_
#define OWBI1_COWREFERENCE_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_COWReferenceBase.hpp"

namespace OWBI1
{

//////////////////////////////////////////////////////////////////////////////
template<class T>
class COWReference : private COWReferenceBase
{
	public:
		typedef T element_type;

		COWReference();
		explicit COWReference(T* ptr);
		COWReference(const COWReference<T>& arg);
		
		/* construct out of a reference to a derived type.  U should be
		derived from T */
		template <class U>
		COWReference(const COWReference<U>& arg);
		~COWReference();
		COWReference<T>& operator= (const COWReference<T>& arg);
		COWReference<T>& operator= (T* newObj);
		void swap(COWReference<T>& arg);
		T* operator->();
		T& operator*();
		const T* operator->() const;
		const T& operator*() const;
		const T* getPtr() const;

		typedef T* volatile COWReference::*safe_bool;
		operator safe_bool () const
			{  return m_pObj ? &COWReference::m_pObj : 0; }
		bool operator!() const
			{  return !m_pObj; }
		
		template <class U>
		COWReference<U> cast_to() const;

#if !defined(__GNUC__) || __GNUC__ > 2 // causes gcc 2.95 to ICE
		/* This is so the templated constructor will work */
		template <class U> friend class COWReference;
	private:
#endif
		T* volatile m_pObj;
		void decRef();
		void getWriteLock();
};
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline COWReference<T>::COWReference()
	: COWReferenceBase(), m_pObj(0)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline COWReference<T>::COWReference(T* ptr)
	: COWReferenceBase(), m_pObj(ptr)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline COWReference<T>::COWReference(const COWReference<T>& arg)
	: COWReferenceBase(arg), m_pObj(arg.m_pObj)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
template<class U>
inline COWReference<T>::COWReference(const COWReference<U>& arg)
	: COWReferenceBase(arg), m_pObj(arg.m_pObj)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline COWReference<T>::~COWReference()
{
	try
	{
		decRef();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline void COWReference<T>::decRef()
{
	typedef char type_must_be_complete[sizeof(T)];
	if (COWReferenceBase::decRef())
	{
		delete m_pObj;
		m_pObj = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline void COWReference<T>::getWriteLock()
{
	if (COWReferenceBase::refCountGreaterThanOne())
	{
		// this needs to happen first to avoid a race condition between 
		// another thread deleting the object and this one making a copy.
		T* tmp = OWBI1::COWReferenceClone(m_pObj);
		// this will decrement the count and then make a new one if we're making a copy.
		if (COWReferenceBase::getWriteLock())
		{
			delete tmp;
		}
		else
		{
			m_pObj = tmp;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline COWReference<T>& COWReference<T>::operator= (const COWReference<T>& arg)
{
	COWReference<T>(arg).swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline COWReference<T>& COWReference<T>::operator= (T* newObj)
{
	COWReference<T>(newObj).swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
template <class T>
inline void COWReference<T>::swap(COWReference<T>& arg)
{
	COWReferenceBase::swap(arg);
	OWBI1::COWRefSwap(m_pObj, arg.m_pObj);
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T* COWReference<T>::operator->()
{
#ifdef OWBI1_CHECK_NULL_REFERENCES
	checkNull(this);
	checkNull(m_pObj);
#endif
	getWriteLock();
	
	return m_pObj;
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T& COWReference<T>::operator*()
{
#ifdef OWBI1_CHECK_NULL_REFERENCES
	checkNull(this);
	checkNull(m_pObj);
#endif
	getWriteLock();
	
	return *(m_pObj);
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline const T* COWReference<T>::operator->() const
{
#ifdef OWBI1_CHECK_NULL_REFERENCES
	checkNull(this);
	checkNull(m_pObj);
#endif
	
	return m_pObj;
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline const T& COWReference<T>::operator*() const
{
#ifdef OWBI1_CHECK_NULL_REFERENCES
	checkNull(this);
	checkNull(m_pObj);
#endif
	
	return *(m_pObj);
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline const T* COWReference<T>::getPtr() const
{
	return m_pObj;
}
//////////////////////////////////////////////////////////////////////////////
template <class T>
template <class U>
inline COWReference<U>
COWReference<T>::cast_to() const
{
	COWReference<U> rval;
	rval.m_pObj = dynamic_cast<U*>(m_pObj);
	if (rval.m_pObj)
	{
		rval.useRefCountOf(*this);
		rval.incRef();
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
// Comparisons
template <class T, class U>
inline bool operator==(const COWReference<T>& a, const COWReference<U>& b)
{
	return a.getPtr() == b.getPtr();
}
//////////////////////////////////////////////////////////////////////////////
template <class T, class U>
inline bool operator!=(const COWReference<T>& a, const COWReference<U>& b)
{
	return a.getPtr() != b.getPtr();
}
//////////////////////////////////////////////////////////////////////////////
template <class T, class U>
inline bool operator<(const COWReference<T>& a, const COWReference<U>& b)
{
	return a.getPtr() < b.getPtr();
}

//////////////////////////////////////////////////////////////////////////////
template <class T>
inline T* COWReferenceClone(T* obj)
{
	// default implementation.  If a certain class doesn't have clone()
	// (like std::vector), then they can overload this function
	return obj->clone();
}

} // end namespace OWBI1

#endif	// OWBI1_COWREFERENCE_HPP_