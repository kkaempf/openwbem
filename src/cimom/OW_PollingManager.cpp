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

#include "OW_config.h"
#include "OW_PollingManager.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_DateTime.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_UserInfo.hpp"
#include "OW_Platform.hpp"
#include "OW_TimeoutException.hpp"

#include <climits>

//////////////////////////////////////////////////////////////////////////////
OW_PollingManager::OW_PollingManager(OW_CIMOMEnvironmentRef env)
	: OW_Thread(true) // true means this thread will be joinable
	, m_shuttingDown(false)
	, m_env(env)
{
	OW_Int32 maxThreads;
	try
	{
		maxThreads = env->getConfigItem(OW_ConfigOpts::POLLING_MANAGER_MAX_THREADS_opt, OW_DEFAULT_POLLING_MANAGER_MAX_THREADS).toInt32();
	}
	catch (const OW_StringConversionException&)
	{
		maxThreads = OW_String(OW_DEFAULT_POLLING_MANAGER_MAX_THREADS).toInt32();
	}
	
	m_triggerRunnerThreadPool = OW_ThreadPoolRef(new OW_ThreadPool(OW_ThreadPool::DYNAMIC_SIZE, maxThreads, maxThreads * 10));
}

//////////////////////////////////////////////////////////////////////////////
OW_PollingManager::~OW_PollingManager()
{
}


//////////////////////////////////////////////////////////////////////////////
namespace
{
	class PollingManagerProviderEnvironment : public OW_ProviderEnvironmentIFC
	{
	public:

		PollingManagerProviderEnvironment(const OW_UserInfo& acl,
			OW_CIMOMEnvironmentRef env)
			: m_acl(acl)
			, m_env(env)
		{}

		virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_acl);
		}

		virtual OW_CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			return m_env->getRepositoryCIMOMHandle();
		}

		virtual OW_RepositoryIFCRef getRepository() const
		{
			return m_env->getRepository();
		}

		virtual OW_String getConfigItem(const OW_String& name, const OW_String& defRetVal="") const
		{
			return m_env->getConfigItem(name, defRetVal);
		}
		
		virtual OW_LoggerRef getLogger() const
		{
			return m_env->getLogger();
		}

        virtual OW_String getUserName() const
        {
            return OW_Platform::getCurrentUserName();
        }

	private:
		OW_UserInfo m_acl;
		OW_CIMOMEnvironmentRef m_env;
	};

	OW_ProviderEnvironmentIFCRef createProvEnvRef(const OW_UserInfo& acl,
		OW_CIMOMEnvironmentRef env)
	{
		return OW_ProviderEnvironmentIFCRef(new PollingManagerProviderEnvironment(
			acl, env));
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_PollingManager::run()
{
	// let OW_CIMOMEnvironment know we're running and ready to go.
	m_startedSem->signal();

	OW_Bool doInit = true;

	// Get all of the indication trigger providers
	OW_ProviderManagerRef pm = m_env->getProviderManager();
	
	OW_PolledProviderIFCRefArray itpra =
			pm->getPolledProviders(createProvEnvRef(OW_UserInfo(), m_env));

	m_env->logDebug(format("OW_PollingManager found %1 polled providers",
		itpra.size()));

	{
		// Get initial polling interval from all polled providers
		OW_NonRecursiveMutexLock ml(m_triggerGuard);

		for (size_t i = 0; i < itpra.size(); ++i)
		{
			TriggerRunnerRef tr(new TriggerRunner(this, OW_UserInfo(), m_env));

			tr->m_pollInterval = itpra[i]->getInitialPollingInterval(
				createProvEnvRef(OW_UserInfo(), m_env));

			m_env->logDebug(format("OW_PollingManager poll interval for provider"
				" %1: %2", i, tr->m_pollInterval));

			if(!tr->m_pollInterval)
			{
				continue;
			}

			tr->m_itp = itpra[i];
			m_triggerRunners.append(tr);
		}
	}

	{
		OW_NonRecursiveMutexLock l(m_triggerGuard);
		while (!m_shuttingDown)
		{
			OW_Bool rightNow;
			OW_UInt32 sleepTime = calcSleepTime(rightNow, doInit);
			doInit = false;

			if(!rightNow)
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
	m_triggerRunnerThreadPool->shutdown(false, 60);

	m_triggerRunners.clear();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_PollingManager::calcSleepTime(OW_Bool& rightNow, OW_Bool doInit)
{
	rightNow = false;
	OW_DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();
	time_t leastTime = LONG_MAX;
	int checkedCount = 0;

	for(size_t i = 0; i < m_triggerRunners.size(); i++)
	{
		if(m_triggerRunners[i]->m_isRunning
			|| m_triggerRunners[i]->m_pollInterval == 0)
		{
			continue;
		}

		if(doInit)
		{
			m_triggerRunners[i]->m_nextPoll =
				tm + m_triggerRunners[i]->m_pollInterval;
		}
		else if(m_triggerRunners[i]->m_nextPoll <= tm)
		{
			rightNow = true;
			return 0;
		}

		checkedCount++;
		time_t diff = m_triggerRunners[i]->m_nextPoll - tm;
		if(diff < leastTime)
		{
			leastTime = diff;
		}
	}

	return (checkedCount == 0) ? 0 : OW_UInt32(leastTime);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_PollingManager::processTriggers()
{
	OW_DateTime dtm;
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
				m_env->logCustInfo("Failed to run polled provider, because there are too many already running!");
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_PollingManager::shutdown()
{
	{
		OW_NonRecursiveMutexLock l(m_triggerGuard);
		m_shuttingDown = true;
		m_triggerCondition.notifyAll();
	}
	// wait until the main thread exits.
	this->join();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_PollingManager::addPolledProvider(const OW_PolledProviderIFCRef& p)
{
	OW_NonRecursiveMutexLock l(m_triggerGuard);
	if (m_shuttingDown)
		return;

	TriggerRunnerRef tr(new TriggerRunner(this, OW_UserInfo(), m_env));

	tr->m_pollInterval = p->getInitialPollingInterval(
		createProvEnvRef(OW_UserInfo(), m_env));

	m_env->logDebug(format("OW_PollingManager poll interval for provider"
		" %1", tr->m_pollInterval));

	if(!tr->m_pollInterval)
	{
		return;
	}

	OW_DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();
	tr->m_nextPoll = tm + tr->m_pollInterval;

	tr->m_itp = p;
	m_triggerRunners.append(tr);

    m_triggerCondition.notifyAll();
}

//////////////////////////////////////////////////////////////////////////////
OW_PollingManager::TriggerRunner::TriggerRunner(OW_PollingManager* svr,
	OW_UserInfo acl, OW_CIMOMEnvironmentRef env)
	: OW_Runnable()
	, m_itp(0)
	, m_nextPoll(0)
	, m_isRunning(false)
	, m_pollInterval(0)
	, m_pollMan(svr)
	, m_acl(acl)
	, m_env(env)
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_PollingManager::TriggerRunner::run()
{
	OW_Int32 nextInterval = 0;
	try
	{
		nextInterval = m_itp->poll(createProvEnvRef(m_acl, m_env));
	}
	catch(std::exception& e)
	{
		m_env->logError(OW_Format("Caught Exception while running poll: %1",
			e.what()));
	}
	catch(OW_ThreadCancelledException& e)
	{
		throw;
	}
	catch(...)
	{
		m_env->logError("Caught Unknown Exception while running poll");
	}

	OW_NonRecursiveMutexLock l(m_pollMan->m_triggerGuard);

	if(nextInterval == 0 || m_pollInterval == 0) // m_pollInterval == 0 means this poller has been instructed to stop
	{
		m_pollInterval = 0;
		m_nextPoll = 0;
	}
	else
	{
		if(nextInterval > 0)
		{
			m_pollInterval = nextInterval;
		}

		OW_DateTime dtm;
		dtm.setToCurrent();
		m_nextPoll = dtm.get() + m_pollInterval;
	}

	m_isRunning = false;
	m_pollMan->m_triggerCondition.notifyOne();
}


