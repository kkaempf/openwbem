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
#ifndef OW_CIMOMENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_CIMOMENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_DAEMON_NAME "owcimomd"
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_LogLevel.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_Map.hpp"
#include "OW_Array.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_DateTime.hpp"
#include "OW_Mutex.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_IntrusiveReference.hpp"

namespace OpenWBEM
{

class CIMServer;
class CIMOMHandleIFC;
class RequestHandlerIFC;
class ServiceIFC;
class CIMInstance;
class ProviderManager;
class WQLIFC;
class UserInfo;
class CIMNameSpace;
class HTTPServer;
class AuthManager;
class Authenticator;
class Logger;
class SelectableIFC;
class SelectableCallbackIFC;
class WQLIFC;
class ProviderManager;
class IndicationRepLayer;
class RepositoryIFC;
class PollingManager;
class IndicationServer;
class IndicationRepLayerMediator;
typedef Reference<SelectableIFC> SelectableIFCRef;
typedef Reference<SelectableCallbackIFC> SelectableCallbackIFCRef;
typedef Reference<Logger> LoggerRef;
typedef Reference<CIMOMHandleIFC> CIMOMHandleIFCRef;
typedef SharedLibraryReference<Reference<WQLIFC> > WQLIFCRef;
typedef Reference<ProviderManager> ProviderManagerRef;
typedef IntrusiveReference<RepositoryIFC> RepositoryIFCRef;
typedef Reference<AuthManager> AuthManagerRef;
typedef Reference<PollingManager> PollingManagerRef;
typedef SharedLibraryReference<Reference<IndicationServer> > IndicationServerRef;
typedef SharedLibraryReference<Reference<ServiceIFC> > ServiceIFCRef;
typedef SharedLibraryReference<Reference<RequestHandlerIFC> > RequestHandlerIFCRef;
typedef SharedLibraryReference<RepositoryIFCRef>
	SharedLibraryRepositoryIFCRef;
typedef Reference<IndicationRepLayerMediator> 
	IndicationRepLayerMediatorRef;
class CIMOMEnvironment;
typedef Reference<CIMOMEnvironment> CIMOMEnvironmentRef;
class CIMOMEnvironment : public ServiceEnvironmentIFC
{
public:
	CIMOMEnvironment();
	~CIMOMEnvironment();
	void init();
	virtual bool authenticate(String &userName, const String &info,
		String &details);
	virtual String getConfigItem(const String &name, const String& defRetVal="") const;
	virtual CIMOMHandleIFCRef getCIMOMHandle(const String &username,
		ESendIndicationsFlag doIndications = E_SEND_INDICATIONS, 
		EBypassProvidersFlag bypassProviders = E_USE_PROVIDERS);
	enum ELockingFlag
	{
		E_NO_LOCKING,
		E_LOCKING
	};
	CIMOMHandleIFCRef getCIMOMHandle(const UserInfo& aclinfo,
		ESendIndicationsFlag doIndications = E_SEND_INDICATIONS, 
		EBypassProvidersFlag bypassProviders = E_USE_PROVIDERS,
		ELockingFlag locking = E_LOCKING);
	CIMOMHandleIFCRef getCIMOMHandle();
	CIMOMHandleIFCRef getRepositoryCIMOMHandle();
	CIMOMHandleIFCRef getWQLFilterCIMOMHandle(const CIMInstance& inst,
		const UserInfo& aclInfo);
	WQLIFCRef getWQLRef();
	virtual RequestHandlerIFCRef getRequestHandler(
		const String &id);
	virtual LoggerRef getLogger() const;
	IndicationServerRef getIndicationServer() const;
	PollingManagerRef getPollingManager() const;
	void clearConfigItems();
	virtual void setConfigItem(const String &item, const String &value,
		EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS);
	virtual void addSelectable(SelectableIFCRef obj,
		SelectableCallbackIFCRef cb);
	virtual void removeSelectable(SelectableIFCRef obj,
		SelectableCallbackIFCRef cb);
	void unloadProviders();
	void startServices();
	void shutdown();
	ProviderManagerRef getProviderManager();
	void runSelectEngine();
	void logCustInfo(const String& s) const;
	void logDebug(const String& s) const;
	void logError(const String& s) const;
	void exportIndication(const CIMInstance& instance,
		const String& instNS);
	void unloadReqHandlers();
	IndicationRepLayerMediatorRef getIndicationRepLayerMediator() const;
	RepositoryIFCRef getRepository() const;
	CIMInstanceArray getCommunicationMechanisms() const;
	// do not use this variable unless absolutely necessary!
	static CIMOMEnvironmentRef g_cimomEnvironment;
private:
	void _createLogger();
	void _loadConfigItemsFromFile(const String& filename);
	void _loadRequestHandlers();
	void _loadServices();
	void _createAuthManager();
	void _createPollingManager();
	void _createIndicationServer();
	SharedLibraryRepositoryIFCRef _getIndicationRepLayer(const RepositoryIFCRef& rref);
	void _clearSelectables();
	// Types
	struct ReqHandlerData
	{
		DateTime dt;
		RequestHandlerIFCRef rqIFCRef;
		String filename;
	};
	typedef Map<String, String> ConfigMap;
	typedef Reference<ConfigMap> ConfigMapRef;
	typedef SortedVectorMap<String, ReqHandlerData> ReqHandlerMap;
	mutable Mutex m_monitor;
	RepositoryIFCRef m_cimRepository;
	RepositoryIFCRef m_cimServer;
	AuthManagerRef m_authManager;
	LoggerRef m_Logger;
	ConfigMapRef m_configItems;
	ProviderManagerRef m_providerManager;
	SharedLibraryRef m_wqlLib;
	SharedLibraryRef m_indicationRepLayerLib;
	PollingManagerRef m_pollingManager;
	IndicationServerRef m_indicationServer;
	bool m_indicationsDisabled;
	Array<SelectableIFCRef> m_selectables;
	Array<SelectableCallbackIFCRef> m_selectableCallbacks;
	Array<ServiceIFCRef> m_services;
	ReqHandlerMap m_reqHandlers;
	mutable Mutex m_reqHandlersLock;
	mutable Mutex m_indicationLock;
	bool m_indicationRepLayerDisabled;
	mutable Mutex m_selectableLock;
	
	bool m_running;
	Mutex m_runningGuard;
	IndicationRepLayerMediatorRef m_indicationRepLayerMediatorRef;
	CIMInstanceArray m_communicationMechanisms;
};

} // end namespace OpenWBEM

#endif
