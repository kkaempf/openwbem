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

#ifndef OW_MAP_HPP_
#define OW_MAP_HPP_

#include "OW_config.h"

#include "OW_COWReference.hpp"

#ifdef OW_NEW
#undef new
#endif

#include <map>
#include <functional>

#ifdef OW_NEW
#define new OW_NEW
#endif

template<class Key, class T, class Compare = std::less<Key> > class OW_Map
{

	typedef std::map<Key, T, Compare > M;
	OW_COWReference<M> m_impl;

public:
	typedef typename M::key_type key_type;
	typedef typename M::mapped_type mapped_type;
	typedef typename M::value_type value_type;
	typedef typename M::key_compare key_compare;    
	typedef typename M::value_compare value_compare;
	typedef typename M::pointer pointer;
	typedef typename M::const_pointer const_pointer;
	typedef typename M::reference reference;
	typedef typename M::const_reference const_reference;
	typedef typename M::iterator iterator;
	typedef typename M::const_iterator const_iterator;
	typedef typename M::reverse_iterator reverse_iterator;
	typedef typename M::const_reverse_iterator const_reverse_iterator;
	typedef typename M::size_type size_type;
	typedef typename M::difference_type difference_type;

	OW_Map() /*throw (std::exception)*/ : m_impl(new M) {  }
	OW_Map(const OW_Map<Key, T, Compare>& arg) : m_impl(arg.m_impl) 
		{ }

	~OW_Map() /*throw ()*/ {  }

	explicit OW_Map(M* toWrap) /*throw (std::exception)*/ : m_impl(toWrap) 
		{ }

	explicit OW_Map(const Compare& comp) /*throw (std::exception)*/ 
		: m_impl(new M(comp)) {  }

	template <class InputIterator>
	OW_Map(InputIterator first, InputIterator last) /*throw (std::exception)*/ :
		m_impl(new M(first, last))
	{
	}

	template <class InputIterator>
	OW_Map(InputIterator first, InputIterator last, const Compare& comp) /*throw (std::exception)*/ :
		m_impl(new M(first, last, comp))
	{
	}

	M* getImpl() /*throw (std::exception)*/
	{
		return &*m_impl;
	}

	key_compare key_comp() const /*throw (std::exception)*/
	{
		return m_impl->key_comp();
	}

	value_compare value_comp() const /*throw (std::exception)*/
	{
		return m_impl->value_comp();
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

	T& operator[](const key_type& k) /*throw (std::exception)*/
	{
		return m_impl->operator[](k);
	}

	void swap(OW_Map<Key, T, Compare>& x) /*throw (std::exception)*/
	{
		m_impl->swap(*x.m_impl);
	}

	std::pair<iterator, bool> insert(const value_type& x) /*throw (std::exception)*/
	{
		return m_impl->insert(x);
	}

	iterator insert(iterator position, const value_type& x) /*throw (std::exception)*/
	{
		return m_impl->insert(position, x);
	}

	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) /*throw (std::exception)*/
	{
		return m_impl->insert(first, last);
	}

	void erase(iterator position) /*throw (std::exception)*/
	{
		return m_impl->erase(position);
	}

	size_type erase(const key_type& x) /*throw (std::exception)*/
	{
		return m_impl->erase(x);
	}

	void erase(iterator first, iterator last) /*throw (std::exception)*/
	{
		return m_impl->erase(first, last);
	}

	void clear() /*throw (std::exception)*/
	{
		return m_impl->clear();
	}

	iterator find(const key_type& x) /*throw (std::exception)*/
	{
		return m_impl->find(x);
	}

	const_iterator find(const key_type& x) const /*throw (std::exception)*/
	{
		return m_impl->find(x);
	}

	size_type count(const key_type& x) const /*throw (std::exception)*/
	{
		return m_impl->count(x);
	}

	iterator lower_bound(const key_type& x) /*throw (std::exception)*/
	{
		return m_impl->lower_bound(x);
	}

	const_iterator lower_bound(const key_type& x) const /*throw (std::exception)*/
	{
		return m_impl->lower_bound(x);
	}

	iterator upper_bound(const key_type& x) /*throw (std::exception)*/
	{
		return m_impl->upper_bound(x);
	}

	const_iterator upper_bound(const key_type& x) const /*throw (std::exception)*/
	{
		return m_impl->upper_bound(x);
	}

	std::pair<iterator, iterator> equal_range(const key_type& x) /*throw (std::exception)*/
	{
		return m_impl->equal_range(x);
	}

	std::pair<const_iterator, const_iterator>
		equal_range(const key_type& x) const /*throw (std::exception)*/
	{
		return m_impl->equal_range(x);
	}

	friend bool operator== <>(const OW_Map<Key, T, Compare>& x, 
		const OW_Map<Key, T, Compare>& y);

	friend bool operator< <>(const OW_Map<Key, T, Compare>& x, 
		const OW_Map<Key, T, Compare>& y);
};

template<class Key, class T, class Compare>
inline bool operator==(const OW_Map<Key, T, Compare>& x,
	const OW_Map<Key, T, Compare>& y) /*throw (std::exception)*/
{
	return *x.m_impl == *y.m_impl;
}

template<class Key, class T, class Compare>
inline bool operator<(const OW_Map<Key, T, Compare>& x,
	const OW_Map<Key, T, Compare>& y) /*throw (std::exception)*/
{
	return *x.m_impl < *y.m_impl;
}

template <class Key, class T, class Compare>
inline void swap(OW_Map<Key, T, Compare>& x,
	OW_Map<Key, T, Compare>& y) /*throw (std::exception)*/
{
	x.swap(y);
}

#endif	// __OW_MAP_HPP__

