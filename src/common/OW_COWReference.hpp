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
void OW_COWRefSwap(T& x, T&y)
{
    T t = x;
    x = y;
    y = t;
}

//////////////////////////////////////////////////////////////////////////////
// This class contains the non-templated code for OW_COWReference, to help 
// minimize code bloat.
class OW_COWReferenceBase
{
protected:
    OW_COWReferenceBase()
        : m_pRefCount(new OW_RefCount) {}

    OW_COWReferenceBase(const OW_COWReferenceBase& arg)
        : m_pRefCount(arg.m_pRefCount)
    {
        m_pRefCount->inc();
    }

    void incRef()
    {
    	if(m_pRefCount)
    	{
            m_pRefCount->inc();
    	}
    }
	
    bool decRef()
    {
        if (m_pRefCount->decAndTest())
        {
            delete m_pRefCount;
            m_pRefCount = 0;
            return true;
        }
        return false;
    }

    // returns true if a copy needs to be made
    bool getWriteLock()
    {
        if (m_pRefCount->decAndTest())
        {
            // only copy--don't need to clone
            incRef();
            return false;
        }
        else
        {
            // need to become unique and clone
    		m_pRefCount = new OW_RefCount;
            return true;
        }
    }
    
    void swap(OW_COWReferenceBase& arg)
    {
        OW_COWRefSwap(m_pRefCount, arg.m_pRefCount);
    }

protected:
    OW_RefCount* volatile m_pRefCount;

};


//////////////////////////////////////////////////////////////////////////////
template<class T>
class OW_COWReference : private OW_COWReferenceBase
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

		OW_COWReference<T>& operator= (OW_COWReference<T> arg);
		OW_COWReference<T>& operator= (T* newObj);
        void swap(OW_COWReference<T>& arg);

		T* operator->();
		T& operator*();
		T* getPtr();
		const T* operator->() const;
		const T& operator*() const;
		const T* getPtr() const;
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

	private:
		T* volatile m_pObj;
		/* This is so the templated constructor will work */
		template <class U> friend class OW_COWReference;

		void decRef();
#ifdef OW_CHECK_NULL_REFERENCES
		void checkNull() const;
#endif
		void getWriteLock();
};


//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>::OW_COWReference()
	: OW_COWReferenceBase(), m_pObj(0)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>::OW_COWReference(T* ptr)
	: OW_COWReferenceBase(), m_pObj(ptr)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>::OW_COWReference(const OW_COWReference<T>& arg)
	: OW_COWReferenceBase(arg), m_pObj(arg.m_pObj)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
template<class U>
inline OW_COWReference<T>::OW_COWReference(const OW_COWReference<U>& arg)
	: OW_COWReferenceBase(arg), m_pObj(arg.m_pObj)
{
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
inline void OW_COWReference<T>::decRef()
{
	typedef char type_must_be_complete[sizeof(T)];
    if (OW_COWReferenceBase::decRef())
    {
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
    if (OW_COWReferenceBase::getWriteLock())
    {
        if (m_pObj)
        {
            m_pObj = OW_COWReferenceClone(m_pObj);
        }
        else
        {
            m_pObj = 0;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_COWReference<T>& OW_COWReference<T>::operator= (OW_COWReference<T> arg)
{
    arg.swap(*this);
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
template <class T>
void OW_COWReference<T>::swap(OW_COWReference<T>& arg)
{
    OW_COWReferenceBase::swap(arg);
    OW_COWRefSwap(m_pObj, arg.m_pObj);
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
inline const T* OW_COWReference<T>::getPtr() const
{
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T* OW_COWReference<T>::getPtr()
{
    getWriteLock();
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline bool OW_COWReference<T>::isNull() const
{
	return (m_pObj == 0);
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
	}
}
#endif

template <class T>
template <class U>
inline OW_COWReference<U>
OW_COWReference<T>::cast_to()
{
	OW_COWReference<U> rval;
	rval.m_pObj = dynamic_cast<U*>(m_pObj);
	if (rval.m_pObj)
	{
		incRef();
		rval.m_pRefCount = m_pRefCount;
	}
	return rval;
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

#endif	// OW_COWREFERENCE_HPP_

