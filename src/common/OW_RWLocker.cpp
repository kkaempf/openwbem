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
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_RWLocker.hpp"
#include "OW_Assertion.hpp"

DEFINE_EXCEPTION(RWLocker);

//////////////////////////////////////////////////////////////////////////////
OW_RWLocker::OW_RWLocker()
	: m_waiting_writers()
	, m_waiting_readers()
	, m_num_waiting_writers(0)
	, m_num_waiting_readers(0)
	, m_readers_next(0)
    , m_guard()
	, m_state(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_RWLocker::~OW_RWLocker()
{
	// ???
	//try
	//{
	//	m_cond.notifyAll();
	//}
	//catch (...)
	//{
		// don't let exceptions escape
	//}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::getReadLock()
{
    OW_NonRecursiveMutexLock l(m_guard);
    
    // Wait until no exclusive lock is held.
    //
    // Note:  Scheduling priorities are enforced in the unlock()
    //   call.  unlock will wake the proper thread.
    while(m_state < 0)
    {
        ++m_num_waiting_readers;
        m_waiting_readers.wait(l);
        --m_num_waiting_readers;
    }
    
    // Increase the reader count
    m_state++;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::getWriteLock()
{
    OW_NonRecursiveMutexLock l(m_guard);

    // Wait until no exclusive lock is held.
    //
    // Note:  Scheduling priorities are enforced in the unlock()
    //   call.  unlock will wake the proper thread.
    while(m_state != 0)
    {
        ++m_num_waiting_writers;
        m_waiting_writers.wait(l);
        --m_num_waiting_writers;
    }
    m_state = -1;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::releaseReadLock()
{
    OW_NonRecursiveMutexLock l(m_guard);
    if(m_state > 0)        // Release a reader.
        --m_state;
    else
		OW_THROW(OW_RWLockerException, "A writer is releasing a read lock");

    if(m_state == 0)
    {
        doWakeups();
    }

}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::releaseWriteLock()
{
    OW_NonRecursiveMutexLock l(m_guard);
    if(m_state == -1)
        m_state = 0;
    else
        OW_THROW(OW_RWLockerException, "A reader is releasing a write lock");

    // After a writer is unlocked, we are always back in the unlocked state.
    //
    doWakeups();

}

//////////////////////////////////////////////////////////////////////////////
void
OW_RWLocker::doWakeups()
{
    if( m_num_waiting_writers > 0 && 
        m_num_waiting_readers > 0)
    {
		if(m_readers_next == 1)
		{
			m_readers_next = 0;
			m_waiting_readers.notifyAll();
		}
		else
		{
			m_waiting_writers.notifyOne();
			m_readers_next = 1;
		}
    }
    else if(m_num_waiting_writers > 0)
    {
        // Only writers - scheduling doesn't matter
        m_waiting_writers.notifyOne();
    }
    else if(m_num_waiting_readers > 0)
    {
        // Only readers - scheduling doesn't matter
        m_waiting_readers.notifyAll();
    }

}




