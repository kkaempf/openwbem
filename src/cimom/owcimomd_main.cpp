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
#include "OW_CIMOMEnvironment.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Platform.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include <exception>
#include <iostream> // for cout

#include <unistd.h> // for execv

using namespace OpenWBEM;

static bool processCommandLine(int argc, char* argv[],
	CIMOMEnvironmentRef env);
static void printUsage(std::ostream& ostrm);
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	CIMOMEnvironmentRef env = CIMOMEnvironment::g_cimomEnvironment = new CIMOMEnvironment;
	try
	{
		bool debugMode = processCommandLine(argc, argv, env);
		// Initilize the cimom environment object
		env->init();
		// Call platform specific code to become a daemon/service
		try
		{
			Platform::daemonize(debugMode, OW_DAEMON_NAME);
		}
		catch (const DaemonException& e)
		{
			env->logFatalError(e.getMessage());
			env->logFatalError("CIMOM failed to initialize. Aborting...");
			return 1;
		}
		// Start all of the cimom services
		env->startServices();
		env->logInfo("CIMOM is now running!");

		// only do this in production mode. During development we want it to crash!
#ifdef OW_DEBUG
		// Do this after initialization to prevent an infinite loop.
		if (env->getConfigItem(ConfigOpts::RESTART_ON_ERROR_opt, OW_DEFAULT_RESTART_ON_ERROR).equalsIgnoreCase("true"))
		{
			Platform::installFatalSignalHandlers();
		}
#endif

		int sig;
		bool shuttingDown(false);
		while(!shuttingDown)
		{
			// runSelectEngine will only return once something has been put into
			// the signal pipe or an error has happened
			env->runSelectEngine();
			sig = Platform::popSig();
			switch (sig)
			{
				case Platform::SHUTDOWN:
					shuttingDown = true;

					// need to remove them so we don't restart while shutting down.
					Platform::removeFatalSignalHandlers(); 

					env->logInfo("CIMOM received shutdown notification."
						" Initiating shutdown");
					env->shutdown();
					break;
				case Platform::REINIT:
					env->logInfo("CIMOM received restart notification."
						" Initiating restart");
					env->shutdown();
					env->clearConfigItems();
					env = CIMOMEnvironment::g_cimomEnvironment = 0;
					// don't try to catch the DeamonException, because if it's thrown, stuff is so whacked, we should just exit!
					Platform::restartDaemon();
					break;
				default:
					break;
			}
		}
		// Call platform specific shutdown routine
		Platform::daemonShutdown(OW_DAEMON_NAME);
	}
	catch (Assertion& e)
	{
		env->logFatalError("**************************************************");
		env->logFatalError("* OW_ASSERTION CAUGHT IN CIMOM MAIN!");
		env->logFatalError(format("* Condition: %1", e.getMessage()));
		env->logFatalError(format("* File: %1", e.getFile()));
		env->logFatalError(format("* Line: %1", e.getLine()));
		env->logFatalError("**************************************************");
	}
	catch (Exception& e)
	{
		env->logFatalError("**************************************************");
		env->logFatalError("* EXCEPTION CAUGHT IN CIMOM MAIN!");
		env->logFatalError(format("* Type: %1", e.type()));
		env->logFatalError(format("* Msg: %1", e.getMessage()));
		env->logFatalError(format("* File: %1", e.getFile()));
		env->logFatalError(format("* Line: %1", e.getLine()));
		env->logFatalError("**************************************************");
	}
	catch (std::exception& se)
	{
		env->logFatalError("**************************************************");
		env->logFatalError("* Standard EXCEPTION CAUGHT IN CIMOM MAIN!");
		env->logFatalError(format("* Type: %1", se.what()));
		env->logFatalError("**************************************************");
	}
	catch(...)
	{
		env->logFatalError("**************************************************");
		env->logFatalError("* UNKNOWN EXCEPTION CAUGHT CIMOM MAIN!");
		env->logFatalError("**************************************************");
	}
	CIMOMEnvironment::g_cimomEnvironment = 0;
	env->logInfo("CIMOM has shutdown");
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
static bool
processCommandLine(int argc, char* argv[], CIMOMEnvironmentRef env)
{
	// Process command line options
	Platform::Options opts = Platform::daemonInit(argc, argv);
	// If the user only specified the help option on the command
	// line then get out
	if(opts.help)
	{
		if (opts.error)
		{
			std::cerr << "Unknown command line argument for CIMOM" << std::endl;
		}
		printUsage(std::cout);
		exit(0);
	}
	if(opts.debug)
	{
		env->setConfigItem(ConfigOpts::DEBUG_opt, "true", ServiceEnvironmentIFC::E_PRESERVE_PREVIOUS);
		env->setConfigItem(ConfigOpts::LOG_LEVEL_opt, "debug");
	}
	if (opts.configFile)
	{
		env->setConfigItem(ConfigOpts::CONFIG_FILE_opt, opts.configFilePath);
	}
	return  opts.debug;
}
//////////////////////////////////////////////////////////////////////////////
static void
printUsage(std::ostream& ostrm)
{
	ostrm << OW_DAEMON_NAME << " [OPTIONS]..." << std::endl;
	ostrm << "Available options:" << std::endl;
	ostrm << "\t-d, --debug  Set debug on (does not detach from terminal"<< std::endl;
	ostrm << "\t-c, --config Specifiy an alternate config file" << std::endl;
	ostrm << "\t-h, --help   Print this help information" << std::endl;
}

