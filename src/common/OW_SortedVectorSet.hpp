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

#ifndef OW_SORTED_VECTOR_SET_HPP_
#define OW_SORTED_VECTOR_SET_HPP_

#include "OW_config.h"

#include "OW_Reference.hpp"

#ifdef OW_NEW
#undef new
#endif

#include <vector>
#include <utility> // for std::pair
#include <functional> // for std::less
#include <algorithm>

#ifdef OW_NEW
#define new OW_NEW
#endif

template<class T, class Compare = std::less<T> >
class OW_SortedVectorSet
{

	typedef std::vector<T> container_t;
	OW_Reference<container_t> m_impl;

public:
	typedef          T key_type;
	typedef          T data_type;
	typedef          T value_type;
	typedef          Compare key_compare;
	typedef          Compare value_compare;
	typedef typename container_t::pointer pointer;
	typedef typename container_t::const_pointer const_pointer;
	typedef typename container_t::reference reference;
	typedef typename container_t::const_reference const_reference;
	typedef typename container_t::iterator iterator;
	typedef typename container_t::const_iterator const_iterator;
	typedef typename container_t::reverse_iterator reverse_iterator;
	typedef typename container_t::const_reverse_iterator const_reverse_iterator;
	typedef typename container_t::size_type size_type;
	typedef typename container_t::difference_type difference_type;

	OW_SortedVectorSet() : m_impl(new container_t) {  }
	OW_SortedVectorSet(const OW_SortedVectorSet<T, Compare>& arg) : m_impl(arg.m_impl)
		{ }

	~OW_SortedVectorSet() {  }

	explicit OW_SortedVectorSet(container_t* toWrap) : m_impl(toWrap)
		{ }

	template <class InputIterator>
	OW_SortedVectorSet(InputIterator first, InputIterator last) :
		m_impl(new container_t(first, last))
	{
		std::sort(m_impl->begin(), m_impl->end(), Compare());
	}

	iterator begin() const { return m_impl->begin(); }
	iterator end() const { return m_impl->end(); }
	reverse_iterator rbegin() const { return m_impl->rbegin(); }
	reverse_iterator rend() const { return m_impl->rend(); }
	bool empty() const { return m_impl->empty(); }
	size_type size() const { return m_impl->size(); }
	size_type max_size() const { return m_impl->max_size(); }

	void swap(OW_SortedVectorSet<T, Compare>& x) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();
		m_impl->swap(*x.m_impl);
	}

	std::pair<iterator, bool> insert(const value_type& x) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();

		iterator i = std::lower_bound(m_impl->begin(), m_impl->end(), x, Compare());
		return m_impl->insert(i, x);
	}

	iterator insert(iterator, const value_type& x) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();

		iterator i = std::lower_bound(m_impl->begin(), m_impl->end(), x, Compare());
		
		return m_impl->insert(i, x);
	}

	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();

		for (; first != last; ++first)
		{
			m_impl->push_back(*first);
		}
		std::sort(m_impl->begin(), m_impl->end(), Compare());

	}

	void erase(iterator position) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();
		return m_impl->erase(position);
	}

	size_type erase(const key_type& x) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();
		iterator i = std::lower_bound(m_impl->begin(), m_impl->end(), x, Compare());
		return m_impl->erase(i);
	}

	void erase(iterator first, iterator last) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();
		return m_impl->erase(first, last);
	}

	void clear() /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();
		return m_impl->clear();
	}

	iterator find(const key_type& x) const /*throw (std::exception)*/
	{
		iterator pos = std::lower_bound(m_impl->begin(), m_impl->end(), x, Compare());
		if (*pos == x)
		{
			return pos;
		}
		else
		{
			return m_impl->end();
		}
	}

	size_type count(const key_type& x) const /*throw (std::exception)*/
	{
		if (std::binary_search(m_impl->begin(), m_impl->end(), x, Compare()))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	iterator lower_bound(const key_type& x) const /*throw (std::exception)*/
	{
		return std::lower_bound(m_impl->begin(), m_impl->end(), x, Compare());
	}

	iterator upper_bound(const key_type& x) const /*throw (std::exception)*/
	{
		return std::upper_bound(m_impl->begin(), m_impl->end(), x, Compare());
	}

	std::pair<iterator, iterator>
		equal_range(const key_type& x) const /*throw (std::exception)*/
	{
		return std::equal_range(m_impl->begin(), m_impl->end(), x, Compare());
	}

	friend bool operator== <>(const OW_SortedVectorSet<T, Compare>& x,
		const OW_SortedVectorSet<T, Compare>& y);

	friend bool operator< <>(const OW_SortedVectorSet<T, Compare>& x,
		const OW_SortedVectorSet<T, Compare>& y);
};

template<class T, class Compare>
inline bool operator==(const OW_SortedVectorSet<T, Compare>& x,
	const OW_SortedVectorSet<T, Compare>& y) /*throw (std::exception)*/
{
	return *x.m_impl == *y.m_impl;
}

template<class T, class Compare>
inline bool operator<(const OW_SortedVectorSet<T, Compare>& x,
	const OW_SortedVectorSet<T, Compare>& y) /*throw (std::exception)*/
{
	return *x.m_impl < *y.m_impl;
}

template <class T, class Compare>
inline void swap(OW_SortedVectorSet<T, Compare>& x,
	OW_SortedVectorSet<T, Compare>& y) /*throw (std::exception)*/
{
	x.swap(y);
}

#endif

