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
#include "OW_CIMOMEnvironment.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ConfigException.hpp"
#include "OW_Format.hpp"
#include "OW_FileSystem.hpp"
#include "OW_SafeLibCreate.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_CIMServer.hpp"
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
#include "OW_UnloaderProvider.hpp"
#include "OW_NameSpaceProvider.hpp"

#include <fstream>
#include <iostream>

using std::cerr;
using std::endl;
using std::ifstream;

namespace
{
class OW_EnvSelectable : public OW_SelectableCallbackIFC
{
public:
	OW_EnvSelectable(OW_SelectEngine& engine)
	: OW_SelectableCallbackIFC()
	, m_engine(engine)
	{}

protected:
	virtual void doSelected(OW_SelectableIFCRef& selectedObject)
	{
		(void)selectedObject;
		m_engine.stop();
	}

private:
	OW_SelectEngine& m_engine;
};


	class CIMOMProviderEnvironment : public OW_ProviderEnvironmentIFC
	{
	public:

		CIMOMProviderEnvironment(const OW_CIMOMEnvironment* pCenv)
			: m_pCenv(pCenv)
		{}

		virtual OW_String getConfigItem(const OW_String &name) const
		{
			return m_pCenv->getConfigItem(name);
		}

		virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return OW_CIMOMHandleIFCRef();
		}
		
		virtual OW_LoggerRef getLogger() const
		{
			return m_pCenv->getLogger();
		}

