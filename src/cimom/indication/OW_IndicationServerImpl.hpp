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
#ifndef OW_INDICATION_SERVER_IMPL_HPP_
#define OW_INDICATION_SERVER_IMPL_HPP_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_CIMFwd.hpp"
#include "OW_Map.hpp"
#include "OW_Array.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_Thread.hpp"
#include "OW_ThreadEvent.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_IndicationServer.hpp"

class OW_IndicationServerImpl;

//////////////////////////////////////////////////////////////////////////////
struct OW_NotifyTrans
{
	OW_NotifyTrans() : m_indication(), m_handler(), m_provider(0) {}

	OW_NotifyTrans(const OW_CIMInstance& indication,
		const OW_CIMInstance& handler,
		const OW_IndicationExportProviderIFCRef provider) :
			m_indication(indication), m_handler(handler), m_provider(provider) {}

	OW_NotifyTrans(const OW_NotifyTrans& arg) : m_indication(arg.m_indication),
		m_handler(arg.m_handler), m_provider(arg.m_provider) {}

	OW_NotifyTrans& operator= (const OW_NotifyTrans& arg)
	{
		m_indication = arg.m_indication;
		m_handler = arg.m_handler;
		m_provider = arg.m_provider;
		return *this;
	}

	OW_CIMInstance m_indication;
	OW_CIMInstance m_handler;
	OW_IndicationExportProviderIFCRef m_provider;
};

//////////////////////////////////////////////////////////////////////////////
class OW_Notifier : public OW_Runnable
{
public:
	OW_Notifier(OW_IndicationServerImpl* pmgr, OW_NotifyTrans& ntrans) :
		m_pmgr(pmgr), m_trans(ntrans) {}

	void start();

	virtual void run();

private:
	OW_IndicationServerImpl* m_pmgr;
	OW_NotifyTrans m_trans;
};

//////////////////////////////////////////////////////////////////////////////
class OW_IndicationServerImpl : public OW_IndicationServer
{
public:
	enum
	{
		MAX_NOTIFIERS = 10
	};

	OW_IndicationServerImpl();
	~OW_IndicationServerImpl();

	virtual void init(OW_CIMOMEnvironmentRef env);

	virtual void run();
	void shutdown();

	void processIndication(const OW_CIMInstance& instance,
		const OW_CIMNameSpace& instNS);

	int getRunCount() { return m_runCount; }

	OW_CIMOMEnvironmentRef getEnvironment() const { return m_env; }

private:

	void _processIndication(const OW_CIMInstance& instance,
		const OW_CIMNameSpace& instNS);


	void addTrans(const OW_CIMInstance& indication, OW_CIMInstance& handler,
		OW_IndicationExportProviderIFCRef provider);

	OW_IndicationExportProviderIFCRef getProvider(const OW_String& className);

	OW_Bool notifyDone(OW_NotifyTrans& outTrans);

	struct ProcIndicationTrans
	{
		ProcIndicationTrans(const OW_CIMInstance& inst,
			const OW_CIMNameSpace& ns)
			: instance(inst)
			, nameSpace(ns) {}

		ProcIndicationTrans(const ProcIndicationTrans& arg)
			: instance(arg.instance)
			, nameSpace(arg.nameSpace) {}

		ProcIndicationTrans& operator= (const ProcIndicationTrans& arg)
		{
			instance = arg.instance;
			nameSpace = arg.nameSpace;
			return *this;
		}

		OW_CIMInstance instance;
		OW_CIMNameSpace nameSpace;
	};

	int m_runCount;
	OW_Map<OW_String, OW_IndicationExportProviderIFCRef> m_providers;
	OW_Array<OW_NotifyTrans> m_trans;
	OW_Mutex m_guard;
	OW_Bool m_shuttingDown;
	OW_ThreadEvent m_wakeEvent;
	OW_Mutex m_procTransGuard;
	OW_Array<ProcIndicationTrans> m_procTrans;
	OW_Bool m_running;
	OW_CIMOMEnvironmentRef m_env;

	friend class OW_Notifier;
};

#endif	// __OW_NOTIFYMANAGER_HPP__


