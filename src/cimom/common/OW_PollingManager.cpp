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

#include "OW_config.h"
#include "OW_PollingManager.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_DateTime.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_Platform.hpp"
#include "OW_TimeoutException.hpp"
#include "OW_OperationContext.hpp"

#include <climits>

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
PollingManager::PollingManager(CIMOMEnvironmentRef env)
	: Thread()
	, m_shuttingDown(false)
	, m_env(env)
	, m_startedBarrier(2)
{
	Int32 maxThreads;
	try
	{
		maxThreads = env->getConfigItem(ConfigOpts::POLLING_MANAGER_MAX_THREADS_opt, OW_DEFAULT_POLLING_MANAGER_MAX_THREADS).toInt32();
	}
	catch (const StringConversionException&)
	{
		maxThreads = String(OW_DEFAULT_POLLING_MANAGER_MAX_THREADS).toInt32();
	}
	
	m_triggerRunnerThreadPool = ThreadPoolRef(new ThreadPool(ThreadPool::DYNAMIC_SIZE, maxThreads, maxThreads * 10, env->getLogger(), "Polling Manager"));
}
//////////////////////////////////////////////////////////////////////////////
PollingManager::~PollingManager()
{
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class PollingManagerProviderEnvironment : public ProviderEnvironmentIFC
	{
	public:
		PollingManagerProviderEnvironment(CIMOMEnvironmentRef env)
			: m_context()
			, m_env(env)
		{}
		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_context);
		}
		virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			return m_env->getRepositoryCIMOMHandle(m_context);
		}
		virtual RepositoryIFCRef getRepository() const
		{
			return m_env->getRepository();
		}
		virtual String getConfigItem(const String& name, const String& defRetVal="") const
		{
			return m_env->getConfigItem(name, defRetVal);
		}
		
		virtual LoggerRef getLogger() const
		{
			return m_env->getLogger();
		}
		virtual String getUserName() const
		{
			return Platform::getCurrentUserName();
		}
		virtual OperationContext& getOperationContext()
		{
			return m_context;
		}
	private:
		mutable OperationContext m_context;
		CIMOMEnvironmentRef m_env;
	};
	ProviderEnvironmentIFCRef createProvEnvRef(CIMOMEnvironmentRef env)
	{
		return ProviderEnvironmentIFCRef(new PollingManagerProviderEnvironment(env));
	}
}
//////////////////////////////////////////////////////////////////////////////
Int32
PollingManager::run()
{
	// let CIMOMEnvironment know we're running and ready to go.
	m_startedBarrier.wait();

	bool doInit = true;
	// Get all of the indication trigger providers
	ProviderManagerRef pm = m_env->getProviderManager();
	
	PolledProviderIFCRefArray itpra =
			pm->getPolledProviders(createProvEnvRef(m_env));
	m_env->logDebug(Format("PollingManager found %1 polled providers",
		itpra.size()));
	{
		// Get initial polling interval from all polled providers
		NonRecursiveMutexLock ml(m_triggerGuard);
		for (size_t i = 0; i < itpra.size(); ++i)
		{
			TriggerRunnerRef tr(new TriggerRunner(this, m_env));
			tr->m_pollInterval = itpra[i]->getInitialPollingInterval(
				createProvEnvRef(m_env));
			m_env->logDebug(Format("PollingManager poll interval for provider"
				" %1: %2", i, tr->m_pollInterval));
			if (!tr->m_pollInterval)
			{
				continue;
			}
			tr->m_itp = itpra[i];
			m_triggerRunners.append(tr);
		}
	}
	{
		NonRecursiveMutexLock l(m_triggerGuard);
		while (!m_shuttingDown)
		{
			bool rightNow;
			UInt32 sleepTime = calcSleepTime(rightNow, doInit);
			doInit = false;
			if (!rightNow)
			{
				if (sleepTime == 0)
				{
					m_triggerCondition.wait(l);
				}
				else
				{
					m_triggerCondition.timedWait(l, sleepTime);
				}
			}
			if (m_shuttingDown)
			{
				break;
			}
			processTriggers();
		}
	}
	// wait until all the threads exit
	m_triggerRunnerThreadPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, 60);
	m_triggerRunners.clear();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
