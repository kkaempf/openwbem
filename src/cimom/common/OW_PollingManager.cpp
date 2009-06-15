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
#include "blocxx/NonRecursiveMutexLock.hpp"
#include "blocxx/DateTime.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "blocxx/Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_Platform.hpp"
#include "blocxx/TimeoutException.hpp"
#include "OW_LocalOperationContext.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_ServiceIFCNames.hpp"

//#include <climits>
#include <limits>

namespace
{
	template <bool b> struct compile_time_assert;
	template <> struct compile_time_assert<true> { };
	using OpenWBEM::Int32;

	// RETURNS: max(x, min(x+y, TIME_T_MAX)) where TIME_T_MAX is largest time_t
	time_t safe_add(time_t x, Int32 y)
	{
		compile_time_assert<(sizeof(time_t) >= sizeof(Int32))> dummy;
		time_t const max_time = std::numeric_limits<time_t>::max();
		return (
			y <= 0 ? x :
			x > max_time - y ? max_time :
			x + y
		);
	}
}

namespace OW_NAMESPACE
{

namespace
{
const String COMPONENT_NAME("ow.owcimomd.PollingManager");
}

//////////////////////////////////////////////////////////////////////////////
PollingManager::PollingManager(const ProviderManagerRef& providerManager)
	: m_pollingManagerThread(new PollingManagerThread(providerManager))
{

}

//////////////////////////////////////////////////////////////////////////////
PollingManager::~PollingManager()
{
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::init(const ServiceEnvironmentIFCRef& env)
{
	m_pollingManagerThread->init(env);
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::start()
{
	m_pollingManagerThread->start();
	m_pollingManagerThread->waitUntilReady();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::shutdown()
{
	m_pollingManagerThread->shutdown();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::addPolledProvider(const PolledProviderIFCRef& p)
{
	m_pollingManagerThread->addPolledProvider(p);
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::addPolledProvider(const PolledProviderIFCRef& p, Int32 initialPollingInterval)
{
	m_pollingManagerThread->addPolledProvider(p, initialPollingInterval);
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManager::removePolledProvider(const PolledProviderIFCRef& p)
{
	m_pollingManagerThread->removePolledProvider(p);
}



//////////////////////////////////////////////////////////////////////////////
PollingManagerThread::PollingManagerThread(const ProviderManagerRef& providerManager)
	: Thread()
	, m_shuttingDown(false)
	, m_providerManager(providerManager)
	, m_logger(COMPONENT_NAME)
	, m_startedBarrier(2)
{
}
//////////////////////////////////////////////////////////////////////////////
PollingManagerThread::~PollingManagerThread()
{
}
//////////////////////////////////////////////////////////////////////////////
String
PollingManager::getName() const
{
	return ServiceIFCNames::PollingManager;
}

//////////////////////////////////////////////////////////////////////////////
StringArray
PollingManager::getDependencies() const
{
	StringArray rv;
	rv.push_back(ServiceIFCNames::CIMServer);
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::init(const ServiceEnvironmentIFCRef& env)
{
	m_env = env;
	Int32 maxThreads;
	try
	{
		maxThreads = env->getConfigItem(ConfigOpts::POLLING_MANAGER_MAX_THREADS_opt, OW_DEFAULT_POLLING_MANAGER_MAX_THREADS).toInt32();
	}
	catch (const StringConversionException&)
	{
		maxThreads = String(OW_DEFAULT_POLLING_MANAGER_MAX_THREADS).toInt32();
	}

	m_triggerRunnerThreadPool = ThreadPoolRef(new ThreadPool(ThreadPool::DYNAMIC_SIZE, maxThreads, maxThreads * 10,
		Logger(COMPONENT_NAME), "Polling Manager"));
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class PollingManagerProviderEnvironment : public ProviderEnvironmentIFC
	{
	public:
		PollingManagerProviderEnvironment(ServiceEnvironmentIFCRef env)
			: m_context()
			, m_env(env)
		{}
		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_context);
		}
		virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_context, ServiceEnvironmentIFC::E_BYPASS_PROVIDERS);
		}
		virtual RepositoryIFCRef getRepository() const
		{
			return m_env->getRepository();
		}
		virtual RepositoryIFCRef getAuthorizingRepository() const
		{
			return m_env->getAuthorizingRepository();
		}
		virtual String getConfigItem(const String& name, const String& defRetVal="") const
		{
			return m_env->getConfigItem(name, defRetVal);
		}
		virtual StringArray getMultiConfigItem(const String &itemName, 
			const StringArray& defRetVal, const char* tokenizeSeparator = 0) const
		{
			return m_env->getMultiConfigItem(itemName, defRetVal, tokenizeSeparator);
		}
		
		virtual String getUserName() const
		{
			return Platform::getCurrentUserName();
		}
		virtual OperationContext& getOperationContext()
		{
			return m_context;
		}
		virtual ProviderEnvironmentIFCRef clone() const
		{
			return ProviderEnvironmentIFCRef(new PollingManagerProviderEnvironment(m_env));
		}
	private:
		mutable LocalOperationContext m_context;
		ServiceEnvironmentIFCRef m_env;
	};
	ProviderEnvironmentIFCRef createProvEnvRef(ServiceEnvironmentIFCRef env)
	{
		return ProviderEnvironmentIFCRef(new PollingManagerProviderEnvironment(env));
	}
}
//////////////////////////////////////////////////////////////////////////////
Int32
PollingManagerThread::run()
{
	// let CIMOMEnvironment know we're running and ready to go.
	m_startedBarrier.wait();

	bool doInit = true;

	// Get all of the indication trigger providers
	PolledProviderIFCRefArray itpra = m_providerManager->getPolledProviders();

	OW_LOG_DEBUG(m_logger, Format("PollingManager found %1 polled providers", itpra.size()));

	{
		// Get initial polling interval from all polled providers
		NonRecursiveMutexLock ml(m_triggerGuard);
		for (size_t i = 0; i < itpra.size(); ++i)
		{
			TriggerRunnerRef tr(new TriggerRunner(this, m_env));
			tr->m_pollInterval = itpra[i]->getInitialPollingInterval(createProvEnvRef(m_env));
			OW_LOG_DEBUG(m_logger, Format("PollingManager initial poll interval for provider %1: %2", i, tr->m_pollInterval));
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
					m_triggerCondition.timedWait(l, Timeout::relative(sleepTime));
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
	m_triggerRunnerThreadPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, Timeout::relative(50), Timeout::relative(60));
	m_triggerRunners.clear();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
UInt32
PollingManagerThread::calcSleepTime(bool& rightNow, bool doInit)
{
	rightNow = false;
	DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();

	Int32 const int32_max = std::numeric_limits<Int32>::max();
	time_t const time_t_max = std::numeric_limits<time_t>::max();
	time_t leastTime = (time_t_max > int32_max ? int32_max : time_t_max);
	// leastTime is now a large positive time_t value that will fit into an
	// Int32, and hence into a UInt32.

	int checkedCount = 0;
	// LOOP INVARIANT: 0 <= leastTime <= int32_max
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
				safe_add(tm, m_triggerRunners[i]->m_pollInterval);
		}
		else if (m_triggerRunners[i]->m_nextPoll <= tm)
		{
			rightNow = true;
			return 0;
		}
		// GUARANTEED: m_triggerRunners[i]->m_nextPoll >= tm
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
PollingManagerThread::processTriggers()
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
				OW_LOG_INFO(m_logger, "Failed to run polled provider, because there are too many already running!");
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::shutdown()
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
	m_providerManager = 0;
	m_triggerRunnerThreadPool = 0;

}
//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::addPolledProvider(const PolledProviderIFCRef& p)
{
	NonRecursiveMutexLock l(m_triggerGuard);
	if (m_shuttingDown)
		return;
	TriggerRunnerRef tr(new TriggerRunner(this, m_env));
	tr->m_pollInterval = p->getInitialPollingInterval(createProvEnvRef(m_env));
	OW_LOG_DEBUG(m_logger, Format("PollingManager addPolledProvider(): initial poll interval for provider %1", tr->m_pollInterval));
	if (!tr->m_pollInterval)
	{
		return;
	}
	DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();
	tr->m_nextPoll = safe_add(tm, tr->m_pollInterval);
	tr->m_itp = p;
	m_triggerRunners.append(tr);
	m_triggerCondition.notifyAll();
}
//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::addPolledProvider(const PolledProviderIFCRef& p, Int32 initialPollingInterval)
{
	if (initialPollingInterval <= 0)
	{
		return;
	}

	NonRecursiveMutexLock l(m_triggerGuard);
	if (m_shuttingDown)
		return;

	TriggerRunnerRef tr(new TriggerRunner(this, m_env));
	tr->m_pollInterval = initialPollingInterval;
	if (!tr->m_pollInterval)
	{
		return;
	}
	DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();
	tr->m_nextPoll = safe_add(tm, tr->m_pollInterval);
	tr->m_itp = p;

	m_triggerRunners.append(tr);
	m_triggerCondition.notifyAll();
}
//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::removePolledProvider(const PolledProviderIFCRef& p)
{
	NonRecursiveMutexLock l(m_triggerGuard);
	
	if (m_shuttingDown)
		return;

	for (size_t i = 0; i < m_triggerRunners.size(); ++i)
	{
		if (m_triggerRunners[i]->m_itp == p)
		{
			m_triggerRunners.remove(i);
			--i;
		}
	}
	
	m_triggerCondition.notifyAll();
}
//////////////////////////////////////////////////////////////////////////////
PollingManagerThread::TriggerRunner::TriggerRunner(PollingManagerThread* svr,
	ServiceEnvironmentIFCRef env)
	: Runnable()
	, m_itp(0)
	, m_nextPoll(0)
	, m_isRunning(false)
	, m_pollInterval(0)
	, m_pollMan(svr)
	, m_env(env)
	, m_logger(COMPONENT_NAME)
{
}
//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::TriggerRunner::run()
{
	Int32 nextInterval = 0;
	try
	{
		nextInterval = m_itp->poll(createProvEnvRef(m_env));
	}
	catch (Exception & e)
	{
		OW_LOG_ERROR(m_logger,
			Format("Caught Exception while running poll: %1", e));
	}
	catch(std::exception& e)
	{
		OW_LOG_ERROR(m_logger,
			Format("Caught standard exception while running poll: %1", e.what()));
	}
	catch(ThreadCancelledException& e)
	{
		throw;
	}
	catch(...)
	{
		OW_LOG_ERROR(m_logger, "Caught Unknown Exception while running poll");
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
		m_nextPoll = safe_add(dtm.get(), m_pollInterval);
	}
	m_isRunning = false;
	m_pollMan->m_triggerCondition.notifyOne();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::TriggerRunner::doShutdown()
{
	m_itp->doShutdown();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::TriggerRunner::doCooperativeCancel()
{
	m_itp->doCooperativeCancel();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::TriggerRunner::doDefinitiveCancel()
{
	m_itp->doDefinitiveCancel();
}

//////////////////////////////////////////////////////////////////////////////
void
PollingManagerThread::doShutdown()
{
	NonRecursiveMutexLock l(m_triggerGuard);
	m_shuttingDown = true;
	m_triggerCondition.notifyAll();
}

} // end namespace OW_NAMESPACE

