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
 * @author Jon Carey
 * @author Dan Nuffer
 * @author Bart Whiteley
 */

#include "OW_config.h"
#include "OW_Platform.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/Format.hpp"
#include "OW_PidFile.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_PlatformSignal.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_Logger.hpp"
#include "blocxx/PosixUnnamedPipe.hpp"

#ifdef OW_NETWARE
#include "blocxx/Condition.hpp"
#include "blocxx/NonRecursiveMutex.hpp"
#include "blocxx/NonRecursiveMutexLock.hpp"
#endif

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(OW_HAVE_GETOPT_H) && !defined(OW_GETOPT_AND_UNISTD_CONFLICT)
	#include <getopt.h>
#else
	#include <stdlib.h> // for getopt on Solaris
#endif


#ifdef OW_WIN32
	#include <process.h>
	#define getpid _getpid
#else
	#include <unistd.h> // for getpid, getuid, etc.
	#include <sys/wait.h>
#endif

#include <signal.h>
#include <fcntl.h>

#if defined (OW_HAVE_PWD_H)
	#include <pwd.h>
#endif

#if defined (OW_HAVE_SYS_RESOURCE_H)
	#include <sys/resource.h>
#endif

#if defined(OW_HAVE_GRP_H)
	#include <grp.h>
#endif

#ifdef OW_NETWARE
	#include <nks/vm.h>
	#include <nks/netware.h>
	#include <netware.h>
	#include <event.h>
	#include <library.h>
#endif
}

#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

namespace OW_NAMESPACE
{

using std::ostream;
using std::endl;

OW_DEFINE_EXCEPTION_WITH_ID(Daemon);

namespace Platform
{

extern "C" {
static void theSigHandler(int sig, siginfo_t* info, void* context);
}

namespace
{
const int RESTART_RETURN_VALUE = 94; // this is just a random value and has no other meaning or significance.

void handleSignal(int sig);
void ignoreSignal(int sig);
void setupSigHandler(bool dbgFlg);

UnnamedPipeRef plat_upipe;

UnnamedPipeRef daemonize_upipe;

char** g_argv = 0;

#ifdef OW_NETWARE
Condition g_shutdownCond;
bool g_shutDown = false;
NonRecursiveMutex g_shutdownGuard;
void* WarnFuncRef = NULL;
rtag_t EventRTag;
event_handle_t DownEvent;
bool FromEventHandler = false;
#endif

}

//////////////////////////////////////////////////////////////////////////////
void
daemonInit( int argc, char* argv[] )
{
	g_argv = argv;
}

// win32 version
#ifdef OW_WIN32
/**
 * daemonize() - detach process from user and disappear into the background
 * Throws DaemonException on error.
 */
void
daemonize(bool dbgFlg, const String& daemonName, const String& pidFile, bool restartOnFatalError, const String& loggerComponentName)
{
	initSig();

}
#endif // #ifdef OW_WIN32

// netware version
#ifdef OW_NETWARE
void
daemonize(bool dbgFlg, const String& daemonName, const String& pidFile, bool restartOnFatalError, const String& loggerComponentName)
{
	{
		NonRecursiveMutexLock l(g_shutdownGuard);
		g_shutDown = false;
	}
	initDaemonizePipe();

	int pid = -1;
	if (!dbgFlg)
	{
		chdir("/");
		close(0);
		close(1);
		close(2);
		open("/dev/null", O_RDONLY);
		open("/dev/null", O_WRONLY);
		dup(1);
	}
	else
	{
		pid = getpid();
	}
	umask(0077); // ensure all files we create are only accessible by us.
	Logger lgr(loggerComponentName);
	OW_LOG_INFO(lgr, Format("Platform::daemonize() pid = %1", ::getpid()));
	initSig();
	setupSigHandler(dbgFlg);

#ifdef OW_HAVE_PTHREAD_ATFORK
	// this registers shutdownSig to be run in the child whenever a fork() happens.
	// This will prevent a child process from writing to the signal pipe and shutting down the parent.
	::pthread_atfork(NULL, NULL, &shutdownSig);
#endif
}
#endif // #ifdef OW_NETWARE


// posix version
#if !defined(OW_WIN32) && !defined(OW_NETWARE)

namespace
{
	void cleanupChild(pid_t childPid, int& status)
	{
		pid_t rv = waitpid(childPid, &status, WNOHANG);
		if (rv != childPid)
		{
			perror("cleanupChild: waitpid()");
			exit(1);
		}
	}

