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
#include "OW_ConfigOpts.hpp"
#include "OW_Platform.hpp"
#include "OW_PlatformSignal.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_Logger.hpp"
#include "OW_CerrLogger.hpp"

// necessary for the environment class
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Socket.hpp"
#include "OW_CppProviderIFC.hpp"
#include "OW_SafeLibCreate.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include "OW_Exception.hpp"
#include "OW_FileSystem.hpp"
#include "OW_LogAppender.hpp"
#include "OW_AppenderLogger.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_ProviderAgent.hpp"
#include "OW_CIMClass.hpp"

#include <exception>
#include <iostream> // for cout
#include <new> // for new handler stuff

using namespace OpenWBEM;

namespace
{

bool processCommandLine(int argc, char* argv[], const ServiceEnvironmentIFCRef& env);
void printUsage(std::ostream& ostrm);

const String COMPONENT_NAME("ow.owprovideragent");
const String DAEMON_NAME("owprovideragent");

#define OW_PA_DEFAULT_CONFIG_FILE OW_DEFAULT_SYSCONF_DIR"/openwbem/owprovideragent.conf"

OW_DECLARE_EXCEPTION(PA);
OW_DEFINE_EXCEPTION(PA);

class MyLifecycleCallback : public ProviderAgentLifecycleCallbackIFC
{
public:
	MyLifecycleCallback(const LoggerRef& logger)
		: m_logger(logger)
	{
	}

	virtual void fatalError(const String& errorDescription)
	{
		OW_LOG_FATAL_ERROR(m_logger, errorDescription);
		Platform::restartDaemon();
	}
private:
	LoggerRef m_logger;
};

class MyServiceEnvironment : public ServiceEnvironmentIFC
{
public:
	MyServiceEnvironment()
		: m_logger(new CerrLogger)
	{

	}

	void init()
	{
		// The config file config item may be set before init() is called.
		String filename = getConfigItem(ConfigOpts::CONFIG_FILE_opt, OW_PA_DEFAULT_CONFIG_FILE);
		OW_LOG_DEBUG(m_logger, "\nUsing config file: " + filename);
		ConfigFile::loadConfigFile(filename, m_configItems);

		// logger is treated special, so it goes in init() not startServices()
		_createLogger();
	}

	void startServices()
	{
		// We start out single-threaded.  The start phase is when threads enter the picture.

		// This is a global thing, so handle it here.
		Socket::createShutDownMechanism();

		// load
		OW_LOG_DEBUG(m_logger, "loading services");

		_loadAuthenticator();
		_loadRequestHandlers();
		_loadProviders();

		OW_LOG_DEBUG(m_logger, "finished loading services");

		// start

		// TODO: get the url from a config item
		String url;

		m_providerAgent = new ProviderAgent(
			m_configItems,
			m_providers,
			CIMClassArray(),
			m_reqHandlers,
			m_authenticator,
			m_logger,
			url,
			ProviderAgentLockerIFCRef(),
			ProviderAgentLifecycleCallbackIFCRef(new MyLifecycleCallback(m_logger)));

		OW_LOG_DEBUG(m_logger, "finished starting services");
	}
	void shutdown()
	{
		// this is a global thing, so do it here
		Socket::shutdownAllSockets();

		// Shutdown provider agent
		m_providerAgent->shutdownHttpServer();

		// PHASE 2: unload/delete

		OW_LOG_DEBUG(m_logger, "unloading and deleting services");

		m_providerAgent = 0;
		m_providers.clear();
		m_reqHandlers.clear();
		m_authenticator.setNull();

		OW_LOG_DEBUG(m_logger, "owprovideragent has shut down");
	}

	void setConfigItem(const String &item, const String &value, EOverwritePreviousFlag overwritePrevious)
	{
		ConfigFile::setConfigItem(m_configItems, item, value, ConfigFile::EOverwritePreviousFlag(overwritePrevious));
	}

	String getConfigItem(const String &name, const String& defRetVal) const
	{
		return ConfigFile::getConfigItem(m_configItems, name, defRetVal);
	}

	LoggerRef getLogger() const
	{
		OW_ASSERT(m_logger);
		return m_logger->clone();
	}
	
