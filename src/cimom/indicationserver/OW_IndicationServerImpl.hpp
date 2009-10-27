/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
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
 * @author Dan Nuffer
 */

#ifndef OW_INDICATION_SERVER_IMPL_HPP_
#define OW_INDICATION_SERVER_IMPL_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "blocxx/List.hpp"
#include "blocxx/Condition.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_IndicationServer.hpp"
#include "blocxx/HashMultiMap.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"
#include "blocxx/ThreadBarrier.hpp"
#include "OW_ProviderFwd.hpp"
#include "blocxx/SortedVectorMap.hpp"
#include "blocxx/Map.hpp"
#include "blocxx/Mutex.hpp"
#include "blocxx/Thread.hpp"
#include "blocxx/Logger.hpp"
#include "OW_IndicationProviderIFC.hpp"

namespace OW_NAMESPACE
{

class NotifyTrans;
class LifecycleIndicationPoller;
class IndicationServerImplThread;

//////////////////////////////////////////////////////////////////////////////
class IndicationServerImpl : public IndicationServer
{
public:
	IndicationServerImpl();
	~IndicationServerImpl();
	virtual blocxx::String getName() const;
	virtual blocxx::StringArray getDependencies() const;
	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void start();
	void shutdown();
	void processIndication(const CIMInstance& indication, const blocxx::String& indicationNS);
	// these are called by the CIM_IndicationSubscription pass-thru provider.
	virtual void startDeleteSubscription(const blocxx::String& subNS, const CIMObjectPath& subPath);
	virtual void startCreateSubscription(const blocxx::String& subNS, const CIMInstance& subInst, const blocxx::String& username);
	virtual void startModifySubscription(const blocxx::String& subNS, const CIMInstance& subInst);

	// these are called by the threads started by the previous functions
	void deleteSubscription(const blocxx::String& subNS, const CIMObjectPath& subPath);
	void createSubscription(const blocxx::String& subNS, const CIMInstance& subInst, const blocxx::String& username);
	void modifySubscription(const blocxx::String& subNS, const CIMInstance& subInst);

	virtual void modifyFilter(OperationContext& context, const blocxx::String& filterNS, const CIMInstance& filterInst, const blocxx::String& userName);

private:
	blocxx::IntrusiveReference<IndicationServerImplThread> m_indicationServerThread;
};

class IndicationServerImplThread : public blocxx::Thread
{
public:
	IndicationServerImplThread();
	~IndicationServerImplThread();
	virtual void init(const CIMOMEnvironmentRef& env);
	virtual void waitUntilReady();
	virtual blocxx::Int32 run();
	void shutdown();
	void processIndication(const CIMInstance& instance,
		const blocxx::String& instNS);
	CIMOMEnvironmentRef getEnvironment() const;
	bool getNewTrans(NotifyTrans& outTrans);
	// these are called by the CIM_IndicationSubscription pass-thru provider.
	virtual void startDeleteSubscription(const blocxx::String& ns, const CIMObjectPath& subPath);
	virtual void startCreateSubscription(const blocxx::String& ns, const CIMInstance& subInst, const blocxx::String& username);
	virtual void startModifySubscription(const blocxx::String& ns, const CIMInstance& subInst);

	// these are called by the threads started by the previous functions
	void deleteSubscription(const blocxx::String& ns, const CIMObjectPath& subPath);
	void createSubscription(const blocxx::String& ns, const CIMInstance& subInst, const blocxx::String& username);
	void modifySubscription(const blocxx::String& ns, const CIMInstance& subInst);

	virtual void modifyFilter(OperationContext& context, const blocxx::String& ns, const CIMInstance& filterInst, const blocxx::String& userName);