UInt32
PollingManager::calcSleepTime(bool& rightNow, bool doInit)
{
	rightNow = false;
	DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();
	time_t leastTime = LONG_MAX;
	int checkedCount = 0;
	for (size_t i = 0; i < m_triggerRunners.size(); i++)
	{
		if (m_triggerRunners[i]->m_isRunning
			|| m_triggerRunners[i]->m_pollInterval == 0)
		{
			continue;
		}
		if (doInit)
		{
			m_triggerRunners[i]->m_nextPoll =
				tm + m_triggerRunners[i]->m_pollInterval;
		}
		else if (m_triggerRunners[i]->m_nextPoll <= tm)
		{
			rightNow = true;
			return 0;
		}
		checkedCount++;
		time_t diff = m_triggerRunners[i]->m_nextPoll - tm;
		if (diff < leastTime)
		{
			leastTime = diff;
		}
	}
	return (checkedCount == 0) ? 0 : UInt32(leastTime);
}
//////////////////////////////////////////////////////////////////////////////
void
PollingManager::processTriggers()
{
	DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();
	for (size_t i = 0; i < m_triggerRunners.size(); i++)
	{
		if (m_triggerRunners[i]->m_isRunning)
		{
			continue;
		}
		if (m_triggerRunners[i]->m_pollInterval == 0)
		{
			// Stopped running - remove it
			m_triggerRunners.remove(i--);
			continue;
		}
		if (tm >= m_triggerRunners[i]->m_nextPoll)
		{
			m_triggerRunners[i]->m_isRunning = true;
			if (!m_triggerRunnerThreadPool->tryAddWork(m_triggerRunners[i]))
			{
				m_env->logInfo("Failed to run polled provider, because there are too many already running!");
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
PollingManager::shutdown()
{
	{
		NonRecursiveMutexLock l(m_triggerGuard);
		m_shuttingDown = true;
		m_triggerCondition.notifyAll();
	}
	// wait until the main thread exits.
	this->join();

	// clear out variables to avoid circular reference counts.
	m_triggerRunners.clear();
	m_env = 0;
	m_triggerRunnerThreadPool = 0;

}
//////////////////////////////////////////////////////////////////////////////
void
PollingManager::addPolledProvider(const PolledProviderIFCRef& p)
{
	NonRecursiveMutexLock l(m_triggerGuard);
	if (m_shuttingDown)
		return;
	TriggerRunnerRef tr(new TriggerRunner(this, m_env));
	tr->m_pollInterval = p->getInitialPollingInterval(
		createProvEnvRef(m_env));
	m_env->logDebug(Format("PollingManager poll interval for provider"
		" %1", tr->m_pollInterval));
	if (!tr->m_pollInterval)
	{
		return;
	}
	DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();
	tr->m_nextPoll = tm + tr->m_pollInterval;
	tr->m_itp = p;
	m_triggerRunners.append(tr);
	m_triggerCondition.notifyAll();
}
//////////////////////////////////////////////////////////////////////////////
PollingManager::TriggerRunner::TriggerRunner(PollingManager* svr,
	CIMOMEnvironmentRef env)
	: Runnable()
	, m_itp(0)
	, m_nextPoll(0)
	, m_isRunning(false)
	, m_pollInterval(0)
	, m_pollMan(svr)
	, m_env(env)
{
}
//////////////////////////////////////////////////////////////////////////////
void
PollingManager::TriggerRunner::run()
{
	Int32 nextInterval = 0;
	try
	{
		nextInterval = m_itp->poll(createProvEnvRef(m_env));
	}
	catch(std::exception& e)
	{
		m_env->logError(Format("Caught Exception while running poll: %1",
			e.what()));
	}
	catch(ThreadCancelledException& e)
	{
		throw;
	}
	catch(...)
	{
		m_env->logError("Caught Unknown Exception while running poll");
	}
	NonRecursiveMutexLock l(m_pollMan->m_triggerGuard);
	if (nextInterval == 0 || m_pollInterval == 0) // m_pollInterval == 0 means this poller has been instructed to stop
	{
		m_pollInterval = 0;
		m_nextPoll = 0;
	}
	else
	{
		if (nextInterval > 0)
		{
			m_pollInterval = nextInterval;
		}
		DateTime dtm;
		dtm.setToCurrent();
		m_nextPoll = dtm.get() + m_pollInterval;
	}
	m_isRunning = false;
	m_pollMan->m_triggerCondition.notifyOne();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::TriggerRunner::doCooperativeCancel()
{
	m_itp->doCooperativeCancel();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::TriggerRunner::doDefinitiveCancel()
{
	m_itp->doDefinitiveCancel();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::doCooperativeCancel()
{
	NonRecursiveMutexLock l(m_triggerGuard);
	m_shuttingDown = true;
	m_triggerCondition.notifyAll();
}

} // end namespace OpenWBEM

