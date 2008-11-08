/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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
 * @author Kevin Harris
 */

/**
 * This program is a used to spawn a child, wait for some amount of time, and
 * handle signals sent to the main process either by ignoring them (so the
 * process must be forcefully terminated) or by logging and responding to them
 * (forwarding to the child if needed).
 *
 * It is used for testing the signalling conditions where differences in the
 * shell would make signal handling impossible to reliably handle in a
 * standard, cross-platform way.
 *
 * This program can be used as an example for using the CmdLineParser.  It can
 * also be used as an example of what should *NOT* be done when writing good
 * (safe) signal handlers -- just because it works here on many platforms
 * doesn't mean it is safe in general.
 */

#include "blocxx/BLOCXX_config.h"
#include "blocxx/CmdLineParser.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/Cstr.hpp"
#include "blocxx/DateTime.hpp"

#include <iostream>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace blocxx;
using namespace std;

namespace // anonymous
{

	const char* PROGRAM_NAME = "ExecTestProgram";
	int OUTPUT_DESCRIPTOR = 1;

	enum
		{
			E_HELP_OPT
			, E_CHILD_PROGRAM_OPT
			, E_BLOCK_SIGNALS_OPT
			, E_FORWARD_SIGNALS_OPT
			, E_SLEEP_TIME_OPT
			, E_SPAWN_CHILD_OPT
			, E_PROGRAM_NAME_OPT
			, E_OUTPUT_DESCRIPTOR_OPT
		};

	static const CmdLineParser::Option g_options[] =
		{
			{ E_HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help" }
			, { E_CHILD_PROGRAM_OPT, '\0', "child-program", CmdLineParser::E_REQUIRED_ARG, 0, "Specify the child program to execute" }
			, { E_BLOCK_SIGNALS_OPT, '\0', "block-signals", CmdLineParser::E_REQUIRED_ARG, 0, "Specify a comma-separated list of signals to block (eg. term, hup)" }
			, { E_FORWARD_SIGNALS_OPT, '\0', "forward-signals", CmdLineParser::E_REQUIRED_ARG, 0, "Specify a comma-separated list of signals to forward to child processes (eg. term, hup)" }
			, { E_SLEEP_TIME_OPT, '\0', "sleep-time", CmdLineParser::E_REQUIRED_ARG, 0, "Specify the amount of time the parent should sleep." }
			, { E_SPAWN_CHILD_OPT, '\0', "spawn-child", CmdLineParser::E_REQUIRED_ARG, 0, "Specify arguments to execute for a child process." }
			, { E_PROGRAM_NAME_OPT, '\0', "program-name", CmdLineParser::E_REQUIRED_ARG, 0, "Specify the name to report this process in all output." }
			, { E_OUTPUT_DESCRIPTOR_OPT, '\0', "output-descriptor", CmdLineParser::E_REQUIRED_ARG, 0, "Send all output to the specified descriptor." }
			, { 0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0 }
		};

	void doOutput(const String& output)
	{
		DateTime stamp = DateTime::getCurrent();
		char buffer[100];
		int count = snprintf(buffer, sizeof(buffer), "\n%ld.%06d ", static_cast<long>(stamp.get()), stamp.getMicrosecond());
		String junk = Format("%1%2", String(buffer, count), output);
		::write(OUTPUT_DESCRIPTOR, junk.c_str(), junk.length());
	}

} // end anonymous namespace


namespace // anonymous
{
	struct SignalNameMapping
	{
		int signum;
		const char* const signame;
	};

	// A mapping structure that is used both ways to allow blocking or
	// forwarding signals and also for output purposes.
	const SignalNameMapping signalMappings[] =
		{
			{ 0, "NONE" }
			, { SIGHUP, "HUP" }
			, { SIGINT, "INT" }
			, { SIGABRT, "ABRT" }
			, { SIGBUS, "BUS" }
			, { SIGTERM, "TERM" }
			, { SIGCHLD, "CHLD" }
			, { SIGKILL, "KILL" }
			, { SIGSTOP, "STOP" }
			, { SIGCONT, "CONT" }
			, { SIGTSTP, "TSTP" }
			, { SIGUSR1, "USR1" }
			, { SIGUSR2, "USR2" }
			, { SIGALRM, "ALRM" }
		};