	LoggerRef getLogger(const String& componentName) const
	{
		OW_ASSERT(m_logger);
		LoggerRef rv(m_logger->clone());
		rv->setDefaultComponent(componentName);
		return rv;
	}
private:
	void _loadAuthenticator()
	{
		String authLib = getConfigItem(ConfigOpts::AUTH_MOD_opt, ""); // don't use the default, since if this is empty, we don't want to use an authenticator.
		if (!authLib.empty())
		{
			OW_LOG_INFO(m_logger, Format("Authentication Manager: Loading authentication module %1", authLib));
			m_authenticator = SafeLibCreate<AuthenticatorIFC>::loadAndCreateObject(authLib, "createAuthenticator", m_logger);
			if (m_authenticator)
			{
				m_authenticator->init(this);
				OW_LOG_INFO(m_logger, Format("Authentication module %1 is now being used for authentication", authLib));
			}
			else
			{
				OW_LOG_FATAL_ERROR(m_logger, Format("Authentication Module %1 failed"
					" to produce authentication module"
					" [No Authentication Mechanism Available!]", authLib));
				OW_THROW(PAException, "No Authentication Mechanism Available");
			}
		}
	}

	void _loadRequestHandlers()
	{
		String libPath = getConfigItem(ConfigOpts::CIMOM_REQUEST_HANDLER_LOCATION_opt, OW_DEFAULT_CIMOM_REQHANDLER_LOCATION);
		if (!libPath.endsWith(OW_FILENAME_SEPARATOR))
		{
			libPath += OW_FILENAME_SEPARATOR;
		}
		OW_LOG_INFO(m_logger, Format("owprovideragent loading request handlers from directory %1", libPath));
		StringArray dirEntries;
		if (!FileSystem::getDirectoryContents(libPath, dirEntries))
		{
			OW_LOG_FATAL_ERROR(m_logger, Format("owprovideragent failed getting the contents of the request handler directory: %1", libPath));
			OW_THROW(PAException, "No RequestHandlers");
		}
		for (size_t i = 0; i < dirEntries.size(); i++)
		{
			if (!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
			{
				continue;
			}
#ifdef OW_DARWIN
			if (dirEntries[i].indexOf(OW_VERSION) != String::npos)
			{
					continue;
			}
#endif // OW_DARWIN
			String libName = libPath;
			libName += dirEntries[i];
			RequestHandlerIFCRef rh = SafeLibCreate<RequestHandlerIFC>::loadAndCreateObject(libName, "createRequestHandler", getLogger(COMPONENT_NAME));
			if (rh)
			{
				m_reqHandlers.push_back(rh);
			}
			else
			{
				OW_LOG_FATAL_ERROR(m_logger, Format("owprovideragent failed to load request handler from file: %1", libName));
				OW_THROW(PAException, "Invalid request handler");
			}
		}
	}

	void _loadProviders()
	{
		// TODO: Use a different config item.
		String libPath = getConfigItem(ConfigOpts::CPPIFC_PROV_LOC_opt, OW_DEFAULT_CPP_PROVIDER_LOCATION);
		if (!libPath.endsWith(OW_FILENAME_SEPARATOR))
		{
			libPath += OW_FILENAME_SEPARATOR;
		}
		OW_LOG_INFO(m_logger, Format("owprovideragent loading providers from directory %1", libPath));
		StringArray dirEntries;
		if (!FileSystem::getDirectoryContents(libPath, dirEntries))
		{
			OW_LOG_FATAL_ERROR(m_logger, Format("owprovideragent failed getting the contents of the provider directory: %1", libPath));
			OW_THROW(PAException, "No Providers");
		}
		for (size_t i = 0; i < dirEntries.size(); i++)
		{
			if (!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
			{
				continue;
			}
#ifdef OW_DARWIN
			if (dirEntries[i].indexOf(OW_VERSION) != String::npos)
			{
					continue;
			}
#endif // OW_DARWIN
			String libName = libPath;
			libName += dirEntries[i];
			CppProviderBaseIFCRef provider = CppProviderIFC::loadProvider(libName, m_logger);
			if (!provider)
			{
				OW_LOG_FATAL_ERROR(m_logger, Format("provider %1 did not load", libName));
				OW_THROW(PAException, "Invalid provider");
			}
			if (!provider->getInstanceProvider()
				&& !provider->getSecondaryInstanceProvider()
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
				&& !provider->getAssociatorProvider()
#endif
				&& !provider->getMethodProvider())
			{
				OW_LOG_FATAL_ERROR(m_logger, Format("provider %1 is not a supported (instance, secondary instance, associator, method) type", libName));
				OW_THROW(PAException, "Invalid provider");
			}
			m_providers.push_back(provider);
		}


	}

	void _createLogger()
	{
		using namespace ConfigOpts;
		Array<LogAppenderRef> appenders;
		
		StringArray additionalLogs = getConfigItem(ADDITIONAL_LOGS_opt, "").tokenize();
	
		bool debugFlag = getConfigItem(DEBUG_opt, OW_DEFAULT_DEBUG).equalsIgnoreCase("true");
		if ( debugFlag )
		{
			// stick it at the beginning as a possible slight logging performance optimization
			additionalLogs.insert(additionalLogs.begin(), LOG_DEBUG_LOG_NAME);
		}

		for (size_t i = 0; i < additionalLogs.size(); ++i)
		{
			const String& logName(additionalLogs[i]);
	
			String logType = getConfigItem(Format(LOG_1_TYPE_opt, logName), OW_DEFAULT_LOG_1_TYPE);
			String logComponents = getConfigItem(Format(LOG_1_COMPONENTS_opt, logName), OW_DEFAULT_LOG_1_COMPONENTS);
			String logCategories = getConfigItem(Format(LOG_1_CATEGORIES_opt, logName), "");
			if (logCategories.empty())
			{
				// convert level into categories
				String logLevel = getConfigItem(Format(LOG_1_LEVEL_opt, logName), OW_DEFAULT_LOG_1_LEVEL);
				if (logLevel.equalsIgnoreCase(Logger::STR_DEBUG_CATEGORY))
				{
					logCategories = Logger::STR_DEBUG_CATEGORY + " " + Logger::STR_INFO_CATEGORY + " " + Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
				}
				else if (logLevel.equalsIgnoreCase(Logger::STR_INFO_CATEGORY))
				{
					logCategories = Logger::STR_INFO_CATEGORY + " " + Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
				}
				else if (logLevel.equalsIgnoreCase(Logger::STR_ERROR_CATEGORY))
				{
					logCategories = Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
				}
				else if (logLevel.equalsIgnoreCase(Logger::STR_FATAL_CATEGORY))
				{
					logCategories = Logger::STR_FATAL_CATEGORY;
				}
			}
			String logFormat = getConfigItem(Format(LOG_1_FORMAT_opt, logName), OW_DEFAULT_LOG_1_FORMAT);
	
			appenders.push_back(LogAppender::createLogAppender(logName, logComponents.tokenize(), logCategories.tokenize(),
				logFormat, logType, m_configItems));
		}
	
	
		m_logger = new AppenderLogger(COMPONENT_NAME, appenders);
	}

private: // data
	AuthenticatorIFCRef m_authenticator;
	LoggerRef m_logger;
	Array<CppProviderBaseIFCRef> m_providers;
	Array<RequestHandlerIFCRef> m_reqHandlers;
	ConfigFile::ConfigMap m_configItems;
	ProviderAgentRef m_providerAgent;
};

typedef IntrusiveReference<MyServiceEnvironment> MyServiceEnvironmentRef;

}

void owprovideragent_new_handler();

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int rval = 0;
	MyServiceEnvironmentRef env = new MyServiceEnvironment();
	
