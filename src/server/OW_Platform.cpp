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
}

#include <iostream>
#include <cstring>
#include <cstdio>

using std::ostream;
using std::endl;

DEFINE_EXCEPTION(Daemon);

OW_Reference<OW_UnnamedPipe> OW_Platform::plat_upipe;

static OW_Platform::Options processCommandLineOptions(int argc, char** argv);
static void handleSignal(int sig);
static void setupSigHandler(const OW_Bool& dbgFlg);

extern "C" {
static void theSigHandler(int sig);
}

//////////////////////////////////////////////////////////////////////////////
OW_Platform::Options
OW_Platform::daemonInit( int argc, char* argv[] )
{
#ifdef OW_USE_GNU_PTH
    // This function is the very first thing called from main, so do pth_init here.
    pth_init();
#endif
	return processCommandLineOptions(argc, argv);
}

/**
 * daemonize() - detach process from user and disappear into the background
 * Throws OW_DaemonException on error.
 */
void
OW_Platform::daemonize(const OW_Bool& dbgFlg, const OW_String& daemonName)
{
	//OW_Bool dbgFlg = OW_Environment::getConfigItem(OW_ConfigOpts::OW_DEBUG_opt).
	//	equalsIgnoreCase("true");

	if(!dbgFlg)
	{
		if(getuid() != 0)
		{
			OW_THROW(OW_DaemonException, format("%1 must run as root. aborting...", daemonName).c_str());
		}
	}

	OW_String pidFile(OW_PIDFILE_DIR);
	pidFile += "/";
	pidFile += daemonName;
	pidFile += ".pid";

	int pid = OW_PidFile::checkPid(pidFile.c_str());

	// Is there already another instance of the cimom running?
	if(pid != -1)
	{
		OW_THROW(OW_DaemonException,
			format("Another instance of %1 is already running [%2]",
				daemonName, pid).c_str());
	}

	if(!dbgFlg)
	{
#ifdef OW_USE_GNU_PTH
        pid = pth_fork();
#else
		pid = fork();
#endif
		switch(pid)
		{
			case 0:
				break;
			case -1:
				OW_THROW(OW_DaemonException,
					format("FAILED TO DETACH FROM THE TERMINAL - First fork : %1",
						strerror(errno)).c_str());

			default:
				_exit(0);			 // exit the original process
		}

		if (setsid() < 0)					  // shoudn't fail on linux
		{
			OW_THROW(OW_DaemonException,
				"FAILED TO DETACH FROM THE TERMINAL - setsid failed");
		}

#ifdef OW_USE_GNU_PTH
        pid = pth_fork();
#else
		pid = fork();
#endif
		switch(pid)
		{
			case 0:
				break;

			case -1:
				OW_THROW(OW_DaemonException,
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

	OW_PidFile::writePid(pidFile.c_str());
	//OW_String msg;
	//msg.format(OW_DAEMON_NAME " is now running [PID=%d]", getpid());
	//OW_Environment::logCustInfo(msg);
	initSig();
	setupSigHandler(dbgFlg);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_Platform::daemonShutdown(const OW_String& daemonName)
{
	OW_String pidFile(OW_PIDFILE_DIR);
	pidFile += "/";
	pidFile += daemonName;
	pidFile += ".pid";
	OW_PidFile::removePid(pidFile.c_str());
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
static OW_Platform::Options
processCommandLineOptions(int argc, char** argv)
{
	OW_Platform::Options rval;

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
static void
handleSignal(int sig)
{
	struct sigaction temp;
	memset(&temp, '\0', sizeof(temp));
	sigaction(sig, 0, &temp);
	if(temp.sa_handler != SIG_IGN)
	{
		temp.sa_handler = theSigHandler;
		sigemptyset(&temp.sa_mask);
		temp.sa_flags = 0;
		sigaction(sig, &temp, NULL);
	}
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
				OW_Platform::pushSig(OW_Platform::SHUTDOWN);
				break;
			case SIGHUP:
				OW_Platform::pushSig(OW_Platform::REINIT);
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
setupSigHandler(const OW_Bool& dbgFlg)
{
	//OW_String dbgFlg = OW_Environment::getConfigItem(OW_ConfigOpts::OW_DEBUG_opt);

	if(dbgFlg)
	{
		handleSignal(SIGINT);
	}
	else
	{
		signal(SIGINT, SIG_IGN);
	}


	handleSignal(SIGTERM);
	handleSignal(SIGHUP);

//	handleSignal(SIGUSR2);
//	handleSignal(SIGUSR1);

	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
#ifdef OW_OPENSERVER
    signal(SIGPOLL, SIG_IGN);
#else
	signal(SIGIO, SIG_IGN);
#endif
	signal(SIGPIPE, SIG_IGN);

	// ?
	signal(SIGIOT, SIG_IGN);
	signal(SIGBUS, SIG_IGN);
	signal(SIGCONT, SIG_IGN);
	signal(SIGURG, SIG_IGN);
	signal(SIGXCPU, SIG_IGN);
	signal(SIGXFSZ, SIG_IGN);
	signal(SIGVTALRM, SIG_IGN);
	signal(SIGPROF, SIG_IGN);
	signal(SIGPWR, SIG_IGN);

	// ?
	//handleSignal(SIGALRM);
	//handleSignal(SIGABRT);
	//handleSignal(SIGILL);
	//handleSignal(SIGSEGV);
	//handleSignal(SIGFPE);
	//handleSignal(SIGSTKFLT);
}