	// Get a number of a signal name
	int getSignalForName(const String& name)
	{
		for( const SignalNameMapping* mapping = signalMappings;
			  mapping != (signalMappings + sizeof(signalMappings) / sizeof(*signalMappings));
			  ++mapping )
		{
			if( name.equalsIgnoreCase(mapping->signame) )
			{
				return mapping->signum;
			}
		}
		return -1;
	}

	// Get a signal name for a signum
	const char* getNameForSignal(int signum)
	{
		for( const SignalNameMapping* mapping = signalMappings;
			  mapping != (signalMappings + sizeof(signalMappings) / sizeof(*signalMappings));
			  ++mapping )
		{
			if( mapping->signum == signum )
			{
				return mapping->signame;
			}
		}
		return "UNKNOWN";
	}

	void blockAllSignals()
	{
		char buffer[1024];
		snprintf(buffer, 1024, "\n%s blocking all signals...", PROGRAM_NAME);
		::write(OUTPUT_DESCRIPTOR, buffer, ::strlen(buffer));
		
		sigset_t mask;
		sigfillset(&mask);

		sigprocmask(SIG_BLOCK, &mask, NULL);
	}

	void unblockAllSignals()
	{
		char buffer[1024];
		snprintf(buffer, 1024, "\n%s unblocking all signals...", PROGRAM_NAME);
		::write(OUTPUT_DESCRIPTOR, buffer, ::strlen(buffer));
		
		sigset_t mask;
		sigfillset(&mask);

		sigprocmask(SIG_UNBLOCK, &mask, NULL);
	}

	bool isempty(const sigset_t& set)
	{
		for(int sig = 0; sig < NSIG; ++sig)
		{
			if( sigismember(&set, sig) == 1 )
			{
				return false;
			}
		}
		return true;
	}

	void deliverPendingSignals()
	{

		sigset_t mask;

		blockAllSignals(); // prevent undefined behavior.

		sigpending(&mask);
		while( !isempty(mask) )
		{
			sigsuspend(&mask); // always returns -1 on some platforms, so it can't be tested.
			sigpending(&mask);
		}
	}

	typedef void (*SignalHandler)(int,siginfo_t*,void*);
	typedef void (*SimpleSignalHandler)(int);

	// Forward decls for handlers supplied here.
	void ignoreSignalHandler(int sig, siginfo_t* info, void* context);
	void terminalSignalHandler(int sig, siginfo_t* info, void* context);
	void signalForwardHandler(int sig, siginfo_t* info, void* context);
	void loggedSignalHandler(int sig, siginfo_t* info, void* context);
	void ignoreHandler(int sig, siginfo_t* info, void* context);

	// The nodefer option allows a signal handler to raise the signal it was
	// called with.  If you have nodefer=false, the handler MUST NOT raise any
	// signals.
	void installHandler(int sig, SimpleSignalHandler handler, bool nodefer = false)
	{
		struct sigaction temp;
		sigaction(sig, 0, &temp);
		temp.sa_handler = handler;
		sigemptyset(&temp.sa_mask);
		if( nodefer )
		{
			temp.sa_flags = SA_NODEFER;
		}
		sigaction(sig, &temp, NULL);
	}

