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
 *******************************************************************************/

#ifndef OW_REFERENCE_HPP_
#define OW_REFERENCE_HPP_

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
void OW_RefSwap(T& x, T&y)
{
    T t = x;
    x = y;
    y = t;
}



//////////////////////////////////////////////////////////////////////////////
// This class contains the non-templated code for OW_Reference, to help 
// minimize code bloat.
class OW_ReferenceBase
{
protected:
    OW_ReferenceBase()
        : m_pRefCount(0) {}

    OW_ReferenceBase(const void* ptr)
        : m_pRefCount((ptr != 0) ? new OW_RefCount : 0) {}

    OW_ReferenceBase(const void* ptr, bool noDelete)
        : m_pRefCount(0) 
    {
        if(ptr != 0 && !noDelete)
        {
            m_pRefCount = new OW_RefCount;
        }
    }

    OW_ReferenceBase(const OW_ReferenceBase& arg)
        : m_pRefCount(0)
    {
    	if(arg.m_pRefCount)
    	{
    		m_pRefCount = arg.m_pRefCount;
            m_pRefCount->inc();
    	}
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
    	if(m_pRefCount)
    	{
            if (m_pRefCount->decAndTest())
    		{
    			delete m_pRefCount;
    			m_pRefCount = 0;
                return true;
    		}
    	}
        return false;
    }

    void swap(OW_ReferenceBase& arg)
    {
        OW_RefSwap(m_pRefCount, arg.m_pRefCount);
    }

    void useRefCountOf(const OW_ReferenceBase& arg)
    {
    	if(m_pRefCount)
    	{
            if (m_pRefCount->decAndTest())
    		{
    			delete m_pRefCount;
    			m_pRefCount = 0;
    		}
    	}
    	m_pRefCount = arg.m_pRefCount;
    	incRef();
    }

protected:
    OW_RefCount* volatile m_pRefCount;

};


//////////////////////////////////////////////////////////////////////////////
template<class T>
class OW_Reference : private OW_ReferenceBase
{
	public:

		OW_Reference();
		explicit OW_Reference(T* ptr);
		OW_Reference(T* ptr, bool noDelete);
		OW_Reference(const OW_Reference<T>& arg);
		
		/* construct out of a reference to a derived type.  U should be
		derived from T */
		template <class U>
		OW_Reference(const OW_Reference<U>& arg);

		~OW_Reference();
		OW_Reference<T>& operator= (OW_Reference<T> arg);
		OW_Reference<T>& operator= (T* newObj);
        void swap(OW_Reference<T>& arg);

		T* operator->() const;
		T& operator*() const;
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
		OW_Reference<U> cast_to();

		template <class U>
		void useRefCountOf(const OW_Reference<U>&);

	private:
		void decRef();

		T* volatile m_pObj;
		/* This is so the templated constructor will work */
		template <class U> friend class OW_Reference;

#ifdef OW_CHECK_NULL_REFERENCES
		void checkNull() const;
#endif
};


//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_Reference<T>::OW_Reference()
	: OW_ReferenceBase(), m_pObj(0)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_Reference<T>::OW_Reference(T* ptr)
	: OW_ReferenceBase(ptr), m_pObj(ptr)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_Reference<T>::OW_Reference(T* ptr, bool noDelete)
	: OW_ReferenceBase(ptr, noDelete), m_pObj(ptr)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_Reference<T>::OW_Reference(const OW_Reference<T>& arg)
	: OW_ReferenceBase(arg), m_pObj(arg.m_pObj)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
template<class U>
inline OW_Reference<T>::OW_Reference(const OW_Reference<U>& arg)
	: OW_ReferenceBase(arg), m_pObj(arg.m_pObj)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_Reference<T>::~OW_Reference()
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
inline void OW_Reference<T>::decRef()
{
	typedef char type_must_be_complete[sizeof(T)];
    if (OW_ReferenceBase::decRef())
    {
        delete m_pObj;
        m_pObj = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_Reference<T>& OW_Reference<T>::operator= (OW_Reference<T> arg)
{
    arg.swap(*this);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline OW_Reference<T>& OW_Reference<T>::operator= (T* newObj)
{
    OW_Reference<T>(newObj).swap(*this);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
template <class T>
inline void OW_Reference<T>::swap(OW_Reference<T>& arg)
{
    OW_ReferenceBase::swap(arg);
    OW_RefSwap(m_pObj, arg.m_pObj);
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T* OW_Reference<T>::operator->() const
{
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T& OW_Reference<T>::operator*() const
{
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return *(m_pObj);
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T* OW_Reference<T>::getPtr() const
{
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
inline bool OW_Reference<T>::isNull() const
{
	return (m_pObj == (T*)0);
}

//////////////////////////////////////////////////////////////////////////////
#ifdef OW_CHECK_NULL_REFERENCES
template<class T>
inline void OW_Reference<T>::checkNull() const
{
	if (this == 0 || isNull())
	{
#ifdef OW_DEBUG
		assert(0); // segfault so we can get a core
#endif
		OW_THROW(OW_Exception, "NULL OW_Reference dereferenced");
	}
}
#endif

template <class T>
template <class U>
inline OW_Reference<U>
OW_Reference<T>::cast_to()
{
	OW_Reference<U> rval;
	rval.m_pObj = dynamic_cast<U*>(m_pObj);
	if (rval.m_pObj)
	{
		incRef();
		rval.m_pRefCount = m_pRefCount;
	}
	return rval;
}

template <class T>
template <class U>
inline void
OW_Reference<T>::useRefCountOf(const OW_Reference<U>& arg)
{
    OW_ReferenceBase::useRefCountOf(arg);
}

//////////////////////////////////////////////////////////////////////////////
// Comparisons
template <class T, class U>
inline bool operator==(const OW_Reference<T>& a, const OW_Reference<U>& b)
{
	return a.getPtr() == b.getPtr();
}

template <class T, class U>
inline bool operator!=(const OW_Reference<T>& a, const OW_Reference<U>& b)
{
	return a.getPtr() != b.getPtr();
}

#endif	// OW_REFERENCE_HPP_

