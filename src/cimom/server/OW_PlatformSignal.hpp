/*******************************************************************************
 * Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
 * @author Kevin Harris
 */

#ifndef OW_PLATFORM_SIGNAL_HPP_INCLUDE_GUARD_
#define OW_PLATFORM_SIGNAL_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_UnnamedPipe.hpp"

#include <iosfwd>

#include <signal.h>

namespace OW_NAMESPACE
{
	namespace Platform
	{
		namespace Signal
		{

			// Some platforms define a sigval_t struct, others define sigval.
			// Autoconf magic was done to define these macros.
#if defined(OW_SIGVAL_TYPE)
			typedef OW_SIGVAL_TYPE sigval_type;
#else
			// This won't be used, except as a placeholder, but a typedef here is
			// much better than a bunch of #ifdefs later.
			typedef int sigval_type;
#endif

			/**
			 * This struct is based on fields contained within the siginfo_t
			 * structure.  It exists to have some structure where all of the
			 * neccessary data fields are available at the same time.  This is
			 * needed for proper flattening/unflattening while retaining
			 * independence from the oddities of the specific platforms (most
			 * platforms use horrible unions inside the siginfo_t structure).
			 */
			struct SignalInformation
			{
				SignalInformation();    // This only zeroes out everything.

				int signalAction;       // Defined for use in the CIMOM server (SHUTDOWN or REINIT)

				int signalNumber;       // si_signo
				int errorNumber;        // si_errno -- useful if non-zero
				int signalCode;         // si_code

				pid_t originatingPID;   // si_pid -- useful iff signalCode is SI_USER
				uid_t originatingUID;   // si_uid -- useful iff signalCode is SI_USER
				sigval_type timerValue; // si_sigval -- useful iff signalCode is SI_TIMER
				void* faultAddress;     // si_addr -- useful if signalCode is >0 and signalNumber equal to one of { SIGILL, SIGFPE, SIGTRAP, SIGBUS, SIGSEGV }
				int fileDescriptor;     // si_fd -- useful if signalCode is SI_ASYNCIO or >0 and equal to SIGPOLL
				int band;               // si_band -- useful if signalCode is SI_ASYNCIO or >0 and equal to SIGPOLL
			};

			/**
			 * Given a siginfo_t structure, complete a SignalInformation structure.
			 */
			void extractSignalInformation(const siginfo_t& source, SignalInformation& dest);

			/**
			 * These functions serve only to flatten and unflatten information for
			 * the same target machine.  No size conversion, endian conversions,
			 * or any other data conversions are done.  Everything is transported
			 * through the pipe as raw binary data.
			 *
			 * @returns true iff the structures could be flattened or unflattened.
			 *
			 */
			bool flattenSignalInformation(const SignalInformation& source, UnnamedPipeRef& destPipe);
			bool unflattenSignalInformation(SignalInformation& dest, UnnamedPipeRef& sourcePipe);

			/**
			 * A function to dump a signal to a stream.  Mostly useful for
			 * debugging.
			 *
			 * NOT FOR USE WITHIN A SIGNAL HANDLER!
			 *
			 * Attempting to use this within a sig handler will potentially cause
			 * an abort, signal deadlock, or some other unfriendly action to
			 * occur.
			 */
			std::ostream& operator<<(std::ostream& o, const SignalInformation& sig);
		}
	}
}

#endif /* !defined(OW_PLATFORM_SIGNAL_HPP_INCLUDE_GUARD_) */

