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
#include "OW_MutexLock.hpp"
#include "OW_Exception.hpp"

#include <iostream>
//////////////////////////////////////////////////////////////////////////////

struct OW_RefCount
{
	OW_RefCount();
	OW_Mutex m_mutex;
	unsigned long volatile m_count;
};

//////////////////////////////////////////////////////////////////////////////
template<class T>
class OW_Reference
{
	public:

		OW_Reference();
		explicit OW_Reference(T* ptr);
		OW_Reference(T* ptr, OW_Bool noDelete);
		OW_Reference(const OW_Reference<T>& arg);
		
		/* construct out of a reference to a derived type.  U should be
		derived from T */
		template <class U>
		OW_Reference(const OW_Reference<U>& arg);

		~OW_Reference();

		void incRef();
		void decRef();

		OW_MutexLock getWriteLock();

		OW_Reference<T>& operator= (const OW_Reference<T> arg);
		OW_Reference<T>& operator= (T* newObj);
		T* operator->() const;
		T& operator*() const;
		T* getPtr() const;
		OW_Bool isNull() const;
		operator void*() const;

		template <class U>
		OW_Reference<U> cast_to();

		template <class U>
		void useRefCountOf(const OW_Reference<U>&);

	private:
		T* volatile m_pObj;
		OW_RefCount* volatile m_pRefCount;
		/* This is so the templated constructor will work */
		template <class U> friend class OW_Reference;

#ifdef OW_CHECK_NULL_REFERENCES
		void checkNull() const;
#endif
};


//////////////////////////////////////////////////////////////////////////////
inline OW_RefCount::OW_RefCount()
	: m_mutex(), m_count(1L)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Reference<T>::OW_Reference()
	: m_pObj(0), m_pRefCount(0)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Reference<T>::OW_Reference(T* ptr)
	: m_pObj(ptr), m_pRefCount((ptr != 0) ? new OW_RefCount : 0)
{
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Reference<T>::OW_Reference(T* ptr, OW_Bool noDelete)
	: m_pObj(ptr), m_pRefCount(0)
{
	if(ptr != 0 && !noDelete)
	{
		m_pRefCount = new OW_RefCount;
	}
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Reference<T>::OW_Reference(const OW_Reference<T>& arg)
	: m_pObj(arg.m_pObj), m_pRefCount(0)
{
	if(arg.m_pRefCount)
	{
		m_pRefCount = arg.m_pRefCount;
		OW_MutexLock ml(m_pRefCount->m_mutex);
		++(m_pRefCount->m_count);
	}
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
template<class U>
OW_Reference<T>::OW_Reference(const OW_Reference<U>& arg)
	: m_pObj(arg.m_pObj), m_pRefCount(0)
{
	if(arg.m_pRefCount)
	{
		m_pRefCount = arg.m_pRefCount;
		OW_MutexLock ml(m_pRefCount->m_mutex);
		++(m_pRefCount->m_count);
	}
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Reference<T>::~OW_Reference()
{
	decRef();
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
void OW_Reference<T>::incRef()
{
	if(m_pRefCount)
	{
		OW_MutexLock l(m_pRefCount->m_mutex);
		++(m_pRefCount->m_count);
	}
}
	
//////////////////////////////////////////////////////////////////////////////
template<class T>
void OW_Reference<T>::decRef()
{
	if(m_pRefCount)
	{
		OW_MutexLock l(m_pRefCount->m_mutex);
		if(m_pRefCount->m_count == 1)
		{
			l.release();
			delete m_pRefCount;
			m_pRefCount = 0;
			delete m_pObj;
			m_pObj = 0;
		}
		else
		{
			--(m_pRefCount->m_count);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_MutexLock OW_Reference<T>::getWriteLock()
{
	if(!m_pRefCount)
	{
		OW_THROW(OW_Exception, "writeLock not allowed on non-delete "
				"or NULL object");
	}

	OW_MutexLock l(m_pRefCount->m_mutex);
	if(m_pRefCount->m_count > 1)
	{
		--(m_pRefCount->m_count);
		m_pObj = new T(*m_pObj);
		m_pRefCount = new OW_RefCount;
	}

	return l;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Reference<T>& OW_Reference<T>::operator= (const OW_Reference<T> arg)
{
	if(arg.m_pRefCount != m_pRefCount
			|| (arg.m_pRefCount == 0 && m_pRefCount == 0))
	{
		decRef();
		m_pObj = arg.m_pObj;
		m_pRefCount = arg.m_pRefCount;
		incRef();
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Reference<T>& OW_Reference<T>::operator= (T* newObj)
{
	if(newObj != m_pObj)
	{
		decRef();
		m_pObj = newObj;
		m_pRefCount = new OW_RefCount;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
T* OW_Reference<T>::operator->() const
{
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
T& OW_Reference<T>::operator*() const
{
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return *(m_pObj);
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
T* OW_Reference<T>::getPtr() const
{
	return m_pObj;
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Bool OW_Reference<T>::isNull() const
{
	return (m_pObj == (T*)0);
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
OW_Reference<T>::operator void*() const
{
	return (void*)(m_pObj);
}

//////////////////////////////////////////////////////////////////////////////
#ifdef OW_CHECK_NULL_REFERENCES
template<class T>
void OW_Reference<T>::checkNull() const
{
	if (this == 0 || isNull())
	{
		OW_THROW(OW_Exception, "NULL OW_Reference dereferenced");
		//throw OW_Exception(__FILE__, __LINE__,
			//"NULL OW_Reference dereferenced");
	}
}
#endif

template <class T>
template <class U>
OW_Reference<U>
OW_Reference<T>::cast_to()
{
	incRef();
	OW_Reference<U> rval;
	rval.m_pObj = reinterpret_cast<U*>(m_pObj);
	rval.m_pRefCount = m_pRefCount;
	return rval;
}

template <class T>
template <class U>
void 
OW_Reference<T>::useRefCountOf(const OW_Reference<U>& arg)
{
	if(m_pRefCount)
	{
		OW_MutexLock l(m_pRefCount->m_mutex);
		if(m_pRefCount->m_count == 1)
		{
			l.release();
			delete m_pRefCount;
			m_pRefCount = 0;
		}
		else
		{
			--(m_pRefCount->m_count);
		}
	}
	m_pRefCount = arg.m_pRefCount;
	incRef();
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

