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

#ifndef OW_AUTOPTR_HPP_INCLUDE_GUARD_
#define OW_AUTOPTR_HPP_INCLUDE_GUARD_
#include "OW_config.h"

namespace OpenWBEM
{

// TODO: Rename this
template <class X> class AutoPtr
{
private:
	X* _ptr;
	
	// no copying
	AutoPtr(const AutoPtr& a);
	AutoPtr& operator= (const AutoPtr& a);

public:
	typedef X element_type;
	/**
	 * Construct a new AutoPtr.
	 * @param p pointer to the object
	 */
	explicit AutoPtr(X* p = 0) : _ptr(p) {}
	AutoPtr& operator= (X* p)
	{
		if (p != _ptr)
		{
			reset();
			_ptr = p;
		}
		return *this;
	}
	~AutoPtr()
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
template <class X> class AutoPtrVec
{
private:
	X* _ptr;
	
	// no copying
	AutoPtrVec(const AutoPtrVec& a);
	AutoPtrVec& operator= (const AutoPtrVec& a);

public:
	typedef X element_type;
	/**
	 * Construct a new AutoPtrVec.
	 * @param p pointer to the object
	 */
	explicit AutoPtrVec(X* p = 0) : _ptr(p) {}
	AutoPtrVec& operator= (X* p)
	{
		if (p != _ptr)
		{
			reset();
			_ptr = p;
		}
		return *this;
	}
	~AutoPtrVec()
	{
		typedef char type_must_be_complete[sizeof(X)];
		delete [] _ptr;
	}
	X& operator*() const {  return *_ptr;}
	X* operator->() const {  return _ptr;}
	X& operator[](unsigned i) { return _ptr[i]; }
	const X& operator[](unsigned i) const { return _ptr[i]; }
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

} // end namespace OpenWBEM

#endif
