/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "OW_OOPCpp1PR.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_String.hpp"

#include <iostream>

using namespace std;
using namespace OpenWBEM;

namespace
{

enum
{
	E_HELP_OPT,
	E_VERSION_OPT,
	E_LIBRARY_VERSION_OPT,
	E_PROVIDER_OPT,
	E_LOG_FILE_OPT,
	E_MONITOR_OPT,
	E_LOG_LEVEL_OPT
};

const CmdLineParser::Option g_options[] =
{
	{E_HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options and exit"},
	{E_VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information and exit"},
	{E_LIBRARY_VERSION_OPT, 'l', "libversion", CmdLineParser::E_NO_ARG, 0, "Show the required OpenWBEM library version and exit"},
	{E_PROVIDER_OPT, 'p', "provider", CmdLineParser::E_REQUIRED_ARG, 0, "Load and call <arg>"},
	{E_LOG_FILE_OPT, 0, "logfile", CmdLineParser::E_REQUIRED_ARG, 0, "Debug log file"},
	{E_MONITOR_OPT, 'm', "monitor", CmdLineParser::E_NO_ARG, 0, "Connect to monitor before loading provider library"},
	{E_LOG_LEVEL_OPT, 0, "loglevel", CmdLineParser::E_REQUIRED_ARG, 0,
	 "Lowest category to log"},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

int processCommandLine(
	int argc, char* argv[], String& provider, String& logfile, String& loglevel
);
void printUsage();

}

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int rval = 0;

	String providerLib;
	String logfile;
	String loglevel;
	int pclrv = processCommandLine(argc, argv, providerLib, logfile, loglevel);
	if (pclrv == -1)
	{
		return 0;
	}
	else if (pclrv != 0)
	{
		return pclrv;
	}
	if (loglevel.empty())
	{
		loglevel = "*";
	}
	return OOPCpp1::runOOPProvider(providerLib, logfile, loglevel);

}

namespace
{

int
processCommandLine(
	int argc, char* argv[], String& providerLib, String& logfile,
	String & loglevel)
{
	try
	{
		CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);

		if (parser.isSet(E_HELP_OPT))
		{
			printUsage();
			return -1;
		}
		else if (parser.isSet(E_VERSION_OPT))
		{
			std::cout << "owoopcpp1pr from " OW_PACKAGE_STRING << std::endl;
			return -1;
		}
		else if (parser.isSet(E_LIBRARY_VERSION_OPT))
		{
			std::cout << OW_OPENWBEM_LIBRARY_VERSION << std::endl;
			return -1;
		}

		providerLib = parser.mustGetOptionValue(E_PROVIDER_OPT, "-p, --provider");
		
		if (parser.isSet(E_LOG_FILE_OPT))
		{
			logfile = parser.getOptionValue(E_LOG_FILE_OPT);
		}
		if (parser.isSet(E_MONITOR_OPT))
		{
			PrivilegeManager::connectToMonitor();
		}
		if (parser.isSet(E_LOG_LEVEL_OPT))
		{
			loglevel = parser.getOptionValue(E_LOG_LEVEL_OPT);
		}
	}
	catch (const CmdLineParserException& e)
	{
		std::cerr << "Command line parser error." << std::endl;
		switch (e.getErrorCode())
		{
			case CmdLineParser::E_INVALID_OPTION:
				cerr << "unknown option: " << e.getMessage() << '\n';
			break;
			case CmdLineParser::E_MISSING_ARGUMENT:
				cerr << "missing argument for option: " << e.getMessage() << '\n';
			break;
			case CmdLineParser::E_INVALID_NON_OPTION_ARG:
				cerr << "invalid non-option argument: " << e.getMessage() << '\n';
			break;
			case CmdLineParser::E_MISSING_OPTION:
				cerr << "missing required option: " << e.getMessage() << '\n';
			break;
			default:
				cerr << "failed parsing command line options: " << e << "\n";
			break;
		}
		printUsage();
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
printUsage()
{
	cerr << "owoopcpp1pr [OPTIONS]..." << std::endl;
	cerr << CmdLineParser::getUsage(g_options) << std::endl;
}

} // end unnamed namespace

