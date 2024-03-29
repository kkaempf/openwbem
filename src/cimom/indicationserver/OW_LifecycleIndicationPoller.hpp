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
 * @author Dan Nuffer
 */

#ifndef OW_LIFE_CYCLE_INDICATION_POLLER_HPP_INCLUDE_GUARD_
#define OW_LIFE_CYCLE_INDICATION_POLLER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/String.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "blocxx/Mutex.hpp"

namespace OW_NAMESPACE
{

class LifecycleIndicationPoller : public CppPolledProviderIFC
{
public:
	LifecycleIndicationPoller(const blocxx::String& ns, const CIMName& className,
		blocxx::UInt32 pollInterval);
	virtual blocxx::Int32 getInitialPollingInterval(const ProviderEnvironmentIFCRef &env);
	virtual blocxx::Int32 poll(const ProviderEnvironmentIFCRef &env);
	// used to determine what types of indications to create
	enum PollOp
	{
		POLL_FOR_INSTANCE_CREATION,
		POLL_FOR_INSTANCE_MODIFICATION,
		POLL_FOR_INSTANCE_DELETION
	};
	// takes a POLL_FOR_INSTANCE* flag and indicates a provider has requested
	// we poll for it.
	void addPollOp(PollOp op);
	// takes a POLL_FOR_INSTANCE* flag and indicates a provider has requested
	// we poll for it, and the subscription was removed.
	// returns !willPoll().
	bool removePollOp(PollOp op);
	// returns true if the there are any poll operations to do
	bool willPoll() const;
	// a new poll interval will be considered.  The new interval will be the
	// min of newPollInterval and the current poll interval.  The new interval
	// is returned.
	blocxx::UInt32 addPollInterval(blocxx::UInt32 newPollInterval);
private:
	blocxx::UInt32 getPollInterval() const;
	blocxx::String m_ns;
	CIMName m_classname;
	blocxx::UInt32 m_pollInterval;
	blocxx::UInt32 m_pollCreation;
	blocxx::UInt32 m_pollModification;
	blocxx::UInt32 m_pollDeletion;
	mutable blocxx::Mutex m_guard;
	CIMInstanceArray m_prevInsts;
	bool m_initializedInstances;
};

} // end namespace OW_NAMESPACE

#endif
