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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_POLLING_MANAGER_HPP_
#define OW_POLLING_MANAGER_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "blocxx/Thread.hpp"
#include "blocxx/Runnable.hpp"
#include "blocxx/List.hpp"
#include "blocxx/Array.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "blocxx/Condition.hpp"
#include "blocxx/ThreadBarrier.hpp"
#include "blocxx/ThreadPool.hpp"
#include "OW_CimomCommonFwd.hpp"
#include "OW_ServiceIFC.hpp"

namespace OW_NAMESPACE
{
class PollingManagerThread;

class OW_CIMOMCOMMON_API PollingManager : public ServiceIFC
{
public:
	PollingManager(const ProviderManagerRef& providerManager);
	virtual ~PollingManager();

	virtual blocxx::String getName() const;
	virtual blocxx::StringArray getDependencies() const;
	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void start();
	virtual void shutdown();

	void addPolledProvider(const PolledProviderIFCRef& p);
	void addPolledProvider(const PolledProviderIFCRef& p, blocxx::Int32 initialPollingInterval);
	void removePolledProvider(const PolledProviderIFCRef& p);

private:
	blocxx::IntrusiveReference<PollingManagerThread> m_pollingManagerThread;
};

class OW_CIMOMCOMMON_API PollingManagerThread : public blocxx::Thread
{
public:
	PollingManagerThread(const ProviderManagerRef& providerManager);
	virtual ~PollingManagerThread();
	void init(const ServiceEnvironmentIFCRef& env);
	void shutdown();
	void waitUntilReady()
	{
		m_startedBarrier.wait();
	}
	void addPolledProvider(const PolledProviderIFCRef& p);
	void addPolledProvider(const PolledProviderIFCRef& p, blocxx::Int32 initialPollingInterval);
	void removePolledProvider(const PolledProviderIFCRef& p);
protected:
	virtual blocxx::Int32 run();
private:
	class TriggerRunner : public blocxx::Runnable
	{
	public:
		TriggerRunner(PollingManagerThread* svr,
			ServiceEnvironmentIFCRef env);
		virtual void run();
		PolledProviderIFCRef m_itp;
		time_t m_nextPoll;
		bool m_isRunning;
		blocxx::Int32 m_pollInterval;
		PollingManagerThread* m_pollMan;
		ServiceEnvironmentIFCRef m_env;
		blocxx::Logger m_logger;
	private:
		void doShutdown();
		void doCooperativeCancel();
		void doDefinitiveCancel();
	};
	typedef blocxx::IntrusiveReference<TriggerRunner> TriggerRunnerRef;
	blocxx::Array<TriggerRunnerRef> m_triggerRunners;
	bool m_shuttingDown;
	blocxx::NonRecursiveMutex m_triggerGuard;
	blocxx::Condition m_triggerCondition;
	ServiceEnvironmentIFCRef m_env;
	ProviderManagerRef m_providerManager;
	blocxx::Logger m_logger;
	blocxx::ThreadBarrier m_startedBarrier;
	blocxx::ThreadPoolRef m_triggerRunnerThreadPool;

	// m_triggerGuard must be locked before calling this function.
	blocxx::UInt32 calcSleepTime(bool& rightNow, bool doInit);
	// m_triggerGuard must be locked before calling this function.
	void processTriggers();
	friend class TriggerRunner;

	virtual void doShutdown();
};

} // end namespace OW_NAMESPACE

#endif
