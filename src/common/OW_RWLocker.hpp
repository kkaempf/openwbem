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

#ifndef __OW_RWLOCKER_HPP__
#define __OW_RWLOCKER_HPP__

#include "OW_config.h"

#include "OW_MutexLock.hpp"
#include "OW_ThreadEvent.hpp"
#include "OW_Reference.hpp"

#ifdef OW_NEW
#undef new
#endif

#include <vector>

#ifdef OW_NEW
#define new OW_NEW
#endif

class OW_RWLocker;

//////////////////////////////////////////////////////////////////////////////
class OW_ReadLock
{
	class RLData
	{
	public:
		RLData(OW_RWLocker* pl) : m_locker(pl), m_released(false) 
		{
		}
		RLData(const RLData& x) :
			m_locker(x.m_locker), m_released(x.m_released) 
		{
		}
		~RLData();
		RLData& operator= (const RLData& x);
		OW_RWLocker* m_locker;
		OW_Bool m_released;
	};

public:
	OW_ReadLock() : m_pdata(NULL) 
	{
	}
	OW_ReadLock(const OW_ReadLock& arg) :
		m_pdata(arg.m_pdata) 
	{
	}

	OW_ReadLock& operator= (const OW_ReadLock& arg)
	{
		m_pdata = arg.m_pdata;
		return *this;
	}

	void release();
	operator void*() 
	{ 
		return (void*)(m_pdata.isNull() == false); 
	}

private:
	OW_ReadLock(OW_RWLocker* locker) :
		m_pdata(new RLData(locker)) 
	{
	}
	
	OW_Reference<RLData> m_pdata;

	friend class OW_RWLocker;
};

//////////////////////////////////////////////////////////////////////////////
class OW_WriteLock
{
	class WLData
	{
	public:
		WLData(OW_RWLocker* pl) : m_locker(pl), m_released(false) 
		{
		}
		WLData(const WLData& x) :
			m_locker(x.m_locker), m_released(x.m_released) 
		{
		}
		~WLData();
		WLData& operator= (const WLData& x);

		OW_RWLocker* m_locker;
		OW_Bool m_released;
	};

public:
	OW_WriteLock() : m_pdata(NULL) 
	{
	}
	OW_WriteLock(const OW_WriteLock& arg) :
		m_pdata(arg.m_pdata) 
	{
	}

	OW_WriteLock& operator= (const OW_WriteLock& arg)
	{
		m_pdata = arg.m_pdata;
		return *this;
	}

	void release();

	operator void*() 
	{ 
		return (void*)(m_pdata.isNull() == false); 
	}

private:
	OW_WriteLock(OW_RWLocker* locker) :
		m_pdata(new WLData(locker)) 
	{
	}
	
	OW_Reference<WLData> m_pdata;

	friend class OW_RWLocker;
};

//////////////////////////////////////////////////////////////////////////////
class OW_RWLocker
{
public:
	OW_RWLocker();
	~OW_RWLocker();
	OW_ReadLock getReadLock();
	OW_WriteLock getWriteLock();

private:

	void releaseReadLock();
	void releaseWriteLock();

	struct OW_RWQEntry
	{
		OW_RWQEntry(OW_Bool isWriter) :
			m_event(), m_isWriter(isWriter) 
		{
		}
		OW_RWQEntry(const OW_RWQEntry& arg) :
			m_event(arg.m_event), m_isWriter(arg.m_isWriter)
		{
		}

		OW_RWQEntry& operator=(const OW_RWQEntry& arg)
		{
			m_event = arg.m_event;
			m_isWriter = arg.m_isWriter;
			return *this;
		}

		OW_ThreadEvent m_event;
		OW_Bool m_isWriter;
	};

	typedef std::vector<OW_RWQEntry> OW_RWQEntryVect;

	OW_Mutex	m_guard;
	OW_RWQEntryVect m_queue;
	int m_state;

	static int m_readLockCount;
	static int m_writeLockCount;
	static OW_Mutex m_countGuard;

	static void incReadLockCount();
	static void decReadLockCount();
	static void incWriteLockCount();
	static void decWriteLockCount();

	friend class OW_ReadLock;
	friend class OW_ReadLock::RLData;
	friend class OW_WriteLock;
	friend class OW_WriteLock::WLData;
};

#endif
