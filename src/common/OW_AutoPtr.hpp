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

#ifndef __OW_AUTOPTR_HPP__
#define __OW_AUTOPTR_HPP__

#include "OW_config.h"

template <class X> class OW_AutoPtr
{
private:
	X* ptr;

public:
	typedef X element_type;
	explicit OW_AutoPtr(X* p = 0) : ptr(p) {}

	OW_AutoPtr(OW_AutoPtr& a) : ptr(a.release()) {}

	OW_AutoPtr& operator= (X* p)
	{
		if(p != ptr)
		{
			reset();
			ptr = p;
		}
		return *this;
	}

	OW_AutoPtr& operator= (OW_AutoPtr& a)
	{
		if(&a != this)
		{
			delete ptr;
			ptr = a.release();
		}
		return *this;
	}

	~OW_AutoPtr()
	{
		delete ptr;
	}

	X& operator*() const {  return *ptr;}
	X* operator->() const {  return ptr;}
	X* get() const {  return ptr;}
	X* release()
	{
		X* rval = ptr;
		ptr = 0;
		return rval;
	}
	void reset(X* p=0)
	{
		delete ptr;
		ptr = p;
	}
};


#endif	// __OW_AUTOPTR_HPP__





