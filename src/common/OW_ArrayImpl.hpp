/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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

#ifndef OW_ARRAY_IMPL_HPP_INCLUDE_GUARD_
#define OW_ARRAY_IMPL_HPP_INCLUDE_GUARD_


#include "OW_config.h"

#include "OW_Array.hpp"

#ifdef OW_NEW
#undef new
#endif

#include <vector>

#ifdef OW_NEW
#define new OW_NEW
#endif

#include "OW_Types.h"

class OW_BinIfcIO;

#ifdef OW_DEBUG
#include <cassert>
#endif

/////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Array<T>::OW_Array() 
: m_impl(new V) 
{
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Array<T>::~OW_Array() 
{
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Array<T>::OW_Array(V* toWrap) 
: m_impl(toWrap) 
{
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Array<T>::OW_Array(size_type n, const T& value) 
: m_impl(new V(n, value)) 
{
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Array<T>::OW_Array(int n, const T& value) 
: m_impl(new V(n, value)) 
{
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Array<T>::OW_Array(long n, const T& value) 
: m_impl(new V(n, value)) 
{
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Array<T>::OW_Array(size_type n) 
: m_impl(new V(n)) 
{
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
template<class InputIterator>
OW_Array<T>::OW_Array(InputIterator first, InputIterator last) 
: m_impl(new V(first, last)) 
{
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::iterator
OW_Array<T>::begin()
{ 
	return m_impl->begin(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::const_iterator
OW_Array<T>::begin() const 
{ 
	return m_impl->begin(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::iterator
OW_Array<T>::end()
{ 
	return m_impl->end(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::const_iterator
OW_Array<T>::end() const 
{ 
	return m_impl->end(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::reverse_iterator
OW_Array<T>::rbegin()
{ 
	return m_impl->rbegin(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::const_reverse_iterator
OW_Array<T>::rbegin() const 
{ 
	return m_impl->rbegin(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::reverse_iterator
OW_Array<T>::rend()
{ 
	return m_impl->rend(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::const_reverse_iterator
OW_Array<T>::rend() const 
{ 
	return m_impl->rend(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::size_type
OW_Array<T>::size() const 
{ 
	return m_impl->size(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::size_type
OW_Array<T>::max_size() const 
{ 
	return m_impl->max_size(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::size_type
OW_Array<T>::capacity() const 
{ 
	return m_impl->capacity(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
bool
OW_Array<T>::empty() const 
{ 
	return m_impl->empty(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::reference
OW_Array<T>::operator[](size_type n)
{
#ifdef OW_CHECK_ARRAY_INDEXING
	checkValidIndex(n);
#endif
	return m_impl->operator[](n);
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::const_reference
OW_Array<T>::operator[](size_type n) const
{
#ifdef OW_CHECK_ARRAY_INDEXING
	checkValidIndex(n);
#endif
	return m_impl->operator[](n);
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Array<T>&
OW_Array<T>::operator+= (const T& x)
{
	m_impl->push_back(x);
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::reserve(size_type n) 
{ 
	m_impl->reserve(n); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::reference
OW_Array<T>::front() 
{ 
	return m_impl->front(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::const_reference
OW_Array<T>::front() const 
{ 
	return m_impl->front(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::reference
OW_Array<T>::back() 
{ 
	return m_impl->back(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::const_reference
OW_Array<T>::back() const 
{ 
	return m_impl->back(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::push_back(const T& x) 
{ 
	m_impl->push_back(x); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::append(const T& x) 
{ 
	push_back(x); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::swap(OW_Array<T>& x) 
{ 
	m_impl.swap(x.m_impl); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::iterator
OW_Array<T>::insert(iterator position, const T& x)
{ 
	return m_impl->insert(position, x); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::insert(size_type position, const T& x)
{ 
	m_impl->insert(m_impl->begin() + position, x); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::remove(size_type index)
{
#ifdef OW_CHECK_ARRAY_INDEXING
	checkValidIndex(index);
#endif
	m_impl->erase(m_impl->begin() + index);
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::remove(size_type begin, size_type end)
{
#ifdef OW_CHECK_ARRAY_INDEXING
	checkValidIndex(begin);
	checkValidIndex(end - 1);
#endif
	m_impl->erase(m_impl->begin() + begin, m_impl->begin() + end);
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
template<class InputIterator>
void
OW_Array<T>::insert(iterator position, InputIterator first, InputIterator last)
{
	m_impl->insert(position, first, last);
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::appendArray(const OW_Array<T>& x)
{
	insert(end(), x.begin(), x.end());
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::pop_back() 
{ 
	m_impl->pop_back(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::iterator
OW_Array<T>::erase(iterator position) 
{ 
	return m_impl->erase(position); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
typename OW_Array<T>::iterator
OW_Array<T>::erase(iterator first, iterator last) 
{ 
	return m_impl->erase(first, last); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::resize(size_type new_size, const T& x) 
{ 
	m_impl->resize(new_size, x); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::resize(size_type new_size) 
{ 
	m_impl->resize(new_size); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::clear() 
{ 
	m_impl->clear(); 
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::readObject(std::istream& istr)
{
	m_impl->clear();
	OW_UInt32 len;
	OW_BinIfcIO::readLen(istr, len);
	
	m_impl->reserve(len);
	for(OW_UInt32 i = 0; i < len; i++)
	{
		T x;
		x.readObject(istr);
		m_impl->push_back(x);
	}

}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::writeObject(std::ostream& ostrm) const
{
	OW_UInt32 len = m_impl->size();
	OW_BinIfcIO::writeLen(ostrm, len);
	for(OW_UInt32 i = 0; i < len; i++)
	{
		m_impl->operator[](i).writeObject(ostrm);
	}
}

#ifdef OW_CHECK_ARRAY_INDEXING
/////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Array<T>::checkValidIndex(size_type index) const
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

#endif
	