	void doNothingHandler(int signo)
	{
		// nothing
	}
}

void
daemonize(bool dbgFlg, const String& daemonName, const String& pidFile, bool restartOnFatalError, const String& loggerComponentName)
{
	Logger logger(loggerComponentName);
	initDaemonizePipe();

	int pid = -1;

	pid = PidFile::checkPid(pidFile.c_str());
	// Is there already another instance of the cimom running?
	if (pid != -1)
	{
		OW_THROW(DaemonException,
			Format("Another instance of %1 is already running [%2]",
				daemonName, pid).c_str());
	}

	if (!dbgFlg)
	{
		if( !getenv("OWNOCHDIR") )
		{
			OW_LOG_DEBUG3(logger, "Changing directories to / ...");
			if( chdir("/") == -1 )
			{
				OW_THROW_ERRNO_MSG(DaemonException, "Failed to change directories to /");
			}
		}
		else
		{
			OW_LOG_DEBUG(logger, "Not changing directories because the OWNOCHDIR environment variable is set.");
		}

		pid = fork();
		switch (pid)
		{
			case 0:
				break;
			case -1:
				OW_THROW_ERRNO_MSG(DaemonException,
					"FAILED TO DETACH FROM THE TERMINAL - First fork");
			default:
				// The output handle needs to be closed so that if the child terminates, the
				// kernel will close the pipe, and the following readInt() will not hang forever.
				dynamic_pointer_cast<PosixUnnamedPipe>(daemonize_upipe)->closeOutputHandle();
				int status = DAEMONIZE_FAIL;

				if (daemonize_upipe->readInt(&status) < 1)
				{
					cerr << Format("Error reading status from child %1: %2", daemonName, strerror(errno)) << endl;
					_exit(1);
				}
				else if (status != DAEMONIZE_SUCCESS)
				{
					cerr << Format("Error starting %1. Check the log files.", daemonName) << endl;
					_exit(1);
				}
				_exit(0); // exit the original process
		}
		if (setsid() < 0)					  // shoudn't fail on linux
		{
			OW_THROW(DaemonException,
				"FAILED TO DETACH FROM THE TERMINAL - setsid failed");
		}

		// second fork really detaches us
		pid = fork();
		switch (pid)
		{
			case 0:
				break;
			case -1:
				{
					// Save the error number, since the sendDaemonizeStatus function can cause it to change.
					int saved_errno = errno;
					sendDaemonizeStatus(DAEMONIZE_FAIL);
					// Restore the real error number.
					errno = saved_errno;
					OW_THROW_ERRNO_MSG(DaemonException,
						"FAILED TO DETACH FROM THE TERMINAL - Second fork");
				}
			default:
				_exit(0);
		}

		// reattach stdin, stdout, stderr
		close(0);
		close(1);
		close(2);
		open("/dev/null", O_RDONLY);
		open("/dev/null", O_WRONLY);

		// dup() reports a warning if the return value is ignored.  We can't do
		// anything about it failing, so we will just ignore it.
		int returnValueIgnored = dup(1);
		(void) returnValueIgnored;

		// to prevent a race condition between parent and child, block the signals now!

		sigset_t newmask, oldmask;
		sigemptyset(&newmask);
		sigaddset(&newmask, SIGCHLD);
		sigaddset(&newmask, SIGTERM);
		sigaddset(&newmask, SIGHUP);

		if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		{
			int saved_errno = errno;
			sendDaemonizeStatus(DAEMONIZE_FAIL);
			errno = saved_errno;
			OW_THROW_ERRNO_MSG(DaemonException,
				"Platform::daemonize(): sigprocmask(SIG_BLOCK)");
		}

		// Write out pid file here so that we write out the pid of the
		// child-watcher parent process, and not the child.
		//
		if (PidFile::writePid(pidFile.c_str()) == -1)
		{
			// Save the error number, since the sendDaemonizeStatus function can cause it to change.
			int saved_errno = errno;
			sendDaemonizeStatus(DAEMONIZE_FAIL);
			// Restore the real error number.
			errno = saved_errno;
			OW_THROW_ERRNO_MSG(DaemonException,
				Format("Failed to write the pid file (%1)", pidFile).c_str());
		}

		// third fork to create the child-watcher parent process. If the child segfaults or requests a restart, then it will be restarted.
fork_child:

		pid = fork();
		switch (pid)
		{
			case 0: // child, so just continue on
				break;
			case -1:
			{
				int saved_errno = errno;
				sendDaemonizeStatus(DAEMONIZE_FAIL);
				errno = saved_errno;
				OW_THROW_ERRNO_MSG(DaemonException,
					"Platform::daemonize(): third fork() failed.");
			}
			break;
			default: // parent
			{
				// Monitor the child's status.
				// We just block waiting for signals: SIGCHLD, SIGTERM, SIGHUP and then take the appropriate action.

				// by default, SIGCHLD is set to be ignored so unless we happen
				// to be blocked on sigwaitinfo() at the time that SIGCHLD
				// is set on us we will not get it.  To fix this, we simply
				// register a signal handler.  Since we've masked the signal
				// above, it will not affect us.  At the same time we will make
				// it a queued signal so that if more than one are set on us,
				// sigwaitinfo() will get them all.
				struct sigaction action;
				action.sa_handler = doNothingHandler;
				sigemptyset(&action.sa_mask);
				action.sa_flags = SA_SIGINFO; // make it a queued signal
				sigaction(SIGCHLD, &action, NULL);

wait_for_signal:

				siginfo_t siginfo;
				int rv = -1;
				do
				{
#if defined(HAVE_DECL_SIGWAITINFO) && HAV_DECL_SIGWAITINFO
					rv = sigwaitinfo(&newmask, &siginfo);
#else
					rv = sigwait(&newmask, &siginfo.si_signo);
					if( rv != 0 )
					{
						errno = rv;
						rv = -1;
					}
#endif
				} while (rv == -1 && errno == EINTR);

				if (rv == -1)
				{
					// shouldn't ever happen
					abort();
				}

				switch (siginfo.si_signo)
				{
					case SIGCHLD:
					{
						// find out what happened to the child
						int status;
						pid_t pidrv = waitpid(pid, &status, WNOHANG);
						if (pidrv == -1)
						{
							perror("parent: waitpid()");
							PidFile::removePid(pidFile.c_str());
							exit(1);
						}

						// if the child exited
						if (pidrv == pid)
						{
							if (WIFEXITED(status) && WEXITSTATUS(status) == RESTART_RETURN_VALUE)
							{
								goto fork_child;
							}
							else if (WIFEXITED(status))
							{
								PidFile::removePid(pidFile.c_str());
								exit(WEXITSTATUS(status));
							}
							else if (WIFSIGNALED(status))
							{
								// the cimom or one of it's providers had a bad bug... restart it.
								int termsig = WTERMSIG(status);
								switch (termsig)
								{
									case SIGABRT:
									case SIGILL:
									case SIGBUS:
									case SIGSEGV:
									case SIGFPE:
										if (restartOnFatalError)
										{
											goto fork_child;
										}
										else
										{
											PidFile::removePid(pidFile.c_str());
											exit(1);
										}
									default:
										{
											PidFile::removePid(pidFile.c_str());
											exit(1);
										}
								}
							}
							else
							{
								abort(); // something is buggy...
							}
						}
						else // if (WIFSTOPPED(status) || WIFCONTINUED(status)) or ??? just ignore it.
						{
							// debugger attached to it, so ignore the signal
							goto wait_for_signal;
						}

					}
					break;
					case SIGHUP:
					case SIGTERM:
					{
						// propagate the signal to the child
						if (kill(pid, siginfo.si_signo) == -1)
						{
							perror("Platform::daemonize(): kill()");
							PidFile::removePid(pidFile.c_str());
							exit(1);
						}

						// The child should exit upon receiving either a SIGHUP or a SIGTERM
						// Wait 2 minutes for it to exit, if it hasn't then send a SIGKILL
						const long WAIT_SECONDS = 120;
						timespec ts = {WAIT_SECONDS, 0};
						int rv = -1;
						sigset_t chldmask;
						sigemptyset(&chldmask);
						sigaddset(&chldmask, SIGCHLD);
						sigaddset(&chldmask, SIGALRM);
						int childSignal = 0;

 						// Install an alarm for the number of seconds required.
						handleSignal(SIGALRM); // The default OW signal handler will ignore this signal.
						alarm(WAIT_SECONDS);
						rv = sigwait(&chldmask, &childSignal);
						ignoreSignal(SIGALRM);
						int childStatus = 0;

						if ( rv == 0 )
						{
							if (childSignal == SIGALRM) // wait timed out
							{
								if (kill(pid, SIGKILL) == -1)
								{
									perror("Platform::daemonize(): kill()");
									PidFile::removePid(pidFile.c_str());
									exit(1);
								}
								cleanupChild(pid, childStatus);
							}
							else if (childSignal == SIGCHLD) // it exited!
							{
								cleanupChild(pid, childStatus);
							}
							else
							{
								abort(); // I screwed up something.  Another signal slipped through. :-(
							}
						}
						else
						{
							abort(); // I screwed up something :-(
						}

						// restart if requested
						if (siginfo.si_signo == SIGHUP)
						{
							goto fork_child;
						}
						else
						{
							PidFile::removePid(pidFile.c_str());
							// if the child exited, return the same value
							if (WIFEXITED(childStatus))
							{
								exit(WEXITSTATUS(childStatus));
							}
							else
							{
								exit(1);
							}
						}
					}
					break;
				}

			}
			break;
		}

		// child needs to restore signal mask
		if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		{
			int saved_errno = errno;
			sendDaemonizeStatus(DAEMONIZE_FAIL);
			errno = saved_errno;
			OW_THROW_ERRNO_MSG(DaemonException,
				"Platform::daemonize(): sigprocmask(SIG_SETMASK)");
		}

	}
	else
	{
		pid = getpid();
	}
	umask(0077); // ensure all files we create are only accessible by us.

	// PidFile::writePid used to be here...

	Logger lgr(loggerComponentName);
	OW_LOG_INFO(lgr, Format("Platform::daemonize() pid = %1", ::getpid()));

	initSig();

	setupSigHandler(dbgFlg);


#ifdef OW_HAVE_PTHREAD_ATFORK
	// this registers shutdownSig to be run in the child whenever a fork() happens.
	// This will prevent a child process from writing to the signal pipe and shutting down the parent.
	::pthread_atfork(NULL, NULL, &shutdownSig);
#endif
}
#endif // #if !defined(OW_WIN32) && !defined(OW_NETWARE)

//////////////////////////////////////////////////////////////////////////////
int
daemonShutdown(const String& daemonName, const String& pidFile)
{
#if defined(OW_NETWARE)
	(void)daemonName;
	{
		NonRecursiveMutexLock l(g_shutdownGuard);
		g_shutDown = true;
		g_shutdownCond.notifyAll();
		pthread_yield();
	}
	if(!FromEventHandler)
	{
		UnRegisterEventNotification(DownEvent);
	}
#endif
	shutdownSig();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void rerunDaemon()
{
	// If we've dropped privileges, just re-running ourselves won't do much, since we'll lose the monitor.

	exit(RESTART_RETURN_VALUE);
}

//////////////////////////////////////////////////////////////////////////////
void restartDaemon()
{
#ifdef WIN32
	rerunDaemon();
#else
	::kill(::getpid(), SIGHUP);
#endif
}

#ifndef WIN32

namespace
{

//////////////////////////////////////////////////////////////////////////////
#if !defined(OW_HAVE_SIGHANDLER_T)
typedef void (*sighandler_t)(int);
#endif

void
handleSignalAux(int sig, sighandler_t handler)
{
	struct sigaction temp;
	memset(&temp, '\0', sizeof(temp));
	sigaction(sig, 0, &temp);
	temp.sa_handler = handler;
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
	 * This also clears the SA_SIGINFO flag so that the sa_handler member
	 * may be safely used.
	 */
	temp.sa_flags = 0;
	sigaction(sig, &temp, NULL);
}

// This typedef is not required to be defined anywhere in the header files,
// but POSIX does show the signature of the function.
// See http://www.opengroup.org/onlinepubs/009695399/functions/sigaction.html
typedef void (*full_sighandler_t)(int,siginfo_t*,void*);

// A signal handler installer for a full sigaction handler.  This is
// different from the normal sighandler type only by the flags in the
// handler and the field used in the sigaction struct
// (sa_sigaction/sa_handler).
void
handleSignalAux(int sig, full_sighandler_t handler)
{
	struct sigaction temp;
	memset(&temp, '\0', sizeof(temp));
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
	sigaction(sig, &temp, NULL);
}

void
handleSignal(int sig)
{
	handleSignalAux(sig, theSigHandler);
}
void
ignoreSignal(int sig)
{
	handleSignalAux(sig, SIG_IGN);
}

} // end unnamed namespace

#endif

//////////////////////////////////////////////////////////////////////////////
extern "C" {
static void
theSigHandler(int sig, siginfo_t* info, void* context)
{
	int savedErrno = errno;
	try
	{
		Signal::SignalInformation extractedSignal;
		if( info )
		{
			Signal::extractSignalInformation( *info, extractedSignal );
		}

		switch (sig)
		{
			case SIGTERM:
			case SIGINT:
#if defined(OW_NETWARE)
			case SIGABRT:
#endif
				extractedSignal.signalAction = SHUTDOWN;
				pushSig(extractedSignal);
				break;
#ifndef OW_WIN32
			case SIGHUP:
				extractedSignal.signalAction = REINIT;
				pushSig(extractedSignal);
				break;
#endif
		}
	}
	catch (...) // can't let exceptions escape from here or we'll segfault.
	{
	}
	errno = savedErrno;

}

#ifndef WIN32

static void
fatalSigHandler(int sig, siginfo_t* info, void* context)
{
	// we can't do much of *anything* besides restart here, since whatever caused the signal has left us in an unpredictable state,
	// it's unknown what could or could not work, not to mention that there are hardly any signal safe functions we could call.
	Platform::rerunDaemon();
}

#ifdef OW_NETWARE
static void
netwareExitHandler(void*)
{
	theSigHandler(SIGTERM);
	pthread_yield();
	NonRecursiveMutexLock l(g_shutdownGuard);
	while(!g_shutDown)
	{
		g_shutdownCond.wait(l);
	}
}

static int
netwareShutDownEventHandler(void*,
	void*, void*)
{
	FromEventHandler = true;
	theSigHandler(SIGTERM);
	pthread_yield();
	NonRecursiveMutexLock l(g_shutdownGuard);
	while(!g_shutDown)
	{
		g_shutdownCond.wait(l);
	}
	return 0;
}
#endif

#endif
} // extern "C"
#ifndef WIN32

namespace
{
//////////////////////////////////////////////////////////////////////////////
void
setupSigHandler(bool dbgFlg)
{
	/* Here's a note from the glibc documentation about signal():
	 *Compatibility Note:* A problem encountered when working with the
	 `signal' function is that it has different semantics on BSD and SVID
	 systems.  The difference is that on SVID systems the signal handler is
	 deinstalled after signal delivery.  On BSD systems the handler must be
	 explicitly deinstalled.  In the GNU C Library we use the BSD version by
	 default.  To use the SVID version you can either use the function
	 `sysv_signal' (see below) or use the `_XOPEN_SOURCE' feature select
	 macro ( Feature Test Macros).  In general, use of these
	 functions should be avoided because of compatibility problems.  It is
	 better to use `sigaction' if it is available since the results are much
	 more reliable.
	 We avoid using signal and use sigaction instead.
	 */
	if (dbgFlg)
	{
		handleSignal(SIGINT);
	}
	else
	{
		ignoreSignal(SIGINT);
	}
	handleSignal(SIGTERM);
	handleSignal(SIGHUP);
//	handleSignal(SIGUSR2);

// The thread code uses SIGUSR1 to implement cooperative cancellation, since
// sending a signal can wake up a blocked system call.
//	handleSignal(SIGUSR1);

	ignoreSignal(SIGTTIN);
	ignoreSignal(SIGTTOU);
	ignoreSignal(SIGTSTP);
#ifdef SIGPOLL
	ignoreSignal(SIGPOLL);
#endif
#ifdef SIGIO
	ignoreSignal(SIGIO);
#endif
	ignoreSignal(SIGPIPE);
	// ?
#ifdef SIGIOT // NetWare doesn't have this signal
	ignoreSignal(SIGIOT);
#endif
	ignoreSignal(SIGCONT);
#ifdef SIGURG // NetWare doesn't have this signal
	ignoreSignal(SIGURG);
#endif
#ifdef SIGXCPU // NetWare doesn't have this signal
	ignoreSignal(SIGXCPU);
#endif
#ifdef SIGXFSZ // NetWare doesn't have this signal
	ignoreSignal(SIGXFSZ);
#endif
#ifdef SIGVTALRM // NetWare doesn't have this signal
	ignoreSignal(SIGVTALRM);
#endif
#ifdef SIGPROF // NetWare doesn't have this signal
	ignoreSignal(SIGPROF);
#endif
#ifdef SIGPWR // FreeBSD doesn't have SIGPWR
	ignoreSignal(SIGPWR);
#endif

	//handleSignal(SIGALRM);
	//handleSignal(SIGSTKFLT);

#ifdef OW_NETWARE
	int rv;
	if ((rv = NXVmRegisterExitHandler(netwareExitHandler, 0) != 0))
	{
		OW_THROW(DaemonException,
			Format("FAILED TO REGISTER EXIT HANDLER "
			"NXVmRegisterExitHandler returned %1", rv).c_str());
	}
	EventRTag = AllocateResourceTag(getnlmhandle(), "Server down event",
		EventSignature);
	if(!EventRTag)
	{
		OW_THROW(DaemonException, "AllocationResourceTag FAILED");
	}
	NX_WRAP_INTERFACE((void*)netwareShutDownEventHandler, 3, &WarnFuncRef);
	DownEvent = RegisterForEventNotification(EventRTag,
		EVENT_DOWN_SERVER | EVENT_CONSUMER_MT_SAFE,
		EVENT_PRIORITY_APPLICATION, (Warn_t)WarnFuncRef, (Report_t)0, 0);
	if(!DownEvent)
	{
		OW_THROW(DaemonException, "FAILED to register for shutdown event");
	}
#endif
}

} // end unnamed namespace

#else // WIN32

BOOL WINAPI CtrlHandlerRoutine(DWORD dwCtrlType)
{
	theSigHandler(SIGTERM, 0, 0);
	return TRUE;
}

#pragma message(Reminder "TODO: Figure out somewhere else to call this code")
void installFatalSignalHandlers()
{
	::SetConsoleCtrlHandler(CtrlHandlerRoutine, TRUE);
}

void removeFatalSignalHandlers()
{
	::SetConsoleCtrlHandler(CtrlHandlerRoutine, FALSE);
}

#endif // WIN32

//////////////////////////////////////////////////////////////////////////////
void initDaemonizePipe()
{
	daemonize_upipe = UnnamedPipe::createUnnamedPipe();
	daemonize_upipe->setTimeouts(Timeout::infinite);
}

//////////////////////////////////////////////////////////////////////////////
void sendDaemonizeStatus(int status)
{
	if (daemonize_upipe)
	{
		daemonize_upipe->writeInt(status);
	}
}

//////////////////////////////////////////////////////////////////////////////
void initSig()
{
	plat_upipe = UnnamedPipe::createUnnamedPipe();
	plat_upipe->setBlocking(UnnamedPipe::E_NONBLOCKING);
}
//////////////////////////////////////////////////////////////////////////////
void pushSig(const Signal::SignalInformation& sig)
{
	if (plat_upipe)
	{
		Signal::flattenSignalInformation(sig, plat_upipe);
	}
	// don't throw from this function, it may cause a segfault or deadlock.
}
//////////////////////////////////////////////////////////////////////////////
int popSig(Signal::SignalInformation& sig)
{
	int tmp = -2;
	if (plat_upipe)
	{
		if( !Signal::unflattenSignalInformation(sig, plat_upipe) )
		{
			return -1;
		}
		tmp = sig.signalAction;
	}
	return tmp;
}
//////////////////////////////////////////////////////////////////////////////
void shutdownSig()
{
	plat_upipe = 0;
}

//////////////////////////////////////////////////////////////////////////////
Select_t getSigSelectable()
{
	return plat_upipe->getReadSelectObj();
}


//////////////////////////////////////////////////////////////////////////////

} // end namespace Platform
} // end namespace OW_NAMESPACE

