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

#ifndef OW_SORTED_VECTOR_HPP_
#define OW_SORTED_VECTOR_HPP_

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

template <class Key, class T, class Compare>
class OW_DataCompare
{
	typedef std::pair<Key, T> Data;
public:
	bool operator()(const Data& lhs, const Data& rhs) const
	{
		return keyLess(lhs.first, rhs.first);
	}
	
	bool operator()(const Data& lhs, const typename Data::first_type& k) const
	{
		return keyLess(lhs.first, k);
	}
	
	bool operator()(const typename Data::first_type& k, const Data& rhs) const
	{
		return keyLess(k, rhs.first);
	}

	bool operator()(const typename Data::first_type& k, const typename Data::first_type& rhs) const
	{
		return keyLess(k, rhs);
	}

private:
	bool keyLess(const typename Data::first_type& k1,
		const typename Data::first_type& k2) const
	{
		return Compare()(k1, k2);
	}
};

template<class Key, class T, class Compare = OW_DataCompare<Key, T, std::less<Key> > >
class OW_SortedVector
{

	typedef std::pair<Key, T> Data;

	typedef std::vector<Data> container_t;
	OW_Reference<container_t> m_impl;

public:
	typedef          Key key_type;
	typedef          T data_type;
	typedef          std::pair<const key_type, data_type> value_type;
	typedef          Compare key_compare;
	typedef          Compare value_compare;
	typedef typename container_t::pointer pointer;
	typedef typename container_t::reference reference;
	typedef typename container_t::const_reference const_reference;
	typedef typename container_t::iterator iterator;
	typedef typename container_t::const_iterator const_iterator;
	typedef typename container_t::reverse_iterator reverse_iterator;
	typedef typename container_t::const_reverse_iterator const_reverse_iterator;
	typedef typename container_t::size_type size_type;
	typedef typename container_t::difference_type difference_type;

	OW_SortedVector() : m_impl(new container_t) {  }
	OW_SortedVector(const OW_SortedVector<Key, T, Compare>& arg) : m_impl(arg.m_impl)
		{ }

	~OW_SortedVector() {  }

	explicit OW_SortedVector(container_t* toWrap) : m_impl(toWrap)
		{ }

	template <class InputIterator>
	OW_SortedVector(InputIterator first, InputIterator last) :
		m_impl(new container_t(first, last))
	{
		std::sort(m_impl->begin(), m_impl->end(), Compare());
	}

	const_iterator begin() const
	{
		return m_impl->begin();
	}

	const_iterator end() const
	{
		return m_impl->end();
	}

	iterator begin() 
	{
		return m_impl->begin();
	}

	iterator end() 
	{
		return m_impl->end();
	}

	const_reverse_iterator rbegin() const
	{
		return m_impl->rbegin();
	}

	const_reverse_iterator rend() const
	{
		return m_impl->rend();
	}

	OW_Bool empty() const
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

	data_type& operator[](const key_type& k) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();

		iterator i = std::lower_bound(m_impl->begin(), m_impl->end(), k, Compare());
		if (i != m_impl->end() && i->first == k)
		{
			return i->second;
		}
		return (*(m_impl->insert(i, value_type(k, data_type())))).second;
	}

	void swap(OW_SortedVector<Key, T, Compare>& x) /*throw (std::exception)*/
	{
		OW_MutexLock lock = m_impl.getWriteLock();
		m_impl->swap(*x.m_impl);
	}

	std::pair<iterator, OW_Bool> insert(const value_type& x) /*throw (std::exception)*/
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

	const_iterator find(const key_type& x) const /*throw (std::exception)*/
	{
		return std::lower_bound(m_impl->begin(), m_impl->end(), x, Compare());
	}

	iterator find(const key_type& x) /*throw (std::exception)*/
	{
		return std::lower_bound(m_impl->begin(), m_impl->end(), x, Compare());
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

	const_iterator lower_bound(const key_type& x) const /*throw (std::exception)*/
	{
		return std::lower_bound(m_impl->begin(), m_impl->end(), x, Compare());
	}

	const_iterator upper_bound(const key_type& x) const /*throw (std::exception)*/
	{
		return std::upper_bound(m_impl->begin(), m_impl->end(), x, Compare());
	}

	std::pair<const_iterator, const_iterator>
		equal_range(const key_type& x) const /*throw (std::exception)*/
	{
		return std::equal_range(m_impl->begin(), m_impl->end(), x, Compare());
	}

	friend OW_Bool operator== <>(const OW_SortedVector<Key, T, Compare>& x,
		const OW_SortedVector<Key, T, Compare>& y);

	friend OW_Bool operator< <>(const OW_SortedVector<Key, T, Compare>& x,
		const OW_SortedVector<Key, T, Compare>& y);
};

template<class Key, class T, class Compare>
inline OW_Bool operator==(const OW_SortedVector<Key, T, Compare>& x,
	const OW_SortedVector<Key, T, Compare>& y) /*throw (std::exception)*/
{
	return *x.m_impl == *y.m_impl;
}

template<class Key, class T, class Compare>
inline OW_Bool operator<(const OW_SortedVector<Key, T, Compare>& x,
	const OW_SortedVector<Key, T, Compare>& y) /*throw (std::exception)*/
{
	return *x.m_impl < *y.m_impl;
}

template <class Key, class T, class Compare>
inline void swap(OW_SortedVector<Key, T, Compare>& x,
	OW_SortedVector<Key, T, Compare>& y) /*throw (std::exception)*/
{
	x.swap(y);
}

#endif

