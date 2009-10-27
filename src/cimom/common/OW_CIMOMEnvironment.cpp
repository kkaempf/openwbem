/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_ConfigException.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/FileSystem.hpp"
#include "OW_SafeLibCreate.hpp"
#include "blocxx/SelectEngine.hpp"
#include "OW_CIMServer.hpp"
#include "OW_CIMRepository.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_IndicationServer.hpp"
#include "OW_PollingManager.hpp"
#include "blocxx/Assertion.hpp"
#include "OW_AuthManager.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_WQLFilterRep.hpp"
#include "OW_IndicationRepLayer.hpp"
#include "OW_Platform.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_SharedLibraryRepository.hpp"
#include "OW_IndicationRepLayerMediator.hpp"
#include "OW_LocalOperationContext.hpp"
#include "OW_Authorizer2IFC.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_AuthorizerManager.hpp"
#include "OW_AuthorizerIFC.hpp"
#include "blocxx/Socket.hpp"
#include "blocxx/Logger.hpp"
#include "blocxx/LogAppender.hpp"
#include "blocxx/MultiAppender.hpp"
#include "blocxx/CerrAppender.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_ProviderEnvironmentException.hpp"
#include "blocxx/GlobalPtr.hpp"

#include <iostream>
#include <map>
#include <set>

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(CIMOMEnvironment)
OW_DEFINE_EXCEPTION_WITH_ID(CIMOMEnvironment)

using std::cerr;
using std::endl;
using namespace blocxx;

namespace
{
struct CimomEnvironmentFactory
{
	static CIMOMEnvironmentRef* create()
	{
		return new CIMOMEnvironmentRef(new CIMOMEnvironment());
	}
};
// the one and only
GlobalPtr<CIMOMEnvironmentRef, CimomEnvironmentFactory> theCimomEnvironment = BLOCXX_GLOBAL_PTR_INIT;
}

CIMOMEnvironmentRef&
CIMOMEnvironment::instance()
{
	return *theCimomEnvironment;
}

namespace
{
	const char* const COMPONENT_NAME("ow.owcimomd");

	class CIMOMProviderEnvironment : public ProviderEnvironmentIFC
	{
	public:
		CIMOMProviderEnvironment(CIMOMEnvironmentRef pCenv)
			: m_pCenv(pCenv)
			, m_context()
		{}
		virtual String getConfigItem(const String &name, const String& defRetVal="") const
		{
			return m_pCenv->getConfigItem(name, defRetVal);
		}
		virtual StringArray getMultiConfigItem(const String &itemName,
			const StringArray& defRetVal, const char* tokenizeSeparator) const
		{
			return m_pCenv->getMultiConfigItem(itemName, defRetVal, tokenizeSeparator);
		}
		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
			BLOCXX_ASSERT("Cannot call CIMOMProviderEnvironment::getCIMOMHandle()" == 0);
			return CIMOMHandleIFCRef();
		}

		virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			BLOCXX_ASSERT("Cannot call CIMOMProviderEnvironment::getRepositoryCIMOMHandle()" == 0);
			return CIMOMHandleIFCRef();
		}

		virtual RepositoryIFCRef getRepository() const
		{
			return m_pCenv->getRepository();
		}
		virtual RepositoryIFCRef getAuthorizingRepository() const
		{
			return m_pCenv->getAuthorizingRepository();
		}
		virtual String getUserName() const
		{
			return Platform::getCurrentUserName();
		}
		virtual OperationContext& getOperationContext()
		{
			return m_context;
		}
		virtual ProviderEnvironmentIFCRef clone() const
		{
			return ProviderEnvironmentIFCRef(new CIMOMProviderEnvironment(m_pCenv));
		}
	private:
		CIMOMEnvironmentRef m_pCenv;
		LocalOperationContext m_context;
	};
	ProviderEnvironmentIFCRef createProvEnvRef(const CIMOMEnvironmentRef& pcenv)
	{
		return ProviderEnvironmentIFCRef(new CIMOMProviderEnvironment(pcenv));
	}
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
// We don't initialize everything here, since startServices() and shutdown() manage the lifecycle.
// We start off with a dumb logger and an empty config map.
CIMOMEnvironment::CIMOMEnvironment()
	: m_Logger(COMPONENT_NAME, new CerrAppender())
	, m_configItems(new ConfigMap)
	, m_indicationsDisabled(true)
	, m_indicationRepLayerDisabled(false)
	, m_state(E_STATE_INVALID)
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
			if (isLoaded(m_state))
			{
				shutdown();
			}
		}
		catch(Exception& e)
		{
			cerr << e << endl;
		}
		m_configItems = 0;
		m_state = E_STATE_INVALID; // just for the heck of it.
	}
	catch (Exception& e)
	{
		BLOCXX_LOG_ERROR(m_Logger, Format("Caught exception in CIMOMEnvironment::~CIMOMEnvironment(): %1", e));
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
	// init() is called from a single-threaded state
	_clearSelectables();

	// The config file config item may be set by main before init() is called.
	_loadConfigItemsFromFile(getConfigItem(ConfigOpts::CONFIG_FILE_opt, OW_DEFAULT_CONFIG_FILE));

	// logger is treated special, so it goes in init() not startServices()
	_createLogger();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::startServices()
{
	// Split up into 3 sections:
	// 1. load
	// 2. init, initialized
	// 3. start, started

	// We start out single-threaded.  The start phase is when threads enter the picture.

	// This is a global thing, so handle it here.
	Socket::createShutDownMechanism();

	// load
	BLOCXX_LOG_DEBUG2(m_Logger, "CIMOMEnvironment loading services");

	m_authorizerManager = new AuthorizerManager;
	m_services.push_back(ServiceIFCRef(SharedLibraryRef(), m_authorizerManager));

	m_providerManager = new ProviderManager;
	m_providerManager->load(ProviderIFCLoader::createProviderIFCLoader(
		this), this);
	m_services.push_back(ServiceIFCRef(SharedLibraryRef(), m_providerManager));

	m_cimRepository = new CIMRepository;
	m_services.push_back(ServiceIFCRef(SharedLibraryRef(), m_cimRepository));

	_loadAuthorizer();  // CIMServer constructor has a dependency on m_authorizer
	_createAuthorizerManager();  // new stuff

	m_cimServer = RepositoryIFCRef(new CIMServer(this,
		m_providerManager, m_cimRepository, m_authorizerManager, m_authorizer));
	m_services.push_back(ServiceIFCRef(SharedLibraryRef(), m_cimServer));

	_createAuthManager();
	_loadRequestHandlers();
	_loadServices();
	if (!getConfigItem(ConfigOpts::HTTP_SERVER_SINGLE_THREAD_opt).equalsIgnoreCase("true"))
	{
		_createPollingManager();
		_createIndicationServer();
	}

	BLOCXX_LOG_DEBUG(m_Logger, "CIMOMEnvironment finished loading services");

	_sortServicesForDependencies();

	// init

	BLOCXX_LOG_DEBUG2(m_Logger, "CIMOMEnvironment initializing services");

	{
		MutexLock l(m_stateGuard);
		m_state = E_STATE_INITIALIZING;
	}

	for (size_t i = 0; i < m_services.size(); i++)
	{
		BLOCXX_LOG_DEBUG2(m_Logger, Format("CIMOM initializing service: %1", m_services[i]->getName()));
		m_services[i]->init(this);
	}
	{
		MutexLock l(m_stateGuard);
		m_state = E_STATE_INITIALIZED;
	}

	for (size_t i = 0; i < m_services.size(); i++)
	{
		BLOCXX_LOG_DEBUG2(m_Logger, Format("CIMOM calling initialized() for service: %1", m_services[i]->getName()));
		m_services[i]->initialized();
	}

	BLOCXX_LOG_DEBUG(m_Logger, "CIMOMEnvironment finished initializing services");

	// start
	BLOCXX_LOG_DEBUG2(m_Logger, "CIMOMEnvironment starting services");
	{
		MutexLock l(m_stateGuard);
		m_state = E_STATE_STARTING;
	}

	for (size_t i = 0; i < m_services.size(); i++)
	{
		BLOCXX_LOG_DEBUG2(m_Logger, Format("CIMOM starting service: %1", m_services[i]->getName()));
		m_services[i]->start();
	}
	{
		MutexLock l(m_stateGuard);
		m_state = E_STATE_STARTED;
	}

	for (size_t i = 0; i < m_services.size(); i++)
	{
		BLOCXX_LOG_DEBUG2(m_Logger, Format("CIMOM calling started() for service: %1", m_services[i]->getName()));
		m_services[i]->started();
	}

	BLOCXX_LOG_DEBUG(m_Logger, "CIMOMEnvironment finished starting services");
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::shutdown()
{

	// notify all services of impending shutdown.
	BLOCXX_LOG_DEBUG2(m_Logger, "CIMOMEnvironment notifying services of shutdown");
	// Do this in reverse order because of dependencies
	for (int i = int(m_services.size())-1; i >= 0; i--)
	{
		try
		{
			BLOCXX_LOG_DEBUG2(m_Logger, Format("CIMOMEnvironment notifying service: %1", m_services[i]->getName()));
			m_services[i]->shuttingDown();
		}
		catch (Exception& e)
		{
			BLOCXX_LOG_ERROR(m_Logger, Format("Caught exception while calling shuttingDown(): %1", e));
		}
		catch (...)
		{
		}
	}

	// PHASE 1: SHUTDOWNS
	BLOCXX_LOG_DEBUG2(m_Logger, "CIMOMEnvironment beginning shutdown process");
	{
		MutexLock l(m_stateGuard);
		m_state = E_STATE_SHUTTING_DOWN;
	}

	// It appears that we don't need this anymore.  Furthermore, it causes
	// problems with out-of-process providers, as we need their sockets to
	// stay open until we are done calling functions like deActivate and
	// shuttingDown on them.
#if 0
	BLOCXX_LOG_DEBUG2(m_Logger, "CIMOMEnvironment shutting down sockets");
	// this is a global thing, so do it here
	Socket::shutdownAllSockets();
#endif

	// Shutdown all services
	BLOCXX_LOG_DEBUG2(m_Logger, "CIMOMEnvironment shutting down services");
	// Do this in reverse order because of dependencies
	for (int i = int(m_services.size())-1; i >= 0; i--)
	{
		try
		{
			BLOCXX_LOG_DEBUG2(m_Logger, Format("CIMOMEnvironment shutting down service: %1", m_services[i]->getName()));
			m_services[i]->shutdown();
		}
		catch (Exception& e)
		{
			BLOCXX_LOG_ERROR(m_Logger, Format("Caught exception while calling shutdown(): %1", e));
		}
		catch (...)
		{
		}
	}

	BLOCXX_LOG_DEBUG(m_Logger, "CIMOMEnvironment shut down services");
	{
		MutexLock l(m_stateGuard);
		m_state = E_STATE_SHUTDOWN;
	}

	// PHASE 2: unload/delete

	// get this lock here so that we delete everything atomically
	MutexLock ml(m_monitor);

	BLOCXX_LOG_DEBUG2(m_Logger, "CIMOMEnvironment unloading and deleting services");

	m_pollingManager = 0;

	// Clear selectable objects
	try
	{
		_clearSelectables();
	}
	catch (Exception& e)
	{
		BLOCXX_LOG_ERROR(m_Logger, Format("Caught exception while calling _clearSelectables(): %1", e));
	}
	catch(...)
	{
	}

	// Shutdown indication processing.
	// This has to happen before the services are cleared to prevent an invalid reference from causing a crash.
	if (m_indicationServer)
	{
		m_indicationServer.setNull();
		m_indicationRepLayerLib = 0;
	}

	// We need to unload these in the opposite order that
	// they were loaded because of bugs in shared library
	// handling on certain OSes.
	for (int i = int(m_services.size())-1; i >= 0; i--)
	{
		m_services[i].setNull();
	}
	m_services.clear();
	// Unload all request handlers
	m_reqHandlers.clear();
	// Unload the wql library if loaded
	m_wqlLib = 0;
	// Delete the authentication manager
	m_authManager = 0;
	// Delete the cim server
	m_cimServer = 0;
	// Delete the cim repository
	m_cimRepository = 0;
	// Delete the authorization manager
	m_authorizerManager = 0;
	// Delete the provider manager
	m_providerManager = 0;

	{
		MutexLock l(m_stateGuard);
		m_state = E_STATE_UNLOADED;
	}

	BLOCXX_LOG_DEBUG(m_Logger, "CIMOMEnvironment has shut down");
}
//////////////////////////////////////////////////////////////////////////////
ProviderManagerRef
CIMOMEnvironment::getProviderManager() const
{
	{
		MutexLock l(m_stateGuard);
		if (!isLoaded(m_state))
		{
			OW_THROW(CIMOMEnvironmentException, "CIMOMEnvironment::getProviderManager() called when state is not constructed");
		}
	}
	BLOCXX_ASSERT(m_providerManager);
	return m_providerManager;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createAuthManager()
{
	m_authManager = AuthManagerRef(new AuthManager);
	m_services.push_back(ServiceIFCRef(SharedLibraryRef(), m_authManager));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createPollingManager()
{
	m_pollingManager = PollingManagerRef(new PollingManager(m_providerManager));
	m_services.push_back(ServiceIFCRef(SharedLibraryRef(), m_pollingManager));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createIndicationServer()
{
	// Determine if user has disabled indication exportation
	m_indicationsDisabled = getConfigItem(
		ConfigOpts::DISABLE_INDICATIONS_opt, OW_DEFAULT_DISABLE_INDICATIONS).equalsIgnoreCase("true");
	if (!m_indicationsDisabled)
	{
		// load the indication server library
		String indicationLib = getConfigItem(ConfigOpts::OWLIBDIR_opt, OW_DEFAULT_OWLIBDIR);
		if (!indicationLib.endsWith(OW_FILENAME_SEPARATOR))
		{
			indicationLib += OW_FILENAME_SEPARATOR;
		}
		indicationLib += "libowindicationserver"OW_SHAREDLIB_EXTENSION;
		m_indicationServer = SafeLibCreate<IndicationServer>::loadAndCreateObject(
				indicationLib, "createIndicationServer", OW_VERSION);
		if (!m_indicationServer)
		{

			BLOCXX_LOG_FATAL_ERROR(m_Logger, Format("CIMOM Failed to load indication server"
				" from library %1. Indication are currently DISABLED!",
				indicationLib));
			OW_THROW(CIMOMEnvironmentException, "Failed to load indication server");
		}
		m_services.push_back(m_indicationServer);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_loadRequestHandlers()
{
	m_reqHandlers.clear();
	int reqHandlerCount = 0;
	const StringArray libPaths = getMultiConfigItem(
		ConfigOpts::REQUEST_HANDLER_PATH_opt,
		String(OW_DEFAULT_REQUEST_HANDLER_PATH).tokenize(OW_PATHNAME_SEPARATOR),
		OW_PATHNAME_SEPARATOR);
	for (size_t i = 0; i < libPaths.size(); ++i)
	{
		String libPath(libPaths[i]);
		if (!libPath.endsWith(OW_FILENAME_SEPARATOR))
		{
			libPath += OW_FILENAME_SEPARATOR;
		}
		BLOCXX_LOG_INFO(m_Logger, Format("CIMOM loading request handlers from"
			" directory %1", libPath));
		StringArray dirEntries;
		if (!FileSystem::getDirectoryContents(libPath, dirEntries))
		{
			BLOCXX_LOG_FATAL_ERROR(m_Logger, Format("CIMOM failed getting the contents of the"
				" request handler directory: %1", libPath));
			OW_THROW(CIMOMEnvironmentException, "No RequestHandlers");
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
			RequestHandlerIFCRef rh =
				SafeLibCreate<RequestHandlerIFC>::loadAndCreateObject(
					libName, "createRequestHandler", OW_VERSION);
			if (rh)
			{
				++reqHandlerCount;
				StringArray supportedContentTypes = rh->getSupportedContentTypes();
				BLOCXX_LOG_INFO(m_Logger, Format("CIMOM loaded request handler from file: %1",
					libName));

				ReqHandlerDataRef rqData(new ReqHandlerData);
				rqData->rqIFCRef = rh;
				for (StringArray::const_iterator iter = supportedContentTypes.begin();
					  iter != supportedContentTypes.end(); iter++)
				{
					MutexLock ml(m_reqHandlersLock);
					m_reqHandlers[(*iter)] = rqData;
					ml.release();
					BLOCXX_LOG_INFO(m_Logger, Format(
						"CIMOM associating Content-Type %1 with Request Handler %2",
						*iter, libName));
				}
				m_services.push_back(rh);
			}
			else
			{
				BLOCXX_LOG_FATAL_ERROR(m_Logger, Format("CIMOM failed to load request handler from file:"
					" %1", libName));
				OW_THROW(CIMOMEnvironmentException, "Invalid request handler");
			}
		}
	}
	BLOCXX_LOG_INFO(m_Logger, Format("CIMOM: Handling %1 Content-Types from %2 Request Handlers",
		m_reqHandlers.size(), reqHandlerCount));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_loadServices()
{
	const StringArray libPaths = getMultiConfigItem(
		ConfigOpts::SERVICES_PATH_opt, String(OW_DEFAULT_SERVICES_PATH).tokenize(OW_PATHNAME_SEPARATOR), OW_PATHNAME_SEPARATOR);
	for (size_t i = 0; i < libPaths.size(); ++i)
	{
		String libPath(libPaths[i]);
		if (!libPath.endsWith(OW_FILENAME_SEPARATOR))
		{
			libPath += OW_FILENAME_SEPARATOR;
		}
		BLOCXX_LOG_INFO(m_Logger, Format("CIMOM loading services from directory %1",
			libPath));
		StringArray dirEntries;
		if (!FileSystem::getDirectoryContents(libPath, dirEntries))
		{
			BLOCXX_LOG_FATAL_ERROR(m_Logger, Format("CIMOM failed getting the contents of the"
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
					"createService", OW_VERSION);
			if (srv)
			{
				m_services.push_back(srv);
				BLOCXX_LOG_INFO(m_Logger, Format("CIMOM loaded service from file: %1", libName));
			}
			else
			{
				BLOCXX_LOG_FATAL_ERROR(m_Logger, Format("CIMOM failed to load service from library: %1",
					libName));
				OW_THROW(CIMOMEnvironmentException, "Invalid service");
			}
		}
	}
	BLOCXX_LOG_INFO(m_Logger, Format("CIMOM: Number of services loaded: %1",
		m_services.size()));
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
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

RepositoryIFCRef authorizingRepository(
	RepositoryIFCRef const & repository, AuthorizerIFCRef const & authorizer
)
{
	AuthorizerIFC * auth1 = authorizer->clone();
	RepositoryIFCRef rep1(auth1);
	auth1->setSubRepositoryIFC(repository);
	return RepositoryIFCRef(
		new SharedLibraryRepository(
			SharedLibraryRepositoryIFCRef(authorizer.getLibRef(), rep1)));
}

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createLogger()
{
	using namespace ConfigOpts;
	Array<LogAppenderRef> appenders;

	StringArray additionalLogs = getMultiConfigItem(ADDITIONAL_LOGS_opt, StringArray(), " \t");

	// this also gets set if owcimomd is run with -d
	bool debugFlag = getConfigItem(DEBUGFLAG_opt, OW_DEFAULT_DEBUGFLAG).equalsIgnoreCase("true");
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
			if (logMainLevel.equalsIgnoreCase(Logger::STR_DEBUG3_CATEGORY))
			{
				logMainCategories = String(Logger::STR_DEBUG3_CATEGORY)
					+ " " + String(Logger::STR_DEBUG2_CATEGORY)
					+ " " + String(Logger::STR_DEBUG_CATEGORY)
					+ " " + String(Logger::STR_INFO_CATEGORY)
					+ " " + String(Logger::STR_ERROR_CATEGORY)
					+ " " + String(Logger::STR_FATAL_CATEGORY);
			}
			else if (logMainLevel.equalsIgnoreCase(Logger::STR_DEBUG2_CATEGORY))
			{
				logMainCategories = String(Logger::STR_DEBUG2_CATEGORY)
					+ " " + String(Logger::STR_DEBUG_CATEGORY)
					+ " " + String(Logger::STR_INFO_CATEGORY)
					+ " " + String(Logger::STR_ERROR_CATEGORY)
					+ " " + String(Logger::STR_FATAL_CATEGORY);
			}
			else if (logMainLevel.equalsIgnoreCase(Logger::STR_DEBUG_CATEGORY))
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
				logMainCategories = Logger::STR_FATAL_CATEGORY;
			}
		}
		String logMainFormat = getConfigItem(Format(LOG_1_FORMAT_opt, logName), OW_DEFAULT_LOG_1_FORMAT);

		appenders.push_back(LogAppender::createLogAppender(logName, logMainComponents.tokenize(), logMainCategories.tokenize(),
			logMainFormat, logMainType, getAppenderConfig(*m_configItems)));
	}

	LogAppender::setDefaultLogAppender(new MultiAppender(appenders));

	m_Logger = Logger(COMPONENT_NAME);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_loadConfigItemsFromFile(const String& filename)
{
	BLOCXX_LOG_DEBUG(m_Logger, "\nUsing config file: " + filename);
	ConfigFile::loadConfigFile(filename, *m_configItems);

	// Now load in additional config files
	StringArray configDirs = ConfigFile::getMultiConfigItem(*m_configItems,
		ConfigOpts::ADDITIONAL_CONFIG_FILES_DIRS_opt,
		String(OW_DEFAULT_ADDITIONAL_CONFIG_FILES_DIRS).tokenize(OW_PATHNAME_SEPARATOR),
		OW_PATHNAME_SEPARATOR);
	for (size_t i = 0; i < configDirs.size(); ++i)
	{
		BLOCXX_LOG_DEBUG(m_Logger, Format("Searching %1 for additional config files (*.conf)", configDirs[i]));
		String const & dir = configDirs[i];
		StringArray dir_entries;
		bool ok = FileSystem::getDirectoryContents(dir, dir_entries);
		if (!ok)
		{
			OW_THROW(ConfigException, Format("Unable to read additional config directory: %1", dir).c_str());
		}
		for (size_t j = 0; j < dir_entries.size(); ++j)
		{
			String const & fname = dir_entries[j];
			if (fname.endsWith(".conf"))
			{
				String confPath = dir + "/" + fname;
				BLOCXX_LOG_DEBUG(m_Logger, Format("Loading additional config items from file: %1", confPath));
				ConfigFile::loadConfigFile(confPath, *m_configItems);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMOMEnvironment::authenticate(String &userName, const String &info,
	String &details, OperationContext& context) const
{
	{
		MutexLock l(m_stateGuard);
		if (!isInitialized(m_state))
		{
			return false;
		}
	}
	MutexLock ml(m_monitor);
	if (!m_authManager)
	{
		return false;
	}
	return m_authManager->authenticate(userName, info, details, context);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMOMEnvironment::getConfigItem(const String &name, const String& defRetVal) const
{
	return ConfigFile::getConfigItem(*m_configItems, name, defRetVal);
}

//////////////////////////////////////////////////////////////////////////////
StringArray
CIMOMEnvironment::getMultiConfigItem(const String &itemName,
	const StringArray& defRetVal, const char* tokenizeSeparator) const
{
	return ConfigFile::getMultiConfigItem(*m_configItems, itemName, defRetVal, tokenizeSeparator);
}

//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
CIMOMEnvironment::getWQLFilterCIMOMHandle(const CIMInstance& inst, OperationContext& context) const
{
	BLOCXX_LOG_DEBUG3(m_Logger, Format("CIMOMEnvironment::getWQLFilterCIMOMHandle(). context.getOperationId() = %1", context.getOperationId()));
	{
		MutexLock l(m_stateGuard);
		if (!isLoaded(m_state))
		{
			OW_THROW(CIMOMEnvironmentException, "CIMOMEnvironment::getWQLFilterCIMOMHandle() called when state is not initialized");
		}
	}
	BLOCXX_ASSERT(m_cimServer);
	return CIMOMHandleIFCRef(new LocalCIMOMHandle(
		const_cast<CIMOMEnvironment *>(this),
		RepositoryIFCRef(new WQLFilterRep(inst, m_cimServer)), context, LocalCIMOMHandle::E_NONE));
}

//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
CIMOMEnvironment::getCIMOMHandle(OperationContext& context,
	EBypassProvidersFlag bypassProviders) const
{
	return getCIMOMHandle(context, E_SEND_INDICATIONS, bypassProviders);
}

//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
CIMOMEnvironment::getLockedCIMOMHandle(OperationContext& context, EInitialLockFlag initialLock) const
{
	return getCIMOMHandle(context, E_SEND_INDICATIONS, E_USE_PROVIDERS, initialLock);
}

namespace
{
	LocalCIMOMHandle::EInitialLockFlag convertInitialLockFlag(ServiceEnvironmentIFC::EInitialLockFlag initialLock)
	{
		switch (initialLock)
		{
			case ServiceEnvironmentIFC::E_NONE:
				return LocalCIMOMHandle::E_NONE;
			case ServiceEnvironmentIFC::E_READ:
				return LocalCIMOMHandle::E_READ;
			case ServiceEnvironmentIFC::E_WRITE:
				return LocalCIMOMHandle::E_WRITE;
		}
		return LocalCIMOMHandle::E_NONE;
	}
}

//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
CIMOMEnvironment::getCIMOMHandle(OperationContext& context,
	ESendIndicationsFlag sendIndications,
	EBypassProvidersFlag bypassProviders,
	EInitialLockFlag initialLock) const
{
	BLOCXX_LOG_DEBUG3(m_Logger, Format("CIMOMEnvironment::getCIMOMHandle(). context.getOperationId() = %1", context.getOperationId()));
	{
		MutexLock l(m_stateGuard);
		if (!isLoaded(m_state))
		{
			OW_THROW(CIMOMEnvironmentException, "CIMOMEnvironment::getCIMOMHandle() called when state is not loaded.");
		}
	}
	MutexLock ml(m_monitor);
	BLOCXX_ASSERT(m_cimServer);

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

	if (sendIndications == E_SEND_INDICATIONS && m_indicationServer && !m_indicationsDisabled)
	{
		SharedLibraryRepositoryIFCRef irl = _getIndicationRepLayer(rref);
		if (irl)
		{
			rref = RepositoryIFCRef(new SharedLibraryRepository(irl));
		}
	}
	if (m_authorizer)
	{
		rref = authorizingRepository(rref, m_authorizer);
	}

	return CIMOMHandleIFCRef(new LocalCIMOMHandle(const_cast<CIMOMEnvironment*>(this), rref, context, convertInitialLockFlag(initialLock)));
}
//////////////////////////////////////////////////////////////////////////////
WQLIFCRef
CIMOMEnvironment::getWQLRef() const
{
	{
		MutexLock l(m_stateGuard);
		if (!isLoaded(m_state))
		{
			OW_THROW(CIMOMEnvironmentException, "CIMOMEnvironment::getWQLRef() called when state is not loaded");
		}
	}
	MutexLock ml(m_monitor);
	if (!m_wqlLib)
	{
		String libname = getConfigItem(ConfigOpts::WQL_LIB_opt, OW_DEFAULT_WQL_LIB);
		BLOCXX_LOG_DEBUG(m_Logger, Format("CIMOM loading wql library %1", libname));
		SharedLibraryLoaderRef sll =
			SharedLibraryLoader::createSharedLibraryLoader();
		m_wqlLib = sll->loadSharedLibrary(libname);
		if (!m_wqlLib)
		{
			BLOCXX_LOG_ERROR(m_Logger, Format("CIMOM Failed to load WQL Libary: %1", libname));
			return WQLIFCRef();
		}
	}
	return  WQLIFCRef(m_wqlLib, SafeLibCreate<WQLIFC>::create(
		m_wqlLib, "createWQL", OW_VERSION));
}
//////////////////////////////////////////////////////////////////////////////
SharedLibraryRepositoryIFCRef
CIMOMEnvironment::_getIndicationRepLayer(const RepositoryIFCRef& rref) const
{
	SharedLibraryRepositoryIFCRef retref;
	if (!m_indicationRepLayerDisabled)
	{
		MutexLock ml(m_indicationLock);
		if (!m_indicationRepLayerLib)
		{
			const String libPath = getConfigItem(ConfigOpts::OWLIBDIR_opt, OW_DEFAULT_OWLIBDIR) + OW_FILENAME_SEPARATOR;
			const String libBase = "libowindicationreplayer";
			String libname = libPath + libBase + OW_SHAREDLIB_EXTENSION;
			BLOCXX_LOG_DEBUG(m_Logger, Format("CIMOM loading indication libary %1",
				libname));
			SharedLibraryLoaderRef sll =
				SharedLibraryLoader::createSharedLibraryLoader();

			if (!sll)
			{
				m_indicationRepLayerDisabled = true;
				BLOCXX_LOG_FATAL_ERROR(m_Logger, Format("CIMOM failed to create SharedLibraryLoader"
					" library %1", libname));
				return retref;
			}
			m_indicationRepLayerLib = sll->loadSharedLibrary(libname);
			if (!m_indicationRepLayerLib)
			{
				m_indicationRepLayerDisabled = true;
				BLOCXX_LOG_FATAL_ERROR(m_Logger, Format("CIMOM failed to load indication rep layer"
					" library %1", libname));
				return retref;
			}
		}
		IndicationRepLayer* pirep =
			SafeLibCreate<IndicationRepLayer>::create(
				m_indicationRepLayerLib, "createIndicationRepLayer", OW_VERSION);
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
	BLOCXX_ASSERT(!m_authorizer);
	String libname = getConfigItem(ConfigOpts::AUTHORIZATION_LIB_opt);

	// no authorization requested
	if (libname.empty())
	{
		return;
	}

	BLOCXX_LOG_DEBUG(m_Logger, Format("CIMOM loading authorization libary %1",
					libname));
	SharedLibraryLoaderRef sll =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!sll)
	{
		String msg = Format("CIMOM failed to create SharedLibraryLoader."
							" library %1", libname);
		BLOCXX_LOG_FATAL_ERROR(m_Logger, msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	SharedLibraryRef authorizerLib = sll->loadSharedLibrary(libname);
	if (!authorizerLib)
	{
		String msg = Format("CIMOM failed to load authorization"
							" library %1", libname);
		BLOCXX_LOG_FATAL_ERROR(m_Logger, msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	AuthorizerIFC* p =
		SafeLibCreate<AuthorizerIFC>::create(
			authorizerLib, "createAuthorizer", OW_VERSION);
	if (!p)
	{
		String msg = Format("CIMOM failed to load authorization"
							" library %1", libname);
		BLOCXX_LOG_FATAL_ERROR(m_Logger, msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	m_authorizer = AuthorizerIFCRef(authorizerLib,
									AuthorizerIFCRef::element_type(p));

	m_services.push_back(m_authorizer);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createAuthorizerManager()
{
	// m_authorizerManager should actually be a valid AuthorizerManager
	// already, but it doesn't have a authorizer loaded yet.
	// It is also already added to the m_services array.

	String libname = getConfigItem(ConfigOpts::AUTHORIZATION2_LIB_opt);

	// no authorization requested
	if (libname.empty())
	{
		return;
	}

	BLOCXX_LOG_DEBUG(m_Logger, Format("CIMOM loading authorization libary %1", libname));

	SharedLibraryLoaderRef sll =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!sll)
	{
		String msg = Format("CIMOM failed to create SharedLibraryLoader."
			" library %1", libname);
		BLOCXX_LOG_FATAL_ERROR(m_Logger, msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	SharedLibraryRef authorizerLib = sll->loadSharedLibrary(libname);
	if (!authorizerLib)
	{
		String msg = Format("CIMOM failed to load authorization"
			" library %1", libname);
		BLOCXX_LOG_FATAL_ERROR(m_Logger, msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}
	Authorizer2IFC* p =
		SafeLibCreate<Authorizer2IFC>::create(
			authorizerLib, "createAuthorizer2", OW_VERSION);
	if (!p)
	{
		String msg = Format("CIMOM failed to load authorization"
			" library %1", libname);
		BLOCXX_LOG_FATAL_ERROR(m_Logger, msg);
		OW_THROW(CIMOMEnvironmentException, msg.c_str());
	}

	m_authorizerManager->setAuthorizer(
		Authorizer2IFCRef(authorizerLib,Authorizer2IFCRef::element_type(p)));
}
//////////////////////////////////////////////////////////////////////////////
RequestHandlerIFCRef
CIMOMEnvironment::getRequestHandler(const String &id) const
{
	RequestHandlerIFCRef ref;
	{
		MutexLock l(m_stateGuard);
		if (!isInitialized(m_state))
		{
			return ref;
		}
	}
	MutexLock ml(m_reqHandlersLock);
	ReqHandlerMap::iterator iter =
			m_reqHandlers.find(id);
	if (iter != m_reqHandlers.end())
	{
		ref = RequestHandlerIFCRef(iter->second->rqIFCRef.getLibRef(),
			iter->second->rqIFCRef->clone());
		BLOCXX_LOG_DEBUG3(m_Logger, Format("handling request for content type %1", id));
	}
	return ref;
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
	ConfigFile::setConfigItem(*m_configItems, item, value,
		overwritePrevious == E_OVERWRITE_PREVIOUS ? ConfigFile::E_OVERWRITE_PREVIOUS : ConfigFile::E_PRESERVE_PREVIOUS);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::runSelectEngine() const
{
	BLOCXX_ASSERT(m_selectables.size() == m_selectableCallbacks.size());
	SelectEngine engine;
	// Insure the signal pipe is at the front of the select list
	engine.addSelectableObject(Platform::getSigSelectable(),
		SelectableCallbackIFCRef(new SelectEngineStopper(engine)),
		SelectableCallbackIFC::E_READ_EVENT);

	for (size_t i = 0; i < m_selectables.size(); ++i)
	{
		engine.addSelectableObject(m_selectables[i]->getSelectObj(), m_selectableCallbacks[i], SelectableCallbackIFC::E_ACCEPT_EVENT);
	}
	engine.go(Timeout::infinite);
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
	BLOCXX_LOG_DEBUG3(m_Logger, "CIMOMEnvironment::exportIndication");
	if (m_indicationServer && !m_indicationsDisabled)
	{
		BLOCXX_LOG_DEBUG3(m_Logger, "CIMOMEnvironment::exportIndication - calling indication server");
		m_indicationServer->processIndication(instance, instNS);
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
RepositoryIFCRef
CIMOMEnvironment::getAuthorizingRepository() const
{
	if (m_authorizer)
	{
		return authorizingRepository(m_cimRepository, m_authorizer);
	}
	else
	{
		return m_cimRepository;
	}
}
//////////////////////////////////////////////////////////////////////////////
AuthorizerManagerRef
CIMOMEnvironment::getAuthorizerManager() const
{
	return m_authorizerManager;
}

//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::unloadProviders()
{
	m_providerManager->unloadProviders(createProvEnvRef(this));
}

namespace
{

//////////////////////////////////////////////////////////////////////////////
struct Node
{
	Node(const String& name_, size_t index_ = size_t(~0))
		: name(name_)
		, index(index_)
	{}

	String name;
	size_t index;
};

//////////////////////////////////////////////////////////////////////////////
bool operator!=(const Node& x, const Node& y)
{
	return x.name != y.name;
}

//////////////////////////////////////////////////////////////////////////////
bool operator<(const Node& x, const Node& y)
{
	return x.name < y.name;
}

//////////////////////////////////////////////////////////////////////////////
Node INVALID_NODE()
{
	return Node("", size_t(~0));
}

//////////////////////////////////////////////////////////////////////////////
class ServiceDependencyGraph
{
public:
	// returns false if serviceName has already been inserted, true otherwise
	bool addNode(const String& serviceName, size_t index);
	// returns false if serviceName already has a dependency on dependentServiceName, true otherwise
	// precondition: a node for serviceName has already been added via addNode()
	bool addDependency(const String& serviceName, const String& dependentServiceName);
	Node findIndependentNode() const;
	void removeNode(const String& serviceName);
	bool empty() const;
	Array<Node> getNodes() const;

private:
	typedef std::map<Node, std::set<String> > deps_t;
	deps_t m_deps;
};

//////////////////////////////////////////////////////////////////////////////
bool
ServiceDependencyGraph::addNode(const String& serviceName, size_t index)
{
	return m_deps.insert(std::make_pair(Node(serviceName, index), deps_t::mapped_type())).second;
}

//////////////////////////////////////////////////////////////////////////////
bool
ServiceDependencyGraph::addDependency(const String& serviceName, const String& dependentServiceName)
{
	return m_deps.find(serviceName)->second.insert(dependentServiceName).second;
}

//////////////////////////////////////////////////////////////////////////////
Node
ServiceDependencyGraph::findIndependentNode() const
{
	for (deps_t::const_iterator nodeiter(m_deps.begin()); nodeiter != m_deps.end(); ++nodeiter)
	{
		if (nodeiter->second.empty())
		{
			return nodeiter->first;
		}
	}

	// didn't find any :-(
	return INVALID_NODE();
}

//////////////////////////////////////////////////////////////////////////////
void
ServiceDependencyGraph::removeNode(const String& serviceName)
{
	// remove it from all dependency lists
	for (deps_t::iterator nodeiter(m_deps.begin()); nodeiter != m_deps.end(); ++nodeiter)
	{
		nodeiter->second.erase(serviceName);
	}
	m_deps.erase(serviceName);
}

//////////////////////////////////////////////////////////////////////////////
bool
ServiceDependencyGraph::empty() const
{
	return m_deps.empty();
}

//////////////////////////////////////////////////////////////////////////////
Array<Node>
ServiceDependencyGraph::getNodes() const
{
	Array<Node> rv;
	rv.reserve(m_deps.size());
	for (deps_t::const_iterator nodeiter(m_deps.begin()); nodeiter != m_deps.end(); ++nodeiter)
	{
		rv.push_back(nodeiter->first);
	}
	return rv;
}

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_sortServicesForDependencies()
{
	// All services can specify a name and dependencies. If a service has an empty name, it can't be specified
	// as a dependency, and our algorithm requires that each service has a name, so if any have an empty name,
	// they'll just be put at the beginning of the list.

	// We need to make sure that a service is initialized before any other services which depend on it.
	// The depedencies reported by the services make a graph.  If it's not a DAG (i.e. contains cycles), we
	// can't turn it into a list of what to do, and we'll detect that and throw an exception. Doing a topological
	// sort on the graph will yield the order we need.
	//
	// The process is quite simple conceptually:
	// while (the graph has a node with no antecedents)
	//    remove one such node from the graph and add it to the list
	//
	// if (the graph is not empty)
	//    the graph contains a cycle
	// else
	//    success

	Array<ServiceIFCRef> sortedServices;

	// first build the graph
	ServiceDependencyGraph depGraph;
	// step 1 insert all the nodes and handle the no-names.
	for (size_t i = 0; i < m_services.size(); ++i)
	{
		String name = m_services[i]->getName();
		if (name == "")
		{
			// no name == no depedency tracking, just do it at the beginning.
			sortedServices.push_back(m_services[i]);
			BLOCXX_LOG_DEBUG3(m_Logger, "Found service with no name, adding to sortedServices");
		}
		else
		{
			BLOCXX_LOG_DEBUG3(m_Logger, Format("Adding node for service %1", name));
			if (!depGraph.addNode(name, i))
			{
				OW_THROW(CIMOMEnvironmentException, Format("Invalid: 2 services with the same name: %1", name).c_str());
			}

		}
	}

	// step 2 insert all the dependencies
	for (size_t i = 0; i < m_services.size(); ++i)
	{
		String name = m_services[i]->getName();
		if (name != "")
		{
			StringArray deps(m_services[i]->getDependencies());
			for (size_t j = 0; j < deps.size(); ++j)
			{
				BLOCXX_LOG_DEBUG2(m_Logger, Format("Adding dependency for service %1->%2", name, deps[j]));
				if (!depGraph.addDependency(name, deps[j]))
				{
					OW_THROW(CIMOMEnvironmentException, Format("Invalid: service %1 has duplicate dependencies: %2", name, deps[j]).c_str());
				}
			}

			// these are just the opposite direction than the dependencies
			StringArray dependentServices(m_services[i]->getDependentServices());
			for (size_t j = 0; j < dependentServices.size(); ++j)
			{
				BLOCXX_LOG_DEBUG2(m_Logger, Format("Adding dependency for service %1->%2", dependentServices[j], name));
				if (!depGraph.addDependency(dependentServices[j], name))
				{
					OW_THROW(CIMOMEnvironmentException, Format("Invalid: service %1 has duplicate dependencies: %2", dependentServices[j], name).c_str());
				}
			}
		}
	}

	// now do the topological sort
	Node curNode = depGraph.findIndependentNode();
	while (curNode != INVALID_NODE())
	{
		BLOCXX_LOG_DEBUG3(m_Logger, Format("Found service with satisfied dependencies: %1", curNode.name));
		sortedServices.push_back(m_services[curNode.index]);
		depGraph.removeNode(curNode.name);
		curNode = depGraph.findIndependentNode();
	}

	if (!depGraph.empty())
	{
		BLOCXX_LOG_FATAL_ERROR(m_Logger, "Service dependency graph contains a cycle:");
		Array<Node> nodes(depGraph.getNodes());
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			BLOCXX_LOG_FATAL_ERROR(m_Logger, Format("Service: %1", nodes[i].name));
		}
		OW_THROW(CIMOMEnvironmentException, "Service dependency graph contains a cycle");
	}

	m_services = sortedServices;
}

} // end namespace OW_NAMESPACE

