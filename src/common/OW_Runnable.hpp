/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_RUNNABLE_HPP_INCLUDE_GUARD_
#define OW_RUNNABLE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_ThreadDoneCallback.hpp"

namespace OpenWBEM
{

/**
 * There are two methods for creating a thread of execution in the OW systems.
 * One is to derive from Thread and implement the run method and call start
 * on instances of the class to get the thread running.
 * The other method is to derive from Runnable and pass references of the
 * derived class to ThreadPool::addWork()
 */
class OW_COMMON_API Runnable : public IntrusiveCountableBase
{
public:
	virtual ~Runnable();
	virtual void run() = 0;

	/**
	 * This function is available for subclasses to override if they
	 * wish to be notified when a cooperative cancel is being invoked on the
	 * thread.  Note that this function will be invoked in a separate thread.
	 * For instance, a thread may use this function to write to a pipe or socket,
	 * if Thread::run() is blocked in select(), it can be unblocked and
	 * instructed to exit.
	 *
	 * It is also possible for an individual thread to override the cancellation
	 * request, if it knows that cancellation at this time may crash the system
	 * or cause a deadlock.  To do this, the thread should throw an 
	 * CancellationDeniedException.  Note that threads are usually only
	 * cancelled in the event of a system shutdown or restart, so a thread
	 * should make a best effort to actually shutdown.
	 *
	 * @throws CancellationDeniedException
	 */
	virtual void doCooperativeCancel();
	/**
	 * See the documentation for doCooperativeCancel().  When definitiveCancel()
	 * is called on a thread, first doCooperativeCancel() will be called, and 
	 * then doDefinitiveCancel() will be called.
	 *
	 * @throws CancellationDeniedException
	 */
	virtual void doDefinitiveCancel();
};

typedef IntrusiveReference<Runnable> RunnableRef;

} // end namespace OpenWBEM

#endif
