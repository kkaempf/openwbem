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
#ifndef OW_INDICATION_SERVER_IMPL_HPP_
#define OW_INDICATION_SERVER_IMPL_HPP_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_CIMFwd.hpp"
#include "OW_Map.hpp"
#include "OW_List.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"
#include "OW_Thread.hpp"
#include "OW_Condition.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_IndicationServer.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_ThreadCounter.hpp"
#include "OW_HashMultiMap.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"


class OW_NotifyTrans;

//////////////////////////////////////////////////////////////////////////////
class OW_IndicationServerImpl : public OW_IndicationServer
{
public:
	enum
	{
		MAX_NOTIFIERS = 10
	};

	OW_IndicationServerImpl();
	~OW_IndicationServerImpl();

	virtual void init(OW_CIMOMEnvironmentRef env);

	virtual void setStartedSemaphore(OW_Semaphore* sem);

	virtual void run();
	void shutdown();

	void processIndication(const OW_CIMInstance& instance,
		const OW_String& instNS);

	OW_CIMOMEnvironmentRef getEnvironment() const { return m_env; }

	bool getNewTrans(OW_NotifyTrans& outTrans);

	OW_ThreadCounterRef m_threadCounter;

	// these are called by the CIM_IndicationSubscription pass-thru provider.
	virtual void deleteSubscription(const OW_String& ns, const OW_CIMObjectPath& subPath);
	virtual void createSubscription(const OW_String& ns, const OW_CIMInstance& subInst);
	virtual void modifySubscription(const OW_String& ns, const OW_CIMInstance& subInst);
private:

	void _processIndication(const OW_CIMInstance& instance,
		const OW_String& instNS);


	void addTrans(const OW_String& ns, const OW_CIMInstance& indication,
		const OW_CIMInstance& handler,
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

	OW_Map<OW_String, OW_IndicationExportProviderIFCRef> m_providers;
	
	// m_procTrans is where new indications to be delivered are put.
	// Both m_procTrans and m_shuttingDown are protected by the same condition
	OW_List<ProcIndicationTrans> m_procTrans;
	OW_Bool m_shuttingDown;
	OW_Mutex m_mainLoopGuard;
	OW_Condition m_mainLoopCondition;

	// This is where the indications get placed if the number of notify 
	// threads maxes out, and they need to be pooled somewhere.
	OW_List<OW_NotifyTrans> m_trans;
	OW_Mutex m_transGuard;

	OW_CIMOMEnvironmentRef m_env;
	OW_Semaphore* m_startedSem;

	struct Subscription
	{
		Subscription()
			: m_subPath(OW_CIMNULL)
		{}

		OW_CIMObjectPath m_subPath;
		OW_CIMInstance m_sub;
		OW_IndicationProviderIFCRefArray m_providers;
		OW_CIMInstance m_filter;
		OW_WQLSelectStatement m_selectStmt;
		OW_WQLCompile m_compiledStmt;
		OW_StringArray m_classes;
	};

	// They key is IndicationName:SourceInstanceClassName.  SourceInstanceClassName will only be used if the WQL filter contains "SourceInstance ISA ClassName"
	typedef OW_HashMultiMap<OW_String, Subscription> subscriptions_t;
	subscriptions_t m_subscriptions;
	OW_Mutex m_subGuard;

};

#endif


