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

#ifndef OW_AUTOPTR_HPP_INCLUDE_GUARD_
#define OW_AUTOPTR_HPP_INCLUDE_GUARD_

#include "OW_config.h"

template <class X> class OW_AutoPtr
{
private:
	X* _ptr;

public:
	typedef X element_type;

	/**
	 * Construct a new OW_AutoPtr.
	 * @param isArray true if memory is allocated with new[], false if
	 *        memory is allocated with new.
	 * @param p pointer to the object
	 */
	explicit OW_AutoPtr(X* p = 0) : _ptr(p) {}

	OW_AutoPtr(OW_AutoPtr& a) : _ptr(a.release()) {}

	OW_AutoPtr& operator= (X* p)
	{
		if(p != _ptr)
		{
			reset();
			_ptr = p;
		}
		return *this;
	}

	OW_AutoPtr& operator= (OW_AutoPtr& a)
	{
		if(&a != this)
		{
			delete _ptr;
			_ptr = a.release();
		}
		return *this;
	}

	~OW_AutoPtr()
	{
		typedef char type_must_be_complete[sizeof(X)];
		delete _ptr;
	}

	X& operator*() const {  return *_ptr;}
	X* operator->() const {  return _ptr;}
	X* get() const {  return _ptr;}
	X* release()
	{
		X* rval = _ptr;
		_ptr = 0;
		return rval;
	}
	void reset(X* p=0)
	{
		delete _ptr;
		_ptr = p;
	}
};


template <class X> class OW_AutoPtrVec
{
private:
	X* _ptr;

public:
	typedef X element_type;

	/**
	 * Construct a new OW_AutoPtrVec.
	 * @param isArray true if memory is allocated with new[], false if
	 *        memory is allocated with new.
	 * @param p pointer to the object
	 */
	explicit OW_AutoPtrVec(X* p = 0) : _ptr(p) {}

	OW_AutoPtrVec(OW_AutoPtrVec& a) : _ptr(a.release()) {}

	OW_AutoPtrVec& operator= (X* p)
	{
		if(p != _ptr)
		{
			reset();
			_ptr = p;
		}
		return *this;
	}

	OW_AutoPtrVec& operator= (OW_AutoPtrVec& a)
	{
		if(&a != this)
		{
			delete [] _ptr;
			_ptr = a.release();
		}
		return *this;
	}

	~OW_AutoPtrVec()
	{
		typedef char type_must_be_complete[sizeof(X)];
		delete [] _ptr;
	}

	X& operator*() const {  return *_ptr;}
	X* operator->() const {  return _ptr;}
	X* get() const {  return _ptr;}
	X* release()
	{
		X* rval = _ptr;
		_ptr = 0;
		return rval;
	}
	void reset(X* p=0)
	{
		delete [] _ptr;
		_ptr = p;
	}
};


#endif






