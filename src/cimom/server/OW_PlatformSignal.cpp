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

#include "OW_Platform.hpp"
#include "OW_PlatformSignal.hpp"
#include "OW_UserUtils.hpp"
#include <ostream>

namespace OW_NAMESPACE
{
	namespace Platform
	{
		namespace Signal
		{
			SignalInformation::SignalInformation() :
				signalAction(0),
				signalNumber(0),
				errorNumber(0),
				signalCode(0),
				originatingPID(0),
				originatingUID(0),
				faultAddress(0),
				fileDescriptor(0),
				band(0)
			{
#if defined(OW_HAVE_SIGVAL_TYPE_SIGVAL_INT)
				timerValue.sigval_int = 0;
#elif defined(OW_HAVE_SIGVAL_TYPE_SIVAL_INT)
				timerValue.sival_int = 0;
#else
				timerValue = 0;
#endif
			}

			void extractSignalInformation(const siginfo_t& source, SignalInformation& dest)
			{
				// Clear out everything.
				dest = SignalInformation();

#ifndef OW_WIN32
				// These three are the only ones guaranteed to always be set in the
				// struct.  Everything else may be set, depending on the values for
				// these three fields.
				dest.signalNumber = source.si_signo;
				dest.errorNumber = source.si_errno;
				dest.signalCode = source.si_code;

				switch(source.si_code)
				{
				case SI_USER:
					dest.originatingPID = source.si_pid;
					dest.originatingUID = source.si_uid;
					break;
				case SI_TIMER:
					dest.timerValue = source.si_value;
					break;
				case SI_ASYNCIO:
#if defined(OW_HAVE_SIGINFO_T_SI_FD)
					dest.fileDescriptor = source.si_fd;
#endif
					dest.band = source.si_band;
					break;
				default:
					// UNKNOWN!
					break;
				}
				switch(source.si_signo)
				{
				case SIGILL:
				case SIGFPE:
				case SIGTRAP:
				case SIGBUS:
				case SIGSEGV:
					dest.faultAddress = source.si_addr;
					break;
#if defined(SIGPOLL)
				case SIGPOLL:
#if defined(OW_HAVE_SIGINFO_T_SI_FD)
					dest.fileDescriptor = source.si_fd;
#endif
					dest.band = source.si_band;
					break;
#endif /* defined(SIGPOLL) */
				default:
					// UNKNOWN.
					break;
				}
#endif
			}

			namespace
			{
				// A helper function to flatten types to a pipe without performing
				// any type conversion (raw data).
				// THIS IS ONLY VALID FOR POD TYPES.
				template<class T>
				void flattenToPipe(const T& t, UnnamedPipeRef& destPipe)
				{
					destPipe->write(&t, sizeof(t));
				}

				// A helper function to unflatten types from a pipe without
				// performing any type conversion (raw data).
				// THIS IS ONLY VALID FOR POD TYPES.
				template <class T>
				bool unflattenFromPipe(T& dest, UnnamedPipeRef& sourcePipe)
				{
					return sourcePipe->read(&dest, sizeof(dest));
				}
			}

			bool flattenSignalInformation(const SignalInformation& source, UnnamedPipeRef& destPipe)
			{
				if( !destPipe )
				{
					return false;
				}

				flattenToPipe(source.signalAction, destPipe);
				flattenToPipe(source.signalNumber, destPipe);
				flattenToPipe(source.errorNumber, destPipe);
				flattenToPipe(source.signalCode, destPipe);
				flattenToPipe(source.originatingPID, destPipe);
				flattenToPipe(source.originatingUID, destPipe);
				flattenToPipe(source.timerValue, destPipe);
				flattenToPipe(source.faultAddress, destPipe);
				flattenToPipe(source.fileDescriptor, destPipe);
				flattenToPipe(source.band, destPipe);
				return true;
			}

			bool unflattenSignalInformation(SignalInformation& dest, UnnamedPipeRef& sourcePipe)
			{
				if( !sourcePipe )
				{
					return false;
				}
				bool success = true;
				dest = SignalInformation();

				success = success && unflattenFromPipe(dest.signalAction, sourcePipe);
				success = success && unflattenFromPipe(dest.signalNumber, sourcePipe);
				success = success && unflattenFromPipe(dest.errorNumber, sourcePipe);
				success = success && unflattenFromPipe(dest.signalCode, sourcePipe);
				success = success && unflattenFromPipe(dest.originatingPID, sourcePipe);
				success = success && unflattenFromPipe(dest.originatingUID, sourcePipe);
				success = success && unflattenFromPipe(dest.timerValue, sourcePipe);
				success = success && unflattenFromPipe(dest.faultAddress, sourcePipe);
				success = success && unflattenFromPipe(dest.fileDescriptor, sourcePipe);
				success = success && unflattenFromPipe(dest.band, sourcePipe);

				return success;
			}

