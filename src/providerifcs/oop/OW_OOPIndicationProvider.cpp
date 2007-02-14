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
#include "OW_OOPIndicationProvider.hpp"
#include "OW_Logger.hpp"
#include "OW_OOPShuttingDownCallback.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

OOPIndicationProvider::OOPIndicationProvider(const OOPProviderInterface::ProvRegInfo& info,
	const OOPProcessState& processState)
	: OOPProviderBase(info, processState)
{

}

OOPIndicationProvider::~OOPIndicationProvider()
{

}

namespace
{

	class ActivateCallback : public OOPProviderBase::MethodCallback
	{
	public:
		ActivateCallback(
			const WQLSelectStatement& filter,
			const String& eventType,
			const String& nameSpace,
			const StringArray& classes,
			bool firstActivation)
			: m_filter(filter)
			, m_eventType(eventType)
			, m_nameSpace(nameSpace)
			, m_classes(classes)
			, m_firstActivation(firstActivation)
		{
		}
		
	private:
		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->activateFilter(out, in, timeout, env, m_filter, m_eventType, m_nameSpace, m_classes, m_firstActivation);
		}
	
		const WQLSelectStatement& m_filter;
		const String& m_eventType;
		const String& m_nameSpace;
		const StringArray& m_classes;
		bool m_firstActivation;
	};

} // end anonymous namespace
void
OOPIndicationProvider::activateFilter(
	const ProviderEnvironmentIFCRef& env,
	const WQLSelectStatement& filter,
	const String& eventType,
	const String& nameSpace,
	const StringArray& classes,
	bool firstActivation
	)
{
	OW_LOG_DEBUG(Logger("OOPIndicationProvider"), "OOPIndicationProvider::activateFilter");
	ActivateCallback filterCallback(filter, eventType, nameSpace, classes, firstActivation);
	startProcessAndCallFunction(env, filterCallback, "OOPMethodProvider::activateFilter");
}

namespace
{

	class AuthorizeFilterCallback : public OOPProviderBase::MethodCallback
	{
	public:
		AuthorizeFilterCallback(
			const WQLSelectStatement& filter,
			const String& eventType,
			const String& nameSpace,
			const StringArray& classes,
			const String& owner)
			: m_filter(filter)
			, m_eventType(eventType)
			, m_nameSpace(nameSpace)
			, m_classes(classes)
			, m_owner(owner)
		{
		}
		
	private:
		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->authorizeFilter(out, in, timeout, env, m_filter, m_eventType, m_nameSpace, m_classes, m_owner);
		}
	
		const WQLSelectStatement& m_filter;
		const String& m_eventType;
		const String& m_nameSpace;
		const StringArray& m_classes;
		const String& m_owner;
	};

} // end anonymous namespace

void
OOPIndicationProvider::authorizeFilter(
	const ProviderEnvironmentIFCRef& env,
	const WQLSelectStatement& filter,
	const String& eventType,
	const String& nameSpace,
	const StringArray& classes,
	const String& owner
	)
{
	OW_LOG_DEBUG(Logger("OOPIndicationProvider"), "OOPIndicationProvider::authorizeFilter");
	AuthorizeFilterCallback filterCallback(filter, eventType, nameSpace, classes, owner);
	startProcessAndCallFunction(env, filterCallback, "OOPMethodProvider::authorizeFilter");
}

namespace
{

	class DeActivateCallback : public OOPProviderBase::MethodCallback
	{
	public:
		DeActivateCallback(
			const WQLSelectStatement& filter,
			const String& eventType,
			const String& nameSpace,
			const StringArray& classes,
			bool lastActivation)
			: m_filter(filter)
			, m_eventType(eventType)
			, m_nameSpace(nameSpace)
			, m_classes(classes)
			, m_lastActivation(lastActivation)
		{
		}
		
	private:
		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->deActivateFilter(out, in, timeout, env, m_filter, m_eventType, m_nameSpace, m_classes, m_lastActivation);
		}
	
		const WQLSelectStatement& m_filter;
		const String& m_eventType;
		const String& m_nameSpace;
		const StringArray& m_classes;
		bool m_lastActivation;
	};

} // end anonymous namespace

void
OOPIndicationProvider::deActivateFilter(
	const ProviderEnvironmentIFCRef& env,
	const WQLSelectStatement& filter,
	const String& eventType,
	const String& nameSpace,
	const StringArray& classes,
	bool lastActivation
	)
{
	OW_LOG_DEBUG(Logger("OOPIndicationProvider"), "OOPIndicationProvider::deActivateFilter");
	DeActivateCallback filterCallback(filter, eventType, nameSpace, classes, lastActivation);
	startProcessAndCallFunction(env, filterCallback, "OOPIndicationProvider::deActivateFilter");
}

namespace
{

	class MustPollCallback : public OOPProviderBase::MethodCallback
	{
	public:
		MustPollCallback(
			int& retval,
			const WQLSelectStatement& filter,
			const String& eventType,
			const String& nameSpace,
			const StringArray& classes)
			: m_retval(retval)
			, m_filter(filter)
			, m_eventType(eventType)
			, m_nameSpace(nameSpace)
			, m_classes(classes)
		{
		}
		
	private:
		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			m_retval = protocol->mustPoll(out, in, timeout, env, m_filter, m_eventType, m_nameSpace, m_classes);
		}
	
		int& m_retval;
		const WQLSelectStatement& m_filter;
		const String& m_eventType;
		const String& m_nameSpace;
		const StringArray& m_classes;
	
	};

} // end anonymous namespace

int
OOPIndicationProvider::mustPoll(
	const ProviderEnvironmentIFCRef& env,
	const WQLSelectStatement& filter,
	const String& eventType,
	const String& nameSpace,
	const StringArray& classes
	)
{
	int retval;
	MustPollCallback mustPollCallback(retval, filter, eventType, nameSpace, classes);
	startProcessAndCallFunction(env, mustPollCallback, "OOPMethodProvider::mustPoll");
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
void
OOPIndicationProvider::shuttingDown(const ProviderEnvironmentIFCRef& env)
{
	OOPShuttingDownCallback shuttingDownCallback;
	startProcessAndCallFunction(env, shuttingDownCallback, "OOPIndicationProvider::shuttingDown");
}


} // end namespace OW_NAMESPACE







