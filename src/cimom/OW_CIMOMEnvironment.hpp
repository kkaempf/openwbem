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
#ifndef OW_CIMOMENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_CIMOMENVIRONMENT_HPP_INCLUDE_GUARD_

#define OW_DAEMON_NAME "owcimomd"

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_LogLevel.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_Map.hpp"
#include "OW_Array.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_DateTime.hpp"

class OW_CIMServer;
class OW_CIMOMHandleIFC;
class OW_RequestHandlerIFC;
class OW_ServiceIFC;
class OW_CIMInstance;
class OW_ProviderManager;
class OW_WQLIFC;
class OW_ACLInfo;
class OW_CIMNameSpace;
class OW_HTTPServer;
class OW_AuthManager;
class OW_Authenticator;
class OW_Logger;
class OW_SelectableIFC;
class OW_SelectableCallbackIFC;
class OW_WQLIFC;
class OW_ProviderManager;
class OW_IndicationRepLayer;
class OW_RepositoryIFC;
class OW_PollingManager;
class OW_IndicationServer;

typedef OW_Reference<OW_SelectableIFC> OW_SelectableIFCRef;
typedef OW_Reference<OW_SelectableCallbackIFC> OW_SelectableCallbackIFCRef;
typedef OW_Reference<OW_Logger> OW_LoggerRef;
typedef OW_Reference<OW_CIMOMHandleIFC> OW_CIMOMHandleIFCRef;
typedef OW_SharedLibraryReference<OW_WQLIFC> OW_WQLIFCRef;
typedef OW_Reference<OW_ProviderManager> OW_ProviderManagerRef;
typedef OW_Reference<OW_RepositoryIFC> OW_RepositoryIFCRef;
typedef OW_Reference<OW_AuthManager> OW_AuthManagerRef;
typedef OW_Reference<OW_PollingManager> OW_PollingManagerRef;
typedef OW_SharedLibraryReference<OW_IndicationServer> OW_IndicationServerRef;
typedef OW_SharedLibraryReference<OW_ServiceIFC> OW_ServiceIFCRef;
typedef OW_SharedLibraryReference<OW_RequestHandlerIFC> OW_RequestHandlerIFCRef;
typedef OW_SharedLibraryReference<OW_RepositoryIFC>
	OW_SharedLibraryRepositoryIFCRef;

class OW_CIMOMEnvironment;
typedef OW_Reference<OW_CIMOMEnvironment> OW_CIMOMEnvironmentRef;

class OW_CIMOMEnvironment : public OW_ServiceEnvironmentIFC
{
public:
	OW_CIMOMEnvironment();
	~OW_CIMOMEnvironment();
	void init();

	virtual OW_Bool authenticate(OW_String &userName, const OW_String &info,
		OW_String &details);

	virtual OW_String getConfigItem(const OW_String &name) const;

	virtual OW_CIMOMHandleIFCRef getCIMOMHandle(const OW_String &username,
		const OW_Bool doIndications=false, const OW_Bool bypassProviders = false);

	OW_CIMOMHandleIFCRef getCIMOMHandle(const OW_ACLInfo& aclinfo,
		const OW_Bool doIndications=false, const OW_Bool bypassProviders = false,
		const OW_Bool noLocking = false);

	OW_CIMOMHandleIFCRef getCIMOMHandle();

	OW_CIMOMHandleIFCRef getRepositoryCIMOMHandle();

	OW_CIMOMHandleIFCRef getWQLFilterCIMOMHandle(const OW_CIMInstance& inst,
		const OW_ACLInfo& aclInfo);

	OW_WQLIFCRef getWQLRef();

	virtual OW_RequestHandlerIFCRef getRequestHandler(
		const OW_String &id);

	virtual OW_LoggerRef getLogger() const;

	OW_IndicationServerRef getIndicationServer() const;

	OW_PollingManagerRef getPollingManager() const;

	void clearConfigItems();

	virtual void setConfigItem(const OW_String &item, const OW_String &value,
		OW_Bool overwritePrevious=true);

	virtual void addSelectable(OW_SelectableIFCRef obj,
		OW_SelectableCallbackIFCRef cb);

	virtual void removeSelectable(OW_SelectableIFCRef obj,
		OW_SelectableCallbackIFCRef cb);

	void unloadProviders();

	void startServices();
	void shutdown();
	OW_ProviderManagerRef getProviderManager();
	void runSelectEngine();
	void logCustInfo(const OW_String& s) const;
	void logDebug(const OW_String& s) const;
	void logError(const OW_String& s) const;
	void exportIndication(const OW_CIMInstance& instance,
		const OW_String& instNS);
	void unloadReqHandlers();

	// do not use this variable unless absolutely necessary!
	static OW_CIMOMEnvironmentRef g_cimomEnvironment;
private:

	void _createLogger();
	void _loadConfigItemsFromFile(const OW_String& filename);
	void _loadRequestHandlers();
	void _loadServices();
	void _createAuthManager();
	void _createPollingManager();
	void _createIndicationServer();
	OW_SharedLibraryRepositoryIFCRef _getIndicationRepLayer(const OW_RepositoryIFCRef& rref);
	void _clearSelectables();

	// Types

	struct ReqHandlerData
	{
		OW_DateTime dt;
		OW_RequestHandlerIFCRef rqIFCRef;
		OW_String filename;
	};

	typedef OW_Map<OW_String, OW_String> ConfigMap;
	typedef OW_Reference<ConfigMap> ConfigMapRef;
	typedef OW_SortedVectorMap<OW_String, ReqHandlerData> ReqHandlerMap;

	mutable OW_Mutex m_monitor;
	OW_RepositoryIFCRef m_cimRepository;
	OW_RepositoryIFCRef m_cimServer;
	OW_AuthManagerRef m_authManager;
	OW_LoggerRef m_Logger;
	ConfigMapRef m_configItems;
	OW_ProviderManagerRef m_providerManager;
	OW_SharedLibraryRef m_wqlLib;
	OW_SharedLibraryRef m_indicationRepLayerLib;
	OW_PollingManagerRef m_pollingManager;
	OW_IndicationServerRef m_indicationServer;
	OW_Bool m_indicationsDisabled;
	OW_Array<OW_SelectableIFCRef> m_selectables;
	OW_Array<OW_SelectableCallbackIFCRef> m_selectableCallbacks;
	OW_Array<OW_ServiceIFCRef> m_services;
	ReqHandlerMap m_reqHandlers;
	mutable OW_Mutex m_reqHandlersLock;
	mutable OW_Mutex m_indicationLock;
	OW_Bool m_indicationRepLayerDisabled;
	mutable OW_Mutex m_selectableLock;
	
	bool m_running;
	OW_Mutex m_runningGuard;
};

#endif


