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

#ifndef OW_POLLING_MANAGER_HPP_
#define OW_POLLING_MANAGER_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_ThreadEvent.hpp"
#include "OW_Thread.hpp"
#include "OW_List.hpp"
#include "OW_Array.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_Semaphore.hpp"
#include "OW_CIMOMEnvironment.hpp"

class OW_PollingManager : public OW_Thread
{
public:
	OW_PollingManager(OW_CIMOMEnvironmentRef env);
	virtual ~OW_PollingManager();

	void shutdown();

protected:
	virtual void run();

private:

	class TriggerRunner : public OW_Runnable
	{
	public:
		TriggerRunner(OW_PollingManager* svr, OW_CIMOMHandleIFCRef lch,
			OW_CIMOMEnvironmentRef env);
		TriggerRunner(const TriggerRunner& arg);
		void start();
		virtual void run();

		OW_PolledProviderIFCRef m_itp;
		time_t m_nextPoll;
		OW_Bool m_isRunning;
		long m_pollInterval;
		OW_PollingManager* m_indServer;
		OW_CIMOMHandleIFCRef m_lch;
		OW_CIMOMEnvironmentRef m_env;
	};

	OW_Array<TriggerRunner> m_triggerRunners;
	OW_ThreadEvent m_tevent;
	OW_Bool m_shuttingDown;
	static OW_Bool m_running;
	OW_Semaphore m_runCount;
	OW_Mutex m_triggerGuard;
	OW_CIMOMEnvironmentRef m_env;

	OW_UInt32 calcSleepTime(OW_Bool& rightNow, OW_Bool doInit);
	void processTriggers();

	friend class TriggerRunner;
};

typedef OW_Reference<OW_PollingManager> OW_PollingManagerRef;

#endif