	private:
		const OW_CIMOMEnvironment* m_pCenv;
	};

	OW_ProviderEnvironmentIFCRef createProvEnvRef(const OW_CIMOMEnvironment* pcenv)
	{
		return OW_ProviderEnvironmentIFCRef(new CIMOMProviderEnvironment(pcenv));
	}
} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMEnvironment::OW_CIMOMEnvironment()
	: OW_ServiceEnvironmentIFC()
	, m_monitor()
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
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMEnvironment::~OW_CIMOMEnvironment()
{
	try
	{
		shutdown();
	}
	catch(OW_Exception& e)
	{
		cerr << e << endl;
	}
	m_configItems = 0;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::init()
{
	OW_MutexLock ml(m_monitor);

	_clearSelectables();
	setConfigItem(OW_ConfigOpts::CONFIG_FILE_opt, DEFAULT_CONFIG_FILE, false);

	_loadConfigItemsFromFile(getConfigItem(OW_ConfigOpts::CONFIG_FILE_opt));

	setConfigItem(OW_ConfigOpts::LIBEXEC_DIR_opt, DEFAULT_LIBEXEC_DIR, false);
	setConfigItem(OW_ConfigOpts::OWLIB_DIR_opt, DEFAULT_OWLIB_DIR, false);

	// Location of log file
	setConfigItem(OW_ConfigOpts::LOG_LOCATION_opt, DEFAULT_LOG_FILE, false);

	// Location for data files
	setConfigItem(OW_ConfigOpts::DATA_DIR_opt,
		DEFAULT_DATA_DIR, false);

	// Provider interface location
	setConfigItem(OW_ConfigOpts::PROVIDER_IFC_LIBS_opt,
		DEFAULT_IFC_LIBS, false);

	// This config item should be handled in the C++ provider interface
	// C++ provider interface - provider location
	setConfigItem(OW_ConfigOpts::CPPIFC_PROV_LOC_opt,
		DEFAULT_CPP_PROVIDER_LOCATION, false);

	// Authorization module
	setConfigItem(OW_ConfigOpts::AUTH_MOD_opt,
		DEFAULT_AUTH_MOD, false);

	// This config item should be handled in the simple auth module
	setConfigItem(OW_ConfigOpts::SIMPLE_AUTH_FILE_opt,
		DEFAULT_SIMPLE_PASSWD_FILE, false);

	setConfigItem(OW_ConfigOpts::DISABLE_INDICATIONS_opt,
		DEFAULT_DISABLE_INDICATIONS, false);
	
	setConfigItem(OW_ConfigOpts::WQL_LIB_opt,
		DEFAULT_WQL_LIB, false);

	setConfigItem(OW_ConfigOpts::REQ_HANDLER_TTL_opt,
		DEFAULT_REQ_HANDLER_TTL, false);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::unloadProviders()
{
	m_providerManager->unloadProviders(createProvEnvRef(this));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::startServices()
{
	OW_MutexLock ml(m_monitor);

	_createLogger();
	
	OW_CIMOMEnvironmentRef eref(this, true);
	m_providerManager = OW_ProviderManagerRef(new OW_ProviderManager);
	m_providerManager->init(OW_ProviderIFCLoader::createProviderIFCLoader(
		eref));

	// Add the unloader provider to the provider manager
	m_providerManager->addCIMOMProvider(OW_CppProviderBaseIFCRef(
		OW_SharedLibraryRef(), new OW_UnloaderProvider(this)));

	// Add the name space provider to the provider manager
	m_providerManager->addCIMOMProvider(OW_CIMServer::NAMESPACE_PROVIDER,
		OW_CppProviderBaseIFCRef(OW_SharedLibraryRef(),
		new OW_NameSpaceProvider));

	m_cimServer = OW_RepositoryIFCRef(new OW_CIMServer(eref,
		m_providerManager));
	m_cimServer->open(getConfigItem(OW_ConfigOpts::DATA_DIR_opt));

	_createAuthManager();
	_loadRequestHandlers();
	_loadServices();

	_createPollingManager();
	_createIndicationServer();

	m_running = true;

	for(size_t i = 0; i < m_services.size(); i++)
	{
		m_services[i]->startService();
	}

	if (!getConfigItem(OW_ConfigOpts::SINGLE_THREAD_opt).equalsIgnoreCase("true"))
	{
		// Start up the polling manager
		m_Logger->logDebug("CIMOM starting Polling Manager");
		m_pollingManager->start();
		OW_Thread::yield();

		// Start up the indication server
		m_Logger->logDebug("CIMOM starting IndicationServer");
		m_indicationServer->init(OW_CIMOMEnvironmentRef(this, true));
		m_indicationServer->start();
		OW_Thread::yield();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::shutdown()
{
	logDebug("CIMOM Environment shutting down...");

	m_running = false;

	OW_MutexLock ml(m_monitor);

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
		m_pollingManager->join();
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
	m_indicationRepLayerLib = 0;
	if(m_indicationServer)
	{
		try
		{
			m_indicationServer->shutdown();
		}
		catch (...)
		{
		}
		m_indicationServer->join();
		m_indicationServer.setNull();
	}

	// Delete the authentication manager
	m_authManager = 0;

	// Shutdown the cim server and delete it
	if(m_cimServer)
	{
		try
		{
			m_cimServer->close();
		}
		catch (...)
		{
		}
		m_cimServer = 0;
	}

	// Delete the provider manager
	m_providerManager = 0;

	logDebug("CIMOM Environment has shut down");

	m_Logger = 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_ProviderManagerRef
OW_CIMOMEnvironment::getProviderManager()
{
	if (!m_running)
	{
		return OW_ProviderManagerRef();
	}
	OW_MutexLock ml(m_monitor);
	OW_ASSERT(m_providerManager);
	return m_providerManager;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_createAuthManager()
{
	OW_ServiceEnvironmentIFCRef env(this, true); // no-delete reference
	m_authManager = OW_AuthManagerRef(new OW_AuthManager);
	m_authManager->init(env);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_createPollingManager()
{
	m_pollingManager = OW_PollingManagerRef(new OW_PollingManager(
		OW_CIMOMEnvironmentRef(this, true)));

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_createIndicationServer()
{
	// Determine if user has disabled indication exportation
	m_indicationsDisabled = getConfigItem(
		OW_ConfigOpts::DISABLE_INDICATIONS_opt).equalsIgnoreCase("true");

	if (!m_indicationsDisabled)
	{
		// load the indication server library
		OW_String indicationLib = getConfigItem(OW_ConfigOpts::OWLIB_DIR_opt);
		if(!indicationLib.endsWith("/"))
		{
			indicationLib += "/";
		}

		indicationLib += "libowindicationserver.so";

		m_indicationServer = OW_SafeLibCreate<OW_IndicationServer>::loadAndCreateObject(
				indicationLib, "createIndicationServer", getLogger());

		if (!m_indicationServer)
		{
			m_Logger->logError(format("CIMOM Failed to load indication server"
				" from library %1. Indication are currently DISABLED!",
				indicationLib));

			m_indicationsDisabled = true;
			return;
		}

	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_loadRequestHandlers()
{
	m_reqHandlers.clear();
	OW_String libPath = getConfigItem(
		OW_ConfigOpts::CIMOM_REQUEST_HANDLER_LOCATION_opt);

	if(libPath.empty())
	{
		libPath = DEFAULT_CIMOM_REQHANDLER_LOCATION;
	}

	if(!libPath.endsWith("/"))
	{
		libPath += "/";
	}

	m_Logger->logCustInfo(format("CIMOM loading request handlers from"
		" directory %1", libPath));

	OW_StringArray dirEntries;
	if(!OW_FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		m_Logger->logError(format("CIMOM failed geeting the contents of the"
			" request handler directory: %1", libPath));
		return;
	}

	int reqHandlerCount = 0;
	for(size_t i = 0; i < dirEntries.size(); i++)
	{
		if(!dirEntries[i].endsWith(".so"))
		{
			continue;
		}

		OW_String libName = libPath;
		libName += dirEntries[i];

		OW_RequestHandlerIFCRef rh =
			OW_SafeLibCreate<OW_RequestHandlerIFC>::loadAndCreateObject(
				libName, "createRequestHandler", getLogger());

		if(rh)
		{
			++reqHandlerCount;
			rh->setEnvironment(OW_ServiceEnvironmentIFCRef(this, true));
			OW_StringArray supportedContentTypes = rh->getSupportedContentTypes();
			logCustInfo(format("CIMOM loaded request handler from file: %1",
				libName));

			for (OW_StringArray::const_iterator iter = supportedContentTypes.begin();
				  iter != supportedContentTypes.end(); iter++)
			{
				ReqHandlerData rqData;
				rqData.filename = libName;

				OW_MutexLock ml(m_reqHandlersLock);
				m_reqHandlers[(*iter)] = rqData;
				ml.release();
				logCustInfo(format(
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

	m_Logger->logCustInfo(format("CIMOM: Handling %1 Content-Types from %2 Request Handlers",
		m_reqHandlers.size(), reqHandlerCount));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_loadServices()
{
	m_services.clear();
	OW_String libPath = getConfigItem(
		OW_ConfigOpts::CIMOM_SERVICES_LOCATION_opt);

	if(libPath.empty())
	{
		libPath = DEFAULT_CIMOM_SERVICES_LOCATION;
	}

	if(!libPath.endsWith("/"))
	{
		libPath += "/";
	}

	m_Logger->logCustInfo(format("CIMOM loading services from directory %1",
		libPath));

	OW_StringArray dirEntries;
	if(!OW_FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		m_Logger->logError(format("CIMOM failed geeting the contents of the"
			" services directory: %1", libPath));
		return;
	}

	for(size_t i = 0; i < dirEntries.size(); i++)
	{
		if(!dirEntries[i].endsWith(".so"))
		{
			continue;
		}

		OW_String libName = libPath;
		libName += dirEntries[i];

		OW_ServiceIFCRef srv =
			OW_SafeLibCreate<OW_ServiceIFC>::loadAndCreateObject(libName,
				"createService", getLogger());

		if(srv)
		{
			srv->setServiceEnvironment(OW_ServiceEnvironmentIFCRef(this, true));
			m_services.append(srv);
			logCustInfo(format("CIMOM loaded service from file: %1", libName));
		}
		else
		{
			logError(format("CIMOM failed to load service from library: %1",
				libName));
		}
	}

	logCustInfo(format("CIMOM: Number of services loaded: %1",
		m_services.size()));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_createLogger()
{
	if(!m_Logger)
	{
		OW_Bool debugFlag = getConfigItem(
			OW_ConfigOpts::OW_DEBUG_opt).equalsIgnoreCase("true");

		m_Logger = OW_LoggerRef(OW_Logger::createLogger(getConfigItem(
			OW_ConfigOpts::LOG_LOCATION_opt), debugFlag));
	}

	m_Logger->setLogLevel(getConfigItem(OW_ConfigOpts::LOG_LEVEL_opt));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_loadConfigItemsFromFile(const OW_String& filename)
{
	ifstream file(filename.c_str(), std::ios::in);

	if (!file)
	{
		file.close();
		OW_THROW(OW_ConfigException, format("Unable to read config"
					" file: %1", filename).c_str());
	}

	// We don't have a logger at this point
    std::cout << "CIMOM reading config file: " << filename << std::endl;

	OW_String line;
	int lineNum = 0;
	while(file)
	{
		lineNum++;
		line = OW_String::getLine(file);
		if (!line.empty())
		{
			// If comment line, ignore
			if (line[0] == '#' || line[0] == ';')
			{
				continue;
			}

			size_t idx = line.indexOf('=');
			if (idx > 0)
			{
				if(idx + 1 < line.length())
				{
					OW_String itemValue = line.substring(idx + 1).trim();
					if(!itemValue.empty())
					{
						setConfigItem(line.substring(0, idx).trim(), itemValue,
							false);
					}
				}
			}
			else
			{
				file.close();
				OW_THROW(OW_ConfigException, format("Error in config file:"
					" %1 at line %2.\n  Line is %3", filename, lineNum,
					line).c_str());
			}
		}
	}
	file.close();
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMOMEnvironment::authenticate(OW_String &userName, const OW_String &info,
	OW_String &details)
{
	if (!m_running)
	{
		return false;
	}
	OW_MutexLock ml(m_monitor);
	OW_ASSERT(m_authManager);
	return m_authManager->authenticate(userName, info, details);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMOMEnvironment::getConfigItem(const OW_String &name) const
{
	ConfigMap::const_iterator i = m_configItems->find(name);

	if(i != m_configItems->end())
	{
		return i->second;
	}
	else
	{
		return OW_String();
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMHandleIFCRef
OW_CIMOMEnvironment::getWQLFilterCIMOMHandle(const OW_CIMInstance& inst,
        const OW_ACLInfo& aclInfo)
{
	if (!m_running)
	{
		return OW_CIMOMHandleIFCRef();
	}
	OW_MutexLock ml(m_monitor);
	OW_ASSERT(m_cimServer);
	OW_CIMServer* psvr = (OW_CIMServer*)m_cimServer.getPtr();
	return OW_CIMOMHandleIFCRef(new OW_LocalCIMOMHandle(
		OW_CIMOMEnvironmentRef(this, true),
		OW_RepositoryIFCRef(new OW_WQLFilterRep(inst, psvr)), aclInfo));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMHandleIFCRef
OW_CIMOMEnvironment::getCIMOMHandle(const OW_ACLInfo& aclInfo,
	OW_Bool doIndications)
{
	if (!m_running)
	{
		return OW_CIMOMHandleIFCRef();
	}

	OW_MutexLock ml(m_monitor);
	OW_ASSERT(m_cimServer);


	OW_CIMOMEnvironmentRef eref(this, true);

	if(doIndications
	   && m_indicationServer
	   && !m_indicationsDisabled)
	{
		OW_SharedLibraryRepositoryIFCRef irl = _getIndicationRepLayer();

		if(irl)
		{
			OW_RepositoryIFCRef rref(new OW_SharedLibraryRepository(irl));
			return OW_CIMOMHandleIFCRef(new OW_LocalCIMOMHandle(eref, rref,
				aclInfo));
		}
	}

	return OW_CIMOMHandleIFCRef(new OW_LocalCIMOMHandle(eref, m_cimServer,
		aclInfo));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMHandleIFCRef
OW_CIMOMEnvironment::getCIMOMHandle(const OW_String &username,
	const OW_Bool doIndications)
{
	return getCIMOMHandle(OW_ACLInfo(username), doIndications);
}

//////////////////////////////////////////////////////////////////////////////
OW_WQLIFCRef
OW_CIMOMEnvironment::getWQLRef()
{
	if (!m_running)
	{
		return OW_WQLIFCRef();
	}

	OW_MutexLock ml(m_monitor);

    if (!m_wqlLib)
    {
        OW_String libname = getConfigItem(OW_ConfigOpts::WQL_LIB_opt);

		logDebug(format("CIMOM loading wql library %1", libname));

		OW_SharedLibraryLoaderRef sll =
			OW_SharedLibraryLoader::createSharedLibraryLoader();

		m_wqlLib = sll->loadSharedLibrary(libname, m_Logger);
		if(!m_wqlLib)
		{
			logError(format("CIMOM Failed to load WQL Libary: %1", libname));
			return OW_WQLIFCRef();
		}
    }

	return  OW_WQLIFCRef(m_wqlLib, OW_SafeLibCreate<OW_WQLIFC>::create(
		m_wqlLib, "createWQL", m_Logger));
}

//////////////////////////////////////////////////////////////////////////////
OW_SharedLibraryRepositoryIFCRef
OW_CIMOMEnvironment::_getIndicationRepLayer()
{
	OW_SharedLibraryRepositoryIFCRef retref;

	if(!m_indicationRepLayerDisabled)
	{
		OW_MutexLock ml(m_indicationLock);
		if (!m_indicationRepLayerLib)
		{
			OW_String libname = getConfigItem(OW_ConfigOpts::OWLIB_DIR_opt);
			libname += "/libowindicationreplayer.so";

			m_Logger->logDebug(format("CIMOM loading indication libary %1",
				libname));

			OW_SharedLibraryLoaderRef sll =
				OW_SharedLibraryLoader::createSharedLibraryLoader();
			if(!sll)
			{
				m_indicationRepLayerDisabled = true;
				logError(format("CIMOM failed to create SharedLibraryLoader"
					" library %1", libname));
				return retref;
			}

			m_indicationRepLayerLib = sll->loadSharedLibrary(libname, m_Logger);
			if(!m_indicationRepLayerLib)
			{
				m_indicationRepLayerDisabled = true;
				logError(format("CIMOM failed to load indication rep layer"
					" library %1", libname));
				return retref;
			}
		}

		OW_IndicationRepLayer* pirep =
			OW_SafeLibCreate<OW_IndicationRepLayer>::create(
				m_indicationRepLayerLib, "createIndicationRepLayer", m_Logger);

		if(pirep)
		{
			pirep->setCIMServer(m_cimServer.getPtr());
			retref = OW_SharedLibraryRepositoryIFCRef(m_indicationRepLayerLib,
				OW_RepositoryIFCRef(pirep));
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
OW_RequestHandlerIFCRef
OW_CIMOMEnvironment::getRequestHandler(const OW_String &id)
{
	OW_RequestHandlerIFCRef ref;
	if (!m_running)
	{
		return ref;
	}


	OW_MutexLock ml(m_reqHandlersLock);
	ReqHandlerMap::iterator iter =
			m_reqHandlers.find(id);
	if (iter != m_reqHandlers.end())
	{
		if (iter->second.rqIFCRef.isNull())
		{
			iter->second.rqIFCRef =
				OW_SafeLibCreate<OW_RequestHandlerIFC>::loadAndCreateObject(
					iter->second.filename, "createRequestHandler", getLogger());
		}
		if (iter->second.rqIFCRef)
		{
			ref = OW_RequestHandlerIFCRef(iter->second.rqIFCRef.getLibRef(),
				iter->second.rqIFCRef->clone());
			iter->second.dt.setToCurrent();
			ref->setEnvironment(OW_ServiceEnvironmentIFCRef(
				const_cast<OW_CIMOMEnvironment*>(this), true));
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
OW_CIMOMEnvironment::unloadReqHandlers()
{
	//logDebug("Running unloadReqHandlers()");
	OW_Int32 ttl;
	try
	{
		ttl = getConfigItem(OW_ConfigOpts::REQ_HANDLER_TTL_opt).toInt32();
	}
	catch (const OW_StringConversionException&)
	{
		logError(format("Invalid value (%1) for %2 config item.  Using default.",
            getConfigItem(OW_ConfigOpts::REQ_HANDLER_TTL_opt),
			OW_ConfigOpts::REQ_HANDLER_TTL_opt));
		ttl = OW_String(DEFAULT_REQ_HANDLER_TTL).toInt32();
	}
	if (ttl < 0)
	{
		logDebug("Non-Positive TTL for Request Handlers: OpenWBEM will not unload request handlers.");
		return;
	}
	OW_DateTime dt;
	dt.setToCurrent();
	OW_MutexLock ml(m_reqHandlersLock);
	for (ReqHandlerMap::iterator iter = m_reqHandlers.begin();
		  iter != m_reqHandlers.end(); ++iter)
	{
		if (iter->second.rqIFCRef)
		{
			OW_DateTime rqDT = iter->second.dt;
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
OW_LoggerRef
OW_CIMOMEnvironment::getLogger() const
{
	OW_ASSERT(m_Logger);
	return m_Logger;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::clearConfigItems()
{
	m_configItems->clear();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::setConfigItem(const OW_String &item,
	const OW_String &value, OW_Bool overwritePrevious)
{
	ConfigMap::iterator it = m_configItems->find(item);
	if(it == m_configItems->end() || overwritePrevious)
	{
		(*m_configItems.getPtr())[item] = value;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::runSelectEngine()
{
	OW_ASSERT(m_selectables.size() == m_selectableCallbacks.size());

	OW_SelectEngine engine;

	// Insure the signal pipe is at the front of the select list
	engine.addSelectableObject(OW_Platform::getSigSelectable(),
		OW_SelectableCallbackIFCRef(new OW_EnvSelectable(engine)));
	
	for(size_t i = 0; i < m_selectables.size(); ++i)
	{
		engine.addSelectableObject(m_selectables[i], m_selectableCallbacks[i]);
	}

	engine.go();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_clearSelectables()
{
	OW_MutexLock ml(m_selectableLock);
	m_selectables.clear();
	m_selectableCallbacks.clear();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::addSelectable(OW_SelectableIFCRef obj,
	OW_SelectableCallbackIFCRef cb)
{
	OW_MutexLock ml(m_selectableLock);
	m_selectables.push_back(obj);
	m_selectableCallbacks.push_back(cb);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::removeSelectable(OW_SelectableIFCRef obj,
	OW_SelectableCallbackIFCRef cb)
{
	OW_MutexLock ml(m_selectableLock);
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
OW_CIMOMEnvironment::exportIndication(const OW_CIMInstance& instance,
	const OW_String& instNS)
{
	logDebug("OW_CIMOMEnvironment::exportIndication");
	if(m_indicationServer && !m_indicationsDisabled)
	{
		logDebug("OW_CIMOMEnvironment::exportIndication - calling indication"
			" server");

		m_indicationServer->processIndication(instance, instNS);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::logCustInfo(const OW_String& s) const
{
	if (m_Logger)
	{
		m_Logger->logCustInfo(s);
	}
	else
	{
        std::cout << s << std::endl;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::logDebug(const OW_String& s) const
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
OW_CIMOMEnvironment::logError(const OW_String& s) const
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






