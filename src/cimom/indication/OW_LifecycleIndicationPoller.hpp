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
#ifndef OW_LIFE_CYCLE_INDICATION_POLLER_HPP_INCLUDE_GUARD_
#define OW_LIFE_CYCLE_INDICATION_POLLER_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Thread.hpp"
#include "OW_String.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CppPolledProviderIFC.hpp"

class OW_LifecycleIndicationPoller : public OW_CppPolledProviderIFC
{
public:
	OW_LifecycleIndicationPoller(const OW_String& ns, const OW_String& className,
		OW_UInt32 pollInterval, OW_UInt32 pollOps);

	virtual OW_Int32 getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef &env);
	virtual OW_Int32 poll(const OW_ProviderEnvironmentIFCRef &env);

	// used to determine what types of indications to create
	enum
	{
		POLL_FOR_INSTANCE_CREATION = 1,
		POLL_FOR_INSTANCE_MODIFICATION = 2,
		POLL_FOR_INSTANCE_DELETION = 4
	};

	// takes a POLL_FOR_INSTANCE* flag and adds it to the current set
	// returns the current flags
	OW_UInt32 addPollOp(OW_UInt32 op);

	// takes a POLL_FOR_INSTANCE* flag and removes it from the current set
	// returns the current flags
	OW_UInt32 removePollOp(OW_UInt32 op);

	// a new poll interval will be considered.  The new interval will be the
	// min of newPollInterval and the current poll interval.  The new interval
	// is returned.
	OW_UInt32 addPollInterval(OW_UInt32 newPollInterval);

private:
	OW_UInt32 getPollOps() const;
	OW_UInt32 getPollInterval() const;

	OW_String m_ns;
	OW_String m_classname;
	OW_UInt32 m_pollInterval;
	OW_UInt32 m_pollOps;
	mutable OW_Mutex m_guard;
	OW_CIMInstanceArray m_prevInsts;
};


#endif


