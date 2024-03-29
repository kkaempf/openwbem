/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ProviderAgent.hpp"
#include "blocxx/String.hpp"
#include "blocxx/Array.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include "OW_XMLExecute.hpp"
#include "OW_CIMClass.hpp"
#include "blocxx/SharedLibraryLoader.hpp"
#include "blocxx/SharedLibrary.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/CmdLineParser.hpp"
#include "OW_CppProviderIFC.hpp"
#include "blocxx/Logger.hpp"
#include "blocxx/LogAppender.hpp"
#include "blocxx/MultiAppender.hpp"
#include "OW_ConfigFile.hpp"

#include <csignal>
#include <iostream> // for cout and cerr

#include <unistd.h> // for getpid()


using namespace OpenWBEM;
using namespace WBEMFlags;
using namespace std;
using namespace blocxx;

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

LoggerConfigMap getAppenderConfig(const ConfigFile::ConfigMap& configItems)
{
	LoggerConfigMap appenderConfig;
	for (ConfigFile::ConfigMap::const_iterator iter = configItems.begin(); iter != configItems.end(); ++iter)
	{
		if (iter->first.startsWith("log") && iter->second.size() > 0)
		{
			appenderConfig[iter->first] = iter->second.back().value;
		}
	}
	return appenderConfig;
}

} // end anonymous namespace


int main(int argc, char* argv[])
{
	try
	{
		CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);

		sigPipe = UnnamedPipe::createUnnamedPipe();
		sigPipe->setWriteBlocking(UnnamedPipe::E_NONBLOCKING);
		sigPipe->setWriteTimeout(Timeout::relative(0.0));
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
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_SERVER_HTTP_PORT_opt, String(-1), ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_SERVER_HTTPS_PORT_opt, String(-1), ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_SERVER_MAX_CONNECTIONS_opt, String(10), ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_SERVER_ENABLE_DEFLATE_opt, "true", ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_SERVER_USE_DIGEST_opt, "false", ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_SERVER_USE_UDS_opt, "true", ConfigFile::E_PRESERVE_PREVIOUS);
		ConfigFile::setConfigItem(cmap, ConfigOpts::HTTP_SERVER_UDS_FILENAME_opt, String("/tmp/owprovideragent-") + String(UInt32(::getpid())), ConfigFile::E_PRESERVE_PREVIOUS);

		String url = parser.getOptionValue(URL_OPT);
		if (!url.empty())
		{
			ConfigFile::setConfigItem(cmap, ProviderAgent::DynamicClassRetrieval_opt, "true", ConfigFile::E_PRESERVE_PREVIOUS);
		}

		using namespace ConfigOpts;
		Array<LogAppenderRef> appenders;

		StringArray additionalLogs = ConfigFile::getMultiConfigItem(cmap, ADDITIONAL_LOGS_opt, StringArray(), " \t");
		additionalLogs.push_back("main");

		for (size_t i = 0; i < additionalLogs.size(); ++i)
		{
			const String& logName(additionalLogs[i]);

			String logMainType = ConfigFile::getConfigItem(cmap, Format(LOG_1_TYPE_opt, logName), OW_DEFAULT_LOG_1_TYPE);
			String logMainComponents = ConfigFile::getConfigItem(cmap, Format(LOG_1_COMPONENTS_opt, logName), OW_DEFAULT_LOG_1_COMPONENTS);
			String logMainCategories = ConfigFile::getConfigItem(cmap, Format(LOG_1_CATEGORIES_opt, logName));
			if (logMainCategories.empty())
			{
				// convert level into categories
				String logMainLevel = ConfigFile::getConfigItem(cmap, Format(LOG_1_LEVEL_opt, logName), OW_DEFAULT_LOG_1_LEVEL);
				if (logMainLevel.equalsIgnoreCase(Logger::STR_DEBUG_CATEGORY))
				{
					logMainCategories = String(Logger::STR_DEBUG_CATEGORY)
						+ " " + String(Logger::STR_INFO_CATEGORY)
						+ " " + String(Logger::STR_ERROR_CATEGORY)
						+ " " + String(Logger::STR_FATAL_CATEGORY);
				}
				else if (logMainLevel.equalsIgnoreCase(Logger::STR_INFO_CATEGORY))
				{
					logMainCategories = String(Logger::STR_INFO_CATEGORY)
						+ " " + String(Logger::STR_ERROR_CATEGORY)
						+ " " + String(Logger::STR_FATAL_CATEGORY);
				}
				else if (logMainLevel.equalsIgnoreCase(Logger::STR_ERROR_CATEGORY))
				{
					logMainCategories = String(Logger::STR_ERROR_CATEGORY) + " " + String(Logger::STR_FATAL_CATEGORY);
				}
				else if (logMainLevel.equalsIgnoreCase(Logger::STR_FATAL_CATEGORY))
				{
					logMainCategories = String(Logger::STR_FATAL_CATEGORY);
				}
			}
			String logMainFormat = ConfigFile::getConfigItem(cmap, Format(LOG_1_FORMAT_opt, logName), OW_DEFAULT_LOG_1_FORMAT);

			appenders.push_back(LogAppender::createLogAppender(logName, logMainComponents.tokenize(), logMainCategories.tokenize(),
				logMainFormat, logMainType, getAppenderConfig(cmap)));
		}

		LogAppender::setDefaultLogAppender(new MultiAppender(appenders));

		/// @todo  set the http server timeout

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
			CppProviderBaseIFCRef provider = CppProviderIFC::loadProvider(libName);
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

		ProviderAgent pa(cmap, pra, cra, rha, authenticator, url);

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

