/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 * @author Bart Whiteley
 */

#include "OW_config.h"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Platform.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_Logger.hpp"
#include "OW_CerrLogger.hpp"

#include <exception>
#include <iostream> // for cout
#include <new> // for new handler stuff

using namespace OpenWBEM;

namespace
{

bool processCommandLine(int argc, char* argv[],
	CIMOMEnvironmentRef env);
void printUsage(std::ostream& ostrm);

const String COMPONENT_NAME("ow.owcimomd");

}

OW_EXPORT
void owcimomd_new_handler();

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int rval = 0;
	CIMOMEnvironmentRef env = CIMOMEnvironment::instance();
	
	// until the config file is read and parsed, just use a logger that prints everything to stderr.
	LoggerRef logger(new CerrLogger());

	try
	{
		bool debugMode = processCommandLine(argc, argv, env);
		// Initilize the cimom environment object
		env->init();

		// debug mode can be activated by -d or by the config file, so check both. The config file is loaded by env->init().
		debugMode = debugMode || env->getConfigItem(ConfigOpts::DEBUG_opt, OW_DEFAULT_DEBUG).equalsIgnoreCase("true");

		// logger's not set up according to the config file until after init()
		logger = env->getLogger(COMPONENT_NAME);
		OW_LOG_INFO(logger, "owcimomd (" OW_VERSION ") beginning startup");

		// Call platform specific code to become a daemon/service
		try
		{
			Platform::daemonize(debugMode, OW_DAEMON_NAME);
		}
		catch (const DaemonException& e)
		{
			OW_LOG_FATAL_ERROR(logger, e.getMessage());
			OW_LOG_FATAL_ERROR(logger, "owcimomd failed to initialize. Aborting...");
			return 1;
		}
		// Start all of the cimom services
		env->startServices();
		OW_LOG_INFO(logger, "owcimomd is now running!");

		// Do this after initialization to prevent an infinite loop.
		std::unexpected_handler oldUnexpectedHandler = 0;
		std::terminate_handler oldTerminateHandler = 0;
		std::new_handler oldNewHandler = std::set_new_handler(owcimomd_new_handler);

		if (env->getConfigItem(ConfigOpts::RESTART_ON_ERROR_opt, OW_DEFAULT_RESTART_ON_ERROR).equalsIgnoreCase("true"))
		{
			const char* const restartDisabledMessage =
				"WARNING: even though the owcimomd.restart_on_error config option = true, it\n"
				"is not enabled. Possible reasons are that OpenWBEM is built in debug mode,\n"
				"owcimomd is running in debug mode (-d), or owcimomd was not run using an\n"
				"absolute path (argv[0][0] != '/')";

			// only do this in production mode. During development we want it to crash!
#if !defined(OW_DEBUG)
			if ((debugMode == false) && argv[0][0] == '/') // if argv[0][0] != '/' the restart will not be predictable
			{
				Platform::installFatalSignalHandlers();
				oldUnexpectedHandler = std::set_unexpected(Platform::rerunDaemon);
				oldTerminateHandler = std::set_terminate(Platform::rerunDaemon);
			}
			else
			{
				OW_LOG_INFO(logger, restartDisabledMessage);
			}
#else
			OW_LOG_INFO(logger, restartDisabledMessage);
#endif
		}

		int sig;
		bool shuttingDown(false);

		Platform::sendDaemonizeStatus(Platform::DAEMONIZE_SUCCESS);
		while (!shuttingDown)
		{
			// runSelectEngine will only return once something has been put into
			// the signal pipe or an error has happened
			env->runSelectEngine();
			sig = Platform::popSig();
			switch (sig)
			{
				case Platform::SHUTDOWN:
					shuttingDown = true;

#if !defined(OW_DEBUG)
					// need to remove them so we don't restart while shutting down.
					Platform::removeFatalSignalHandlers();
					if (oldUnexpectedHandler)
					{
						std::set_unexpected(oldUnexpectedHandler);
					}
					if (oldTerminateHandler)
					{
						std::set_terminate(oldTerminateHandler);
					}
#endif

					OW_LOG_INFO(logger, "owcimomd received shutdown notification."
						" Initiating shutdown");
					env->shutdown();
					break;
				case Platform::REINIT:
					OW_LOG_INFO(logger, "owcimomd received restart notification."
						" Initiating restart");
					env->shutdown();
					env->clearConfigItems();
					env = CIMOMEnvironment::instance() = 0;
					// don't try to catch the DeamonException, because if it's thrown, stuff is so whacked, we should just exit!
					Platform::rerunDaemon();

					// typically on *nix, restartDaemon() doesn't return, however to account for environments where
					// it won't we'll leave this code here to re-initialize.
					env = CIMOMEnvironment::instance() = new CIMOMEnvironment;
					processCommandLine(argc, argv, env);
					env->init();
					env->startServices();
					break;
				default:
					break;
			}
		}
	}
	catch (Exception& e)
	{
		OW_LOG_FATAL_ERROR(logger, "* EXCEPTION CAUGHT IN owcimomd MAIN!");
		OW_LOG_FATAL_ERROR(logger, Format("* %1", e));
		Platform::sendDaemonizeStatus(Platform::DAEMONIZE_FAIL);
		rval = 1;
	}
	catch (std::exception& se)
	{
		OW_LOG_FATAL_ERROR(logger, "* std::exception CAUGHT IN owcimomd MAIN!");
		OW_LOG_FATAL_ERROR(logger, Format("* Message: %1", se.what()));
		Platform::sendDaemonizeStatus(Platform::DAEMONIZE_FAIL);
		rval = 1;
	}
	catch(...)
	{
		OW_LOG_FATAL_ERROR(logger, "* UNKNOWN EXCEPTION CAUGHT owcimomd MAIN!");
		Platform::sendDaemonizeStatus(Platform::DAEMONIZE_FAIL);
		rval = 1;
	}
	CIMOMEnvironment::instance() = 0;
	
	// Call platform specific shutdown routine
	Platform::daemonShutdown(OW_DAEMON_NAME);

	OW_LOG_INFO(logger, "owcimomd has shutdown");
	return rval;
}

namespace
{

//////////////////////////////////////////////////////////////////////////////
bool
processCommandLine(int argc, char* argv[], CIMOMEnvironmentRef env)
{
	// Process command line options
	Platform::Options opts = Platform::daemonInit(argc, argv);
	// If the user only specified the help option on the command
	// line then get out
	if (opts.help)
	{
		if (opts.error)
		{
			std::cerr << "Unknown command line argument for owcimomd" << std::endl;
		}
		printUsage(std::cout);
		exit(0);
	}
	if (opts.debug)
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
void
printUsage(std::ostream& ostrm)
{
	ostrm << OW_DAEMON_NAME << " [OPTIONS]..." << std::endl;
	ostrm << "Available options:" << std::endl;
	ostrm << "\t-d, --debug  Set debug on (does not detach from terminal"<< std::endl;
	ostrm << "\t-c, --config Specifiy an alternate config file" << std::endl;
	ostrm << "\t-h, --help   Print this help information" << std::endl;
}

} // end unnamed namespace
//////////////////////////////////////////////////////////////////////////////
OW_EXPORT
void owcimomd_new_handler()
{
#if defined (OW_DEBUG)  || defined (OW_NETWARE)
	abort();
#endif

	Platform::restartDaemon();
	throw std::bad_alloc();
}

