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
 * @author Dan Nuffer
 */

#ifndef OW_THREAD_CANCELLED_EXCEPTION_HPP_INCLUDE_GUARD_
#define OW_THREAD_CANCELLED_EXCEPTION_HPP_INCLUDE_GUARD_
#include "OW_config.h"

namespace OW_NAMESPACE
{

/**
 * In the event a thread has been cancelled, a 
 * ThreadCancelledException will be thrown.  DO NOT catch this exception.
 * ThreadCancelledException is not derived from anything.
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
 * } catch (ThreadCancelledException&) {
 *  throw;
 * } catch (std::exception& e) {
 *  // handle the exception
 * }
 * The only place ThreadCancelledException should be caught is in 
 * Thread::threadRunner(). main() shouldn't need to catch it, as the main
 * thread of an application should never be cancelled.  The main thread
 * shouldn't need to ever call testCancel.
 */
struct ThreadCancelledException {};

} // end namespace OW_NAMESPACE

#endif
