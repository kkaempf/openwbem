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

#ifndef OW_THREADIMPL_HPP_INCLUDE_GUARD_
#define OW_THREADIMPL_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_ThreadTypes.hpp"

/*----------------------------------
 * Definitions to support threading.
 -----------------------------------*/
#define OW_THREAD_FLG_JOINABLE 0x000000001

typedef OW_Int32 (*OW_ThreadFunction)(void*);

/**
 * The OW_ThreadImpl class represents the functionality needed by the
 * OpenWbem Thread class (OW_Thread). It also contains other misellaneous 
 * functions which are useful for synchronization and threads.
 * The implementation for this class
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
	#if defined(OW_USE_GNU_PTH) || defined(OW_WIN32)
		return handle1 == handle2;
	#elif OW_HAVE_PTHREAD_H
		return pthread_equal(handle1, handle2);
	#endif
	}


	/**
	 * Exit thread method. This method is called everytime a thread exits.
	 * When the POSIX threads are being used, pthread_exit is called.
	 * This function does not return.
	 * @param handle The thread handle of the calling thread.
	 * @param rval The thread's return value. This can get picked up by joinThread.
	 */
	static void exitThread(OW_Thread_t& handle, OW_Int32 rval);

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
	#elif defined(OW_WIN32)
		return GetCurrentThreadId();
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
	 * @param rval An out parameter of the thread's return code.
	 * @return 0 on success. Otherwise -1
	 */
	static int joinThread(OW_Thread_t& handle, OW_Int32& rval);

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

	/**
	 * "Multi-processor cache coherency.  Certain multi-processor platforms,
	 * such as the COMPAQ Alpha and Intel Itanium, perform aggressive memory
	 * caching optimization in which read and write operation can execute
	 * 'out of order' across multiple CPU caches.  On these platforms, it may
	 * not be possible to use the Double-Checked Locking Optimization pattern
	 * without further modification because CPU cache lines will not be flushed
	 * properly if shared data is accessed without locks held." 
	 * (Pattern-Oriented Software Architecture Vol. 2, Schmidt, Stal, Rohnert, 
	 * Buschmann. p. 361
	 *
	 * This function executes a memory barrier if necessary for the platform,
	 * else it is a noop.
	 */
	static void memoryBarrier()
	{
		// DEC/COMPAQ/HP Alpha
		#if defined(__alpha)
		__asm__ __volatile__("mb");
		#endif
		
		// Intel Itanium
		#if defined(__ia64__) || defined(__ia64)
		__asm__ __volatile__("mf");
		#endif

	}
};

#endif



