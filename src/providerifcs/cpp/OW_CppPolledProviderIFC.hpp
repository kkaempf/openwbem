/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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

#ifndef OW_CPPPOLLEDPROVIDERIFC_HPP_
#define OW_CPPPOLLEDPROVIDERIFC_HPP_
#include "OW_config.h"
#include "OW_CppProviderBaseIFC.hpp"
#include "blocxx/SharedLibraryReference.hpp"

namespace OW_NAMESPACE
{

class OW_CPPPROVIFC_API CppPolledProviderIFC : public virtual CppProviderBaseIFC
{
public:
	virtual ~CppPolledProviderIFC();
	/**
	 * Called by the CIMOM to give this CppPolledProviderIFC to
	 * opportunity to export indications if needed.
	 * @param lch	A local CIMOM handle the provider can use to export
	 *					indications if needed.
	 * @return How many seconds before the next call to the poll method. If this
	 * method returns -1 then the last polling interval will be used. If it
	 * returns 0 then the poll method will never be called again.
	 */
	virtual Int32 poll(const ProviderEnvironmentIFCRef& env) = 0;
	/**
	 * @return The amount of seconds before the first call to the poll method.
	 * If this method returns zero, then the poll method is never called.
	 */
	virtual Int32 getInitialPollingInterval(const ProviderEnvironmentIFCRef& env) = 0;
	/**
	 * This function is available for subclasses to override if they
	 * wish to be notified when a cooperative cancel is being invoked on the
	 * thread.  Note that this function will be invoked in a separate thread.
	 * For instance, a thread may use this function to:
	 * 1. Set a flag and then signal a condition variable to wake up the thread.
	 * 2. Write to a pipe or socket, if Thread::run() is blocked in select(),
	 * it can be unblocked and then exit.
	 */
	virtual void doShutdown();

	/**
	 * This function is available for subclasses to override if they
	 * wish to be notified when a cooperative cancel is being invoked on the
	 * thread.  Note that this function will be invoked in a separate thread.
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

	virtual CppPolledProviderIFC* getPolledProvider();
};
typedef SharedLibraryReference< IntrusiveReference<CppPolledProviderIFC> > CppPolledProviderIFCRef;

} // end namespace OW_NAMESPACE

#endif
