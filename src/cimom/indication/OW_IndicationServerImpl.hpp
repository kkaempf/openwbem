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
 * @author Dan Nuffer
 */

#ifndef OW_INDICATION_SERVER_IMPL_HPP_
#define OW_INDICATION_SERVER_IMPL_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_List.hpp"
#include "OW_Condition.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_IndicationServer.hpp"
#include "OW_HashMultiMap.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_ThreadBarrier.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_Map.hpp"
#include "OW_Mutex.hpp"
#include "OW_Thread.hpp"

namespace OpenWBEM
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
	virtual String getName() const;
	virtual StringArray getDependencies() const;
	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void start();
	void shutdown();
	void processIndication(const CIMInstance& instance,
		const String& instNS);
	// these are called by the CIM_IndicationSubscription pass-thru provider.
	virtual void startDeleteSubscription(const String& ns, const CIMObjectPath& subPath);
	virtual void startCreateSubscription(const String& ns, const CIMInstance& subInst, const String& username);
	virtual void startModifySubscription(const String& ns, const CIMInstance& subInst);
	
	// these are called by the threads started by the previous functions
	void deleteSubscription(const String& ns, const CIMObjectPath& subPath);
	void createSubscription(const String& ns, const CIMInstance& subInst, const String& username);
	void modifySubscription(const String& ns, const CIMInstance& subInst);
	
	virtual void modifyFilter(const String& ns, const CIMInstance& filterInst, const String& userName);

private:
	IntrusiveReference<IndicationServerImplThread> m_indicationServerThread;
};

class IndicationServerImplThread : public Thread
{
public:
	IndicationServerImplThread();
	~IndicationServerImplThread();
	virtual void init(const CIMOMEnvironmentRef& env);
	virtual void waitUntilReady();
	virtual Int32 run();
	void shutdown();
	void processIndication(const CIMInstance& instance,
		const String& instNS);
	CIMOMEnvironmentRef getEnvironment() const;
	bool getNewTrans(NotifyTrans& outTrans);
	// these are called by the CIM_IndicationSubscription pass-thru provider.
	virtual void startDeleteSubscription(const String& ns, const CIMObjectPath& subPath);
	virtual void startCreateSubscription(const String& ns, const CIMInstance& subInst, const String& username);
	virtual void startModifySubscription(const String& ns, const CIMInstance& subInst);
	
	// these are called by the threads started by the previous functions
	void deleteSubscription(const String& ns, const CIMObjectPath& subPath);
	void createSubscription(const String& ns, const CIMInstance& subInst, const String& username);
	void modifySubscription(const String& ns, const CIMInstance& subInst);
	
	virtual void modifyFilter(const String& ns, const CIMInstance& filterInst, const String& userName);

	virtual void doCooperativeCancel();

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
		StringArray m_classes;
		String m_filterSourceNameSpace;
		Array<bool> m_isPolled; // each bool corresponds to a provider
	};
	typedef IntrusiveReference<Subscription> SubscriptionRef;

	// They key is indicationname:sourceinstanceclassname. All lower case. SourceInstanceClassName will only be used if 
	// the WQL filter contains "SourceInstance ISA ClassName".  A given SubscriptionRef may be inserted multiple times
	// for the same subscription, but only one Subscription instance will exist. The SubscriptionRef will never be null.
	typedef HashMultiMap<String, SubscriptionRef> subscriptions_t;

#if defined(OW_AIX)
	typedef subscriptions_t subscriptions_copy_t;
#else
	// Save setup time and memory by using a vector instead of a map.
	typedef std::vector<subscriptions_t>::value_type subscriptions_copy_t;
#endif // AIX
	typedef subscriptions_copy_t::iterator subscriptions_iterator;

	void _processIndication(const CIMInstance& instance,
		const String& instNS);
	
	void _processIndicationRange(
		const CIMInstance& instanceArg, const String instNS,
		subscriptions_iterator first, subscriptions_iterator last);
	
	void addTrans(const String& ns, const CIMInstance& indication,
		const CIMInstance& handler,
		const CIMInstance& subscription,
		IndicationExportProviderIFCRef provider);
	
	IndicationExportProviderIFCRef getProvider(const CIMName& className);
	
	void deactivateAllSubscriptions();

	struct ProcIndicationTrans
	{
		ProcIndicationTrans(const CIMInstance& inst,
			const String& ns)
			: instance(inst)
			, nameSpace(ns) {}
		CIMInstance instance;
		String nameSpace;
	};
	typedef SortedVectorMap<CIMName, IndicationExportProviderIFCRef> provider_map_t;
	provider_map_t m_providers;
	
	// m_procTrans is where new indications to be delivered are put.
	// Both m_procTrans and m_shuttingDown are protected by the same condition
	List<ProcIndicationTrans> m_procTrans;
	bool m_shuttingDown;
	NonRecursiveMutex m_mainLoopGuard;
	Condition m_mainLoopCondition;
	CIMOMEnvironmentRef m_env;
	ThreadBarrier m_startedBarrier;
	subscriptions_t m_subscriptions;
	Mutex m_subGuard;
	typedef SharedLibraryReference< IntrusiveReference<LifecycleIndicationPoller> > LifecycleIndicationPollerRef;
	typedef Map<CIMName, LifecycleIndicationPollerRef > poller_map_t;
	poller_map_t m_pollers;
	ThreadPoolRef m_notifierThreadPool;
	ThreadPoolRef m_subscriptionPool;
	WQLIFCRef m_wqlRef;
	LoggerRef m_logger;
};

} // end namespace OpenWBEM

#endif
