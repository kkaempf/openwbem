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

#ifndef OW_ARRAY_HPP_INCLUDE_GUARD_
#define OW_ARRAY_HPP_INCLUDE_GUARD_


#include "OW_config.h"

#include "OW_COWReference.hpp"
#include "OW_Types.h"
#include "OW_Exception.hpp"
DECLARE_EXCEPTION(OutOfBounds);

#ifdef OW_NEW
#undef new
#endif

#include <vector>

#ifdef OW_NEW
#define new OW_NEW
#endif


/**
 * The OW_Array class essentially takes the vector class of the stl and
 * adds ref counting and copy on write capability.
 */
template<class T> class OW_Array
{
	typedef std::vector<T, std::allocator<T> > V;
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

	OW_Array();
	~OW_Array();
	explicit OW_Array(V* toWrap);
	OW_Array(size_type n, const T& value);
	OW_Array(int n, const T& value);
	OW_Array(long n, const T& value);
	explicit OW_Array(size_type n);

	template<class InputIterator>
	OW_Array(InputIterator first, InputIterator last);

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
	OW_Array<T>& operator+= (const T& x);
	void reserve(size_type n);
	reference front();
	const_reference front() const;
	reference back();
	const_reference back() const;
	void push_back(const T& x);
	void append(const T& x);
	void swap(OW_Array<T>& x);
	iterator insert(iterator position, const T& x);
	void insert(size_type position, const T& x);
	void remove(size_type index);
	void remove(size_type begin, size_type end);
	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last);
	void appendArray(const OW_Array<T>& x);
	void pop_back();
	iterator erase(iterator position);
	iterator erase(iterator first, iterator last);
	void resize(size_type new_size, const T& x);
	void resize(size_type new_size);
	void clear();
	friend bool operator== <>(const OW_Array<T>& x, const OW_Array<T>& y);
	friend bool operator< <>(const OW_Array<T>& x, const OW_Array<T>& y);

private:
#ifdef OW_CHECK_ARRAY_INDEXING
	void checkValidIndex(size_type index) const;
#endif
};


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

#include "OW_ArrayImpl.hpp"

#endif
	