	// Pretty much taken from handleSignalAux() in OW_Platform.cpp, but
	// allows the SA_NODEFER flag to be set so queued signals can be
	// avoided when needed.
	void installHandler(int sig, SignalHandler handler, bool nodefer = false)
	{
		String handlerName;
		if( handler == &ignoreSignalHandler )
		{
			handlerName = "ignoreSignalHandler";
		}
		else if( handler == &terminalSignalHandler )
		{
			handlerName = "terminalSignalHandler";
		}
		else if( handler == &signalForwardHandler )
		{
			handlerName = "signalForwardHandler";
		}
		else if( handler == &ignoreHandler )
		{
			handlerName = "ignoreHandler";
		}
		else if( handler == &loggedSignalHandler )
		{
			handlerName = "loggedSignalHandler";
		}
		
		doOutput(Format("%1 installing signal handler %2 for signal %3 (%4)\n",
						PROGRAM_NAME, handlerName, getNameForSignal(sig), sig));
		
		struct sigaction temp;

		sigaction(sig, 0, &temp);
		temp.sa_sigaction = handler;
		sigemptyset(&temp.sa_mask);

		/* Here's a note from the glibc documentation:
		 * When you don't specify with `sigaction' or `siginterrupt' what a
		 * particular handler should do, it uses a default choice.  The default
		 * choice in the GNU library depends on the feature test macros you have
		 * defined.  If you define `_BSD_SOURCE' or `_GNU_SOURCE' before calling
		 * `signal', the default is to resume primitives; otherwise, the default
		 * is to make them fail with `EINTR'.  (The library contains alternate
		 * versions of the `signal' function, and the feature test macros
		 * determine which one you really call.)
		 *
		 * We want the EINTR behavior, so we can cancel threads and shutdown
		 * or restart if the occasion arises, so we set flags to 0.
		 *
		 * We also want to use the sa_sigaction field, so we set SA_SIGINFO flag.
		 */
		temp.sa_flags = SA_SIGINFO;
		if( nodefer )
		{
			temp.sa_flags |= SA_NODEFER;
		}
		sigaction(sig, &temp, NULL);
	}


	void ignoreHandler(int sig, siginfo_t* info, void* context)
	{
	}

	void installSignalHandlers(const StringArray& signalList, SignalHandler handler = &ignoreSignalHandler, bool nodefer = false)
	{
		for( StringArray::const_iterator signal = signalList.begin();
			signal != signalList.end();
			++signal )
		{
			int sig = getSignalForName(*signal);
			if( sig > 0 )
			{
				installHandler(sig, handler, nodefer);
			}
		}
	}


	void ignoreSignalHandler(int sig, siginfo_t* info, void* context)
	{
		// This should be considered evil, since it allocates memory.
		// It is NOT safe to call new from a signal handler.
		String output = Format("%1: Ignoring signal %2 (%3)\n",
			PROGRAM_NAME, getNameForSignal(sig), sig);

		doOutput(output);
	}

	void loggedSignalHandler(int sig, siginfo_t* info, void* context)
	{
		// This should be considered evil, since it allocates memory.
		// It is NOT safe to call new from a signal handler.
		String output = Format("%1: Detected signal %2 (%3)\n",
			PROGRAM_NAME, getNameForSignal(sig), sig);

		doOutput(output);

		// Install the default handler, send the signal again, and
		// restore the existing handler.
		struct sigaction original;
		sigaction(sig, NULL, &original);
		installHandler(sig, SIG_DFL, true);
		kill(getpid(), sig);
		sigaction(sig, &original, NULL);
	}

	void terminalSignalHandler(int sig, siginfo_t* info, void* context)
	{
		// This should be considered evil, since it allocates memory.
		// It is NOT safe to call new from a signal handler.
		String output = Format("%1: Received signal %2 (%3)\n",
			PROGRAM_NAME, getNameForSignal(sig), sig);

		doOutput(output);

		_exit(sig);
	}

	void signalForwardHandler(int sig, siginfo_t* info, void* context)
	{
		// This should be considered evil, since it allocates memory.
		// It is NOT safe to call new from a signal handler.
		String output = Format("%1: Forwarding signal %2 to children.\n",
			PROGRAM_NAME, getNameForSignal(sig));

		doOutput(output);

		// Install a handler to ignore the signal, send it to the whole
		// process group, and reinstall the handler.
		struct sigaction original;
		sigaction(sig, NULL, &original);
		installHandler(sig, &ignoreHandler, true);
		kill(0, sig);
		sigaction(sig, &original, NULL);
	}

