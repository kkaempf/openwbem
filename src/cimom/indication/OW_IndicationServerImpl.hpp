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
#ifndef OW_INDICATION_SERVER_IMPL_HPP_
#define OW_INDICATION_SERVER_IMPL_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_HashMap.hpp"
#include "OW_List.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"
#include "OW_Condition.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_IndicationServer.hpp"
#include "OW_HashMultiMap.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_LifecycleIndicationPoller.hpp"
#include "OW_ThreadPool.hpp"
#include "OW_ThreadBarrier.hpp"

namespace OpenWBEM
{

class NotifyTrans;
//////////////////////////////////////////////////////////////////////////////
class IndicationServerImpl : public IndicationServer
{
public:
	IndicationServerImpl();
	~IndicationServerImpl();
	virtual void init(CIMOMEnvironmentRef env);
	virtual void waitUntilReady();
	virtual Int32 run();
	void shutdown();
	void processIndication(const CIMInstance& instance,
		const String& instNS);
	CIMOMEnvironmentRef getEnvironment() const { return m_env; }
	bool getNewTrans(NotifyTrans& outTrans);
	// these are called by the CIM_IndicationSubscription pass-thru provider.
	virtual void deleteSubscription(const String& ns, const CIMObjectPath& subPath);
	virtual void createSubscription(const String& ns, const CIMInstance& subInst, const String& username);
	virtual void modifySubscription(const String& ns, const CIMInstance& subInst);
	virtual void modifyFilter(const String& ns, const CIMInstance& filterInst);
private:
	struct Subscription
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
	// They key is IndicationName:SourceInstanceClassName.  SourceInstanceClassName will only be used if the WQL filter contains "SourceInstance ISA ClassName"
	typedef HashMultiMap<String, Subscription> subscriptions_t;
	void _processIndication(const CIMInstance& instance,
		const String& instNS);
	void _processIndicationRange(
		const CIMInstance& instanceArg, const String instNS,
		std::vector<subscriptions_t::value_type>::iterator first, std::vector<subscriptions_t::value_type>::iterator last);
	void addTrans(const String& ns, const CIMInstance& indication,
		const CIMInstance& handler,
		const CIMInstance& subscription,
		IndicationExportProviderIFCRef provider);
	IndicationExportProviderIFCRef getProvider(const String& className);
	struct ProcIndicationTrans
	{
		ProcIndicationTrans(const CIMInstance& inst,
			const String& ns)
			: instance(inst)
			, nameSpace(ns) {}
		CIMInstance instance;
		String nameSpace;
	};
	typedef HashMap<String, IndicationExportProviderIFCRef> provider_map_t;
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
	typedef SharedLibraryReference< Reference<LifecycleIndicationPoller> > LifecycleIndicationPollerRef;
	typedef HashMap<String, LifecycleIndicationPollerRef > poller_map_t;
	poller_map_t m_pollers;
	ThreadPoolRef m_notifierThreadPool;
};

} // end namespace OpenWBEM

#endif
