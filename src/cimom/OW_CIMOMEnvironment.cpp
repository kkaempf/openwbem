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
	virtual void doSelected()
	{
		m_engine.stop();
	}

private:
	OW_SelectEngine& m_engine;
};
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
	, m_wqlLib(0)
	, m_indicationServerLib(0)
	, m_indicationRepLayerLib(0)
	, m_pollingManager(0)
	, m_indicationServer(0)
	, m_indicationsDisabled(true)
	, m_selectables()
	, m_selectableCallbacks()
	, m_services()
	, m_reqHandlers()
	, m_indicationLock()
	, m_indicationRepLayerDisabled(false)
	, m_selectableLock()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMEnvironment::~OW_CIMOMEnvironment()
{
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

	_createLogger();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::startServices()
{
	OW_MutexLock ml(m_monitor);

	OW_CIMOMEnvironmentRef eref(this, true);
	m_providerManager = OW_ProviderManagerRef(new OW_ProviderManager);
	m_providerManager->init(OW_ProviderIFCLoader::createProviderIFCLoader(
		eref));

	m_cimServer = OW_RepositoryIFCRef(new OW_CIMServer(eref,
		m_providerManager));
	m_cimServer->open(getConfigItem(OW_ConfigOpts::DATA_DIR_opt));

	_createAuthManager();
	_loadRequestHandlers();
	_loadServices();
	for(size_t i = 0; i < m_services.size(); i++)
	{
		m_services[i].m_obj->startService();
	}

	_createPollingManager();
	_createIndicationServer();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::shutdown()
{
	OW_MutexLock ml(m_monitor);

	// Shutdown indication processing
	m_indicationRepLayerLib = 0;
	if(m_indicationServer)
	{
		m_indicationServer->shutdown();
		m_indicationServer->join();
		m_indicationServer = 0;
	}
	m_indicationServerLib = 0;

	// Shutdown the polling manager
	if(m_pollingManager)
	{
		m_pollingManager->shutdown();
		m_pollingManager->join();
		m_pollingManager = 0;
	}

	// Clear selectable objects
	_clearSelectables();

	// Shutdown any loaded services
	for(size_t i = 0; i < m_services.size(); i++)
	{
		m_services[i].m_obj->shutdown();
	}

	// Unload all services
	m_services.clear();

	// Unload all request handlers
	m_reqHandlers.clear();

	// Unload the wql library if loaded
    m_wqlLib = 0;

	// Delete the authentication manager
	m_authManager = 0;

	// Shutdown the cim server and delete it
	if(m_cimServer)
	{
		m_cimServer->close();
		m_cimServer = 0;
	}

	// Delete the provider manager
	m_providerManager = 0;

	// Delete the loger
	m_Logger = 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_ProviderManagerRef
OW_CIMOMEnvironment::getProviderManager()
{
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

	m_pollingManager->start();
	OW_Thread::yield();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_createIndicationServer()
{
	// Determine if user has disabled indication exportation
	m_indicationsDisabled = getConfigItem(
		OW_ConfigOpts::DISABLE_INDICATIONS_opt).equalsIgnoreCase("true");

	// Note: The indication server is always started up, because it is
	// responsible for the polled providers. If indications are disabled,
	// then the exportIndication method will never actually export an
	// indication.

	if (!m_indicationsDisabled)
	{
		// load the indication server library
		OW_String indicationLib = getConfigItem(OW_ConfigOpts::OWLIB_DIR_opt);
		if(!indicationLib.endsWith("/"))
		{
			indicationLib += "/";
		}

		indicationLib += "libowindicationserver.so";

		std::pair<OW_Reference<OW_IndicationServer>, OW_SharedLibraryRef> rv =
			OW_SafeLibCreate<OW_IndicationServer>::loadAndCreate(indicationLib,
				"createIndicationServer", getLogger());

		m_indicationServer = rv.first;
		m_indicationServerLib = rv.second;

		if (!m_indicationServer || !m_indicationServerLib)
		{
			m_Logger->logError(format("CIMOM Failed to load indication server"
				" from library %1. Indication are currently DISABLED!",
				indicationLib));

			m_indicationServer = 0;
			m_indicationServerLib = 0;
			m_indicationsDisabled = true;
			return;
		}

		// Start up the indication server
		m_Logger->logDebug("CIMOM starting IndicationServer");
		m_indicationServer->init(OW_CIMOMEnvironmentRef(this, true));
		m_indicationServer->start();
		OW_Thread::yield();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_loadRequestHandlers()
{
	m_reqHandlers.clear();
	OW_String libPath = getConfigItem(
		OW_ConfigOpts::CIMOM_REQUEST_HANDLER_LOCATION_opt);

	if(libPath.length() == 0)
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

	for(size_t i = 0; i < dirEntries.size(); i++)
	{
		if(!dirEntries[i].endsWith(".so"))
		{
			continue;
		}

		OW_String libName = libPath;
		libName += dirEntries[i];

		OW_SafeLibCreate<OW_RequestHandlerIFC>::return_type r =
			OW_SafeLibCreate<OW_RequestHandlerIFC>::loadAndCreate(libName,
				"createRequestHandler", getLogger());

		if(r.first)
		{
			ReqHandlerEntry re(r.first, r.second);
			re.m_obj->setEnvironment(OW_ServiceEnvironmentIFCRef(this, true));
			m_reqHandlers.append(re);
			m_Logger->logCustInfo(format("CIMOM loaded request handler from"
				" file: %1", libName));
		}
	}

	m_Logger->logCustInfo(format("CIMOM: Number of request handlers loaded: %1",
		m_reqHandlers.size()));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMOMEnvironment::_loadServices()
{
	m_services.clear();
	OW_String libPath = getConfigItem(
		OW_ConfigOpts::CIMOM_SERVICES_LOCATION_opt);

	if(libPath.length() == 0)
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

		OW_SafeLibCreate<OW_ServiceIFC>::return_type r =
			OW_SafeLibCreate<OW_ServiceIFC>::loadAndCreate(libName,
				"createService", getLogger());

		if(r.first)
		{
			ServiceEntry se(r.first, r.second);
			se.m_obj->setServiceEnvironment(OW_ServiceEnvironmentIFCRef(this,
				true));
			m_services.append(se);

			m_Logger->logCustInfo(format("CIMOM loaded service from file: %1",
				libName));
		}
	}

	m_Logger->logCustInfo(format("CIMOM: Number of services loaded: %1",
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
	cout << "CIMOM reading config file: " << filename << endl;

	OW_String line;
	int lineNum = 0;
	while(file)
	{
		lineNum++;
		line = OW_String::getLine(file);
		if (line.length() > 0)
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
					if(itemValue.length() > 0)
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
OW_CIMOMHandleRef
OW_CIMOMEnvironment::getWQLFilterCIMOMHandle(const OW_CIMInstance& inst,
        const OW_ACLInfo& aclInfo)
{
	OW_MutexLock ml(m_monitor);
	OW_ASSERT(m_cimServer);
	OW_CIMServer* psvr = (OW_CIMServer*)m_cimServer.getPtr();
	return OW_CIMOMHandleRef(new OW_LocalCIMOMHandle(
		OW_CIMOMEnvironmentRef(this, true),
		OW_RepositoryIFCRef(new OW_WQLFilterRep(inst, psvr)), aclInfo));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMHandleRef
OW_CIMOMEnvironment::getCIMOMHandle(const OW_ACLInfo& aclInfo,
	OW_Bool doIndications)
{
	OW_MutexLock ml(m_monitor);
	OW_ASSERT(m_cimServer);

	OW_CIMOMEnvironmentRef eref(this, true);

	if(doIndications
	   && m_indicationServer 
	   && !m_indicationsDisabled)
	{
		OW_Reference<OW_IndicationRepLayer> irl = _getIndicationRepLayer();
		if(irl)
		{
			irl->setCIMServer(m_cimServer.getPtr());
			return OW_CIMOMHandleRef(new OW_LocalCIMOMHandle(eref, irl,
				aclInfo));
		}
	}

	return OW_CIMOMHandleRef(new OW_LocalCIMOMHandle(eref, m_cimServer,
		aclInfo));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMHandleRef
OW_CIMOMEnvironment::getCIMOMHandle(const OW_String &username,
	const OW_Bool doIndications)
{
	return getCIMOMHandle(OW_ACLInfo(username), doIndications);
}

//////////////////////////////////////////////////////////////////////////////
OW_WQLIFCRef
OW_CIMOMEnvironment::getWQLRef()
{
	OW_MutexLock ml(m_monitor);

    if (!m_wqlLib)
    {
        OW_String libname = getConfigItem(OW_ConfigOpts::WQL_LIB_opt);

		logDebug(format("CIMOM loading wql library %1", libname));

        std::pair<OW_WQLIFCRef, OW_SharedLibraryRef> rv =
            OW_SafeLibCreate<OW_WQLIFC>::loadAndCreate(libname, "createWQL",
                getLogger());

        m_wqlLib = rv.second;
        return rv.first;
    }

	OW_WQLIFC* ptr = 0;
	ptr = OW_SafeLibCreate<OW_WQLIFC>::create(m_wqlLib, "createWQL",
		getLogger());
	return OW_WQLIFCRef(ptr);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationRepLayerRef
OW_CIMOMEnvironment::_getIndicationRepLayer()
{
	OW_IndicationRepLayerRef retref(0);

	if(!m_indicationRepLayerDisabled)
	{
		OW_MutexLock ml(m_indicationLock);
		if (!m_indicationRepLayerLib)
		{
			OW_String libname = getConfigItem(OW_ConfigOpts::OWLIB_DIR_opt);
			libname += "/libowindicationreplayer.so";

			m_Logger->logDebug(format("CIMOM loading indication libary %1",
				libname));

			std::pair<OW_Reference<OW_IndicationRepLayer>, OW_SharedLibraryRef>
				rv = OW_SafeLibCreate<OW_IndicationRepLayer>::loadAndCreate(
					libname, "createIndicationRepLayer", getLogger());

			m_indicationRepLayerLib = rv.second;
			if(!rv.first)
			{
				m_indicationRepLayerDisabled = true;
			}

			retref = rv.first;
		}
		else
		{
			OW_IndicationRepLayer* ptr = 0;
			ptr = OW_SafeLibCreate<OW_IndicationRepLayer>::create(
				m_indicationRepLayerLib, "createIndicationRepLayer",
				getLogger());

			retref = OW_IndicationRepLayerRef(ptr);
		}

		if(!retref)
		{
			m_indicationRepLayerDisabled = true;
			m_indicationRepLayerLib = 0;
		}
	}

	return retref;
}

//////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFCRef
OW_CIMOMEnvironment::getRequestHandler(const OW_String &id) const
{
	OW_MutexLock ml(m_monitor);

	OW_RequestHandlerIFCRef ref(0);
	for(size_t i = 0; i < m_reqHandlers.size(); i++)
	{
		if(m_reqHandlers[i].m_obj->getId().equals(id))
		{
			ref = m_reqHandlers[i].m_obj->clone();
			ref->setEnvironment(OW_ServiceEnvironmentIFCRef(
				const_cast<OW_CIMOMEnvironment*>(this), true));
			break;
		}
	}

	return ref;
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
	const OW_CIMNameSpace& instNS)
{
	logDebug("OW_CIMOMEnvironment::exportIndication");
	if(m_indicationServer && !m_indicationsDisabled)
	{
		logDebug("OW_CIMOMEnvironment::exportIndication - calling indication"
			" server");

		m_indicationServer->processIndication(instance, instNS);
	}
}





