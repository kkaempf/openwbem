/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_ARRAY_HPP_INCLUDE_GUARD_
#define OW_ARRAY_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ArrayFwd.hpp"
#include "OW_COWReference.hpp"
#include "OW_Types.hpp"
#include "OW_Exception.hpp"
#include <vector>

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(OutOfBounds);

#ifdef OW_CHECK_ARRAY_INDEXING
void throwArrayOutOfBoundsException(size_t size, size_t idx);
#endif

/**
 * Array<> wraps std::vector<> in COWReference<> adding ref counting and copy 
 * on write capability.  It also adds valid range checks to operator[] if
 * OW_CHECK_ARRAY_INDEXING is defined.
 *
 * Invariants: See std::vector<>
 * Thread safety: read
 * Copy semantics: Copy On Write
 * Exception safety: same as std::vector<T>
 */
template<class T> class Array
{
	typedef std::vector<T, std::allocator<T> > V;
	COWReference<V> m_impl;
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
	Array();
	~Array();
	explicit Array(V* toWrap);
	Array(size_type n, const T& value);
	Array(int n, const T& value);
	Array(long n, const T& value);
	explicit Array(size_type n);
	template<class InputIterator>
	Array(InputIterator first, InputIterator last);
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;
	size_type size() const;
	size_type max_size() const;
	size_type capacity() const;
	bool empty() const;
	reference operator[](size_type n);
	const_reference operator[](size_type n) const;
	Array<T>& operator+= (const T& x);
	void reserve(size_type n);
	reference front();
	const_reference front() const;
	reference back();
	const_reference back() const;
	void push_back(const T& x);
	void append(const T& x);
	void swap(Array<T>& x);
	iterator insert(iterator position, const T& x);
	void insert(size_type position, const T& x);
	void remove(size_type index);
	void remove(size_type begin, size_type end);
	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last);
	void appendArray(const Array<T>& x);
	void pop_back();
	iterator erase(iterator position);
	iterator erase(iterator first, iterator last);
	void resize(size_type new_size, const T& x);
	void resize(size_type new_size);
	void clear();
	friend bool operator== <>(const Array<T>& x, const Array<T>& y);
	friend bool operator< <>(const Array<T>& x, const Array<T>& y);
private:
#ifdef OW_CHECK_ARRAY_INDEXING
	void checkValidIndex(size_type index) const;
#endif
};

template <class T>
inline bool operator != (const Array<T>& x, const Array<T>& y)
{
	return !(x == y);
}

template <class T>
inline bool operator <= (const Array<T>& x, const Array<T>& y)
{
	return !(y < x);
}

template <class T>
inline bool operator >= (const Array<T>& x, const Array<T>& y)
{
	return !(x < y);
}

template <class T>
inline bool operator > (const Array<T>& x, const Array<T>& y)
{
	return y < x;
}
  
template <class T>
inline std::vector<T>* COWReferenceClone(std::vector<T>* obj);

typedef Array<UInt8>      UInt8Array;
typedef Array<Int8>       Int8Array;
typedef Array<UInt16>     UInt16Array;
typedef Array<Int16>      Int16Array;
typedef Array<UInt32>     UInt32Array;
typedef Array<Int32>      Int32Array;
typedef Array<UInt64>     UInt64Array;
typedef Array<Int64>      Int64Array;
typedef Array<Real64>     Real64Array;
typedef Array<Real32>     Real32Array;

} // end namespace OpenWBEM

#include "OW_ArrayImpl.hpp"

typedef OpenWBEM::UInt8Array OW_UInt8Array;
typedef OpenWBEM::Int8Array OW_Int8Array;
typedef OpenWBEM::UInt16Array OW_UInt16Array;
typedef OpenWBEM::Int16Array OW_Int16Array;
typedef OpenWBEM::UInt32Array OW_UInt32Array;
typedef OpenWBEM::Int32Array OW_Int32Array;
typedef OpenWBEM::UInt64Array OW_UInt64Array;
typedef OpenWBEM::Int64Array OW_Int64Array;
typedef OpenWBEM::Real64Array OW_Real64Array;
typedef OpenWBEM::Real32Array OW_Real32Array;

#endif
	
