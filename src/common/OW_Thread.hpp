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
#include "OW_Condition.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_ThreadDoneCallback.hpp"
#include "OW_ThreadCancelledException.hpp"


//////////////////////////////////////////////////////////////////////////////
DECLARE_EXCEPTION(CancellationDenied);
DECLARE_EXCEPTION(Thread);

//////////////////////////////////////////////////////////////////////////////
class OW_Thread
{
public:

	/**
	 * Create a new OW_Thread object.
	 */
	OW_Thread();

	/**
	 * Destroy this OW_Thread object.  The destructor will call join().
	 * This function won't return until the thread has exited.
	 */
	virtual ~OW_Thread();

	/**
	 * Start this OW_Thread's execution.
	 * @exception OW_ThreadException
	 */
	virtual void start(OW_ThreadDoneCallbackRef cb = OW_ThreadDoneCallbackRef(0));

	/**
	 * Attempt to cooperatively cancel this OW_Threads execution.  
	 * You should still call join() in order to clean up resources allocated
	 * for this thread.  
	 * This function will set a flag that the thread has been 
	 * cancelled, which can be checked by testCancel().  If the thread does
	 * not call testCancel(), it may keep running.
	 * The thread may (probably) still be running after this function returns,
	 * and it will exit as soon as it calls testCancel().
	 *
	 * It is also possible for an individual thread to override the cancellation
	 * request, if it knows that cancellation at this time may crash the system
	 * or cause a deadlock.  If this happens, an OW_CancellationDeniedException
	 * will be thrown.
	 *
	 * @exception OW_CancellationDeniedException may be thrown if the thread
	 * cannot be safely cancelled at this time.
	 */
	void cooperativeCancel();

	/**
	 * Attempt to cooperatively and then definitively cancel this OW_Threads 
	 * execution.  You should still call join() in order to clean up resources 
	 * allocated for this thread.  
	 * This function will set a flag that the thread has been 
	 * cancelled, which can be checked by testCancel().  
	 * definitiveCancel() wil first try to stop the thread in a cooperative
	 * manner to avoid leaks or corruption. If the thread has not
	 * exited after waitForCoopeartiveSecs seconds, this it will be cancelled 
	 * (pthread_cancel will
	 * be called)  Note that this will not clean up any objects on the stack,
	 * (except for on some Linux systems newer than Nov. 2003.  Right now,
	 * the only system I know of that does C++ stack unwinding on thread 
	 * cancellation is Fedora Core 1.)
	 * so it may cause memory leaks or inconsistent state or even memory corruption.
	 * Also note that this still may not stop the thread, since a thread can
	 * make itself non-cancellable, or it may not every call any cancellation
	 * points.
	 * By default all OW_Thread objects are asynchronously cancellable, and
	 * so may be immediately cancelled.
	 * The thread may (unlikely) still be running after this function returns.
	 *
	 * It is also possible for an individual thread to override the cancellation
	 * request, if it knows that cancellation at this time may crash the system
	 * or cause a deadlock.  If this happens, an OW_CancellationDeniedException
	 * will be thrown.
	 *
	 * @param waitForCooperativeSecs The number of seconds to wait for
	 * cooperative cancellation to succeed before attempting to forcibly
	 * cancel the thread.
	 *
	 * @return true if the thread exited cleanly.  false if the thread was
	 * forcibly cancelled.
	 *
	 * @exception OW_CancellationDeniedException may be thrown if the thread
	 * cannot be safely cancelled at this time.
	 */
	bool definitiveCancel(OW_UInt32 waitForCooperativeSecs = 60);

	/**
	 * Definitively cancel this OW_Threads execution. The thread is *NOT*
	 * given a chance to clean up or override the cancellation.
	 * DO NOT call this function without first trying definitiveCancel().
	 *
	 * You should still call join() in order to clean up resources 
	 * allocated for this thread.  
	 *
	 * pthread_cancel will be called.
	 * Note that using this function will not clean up any objects on the 
	 * thread's stack,
	 * (except for on some Linux systems newer than Nov. 2003.  Right now,
	 * the only system I know of that does C++ stack unwinding on thread 
	 * cancellation is Fedora Core 1.)
	 * so it may cause memory leaks or inconsistent state or even memory corruption.
	 * Also note that this still may not stop the thread, since a thread can
	 * make itself non-cancellable, or it may not every call any cancellation
	 * points.
	 * By default all OW_Thread objects are asynchronously cancellable, and
	 * so may be immediately cancelled.
	 * The thread may (unlikely) still be running after this function returns.
	 */
	void cancel();

