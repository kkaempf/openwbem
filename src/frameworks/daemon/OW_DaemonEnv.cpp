/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
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
#include "OW_DaemonEnv.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Logger.hpp"
#include "OW_ConfigException.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_Platform.hpp"
#include "OW_Thread.hpp"
#include "OW_ServerSocket.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_SafeLibCreate.hpp"
#include "OW_CIMProtocolIFC.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_HTTPClient.hpp"

#include <fstream>
#include <iostream>
#include <algorithm> // for std::remove

using std::cerr;
using std::endl;
using std::ifstream;

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::init()
{
	setConfigItem(OW_ConfigOpts::CONFIG_FILE_opt, OW_DEFAULT_CONFIG_FILE, false);

	loadConfigItemsFromFile(getConfigItem( OW_ConfigOpts::CONFIG_FILE_opt ));

	setConfigItem(OW_ConfigOpts::LIBEXEC_DIR_opt, OW_DEFAULT_OWLIBEXEC_DIR, false);

	setConfigItem(OW_ConfigOpts::OWLIB_DIR_opt, OW_DEFAULT_OWLIB_DIR, false);

	// HTTP Port number
	setConfigItem(OW_ConfigOpts::HTTP_PORT_opt, OW_DEFAULT_HTTP_PORT, false);

	// HTTPS Port number
	setConfigItem(OW_ConfigOpts::HTTPS_PORT_opt, OW_DEFAULT_HTTPS_PORT, false);

	// Location of log file
	setConfigItem(OW_ConfigOpts::LOG_LOCATION_opt, OW_DEFAULT_LOG_FILE, false);

	// Allow anonymous connections to cimom
	setConfigItem(OW_ConfigOpts::ALLOW_ANONYMOUS_opt,
		OW_DEFAULT_ALLOW_ANONYMOUS, false);

	// Max allowed concurrent connections
	setConfigItem(OW_ConfigOpts::MAX_CONNECTIONS_opt,
		OW_DEFAULT_MAX_CONNECTIONS, false);

	setConfigItem(OW_ConfigOpts::ENABLE_DEFLATE_opt,
		OW_DEFAULT_ENABLE_DEFLATE, false);

	createLogger();

	// set up socket dump files
	OW_String dumpPrefix = getConfigItem(OW_ConfigOpts::DUMP_SOCKET_IO_opt);
	if (!dumpPrefix.empty())
	{
		OW_SocketBaseImpl::setDumpFiles(dumpPrefix + "/owSockDumpIn",
			dumpPrefix + "/owSockDumpOut");
		logDebug(format("Logging socket dump to files: %1 and %2",
			dumpPrefix + "/owSockDumpIn", dumpPrefix + "/owSockDumpOut"));
	}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::startServices()
{
	createAuthManager();
	createRequestHandler();
	createService();
	startService();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::shutdown()
{
	deleteService();
	deleteRequestHandler();
	deleteAuthManager();
	deleteLogger();
	clearConfigItems();
	deleteConfigMap();
	m_selectables.clear();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMHandleIFCRef
OW_DaemonEnv::getCIMOMHandle(const OW_String& username, ESendIndicationsFlag doIndications, EBypassProvidersFlag bypassProviders)
{
	(void)username;
	(void)doIndications;
	(void)bypassProviders;
	OW_CIMProtocolIFCRef client;
	client = new OW_HTTPClient("ipc://localhost/owbinary");

	OW_CIMOMHandleIFCRef chRef;
	chRef = new OW_BinaryCIMOMHandle(client);

	return chRef;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::createLogger()
{
	OW_ASSERT(m_Logger == 0);
	m_Logger = OW_Logger::createLogger(OW_DaemonEnv::getConfigItem(
		OW_ConfigOpts::LOG_LOCATION_opt),
		getConfigItem( OW_ConfigOpts::OW_DEBUG_opt ).
		equalsIgnoreCase( "true" ));

	setLogLevel(OW_DaemonEnv::getConfigItem(OW_ConfigOpts::LOG_LEVEL_opt));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::logDebug(const OW_String& message)
{
	if (m_Logger)
	{
		m_Logger->logDebug(message);
	}
	else
	{
		if (OW_DaemonEnv::getConfigItem(OW_ConfigOpts::OW_DEBUG_opt).equalsIgnoreCase("true"))
		{
			cerr << message << endl;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::logCustInfo(const OW_String& message)
{
	if (m_Logger)
	{
		m_Logger->logCustInfo(message);
	}
	else
	{
		if (OW_DaemonEnv::getConfigItem(OW_ConfigOpts::OW_DEBUG_opt).equalsIgnoreCase("true"))
		{
			cerr << message << endl;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::logError(const OW_String& message)
{
	if (m_Logger)
	{
		m_Logger->logError(message);
	}
	else
	{
		cerr << message << endl;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::setLogLevel(const OW_LogLevel level)
{
	OW_ASSERT(m_Logger);
	m_Logger->setLogLevel(level);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::setLogLevel(const OW_String& level)
{
	OW_ASSERT(m_Logger);
	m_Logger->setLogLevel(level);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::deleteLogger()
{
	if(m_Logger)
	{
		m_Logger = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::deleteConfigMap()
{
	m_configItems.clear();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::setConfigItem( const OW_String& item,
		const OW_String& value, EOverwritePreviousFlag overwritePrevious )
{
	if (overwritePrevious || getConfigItem(item) == "")
		(m_configItems)[item] = value;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_DaemonEnv::getConfigItem( const OW_String& item ) const
{
	config_t::const_iterator i = m_configItems.find(item);
	if(i != m_configItems.end())
	{
		return i->second;
	}
	else
	{
		return OW_String();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::loadConfigItemsFromFile( const OW_String& filename )
{
	ifstream file(filename.c_str(), std::ios::in);
	if (!file)
	{
		file.close();
		OW_THROW(OW_ConfigException, format("Unable to read config"
					" file: %1", filename).c_str());
	}

	logDebug(format("Reading config file: %1", filename));

	OW_String line;
	int lineNum = 0;
	while(file)
	{
		lineNum++;
		line = OW_String::getLine(file);
		if (!line.empty())
		{
			if (line[0] == '#' || line[0] == ';')
			{
				continue;
			}
			size_t idx = line.indexOf('=');
			if (idx != OW_String::npos)
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
							" %1 at line %2.\n  Line is %3",
							filename, lineNum, line).c_str());
			}
		}
	}
	file.close();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::clearConfigItems()
{
	m_configItems.clear();
}


//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::addSelectable(OW_SelectableIFCRef selectable,
		OW_SelectableCallbackIFCRef cb)
{
	m_selectables.push_back(std::make_pair(selectable, cb));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::removeSelectable(OW_SelectableIFCRef obj,
	OW_SelectableCallbackIFCRef cb)
{
	m_selectables.erase(std::remove(m_selectables.begin(), m_selectables.end(),
		std::make_pair(obj, cb)), m_selectables.end());
}


//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::runSelectEngine()
{
	OW_SelectEngine engine;

	// Insure signal pipe is the first in the select list
	engine.addSelectableObject(OW_Platform::getSigSelectable(),
		OW_SelectableCallbackIFCRef(new OW_SelectEngineStopper(engine)));
	
	for (size_t i = 0; i < m_selectables.size(); ++i)
	{
		engine.addSelectableObject(m_selectables[i].first, m_selectables[i].second);
	}
	engine.go();
}

//////////////////////////////////////////////////////////////////////////////
OW_LoggerRef
OW_DaemonEnv::getLogger() const
{
	return m_Logger;
}


//////////////////////////////////////////////////////////////////////////////
bool
OW_DaemonEnv::authenticate(OW_String &userName, const OW_String &info, OW_String &details)
{
	return m_authManager->authenticate(userName,info,details);
}

//////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFCRef
OW_DaemonEnv::getRequestHandler(const OW_String&)
{
	return OW_RequestHandlerIFCRef(m_requestHandler.getLibRef(),
		m_requestHandler->clone());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::createAuthManager()
{
	OW_ASSERT( m_authManager == 0 );

	OW_ServiceEnvironmentIFCRef env(this, true); // no-delete reference
	// Setup authentication
	m_authManager = new OW_AuthManager;
	m_authManager->init(env);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::createService()
{
	OW_ASSERT(m_service == 0);
	OW_String libname = getConfigItem(OW_ConfigOpts::SERVICE_LIB_PATH_opt);
	m_service = OW_SafeLibCreate<OW_ServiceIFC>::loadAndCreateObject(libname,
		"createService", getLogger());
	if(m_service)
	{
		OW_ServiceEnvironmentIFCRef env(this, true); // no-delete reference
		m_service->setServiceEnvironment(env);
	}
	else
	{
		m_Logger->logError(format("%1 failed to load service from library %2",
			OW_DAEMON_NAME, libname));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::createRequestHandler()
{
	OW_ASSERT( m_requestHandler == 0 );

	OW_String libname = getConfigItem(OW_ConfigOpts::REQUEST_HANDLER_LIB_PATH_opt);
	m_requestHandler =
		OW_SafeLibCreate<OW_RequestHandlerIFC>::loadAndCreateObject(libname,
			"createRequestHandler", getLogger());
	if(!m_requestHandler)
	{
		m_Logger->logError(format("%1 failed to load request handler from"
			" library %2", OW_DAEMON_NAME, libname));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::startService()
{
	OW_ASSERT( m_service );
	try
	{
		m_service->startService();
	}
	catch (OW_SocketException &e)
	{
		logError(format("Unable to bind to port: %1", e));
		OW_Platform::pushSig(OW_Platform::SHUTDOWN);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::deleteAuthManager()
{
	m_authManager = 0;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::deleteService()
{
	if(m_service)
	{
		m_service->shutdown();
	}
	m_service.setNull();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DaemonEnv::deleteRequestHandler()
{
	m_requestHandler.setNull();
}




