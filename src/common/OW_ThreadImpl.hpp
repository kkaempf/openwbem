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

#ifndef __OW_THREADIMPL_HPP__
#define __OW_THREADIMPL_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_ThreadTypes.hpp"

/*----------------------------------
 * Definitions to support threading.
 -----------------------------------*/
#define OW_THREAD_FLG_JOINABLE 0x000000001

typedef void* (*OW_ThreadFunction)(void*);

/**
 * The OW_ThreadImpl class represents the functionality needed by the
 * OpenWbem Thread class (OW_Thread). The implementation for this class
 * must be provided on all platforms that OpenWbem runs on. It is essentially
 * an abstraction layer over another thread implementation.
 */
class OW_ThreadImpl
{
public:

	/**
	 * Starts a thread running the given function.
	 * @param handle	A platform specific thread handle
	 * @param func		The function that will run within the new thread.
	 * @param funcParm The parameter to func
	 * @param flags	The flags to use when creating the thread.
	 *	Currently flags can contain the following:
	 *		OW_THREAD_JOINABLE - Thread will be created in the joinable state.
	 *			
	 * @return 0 on success. Otherwise -1
	 */
	static int createThread(OW_Thread_t& handle, OW_ThreadFunction func,
		void* funcParm, OW_UInt32 threadFlags);

	/**
	 * Destroy any resources associated with a thread that was created with
	 * the createThread method.
	 * @param handle	A platform specific thread handle
	 */
	static void destroyThread(OW_Thread_t& handle);

	/**
	 * Check two platform dependant thread types for equality.
	 * @param handle1	The 1st thread type for the comparison.
	 * @param handle2	The 2nd thread type for the comparison.
	 * @return true if the thread types are equal. Otherwise false
	 */
	static bool sameThreads(const volatile OW_Thread_t& handle1,
		const volatile OW_Thread_t& handle2)
	{
	#ifdef OW_USE_GNU_PTH
		return handle1 == handle2;
	#else
		return pthread_equal(handle1, handle2);
	#endif
	}


	/**
	 * Exit thread method. This method is called everytime a thread exits.
	 * When the POSIX threads are being used, pthread_exit is called.
	 * This function does not return.
	 * @param handle The thread handle of the calling thread.
	 */
	static void exitThread(OW_Thread_t& handle);

	#ifdef OW_USE_GNU_PTH
	static void initThreads();
	#endif

	/**
	 * @return The thread handle for the current running thread.
	 */
	static OW_Thread_t currentThread()
	{
	#ifdef OW_USE_GNU_PTH
		initThreads();
		return pth_self();
	#else
		return pthread_self();
	#endif
	}


	/**
	 * Set a thread that was previously in the joinable state to a detached
	 * state. This will allow the threads resources to be released upon
	 * termination without being joined. A thread that is in the detached
	 * state can no longer be joined.
	 * @param handle		The thread to set to the detached state.
	 * @return 0 on success. Otherwise -1
	 */
	static int setThreadDetached(OW_Thread_t& handle);

	/**
	 * Join a thread that has been previously set to joinable. It is
	 * Assumed that if the thread has already terminated, this method
	 * will still succeed and return immediately.
	 * @param handle	A handle to the thread to join with.
	 * @return 0 on success. Otherwise -1
	 */
	static int joinThread(OW_Thread_t& handle);

	/**
	 * Voluntarily yield to the processor giving the next thread in the chain
	 * the opportunity to run.
	 */
	static void yield();

	/**
	 * Suspend execution of the current thread until the given number
	 * of milliSeconds have elapsed.
	 * @param milliSeconds	The number of milliseconds to suspend execution for.
	 */
	static void sleep(OW_UInt32 milliSeconds);
};

/**
 * The OW_ThreadEventImpl class represents the functionality needed by the
 * OpenWbem ThreadEvent class (OW_ThreadEvent). The implementation for this
 * class must be provided on all platforms that OpenWbem runs on. It is
 * essentially an abstraction layer over another thread event implementation.
 */
class OW_ThreadEventImpl
{
public:

	/**
	 * Create an event object.
	 * An event object is used primarily for thread syncronization. It can be in
	 * a signaled or non-signaled state. If a ThreadEvent object is in a non-signaled
    * state and a thread calls waitForThreadEventSignal, it will block until another
	 * thread puts the ThreadEvent in a signaled state or a given amount of time has
	 * elapsed. If more than one thread is waiting for the ThreadEvent to transition
	 * to a signaled state (blocking) and the ThreadEvent gets signaled, all threads
	 * will be released.
	 *
	 * @param handle	The handle that will be set to the new event handle.
	 * @return 0 on success. Otherwise -1
	 */
	static int createThreadEvent(OW_ThreadEvent_t& handle);

	/**
	 * Destroy an event object previously created with createThreadEvent.
	 * @param handle The handle to the event that will be destroyed.
	 * @return - 0 on success. Otherwise -1
	 */
	static int destroyThreadEvent(OW_ThreadEvent_t& handle);

	/**
	 * Wait for an event to be in the signaled state. If the event is not
	 * currently in the signaled state, then this method will block until
	 * another thread put it into the signaled state. If the event is not in
	 * the signaled state, this method will return immediately.
	 * @param handle	The event handle to wait for a signal on.
	 * @param ms		The amount of time to wait in milliseconds for the event
	 *						to be signaled.
	 * @return true if the event is now signaled. Otherwise false.
	 */
	static bool waitForThreadEventSignal(OW_ThreadEvent_t& handle, OW_UInt32 ms);

	/**
	 * Put the given event into the signaled state. If any threads are
	 * waiting for this event to get signaled (waitForThreadEventSignal), they will
	 * stop blocking after this method returns.
	 * @param handle	The event that will be put in the signaled state.
	 */
	static void signalThreadEvent(OW_ThreadEvent_t& handle);

	/**
	 * If the event is in the non-signaled state, and threads are waiting for
	 * it to transition to the signaled state, then allow one thread to stop
	 * blocking, but keep the event in the non-signaled state.
	 */
	static void pulseThreadEvent(OW_ThreadEvent_t& handle);

	/**
	 * Put the given event into the non-signaled state. If any threads call
	 * waitForThreadEventSignal after this function returns, they will block until
	 * signalThreadEvent is called.
	 * @param hanel	The event to put in the non-signaled state.
	 */
	static void resetThreadEvent(OW_ThreadEvent_t& handle);

	/**
	 * Determine if an event is in the signaled state.
	 * @param handle	The event that is being check for the signaled state.
	 * @return true if the event is currently signaled. Otherwise false.
	 */
	static bool isThreadEventSignaled(OW_ThreadEvent_t& handle);
};

class OW_SemaphoreImpl
{
public:
	static bool wait(OW_ConditionVar_t& cond, OW_Mutex_t& mutex,
		OW_Int32& curCount, OW_UInt32 ms);
	static void signal(OW_ConditionVar_t& cond, OW_Mutex_t& mutex,
		OW_Int32& curCount);
	static OW_Int32 getCount(OW_Mutex_t& mutex, OW_Int32& count);
	static void createConditionVar(OW_ConditionVar_t& cond);
};

#endif	// __OW_THREADIMPL_HPP__


