/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_OOPProviderBase.hpp"
#include "OW_OOPProtocolCPP1.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_Process.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_ThreadCancelledException.hpp"
#include "OW_IOException.hpp"
#include "OW_CIMException.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Logger.hpp"
#include "OW_OpenWBEM_OOPProviderRegistration.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_UserUtils.hpp"
#include "OW_Secure.hpp"
#include "OW_OperationContext.hpp"
#include "OW_UserInfo.hpp"
#include "OW_Exec.hpp"
#include "OW_OOPClonedProviderEnv.hpp"
#include "OW_Assertion.hpp"

namespace OW_NAMESPACE
{

namespace
{
	OW_DECLARE_EXCEPTION(OOPProviderBase)
	OW_DEFINE_EXCEPTION(OOPProviderBase)
	const String COMPONENT_NAME("ow.provider.OOP.ifc");


	String getStderr(const ProcessRef& proc)
	{
		// something went wrong, now we'll check stderr
		proc->err()->setReadTimeout(Timeout::relative(1.0));
		String output;
		Logger lgr(COMPONENT_NAME);
		try
		{
			output = proc->err()->readAll();
		}
		catch (IOException& e)
		{
			OW_LOG_ERROR(lgr, "Failed reading stderr");
			// ignore it
		}
		if (!output.empty())
		{
			OW_LOG_ERROR(lgr, Format("Communication with process failed.  error output = %1", output));
		}
		return output;
	}

	void processFinish(const ProcessRef& proc, const char* fname, const String& procName)
	{
		Process::Status status = proc->processStatus();
		Logger lgr(COMPONENT_NAME);
		OW_LOG_DEBUG(lgr, Format("%1 got exit status: %2", fname, status.toString()));
		if (!status.running() && !status.terminatedSuccessfully())
		{
			String output = getStderr(proc);
			String msg = Format("%1 failed. exitStatus = %2, stderr output = %3", 
				procName, status.toString(), output);
			OW_LOG_ERROR(lgr, msg);
			OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
		}

		// still running, so shut it down. 0 for second parameter because we don't want
		// to close the pipes. Later we try to read stderr if the process failed.
		OW_LOG_DEBUG(lgr, Format("Cleaning up process \"%1\" (%2)", procName, proc->pid()));
		proc->waitCloseTerm(Timeout::relative(10.0), Timeout::relative(0), Timeout::relative(10.1));
		status = proc->processStatus();

		if (!status.terminatedSuccessfully())
		{
			String output = getStderr(proc);
			String msg = Format("%1 failed. exitStatus = %2, stderr output = %3", 
				procName, status.toString(), output);
			OW_LOG_ERROR(lgr, msg);
			OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
		}
	}

} // end unnamed namespace



OOPProviderBase::OOPProviderBase(const OOPProviderInterface::ProvRegInfo& info,
		const Reference<RWLocker>& guardRef,
		const Reference<ProcessRef>& persistentProcessRef,
		const Reference<String>& persistentProcessUserNameRef
		)
	: m_provInfo(info)
	, m_guardRef(guardRef)
	, m_persistentProcessRef(persistentProcessRef)
	, m_persistentProcessUserNameRef(persistentProcessUserNameRef)
	, m_threadPool(ThreadPool::DYNAMIC_SIZE_NO_QUEUE, 10, 10, "OOPProviderBase")
	, m_unloadTimer(info.unloadTimeout)
{
	// persistent provider instances must have non-null pointers.
	if (!m_guardRef || !m_persistentProcessRef || !m_persistentProcessUserNameRef)
	{
		OW_ASSERT(m_provInfo.providerNeedsNewProcessForEveryInvocation() == true);
		m_provInfo.isPersistent = false;
	}

	if (info.protocol == "owcpp1")
	{
		m_protocol = new OOPProtocolCPP1(this);
	}
	else
	{
		OW_THROW(OOPProviderBaseException, Format("Invalid protocol: %1", info.protocol).c_str());
	}
}

OOPProviderBase::~OOPProviderBase()
{
	m_threadPool.shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE,
		Timeout::relative(0.1), Timeout::infinite);
}

inline void OOPProviderBase::resetUnloadTimer()
{
	m_unloadTimer.resetOnLoop();
}