			std::ostream& operator<<(std::ostream& o, const SignalInformation& sig)
			{
				const char* sigtext = NULL;
#if defined(OW_HAVE_STRSIGNAL)
				sigtext = strsignal(sig.signalNumber);
#endif

				if( !sigtext )
				{
					sigtext = "UNKNOWN";
				}
				o << "  Signal: " << sigtext << " (" << sig.signalNumber << ")" << std::endl;
				if( sig.errorNumber )
				{
					o << "  Error: " << strerror(sig.errorNumber) << std::endl;
				}

				const char* signal_type = "Unknown-origin signal";

				enum signal_specifications
					{
						SIGNAL_ANY,
						SIGNAL_SPECIFIC
					};
				// A struct to avoid having a huge case statement (using a loop instead)
				struct signal_code_text
				{
					signal_specifications signal_spec;
					int signal_number;
					int signal_code;
					const char* signal_type_text;
				};

#ifndef OW_WIN32
				// TODO: The signal code conversion into something that is
				// human readable.  This is partially done below.
				// Check the signal code table at
				// http://www.opengroup.org/onlinepubs/009695399/basedefs/signal.h.html
				const signal_code_text signal_text[] =
					{
#if defined(SI_USER)
						{ SIGNAL_ANY, 0, SI_USER, "User-originated (kill, sigsend, or raise) signal" },
#endif
#if defined(SI_KERNEL)
						{ SIGNAL_ANY, 0, SI_KERNEL, "Kernel-originated signal" },
#endif
#if defined(SI_QUEUE)
						{ SIGNAL_ANY, 0, SI_QUEUE, "sigqueue queued signal" },
#endif
#if defined(SI_TIMER)
						{ SIGNAL_ANY, 0, SI_TIMER, "Timer expiration signal"},
#endif
#if defined(SI_ASYNCIO)
						{ SIGNAL_ANY, 0, SI_ASYNCIO, "Asynchronous I/O signal"},
#endif
#if defined(SI_MESGQ)
						{ SIGNAL_ANY, 0, SI_MESGQ, "Empty message queue signal"},
#endif
#if defined(SIGILL) && defined(ILL_ILLOPC)
						{ SIGNAL_SPECIFIC, SIGILL, ILL_ILLOPC, "Illegal instruction: Illegal opcode"},
#endif
#if defined(SIGILL) && defined(ILL_ILLOPN)
						{ SIGNAL_SPECIFIC, SIGILL, ILL_ILLOPN, "Illegal instruction: Illegal operand"},
#endif
#if defined(SIGILL) && defined(ILL_ILLADR)
						{ SIGNAL_SPECIFIC, SIGILL, ILL_ILLADR, "Illegal instruction: Illegal addressing mode"},
#endif
#if defined(SIGILL) && defined(ILL_ILLTRP)
						{ SIGNAL_SPECIFIC, SIGILL, ILL_ILLTRP, "Illegal instruction: Illegal trap"},
#endif
#if defined(SIGILL) && defined(ILL_PRVOPC)
						{ SIGNAL_SPECIFIC, SIGILL, ILL_PRVOPC, "Illegal instruction: Privileged opcode"},
#endif
#if defined(SIGILL) && defined(ILL_PRVREG)
						{ SIGNAL_SPECIFIC, SIGILL, ILL_PRVREG, "Illegal instruction: Privileged register"},
#endif
#if defined(SIGILL) && defined(ILL_COPROC)
						{ SIGNAL_SPECIFIC, SIGILL, ILL_COPROC, "Illegal instruction: Coprocessor error"},
#endif
#if defined(SIGILL) && defined(ILL_BADSTK)
						{ SIGNAL_SPECIFIC, SIGILL, ILL_BADSTK, "Illegal instruction: Internal stack error"},
#endif
					};

				for( const signal_code_text* test_signal = signal_text;
					  test_signal != (signal_text + sizeof(signal_text) / sizeof(*signal_text));
					  ++test_signal )
				{
					if( test_signal->signal_spec == SIGNAL_ANY )
					{
						if( sig.signalCode == test_signal->signal_code )
						{
							signal_type = test_signal->signal_type_text;
							break;
						}
					}
					else
					{
						if(
							(sig.signalNumber == test_signal->signal_number) &&
							(sig.signalCode == test_signal->signal_code) )
						{
							signal_type = test_signal->signal_type_text;
							break;
						}
					}
				}
#else
				signal_type = "UNKNOWN";
#endif

				o << "  Signal Type: " << signal_type << " (" << sig.signalCode << ")" << std::endl;

				// Some of these fields will be meaningless for some types
				// (add specific types in their own case).
				o << "  Fault address: " << sig.faultAddress << std::endl;
				o << "  File Descriptor: " << sig.fileDescriptor << std::endl;
				o << "  Band: " << sig.band << std::endl;
				o << "  Origination PID: " << sig.originatingPID << std::endl;
				o << "  Originating UID: " << sig.originatingUID;
				bool success = false;
				String username = UserUtils::getUserName(sig.originatingUID, success);
				if( success )
				{
					o << " (" << username << ")";
				}
				o << std::endl;

				const char* action;
				switch( sig.signalAction )
				{
				case ::OW_NAMESPACE::Platform::SHUTDOWN:
					action = "shutdown";
					break;
				case ::OW_NAMESPACE::Platform::REINIT:
					action = "restart";
					break;
				default:
					action = "unknown";
					break;
				}
				o << "  CIMOM Action: " << action << std::endl;
				return o;
			}

		} // end namespace Signal
	} // end namespace Platform
} // end namespace OW_NAMESPACE
