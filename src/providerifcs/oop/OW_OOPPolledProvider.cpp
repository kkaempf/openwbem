/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_OOPPolledProvider.hpp"
#include "OW_OOPShuttingDownCallback.hpp"
#include "OW_OOPProtocolIFC.hpp"

namespace OW_NAMESPACE
{

OOPPolledProvider::OOPPolledProvider(const OOPProviderInterface::ProvRegInfo& info,
	const OOPProcessState& processState)
	: OOPProviderBase(info, processState)
{

}
	
OOPPolledProvider::~OOPPolledProvider()
{

}
	
namespace
{
	class PollCallback : public OOPProviderBase::MethodCallback
	{
	public:
		PollCallback(
			Int32& retval)
			: m_retval(retval)
		{
		}
		
	private:
		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			m_retval = protocol->poll(out, in, timeout, env);
		}
	
		Int32& m_retval;
	};

	class GetInitialPollingIntervalCallback : public OOPProviderBase::MethodCallback
	{
	public:
		GetInitialPollingIntervalCallback(
			Int32& retval)
			: m_retval(retval)
		{
		}
		
	private:
		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			m_retval = protocol->getInitialPollingInterval(out, in, timeout, env);
		}
	
		Int32& m_retval;
	};
}

Int32
OOPPolledProvider::poll(const ProviderEnvironmentIFCRef& env)
{
	Int32 retval(-1);
	PollCallback pollCallback(retval);
	startProcessAndCallFunction(env, pollCallback, "OOPPolledProvider::poll");
	return retval;
}

Int32
OOPPolledProvider::getInitialPollingInterval(const ProviderEnvironmentIFCRef& env)
{
	Int32 retval(-1);
	GetInitialPollingIntervalCallback getInitialPollingIntervalCallback(retval);
	startProcessAndCallFunction(env, getInitialPollingIntervalCallback, "OOPPolledProvider::getInitialPollingInterval");
	return retval;
}

void
OOPPolledProvider::doShutdown()
{
	// TODO: Figure out if doing anything here is necessary.
}

void
OOPPolledProvider::doCooperativeCancel()
{
}

void
OOPPolledProvider::doDefinitiveCancel()
{
}

void
OOPPolledProvider::shuttingDown(const ProviderEnvironmentIFCRef& env)
{
	OOPShuttingDownCallback shuttingDownCallback;
	startProcessAndCallFunction(env, shuttingDownCallback, "OOPPolledProvider::shuttingDown");
}


} // end namespace OW_NAMESPACE








