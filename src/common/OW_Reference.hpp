/*******************************************************************************
 * Copyright (C) 2001 Center 7, Inc All rights reserved.
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
 *  - Neither the name of Center 7 nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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

namespace OpenWBEM
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
class ReferenceBase
{
protected:
	ReferenceBase()
		: m_pRefCount(0) {}
	ReferenceBase(const void* ptr)
		: m_pRefCount((ptr != 0) ? new RefCount : 0) {}
	ReferenceBase(const void* ptr, bool noDelete)
		: m_pRefCount(0) 
	{
		if(ptr != 0 && !noDelete)
		{
			m_pRefCount = new RefCount;
		}
	}
	ReferenceBase(const ReferenceBase& arg)
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
				// TODO: Measure how much of a performance impact the following line has.
				m_pRefCount = 0;
				return true;
			}
		}
		return false;
	}
	void swap(ReferenceBase& arg)
	{
		RefSwap(m_pRefCount, arg.m_pRefCount);
	}
	void useRefCountOf(const ReferenceBase& arg)
	{
		/*
		if(m_pRefCount)
		{
			if (m_pRefCount->decAndTest())
			{
				delete m_pRefCount;
				m_pRefCount = 0;
			}
		}
		*/
		decRef();
		m_pRefCount = arg.m_pRefCount;
		incRef();
	}
protected:
	RefCount* volatile m_pRefCount;
};
//////////////////////////////////////////////////////////////////////////////
template<class T>
class Reference : private ReferenceBase
{
	public:
		Reference();
		explicit Reference(T* ptr);
		Reference(T* ptr, bool noDelete);
		Reference(const Reference<T>& arg);
		
		/* construct out of a reference to a derived type.  U should be
		derived from T */
		template <class U>
		Reference(const Reference<U>& arg);
		~Reference();
		Reference<T>& operator= (Reference<T> arg);
		Reference<T>& operator= (T* newObj);
		void swap(Reference<T>& arg);
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
		Reference<U> cast_to() const;
		template <class U>
		void useRefCountOf(const Reference<U>&);
	private:
		void decRef();
		T* volatile m_pObj;
		/* This is so the templated constructor will work */
		template <class U> friend class Reference;
#ifdef OW_CHECK_NULL_REFERENCES
		void checkNull() const;
#endif
};
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline Reference<T>::Reference()
	: ReferenceBase(), m_pObj(0)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline Reference<T>::Reference(T* ptr)
	: ReferenceBase(ptr), m_pObj(ptr)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline Reference<T>::Reference(T* ptr, bool noDelete)
	: ReferenceBase(ptr, noDelete), m_pObj(ptr)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline Reference<T>::Reference(const Reference<T>& arg)
	: ReferenceBase(arg), m_pObj(arg.m_pObj)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
template<class U>
inline Reference<T>::Reference(const Reference<U>& arg)
	: ReferenceBase(arg), m_pObj(arg.m_pObj)
{
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline Reference<T>::~Reference()
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
inline void Reference<T>::decRef()
{
	typedef char type_must_be_complete[sizeof(T)];
	if (ReferenceBase::decRef())
	{
		delete m_pObj;
		// TODO: Measure how much of a performance hit the following line has.
		m_pObj = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline Reference<T>& Reference<T>::operator= (Reference<T> arg)
{
	arg.swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline Reference<T>& Reference<T>::operator= (T* newObj)
{
	Reference<T>(newObj).swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
template <class T>
inline void Reference<T>::swap(Reference<T>& arg)
{
	ReferenceBase::swap(arg);
	RefSwap(m_pObj, arg.m_pObj);
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T* Reference<T>::operator->() const
{
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return m_pObj;
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T& Reference<T>::operator*() const
{
#ifdef OW_CHECK_NULL_REFERENCES
	checkNull();
#endif
	
	return *(m_pObj);
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline T* Reference<T>::getPtr() const
{
	return m_pObj;
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
inline bool Reference<T>::isNull() const
{
	return (m_pObj == 0);
}
//////////////////////////////////////////////////////////////////////////////
#ifdef OW_CHECK_NULL_REFERENCES
template<class T>
inline void Reference<T>::checkNull() const
{
	if (this == 0 || isNull())
	{
#ifdef OW_DEBUG
		assert(0); // segfault so we can get a core
#endif
		OW_THROW(Exception, "NULL Reference dereferenced");
	}
}
#endif
template <class T>
template <class U>
inline Reference<U>
Reference<T>::cast_to() const
{
	Reference<U> rval;
	rval.m_pObj = dynamic_cast<U*>(m_pObj);
	if (rval.m_pObj)
	{
		rval.m_pRefCount = m_pRefCount;
		rval.incRef();
	}
	return rval;
}
template <class T>
template <class U>
inline void
Reference<T>::useRefCountOf(const Reference<U>& arg)
{
	ReferenceBase::useRefCountOf(arg);
}
//////////////////////////////////////////////////////////////////////////////
// Comparisons
template <class T, class U>
inline bool operator==(const Reference<T>& a, const Reference<U>& b)
{
	return a.getPtr() == b.getPtr();
}
template <class T, class U>
inline bool operator!=(const Reference<T>& a, const Reference<U>& b)
{
	return a.getPtr() != b.getPtr();
}
#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96
// Resolve the ambiguity between our op!= and the one in rel_ops
template <class T>
inline bool operator!=(const Reference<T>& a, const Reference<T>& b)
{
	return a.getPtr() != b.getPtr();
}
#endif
template <class T, class U>
inline bool operator<(const Reference<T>& a, const Reference<U>& b)
{
	return a.getPtr() < b.getPtr();
}

} // end namespace OpenWBEM

#endif	// OW_REFERENCE_HPP_
