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

#ifndef OW_THREADEVENT_HPP_
#define OW_THREADEVENT_HPP_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Reference.hpp"
#include "OW_ThreadImpl.hpp"

/**
 * The ThreadEvent class is used primarily for thread syncronization. It can be in a 
 * signaled or non-signaled state. If an ThreadEvent object is in a non-signaled
 * state and a thread calls waitForSignal, it will block until another thread
 * puts the ThreadEvent in a signaled state or a given amount of time has 
 * elapsed. If more than one thread is waiting for the ThreadEvent to transition
 * to a signaled state (blocking) and the ThreadEvent gets signaled, all threads
 * will be released.
 */
class OW_ThreadEvent
{
private:

	class ThreadEvent
	{
	public:
		ThreadEvent();
		~ThreadEvent();
		OW_Bool waitForSignal(OW_UInt32 ms=0);
		void signal();
		void pulse();
		void reset();
		OW_Bool isSignaled();
	
	protected:
	
		OW_ThreadEvent_t m_hdl;

	private:
		ThreadEvent(const ThreadEvent& arg);	// shouldn't happen
		ThreadEvent& operator =(const ThreadEvent& arg); // shouldn't happen
	};

public:

	/**
	 * Create a new ThreadEvent object
	 * 
	 * @exception OW_ThreadEventCreationException
	 *		Thrown if there was a failure in creating the underlying mutex and
	 *    condition variable.
	 */
	OW_ThreadEvent() : m_impl(new ThreadEvent) 
	{
	}

	/**
	 * Copy constructor. Upon return this OW_ThreadEvent object will share the same
	 * implementation object as the given OW_ThreadEvent object.
	 * @param arg	The OW_ThreadEvent to use the implementation from.
	 */
	OW_ThreadEvent(const OW_ThreadEvent& arg) : m_impl(arg.m_impl) 
	{
	}

	/**
	 * Assignment operator. Basically this sets this objects implementation to
	 * the implementation of the given OW_ThreadEvent object. They will reference the
	 * same implementation upon return.
	 * @param arg	The OW_ThreadEvent object to share implementations with.
	 * @return A reference to this OW_ThreadEvent object.
	 */
	OW_ThreadEvent& operator= (const OW_ThreadEvent& arg) 
	{ 
		m_impl = arg.m_impl; return *this; 
	}

	/**
	 * Wait for this ThreadEvent object to transition to a signaled state. If the ThreadEvent
	 * is already in a signaled state, this method will return immediately. 
	 * Otherwise this method will block until the ThreadEvent gets signaled or the 
	 * given amount of time has elapsed.
	 * 
	 * @param ms	The maximum amount of time to wait for this ThreadEvent to get 
	 *					signaled. If this value is zero, the method will not return 
	 *					until the event get signaled.
	 * @return true if the ThreadEvent is now in the signaled state. Otherwise false.
	 */
	OW_Bool waitForSignal(OW_UInt32 ms=0) 
	{ 
		return m_impl->waitForSignal(ms); 
	}

	/**
	 * Set the state of this ThreadEvent object to signaled
	 */
	void signal() 
	{ 
		m_impl->signal(); 
	}

	/**
	 * Allow one thread that is waiting for signal to stop blocking.
	 */
	void pulse() 
	{ 
		m_impl->pulse(); 
	}

	/**
	 * Set the state of this ThreadEvent object to non-signaled.
	 */
	void reset() 
	{ 
		m_impl->reset(); 
	}

	/**
	 * Determine the state of this ThreadEvent object.
	 * 
	 * @return true if this ThreadEvent object is currently signaled. Otherwise false.
	 */
	OW_Bool isSignaled() 
	{ 
		return m_impl->isSignaled(); 
	}

protected:

	/** The implementation for this ThreadEvent object */
	OW_Reference<ThreadEvent> m_impl;
};

#endif	// OW_THREADEVENT_HPP

