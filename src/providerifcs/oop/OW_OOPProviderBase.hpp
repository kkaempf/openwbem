/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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

#ifndef OW_OOP_PROVIDER_BASE_HPP_INCLUDE_GUARD_
#define OW_OOP_PROVIDER_BASE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_OOPProviderInterface.hpp"
#include "OW_OOPFwd.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_OOPFwd.hpp"
#include "blocxx/ThreadPool.hpp"
#include "blocxx/TimeoutTimer.hpp"
#include "OW_OOPProcessState.hpp"
#include "blocxx/NonRecursiveMutex.hpp"

namespace OW_NAMESPACE
{

class OOPProviderBase
{
public:
	OOPProviderBase(const OOPProviderInterface::ProvRegInfo& info,
		const OOPProcessState& processState);

	virtual ~OOPProviderBase();

	blocxx::UnnamedPipeRef startClonedProviderEnv(const ProviderEnvironmentIFCRef& env);
	void terminate(const ProviderEnvironmentIFCRef& env, const blocxx::String& providerID);

	class MethodCallback
	{
	public:
		virtual ~MethodCallback();
		virtual void call(const OOPProtocolIFCRef& protocol, const blocxx::UnnamedPipeRef& out, const blocxx::UnnamedPipeRef& in,
			const blocxx::Timeout& timeout, const ProviderEnvironmentIFCRef& env) const = 0;
	};

	const OOPProviderInterface::ProvRegInfo& getProvInfo() const
	{
		return m_provInfo;
	}

	bool unloadTimeoutExpired();

	bool haveUnloadTimeout();

protected:

	const OOPProtocolIFCRef& getProtocol() const
	{
		return m_protocol;
	}

	enum EUsePersistentProcessFlag
	{
		E_USE_PERSISTENT_PROCESS,
		E_SPAWN_NEW_PROCESS
	};
	void startProcessAndCallFunction(const ProviderEnvironmentIFCRef& env, const MethodCallback& func, const char* fname);

private:
	blocxx::ThreadSafeProcessRef getProcess(const char* fname, const ProviderEnvironmentIFCRef& env, EUsePersistentProcessFlag usePersistentProcess, blocxx::String& procUserName);

	void resetUnloadTimer();

	OOPProviderInterface::ProvRegInfo m_provInfo;
	OOPProtocolIFCRef m_protocol;

	OOPProcessState m_processState;
	blocxx::ThreadPool m_threadPool;
	blocxx::NonRecursiveMutex m_unloadTimerGuard;
	blocxx::TimeoutTimer m_unloadTimer;
};

} // end namespace OW_NAMESPACE


#endif