	// Sleep for some number of seconds.  This will resume the sleep if interrupted.
	void doSleep(int sleep_time)
	{
		doOutput(Format("%1: Sleeping for %2 seconds\n", PROGRAM_NAME, sleep_time));

		struct timespec sleeper;
		sleeper.tv_sec = sleep_time;
		sleeper.tv_nsec = 0;

		unblockAllSignals();
		while( (::nanosleep(&sleeper, &sleeper) == -1) && (errno == EINTR) )
		{
			blockAllSignals();
			doOutput(Format("%1: Sleep interrupted.  Have %2 seconds left, will continue.\n"
					, PROGRAM_NAME
					, double(sleeper.tv_sec) + double(sleeper.tv_nsec) / 1000000000));
			unblockAllSignals();
		}
		blockAllSignals();
		doOutput(Format("%1: Done sleeping.\n", PROGRAM_NAME));
	}


	// Untokenize a string array with given start finish and between tokens.
	String untokenize(const StringArray& arr, const String& start = "\"", const String& finish = "\"", const String& separator = "\" \"")
	{
		String retval = start;

		if( !arr.empty() )
		{
			StringArray::const_iterator pos = arr.begin();

			retval += *pos;

			for(++pos; pos != arr.end(); ++pos)
			{
				retval += separator + *pos;
			}
		}

		retval += finish;
		return retval;
	}

	// Convert a null-terminated static string array to a StringArray
	StringArray makeStringArray(const char* const* vals)
	{
		StringArray retval;
		for( ; *vals; ++vals)
		{
			retval += *vals;
		}
		return retval;
	}

	void resetAllSignals(bool logged = false)
	{
		// NSIG may be defined by signal.h, otherwise 64 should be plenty.
#ifndef NSIG
#define NSIG 64
#endif

			// HP-UX seems to dispatch signals at the wrong time.  What appears to
			// happen to me is:
			// 1. The signal is sent
			// 2. Our handler receives it and logs it.
			// 3. Our handler alters the signal handler to use the default
			// 4. Our handler raises the signal again.
			// 5. The signal is not fired immediately but is queued.
			// 6. Our handler restores the our handler as the signal handler.
			// 7. The signal leaves the queue and hits our handler again.
			//
			// At no point does our handler ever finish, so the stack eventually
			// overflows.
			//
			// The SA_NODEFER flag is supposed to prevent that kind of behavior,
			// but it doesn't seem to in this case.  For this reason, I don't
			// think our "forwarding" will work on HPUX either.
#if defined(BLOCXX_HPUX)
		if( logged )
		{
			doOutput("resetAllSignals ignoring request to install logged signal handlers");
			logged = false;
		}
#endif
		// Reset all signals to the default.
		for(int sig = 0; sig < NSIG; ++sig)
		{
			if( !logged )
			{
				installHandler(sig, SIG_DFL);
			}
			else
			{
				// Logged, but retains the default actions.  If we don't set the
				// NODEFER flag, this will cause queued signals to fire forever.
				installHandler(sig, &loggedSignalHandler, true);
			}
		}
	}

