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

#ifndef __OW_MUTEXLOCK_HPP
#define __OW_MUTEXLOCK_HPP

#include "OW_config.h"
#include "OW_Mutex.hpp"
#include <cassert>

//////////////////////////////////////////////////////////////////////////////
class OW_MutexLock
{
public:
	explicit OW_MutexLock(OW_Mutex& mutex, bool initially_locked=true)
		: m_mutex(&mutex), m_locked(false)
	{
		if(initially_locked)
		{
			lock();
		}
	}

	~OW_MutexLock()
	{
		try
		{
			if (m_locked)
			{
				release();
			}
		}
		catch (...)
		{
			// don't let exceptions escape
		}
	}

	void lock()
	{
		assert(m_locked == false);
		m_mutex->acquire();
		m_locked = true;
	}

	void release()
	{
		assert(m_locked == true);
		m_mutex->release();
		m_locked = false;
	}

	OW_MutexLock(const OW_MutexLock& arg)
		: m_mutex(arg.m_mutex), m_locked(arg.m_locked)
	{
		arg.m_locked = false;
	}

	bool isLocked() const
	{
		return m_locked;
	}

	/*
	OW_MutexLock& operator= (const OW_MutexLock& arg)
	{
		release();
		m_locked = arg.m_locked;
		m_mutex = arg.m_mutex;
		arg.m_locked = false;
		return *this;
	}
	*/

private:


	OW_Mutex* m_mutex;
	mutable bool m_locked;

	friend class OW_Condition;
};


#endif	// __OW_MUTEXLOCK_HPP