	/**
	 * Test if this thread has been cancelled.  If so, a 
	 * OW_ThreadCancelledException will be thrown.  DO NOT catch this exception.
	 * OW_ThreadCancelledException is not derived from anything.
	 * Do not write code like this:
	 * try {
	 *  //...
	 * } catch (...) {
	 *  // swallow all exceptions
	 * }
	 *
	 * Instead do this:
	 * try {
	 *  //...
	 * } catch (OW_ThreadCancelledException&) {
	 *  throw;
	 * } catch (std::exception& e) {
	 *  // handle the exception
	 * }
	 * The only place OW_ThreadCancelledException should be caught is in 
	 * OW_Thread::threadRunner(). main() shouldn't need to catch it, as the main
	 * thread of an application should never be cancelled.  The main thread
	 * shouldn't need to ever call testCancel.
	 * Note that this method is staic, and it will check the the current running
	 * thread has been cacelled.  Thus, you can't call it on an object that doesn't
	 * represent the current running thread and expect it to work.
	 *
	 */
	static void testCancel();

private:
	/**
	 * This function is available for subclasses of OW_Thread to override if they
	 * wish to be notified when a cooperative cancel is being invoked on the
	 * instance.  Note that this function will be invoked in a separate thread.
	 * For instance, a thread may use this function to write to a pipe or socket,
	 * if OW_Thread::run() is blocked in select(), it can be unblocked and
	 * instructed to exit.
	 *
	 * It is also possible for an individual thread to override the cancellation
	 * request, if it knows that cancellation at this time may crash the system
	 * or cause a deadlock.  To do this, the thread should throw an 
	 * OW_CancellationDeniedException.  Note that threads are usually only
	 * cancelled in the event of a system shutdown or restart, so a thread
	 * should make a best effort to actually shutdown.
	 *
	 * @throws OW_CancellationDeniedException
	 */
	virtual void doCooperativeCancel();

	/**
	 * See the documentation for doCooperativeCancel().  When definitiveCancel()
	 * is called on a thread, first doCooperativeCancel() will be called, and 
	 * then doDefinitiveCancel() will be called.
	 *
	 * @throws OW_CancellationDeniedException
	 */
	virtual void doDefinitiveCancel();

public:
	/**
	 * @return true if this thread is currently running. Otherwise false.
	 */
	bool isRunning()
	{
		return m_isRunning == true;
	}


	/**
	 * Join with this OW_Thread's execution. This method should be called 
	 * on all joinable threads. The destructor will call it as well.
	 * If this OW_Thread object is executing, this method
	 * will block until this OW_Thread's run method returns.
	 * join() should not be called until after start() has returned.  It may
	 * be called by a different thread.
	 *
	 * @exception OW_ThreadException
	 * @return The return value from the thread's run()
	 */
	OW_Int32 join() /*throw (OW_ThreadException)*/;

	/**
	 * Get this OW_Thread object's id.
	 *
	 * This function cannot be called on a non-joinable self-deleting thread
	 * after it has started.
	 *
	 * @return The id of this OW_Thread if it is currently running. Otherwise
	 * return a NULL thread id.
	 */
	OW_Thread_t getId()
	{
		return m_id;
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

protected:

	/**
	 * The method that will be run when the start method is called.
	 */
	virtual OW_Int32 run() = 0;

	// thread state
	OW_Thread_t m_id;
	bool m_isRunning;
	bool m_isStarting;


	// used to implement cancellation.
	friend void OW_ThreadImpl::testCancel();

	OW_NonRecursiveMutex m_cancelLock;
	OW_Condition m_cancelCond;
	bool m_cancelRequested;
	bool m_cancelled;

private:

	static OW_Int32 threadRunner(void* paramPtr);
	
	// non-copyable
	OW_Thread(const OW_Thread&);
	OW_Thread& operator=(const OW_Thread&);
	
};

typedef OW_Reference<OW_Thread> OW_ThreadRef;

#endif
