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
#include "OW_Types.h"
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


class OW_NotifyTrans;

//////////////////////////////////////////////////////////////////////////////
class OW_IndicationServerImpl : public OW_IndicationServer
{
public:
	OW_IndicationServerImpl();
	~OW_IndicationServerImpl();

	virtual void init(OW_CIMOMEnvironmentRef env);

	virtual void setStartedSemaphore(OW_Semaphore* sem);

	virtual OW_Int32 run();
	void shutdown();

	void processIndication(const OW_CIMInstance& instance,
		const OW_String& instNS);

	OW_CIMOMEnvironmentRef getEnvironment() const { return m_env; }

	bool getNewTrans(OW_NotifyTrans& outTrans);

	// these are called by the CIM_IndicationSubscription pass-thru provider.
	virtual void deleteSubscription(const OW_String& ns, const OW_CIMObjectPath& subPath);
	virtual void createSubscription(const OW_String& ns, const OW_CIMInstance& subInst, const OW_String& username);
	virtual void modifySubscription(const OW_String& ns, const OW_CIMInstance& subInst);
	virtual void modifyFilter(const OW_String& ns, const OW_CIMInstance& filterInst);
private:

	struct Subscription
	{
		Subscription()
			: m_subPath(OW_CIMNULL)
			, m_sub(OW_CIMNULL)
			, m_filter(OW_CIMNULL)
		{}

		OW_CIMObjectPath m_subPath;
		OW_CIMInstance m_sub;
		OW_IndicationProviderIFCRefArray m_providers;
		OW_CIMInstance m_filter;
		OW_WQLSelectStatement m_selectStmt;
		OW_WQLCompile m_compiledStmt;
		OW_StringArray m_classes;
		OW_String m_filterSourceNameSpace;
		OW_Array<bool> m_isPolled; // each bool corresponds to a provider
	};

	// They key is IndicationName:SourceInstanceClassName.  SourceInstanceClassName will only be used if the WQL filter contains "SourceInstance ISA ClassName"
	typedef OW_HashMultiMap<OW_String, Subscription> subscriptions_t;


	void _processIndication(const OW_CIMInstance& instance,
		const OW_String& instNS);

	void _processIndicationRange(
		const OW_CIMInstance& instanceArg, const OW_String instNS,
		std::vector<subscriptions_t::value_type>::iterator first, std::vector<subscriptions_t::value_type>::iterator last);

	void addTrans(const OW_String& ns, const OW_CIMInstance& indication,
		const OW_CIMInstance& handler,
		const OW_CIMInstance& subscription,
		OW_IndicationExportProviderIFCRef provider);

	OW_IndicationExportProviderIFCRef getProvider(const OW_String& className);

	struct ProcIndicationTrans
	{
		ProcIndicationTrans(const OW_CIMInstance& inst,
			const OW_String& ns)
			: instance(inst)
			, nameSpace(ns) {}

		OW_CIMInstance instance;
		OW_String nameSpace;
	};

	typedef OW_HashMap<OW_String, OW_IndicationExportProviderIFCRef> provider_map_t;
	provider_map_t m_providers;
	
	// m_procTrans is where new indications to be delivered are put.
	// Both m_procTrans and m_shuttingDown are protected by the same condition
	OW_List<ProcIndicationTrans> m_procTrans;
	bool m_shuttingDown;
	OW_NonRecursiveMutex m_mainLoopGuard;
	OW_Condition m_mainLoopCondition;

	OW_CIMOMEnvironmentRef m_env;
	OW_Semaphore* m_startedSem;

	subscriptions_t m_subscriptions;
	OW_Mutex m_subGuard;

	typedef OW_SharedLibraryReference<OW_LifecycleIndicationPoller> OW_LifecycleIndicationPollerRef;

	typedef OW_HashMap<OW_String, OW_LifecycleIndicationPollerRef > poller_map_t;
	poller_map_t m_pollers;

	OW_ThreadPoolRef m_notifierThreadPool;
};

#endif


