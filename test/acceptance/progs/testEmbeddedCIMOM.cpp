/*******************************************************************************
* Copyright (C) 2001-2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
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
#include "OW_EmbeddedCIMOMEnvironment.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_Logger.hpp"
#include "OW_CerrLogger.hpp"
#include "OW_OperationContext.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"

#include <exception>
#include <iostream> // for cout
#include <new> // for new handler stuff

#include <getopt.h>

using namespace OpenWBEM;

namespace
{

bool processCommandLine(int argc, char* argv[],
	EmbeddedCIMOMEnvironmentRef env);
void printUsage(std::ostream& ostrm);

const String COMPONENT_NAME("ow.owcimomd");

}

struct Options
{
	Options()
	: debug(false)
	, configFile(false)
	, configFilePath()
	, help(false)
	, error(false)
	{}
	bool debug;
	bool configFile;
	String configFilePath;
	bool help;
	bool error;
};

void owcimomd_new_handler();
Options processCommandLineOptions(int argc, char** argv);

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int rval = 1;
	EmbeddedCIMOMEnvironmentRef env = EmbeddedCIMOMEnvironment::instance();
	
	// until the config file is read and parsed, just use a logger that prints everything to stderr.
	LoggerRef logger(new CerrLogger());

	try
	{
		bool debugMode = processCommandLine(argc, argv, env);
		// Initilize the cimom environment object
		env->init();

		// debug mode can be activated by -d or by the config file, so check both. The config file is loaded by env->init().
		debugMode = debugMode || env->getConfigItem(ConfigOpts::DEBUGFLAG_opt, OW_DEFAULT_DEBUGFLAG).equalsIgnoreCase("true");

		// logger's not set up according to the config file until after init()
		logger = env->getLogger(COMPONENT_NAME);
		OW_LOG_INFO(logger, "owcimomd (" OW_VERSION ") beginning startup");

		env->setConfigItem(ConfigOpts::AUTHORIZATION_LIB_opt, "");
		env->setConfigItem(ConfigOpts::AUTHORIZATION2_LIB_opt, "");

		env->startServices();
		OW_LOG_INFO(logger, "owcimomd is now running!");

		OperationContext oc; 

		CIMOMHandleIFCRef ch = env->getCIMOMHandle(oc); 

		std::cout << "Running!" << std::endl;
		CIMClass cc = ch->getClass("root/testsuite", "TestInstance"); 
		CIMInstance inst = cc.newInstance(); 
		inst.updatePropertyValue("Name", CIMValue(String("one"))); 
		StringArray props; 
		props.push_back(String("one.one")); 
		props.push_back(String("one.two")); 
		inst.updatePropertyValue("Params", CIMValue(props)); 
		ch->createInstance("root/testsuite", inst); 

		inst.updatePropertyValue("Name", CIMValue(String("two"))); 
		props.clear(); 
		props.push_back(String("two.one")); 
		props.push_back(String("two.two")); 
		inst.updatePropertyValue("Params", CIMValue(props)); 
		ch->createInstance("root/testsuite", inst); 
		
		CIMInstanceArray cia = ch->enumInstancesA("root/testsuite", "TestInstance"); 

		OW_ASSERT(cia.size() == 2); 
		OW_ASSERT(cia[0].getPropertyValue("Name").toString() == "one"); 
		OW_ASSERT(cia[1].getPropertyValue("Name").toString() == "two"); 
		props.clear(); 
		cia[0].getPropertyValue("Params").get(props); 
		OW_ASSERT(props.size() == 2); 
		OW_ASSERT(props[0] == "one.one"); 
		OW_ASSERT(props[1] == "one.two"); 
		props.clear(); 
		cia[1].getPropertyValue("Params").get(props); 
		OW_ASSERT(props.size() == 2); 
		OW_ASSERT(props[0] == "two.one"); 
		OW_ASSERT(props[1] == "two.two"); 
		

		for (CIMInstanceArray::const_iterator iter = cia.begin(); 
			  iter != cia.end(); ++iter)
		{
			std::cout << iter->toMOF() << std::endl;
		}
		
		StringArray classnames = ch->enumClassNamesA("root/testsuite", ""); 
		for (StringArray::const_iterator iter = classnames.begin(); 
			  iter != classnames.end(); ++iter)
		{
			std::cout << *iter << std::endl;
		}

		rval = 0; 

	}
	catch (AssertionException& e)
	{
		OW_LOG_FATAL_ERROR(logger, "* ASSERTION EXCEPTION CAUGHT IN owcimomd MAIN!");
		OW_LOG_FATAL_ERROR(logger, Format("* %1", e));
		rval = 1;
	}
	catch (Exception& e)
	{
		OW_LOG_FATAL_ERROR(logger, "* EXCEPTION CAUGHT IN owcimomd MAIN!");
		OW_LOG_FATAL_ERROR(logger, Format("* %1", e));
		rval = 1;
	}
	catch (std::exception& se)
	{
		OW_LOG_FATAL_ERROR(logger, "* std::exception CAUGHT IN owcimomd MAIN!");
		OW_LOG_FATAL_ERROR(logger, Format("* Message: %1", se.what()));
		rval = 1;
	}
	catch(...)
	{
		OW_LOG_FATAL_ERROR(logger, "* UNKNOWN EXCEPTION CAUGHT owcimomd MAIN!");
		rval = 1;
	}
	// Call platform specific shutdown routine

	EmbeddedCIMOMEnvironment::instance() = env = 0;
	
	OW_LOG_INFO(logger, "owcimomd has shutdown");
	return rval;
}

namespace
{

//////////////////////////////////////////////////////////////////////////////
bool
processCommandLine(int argc, char* argv[], EmbeddedCIMOMEnvironmentRef env)
{
	// Process command line options
	Options opts = processCommandLineOptions(argc, argv);
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
		env->setConfigItem(ConfigOpts::DEBUGFLAG_opt, "true", ServiceEnvironmentIFC::E_PRESERVE_PREVIOUS);
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
void owcimomd_new_handler()
{
#if defined (OW_DEBUG)  || defined (OW_NETWARE)
	abort();
#endif

	throw std::bad_alloc();
}

#ifdef OW_HAVE_GETOPT_LONG
//////////////////////////////////////////////////////////////////////////////
struct option   long_options[] =
{
	{ "debug", 0, NULL, 'd' },
	{ "config", required_argument, NULL, 'c' },
	{ "help", 0, NULL, 'h' },
	{ 0, 0, 0, 0 }
};
#endif
const char* const short_options = "dc:h";
Options
processCommandLineOptions(int argc, char** argv)
{
	Options rval;
#ifndef WIN32
#ifdef OW_HAVE_GETOPT_LONG
	int optndx = 0;
	optind = 1;
	int c = getopt_long(argc, argv, short_options, long_options, &optndx);
#else
	optind = 1;
	int c = getopt(argc, argv, short_options);
#endif
	while (c != -1)
	{
		switch (c)
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
#endif
	return rval;
}

