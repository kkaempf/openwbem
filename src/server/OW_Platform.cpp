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
#include "OW_config.h"
#include "OW_Platform.hpp"
//#include "OW_Environment.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Format.hpp"
#include "OW_PidFile.hpp"
extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#ifdef OW_HAVE_GETOPT_H
#include <getopt.h>
#else
#include <stdlib.h> // for getopt on Solaris
#endif
#include <unistd.h> // for getpid, getuid, etc.
#include <signal.h>
#include <fcntl.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
#include <pwd.h>
}
#include <cstring>
#include <cstdio>

namespace OpenWBEM
{

using std::ostream;
using std::endl;

OW_DEFINE_EXCEPTION(Daemon);

namespace Platform
{

extern "C" {
static void theSigHandler(int sig);
}


static Options processCommandLineOptions(int argc, char** argv);
static void handleSignal(int sig);
static void setupSigHandler(bool dbgFlg);

static Reference<UnnamedPipe> plat_upipe;

//////////////////////////////////////////////////////////////////////////////
Options
daemonInit( int argc, char* argv[] )
{
	return processCommandLineOptions(argc, argv);
}
/**
 * daemonize() - detach process from user and disappear into the background
 * Throws DaemonException on error.
 */
void
daemonize(bool dbgFlg, const String& daemonName)
{
	if(!dbgFlg)
	{
		if(getuid() != 0)
		{
			OW_THROW(DaemonException, format("%1 must run as root. aborting...", daemonName).c_str());
		}
	}
	String pidFile(OW_PIDFILE_DIR);
	pidFile += "/";
	pidFile += daemonName;
	pidFile += ".pid";
	int pid = PidFile::checkPid(pidFile.c_str());
	// Is there already another instance of the cimom running?
	if(pid != -1)
	{
		OW_THROW(DaemonException,
			format("Another instance of %1 is already running [%2]",
				daemonName, pid).c_str());
	}
	if(!dbgFlg)
	{
		pid = fork();
		switch(pid)
		{
			case 0:
				break;
			case -1:
				OW_THROW(DaemonException,
					format("FAILED TO DETACH FROM THE TERMINAL - First fork : %1",
						strerror(errno)).c_str());
			default:
				_exit(0);			 // exit the original process
		}
		if (setsid() < 0)					  // shoudn't fail on linux
		{
			OW_THROW(DaemonException,
				"FAILED TO DETACH FROM THE TERMINAL - setsid failed");
		}
		pid = fork();
		switch(pid)
		{
			case 0:
				break;
			case -1:
				OW_THROW(DaemonException,
					format("FAILED TO DETACH FROM THE TERMINAL - Second fork : %1",
						strerror(errno)).c_str());
				exit(0);
			default:
				_exit(0);
		}
		chdir("/");
		umask(0);
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
	PidFile::writePid(pidFile.c_str());
	//String msg;
	//msg.format(DAEMON_NAME " is now running [PID=%d]", getpid());
	//Environment::logInfo(msg);
	initSig();
	setupSigHandler(dbgFlg);
}
//////////////////////////////////////////////////////////////////////////////
int
daemonShutdown(const String& daemonName)
{
	String pidFile(OW_PIDFILE_DIR);
	pidFile += "/";
	pidFile += daemonName;
	pidFile += ".pid";
	PidFile::removePid(pidFile.c_str());
	shutdownSig();
	return 0;
}
#ifdef OW_HAVE_GETOPT_LONG
//////////////////////////////////////////////////////////////////////////////
static struct option   long_options[] =
{
	{ "debug", 0, NULL, 'd' },
	{ "config", required_argument, NULL, 'c' },
	{ "help", 0, NULL, 'h' },
	{ 0, 0, 0, 0 }
};
#endif
static const char* const short_options = "dc:h";
//////////////////////////////////////////////////////////////////////////////
static Options
processCommandLineOptions(int argc, char** argv)
{
	Options rval;
#ifdef OW_HAVE_GETOPT_LONG
	int optndx = 0;
	optind = 1;
	int c = getopt_long(argc, argv, short_options, long_options, &optndx);
#else
	optind = 1;
	int c = getopt(argc, argv, short_options);
#endif
	while(c != -1)
	{
		switch(c)
		{
			case 'd':
				rval.debug = true;
				break;
			case 'c':
				rval.configFile = true;
				rval.configFilePath = optarg;
				break;
			case 'h':
				rval.help = true;
				return rval;
			default:
				rval.help = true;
				rval.error = true;
				return rval;
		}
#ifdef OW_HAVE_GETOPT_LONG
		c = getopt_long(argc, argv, short_options, long_options, &optndx);
#else
		c = getopt(argc, argv, short_options);
#endif
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
#if defined (OW_FREEBSD) || defined (OW_DARWIN)
typedef void (*sighandler_t)(int);
#endif

static void
handleSignalAux(int sig, sighandler_t handler)
{
	struct sigaction temp;
	memset(&temp, '\0', sizeof(temp));
	sigaction(sig, 0, &temp);
	if(temp.sa_handler != SIG_IGN)
	{
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
		 */
		temp.sa_flags = 0;
		sigaction(sig, &temp, NULL);
	}
}
static void
handleSignal(int sig)
{
	handleSignalAux(sig, theSigHandler);
}
static void
ignoreSignal(int sig)
{
	handleSignalAux(sig, SIG_IGN);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" {
static void
theSigHandler(int sig)
{
	try
	{
		switch(sig)
		{
			case SIGTERM:
			case SIGINT:
				pushSig(SHUTDOWN);
				break;
			case SIGHUP:
				pushSig(REINIT);
				break;
		}
	}
	catch (...) // can't let exceptions escape from here or we'll segfault.
	{
	}
}
} // extern "C"
//////////////////////////////////////////////////////////////////////////////
static void
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
	if(dbgFlg)
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
//	handleSignal(SIGUSR1);
	ignoreSignal(SIGTTIN);
	ignoreSignal(SIGTTOU);
	ignoreSignal(SIGTSTP);
#ifdef OW_OPENSERVER
	ignoreSignal(SIGPOLL);
#else
	ignoreSignal(SIGIO);
#endif
	ignoreSignal(SIGPIPE);
	// ?
	ignoreSignal(SIGIOT);
	ignoreSignal(SIGBUS);
	ignoreSignal(SIGCONT);
	ignoreSignal(SIGURG);
	ignoreSignal(SIGXCPU);
	ignoreSignal(SIGXFSZ);
	ignoreSignal(SIGVTALRM);
	ignoreSignal(SIGPROF);
#ifdef SIGPWR // FreeBSD doesn't have SIGPWR
	ignoreSignal(SIGPWR);
#endif
	// ?
	//handleSignal(SIGALRM);
	//handleSignal(SIGABRT);
	//handleSignal(SIGILL);
	//handleSignal(SIGSEGV);
	//handleSignal(SIGFPE);
	//handleSignal(SIGSTKFLT);
}
//////////////////////////////////////////////////////////////////////////////
String getCurrentUserName()
{
	uid_t uid = getuid();
	struct passwd* p = getpwuid(uid);
	if (p)
	{
		return p->pw_name;
	}
	return "";
}

//////////////////////////////////////////////////////////////////////////////
void initSig()
{
	plat_upipe = UnnamedPipe::createUnnamedPipe();
}
//////////////////////////////////////////////////////////////////////////////
void pushSig(int sig)
{
	plat_upipe->writeInt(sig);
	// don't throw from this function, it may cause a segfault or deadlock.
}
//////////////////////////////////////////////////////////////////////////////
int popSig()
{
	int tmp = -2;
	if (plat_upipe->readInt(&tmp) < 0)
		return -1;
	return tmp;
}
//////////////////////////////////////////////////////////////////////////////
void shutdownSig()
{
	plat_upipe = 0;
}

//////////////////////////////////////////////////////////////////////////////
SelectableIFCRef getSigSelectable()
{
	return plat_upipe;
}

} // end namespace Platform
} // end namespace OpenWBEM

