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

#ifndef OW_POLLING_MANAGER_HPP_
#define OW_POLLING_MANAGER_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_Thread.hpp"
#include "OW_List.hpp"
#include "OW_Array.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_Condition.hpp"
#include "OW_Semaphore.hpp"
#include "OW_ThreadCounter.hpp"
#include "OW_UserInfo.hpp"

class OW_PollingManager : public OW_Thread
{
public:
	OW_PollingManager(OW_CIMOMEnvironmentRef env);
	virtual ~OW_PollingManager();

	void shutdown();

	void setStartedSemaphore(OW_Semaphore* sem)
	{
		m_startedSem = sem;
	}

	void addPolledProvider(const OW_PolledProviderIFCRef& p);

protected:
	virtual void run();

private:

	class TriggerRunner : public OW_Runnable
	{
	public:
		TriggerRunner(OW_PollingManager* svr, OW_UserInfo acl,
			OW_CIMOMEnvironmentRef env);
		void start(const OW_RunnableRef& tr);
		virtual void run();

		OW_PolledProviderIFCRef m_itp;
		time_t m_nextPoll;
		OW_Bool m_isRunning;
		OW_Int32 m_pollInterval;
		OW_PollingManager* m_pollMan;
		OW_UserInfo m_acl;
		OW_CIMOMEnvironmentRef m_env;
	};
	typedef OW_Reference<TriggerRunner> TriggerRunnerRef;

	OW_Array<TriggerRunnerRef> m_triggerRunners;
	OW_Bool m_shuttingDown;
	OW_NonRecursiveMutex m_triggerGuard;
	OW_Condition m_triggerCondition;
	
	OW_ThreadCounterRef m_threadCount;

	OW_CIMOMEnvironmentRef m_env;

	OW_Semaphore* m_startedSem;

    // m_triggerGuard must be locked before calling this function.
	OW_UInt32 calcSleepTime(OW_Bool& rightNow, OW_Bool doInit);
    // m_triggerGuard must be locked before calling this function.
	void processTriggers();

	friend class TriggerRunner;
};

typedef OW_Reference<OW_PollingManager> OW_PollingManagerRef;

#endif


