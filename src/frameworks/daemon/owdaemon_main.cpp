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
#include "OW_DaemonEnv.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ThreadEvent.hpp"
#include "OW_Platform.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include <exception>

using std::ostream;
using std::endl;
using std::cout;

static bool processCommandLine(int argc, char* argv[]);
static void printUsage(ostream& ostrm);

static OW_DaemonEnv env;

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{

	try
	{
		bool debugMode = processCommandLine(argc, argv);

		// Set up environment and start up all server components.
		env.init();

		// Call platform specific code to become a daemon/service
		try
		{
			OW_Platform::daemonize(debugMode, OW_DAEMON_NAME);
		}
		catch (const OW_DaemonException& e)
		{
			env.logError(e.getMessage());
			env.logError("Failed to initialize - aborting...");
			return 1;
		}

		env.startServices();

		int sig;
		OW_Bool shuttingDown(false);
		while(!shuttingDown)
		{
			// runSelectEngine will only return once something has been put into
			// the signal pipe.
			env.runSelectEngine();
			sig = OW_Platform::popSig();
			switch (sig)
			{
				case OW_Platform::SHUTDOWN:
					shuttingDown = true;
					env.logCustInfo("Open WBEM cimom received shutdown notification");
					env.shutdown();
					break;
				case OW_Platform::REINIT:
					env.logCustInfo("Open WBEM cimom received restart notification");
					env.shutdown();
					processCommandLine(argc, argv);
					env.init();
					env.startServices();
					break;
				default:
					break;
			}
		}

		// Call platform specific shutdown routine
		OW_Platform::daemonShutdown(OW_DAEMON_NAME);
	}
	catch (OW_Assertion& e)
	{
		env.logError("**************************************************");
		env.logError("* ASSERTION CAUGHT IN MAIN!");
		env.logError(format("* Condition: %1", e.getMessage()));
		env.logError(format("* File: %1", e.getFile()));
		env.logError(format("* Line: %1", e.getLine()));
		env.logError("**************************************************");
	}
	catch (OW_Exception& e)
	{
		env.logError("**************************************************");
		env.logError("* EXCEPTION CAUGHT IN MAIN!");
		env.logError(format("* Type: %1", e.type()));
		env.logError(format("* Msg: %1", e.getMessage()));
		env.logError(format("* File: %1", e.getFile()));
		env.logError(format("* Line: %1", e.getLine()));
		env.logError(format("* Stack Trace: %1", e.getStackTrace()));
		env.logError("**************************************************");
	}
	catch (std::exception& se)
	{
		env.logError("**************************************************");
		env.logError("* Standard EXCEPTION CAUGHT IN MAIN!");
		env.logError(format("* Type: %1", se.what()));
		env.logError("**************************************************");
	}
	catch(...)
	{
		env.logError("**************************************************");
		env.logError("* UNKNOWN EXCEPTION CAUGHT MAIN!");
		env.logError("**************************************************");
	}

	return 0;
}

bool
processCommandLine(int argc, char* argv[])
{
	bool debug = false;
	// Process command line options
	OW_Platform::Options opts = OW_Platform::daemonInit(argc, argv);

	// If the user only specified the help option on the command
	// line then get out
	if(opts.help)
	{
		if (opts.error)
		{
			cout << "Unknown command line argument." << endl;
		}
		printUsage(cout);
		exit(0);
	}

	if (opts.debug)
	{
		env.setConfigItem(OW_ConfigOpts::OW_DEBUG_opt, "true",
			true);
		env.setConfigItem(OW_ConfigOpts::LOG_LEVEL_opt,
			"debug");
		debug = true;
	}

	if (opts.configFile)
	{
		env.setConfigItem(OW_ConfigOpts::CONFIG_FILE_opt,
			opts.configFilePath, true);
	}
	return debug;
}


//////////////////////////////////////////////////////////////////////////////
static void
printUsage(ostream& ostrm)
{
	ostrm << OW_DAEMON_NAME << " [OPTIONS]..." << endl;
	ostrm << "Available options:" << endl;
	ostrm << "\t-d, --debug  Set debug on (does not detach from terminal"<< endl;
	ostrm << "\t-c, --config Specifiy an alternate config file" << endl;
	ostrm << "\t-h, --help   Print this help information" << endl;
}


