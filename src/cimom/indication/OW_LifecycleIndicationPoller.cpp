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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_LifecycleIndicationPoller.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_CIMValue.hpp"
#include "OW_MutexLock.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
LifecycleIndicationPoller::LifecycleIndicationPoller(
	const String& ns, const String& className,
	UInt32 pollInterval)
	: m_ns(ns)
	, m_classname(className)
	, m_pollInterval(pollInterval)
	, m_pollCreation(0)
	, m_pollModification(0)
	, m_pollDeletion(0)
	, m_initializedInstances(false)
{
}
//////////////////////////////////////////////////////////////////////////////
void
LifecycleIndicationPoller::addPollOp(PollOp op)
{
	MutexLock l(m_guard);
	switch (op)
	{
		case POLL_FOR_INSTANCE_CREATION:
			++m_pollCreation;
			break;
		case POLL_FOR_INSTANCE_MODIFICATION:
			++m_pollModification;
			break;
		case POLL_FOR_INSTANCE_DELETION:
			++m_pollDeletion;
			break;
	}
}
//////////////////////////////////////////////////////////////////////////////
// takes a POLL_FOR_INSTANCE* flag
bool
LifecycleIndicationPoller::removePollOp(PollOp op)
{
	MutexLock l(m_guard);
	switch (op)
	{
		case POLL_FOR_INSTANCE_CREATION:
			--m_pollCreation;
			break;
		case POLL_FOR_INSTANCE_MODIFICATION:
			--m_pollModification;
			break;
		case POLL_FOR_INSTANCE_DELETION:
			--m_pollDeletion;
			break;
	}
	return !willPoll();
}
//////////////////////////////////////////////////////////////////////////////
bool
LifecycleIndicationPoller::willPoll() const
{
	MutexLock l(m_guard);
	return m_pollCreation > 0 || m_pollModification > 0 || m_pollDeletion > 0;
}
//////////////////////////////////////////////////////////////////////////////
UInt32
LifecycleIndicationPoller::addPollInterval(UInt32 newPollInterval)
{
	MutexLock l(m_guard);
	m_pollInterval = newPollInterval < m_pollInterval ? newPollInterval : m_pollInterval;
	return m_pollInterval;
}

//////////////////////////////////////////////////////////////////////////////
UInt32
LifecycleIndicationPoller::getPollInterval()	const
{
	MutexLock l(m_guard);
	return m_pollInterval;
}
namespace
{
//////////////////////////////////////////////////////////////////////////////
	class InstanceArrayBuilder : public CIMInstanceResultHandlerIFC
	{
	public:
		InstanceArrayBuilder(CIMInstanceArray& cia_)
		: cia(cia_)
		{}
	protected:
		virtual void doHandle(const CIMInstance &i)
		{
			cia.push_back(i);
		}
	private:
		CIMInstanceArray& cia;
	};
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
Int32
LifecycleIndicationPoller::getInitialPollingInterval(
	const ProviderEnvironmentIFCRef &)
{
	return 1; // have poll called again in 1 second.
}
namespace
{
struct sortByInstancePath
{
	bool operator()(const CIMInstance& x, const CIMInstance& y) const
	{
		return CIMObjectPath("", x) < CIMObjectPath("", y);
	}
};

const String COMPONENT_NAME("ow.owcimomd.indication.LifecyclePoller");
	
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
Int32
LifecycleIndicationPoller::poll(const ProviderEnvironmentIFCRef &env)
{
	LoggerRef logger(env->getLogger(COMPONENT_NAME));
	// do enumInstances to populate m_prevInsts
	if (!m_initializedInstances)
	{
		InstanceArrayBuilder iab(m_prevInsts);
		env->getCIMOMHandle()->enumInstances(m_ns, m_classname, iab, E_SHALLOW, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
		m_initializedInstances = true;
		return 1; // have poll called again in 1 second.
	}

	logger->logDebug(Format("LifecycleIndicationPoller::poll creation %1 modification %2 deletion %3", m_pollCreation, m_pollModification, m_pollDeletion));
	if (!willPoll())
	{
		// nothing to do, so return 0 to stop polling.
		logger->logDebug("LifecycleIndicationPoller::poll nothing to do, returning 0");
		return 0;
	}
	
	// do enumInstances of the class
	CIMInstanceArray curInstances;
	InstanceArrayBuilder iab(curInstances);
	CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
	try
	{
		hdl->enumInstances(m_ns, m_classname, iab, E_SHALLOW, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
	}
	catch (const CIMException& e)
	{
		logger->logError(Format("LifecycleIndicationPoller::poll caught exception: %1", e));
		return 0;
	}
	
	logger->logDebug(Format("LifecycleIndicationPoller::poll got %1 instances", curInstances.size()));
	// Compare the new instances with the previous instances
	// and send any indications that may be necessary.
	typedef SortedVectorSet<CIMInstance, sortByInstancePath> instSet_t;
	instSet_t prevSet(m_prevInsts.begin(), m_prevInsts.end());
	instSet_t curSet(curInstances.begin(), curInstances.end());
	typedef instSet_t::const_iterator iter_t;
	iter_t pi = prevSet.begin();
	iter_t ci = curSet.begin();
	while (pi != prevSet.end() && ci != curSet.end())
	{
		if (sortByInstancePath()(*pi, *ci))
		{
			// *pi has been deleted
			if (m_pollDeletion)
			{
				CIMInstance expInst;
				expInst.setClassName("CIM_InstDeletion");
				expInst.setProperty("SourceInstance", CIMValue(*pi));
				hdl->exportIndication(expInst, m_ns);
			}
			++pi;
		}
		else if (sortByInstancePath()(*ci, *pi))
		{
			// *ci is new
			if (m_pollCreation)
			{
				CIMInstance expInst;
				expInst.setClassName("CIM_InstCreation");
				expInst.setProperty("SourceInstance", CIMValue(*ci));
				hdl->exportIndication(expInst, m_ns);
			}
			++ci;
		}
		else // *pi == *ci
		{
			if (m_pollModification)
			{
				if (!pi->propertiesAreEqualTo(*ci))
				{
					CIMInstance expInst;
					expInst.setClassName("CIM_InstModification");
					expInst.setProperty("PreviousInstance", CIMValue(*pi));
					expInst.setProperty("SourceInstance", CIMValue(*ci));
					hdl->exportIndication(expInst, m_ns);
				}
			}
			++pi;
			++ci;
		}
	}
	while (pi != prevSet.end())
	{
		// *pi has been deleted
		if (m_pollDeletion)
		{
			CIMInstance expInst;
			expInst.setClassName("CIM_InstDeletion");
			expInst.setProperty("SourceInstance", CIMValue(*pi));
			hdl->exportIndication(expInst, m_ns);
		}
		++pi;
	}
	while (ci != curSet.end())
	{
		// *ci is new
		if (m_pollCreation)
		{
			CIMInstance expInst;
			expInst.setClassName("CIM_InstCreation");
			expInst.setProperty("SourceInstance", CIMValue(*ci));
			hdl->exportIndication(expInst, m_ns);
		}
		++ci;
	}
	
	// save the current instances to m_prevInsts
	m_prevInsts = curInstances;
	return getPollInterval();
}

} // end namespace OpenWBEM

