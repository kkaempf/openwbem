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

#include "OW_config.h"
#include "OW_PollingManager.hpp"
#include "OW_MutexLock.hpp"
#include "OW_DateTime.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_ACLInfo.hpp"

#include <climits>

//////////////////////////////////////////////////////////////////////////////
OW_PollingManager::OW_PollingManager(OW_CIMOMEnvironmentRef env)
	: OW_Thread(true) // true means this thread will be joinable
	, m_shuttingDown(false)
	, m_threadCount(new OW_ThreadCounter(256)) // TODO: Make this maximum configurable
	, m_env(env)
{
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

		PollingManagerProviderEnvironment(const OW_ACLInfo& acl,
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

		virtual OW_String getConfigItem(const OW_String& name) const
		{
			return m_env->getConfigItem(name);
		}
		
		virtual OW_LoggerRef getLogger() const
		{
			return m_env->getLogger();
		}

	private:
		OW_ACLInfo m_acl;
		OW_CIMOMEnvironmentRef m_env;
	};

	OW_ProviderEnvironmentIFCRef createProvEnvRef(const OW_ACLInfo& acl,
		OW_CIMOMEnvironmentRef env)
	{
		return OW_ProviderEnvironmentIFCRef(new PollingManagerProviderEnvironment(
			acl, env));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_PollingManager::run()
{
	// let OW_CIMOMEnvironment know we're running and ready to go.
	m_startedSem->signal();

	OW_Bool doInit = true;

	// Get all of the indication trigger providers
	OW_ProviderManagerRef pm = m_env->getProviderManager();
	
	OW_PolledProviderIFCRefArray itpra =
			pm->getPolledProviders(createProvEnvRef(OW_ACLInfo(), m_env));

	m_env->logDebug(format("OW_PollingManager found %1 polled providers",
		itpra.size()));

	{
		// Get initial polling interval from all polled providers
		OW_MutexLock ml(m_triggerGuard);

		for (size_t i = 0; i < itpra.size(); ++i)
		{
			TriggerRunner tr(this, OW_ACLInfo(), m_env);

			tr.m_pollInterval = itpra[i]->getInitialPollingInterval(
				createProvEnvRef(OW_ACLInfo(), m_env));

			m_env->logDebug(format("OW_PollingManager poll interval for provider"
				" %1: %2", i, tr.m_pollInterval));

			if(!tr.m_pollInterval)
			{
				continue;
			}

			tr.m_itp = itpra[i];
			m_triggerRunners.append(tr);
		}
	}

	{
		OW_MutexLock l(m_triggerGuard);
		while (!m_shuttingDown)
		{
			OW_Bool rightNow;
			OW_UInt32 sleepTime = calcSleepTime(rightNow, doInit);
			doInit = false;

			if(!rightNow)
			{
				m_triggerCondition.timedWait(l, sleepTime);
			}

			if (m_shuttingDown)
			{
				break;
			}

			processTriggers();
		}
	}

	// wait until all the threads exit
	m_threadCount->waitForAll();

	m_triggerRunners.clear();
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

	OW_MutexLock ml(m_triggerGuard);

	for(size_t i = 0; i < m_triggerRunners.size(); i++)
	{
		if(m_triggerRunners[i].m_isRunning
			|| m_triggerRunners[i].m_pollInterval == 0)
		{
			continue;
		}

		if(doInit)
		{
			m_triggerRunners[i].m_nextPoll =
				tm + m_triggerRunners[i].m_pollInterval;
		}
		else if(m_triggerRunners[i].m_nextPoll <= tm)
		{
			rightNow = true;
			return 0;
		}

		checkedCount++;
		time_t diff = m_triggerRunners[i].m_nextPoll - tm;
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
	OW_MutexLock ml(m_triggerGuard);

	OW_DateTime dtm;
	dtm.setToCurrent();
	time_t tm = dtm.get();

	for (size_t i = 0; i < m_triggerRunners.size(); i++)
	{
		if(m_triggerRunners[i].m_isRunning
			|| m_triggerRunners[i].m_pollInterval == 0)
		{
			continue;
		}

		if (tm >= m_triggerRunners[i].m_nextPoll)
		{
			m_threadCount->incThreadCount();
			m_triggerRunners[i].start();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_PollingManager::shutdown()
{
	{
		OW_MutexLock l(m_triggerGuard);
		m_shuttingDown = true;
		m_triggerCondition.notifyAll();
	}
	// wait until the main thread exits.
	this->join();
}

//////////////////////////////////////////////////////////////////////////////
OW_PollingManager::TriggerRunner::TriggerRunner(OW_PollingManager* svr,
	OW_ACLInfo acl, OW_CIMOMEnvironmentRef env)
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
OW_PollingManager::TriggerRunner::start()
{
	m_isRunning = true;
	OW_RunnableRef rref(this, true);
	OW_Bool isSepThread = !m_env->getConfigItem(
		OW_ConfigOpts::SINGLE_THREAD_opt).equalsIgnoreCase("true");
	OW_Thread::run(rref, isSepThread, OW_ThreadDoneCallbackRef(new OW_ThreadCountDecrementer(m_pollMan->m_threadCount)));
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
	catch(...)
	{
		m_env->logError("Caught Unknown Exception while running poll");
	}

	if(nextInterval == 0)
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

	OW_MutexLock l(m_pollMan->m_triggerGuard);
	m_isRunning = false;
	m_pollMan->m_triggerCondition.notifyOne();
}


