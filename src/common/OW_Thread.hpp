/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
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

#ifndef OW_THREAD_HPP_INCLUDE_GUARD_
#define OW_THREAD_HPP_INCLUDE_GUARD_

#include "OW_config.h"

#include "OW_Exception.hpp"
#include "OW_String.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_Reference.hpp"
#include "OW_Assertion.hpp"
#include "OW_Runnable.hpp"
#include "OW_ThreadDoneCallback.hpp"


DECLARE_EXCEPTION(Thread);




//////////////////////////////////////////////////////////////////////////////
class OW_Thread
{
public:

	/**
	 * Create a new OW_Thread object.
	 * @param isjoinable	If true this thread will be a joinable thread.
	 * This allows other threads to call join on this thread, causing them
	 * to block until this OW_Thread exits.
	 * If a thread is created as joinable, then join must be called on this
	 * thread to release the resources associated with this thread.
	 */
	OW_Thread(OW_Bool isjoinable=false);

	/**
	 * Destroy this OW_Thread object.
	 */
	virtual ~OW_Thread();

	/**
	 * Start this OW_Thread's execution.
	 * @exception OW_ThreadException
	 */
	virtual void start(OW_ThreadDoneCallbackRef cb = OW_ThreadDoneCallbackRef(0));

	/**
	 * Cancel this OW_Threads execution.
	 */
	void cancel();

	/**
	 * Set the self delete flag on this OW_Thread object. If the self delete
	 * flag is set on this OW_Thread, it is assumed that this OW_Thread was
	 * dynamically allocated and the thread will be deleted when it completes
	 * its execution.
	 * @param selfDelete	If true this thread will self destruct on exit.
	 */
	void setSelfDelete(OW_Bool selfDelete=true)
	{
		OW_ASSERT(!m_isJoinable); // a joinable thread can't be set to self-delete.
		m_deleteSelf = selfDelete;
	}

	/**
	 * Get the value of the self delete flag.
	 * @return true if this thread will self destruct. Otherwise false.
	 */
	OW_Bool getSelfDelete()
	{
		return m_deleteSelf;
	}

	/**
	 * @return true if this thread is currently running. Otherwise false.
	 */
	OW_Bool isRunning()
	{
		return OW_Bool(m_isRunning == true);
	}


	/**
	 * Join with this OW_Thread's execution. The thread must be a joinable
	 * thread for this method to be called. This method must be called at on
	 * all joinable threads. If this OW_Thread object is executing, this method
	 * will block until this OW_Thread's run method returns.
	 * join() should not be called until after start() has returned.  It may
	 * be called by a different thread.
	 * @exception OW_ThreadException
	 * @return The return value from the thread's run()
	 */
	OW_Int32 join() /*throw (OW_ThreadException)*/;

	/**
	 * Get this OW_Thread object's id.
	 * @return The id of this OW_Thread if it is currently running. Otherwise
	 * return a NULL thread id.
	 */
	OW_Thread_t getId()
	{
		return m_id;
	}

	/**
	 * @return true if this OW_Thread object is a joinable thread. Otherwise
	 * return false.
	 */
	OW_Bool isJoinable()
	{
		return m_isJoinable;
	}


	/**
	 * Suspend execution of the current thread until the given number
	 * of milliSeconds have elapsed.
	 * @param milliSeconds	The number of milliseconds to suspend execution for.
	 */
	static void sleep(OW_UInt32 milliSeconds)
	{
		OW_ThreadImpl::sleep(milliSeconds);
	}

	/**
	 * Voluntarily yield to the processor giving the next thread in the chain
	 * the opportunity to run.
	 */
	static void yield()
	{
		OW_ThreadImpl::yield();
	}

	/**
	 * Start a thread and run the run method of a given OW_Runnable object.
	 * @param theRunnable	A reference to an OW_Runnable object as an
	 *								OW_RunnableRef to run.
	 */
	static void run(OW_RunnableRef theRunnable, OW_Bool separateThread = true,
		OW_ThreadDoneCallbackRef cb = OW_ThreadDoneCallbackRef(0));

protected:

	/**
	 * The method that will be run when the start method is called on this
	 * OW_Thread object.
	 */
	virtual OW_Int32 run() = 0;

	OW_Thread_t m_id;
	OW_Bool m_isJoinable;
	OW_Bool m_deleteSelf;
	OW_Bool m_isRunning;
	OW_Bool m_isStarting;

private:

	static OW_Int32 threadRunner(void* paramPtr);
	
	// non-copyable
	OW_Thread(const OW_Thread&);
	OW_Thread& operator=(const OW_Thread&);
	
};

typedef OW_Reference<OW_Thread> OW_ThreadRef;

#endif