	virtual void doShutdown();

private:
	struct Subscription : public IntrusiveCountableBase
	{
		Subscription()
			: m_subPath(CIMNULL)
			, m_sub(CIMNULL)
			, m_filter(CIMNULL)
		{}
		CIMObjectPath m_subPath;
		CIMInstance m_sub;
		IndicationProviderIFCRefArray m_providers;
		CIMInstance m_filter;
		WQLSelectStatement m_selectStmt;
		WQLCompile m_compiledStmt;
		blocxx::StringArray m_classes;
		blocxx::String m_filterSourceNameSpace;
		blocxx::Array<bool> m_isPolled; // each bool corresponds to a provider
	};
	typedef blocxx::IntrusiveReference<Subscription> SubscriptionRef;

	// They key is indicationname:sourceinstanceclassname. All lower case. SourceInstanceClassName will only be used if
	// the WQL filter contains "SourceInstance ISA ClassName".  A given SubscriptionRef may be inserted multiple times
	// for the same subscription, but only one Subscription instance will exist. The SubscriptionRef will never be null.
	typedef HashMultiMap<blocxx::String, SubscriptionRef> subscriptions_t;

#if defined(OW_AIX)
	typedef subscriptions_t subscriptions_copy_t;
#else
	// Save setup time and memory by using a vector instead of a map.
	typedef std::vector<subscriptions_t>::value_type subscriptions_copy_t;
#endif // AIX
	typedef subscriptions_copy_t::iterator subscriptions_iterator;

	void _processIndication(const CIMInstance& instance,
		const blocxx::String& instNS);

	void _processIndicationRange(
		const CIMInstance& instanceArg, const blocxx::String instNS,
		subscriptions_iterator first, subscriptions_iterator last);

	void addTrans(const blocxx::String& ns, const CIMInstance& indication,
		const CIMInstance& handler,
		const blocxx::String& subscriptionNS,
		const CIMInstance& subscription,
		IndicationExportProviderIFCRef provider);

	IndicationExportProviderIFCRef getProvider(const CIMName& className);

	blocxx::UInt32 activateFilterOnProvider(IndicationProviderIFCRef& prov);
	blocxx::UInt32 deActivateFilterOnProvider(IndicationProviderIFCRef& prov);

	void deactivateAllSubscriptions();

	struct ProcIndicationTrans
	{
		ProcIndicationTrans(const CIMInstance& indication_,
			const blocxx::String& indicationNS_)
			: indication(indication_)
			, indicationNS(indicationNS_) {}
		CIMInstance indication;
		blocxx::String indicationNS;
	};
	typedef blocxx::SortedVectorMap<CIMName, IndicationExportProviderIFCRef> provider_map_t;
	provider_map_t m_providers;

	typedef blocxx::SortedVectorMap<IndicationProviderIFCRef, blocxx::UInt32> activatecount_map_t;
	activatecount_map_t m_activations;

	// m_procTrans is where new indications to be delivered are put.
	// Both m_procTrans and m_shuttingDown are protected by the same condition
	blocxx::List<ProcIndicationTrans> m_procTrans;
	bool m_shuttingDown;
	blocxx::NonRecursiveMutex m_mainLoopGuard;
	blocxx::Condition m_mainLoopCondition;
	CIMOMEnvironmentRef m_env;
	blocxx::ThreadBarrier m_startedBarrier;
	subscriptions_t m_subscriptions;
	blocxx::Mutex m_subGuard;
	blocxx::Mutex m_actCountGuard;
	typedef blocxx::SharedLibraryReference< blocxx::IntrusiveReference<LifecycleIndicationPoller> > LifecycleIndicationPollerRef;

	// the key is <Source Namespace>:<Classname>
	typedef blocxx::Map<blocxx::String, LifecycleIndicationPollerRef > poller_map_t;
	poller_map_t m_pollers;
	blocxx::ThreadPoolRef m_notifierThreadPool;
	blocxx::ThreadPoolRef m_subscriptionPool;
	WQLIFCRef m_wqlRef;
	blocxx::Logger m_logger;
};

} // end namespace OW_NAMESPACE

#endif
