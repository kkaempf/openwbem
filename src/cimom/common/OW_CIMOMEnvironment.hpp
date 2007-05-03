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
 */

#ifndef OW_CIMOMENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_CIMOMENVIRONMENT_HPP_INCLUDE_GUARD_
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
#include "OW_Reference.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_HashMap.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_CimomCommonFwd.hpp"
#include "OW_CimomServerFwd.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_Logger.hpp"

namespace OW_NAMESPACE
{

class OW_CIMOMCOMMON_API CIMOMEnvironment : public ServiceEnvironmentIFC
{
public:
	CIMOMEnvironment();
	~CIMOMEnvironment();
	void init();
	virtual bool authenticate(String &userName, const String &info,
		String &details, OperationContext& context) const;
	virtual String getConfigItem(const String &name, const String& defRetVal="") const;
	virtual StringArray getMultiConfigItem(const String &itemName, 
		const StringArray& defRetVal, const char* tokenizeSeparator) const;
	
	// from ServiceEnvironmentIFC
	/**
	 * WARNING: The return value is valid only for the lifetime of context.
	 */
	CIMOMHandleIFCRef getCIMOMHandle(OperationContext& context,
		EBypassProvidersFlag bypassProviders = E_USE_PROVIDERS) const;

	enum ESendIndicationsFlag
	{
		E_DONT_SEND_INDICATIONS,
		E_SEND_INDICATIONS
	};

	/**
	 * WARNING: The return value is valid only for the lifetime of context.
	 */
	CIMOMHandleIFCRef getCIMOMHandle(OperationContext& context,
		ESendIndicationsFlag sendIndications,
		EBypassProvidersFlag bypassProviders = E_USE_PROVIDERS) const;

	/**
	 * WARNING: The return value is valid only for the lifetime of context.
	 */
	//virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle(OperationContext& context) const;

	/**
	 * WARNING: The return value is valid only for the lifetime of context.
	 */
	CIMOMHandleIFCRef getWQLFilterCIMOMHandle(const CIMInstance& inst,
		OperationContext& context) const;

	virtual WQLIFCRef getWQLRef() const;
	virtual RequestHandlerIFCRef getRequestHandler(const String &id) const;
	IndicationServerRef getIndicationServer() const;
	PollingManagerRef getPollingManager() const;
	void clearConfigItems();
	virtual void setConfigItem(const String &item, const String &value,
		EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS);
	virtual void addSelectable(const SelectableIFCRef& obj,
		const SelectableCallbackIFCRef& cb);
	virtual void removeSelectable(const SelectableIFCRef& obj);
	void unloadProviders();
	void startServices();
	void shutdown();
	ProviderManagerRef getProviderManager() const;
	void runSelectEngine() const;
	void exportIndication(const CIMInstance& instance,
		const String& instNS);
	IndicationRepLayerMediatorRef getIndicationRepLayerMediator() const;
	RepositoryIFCRef getRepository() const;
	RepositoryIFCRef getAuthorizingRepository() const;
	AuthorizerManagerRef getAuthorizerManager() const;


	// do not use this unless absolutely necessary!
	static CIMOMEnvironmentRef& instance();

private:
	void _createLogger();
	void _loadConfigItemsFromFile(const String& filename);
	void _loadRequestHandlers();
	void _loadServices();
	void _createAuthManager();
	void _createPollingManager();
	void _createIndicationServer();
	SharedLibraryRepositoryIFCRef _getIndicationRepLayer(const RepositoryIFCRef& rref) const;
	void _clearSelectables();
	void _loadAuthorizer();
	void _createAuthorizerManager();
	void _sortServicesForDependencies();

	// Types
	typedef ConfigFile::ConfigMap ConfigMap;
	typedef Reference<ConfigMap> ConfigMapRef;
	struct ReqHandlerData : public IntrusiveCountableBase
	{
		RequestHandlerIFCRef rqIFCRef;
	};
	typedef IntrusiveReference<ReqHandlerData> ReqHandlerDataRef;
	typedef SortedVectorMap<String, ReqHandlerDataRef> ReqHandlerMap;
	mutable Mutex m_monitor;
	RepositoryIFCRef m_cimRepository;
	RepositoryIFCRef m_cimServer;
	AuthorizerIFCRef m_authorizer;

	AuthorizerManagerRef m_authorizerManager;
	//AuthorizerIFCRef m_authorizer;

	AuthManagerRef m_authManager;
	Logger m_Logger;
	ConfigMapRef m_configItems;
	ProviderManagerRef m_providerManager;
	mutable SharedLibraryRef m_wqlLib;
	mutable SharedLibraryRef m_indicationRepLayerLib;
	PollingManagerRef m_pollingManager;
	IndicationServerRef m_indicationServer;
	bool m_indicationsDisabled;
	Array<SelectableIFCRef> m_selectables;
	Array<SelectableCallbackIFCRef> m_selectableCallbacks;
	mutable Array<ServiceIFCRef> m_services;
	mutable ReqHandlerMap m_reqHandlers;
	mutable Mutex m_reqHandlersLock;
	mutable Mutex m_indicationLock;
	mutable bool m_indicationRepLayerDisabled;
	mutable Mutex m_selectableLock;
	
	enum EEnvState
	{
		E_STATE_INVALID,
		E_STATE_INITIALIZING,
		E_STATE_INITIALIZED,
		E_STATE_STARTING,
		E_STATE_STARTED,
		E_STATE_SHUTTING_DOWN,
		E_STATE_SHUTDOWN,
		E_STATE_UNLOADED
	};

	static bool isLoaded(EEnvState s)
	{
		return s >= E_STATE_INITIALIZING && s <= E_STATE_SHUTDOWN;
	}
	static bool isInitialized(EEnvState s)
	{
		return s >= E_STATE_INITIALIZED && s <= E_STATE_STARTED;
	}

	EEnvState m_state;
	mutable Mutex m_stateGuard;
	IndicationRepLayerMediatorRef m_indicationRepLayerMediatorRef;

};

} // end namespace OW_NAMESPACE

#endif
