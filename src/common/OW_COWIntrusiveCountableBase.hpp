/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

//
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

#ifndef OW_COW_INTRUSIVE_COUNTABLE_BASE_HPP_INCLUDE_GUARD_
#define OW_COW_INTRUSIVE_COUNTABLE_BASE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_RefCount.hpp"

namespace OW_NAMESPACE
{

/**
 * If you want your class to be managed by COWIntrusiveReference, then derive
 * from this class.  Note that if multiple inheritance is used, you must derive
 * "virtual"ly.
 * 
 * Derived classes must implement: Derived* clone()
 */

class COWIntrusiveCountableBase; 
void COWIntrusiveReferenceAddRef(COWIntrusiveCountableBase * p); 
void COWIntrusiveReferenceRelease(COWIntrusiveCountableBase * p); 
bool COWIntrusiveReferenceUnique(COWIntrusiveCountableBase* p); 
template <typename T> T* COWIntrusiveReferenceClone(T* p); 

class OW_COMMON_API COWIntrusiveCountableBase
{
private:
	RefCount m_usecount;

protected:
	COWIntrusiveCountableBase(COWIntrusiveCountableBase const &)
		: m_usecount(0)
	{}

	COWIntrusiveCountableBase & operator=(COWIntrusiveCountableBase const &x)
	{
		// don't assign or change the ref count, since it won't be different.
		return *this;
	}

	COWIntrusiveCountableBase(): m_usecount(0)
	{
	}

	virtual ~COWIntrusiveCountableBase();

	RefCount getRefCount() const
	{
		return m_usecount;
	}

public:
	inline friend void COWIntrusiveReferenceAddRef(COWIntrusiveCountableBase * p)
	{
		p->m_usecount.inc();
	}

	inline friend void COWIntrusiveReferenceRelease(COWIntrusiveCountableBase * p)
	{
		if (p->m_usecount.decAndTest())
			delete p;
	}
	
	inline friend bool COWIntrusiveReferenceUnique(COWIntrusiveCountableBase* p)
	{
		return p->m_usecount.get() == 1;
	}

	template <typename T>
	friend T* COWIntrusiveReferenceClone(T* p); 
};

template <typename T>
inline T* COWIntrusiveReferenceClone(T* p)
{
	// this needs to happen first to avoid a race condition between 
	// another thread deleting the object and this one making a copy.
	T* tmp = p->clone();
	if (p->m_usecount.decAndTest())
	{
		// only copy--don't need to clone, also not a race condition.
		// undo the decAndTest.
		p->m_usecount.inc();
		delete tmp; // we won't need this anymore.
		return p;
	}
	else
	{
		// need to become unique
		if (tmp) COWIntrusiveReferenceAddRef(tmp);

		return tmp;
	}
}

} // end namespace OW_NAMESPACE

#endif