	// Run a child process.
	pid_t spawnChild(const StringArray args)
	{
		doOutput(Format("%1: Spawning child with args %2\n", PROGRAM_NAME, untokenize(args)));

		Cstr::CstrArr<StringArray> safeArgs(args);

		pid_t val = fork();
		if( val == 0 )
		{
			resetAllSignals(true);

			if( OUTPUT_DESCRIPTOR != 1 )
			{
				// Close stdout and dup it.
				::close(1);
				::dup2(OUTPUT_DESCRIPTOR, 1);
			}
			const char* path = safeArgs.sarr[0];
			char* const* sargs = const_cast<char* const*>(safeArgs.sarr);
			execv(path, sargs);

			// Should not be reached unless the child could not execute.
			doOutput(Format("%1: Failed to spawn child.", PROGRAM_NAME));
			_exit(SIGALRM); // A better exit code is needed.
		}
		return val;
	}

} // end anonymous namespace

void Usage()
{
	doOutput(Format("Usage: %1 [options]\n\n", PROGRAM_NAME));
	doOutput(CmdLineParser::getUsage(g_options));
	doOutput("\n");
}

int main(int argc, const char** argv)
{
	String progname(PROGRAM_NAME);
	try
	{

		CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);
		if (parser.isSet(E_HELP_OPT))
		{
			Usage();
			return 0;
		}

		if (parser.isSet(E_OUTPUT_DESCRIPTOR_OPT))
		{
			OUTPUT_DESCRIPTOR = parser.getOptionValue(E_OUTPUT_DESCRIPTOR_OPT).toInt();
		}

		resetAllSignals(true);

		// Set up signal handlers to report the signal and quit.
		installSignalHandlers(String("term,int,bus,abrt,hup").tokenize(), &terminalSignalHandler, true);

		// Change the PROGRAM_NAME so things report properly.
		if (parser.isSet(E_PROGRAM_NAME_OPT))
		{
			progname = parser.getOptionValue(E_PROGRAM_NAME_OPT);
			PROGRAM_NAME = progname.c_str();
		}
		doOutput(Format("%1: Started with options %2\n", PROGRAM_NAME, untokenize(makeStringArray(argv))));
		doOutput(Format("%1: Started with pid %2 pgid %3\n", PROGRAM_NAME, ::getpid(), ::getpgid(0)));


		// Install signal handlers to block signals.
		if (parser.isSet(E_BLOCK_SIGNALS_OPT))
		{
			installSignalHandlers(parser.getOptionValue(E_BLOCK_SIGNALS_OPT).tokenize(","));
		}

		// Install signal handlers to forward signals to any children (the whole
		// process group).
		if (parser.isSet(E_FORWARD_SIGNALS_OPT))
		{
			installSignalHandlers(parser.getOptionValue(E_FORWARD_SIGNALS_OPT).tokenize(","), &signalForwardHandler, true);
		}


		// Spawn a child if needed.
		pid_t childpid = 0;
		if (parser.isSet(E_SPAWN_CHILD_OPT))
		{
			StringArray args = parser.getOptionValue(E_SPAWN_CHILD_OPT).tokenize();
			childpid = spawnChild(args);
		}

		// Sleep if needed.
		if (parser.isSet(E_SLEEP_TIME_OPT))
		{
			doSleep(parser.getOptionValue(E_SLEEP_TIME_OPT).toInt());
		}


		// Zombie repellent: Wait for a child if it was started
		if( childpid != 0 )
		{
			doOutput(Format("%1: Checking child status\n", PROGRAM_NAME));

			int status = 0;

			// Waitpid can be interrupted.  Wait until the child quits.
			unblockAllSignals();
			pid_t childval = ::waitpid(childpid, &status, 0);
			while( (childval == -1) && (errno == EINTR) )
			{
				childval = ::waitpid(childpid, &status, 0);
			}
			blockAllSignals();

			// Make sure things worked before reporting things incorrectly.
			if( childval == childpid )
			{
				if( WIFSIGNALED(status) )
				{
					doOutput(Format("%1: Rough child exit: %2 (%3)\n", PROGRAM_NAME, getNameForSignal(WTERMSIG(status)), WTERMSIG(status)));
				}
				else if( WIFEXITED(status) )
				{
					int code = WEXITSTATUS(status);
					doOutput(Format("%1: Child exit: %2 (%3, %4)\n", PROGRAM_NAME, getNameForSignal(code), code, status));
				}
				else
				{
					int code = WEXITSTATUS(status);
					doOutput(Format("%1: Unknown child exit: %2 (%3, %4)\n", PROGRAM_NAME, getNameForSignal(code), code, status));
				}
			}
			else
			{
				doOutput(Format("%1: Child did not exit.  Wait returned %2, error=%3 (%4)\n", PROGRAM_NAME, childval, strerror(errno), errno));
			}
		}
	}
	catch (CmdLineParserException& e)
	{
		switch (e.getErrorCode())
		{
		case CmdLineParser::E_INVALID_OPTION:
			doOutput(Format("Invalid option: %1\n", e));
			break;
		case CmdLineParser::E_MISSING_ARGUMENT:
			doOutput(Format("Argument not specified for option: %1\n", e));
			break;
		default:
			doOutput(Format("Command line parsing error: %1\n", e));
			break;
		}
		Usage();
		return 126;
	}

	deliverPendingSignals();
	
	doOutput(Format("%1: Quitting.\n", PROGRAM_NAME));

	return 0;
}
