/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
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
#include "OW_ConfigException.hpp"
#include "OW_Format.hpp"
#include "OW_FileSystem.hpp"
#include "OW_SafeLibCreate.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_CIMServer.hpp"
#include "OW_CIMRepository2.hpp"
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

#include <iostream>

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(CIMOMEnvironment)
OW_DEFINE_EXCEPTION(CIMOMEnvironment)

using std::cerr;
using std::endl;

// static global
CIMOMEnvironmentRef CIMOMEnvironment::g_cimomEnvironment;

namespace
{
	class CIMOMProviderEnvironment : public ProviderEnvironmentIFC
	{
	public:
		CIMOMProviderEnvironment(const CIMOMEnvironment* pCenv)
			: m_pCenv(pCenv)
			, m_context("")
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
		, m_context("")
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
	{
		MutexLock ml(m_monitor);
		_createLogger();
		m_providerManager = ProviderManagerRef(new ProviderManager);
		m_providerManager->load(ProviderIFCLoader::createProviderIFCLoader(
			g_cimomEnvironment));
		m_cimRepository = RepositoryIFCRef(new CIMRepository2(g_cimomEnvironment));
		m_cimRepository->open(getConfigItem(ConfigOpts::DATA_DIR_opt));
		m_cimServer = RepositoryIFCRef(new CIMServer(g_cimomEnvironment,
			m_providerManager, m_cimRepository));
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
	m_providerManager->init(ProviderEnvironmentIFCRef(
		new ProviderEnvironmentServiceEnvironmentWrapper(this)));
	for(size_t i = 0; i < m_services.size(); i++)
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
	// Shutdown the polling manager
	if(m_pollingManager)
	{
		try
		{
			m_pollingManager->shutdown();
		}
		catch (...)
		{
		}
		m_pollingManager = 0;
	}
	
	// Clear selectable objects
	try
	{
		_clearSelectables();
	}
	catch(...)
	{
	}
	// Shutdown any loaded services
	// For now. We need to unload these in the opposite order that
	// they were loaded.
	for(int i = int(m_services.size())-1; i >= 0; i--)
	{
		try
		{
			m_services[i]->shutdown();
		}
		catch (...)
		{
		}
		m_services[i].setNull();
	}
	m_services.clear();
	// Unload all request handlers
	m_reqHandlers.clear();
	// Unload the wql library if loaded
	m_wqlLib = 0;
	// Shutdown indication processing
	if(m_indicationServer)
	{
		try
		{
			m_indicationServer->shutdown();
		}
		catch (...)
		{
		}
		m_indicationServer.setNull();
		m_indicationRepLayerLib = 0;
	}
	// Delete the authentication manager
	m_authManager = 0;
	// Shutdown the cim server and delete it
	if(m_cimServer)
	{
		m_cimServer = 0;
	}
	// Shutdown the cim repository and delete it
	if(m_cimRepository)
	{
		try
		{
			m_cimRepository->close();
		}
		catch (...)
		{
		}
		m_cimRepository = 0;
	}
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
		if(!indicationLib.endsWith("/"))
		{
			indicationLib += "/";
		}
		indicationLib += "libowindicationserver"OW_SHAREDLIB_EXTENSION;
		m_indicationServer = SafeLibCreate<IndicationServer>::loadAndCreateObject(
				indicationLib, "createIndicationServer", getLogger());
		if (!m_indicationServer)
		{
			logError(format("CIMOM Failed to load indication server"
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
	if(!libPath.endsWith("/"))
	{
		libPath += "/";
	}
	logInfo(format("CIMOM loading request handlers from"
		" directory %1", libPath));
	StringArray dirEntries;
	if(!FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		logFatalError(format("CIMOM failed geeting the contents of the"
			" request handler directory: %1", libPath));
		OW_THROW(CIMOMEnvironmentException, "No RequestHandlers");
	}
	int reqHandlerCount = 0;
	for(size_t i = 0; i < dirEntries.size(); i++)
	{
		if(!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
		{
			continue;
		}
		String libName = libPath;
		libName += dirEntries[i];
		RequestHandlerIFCRef rh =
			SafeLibCreate<RequestHandlerIFC>::loadAndCreateObject(
				libName, "createRequestHandler", getLogger());
		if(rh)
		{
			++reqHandlerCount;
			rh->setEnvironment(g_cimomEnvironment);
			StringArray supportedContentTypes = rh->getSupportedContentTypes();
			logInfo(format("CIMOM loaded request handler from file: %1",
				libName));
			for (StringArray::const_iterator iter = supportedContentTypes.begin();
				  iter != supportedContentTypes.end(); iter++)
			{
				ReqHandlerData rqData;
				rqData.filename = libName;
				MutexLock ml(m_reqHandlersLock);
				m_reqHandlers[(*iter)] = rqData;
				ml.release();
				logInfo(format(
					"CIMOM associating Content-Type %1 with Request Handler %2",
					*iter, libName));
			}
		}
		else
		{
			logError(format("CIMOM failed to load request handler from file:"
				" %1", libName));
		}
	}
	logInfo(format("CIMOM: Handling %1 Content-Types from %2 Request Handlers",
		m_reqHandlers.size(), reqHandlerCount));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_loadServices()
{
	m_services.clear();
	String libPath = getConfigItem(
		ConfigOpts::CIMOM_SERVICES_LOCATION_opt, OW_DEFAULT_CIMOM_SERVICES_LOCATION);
	if(!libPath.endsWith("/"))
	{
		libPath += "/";
	}
	logInfo(format("CIMOM loading services from directory %1",
		libPath));
	StringArray dirEntries;
	if(!FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		logFatalError(format("CIMOM failed geeting the contents of the"
			" services directory: %1", libPath));
		OW_THROW(CIMOMEnvironmentException, "No Services");
	}
	for(size_t i = 0; i < dirEntries.size(); i++)
	{
		if(!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
		{
			continue;
		}
		String libName = libPath;
		libName += dirEntries[i];
		ServiceIFCRef srv =
			SafeLibCreate<ServiceIFC>::loadAndCreateObject(libName,
				"createService", getLogger());
		if(srv)
		{
			// save it first so if setServiceEnvironment throws it won't get
			// unloaded until later.
			m_services.append(srv);
			srv->setServiceEnvironment(g_cimomEnvironment);
			logInfo(format("CIMOM loaded service from file: %1", libName));
		}
		else
		{
			logError(format("CIMOM failed to load service from library: %1",
				libName));
		}
	}
	logInfo(format("CIMOM: Number of services loaded: %1",
		m_services.size()));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::_createLogger()
{
	bool debugFlag = getConfigItem(
		ConfigOpts::DEBUG_opt).equalsIgnoreCase("true");
	m_Logger = LoggerRef(Logger::createLogger(getConfigItem(
		ConfigOpts::LOG_LOCATION_opt), debugFlag));
	m_Logger->setLogLevel(getConfigItem(ConfigOpts::LOG_LEVEL_opt));
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
	String &details)
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
	return m_authManager->authenticate(userName, info, details);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMOMEnvironment::getConfigItem(const String &name, const String& defRetVal) const
{
	ConfigMap::const_iterator i = m_configItems->find(name);
	if(i != m_configItems->end())
	{
		return i->second;
	}
	else
	{
		return defRetVal;
	}
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
	RepositoryIFCRef rref;
	if (bypassProviders)
	{
		rref = m_cimRepository;
	}
	else
	{
		rref = m_cimServer;
	}
	if(doIndications
	   && m_indicationServer
	   && !m_indicationsDisabled)
	{
		SharedLibraryRepositoryIFCRef irl = _getIndicationRepLayer(rref);
		if(irl)
		{
			RepositoryIFCRef rref2(new SharedLibraryRepository(irl));
			return CIMOMHandleIFCRef(new LocalCIMOMHandle(g_cimomEnvironment, rref2,
				context, locking == E_LOCKING ? LocalCIMOMHandle::E_LOCKING : LocalCIMOMHandle::E_NO_LOCKING));
		}
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
		logDebug(format("CIMOM loading wql library %1", libname));
		SharedLibraryLoaderRef sll =
			SharedLibraryLoader::createSharedLibraryLoader();
		m_wqlLib = sll->loadSharedLibrary(libname, m_Logger);
		if(!m_wqlLib)
		{
			logError(format("CIMOM Failed to load WQL Libary: %1", libname));
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
	if(!m_indicationRepLayerDisabled)
	{
		MutexLock ml(m_indicationLock);
		if (!m_indicationRepLayerLib)
		{
			String libname = getConfigItem(ConfigOpts::OWLIB_DIR_opt);
			libname += "/libowindicationreplayer"OW_SHAREDLIB_EXTENSION;
			logDebug(format("CIMOM loading indication libary %1",
				libname));
			SharedLibraryLoaderRef sll =
				SharedLibraryLoader::createSharedLibraryLoader();
			if(!sll)
			{
				m_indicationRepLayerDisabled = true;
				logFatalError(format("CIMOM failed to create SharedLibraryLoader"
					" library %1", libname));
				return retref;
			}
			m_indicationRepLayerLib = sll->loadSharedLibrary(libname, m_Logger);
			if(!m_indicationRepLayerLib)
			{
				m_indicationRepLayerDisabled = true;
				logFatalError(format("CIMOM failed to load indication rep layer"
					" library %1", libname));
				return retref;
			}
		}
		IndicationRepLayer* pirep =
			SafeLibCreate<IndicationRepLayer>::create(
				m_indicationRepLayerLib, "createIndicationRepLayer", m_Logger);
		if(pirep)
		{
			pirep->setCIMServer(rref);
			retref = SharedLibraryRepositoryIFCRef(m_indicationRepLayerLib,
				RepositoryIFCRef(pirep));
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
		if (iter->second.rqIFCRef.isNull())
		{
			iter->second.rqIFCRef =
				SafeLibCreate<RequestHandlerIFC>::loadAndCreateObject(
					iter->second.filename, "createRequestHandler", getLogger());
		}
		if (iter->second.rqIFCRef)
		{
			ref = RequestHandlerIFCRef(iter->second.rqIFCRef.getLibRef(),
				iter->second.rqIFCRef->clone());
			iter->second.dt.setToCurrent();
			ref->setEnvironment(g_cimomEnvironment);
			logDebug(format("Request Handler %1 handling request for content type %2",
				iter->second.filename, id));
		}
		else
		{
			logError(format(
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
		logError(format("Invalid value (%1) for %2 config item.  Using default.",
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
				logDebug(format("Unloaded request handler lib %1 for content type %2",
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
	return m_Logger;
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
	ConfigMap::iterator it = m_configItems->find(item);
	if(it == m_configItems->end() || overwritePrevious)
	{
		(*m_configItems)[item] = value;
	}
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
	
	for(size_t i = 0; i < m_selectables.size(); ++i)
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
CIMOMEnvironment::addSelectable(SelectableIFCRef obj,
	SelectableCallbackIFCRef cb)
{
	MutexLock ml(m_selectableLock);
	m_selectables.push_back(obj);
	m_selectableCallbacks.push_back(cb);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::removeSelectable(SelectableIFCRef obj,
	SelectableCallbackIFCRef cb)
{
	MutexLock ml(m_selectableLock);
	for(size_t i = 0; i < m_selectables.size(); i++)
	{
		if(obj == m_selectables[i]
		   && cb == m_selectableCallbacks[i])
		{
			m_selectables.remove(i);
			m_selectableCallbacks.remove(i);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMEnvironment::exportIndication(const CIMInstance& instance,
	const String& instNS)
{
	logDebug("CIMOMEnvironment::exportIndication");
	if(m_indicationServer && !m_indicationsDisabled)
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
CIMInstanceArray
CIMOMEnvironment::getCommunicationMechanisms() const
{
	return m_communicationMechanisms;
}

} // end namespace OpenWBEM

