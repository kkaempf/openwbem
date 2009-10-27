/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
* Copyright (C) 2005 Novell, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc., Novell, Inc., nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc., Novell, Inc., OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 * @author Jon Carey
 * @author Dan Nuffer
 */

/**
 * NOTE: This header is experimental and subject to change
 */

#ifndef OW_EMBEDDEDCIMOMENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_EMBEDDEDCIMOMENVIRONMENT_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "blocxx/LogLevel.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "blocxx/SharedLibrary.hpp"
#include "blocxx/SortedVectorMap.hpp"
#include "blocxx/Map.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/SharedLibraryReference.hpp"
#include "blocxx/DateTime.hpp"
#include "blocxx/Mutex.hpp"
#include "OW_CIMInstance.hpp"
#include "blocxx/Reference.hpp"
#include "blocxx/IntrusiveReference.hpp"
#include "OW_ConfigFile.hpp"
#include "blocxx/HashMap.hpp"
#include "blocxx/SortedVectorSet.hpp"
#include "OW_CimomCommonFwd.hpp"
#include "OW_CimomServerFwd.hpp"
#include "OW_CIMFwd.hpp"
#include "blocxx/Logger.hpp"

namespace OW_NAMESPACE
{

class OW_CIMOMCOMMON_API EmbeddedCIMOMEnvironment : public ServiceEnvironmentIFC
{
public:
	EmbeddedCIMOMEnvironment();
	~EmbeddedCIMOMEnvironment();
	void init();
	virtual blocxx::String getConfigItem(const blocxx::String &name, const blocxx::String& defRetVal="") const;
	virtual blocxx::StringArray getMultiConfigItem(const blocxx::String &itemName,
		const blocxx::StringArray& defRetVal, const char* tokenizeSeparator) const;

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
	IndicationServerRef getIndicationServer() const;
	PollingManagerRef getPollingManager() const;
	void clearConfigItems();
	virtual void setConfigItem(const blocxx::String &item, const blocxx::String &value,
		EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS);
	void unloadProviders();
	void startServices();
	void shutdown();
	ProviderManagerRef getProviderManager() const;
	void unloadReqHandlers();
	IndicationRepLayerMediatorRef getIndicationRepLayerMediator() const;
	RepositoryIFCRef getRepository() const;
	RepositoryIFCRef getAuthorizingRepository() const;
	AuthorizerManagerRef getAuthorizerManager() const;


	// do not use this unless absolutely necessary!
	static EmbeddedCIMOMEnvironmentRef& instance();

private:
	void _createLogger();
	void _loadConfigItemsFromFile(const blocxx::String& filename);
	void _loadRequestHandlers();
	void _loadServices();
	void _createPollingManager();
	void _createIndicationServer();
	void _loadAuthorizer();
	void _createAuthorizerManager();
	void _sortServicesForDependencies();

	// Types
	typedef ConfigFile::ConfigMap ConfigMap;
	typedef blocxx::Reference<ConfigMap> ConfigMapRef;
	struct ReqHandlerData : public IntrusiveCountableBase
	{
		blocxx::DateTime dt;
		RequestHandlerIFCRef rqIFCRef;
		blocxx::String filename;
	};
	typedef blocxx::IntrusiveReference<ReqHandlerData> ReqHandlerDataRef;
	typedef blocxx::SortedVectorMap<blocxx::String, ReqHandlerDataRef> ReqHandlerMap;
	mutable blocxx::Mutex m_monitor;
	RepositoryIFCRef m_cimRepository;
	RepositoryIFCRef m_cimServer;
	AuthorizerIFCRef m_authorizer;

	AuthorizerManagerRef m_authorizerManager;

	blocxx::Logger m_Logger;
	ConfigMapRef m_configItems;
	ProviderManagerRef m_providerManager;
	mutable blocxx::SharedLibraryRef m_wqlLib;
	mutable blocxx::SharedLibraryRef m_indicationRepLayerLib;
	PollingManagerRef m_pollingManager;
	IndicationServerRef m_indicationServer;
	bool m_indicationsDisabled;
	blocxx::Array<blocxx::SelectableIFCRef> m_selectables;
	blocxx::Array<blocxx::SelectableCallbackIFCRef> m_selectableCallbacks;
	mutable blocxx::Array<ServiceIFCRef> m_services;
	mutable ReqHandlerMap m_reqHandlers;
	mutable blocxx::Mutex m_reqHandlersLock;
	mutable blocxx::Mutex m_indicationLock;
	mutable bool m_indicationRepLayerDisabled;
	mutable blocxx::Mutex m_selectableLock;

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
	mutable blocxx::Mutex m_stateGuard;
	IndicationRepLayerMediatorRef m_indicationRepLayerMediatorRef;

	static blocxx::String COMPONENT_NAME;
};

} // end namespace OW_NAMESPACE

#endif
