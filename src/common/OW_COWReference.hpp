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

#ifndef OW_COWREFERENCE_HPP_
#define OW_COWREFERENCE_HPP_

#include "OW_config.h"
#include "OW_RefCount.hpp"
#ifdef OW_CHECK_NULL_REFERENCES
#include "OW_Exception.hpp"
#endif

#ifdef OW_DEBUG
#include <cassert>
#endif

//////////////////////////////////////////////////////////////////////////////
template<class T>
class OW_COWReference
{
	public:

		OW_COWReference();
		explicit OW_COWReference(T* ptr);
		OW_COWReference(const OW_COWReference<T>& arg);
		
		/* construct out of a reference to a derived type.  U should be
		derived from T */
		template <class U>
		OW_COWReference(const OW_COWReference<U>& arg);

		~OW_COWReference();

		OW_COWReference<T>& operator= (const OW_COWReference<T>& arg);
		OW_COWReference<T>& operator= (T* newObj);
        void swap(OW_COWReference<T>& arg);

		T* operator->();
		T& operator*();
		const T* operator->() const;
		const T& operator*() const;
		T* getPtr() const;
		bool isNull() const;

	private:
		struct dummy
		{
			void nonnull() {};
		};
	
		typedef void (dummy::*safe_bool)();
	
	public:
		operator safe_bool () const
			{  return (!isNull()) ? &dummy::nonnull : 0; }
		safe_bool operator!() const
			{  return (!isNull()) ? 0: &dummy::nonnull; }

		template <class U>
		OW_COWReference<U> cast_to();

		template <class U>
		void useRefCountOf(const OW_COWReference<U>&);

	private:
		T* volatile m_pObj;
		OW_RefCount* volatile m_pRefCount;
		/* This is so the templated constructor will work */
		template <class U> friend class OW_COWReference;

		void incRef();
		void decRef();
#ifdef OW_CHECK_NULL_REFERENCES
		void checkNull() const;
#endif
		void getWriteLock();
};


//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>::OW_COWReference()
	: m_pObj(0), m_pRefCount(new OW_RefCount)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>::OW_COWReference(T* ptr)
	: m_pObj(ptr), m_pRefCount(new OW_RefCount)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>::OW_COWReference(const OW_COWReference<T>& arg)
	: m_pObj(arg.m_pObj), m_pRefCount(arg.m_pRefCount)
{
    m_pRefCount->inc();
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
template<class U>
inline OW_COWReference<T>::OW_COWReference(const OW_COWReference<U>& arg)
	: m_pObj(arg.m_pObj), m_pRefCount(arg.m_pRefCount)
{
    m_pRefCount->inc();
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>::~OW_COWReference()
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
inline void OW_COWReference<T>::incRef()
{
    m_pRefCount->inc();
}
	
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline void OW_COWReference<T>::decRef()
{
	typedef char type_must_be_complete[sizeof(T)];
    if (m_pRefCount->decAndTest())
    {
        delete m_pRefCount;
        m_pRefCount = 0;
        delete m_pObj;
        m_pObj = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////
template <class T>
T* OW_COWReferenceClone(T* obj)
{
    // default implementation.  If a certain class doesn't have clone()
    // (like std::vector), then they can overload this function
    return obj->clone();
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline void OW_COWReference<T>::getWriteLock()
{
    if (m_pRefCount->decAndTest())
    {
        incRef();
        return;
    }
    else
    {
		m_pObj = OW_COWReferenceClone(m_pObj);
		m_pRefCount = new OW_RefCount;
    }
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>& OW_COWReference<T>::operator= (const OW_COWReference<T>& arg)
{
    OW_COWReference<T>(arg).swap(*this);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>& OW_COWReference<T>::operator= (T* newObj)
{
    OW_COWReference<T>(newObj).swap(*this);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
void OW_COWRefSwap(T& x, T&y)
{
    T t = x;
    x = y;
    y = t;
}

//////////////////////////////////////////////////////////////////////////////
template <class T>
void OW_COWReference<T>::swap(OW_COWReference<T>& arg)
{
    OW_COWRefSwap(m_pObj, arg.m_pObj);
    OW_COWRefSwap(m_pRefCount, arg.m_pRefCount);
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T* OW_COWReference<T>::operator->()
{
    getWriteLock();
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T& OW_COWReference<T>::operator*()
{
    getWriteLock();
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return *(m_pObj);
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline const T* OW_COWReference<T>::operator->() const
{
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline const T& OW_COWReference<T>::operator*() const
{
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return *(m_pObj);
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T* OW_COWReference<T>::getPtr() const
{
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline bool OW_COWReference<T>::isNull() const
{
	return (m_pObj == (T*)0);
}

//////////////////////////////////////////////////////////////////////////////
#ifdef OW_CHECK_NULL_REFERENCES
template<class T>
inline void OW_COWReference<T>::checkNull() const
{
	if (this == 0 || isNull())
	{
#ifdef OW_DEBUG
		assert(0); // segfault so we can get a core
#endif
		OW_THROW(OW_Exception, "NULL OW_COWReference dereferenced");
		//throw OW_Exception(__FILE__, __LINE__,
			//"NULL OW_COWReference dereferenced");
	}
}
#endif

template <class T>
template <class U>
inline OW_COWReference<U>
OW_COWReference<T>::cast_to()
{
	incRef();
	OW_COWReference<U> rval;
	rval.m_pObj = reinterpret_cast<U*>(m_pObj);
	rval.m_pRefCount = m_pRefCount;
	return rval;
}

template <class T>
template <class U>
inline void
OW_COWReference<T>::useRefCountOf(const OW_COWReference<U>& arg)
{
    if (m_pRefCount->decAndTest())
    {
        delete m_pRefCount;
        m_pRefCount = 0;
    }
	m_pRefCount = arg.m_pRefCount;
	incRef();
}

//////////////////////////////////////////////////////////////////////////////
// Comparisons
template <class T, class U>
inline bool operator==(const OW_COWReference<T>& a, const OW_COWReference<U>& b)
{
	return a.getPtr() == b.getPtr();
}

template <class T, class U>
inline bool operator!=(const OW_COWReference<T>& a, const OW_COWReference<U>& b)
{
	return a.getPtr() != b.getPtr();
}

#endif	// OW_REFERENCE_HPP_