	// until the config file is read and parsed, just use a logger that prints everything to stderr.
	LoggerRef logger(new CerrLogger());

	try
	{
		bool debugMode = processCommandLine(argc, argv, env);

		// initialize the environment.  This reads the config file.
		env->init();

		// debug mode can be activated by -d or by the config file, so check both. The config file is loaded by env->init().
		debugMode = debugMode || env->getConfigItem(ConfigOpts::DEBUG_opt, OW_DEFAULT_DEBUG).equalsIgnoreCase("true");

		// logger's not set up according to the config file until after init()
		logger = env->getLogger(COMPONENT_NAME);
		OW_LOG_INFO(logger, "owprovideragent (" OW_VERSION ") beginning startup");

		// Call platform specific code to become a daemon/service
		try
		{
			Platform::daemonize(debugMode, DAEMON_NAME, env);
		}
		catch (const DaemonException& e)
		{
			OW_LOG_FATAL_ERROR(logger, e.getMessage());
			OW_LOG_FATAL_ERROR(logger, "owprovideragent failed to initialize. Aborting...");
			return 1;
		}
		// Start all of the cimom services
		env->startServices();
		OW_LOG_INFO(logger, "owprovideragent is now running!");

		// Do this after initialization to prevent an infinite loop.
		std::unexpected_handler oldUnexpectedHandler = 0;
		std::terminate_handler oldTerminateHandler = 0;
		std::new_handler oldNewHandler = std::set_new_handler(owprovideragent_new_handler);

		if (env->getConfigItem(ConfigOpts::RESTART_ON_ERROR_opt, OW_DEFAULT_RESTART_ON_ERROR).equalsIgnoreCase("true"))
		{
			const char* const restartDisabledMessage =
				"WARNING: even though the owprovideragent.restart_on_error config option = true, it\n"
				"is not enabled. Possible reasons are that OpenWBEM is built in debug mode,\n"
				"owprovideragent is running in debug mode (-d), or owprovideragent was not run using an\n"
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
			Platform::Signal::SignalInformation signalInfo;

			sig = Platform::popSig(signalInfo);
			switch (sig)
			{
				case Platform::SHUTDOWN:

					OW_LOG_INFO(logger, "owprovideragent received shutdown notification."
						" Initiating shutdown");
					OW_LOG_INFO(logger, Format("signal details:\n%1", signalInfo));
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

					env->shutdown();
					break;
				case Platform::REINIT:
					OW_LOG_INFO(logger, "owprovideragent received restart notification."
						" Initiating restart");
					OW_LOG_INFO(logger, Format("signal details: %1", signalInfo));
					env->shutdown();
					env = 0;
					// don't try to catch the DeamonException, because if it's thrown, stuff is so whacked, we should just exit!
					Platform::rerunDaemon();

					// typically on *nix, restartDaemon() doesn't return, however to account for environments where
					// it won't we'll leave this code here to re-initialize.
					env = new MyServiceEnvironment;
					processCommandLine(argc, argv, env);
					env->init();
					env->startServices();
					break;
				default:
					OW_LOG_INFO(logger, Format("Ignoring signal. Details: %1", signalInfo));
					break;
			}
		}
	}
	catch (Exception& e)
	{
		OW_LOG_FATAL_ERROR(logger, "* EXCEPTION CAUGHT IN owprovideragent MAIN!");
		OW_LOG_FATAL_ERROR(logger, Format("* %1", e));
		Platform::sendDaemonizeStatus(Platform::DAEMONIZE_FAIL);
		rval = 1;
	}
	catch (std::exception& se)
	{
		OW_LOG_FATAL_ERROR(logger, "* std::exception CAUGHT IN owprovideragent MAIN!");
		OW_LOG_FATAL_ERROR(logger, Format("* Message: %1", se.what()));
		Platform::sendDaemonizeStatus(Platform::DAEMONIZE_FAIL);
		rval = 1;
	}
	catch(...)
	{
		OW_LOG_FATAL_ERROR(logger, "* UNKNOWN EXCEPTION CAUGHT owprovideragent MAIN!");
		Platform::sendDaemonizeStatus(Platform::DAEMONIZE_FAIL);
		rval = 1;
	}
	// Call platform specific shutdown routine
	Platform::daemonShutdown(DAEMON_NAME, env);

	env = 0;
	
	OW_LOG_INFO(logger, "owprovideragent has shutdown");
	return rval;
}

namespace
{

//////////////////////////////////////////////////////////////////////////////
bool
processCommandLine(int argc, char* argv[], const ServiceEnvironmentIFCRef& env)
{
	// Process command line options
	Platform::Options opts = Platform::daemonInit(argc, argv);
	// If the user only specified the help option on the command
	// line then get out
	if (opts.help)
	{
		if (opts.error)
		{
			std::cerr << "Unknown command line argument for owprovideragent" << std::endl;
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
	ostrm << DAEMON_NAME << " [OPTIONS]..." << std::endl;
	ostrm << "Available options:" << std::endl;
	ostrm << "\t-d, --debug  Set debug on (does not detach from terminal"<< std::endl;
	ostrm << "\t-c, --config Specifiy an alternate config file" << std::endl;
	ostrm << "\t-h, --help   Print this help information" << std::endl;
}

} // end unnamed namespace
//////////////////////////////////////////////////////////////////////////////
void owprovideragent_new_handler()
{
#if defined (OW_DEBUG)  || defined (OW_NETWARE)
	abort();
#endif

	Platform::restartDaemon();
	throw std::bad_alloc();
}

