/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ProviderAgent.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_XMLExecute.hpp"
#include "OW_CIMClass.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_Format.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_CppProviderIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_LogAppender.hpp"
#include "OW_AppenderLogger.hpp"

#include <csignal>
#include <iostream> // for cout and cerr

#include <unistd.h> // for getpid()


using namespace OpenWBEM;
using namespace WBEMFlags;
using namespace std;

UnnamedPipeRef sigPipe;
extern "C"
void sig_handler(int)
{
	sigPipe->writeInt(0);
}

namespace
{

enum
{
	HELP_OPT,
	VERSION_OPT,
	URL_OPT,
	CONFIG_OPT,
	HTTP_PORT_OPT,
	HTTPS_PORT_OPT,
	UDS_FILENAME_OPT,
	PROVIDER_OPT
};

CmdLineParser::Option g_options[] =
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{CONFIG_OPT, 'c', "config", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the config file.  Command line options take precedence over config file options."},
	{URL_OPT, 'u', "url", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the cimom callback url."},
	{HTTP_PORT_OPT, '\0', "http-port", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the http port."},
	{HTTPS_PORT_OPT, '\0', "https-port", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the https port."},
	{UDS_FILENAME_OPT, '\0', "uds-filename", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the filename of the unix domain socket."},
	{PROVIDER_OPT, 'p', "provider", CmdLineParser::E_REQUIRED_ARG, 0, "Specify a filename of a provider library to use. May be used multiple times."},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void Usage()
{
	cerr << "Usage: owprovideragent [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << endl;
}

LoggerRef
createLogger(const String& type_)
{
	String type(type_);
	StringArray components;
	components.push_back("*");

	StringArray categories;
	categories.push_back(Logger::STR_FATAL_CATEGORY);
	categories.push_back(Logger::STR_ERROR_CATEGORY);

	ConfigFile::ConfigMap configItems;

	// TODO: Fix this to use the new logging configuration scheme
	String filename = type;
	if (type != "syslog")
	{
		type = "file";
		configItems["log.test.location"] = filename;
	}

	LogAppenderRef logAppender = LogAppender::createLogAppender("", components, categories,
		LogMessagePatternFormatter::STR_DEFAULT_MESSAGE_PATTERN, type, configItems);

	return LoggerRef(new AppenderLogger("owcimomd", E_ERROR_LEVEL, logAppender));

}

} // end anonymous namespace


int main(int argc, char* argv[])
{
	try
	{
		CmdLineParser parser(argc, argv, g_options);

		sigPipe = UnnamedPipe::createUnnamedPipe();
		sigPipe->setOutputBlocking(UnnamedPipe::E_NONBLOCKING);
		sigPipe->setWriteTimeout(0);
		signal(SIGINT, sig_handler);

		if (parser.isSet(HELP_OPT))
		{
			Usage();
			return 0;
		}
		else if (parser.isSet(VERSION_OPT))
		{
			cout << "owprovideragent (OpenWBEM) " << OW_VERSION << '\n';
			cout << "Written by Bart Whiteley and Dan Nuffer.\n";
			return 0;
		}

		ConfigFile::ConfigMap cmap;

		String configFile = parser.getOptionValue(CONFIG_OPT);
		if (!configFile.empty())
		{
			ConfigFile::loadConfigFile(configFile, cmap);
		}

		// set up some defaults if not in the config file
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_PORT_opt, String(-1), ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTPS_PORT_opt, String(-1), ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::MAX_CONNECTIONS_opt, String(10), ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::ENABLE_DEFLATE_opt, "true", ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_USE_DIGEST_opt, "false", ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::USE_UDS_opt, "true", ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::UDS_FILENAME_opt, String("/tmp/owprovideragent-") + String(UInt32(::getpid())), ConfigFile::E_PRESERVE_PREVIOUS);

		String url = parser.getOptionValue(URL_OPT);
		if (!url.empty())
		{
			ConfigFile::setConfigItem(cmap, ProviderAgent::DynamicClassRetrieval_opt, "true", ConfigFile::E_PRESERVE_PREVIOUS);
		}

		bool debugMode = false;

		LoggerRef logger = createLogger(ConfigFile::getConfigItem(cmap, ConfigOpts::LOG_LOCATION_opt, OW_DEFAULT_LOG_LOCATION));
		logger->setLogLevel(ConfigFile::getConfigItem(cmap, ConfigOpts::LOG_LEVEL_opt, OW_DEFAULT_LOG_LEVEL));

		// TODO: set the http server timeout

		AuthenticatorIFCRef authenticator;
		RequestHandlerIFCRef rh(SharedLibraryRef(0), new XMLExecute);
		Array<RequestHandlerIFCRef> rha;
		rha.push_back(rh);

		Array<CppProviderBaseIFCRef> pra;
		StringArray providers = parser.getOptionValueList(PROVIDER_OPT);
		if (providers.empty())
		{
			cerr << "Error: no providers specified\n";
			Usage();
			return 1;
		}
		for (size_t i = 0; i < providers.size(); ++i)
		{
			String libName(providers[i]);
			CppProviderBaseIFCRef provider = CppProviderIFC::loadProvider(libName, logger);
			if (!provider->getInstanceProvider()
				&& !provider->getSecondaryInstanceProvider()
	#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
				&& !provider->getAssociatorProvider()
	#endif
				&& !provider->getMethodProvider())
			{
				cerr << "Error: Provider " << libName << " is not a supported type" << endl;
				return 1;
			}
			pra.push_back(provider);
		}

		CIMClassArray cra;

		ProviderAgent pa(cmap, pra, cra, rha, authenticator, logger, url);
		
		// wait until we get a SIGINT as a shutdown signal
		int dummy;
		sigPipe->readInt(&dummy);

		cout << "Shutting down." << endl;
		pa.shutdownHttpServer();
		return 0;
	}
	catch (CmdLineParserException& e)
	{
		switch (e.getErrorCode())
		{
			case CmdLineParser::E_INVALID_OPTION:
				cerr << "unknown option: " << e.getMessage() << '\n';
			break;
			case CmdLineParser::E_MISSING_ARGUMENT:
				cerr << "missing argument for option: " << e.getMessage() << '\n';
			break;
			default:
				cerr << "failed parsing command line options\n";
			break;
		}
		Usage();
	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
	}
	catch (...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}

