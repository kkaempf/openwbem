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

#ifndef __OW_ARRAY_HPP__
#define __OW_ARRAY_HPP__


#include "OW_config.h"

#include "OW_Reference.hpp"
#include "OW_MutexLock.hpp"

#ifdef OW_NEW
#undef new
#endif

#include <vector>

#ifdef OW_NEW
#define new OW_NEW
#endif

#include "OW_Types.h"

DEFINE_EXCEPTION(OutOfBounds);

/**
 * The OW_Array class is essentially takes the vector class of the stl and
 * adds ref counting and copy on write capability.
 */
template<class T> class OW_Array
{
	typedef std::vector<T> V;
	OW_Reference<V> m_impl;

public:
	typedef typename V::value_type value_type;
	typedef typename V::pointer pointer;
	typedef typename V::const_pointer const_pointer;
	typedef typename V::iterator iterator;
	typedef typename V::const_iterator const_iterator;
	typedef typename V::reference reference;
	typedef typename V::const_reference const_reference;
	typedef typename V::size_type size_type;
	typedef typename V::difference_type difference_type;
	typedef typename V::reverse_iterator reverse_iterator;
	typedef typename V::const_reverse_iterator const_reverse_iterator;

	OW_Array() : m_impl(new V) {}
	OW_Array(const OW_Array<T>& arg) : m_impl(arg.m_impl) {}
	~OW_Array() {}
	explicit OW_Array(V* toWrap) : m_impl(toWrap) {}
	OW_Array(size_type n, const T& value) : m_impl(new V(n, value)) {}
	OW_Array(int n, const T& value) : m_impl(new V(n, value)) {}
	OW_Array(long n, const T& value) : m_impl(new V(n, value)) {}
	explicit OW_Array(size_type n) : m_impl(new V(n)) {}

	OW_Array<T>& operator= (const OW_Array<T>& arg)
	{
		m_impl = arg.m_impl;
		return *this;
	}

	template<class InputIterator>
	OW_Array(InputIterator first, InputIterator last) : m_impl(new V(first, last)) { }

	iterator begin() 
		{ OW_MutexLock mlock = m_impl.getWriteLock(); return m_impl->begin(); }
	const_iterator begin() const { return m_impl->begin(); }
	iterator end() 
		{ OW_MutexLock mlock = m_impl.getWriteLock(); return m_impl->end(); }
	const_iterator end() const { return m_impl->end(); }
	reverse_iterator rbegin() 
		{ OW_MutexLock mlock = m_impl.getWriteLock(); return m_impl->rbegin(); }
	const_reverse_iterator rbegin() const { return m_impl->rbegin(); }
	reverse_iterator rend() 
		{ OW_MutexLock mlock = m_impl.getWriteLock(); return m_impl->rend(); }
	const_reverse_iterator rend() const { return m_impl->rend(); }
	size_type size() const { return m_impl->size(); }
	size_type max_size() const { return m_impl->max_size(); }
	size_type capacity() const { return m_impl->capacity(); }
	OW_Bool empty() const { return m_impl->empty(); }
	reference operator[](size_type n)
	{
		OW_MutexLock mlock = m_impl.getWriteLock();
#ifdef OW_CHECK_ARRAY_INDEXING
		checkValidIndex(n);
#endif
		return m_impl->operator[](n);
	}

	OW_Array<T>& operator+= (const T& x)
	{
		OW_MutexLock mlock = m_impl.getWriteLock();
		m_impl->push_back(x);
	}

	const_reference operator[](size_type n) const
	{
#ifdef OW_CHECK_ARRAY_INDEXING
		checkValidIndex(n);
#endif
		return m_impl->operator[](n);
	}
	void reserve(size_type n) { OW_MutexLock mlock = m_impl.getWriteLock(); m_impl->reserve(n); }
	reference front() { return m_impl->front(); }
	const_reference front() const { return m_impl->front(); }
	reference back() { return m_impl->back(); }
	const_reference back() const { return m_impl->back(); }
	void push_back(const T& x) { OW_MutexLock mlock = m_impl.getWriteLock(); m_impl->push_back(x); }
	void append(const T& x) { push_back(x); }
	void swap(OW_Array<T>& x) { OW_MutexLock mlock = m_impl.getWriteLock(); m_impl->swap(*x.m_impl); }
	iterator insert(iterator position, const T& x)
		{ OW_MutexLock mlock = m_impl.getWriteLock(); return m_impl->insert(position, x); }

	void insert(size_t position, const T& x)
		{ OW_MutexLock mlock = m_impl.getWriteLock(); m_impl->insert(m_impl->begin() + position, x); }

	void remove(size_t index)
	{
		OW_MutexLock mlock = m_impl.getWriteLock();
#ifdef OW_CHECK_ARRAY_INDEXING
		checkValidIndex(index);
#endif
		m_impl->erase(m_impl->begin() + index);
	}
	void remove(size_t begin, size_t end)
	{
		OW_MutexLock mlock = m_impl.getWriteLock();
#ifdef OW_CHECK_ARRAY_INDEXING
		checkValidIndex(begin);
		checkValidIndex(end - 1);
#endif
		m_impl->erase(m_impl->begin() + begin, m_impl->begin() + end);
	}

	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last)
	{
		OW_MutexLock mlock = m_impl.getWriteLock();
		m_impl->insert(position, first, last);
	}

	void appendArray(const OW_Array<T>& x)
	{
		insert(end(), x.begin(), x.end());
	}

	void pop_back() { OW_MutexLock mlock = m_impl.getWriteLock(); m_impl->pop_back(); }

	iterator erase(iterator position) { OW_MutexLock mlock = m_impl.getWriteLock(); return m_impl->erase(position); }
	iterator erase(iterator first, iterator last) { OW_MutexLock mlock = m_impl.getWriteLock(); return m_impl->erase(first, last); }
	void resize(size_type new_size, const T& x) { OW_MutexLock mlock = m_impl.getWriteLock(); m_impl->resize(new_size, x); }
	void resize(size_type new_size) { OW_MutexLock mlock = m_impl.getWriteLock(); m_impl->resize(new_size); }
	void clear() { OW_MutexLock mlock = m_impl.getWriteLock(); m_impl->clear(); }

	friend OW_Bool operator== <>(const OW_Array<T>& x, const OW_Array<T>& y);
	friend OW_Bool operator< <>(const OW_Array<T>& x, const OW_Array<T>& y);