UnnamedPipeRef
OOPProviderBase::startClonedProviderEnv(
	const ProviderEnvironmentIFCRef& env)
{
	UnnamedPipeRef connToKeep;	// This one gets returned to caller. Descriptors will be used by the client
	UnnamedPipeRef connToSend;	// This one gets used by the provider environment.

	UnnamedPipe::createConnectedPipes(connToKeep, connToSend);
	connToKeep->setTimeouts(Timeout::infinite);
	ProviderEnvironmentIFCRef clonedEnv(env->clone());
	RunnableRef newConnection;

	if (m_provInfo.protocol == "owcpp1")
	{
		newConnection = RunnableRef(new OOPClonedProviderEnv(this, connToKeep, clonedEnv));
	}
	else
	{
		OW_THROW(OOPProviderBaseException, Format("Invalid protocol: %1", m_provInfo.protocol).c_str());
	}

	if (m_threadPool.tryAddWork(newConnection, Timeout::relative(10)))
	{
		return connToSend;
	}

	connToKeep->close();
	connToSend->close();
	return UnnamedPipeRef();
}

ProcessRef
OOPProviderBase::getProcess(const char* fname, const ProviderEnvironmentIFCRef& env, EUsePersistentProcessFlag usePersistentProcess, String& procUserName)
{
	Logger lgr(COMPONENT_NAME);

	// figure out procUserName
	switch (m_provInfo.userContext)
	{
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_SUPER_USER:
		{
#if OW_WIN32
			const char superUser[] = "SYSTEM";
#else
			const char superUser[] = "root";
#endif
			procUserName = superUser;
		}
		break;
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION:
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_UNPRIVILEGED:
		{
			String currentUserName = UserUtils::getCurrentUserName();
			procUserName = currentUserName;
			if (m_provInfo.userContext == OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION)
			{
				String operationUserName = env->getOperationContext().getUserInfo().getUserName();
				if (!operationUserName.empty())
				{
					procUserName = operationUserName;
				}
			}

		}
		break;
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION_MONITORED:
		{
			String currentUserName = UserUtils::getCurrentUserName();
			procUserName = currentUserName;
			String operationUserName = env->getOperationContext().getUserInfo().getUserName();
			if (!operationUserName.empty())
			{
				procUserName = operationUserName;
			}
		}
		break;
	}

	if (usePersistentProcess == E_USE_PERSISTENT_PROCESS && *m_persistentProcessRef)
	{
		// if the user of the persistent process has changed, we need to kill the old process and start a new one.
		if ((*m_persistentProcessRef)->processStatus().running() && procUserName == *m_persistentProcessUserNameRef)
		{
			OW_LOG_DEBUG(lgr, Format("Using persistent process %1", (*m_persistentProcessRef)->pid()));
			return *m_persistentProcessRef;
		}
		else
		{
			String output = getStderr(*m_persistentProcessRef);
			OW_LOG_ERROR(lgr, Format("Detected that persistent provider process %1[%2] has terminated: %3, stderr output = %4", 
				m_provInfo.process, (*m_persistentProcessRef)->pid(), (*m_persistentProcessRef)->processStatus().toString(), output));

			*m_persistentProcessRef = ProcessRef();
		}
	}


	// launch the process
	OW_LOG_DEBUG(lgr, Format("%1 about to spawn %2", fname, m_provInfo.process));
	for (size_t i = 0; i < m_provInfo.args.size(); ++i)
	{
		OW_LOG_DEBUG(lgr, Format("param %1=\"%2\"", i, m_provInfo.args[i]));
	}
	// argv array has to start with the process
	StringArray argv(m_provInfo.args);
	argv.insert(argv.begin(), m_provInfo.process);

	ProcessRef proc;

	PrivilegeManager privMan = PrivilegeManager::getPrivilegeManager();
	switch (m_provInfo.userContext)
	{
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_SUPER_USER:
		{
			proc = privMan.userSpawn(m_provInfo.process, argv, Secure::minimalEnvironment(), procUserName);
		}
		break;
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION:
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_UNPRIVILEGED:
		{
			String currentUserName = UserUtils::getCurrentUserName();
			if (currentUserName == procUserName)
			{
				proc = Exec::spawn(argv);
			}
			else // the user is different
			{
				proc = privMan.userSpawn(m_provInfo.process, argv, Exec::currentEnvironment, procUserName);
			}

		}
		break;
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_MONITORED:
		{
			proc = privMan.monitoredSpawn(
				m_provInfo.process, m_provInfo.monitorPrivilegesFile, argv,
				Exec::currentEnvironment
			);
		}
		break;
		case OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_OPERATION_MONITORED:
		{
			proc = privMan.monitoredUserSpawn(
				m_provInfo.process, m_provInfo.monitorPrivilegesFile, argv,
				Exec::currentEnvironment, procUserName); 
		}
		break;
	}

	OW_LOG_DEBUG(lgr, Format("%1: %2 was spawned with pid %3", fname, m_provInfo.process, proc->pid()));

	proc->in()->setTimeouts(m_provInfo.timeout);
	proc->out()->setTimeouts(m_provInfo.timeout);
	proc->err()->setTimeouts(m_provInfo.timeout);

	if (usePersistentProcess == E_USE_PERSISTENT_PROCESS)
	{
		m_protocol->setPersistent(proc->out(), proc->in(), m_provInfo.timeout, env, true);
	}

	return proc;
}

