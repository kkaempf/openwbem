/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*	this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*	this list of conditions and the following disclaimer in the documentation
*	and/or other materials provided with the distribution.
*
*  - Neither the name of Vintela, Inc. nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
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
 */

#include "OW_config.h"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ConfigException.hpp"
#include "OW_Format.hpp"
#include "OW_FileSystem.hpp"
#include "OW_SafeLibCreate.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_CIMServer.hpp"
#include "OW_CIMRepository.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_IndicationServer.hpp"
#include "OW_PollingManager.hpp"
#include "OW_Assertion.hpp"
#include "OW_AuthManager.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_WQLFilterRep.hpp"
#include "OW_IndicationRepLayer.hpp"
#include "OW_Platform.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_SharedLibraryRepository.hpp"
#include "OW_IndicationRepLayerMediator.hpp"
#include "OW_OperationContext.hpp"
#include "OW_Authorizer2IFC.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_AuthorizerManager.hpp"
#include "OW_AuthorizerIFC.hpp"
#include "OW_Socket.hpp"
#include "OW_LogAppender.hpp"
#include "OW_AppenderLogger.hpp"

#include <iostream>

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(CIMOMEnvironment)
OW_DEFINE_EXCEPTION_WITH_ID(CIMOMEnvironment)

using std::cerr;
using std::endl;

// static global
CIMOMEnvironmentRef CIMOMEnvironment::g_cimomEnvironment;

String CIMOMEnvironment::COMPONENT_NAME("ow.owcimomd");

namespace
{
	class CIMOMProviderEnvironment : public ProviderEnvironmentIFC
	{
	public:
		CIMOMProviderEnvironment(const CIMOMEnvironment* pCenv)
			: m_pCenv(pCenv)
			, m_context()
		{}
		virtual String getConfigItem(const String &name, const String& defRetVal="") const
		{
			return m_pCenv->getConfigItem(name, defRetVal);
		}
		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
			OW_ASSERT("Cannot call CIMOMProviderEnvironment::getCIMOMHandle()" == 0);
			return CIMOMHandleIFCRef();
		}
		
		virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			OW_ASSERT("Cannot call CIMOMProviderEnvironment::getRepositoryCIMOMHandle()" == 0);
			return CIMOMHandleIFCRef();
		}
		
		virtual RepositoryIFCRef getRepository() const
		{
			return m_pCenv->getRepository();
		}
		virtual LoggerRef getLogger() const
		{
			return m_pCenv->getLogger();
		}
		virtual LoggerRef getLogger(const String& componentName) const
		{
			return m_pCenv->getLogger(componentName);
		}
		virtual String getUserName() const
		{
			return Platform::getCurrentUserName();
		}
		virtual OperationContext& getOperationContext()
		{
			return m_context;
		}
	private:
		const CIMOMEnvironment* m_pCenv;
		OperationContext m_context;
	};
	ProviderEnvironmentIFCRef createProvEnvRef(const CIMOMEnvironment* pcenv)
	{
		return ProviderEnvironmentIFCRef(new CIMOMProviderEnvironment(pcenv));
	}
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
CIMOMEnvironment::CIMOMEnvironment()
	: ServiceEnvironmentIFC()
	, m_monitor()
	, m_cimRepository(0)
	, m_cimServer(0)
	, m_authManager(0)
	, m_Logger(0)
	, m_configItems(new ConfigMap)
	, m_providerManager(0)
	, m_wqlLib()
	, m_indicationRepLayerLib(0)
	, m_pollingManager(0)
	, m_indicationServer()
	, m_indicationsDisabled(true)
	, m_selectables()
	, m_selectableCallbacks()
	, m_services()
	, m_reqHandlers()
	, m_indicationLock()
	, m_indicationRepLayerDisabled(false)
	, m_selectableLock()
	, m_running(false)
	, m_indicationRepLayerMediatorRef(new IndicationRepLayerMediator)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMOMEnvironment::~CIMOMEnvironment()
{
	try
	{
		try
		{
			shutdown();
		}
		catch(Exception& e)
		{
			cerr << e << endl;
		}
		m_configItems = 0;
	}
	catch (...)
	{
		// don't let exceptions escape!
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::init()
{
	MutexLock ml(m_monitor);
	_clearSelectables();
	setConfigItem(ConfigOpts::CONFIG_FILE_opt, OW_DEFAULT_CONFIG_FILE, E_PRESERVE_PREVIOUS);
	_loadConfigItemsFromFile(getConfigItem(ConfigOpts::CONFIG_FILE_opt));

	// TODO: All these should go away, and the corresponding calls to getConfigItem should use the default.
	setConfigItem(ConfigOpts::LIBEXEC_DIR_opt, OW_DEFAULT_OWLIBEXEC_DIR, E_PRESERVE_PREVIOUS);
	setConfigItem(ConfigOpts::OWLIB_DIR_opt, OW_DEFAULT_OWLIB_DIR, E_PRESERVE_PREVIOUS);
	// Location of log file
	setConfigItem(ConfigOpts::LOG_LOCATION_opt, OW_DEFAULT_LOG_FILE, E_PRESERVE_PREVIOUS);
	// Location for data files
	setConfigItem(ConfigOpts::DATA_DIR_opt,
		OW_DEFAULT_DATA_DIR, E_PRESERVE_PREVIOUS);
	// Provider interface location
	setConfigItem(ConfigOpts::PROVIDER_IFC_LIBS_opt,
		OW_DEFAULT_IFC_LIBS, E_PRESERVE_PREVIOUS);
	// This config item should be handled in the C++ provider interface
	// C++ provider interface - provider location
	setConfigItem(ConfigOpts::CPPIFC_PROV_LOC_opt,
		OW_DEFAULT_CPP_PROVIDER_LOCATION, E_PRESERVE_PREVIOUS);
	// Authorization module
	setConfigItem(ConfigOpts::AUTH_MOD_opt,
		OW_DEFAULT_AUTH_MOD, E_PRESERVE_PREVIOUS);
	// This config item should be handled in the simple auth module
	setConfigItem(ConfigOpts::SIMPLE_AUTH_FILE_opt,
		OW_DEFAULT_SIMPLE_PASSWD_FILE, E_PRESERVE_PREVIOUS);
	setConfigItem(ConfigOpts::DISABLE_INDICATIONS_opt,
		OW_DEFAULT_DISABLE_INDICATIONS, E_PRESERVE_PREVIOUS);
	
	setConfigItem(ConfigOpts::WQL_LIB_opt,
		OW_DEFAULT_WQL_LIB, E_PRESERVE_PREVIOUS);
	setConfigItem(ConfigOpts::REQ_HANDLER_TTL_opt,
		OW_DEFAULT_REQ_HANDLER_TTL, E_PRESERVE_PREVIOUS);

	// Default Content-Language
	setConfigItem(ConfigOpts::HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE_opt,
		OW_DEFAULT_HTTP_SERVER_CONTENT_LANGUAGE, E_PRESERVE_PREVIOUS);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::unloadProviders()
{
	m_providerManager->unloadProviders(createProvEnvRef(this));
}
//////////////////////////////////////////////////////////////////////////////
namespace {
class ProviderEnvironmentServiceEnvironmentWrapper : public ProviderEnvironmentIFC
{
public:
	ProviderEnvironmentServiceEnvironmentWrapper(CIMOMEnvironment* env_)
		: env(env_)
		, m_context()
	{}
	virtual CIMOMHandleIFCRef getCIMOMHandle() const
	{
		return env->getCIMOMHandle(m_context);
	}
	
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
	{
		return env->getRepositoryCIMOMHandle(m_context);
	}
	
	virtual RepositoryIFCRef getRepository() const
	{
		return env->getRepository();
	}
	virtual LoggerRef getLogger() const
	{
		return env->getLogger();
	}
	virtual LoggerRef getLogger(const String& componentName) const
	{
		return env->getLogger(componentName);
	}
	virtual String getConfigItem(const String &name, const String& defRetVal="") const
	{
		return env->getConfigItem(name, defRetVal);
	}
	virtual String getUserName() const
	{
		return Platform::getCurrentUserName();
	}
	virtual OperationContext& getOperationContext()
	{
		return m_context;
	}
private:
	CIMOMEnvironment* env;
	mutable OperationContext m_context;
};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::startServices()
{
	// TODO: Split this up into 3 sections: load, init, start
	{
		MutexLock ml(m_monitor);
		_createLogger();

		// This is a global thing, so handle it here.
		Socket::createShutDownMechanism();

		// Create NOP authorizer manager so CIMServer can use it if it needs to.
		// This will have an authorizer loaded into it later
		m_authorizerManager = AuthorizerManagerRef(new AuthorizerManager);

		m_providerManager = ProviderManagerRef(new ProviderManager);
		m_providerManager->load(ProviderIFCLoader::createProviderIFCLoader(
			g_cimomEnvironment));
		m_cimRepository = RepositoryIFCRef(new CIMRepository(g_cimomEnvironment));
		m_cimRepository->open(getConfigItem(ConfigOpts::DATA_DIR_opt));
		m_cimServer = RepositoryIFCRef(new CIMServer(g_cimomEnvironment,
			m_providerManager, m_cimRepository, m_authorizerManager));
		_loadAuthorizer();  // old stuff
		_createAuthorizerManager();  // new stuff
		_createAuthManager();
		_loadRequestHandlers();
		_loadServices();
		_createPollingManager();
		_createIndicationServer();
	}
	{
		MutexLock l(m_runningGuard);
		m_running = true;
	}

	ProviderEnvironmentIFCRef penvRef = ProviderEnvironmentIFCRef(
		new ProviderEnvironmentServiceEnvironmentWrapper(this));

	m_providerManager->init(penvRef);
	m_authorizerManager->init(g_cimomEnvironment);

	for (size_t i = 0; i < m_services.size(); i++)
	{
		m_services[i]->startService();
	}
	if (!getConfigItem(ConfigOpts::SINGLE_THREAD_opt).equalsIgnoreCase("true"))
	{
		if (m_pollingManager)
		{
			// Start up the polling manager
			logDebug("CIMOM starting Polling Manager");
			m_pollingManager->start();
			m_pollingManager->waitUntilReady();
		}
		if (m_indicationServer)
		{
			// Start up the indication server
			logDebug("CIMOM starting IndicationServer");
			m_indicationServer->init(g_cimomEnvironment);
			m_indicationServer->start();
			m_indicationServer->waitUntilReady();
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::shutdown()
{
	logDebug("CIMOM Environment shutting down...");
	{
		MutexLock l(m_runningGuard);
		m_running = false;
	}
	MutexLock ml(m_monitor);

	// PHASE 1: SHUTDOWNS

	// this is a global thing, so do it here
	Socket::shutdownAllSockets();

	// Shutdown the polling manager
	if (m_pollingManager)
	{
		try
		{
			m_pollingManager->shutdown();
		}
		catch (...)
		{
		}
	}

	// Shutdown any loaded services
	// For now. We need to unload these in the opposite order that
	// they were loaded.
	for (int i = int(m_services.size())-1; i >= 0; i--)
	{
		try
		{
			m_services[i]->shutdown();
		}
		catch (...)
		{
		}
	}

	// Shutdown indication processing
	if (m_indicationServer)
	{
		try
		{
			m_indicationServer->shutdown();
		}
		catch (...)
		{
		}
	}

	// Shutdown the cim server
	if (m_cimServer)
	{
		m_cimServer->shutdown();
	}

	// Shutdown the cim repository
	if (m_cimRepository)
	{
		try
		{
			m_cimRepository->close();
		}
		catch (...)
		{
		}
		m_cimRepository->shutdown();
	}

	// Shut down the provider manager
	if (m_providerManager)
	{
		m_providerManager->shutdown();
	}

	// PHASE 2: unload/delete

	m_pollingManager = 0;
	
	// Clear selectable objects
	try
	{
		_clearSelectables();
	}
	catch(...)
	{
	}

	// We need to unload these in the opposite order that
	// they were loaded.
	for (int i = int(m_services.size())-1; i >= 0; i--)
	{
		m_services[i].setNull();
	}
	m_services.clear();
	// Unload all request handlers
	m_reqHandlers.clear();
	// Unload the wql library if loaded
	m_wqlLib = 0;
	// Shutdown indication processing
	if (m_indicationServer)
	{
		m_indicationServer.setNull();
		m_indicationRepLayerLib = 0;
	}
	// Delete the authentication manager
	m_authManager = 0;
	// Delete the cim server
	m_cimServer = 0;
	// Delete the cim repository
	m_cimRepository = 0;
	// Delete the authorization managerw
	m_authorizerManager = 0;
	// Delete the provider manager
	m_providerManager = 0;

	logDebug("CIMOM Environment has shut down");
}
//////////////////////////////////////////////////////////////////////////////
ProviderManagerRef
CIMOMEnvironment::getProviderManager()
{
	{
		MutexLock l(m_runningGuard);
		if (!m_running)
		{
			OW_THROW(CIMOMEnvironmentException, "CIMOMEnvironment is shutting down");
		}
	}
	MutexLock ml(m_monitor);
	OW_ASSERT(m_providerManager);
	return m_providerManager;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createAuthManager()
{
	m_authManager = AuthManagerRef(new AuthManager);
	m_authManager->init(g_cimomEnvironment);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createPollingManager()
{
	m_pollingManager = PollingManagerRef(new PollingManager(
		g_cimomEnvironment));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createIndicationServer()
{
	// Determine if user has disabled indication exportation
	m_indicationsDisabled = getConfigItem(
		ConfigOpts::DISABLE_INDICATIONS_opt).equalsIgnoreCase("true");
	if (!m_indicationsDisabled)
	{
		// load the indication server library
		String indicationLib = getConfigItem(ConfigOpts::OWLIB_DIR_opt);
		if (!indicationLib.endsWith(OW_FILENAME_SEPARATOR))
		{
			indicationLib += OW_FILENAME_SEPARATOR;
		}
		indicationLib += "libowindicationserver"OW_SHAREDLIB_EXTENSION;
		m_indicationServer = SafeLibCreate<IndicationServer>::loadAndCreateObject(
				indicationLib, "createIndicationServer", getLogger(COMPONENT_NAME));
		if (!m_indicationServer)
		{

			logError(Format("CIMOM Failed to load indication server"
				" from library %1. Indication are currently DISABLED!",
				indicationLib));
			m_indicationsDisabled = true;
			return;
		}
	
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_loadRequestHandlers()
{
	m_reqHandlers.clear();
	String libPath = getConfigItem(
		ConfigOpts::CIMOM_REQUEST_HANDLER_LOCATION_opt, OW_DEFAULT_CIMOM_REQHANDLER_LOCATION);
	if (!libPath.endsWith(OW_FILENAME_SEPARATOR))
	{
		libPath += OW_FILENAME_SEPARATOR;
	}
	logInfo(Format("CIMOM loading request handlers from"
		" directory %1", libPath));
	StringArray dirEntries;
	if (!FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		logFatalError(Format("CIMOM failed geeting the contents of the"
			" request handler directory: %1", libPath));
		OW_THROW(CIMOMEnvironmentException, "No RequestHandlers");
	}
	int reqHandlerCount = 0;
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
		RequestHandlerIFCRef rh =
			SafeLibCreate<RequestHandlerIFC>::loadAndCreateObject(
				libName, "createRequestHandler", getLogger(COMPONENT_NAME));
		if (rh)
		{
			++reqHandlerCount;
			rh->setEnvironment(g_cimomEnvironment);
			StringArray supportedContentTypes = rh->getSupportedContentTypes();
			logInfo(Format("CIMOM loaded request handler from file: %1",
				libName));
			for (StringArray::const_iterator iter = supportedContentTypes.begin();
				  iter != supportedContentTypes.end(); iter++)
			{
				ReqHandlerData rqData;
				rqData.filename = libName;
				MutexLock ml(m_reqHandlersLock);
				m_reqHandlers[(*iter)] = rqData;
				ml.release();
				logInfo(Format(
					"CIMOM associating Content-Type %1 with Request Handler %2",
					*iter, libName));
			}
		}
		else
		{
			logError(Format("CIMOM failed to load request handler from file:"
				" %1", libName));
		}
	}
	logInfo(Format("CIMOM: Handling %1 Content-Types from %2 Request Handlers",
		m_reqHandlers.size(), reqHandlerCount));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_loadServices()
{
	m_services.clear();
	String libPath = getConfigItem(
		ConfigOpts::CIMOM_SERVICES_LOCATION_opt, OW_DEFAULT_CIMOM_SERVICES_LOCATION);
	if (!libPath.endsWith(OW_FILENAME_SEPARATOR))
	{
		libPath += OW_FILENAME_SEPARATOR;
	}
	logInfo(Format("CIMOM loading services from directory %1",
		libPath));
	StringArray dirEntries;
	if (!FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		logFatalError(Format("CIMOM failed getting the contents of the"
			" services directory: %1", libPath));
		OW_THROW(CIMOMEnvironmentException, "No Services");
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
		ServiceIFCRef srv =
			SafeLibCreate<ServiceIFC>::loadAndCreateObject(libName,
				"createService", getLogger(COMPONENT_NAME));
		if (srv)
		{
			// save it first so if setServiceEnvironment throws it won't get
			// unloaded until later.
			m_services.append(srv);
			srv->setServiceEnvironment(g_cimomEnvironment);
			logInfo(Format("CIMOM loaded service from file: %1", libName));
		}
		else
		{
			logError(Format("CIMOM failed to load service from library: %1",
				libName));
		}
	}
	logInfo(Format("CIMOM: Number of services loaded: %1",
		m_services.size()));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createLogger()
{
	using namespace ConfigOpts;
	Array<LogAppenderRef> appenders;
	
	StringArray additionalLogs = getConfigItem(ADDITIONAL_LOGS_opt).tokenize();

	// this also gets set if owcimomd is run with -d
	bool debugFlag = getConfigItem(DEBUG_opt).equalsIgnoreCase("true");
	if ( debugFlag )
	{
		// stick it at the beginning as a possible slight logging performance optimization
		additionalLogs.insert(additionalLogs.begin(), LOG_DEBUG_LOG_NAME);
	}

	for (size_t i = 0; i < additionalLogs.size(); ++i)
	{
		const String& logName(additionalLogs[i]);

		String logMainType = getConfigItem(Format(LOG_1_TYPE_opt, logName), OW_DEFAULT_LOG_1_TYPE);
		String logMainComponents = getConfigItem(Format(LOG_1_COMPONENTS_opt, logName), OW_DEFAULT_LOG_1_COMPONENTS);
		String logMainCategories = getConfigItem(Format(LOG_1_CATEGORIES_opt, logName));
		if (logMainCategories.empty())
		{
			// convert level into categories
			String logMainLevel = getConfigItem(Format(LOG_1_LEVEL_opt, logName), OW_DEFAULT_LOG_1_LEVEL);
			if (logMainLevel.equalsIgnoreCase(Logger::STR_DEBUG_CATEGORY))
			{
				logMainCategories = Logger::STR_DEBUG_CATEGORY + " " + Logger::STR_INFO_CATEGORY + " " + Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
			}
			else if (logMainLevel.equalsIgnoreCase(Logger::STR_INFO_CATEGORY))
			{
				logMainCategories = Logger::STR_INFO_CATEGORY + " " + Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
			}
			else if (logMainLevel.equalsIgnoreCase(Logger::STR_ERROR_CATEGORY))
			{
				logMainCategories = Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
			}
			else if (logMainLevel.equalsIgnoreCase(Logger::STR_FATAL_CATEGORY))
			{
				logMainCategories = Logger::STR_FATAL_CATEGORY;
			}
		}
		String logMainFormat = getConfigItem(Format(LOG_1_FORMAT_opt, logName), OW_DEFAULT_LOG_1_FORMAT);

		appenders.push_back(LogAppender::createLogAppender(logName, logMainComponents.tokenize(), logMainCategories.tokenize(),
			logMainFormat, logMainType, *m_configItems));
	}


	// This one will eventually be handled the same as all other logs by just sticking "main" in the additionalLogs array
	// but we need to handle deprecated options for now, so it needs special treatment.
	String logName(LOG_MAIN_LOG_NAME);
	String logMainType = getConfigItem(Format(LOG_1_TYPE_opt, logName));
	String logMainComponents = getConfigItem(Format(LOG_1_COMPONENTS_opt, logName), OW_DEFAULT_LOG_1_COMPONENTS);
	String logMainCategories = getConfigItem(Format(LOG_1_CATEGORIES_opt, logName));
	String logMainLevel = getConfigItem(Format(LOG_1_LEVEL_opt, logName));
	String logMainFormat = getConfigItem(Format(LOG_1_FORMAT_opt, logName), OW_DEFAULT_LOG_1_FORMAT);

	// map the old log_location onto log.main.type and log.main.location if necessary
	if (logMainType.empty())
	{
		String deprecatedLogLocation = getConfigItem(ConfigOpts::LOG_LOCATION_opt);
		if (deprecatedLogLocation.empty() || deprecatedLogLocation.equalsIgnoreCase("syslog"))
		{
			logMainType = "syslog";
		}
		else if (deprecatedLogLocation.equalsIgnoreCase("null"))
		{
			logMainType = "null";
		}
		else
		{
			logMainType = "file";
			setConfigItem(Format(LOG_1_LOCATION_opt, logName), deprecatedLogLocation);
		}
	}

	// map the old log_level onto log.main.level if necessary
	if (logMainCategories.empty() && logMainLevel.empty())
	{
		String deprecatedLogLevel = getConfigItem(ConfigOpts::LOG_LEVEL_opt);
		if (deprecatedLogLevel.empty())
		{
			logMainLevel = OW_DEFAULT_LOG_1_LEVEL;
		}
		else
		{
			// old used "fatalerror", now we just use FATAL
			if (deprecatedLogLevel.equalsIgnoreCase("fatalerror"))
			{
				logMainLevel = Logger::STR_FATAL_CATEGORY;
			}
			else
			{
				deprecatedLogLevel.toUpperCase();
				logMainLevel = deprecatedLogLevel;
			}
		}
	}
	
	// convert level into categories
	if (logMainCategories.empty())
	{
		// convert level into categories
		String logMainLevel = getConfigItem(Format(LOG_1_LEVEL_opt, logName), OW_DEFAULT_LOG_1_LEVEL);
		if (logMainLevel.equalsIgnoreCase(Logger::STR_DEBUG_CATEGORY))
		{
			logMainCategories = Logger::STR_DEBUG_CATEGORY + " " + Logger::STR_INFO_CATEGORY + " " + Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
		}
		else if (logMainLevel.equalsIgnoreCase(Logger::STR_INFO_CATEGORY))
		{
			logMainCategories = Logger::STR_INFO_CATEGORY + " " + Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
		}
		else if (logMainLevel.equalsIgnoreCase(Logger::STR_ERROR_CATEGORY))
		{
			logMainCategories = Logger::STR_ERROR_CATEGORY + " " + Logger::STR_FATAL_CATEGORY;
		}
		else if (logMainLevel.equalsIgnoreCase(Logger::STR_FATAL_CATEGORY))
		{
			logMainCategories = Logger::STR_FATAL_CATEGORY;
		}
	}

	appenders.push_back(LogAppender::createLogAppender(logName, logMainComponents.tokenize(), logMainCategories.tokenize(),
		logMainFormat, logMainType, *m_configItems));


	m_Logger = new AppenderLogger(COMPONENT_NAME, appenders);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_loadConfigItemsFromFile(const String& filename)
{
	// We don't have a logger at this point, but it will get printed anyway
	logDebug("\nUsing config file: " + filename);
	ConfigFile::loadConfigFile(filename, *m_configItems);
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMOMEnvironment::authenticate(String &userName, const String &info,
	String &details, OperationContext& context)
{
	{
		MutexLock l(m_runningGuard);
		if (!m_running)
		{
			return false;
		}
	}
	MutexLock ml(m_monitor);
	OW_ASSERT(m_authManager);
	return m_authManager->authenticate(userName, info, details, context);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMOMEnvironment::getConfigItem(const String &name, const String& defRetVal) const
{
	return ConfigFile::getConfigItem(*m_configItems, name, defRetVal);
}
//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
CIMOMEnvironment::getWQLFilterCIMOMHandle(const CIMInstance& inst,
		OperationContext& context)
{
	{
		MutexLock l(m_runningGuard);
		if (!m_running)
		{
			OW_THROW(CIMOMEnvironmentException, "CIMOMEnvironment is shutting down");
		}
	}
	MutexLock ml(m_monitor);
	OW_ASSERT(m_cimServer);
	return CIMOMHandleIFCRef(new LocalCIMOMHandle(
		g_cimomEnvironment,
		RepositoryIFCRef(new WQLFilterRep(inst, m_cimServer)), context));
}
//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
CIMOMEnvironment::getCIMOMHandle(OperationContext& context,
	ESendIndicationsFlag doIndications,
	EBypassProvidersFlag bypassProviders)
{
	return getCIMOMHandle(context,doIndications,bypassProviders,E_LOCKING);
}
//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
CIMOMEnvironment::getCIMOMHandle(OperationContext& context,
	ESendIndicationsFlag doIndications,
	EBypassProvidersFlag bypassProviders,
	ELockingFlag locking)
{
	{
		MutexLock l(m_runningGuard);
		if (!m_running)
		{
			OW_THROW(CIMOMEnvironmentException, "CIMOMEnvironment is shutting down");
		}
	}
	MutexLock ml(m_monitor);
	OW_ASSERT(m_cimServer);

	// Here we construct a pipeline.  Currently it looks like:
	// LocalCIMOMHandle -> [ Authorizer -> ] [Indication Rep Layer -> ] [ CIM Server -> ] CIM Repository

	RepositoryIFCRef rref;
	if (bypassProviders == E_BYPASS_PROVIDERS)
	{
		rref = m_cimRepository;
	}
	else
	{
		rref = m_cimServer;
	}

	if (doIndications
	   && m_indicationServer
	   && !m_indicationsDisabled)
	{
		SharedLibraryRepositoryIFCRef irl = _getIndicationRepLayer(rref);
		if (irl)
		{
			rref = RepositoryIFCRef(new SharedLibraryRepository(irl));
		}
	}
	if (m_authorizer)
	{
		AuthorizerIFC* p = m_authorizer->clone();
		p->setSubRepositoryIFC(rref);
		rref = RepositoryIFCRef(new SharedLibraryRepository(SharedLibraryRepositoryIFCRef(m_authorizer.getLibRef(), RepositoryIFCRef(p))));
	}

	return CIMOMHandleIFCRef(new LocalCIMOMHandle(g_cimomEnvironment, rref,
		context, locking == E_LOCKING ? LocalCIMOMHandle::E_LOCKING : LocalCIMOMHandle::E_NO_LOCKING));
}
//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
CIMOMEnvironment::getRepositoryCIMOMHandle(OperationContext& context)
{
	return getCIMOMHandle(context, E_DONT_SEND_INDICATIONS, E_BYPASS_PROVIDERS);
}
//////////////////////////////////////////////////////////////////////////////
WQLIFCRef
CIMOMEnvironment::getWQLRef()
{
	{
		MutexLock l(m_runningGuard);
		if (!m_running)
		{
			OW_THROW(CIMOMEnvironmentException, "CIMOMEnvironment is shutting down");
		}
	}
	MutexLock ml(m_monitor);
	if (!m_wqlLib)
	{
		String libname = getConfigItem(ConfigOpts::WQL_LIB_opt);
		logDebug(Format("CIMOM loading wql library %1", libname));
		SharedLibraryLoaderRef sll =
			SharedLibraryLoader::createSharedLibraryLoader();
		m_wqlLib = sll->loadSharedLibrary(libname, m_Logger);
		if (!m_wqlLib)
		{
			logError(Format("CIMOM Failed to load WQL Libary: %1", libname));
			return WQLIFCRef();
		}
	}
	return  WQLIFCRef(m_wqlLib, SafeLibCreate<WQLIFC>::create(
		m_wqlLib, "createWQL", m_Logger));
}
//////////////////////////////////////////////////////////////////////////////
SharedLibraryRepositoryIFCRef
CIMOMEnvironment::_getIndicationRepLayer(const RepositoryIFCRef& rref)
{
	SharedLibraryRepositoryIFCRef retref;
	if (!m_indicationRepLayerDisabled)
	{
		MutexLock ml(m_indicationLock);
		if (!m_indicationRepLayerLib)
		{
			const String libPath = getConfigItem(ConfigOpts::OWLIB_DIR_opt) + OW_FILENAME_SEPARATOR;
			const String libBase = "libowindicationreplayer";
			String libname = libPath + libBase + OW_SHAREDLIB_EXTENSION;
			logDebug(Format("CIMOM loading indication libary %1",
				libname));
			SharedLibraryLoaderRef sll =
				SharedLibraryLoader::createSharedLibraryLoader();

			if (!sll)
			{
				m_indicationRepLayerDisabled = true;
				logFatalError(Format("CIMOM failed to create SharedLibraryLoader"
					" library %1", libname));
				return retref;
			}
			m_indicationRepLayerLib = sll->loadSharedLibrary(libname, m_Logger);
			if (!m_indicationRepLayerLib)
			{
				m_indicationRepLayerDisabled = true;
				logFatalError(Format("CIMOM failed to load indication rep layer"
					" library %1", libname));
				return retref;
			}
		}
		IndicationRepLayer* pirep =
			SafeLibCreate<IndicationRepLayer>::create(
				m_indicationRepLayerLib, "createIndicationRepLayer", m_Logger);
		if (pirep)
		{
			retref = SharedLibraryRepositoryIFCRef(m_indicationRepLayerLib,
				RepositoryIFCRef(pirep));
			pirep->setCIMServer(rref);
		}
		else
		{
			m_indicationRepLayerDisabled = true;
			m_indicationRepLayerLib = 0;
		}
	}
	return retref;
}

//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_loadAuthorizer()
{
	OW_ASSERT(!m_authorizer);
	String libname = getConfigItem(ConfigOpts::AUTHORIZATION_LIB_opt);

	// no authorization requested
	if (libname.empty())
	{
		return;
	}

	logDebug(Format("CIMOM loading authorization libary %1",
					libname));
	SharedLibraryLoaderRef sll =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!sll)
	{
		String msg = Format("CIMOM failed to create SharedLibraryLoader."
							" library %1", libname);
		logFatalError(msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	SharedLibraryRef authorizerLib = sll->loadSharedLibrary(libname, m_Logger);
	if (!authorizerLib)
	{
		String msg = Format("CIMOM failed to load authorization"
							" library %1", libname);
		logFatalError(msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	AuthorizerIFC* p =
		SafeLibCreate<AuthorizerIFC>::create(
			authorizerLib, "createAuthorizer", m_Logger);
	if (!p)
	{
		String msg = Format("CIMOM failed to load authorization"
							" library %1", libname);
		logFatalError(msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	m_authorizer = AuthorizerIFCRef(authorizerLib,
									AuthorizerIFCRef::element_type(p));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createAuthorizerManager()
{
	// m_authorizerManager should actually be a valid AuthorizerManager
	// already, but it doesn't have a authorizer loaded yet.

	String libname = getConfigItem(ConfigOpts::AUTHORIZATION2_LIB_opt);

	// no authorization requested
	if (libname.empty())
	{
		return;
	}

	logDebug(Format("CIMOM loading authorization libary %1", libname));

	SharedLibraryLoaderRef sll =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!sll)
	{
		String msg = Format("CIMOM failed to create SharedLibraryLoader."
			" library %1", libname);
		logFatalError(msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	SharedLibraryRef authorizerLib = sll->loadSharedLibrary(libname, m_Logger);
	if (!authorizerLib)
	{
		String msg = Format("CIMOM failed to load authorization"
			" library %1", libname);
		logFatalError(msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	Authorizer2IFC* p =
		SafeLibCreate<Authorizer2IFC>::create(
			authorizerLib, "createAuthorizer2", m_Logger);
	if (!p)
	{
		String msg = Format("CIMOM failed to load authorization"
			" library %1", libname);
		logFatalError(msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}

	m_authorizerManager->setAuthorizer(
		Authorizer2IFCRef(authorizerLib,Authorizer2IFCRef::element_type(p)));
}
//////////////////////////////////////////////////////////////////////////////
RequestHandlerIFCRef
CIMOMEnvironment::getRequestHandler(const String &id)
{
	RequestHandlerIFCRef ref;
	{
		MutexLock l(m_runningGuard);
		if (!m_running)
		{
			return ref;
		}
	}
	MutexLock ml(m_reqHandlersLock);
	ReqHandlerMap::iterator iter =
			m_reqHandlers.find(id);
	if (iter != m_reqHandlers.end())
	{
		if (!iter->second.rqIFCRef)
		{
			iter->second.rqIFCRef =
				SafeLibCreate<RequestHandlerIFC>::loadAndCreateObject(
					iter->second.filename, "createRequestHandler", getLogger(COMPONENT_NAME));
		}
		if (iter->second.rqIFCRef)
		{
			ref = RequestHandlerIFCRef(iter->second.rqIFCRef.getLibRef(),
				iter->second.rqIFCRef->clone());
			iter->second.dt.setToCurrent();
			ref->setEnvironment(g_cimomEnvironment);
			logDebug(Format("Request Handler %1 handling request for content type %2",
				iter->second.filename, id));
		}
		else
		{
			logError(Format(
				"Error loading request handler library %1 for content type %2",
				iter->second.filename, id));
		}
	}
	return ref;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::unloadReqHandlers()
{
	//logDebug("Running unloadReqHandlers()");
	Int32 ttl;
	try
	{
		ttl = getConfigItem(ConfigOpts::REQ_HANDLER_TTL_opt).toInt32();
	}
	catch (const StringConversionException&)
	{
		logError(Format("Invalid value (%1) for %2 config item.  Using default.",
			getConfigItem(ConfigOpts::REQ_HANDLER_TTL_opt),
			ConfigOpts::REQ_HANDLER_TTL_opt));
		ttl = String(OW_DEFAULT_REQ_HANDLER_TTL).toInt32();
	}
	if (ttl < 0)
	{
		logDebug("Non-Positive TTL for Request Handlers: OpenWBEM will not unload request handlers.");
		return;
	}
	DateTime dt;
	dt.setToCurrent();
	MutexLock ml(m_reqHandlersLock);
	for (ReqHandlerMap::iterator iter = m_reqHandlers.begin();
		  iter != m_reqHandlers.end(); ++iter)
	{
		if (iter->second.rqIFCRef)
		{
			DateTime rqDT = iter->second.dt;
			rqDT.addMinutes(ttl);
			if (rqDT < dt)
			{
				iter->second.rqIFCRef.setNull();
				logDebug(Format("Unloaded request handler lib %1 for content type %2",
					iter->second.filename, iter->first));
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
LoggerRef
CIMOMEnvironment::getLogger() const
{
	OW_ASSERT(m_Logger);
	return m_Logger->clone();
}
//////////////////////////////////////////////////////////////////////////////
LoggerRef
CIMOMEnvironment::getLogger(const String& componentName) const
{
	OW_ASSERT(m_Logger);
	LoggerRef rv(m_Logger->clone());
	rv->setDefaultComponent(componentName);
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
IndicationServerRef
CIMOMEnvironment::getIndicationServer() const
{
	return m_indicationServer;
}
//////////////////////////////////////////////////////////////////////////////
PollingManagerRef
CIMOMEnvironment::getPollingManager() const
{
	return m_pollingManager;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::clearConfigItems()
{
	m_configItems->clear();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::setConfigItem(const String &item,
	const String &value, EOverwritePreviousFlag overwritePrevious)
{
	ConfigFile::setConfigItem(*m_configItems, item, value, ConfigFile::EOverwritePreviousFlag(overwritePrevious));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::runSelectEngine()
{
	OW_ASSERT(m_selectables.size() == m_selectableCallbacks.size());
	SelectEngine engine;
	// Insure the signal pipe is at the front of the select list
	engine.addSelectableObject(Platform::getSigSelectable(),
		SelectableCallbackIFCRef(new SelectEngineStopper(engine)));
	
	for (size_t i = 0; i < m_selectables.size(); ++i)
	{
		engine.addSelectableObject(m_selectables[i], m_selectableCallbacks[i]);
	}
	engine.go();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_clearSelectables()
{
	MutexLock ml(m_selectableLock);
	m_selectables.clear();
	m_selectableCallbacks.clear();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::addSelectable(const SelectableIFCRef& obj,
	const SelectableCallbackIFCRef& cb)
{
	MutexLock ml(m_selectableLock);
	m_selectables.push_back(obj);
	m_selectableCallbacks.push_back(cb);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::removeSelectable(const SelectableIFCRef& obj)
{
	MutexLock ml(m_selectableLock);
    for (size_t i = 0; i < m_selectables.size(); i++)
    {
        if (obj == m_selectables[i])
        {
            m_selectables.remove(i);
            m_selectableCallbacks.remove(i);
            --i;
            continue;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::exportIndication(const CIMInstance& instance,
	const String& instNS)
{
	logDebug("CIMOMEnvironment::exportIndication");
	if (m_indicationServer && !m_indicationsDisabled)
	{
		logDebug("CIMOMEnvironment::exportIndication - calling indication"
			" server");
		m_indicationServer->processIndication(instance, instNS);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::logInfo(const String& s) const
{
	if (m_Logger)
	{
		m_Logger->logInfo(s);
	}
	else
	{
		std::cout << s << std::endl;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::logDebug(const String& s) const
{
	if (m_Logger)
	{
		m_Logger->logDebug(s);
	}
	else
	{
		std::cout << s << std::endl;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::logError(const String& s) const
{
	if (m_Logger)
	{
		m_Logger->logError(s);
	}
	else
	{
		std::cerr << s << std::endl;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::logFatalError(const String& s) const
{
	if (m_Logger)
	{
		m_Logger->logFatalError(s);
	}
	else
	{
		std::cerr << s << std::endl;
	}
}
//////////////////////////////////////////////////////////////////////////////
IndicationRepLayerMediatorRef
CIMOMEnvironment::getIndicationRepLayerMediator() const
{
	return m_indicationRepLayerMediatorRef;
}
//////////////////////////////////////////////////////////////////////////////
RepositoryIFCRef
CIMOMEnvironment::getRepository() const
{
	return m_cimRepository;
}
//////////////////////////////////////////////////////////////////////////////
AuthorizerManagerRef
CIMOMEnvironment::getAuthorizerManager() const
{
	return m_authorizerManager;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray CIMOMEnvironment::getInteropInstances(const String& className) const
{
	MutexLock lock(m_interopInstancesLock);
	interopInstances_t::const_iterator citer = m_interopInstances.find(className);
	if (citer == m_interopInstances.end())
	{
		return CIMInstanceArray();
	}

	return CIMInstanceArray(citer->second.begin(), citer->second.end());
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::setInteropInstance(const CIMInstance& inst)
{
	String className = inst.getClassName();

	MutexLock lock(m_interopInstancesLock);
	interopInstances_t::iterator iter = m_interopInstances.find(className);
	if (iter == m_interopInstances.end())
	{
		interopInstances_t::data_type s;
		s.insert(inst);
		m_interopInstances.insert(interopInstances_t::value_type(className, s));
	}
	else
	{
		// look for and erase a pre-existing instance with the same path first, to handle updates.
		CIMObjectPath newInstPath = CIMObjectPath(String(), inst);
		typedef interopInstances_t::data_type::const_iterator citer_t;
		for (citer_t curInstance = iter->second.begin(); curInstance != iter->second.end(); ++iter)
		{
			if (newInstPath == CIMObjectPath(String(), *curInstance))
			{
				iter->second.erase(*curInstance);
				break;
			}
		}

		iter->second.insert(inst);
	}
}

} // end namespace OpenWBEM

