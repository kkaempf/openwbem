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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_LIST_HPP_INCLUDE_GUARD_
#define OW_LIST_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_COWReference.hpp"

#ifdef OW_NEW
#undef new
#endif

#include <list>

#ifdef OW_NEW
#define new OW_NEW
#endif


template<class T> class OW_List
{
private:

	typedef std::list<T> L;
	OW_COWReference<L> m_impl;

public:
	typedef typename L::value_type value_type;
	typedef typename L::pointer pointer;
	typedef typename L::const_pointer const_pointer;
	typedef typename L::reference reference;
	typedef typename L::const_reference const_reference;
	typedef typename L::size_type size_type;
	typedef typename L::difference_type difference_type;
	typedef typename L::iterator iterator;
	typedef typename L::const_iterator const_iterator;
	typedef typename L::reverse_iterator reverse_iterator;
	typedef typename L::const_reverse_iterator const_reverse_iterator;

	OW_List() /*throw (std::exception)*/ : m_impl(new L) {}

	explicit OW_List(L* toWrap) /*throw (std::exception)*/ : m_impl(toWrap)
	{  }

	template<class InputIterator>
	OW_List(InputIterator first, InputIterator last) /*throw (std::exception)*/ : m_impl(new L(first, last))
	{
	}

	OW_List(size_type n, const T& value) /*throw (std::exception)*/ : m_impl(new L(n, value))
	{
	}

	OW_List(int n, const T& value) /*throw (std::exception)*/ : m_impl(new L(n, value))
	{
	}

	OW_List(long n, const T& value) /*throw (std::exception)*/ : m_impl(new L(n, value))
	{
	}

	explicit OW_List(size_type n) /*throw (std::exception)*/ : m_impl(new L(n))
	{
	}

	L* getImpl() /*throw (std::exception)*/
	{
		return &*m_impl;
	}

	iterator begin() /*throw (std::exception)*/
	{
		return m_impl->begin();
	}

	const_iterator begin() const /*throw (std::exception)*/
	{
		return m_impl->begin();
	}

	iterator end() /*throw (std::exception)*/
	{
		return m_impl->end();
	}

	const_iterator end() const /*throw (std::exception)*/
	{
		return m_impl->end();
	}

	reverse_iterator rbegin() /*throw (std::exception)*/
	{
		return m_impl->rbegin();
	}

	const_reverse_iterator rbegin() const /*throw (std::exception)*/
	{
		return m_impl->rbegin();
	}

	reverse_iterator rend() /*throw (std::exception)*/
	{
		return m_impl->rend();
	}

	const_reverse_iterator rend() const /*throw (std::exception)*/
	{
		return m_impl->rend();
	}

	bool empty() const /*throw (std::exception)*/
	{
		return m_impl->empty();
	}

	size_type size() const /*throw (std::exception)*/
	{
		return m_impl->size();
	}

	size_type max_size() const /*throw (std::exception)*/
	{
		return m_impl->max_size();
	}

	reference front() /*throw (std::exception)*/
	{
		return m_impl->front();
	}

	const_reference front() const /*throw (std::exception)*/
	{
		return m_impl->front();
	}

	reference back() /*throw (std::exception)*/
	{
		return m_impl->back();
	}

	const_reference back() const /*throw (std::exception)*/
	{
		return m_impl->back();
	}

	void swap(OW_List<T>& x) /*throw (std::exception)*/
	{
		m_impl.swap(x.m_impl);
	}

	iterator insert(iterator position, const T& x) /*throw (std::exception)*/
	{
		return m_impl->insert(position, x);
	}

	iterator insert(iterator position) /*throw (std::exception)*/
	{
		return m_impl->insert(position);
	}

	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last)
	/*throw (std::exception)*/
	{
		m_impl->insert(position, first, last);
	}

	void insert(iterator pos, size_type n, const T& x) /*throw (std::exception)*/
	{
		m_impl->insert(pos, n, x);
	}

	void insert(iterator pos, int n, const T& x) /*throw (std::exception)*/
	{
		m_impl->insert(pos, n, x);
	}

	void insert(iterator pos, long n, const T& x) /*throw (std::exception)*/
	{
		m_impl->insert(pos, n, x);
	}

	void push_front(const T& x) /*throw (std::exception)*/
	{
		m_impl->push_front(x);
	}

	void push_back(const T& x) /*throw (std::exception)*/
	{
		m_impl->push_back(x);
	}

	iterator erase(iterator position) /*throw (std::exception)*/
	{
		return m_impl->erase(position);
	}

	iterator erase(iterator first, iterator last) /*throw (std::exception)*/
	{
		return m_impl->erase(first, last);
	}

	void resize(size_type new_size, const T& x) /*throw (std::exception)*/
	{
		m_impl->resize(new_size, x);
	}

	void resize(size_type new_size) /*throw (std::exception)*/
	{
		m_impl->resize(new_size);
	}

	void clear() /*throw (std::exception)*/
	{
		m_impl->clear();
	}

	void pop_front() /*throw (std::exception)*/
	{
		m_impl->pop_front();
	}

	void pop_back() /*throw (std::exception)*/
	{
		m_impl->pop_back();
	}

	void splice(iterator position, OW_List& x) /*throw (std::exception)*/
	{
		m_impl->splice(position, *x.m_impl);
	}

	void splice(iterator position, OW_List& x, iterator i) /*throw (std::exception)*/
	{
		m_impl->splice(position, *x.m_impl, i);
	}

	void splice(iterator position, OW_List& x, iterator first, iterator last)
	/*throw (std::exception)*/
	{
		m_impl->splice(position, *x.m_impl, first, last);
	}

	void remove(const T& value) /*throw (std::exception)*/
	{
		m_impl->remove(value);
	}

	void unique() /*throw (std::exception)*/
	{
		m_impl->unique();
	}

	void merge(OW_List& x) /*throw (std::exception)*/
	{
		m_impl->merge(*x.m_impl);
	}

	void reverse() /*throw (std::exception)*/
	{
		m_impl->reverse();
	}

	void sort() /*throw (std::exception)*/
	{
		m_impl->sort();
	}

	template<class Predicate> void remove_if(Predicate p) /*throw (std::exception)*/
	{
		m_impl->remove_if(p);
	}

	template<class BinaryPredicate> void unique(BinaryPredicate bp) /*throw (std::exception)*/
	{
		m_impl->unique(bp);
	}

	template<class StrictWeakOrdering> void merge(OW_List& x, StrictWeakOrdering swo)
	/*throw (std::exception)*/
	{
		m_impl->merge(*x.m_impl, swo);
	}

	template<class StrictWeakOrdering> void
		sort(StrictWeakOrdering swo) /*throw (std::exception)*/
	{
		m_impl->sort(swo);
	}

	friend bool operator== <>(const OW_List<T>& x,
		const OW_List<T>& y);

	friend bool operator< <>(const OW_List<T>& x,
		const OW_List<T>& y);
};

template <class T>
inline bool operator==(const OW_List<T>& x, const OW_List<T>& y) /*throw (std::exception)*/
{
	return *x.m_impl == *y.m_impl;
}

template <class T>
inline bool operator<(const OW_List<T>& x, const OW_List<T>& y) /*throw (std::exception)*/
{
	return *x.m_impl < *y.m_impl;
}

template <class T>
inline void swap(OW_List<T>& x, OW_List<T>& y) /*throw (std::exception)*/
{
	x.swap(y);
}

template <class T>
std::list<T>* OW_COWReferenceClone(std::list<T>* obj)
{
    return new std::list<T>(*obj);
}

#endif


