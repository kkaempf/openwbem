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

#ifndef OW_ARRAY_HPP_
#define OW_ARRAY_HPP_


#include "OW_config.h"

#include "OW_COWReference.hpp"

#ifdef OW_NEW
#undef new
#endif

#include <vector>

#ifdef OW_NEW
#define new OW_NEW
#endif

#include "OW_Types.h"

class OW_BinIfcIO;

#include <iosfwd>
#ifdef OW_DEBUG
#include <cassert>
#endif

#ifdef OW_CHECK_ARRAY_INDEXING
#include "OW_Exception.hpp"
DEFINE_EXCEPTION(OutOfBounds);
#endif

/**
 * The OW_Array class is essentially takes the vector class of the stl and
 * adds ref counting and copy on write capability.
 */
template<class T> class OW_Array
{
	typedef std::vector<T> V;
	OW_COWReference<V> m_impl;

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
	//OW_Array(const OW_Array<T>& arg) : m_impl(arg.m_impl) {}
	~OW_Array() {}
	explicit OW_Array(V* toWrap) : m_impl(toWrap) {}
	OW_Array(size_type n, const T& value) : m_impl(new V(n, value)) {}
	OW_Array(int n, const T& value) : m_impl(new V(n, value)) {}
	OW_Array(long n, const T& value) : m_impl(new V(n, value)) {}
	explicit OW_Array(size_type n) : m_impl(new V(n)) {}

	//OW_Array<T>& operator= (const OW_Array<T>& arg)
	//{
		//m_impl = arg.m_impl;
		//return *this;
	//}

	template<class InputIterator>
	OW_Array(InputIterator first, InputIterator last) : m_impl(new V(first, last)) { }

	iterator begin()
		{ return m_impl->begin(); }
	const_iterator begin() const { return m_impl->begin(); }
	iterator end()
		{ return m_impl->end(); }
	const_iterator end() const { return m_impl->end(); }
	reverse_iterator rbegin()
		{ return m_impl->rbegin(); }
	const_reverse_iterator rbegin() const { return m_impl->rbegin(); }
	reverse_iterator rend()
		{ return m_impl->rend(); }
	const_reverse_iterator rend() const { return m_impl->rend(); }
	size_type size() const { return m_impl->size(); }
	size_type max_size() const { return m_impl->max_size(); }
	size_type capacity() const { return m_impl->capacity(); }
	bool empty() const { return m_impl->empty(); }
	reference operator[](size_type n)
	{
#ifdef OW_CHECK_ARRAY_INDEXING
		checkValidIndex(n);
#endif
		return m_impl->operator[](n);
	}

	OW_Array<T>& operator+= (const T& x)
	{
		m_impl->push_back(x);
	}

	const_reference operator[](size_type n) const
	{
#ifdef OW_CHECK_ARRAY_INDEXING
		checkValidIndex(n);
#endif
		return m_impl->operator[](n);
	}
	void reserve(size_type n) { m_impl->reserve(n); }
	reference front() { return m_impl->front(); }
	const_reference front() const { return m_impl->front(); }
	reference back() { return m_impl->back(); }
	const_reference back() const { return m_impl->back(); }
	void push_back(const T& x) { m_impl->push_back(x); }
	void append(const T& x) { push_back(x); }
	void swap(OW_Array<T>& x) { m_impl.swap(x.m_impl); }
	iterator insert(iterator position, const T& x)
		{ return m_impl->insert(position, x); }

	void insert(size_t position, const T& x)
		{ m_impl->insert(m_impl->begin() + position, x); }

	void remove(size_t index)
	{
#ifdef OW_CHECK_ARRAY_INDEXING
		checkValidIndex(index);
#endif
		m_impl->erase(m_impl->begin() + index);
	}
	void remove(size_t begin, size_t end)
	{
#ifdef OW_CHECK_ARRAY_INDEXING
		checkValidIndex(begin);
		checkValidIndex(end - 1);
#endif
		m_impl->erase(m_impl->begin() + begin, m_impl->begin() + end);
	}

	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last)
	{
		m_impl->insert(position, first, last);
	}

	void appendArray(const OW_Array<T>& x)
	{
		insert(end(), x.begin(), x.end());
	}

	void pop_back() { m_impl->pop_back(); }

	iterator erase(iterator position) { return m_impl->erase(position); }
	iterator erase(iterator first, iterator last) { return m_impl->erase(first, last); }
	void resize(size_type new_size, const T& x) { m_impl->resize(new_size, x); }
	void resize(size_type new_size) { m_impl->resize(new_size); }
	void clear() { m_impl->clear(); }

	void readObject(std::istream& istr)
	{
		m_impl->clear();
		OW_UInt32 len;
		OW_BinIfcIO::read(istr, len);
		
		m_impl->reserve(len);
		for(OW_UInt32 i = 0; i < len; i++)
		{
			T x;
			x.readObject(istr);
			m_impl->push_back(x);
		}

	}

	void writeObject(std::ostream& ostrm) const
	{
		OW_UInt32 len = m_impl->size();
		OW_BinIfcIO::write(ostrm, len);
		for(OW_UInt32 i = 0; i < len; i++)
		{
			m_impl->operator[](i).writeObject(ostrm);
		}
	}

	friend bool operator== <>(const OW_Array<T>& x, const OW_Array<T>& y);
	friend bool operator< <>(const OW_Array<T>& x, const OW_Array<T>& y);

private:
#ifdef OW_CHECK_ARRAY_INDEXING
	void checkValidIndex(size_t index) const
	{
		if (index >= size())
		{
#ifdef OW_DEBUG
			assert(0); // segfault so we can get a core
#endif
			OW_THROW(OW_OutOfBoundsException,
				"Array Index out of bounds");
		}
	}
#endif
};

template <class T>
std::vector<T>* OW_COWReferenceClone(std::vector<T>* obj)
{
    return new std::vector<T>(*obj);
}

template<class T>
inline bool operator==(const OW_Array<T>& x, const OW_Array<T>& y)
{
	return *x.m_impl == *y.m_impl;
}

template<class T>
inline bool operator<(const OW_Array<T>& x, const OW_Array<T>& y)
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


#endif
	

