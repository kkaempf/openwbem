/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

/**
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_LIST_HPP_INCLUDE_GUARD_
#define OW_LIST_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_COWReference.hpp"
#include <list>

// This class is a wrapper around std::list<> and adds COW capabilities.
namespace OpenWBEM
{

template<class T> class List
{
private:
	typedef std::list<T> L;
	COWReference<L> m_impl;
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
	List() : m_impl(new L) {}
	explicit List(L* toWrap) : m_impl(toWrap)
	{  }
	template<class InputIterator>
	List(InputIterator first, InputIterator last) : m_impl(new L(first, last))
	{
	}
	List(size_type n, const T& value) : m_impl(new L(n, value))
	{
	}
	List(int n, const T& value) : m_impl(new L(n, value))
	{
	}
	List(long n, const T& value) : m_impl(new L(n, value))
	{
	}
	explicit List(size_type n) : m_impl(new L(n))
	{
	}
	L* getImpl()
	{
		return &*m_impl;
	}
	iterator begin()
	{
		return m_impl->begin();
	}
	const_iterator begin() const
	{
		return m_impl->begin();
	}
	iterator end()
	{
		return m_impl->end();
	}
	const_iterator end() const
	{
		return m_impl->end();
	}
	reverse_iterator rbegin()
	{
		return m_impl->rbegin();
	}
	const_reverse_iterator rbegin() const
	{
		return m_impl->rbegin();
	}
	reverse_iterator rend()
	{
		return m_impl->rend();
	}
	const_reverse_iterator rend() const
	{
		return m_impl->rend();
	}
	bool empty() const
	{
		return m_impl->empty();
	}
	size_type size() const
	{
		return m_impl->size();
	}
	size_type max_size() const
	{
		return m_impl->max_size();
	}
	reference front()
	{
		return m_impl->front();
	}
	const_reference front() const
	{
		return m_impl->front();
	}
	reference back()
	{
		return m_impl->back();
	}
	const_reference back() const
	{
		return m_impl->back();
	}
	void swap(List<T>& x)
	{
		m_impl.swap(x.m_impl);
	}
	iterator insert(iterator position, const T& x)
	{
		return m_impl->insert(position, x);
	}
	iterator insert(iterator position)
	{
		return m_impl->insert(position);
	}
	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last)
	{
		m_impl->insert(position, first, last);
	}
	void insert(iterator pos, size_type n, const T& x)
	{
		m_impl->insert(pos, n, x);
	}
	void insert(iterator pos, int n, const T& x)
	{
		m_impl->insert(pos, n, x);
	}
	void insert(iterator pos, long n, const T& x)
	{
		m_impl->insert(pos, n, x);
	}
	void push_front(const T& x)
	{
		m_impl->push_front(x);
	}
	void push_back(const T& x)
	{
		m_impl->push_back(x);
	}
	iterator erase(iterator position)
	{
		return m_impl->erase(position);
	}
	iterator erase(iterator first, iterator last)
	{
		return m_impl->erase(first, last);
	}
	void resize(size_type new_size, const T& x)
	{
		m_impl->resize(new_size, x);
	}
	void resize(size_type new_size)
	{
		m_impl->resize(new_size);
	}
	void clear()
	{
		m_impl->clear();
	}
	void pop_front()
	{
		m_impl->pop_front();
	}
	void pop_back()
	{
		m_impl->pop_back();
	}
	void splice(iterator position, List& x)
	{
		m_impl->splice(position, *x.m_impl);
	}
	void splice(iterator position, List& x, iterator i)
	{
		m_impl->splice(position, *x.m_impl, i);
	}
	void splice(iterator position, List& x, iterator first, iterator last)
	{
		m_impl->splice(position, *x.m_impl, first, last);
	}
	void remove(const T& value)
	{
		m_impl->remove(value);
	}
	void unique()
	{
		m_impl->unique();
	}
	void merge(List& x)
	{
		m_impl->merge(*x.m_impl);
	}
	void reverse()
	{
		m_impl->reverse();
	}
	void sort()
	{
		m_impl->sort();
	}
	template<class Predicate> void remove_if(Predicate p)
	{
		m_impl->remove_if(p);
	}
	template<class BinaryPredicate> void unique(BinaryPredicate bp)
	{
		m_impl->unique(bp);
	}
	template<class StrictWeakOrdering> void merge(List& x, StrictWeakOrdering swo)
	{
		m_impl->merge(*x.m_impl, swo);
	}
	template<class StrictWeakOrdering> void
		sort(StrictWeakOrdering swo)
	{
		m_impl->sort(swo);
	}
	friend bool operator== <>(const List<T>& x,
		const List<T>& y);
	friend bool operator< <>(const List<T>& x,
		const List<T>& y);
};
template <class T>
inline bool operator==(const List<T>& x, const List<T>& y)
{
	return *x.m_impl == *y.m_impl;
}
template <class T>
inline bool operator<(const List<T>& x, const List<T>& y)
{
	return *x.m_impl < *y.m_impl;
}
template <class T>
inline void swap(List<T>& x, List<T>& y)
{
	x.swap(y);
}
template <class T>
std::list<T>* COWReferenceClone(std::list<T>* obj)
{
	return new std::list<T>(*obj);
}

} // end namespace OpenWBEM

#endif
