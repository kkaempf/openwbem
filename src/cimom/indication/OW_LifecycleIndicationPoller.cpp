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
#include "OW_LifecycleIndicationPoller.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_CIMValue.hpp"

//////////////////////////////////////////////////////////////////////////////
OW_LifecycleIndicationPoller::OW_LifecycleIndicationPoller(
	const OW_String& ns, const OW_String& className,
	OW_UInt32 pollInterval)
	: m_ns(ns)
	, m_classname(className)
	, m_pollInterval(pollInterval)
	, m_pollCreation(0)
	, m_pollModification(0)
	, m_pollDeletion(0)
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LifecycleIndicationPoller::addPollOp(PollOp op)
{
	OW_MutexLock l(m_guard);
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
void
OW_LifecycleIndicationPoller::removePollOp(PollOp op)
{
	OW_MutexLock l(m_guard);
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
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_LifecycleIndicationPoller::willPoll() const
{
	return m_pollCreation > 0 || m_pollModification > 0 || m_pollDeletion > 0;
}
//////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_LifecycleIndicationPoller::addPollInterval(OW_UInt32 newPollInterval)
{
	OW_MutexLock l(m_guard);
	m_pollInterval = newPollInterval < m_pollInterval ? newPollInterval : m_pollInterval;
	return m_pollInterval;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_LifecycleIndicationPoller::getPollInterval()	const
{
	OW_MutexLock l(m_guard);
	return m_pollInterval;
}

namespace
{
//////////////////////////////////////////////////////////////////////////////
	class InstanceArrayBuilder : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		InstanceArrayBuilder(OW_CIMInstanceArray& cia_)
		: cia(cia_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &i)
		{
			cia.push_back(i);
		}
	private:
		OW_CIMInstanceArray& cia;
	};
} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_LifecycleIndicationPoller::getInitialPollingInterval(
	const OW_ProviderEnvironmentIFCRef &env)
{
	// do enumInstances to populate m_prevInsts
	InstanceArrayBuilder iab(m_prevInsts);
	env->getCIMOMHandle()->enumInstances(m_ns, m_classname, iab, false, false, true, true, 0);

	return getPollInterval();
}

namespace
{
	struct sortByInstancePath
	{
		bool operator()(const OW_CIMInstance& x, const OW_CIMInstance& y) const
		{
			return OW_CIMObjectPath(x) == OW_CIMObjectPath(y);
		}
	};
} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_LifecycleIndicationPoller::poll(const OW_ProviderEnvironmentIFCRef &env)
{
	if (!willPoll())
	{
		// nothing to do, so return 0 to stop polling.
		return 0;
	}

	// do enumInstances of the class
	OW_CIMInstanceArray curInstances;
	InstanceArrayBuilder iab(curInstances);
	OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
	try
	{
		hdl->enumInstances(m_ns, m_classname, iab, false, false, true, true, 0);
	}
	catch (const OW_CIMException& e)
	{
		env->getLogger()->logError(format("OW_LifecycleIndicationPoller::poll caught exception: %1", e));
		return 0;
	}

	// Compare the new instances with the previous instances
	// and send any indications that may be necessary.
	typedef OW_SortedVectorSet<OW_CIMInstance, sortByInstancePath> instSet_t;
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
				OW_CIMInstance expInst;
				expInst.setClassName("CIM_InstDeletion");
				expInst.setProperty("SourceInstance", OW_CIMValue(*pi));
				hdl->exportIndication(expInst, m_ns);
			}
			++pi;
		}
		else if (sortByInstancePath()(*ci, *pi))
		{
			// *ci is new
			if (m_pollCreation)
			{
				OW_CIMInstance expInst;
				expInst.setClassName("CIM_InstCreation");
				expInst.setProperty("SourceInstance", OW_CIMValue(*pi));
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
					OW_CIMInstance expInst;
					expInst.setClassName("CIM_InstModification");
					expInst.setProperty("PreviousInstance", OW_CIMValue(*pi));
					expInst.setProperty("SourceInstance", OW_CIMValue(*ci));
					hdl->exportIndication(expInst, m_ns);
				}
			}
			++pi;
			++ci;
		}
	}

	// save the current instances to m_prevInsts
	m_prevInsts = curInstances;

	return getPollInterval();
}