private:
#ifdef OW_CHECK_ARRAY_INDEXING
	void checkValidIndex(size_t index) const
	{
		if (index >= size())
		{
			OW_THROW(OW_OutOfBoundsException,
				"Array Index out of bounds");
		}
	}
#endif
};

template<class T>
inline OW_Bool operator==(const OW_Array<T>& x, const OW_Array<T>& y)
{
	return *x.m_impl == *y.m_impl;
}

template<class T>
inline OW_Bool operator<(const OW_Array<T>& x, const OW_Array<T>& y)
{
	return *x.m_impl < *y.m_impl;
}

template<class T, class Alloc>
inline void swap(OW_Array<T>& x, OW_Array<T>& y)
{
	x.swap(y);
}

typedef OW_Array<OW_Byte>       OW_ByteArray;
typedef OW_Array<OW_UInt8>      OW_UInt8Array;
typedef OW_Array<OW_Int8>       OW_Int8Array;
typedef OW_Array<OW_UInt16>     OW_UInt16Array;
typedef OW_Array<OW_Int16>      OW_Int16Array;
typedef OW_Array<OW_UInt32>     OW_UInt32Array;
typedef OW_Array<OW_Int32>      OW_Int32Array;
typedef OW_Array<OW_UInt64>     OW_UInt64Array;
typedef OW_Array<OW_Int64>      OW_Int64Array;
typedef OW_Array<OW_Real64>     OW_Real64Array;
typedef OW_Array<OW_Real32>     OW_Real32Array;
typedef OW_Array<OW_Bool>       OW_BoolArray;


#endif	// __OW_ARRAY_HPP__
	