void
OOPProviderBase::terminate(const ProviderEnvironmentIFCRef& env, const String& providerID)
{
	Logger lgr(COMPONENT_NAME);
	ProcessRef proc;
	if (m_persistentProcessRef && (*m_persistentProcessRef))
	{
		OW_LOG_DEBUG(lgr, Format("OOPProviderBase::terminate terminating provider: %1", providerID));
		ProcessRef proc = *m_persistentProcessRef;
		m_protocol->setPersistent(proc->out(), proc->in(), m_provInfo.timeout, env, false);
	}
	else
	{
		OW_LOG_DEBUG(lgr, Format("OOPProviderBase::terminate provider: %1 is not running", providerID));
	}

	// Shutdown the clonedEnv threadPool
	m_threadPool.shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, Timeout::relative(0.1), Timeout::infinite);
}

void 
OOPProviderBase::startProcessAndCallFunction(const ProviderEnvironmentIFCRef& env, const OOPProviderBase::MethodCallback& func, 
	const char* fname)
{
	Logger lgr(COMPONENT_NAME);

	try
	{
		EUsePersistentProcessFlag usePersistentProcess = E_USE_PERSISTENT_PROCESS;
		if (m_provInfo.providerNeedsNewProcessForEveryInvocation())
		{
			 usePersistentProcess = E_SPAWN_NEW_PROCESS;
		}

		bool doLock = (usePersistentProcess == E_USE_PERSISTENT_PROCESS);
		// This lock guards modification of m_persistentProcess as well as serializing 
		// communication requests to it.
		Reference<WriteLock> lock;
		if (doLock)
		{
			lock = new WriteLock(*m_guardRef, m_provInfo.timeout);
		}

		resetUnloadTimer();

		String procUserName;
		ProcessRef proc = getProcess(fname, env, usePersistentProcess, procUserName);

		try
		{
			func.call(m_protocol, proc->out(), proc->in(), m_provInfo.timeout, env);
		}
		catch (ThreadCancelledException&)
		{
			throw;
		}
		catch (Exception& e)
		{
			OW_LOG_DEBUG(lgr, Format("%1 caught %2", fname, e));
			// If we got a CIMException, it was thrown by the provider, so we don't want to terminate a persistent process.
			// If we got any other exception, something bad happened, so terminate it.
			if (e.getSubClassId() == ExceptionIds::CIMExceptionId && usePersistentProcess == E_USE_PERSISTENT_PROCESS)
			{
				*m_persistentProcessRef = proc;
				*m_persistentProcessUserNameRef = procUserName;
				throw;
			}
			else
			{
				OW_LOG_DEBUG(lgr, Format("Forcing process \"%1\" (%2) to finish: Not a persistent process (func threw exception)", m_provInfo.process, proc->pid()));
				processFinish(proc, fname, m_provInfo.process);
				throw;
			}
		}

		if (usePersistentProcess == E_USE_PERSISTENT_PROCESS)
		{
			*m_persistentProcessRef = proc;
			*m_persistentProcessUserNameRef = procUserName;
		}
		else
		{
			OW_LOG_DEBUG(lgr, Format("Forcing process \"%1\" (%2) to finish: Not a persistent process (func finished)", m_provInfo.process, proc->pid()));
			processFinish(proc, fname, m_provInfo.process);
		}
	}
	catch (ProcessErrorException& e)
	{
		OW_LOG_DEBUG(lgr, Format("%1 caught ProcessErrorException: %2", fname, e));
		OW_THROWCIMMSG(CIMException::FAILED, Format("Failed to run %1: %2", m_provInfo.process, e).c_str());
	}
}

bool
OOPProviderBase::unloadTimeoutExpired()
{
	OW_ASSERT(m_guardRef);
	WriteLock lock(*m_guardRef, m_provInfo.timeout);
	m_unloadTimer.loop();
	return m_unloadTimer.expired();
}


OOPProviderBase::MethodCallback::~MethodCallback()
{

}

} // end namespace OW_NAMESPACE





