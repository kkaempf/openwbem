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
#include "OW_IndicationServerImpl.hpp"
#include "OW_CIM.hpp"
#include "OW_DateTime.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_ACLInfo.hpp"

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class IndicationServerProviderEnvironment : public OW_ProviderEnvironmentIFC
	{
	public:

		IndicationServerProviderEnvironment(const OW_CIMOMHandleIFCRef& ch,
			OW_CIMOMEnvironmentRef env)
			: OW_ProviderEnvironmentIFC()
			, m_ch(ch)
			, m_env(env)
		{}

		virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_ch;
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
		OW_CIMOMHandleIFCRef m_ch;
		OW_CIMOMEnvironmentRef m_env;
	};

	OW_ProviderEnvironmentIFCRef createProvEnvRef(OW_CIMOMEnvironmentRef env,
		const OW_CIMOMHandleIFCRef& ch)
	{
		return OW_ProviderEnvironmentIFCRef(
			new IndicationServerProviderEnvironment(ch, env));
	}

	class runCountDecrementer : public OW_ThreadDoneCallback
	{
	public:
		runCountDecrementer(OW_IndicationServerImpl* i_)
		: i(i_)
		{}
	protected:
		virtual void doNotifyThreadDone(OW_Thread *)
		{
			i->decRunCount();
		}
	private:
		OW_IndicationServerImpl* i;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_Notifier::start()
{
	OW_CIMOMEnvironmentRef eref = m_pmgr->getEnvironment();

	OW_Bool singleThread = eref->getConfigItem(
		OW_ConfigOpts::SINGLE_THREAD_opt).equalsIgnoreCase("true");

	OW_Thread::run(OW_RunnableRef(this), !singleThread,
		OW_ThreadDoneCallbackRef(new runCountDecrementer(m_pmgr)));
}


//////////////////////////////////////////////////////////////////////////////
void
OW_Notifier::run()
{
	OW_ACLInfo aclInfo;
	OW_CIMOMEnvironmentRef env = m_pmgr->getEnvironment();
	OW_CIMOMHandleIFCRef lch = env->getCIMOMHandle(aclInfo, false);

	while(true)
	{
		try
		{
			m_trans->m_provider->exportIndication(createProvEnvRef(
				m_pmgr->getEnvironment(), lch), m_trans->m_ns, m_trans->m_handler,
				m_trans->m_indication);
		}
		catch(OW_Exception& e)
		{
			env->logError(format("%1 caught in OW_Notifier::run", e.type()));
			env->logError(format("File: %1", e.getFile()));
			env->logError(format("Line: %1", e.getLine()));
			env->logError(format("Msg: %1", e.getMessage()));
		}
		catch(...)
		{
			env->logError("Unknown exception caught in OW_Notifier::run");
		}

		// delete the transaction before we notifyDone, to avoid a race
		// condition between the transaction destructor and the CIMOM unloading
		// the indication library.
		m_trans.reset();
		OW_NotifyTrans* t = 0;
		if(!m_pmgr->notifyDone(t))
		{
			break;
		}
		m_trans = t;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationServerImpl::OW_IndicationServerImpl()
	: OW_IndicationServer()
	, m_runCount(0)
	, m_shuttingDown(false)
	, m_running(false)
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::init(OW_CIMOMEnvironmentRef env)
{
	m_env = env;
	OW_ACLInfo aclInfo;

	//-----------------
	// Load map with available indication export providers
	//-----------------
	OW_ProviderManagerRef pProvMgr = m_env->getProviderManager();

	OW_CIMOMHandleIFCRef lch = m_env->getCIMOMHandle(aclInfo, false);

	OW_IndicationExportProviderIFCRefArray pra =
		pProvMgr->getIndicationExportProviders(createProvEnvRef(m_env, lch));

	m_env->logDebug(format("OW_IndicationServerImpl: %1 export providers found",
		pra.size()));

	for(size_t i = 0; i < pra.size(); i++)
	{
		OW_StringArray clsNames = pra[i]->getHandlerClassNames();
		for(size_t j = 0; j < clsNames.size(); j++)
		{
			OW_String lowerClsName = clsNames[j];
			lowerClsName.toLowerCase();
			m_providers[lowerClsName] = pra[i];
			m_env->logDebug(format("OW_IndicationServerImpl: Handling"
				" indication type %1", clsNames[j]));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationServerImpl::~OW_IndicationServerImpl()
{
	m_trans.clear();
	m_providers.clear();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::run()
{
	m_shuttingDown = false;
	m_running = true;
	m_wakeEvent.reset();

	while(!m_shuttingDown)
	{
		m_wakeEvent.waitForSignal();
		m_wakeEvent.reset();

		try
		{
			OW_MutexLock ml(m_procTransGuard);
			while(m_procTrans.size() > 0 && !m_shuttingDown)
			{
				ProcIndicationTrans trans = m_procTrans[0];
				m_procTrans.remove(0);
				
				ml.release();
				_processIndication(trans.instance, trans.nameSpace);
				ml.lock();
			}
		}
		catch(...)
		{
			m_env->logError("OW_IndicationServerImpl::run caught unknown"
				" exception");
			// Ignore?
		}

		if(m_shuttingDown)
		{
			m_env->logDebug("OW_IndicationServerImpl::run shutting down");
			break;
		}
	}

	// Wait for notify manager to complete any pending notifications
	while(getRunCount() > 0)
	{
		OW_Thread::yield();
	}

	m_running = false;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::shutdown()
{
	if(m_running)
	{
		m_shuttingDown = true;
		m_wakeEvent.signal();
		while(m_running)
		{
			OW_Thread::yield();
		}
		OW_Thread::yield();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::processIndication(const OW_CIMInstance& instanceArg,
	const OW_String& instNS)
{
	OW_MutexLock ml(m_procTransGuard);
	ProcIndicationTrans trans(instanceArg, instNS);
	m_procTrans.push_back(trans);
	m_wakeEvent.signal();
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMInstanceArrayBuilder : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceArrayBuilder(OW_CIMInstanceArray& cia_)
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
}
//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::_processIndication(const OW_CIMInstance& instanceArg,
	const OW_String& instNS)
{
	OW_ACLInfo aclInfo;

	OW_CIMInstance instance(instanceArg);
	OW_DateTime dtm;
	dtm.setToCurrent();
	OW_CIMDateTime cdt(dtm);

	instance.setProperty("IndicationTime", OW_CIMValue(cdt));
	OW_CIMOMHandleIFCRef hdl = m_env->getCIMOMHandle(aclInfo, false);
	if (!hdl)
	{
		return;
	}

	OW_CIMInstanceEnumeration subscriptions;
	try
	{
		subscriptions = hdl->enumInstancesE(instNS,
			"CIM_IndicationSubscription", true);
	}
	catch(OW_CIMException& e)
	{
		m_env->logError(format("%1 caught in OW_IndicationServerImpl::_processIndication", e.type()));
		m_env->logError(format("File: %1", e.getFile()));
		m_env->logError(format("Line: %1", e.getLine()));
		m_env->logError(format("Msg: %1", e.getMessage()));
		return;
	}

	OW_CIMOMHandleIFCRef wqllch = m_env->getWQLFilterCIMOMHandle(instance,
		aclInfo);
	if (!wqllch)
	{
		m_env->logError("Cannot process indications, because there is no "
			"WQL library.");
		return;
	}

	OW_WQLIFCRef wqlRef = m_env->getWQLRef();

	if (!wqlRef)
	{
		m_env->logError("Cannot process indications, because there is no "
			"WQL library.");
		return;
	}

	while(subscriptions.hasMoreElements())
	{
		try
		{
			OW_CIMInstance subscription = subscriptions.nextElement();
			OW_CIMObjectPath cop;

			// Get object path to filter object
			subscription.getProperty("Filter").getValue().get(cop);
			OW_CIMInstance filter = hdl->getInstance(cop.getNameSpace(), cop);

			// Get query string
			OW_String query;
			filter.getProperty("Query").getValue().get(query);

			// Get query language
			OW_String queryLanguage;
			filter.getProperty("QueryLanguage").getValue().get(queryLanguage);

			// TEMP
			OW_CIMInstance filteredInstance = instance;

			//-----------------------------------------------------------------
			// Here we need to call into the WQL process with the query string
			// and the indication instance
			//-----------------------------------------------------------------
			if (wqlRef->supportsQueryLanguage(queryLanguage))
			{
				OW_CIMInstanceArray cia;
				CIMInstanceArrayBuilder handler(cia);
				wqlRef->evaluate(instNS, handler, query,
					queryLanguage, wqllch);

				if(cia.size() != 1)
				{
					continue;
				}

				filteredInstance = cia[0];
			}
			else
			{
				m_env->logError(format("Filter uses queryLanguage %1, which is"
					" not supported", queryLanguage));
				continue;
			}
			

			// Now get the export handler for this indication subscription
			OW_CIMObjectPath handlerCOP;
			OW_CIMProperty cp = subscription.getProperty("Handler");
			if(!cp)
			{
				m_env->logError("Handler property does not exist");
				continue;
			}

			OW_CIMValue cv = cp.getValue();
			if(!cv)
			{
				m_env->logError("Handler property has no value");
				continue;
			}

			if(cv.getType() != OW_CIMDataType::REFERENCE)
			{
				m_env->logError(format("Handler property is not reference type."
                    " Type = %1", OW_CIMDataType(cv.getType()).toString()));
				continue;
			}

			cv.get(handlerCOP);
			OW_CIMInstance handler = hdl->getInstance(handlerCOP.getNameSpace(),
				handlerCOP);

			if(!handler)
			{
				m_env->logError(format("Handler does not exist: %1",
					handlerCOP.toString()));
				continue;
			}

			// Get the appropriate export provider for the subscription
			OW_IndicationExportProviderIFCRef pref = getProvider(
				handler.getClassName());
			
			if(!pref)
			{
				m_env->logError(format("No indication handler for class name:"
					" %1", handler.getClassName()));
			
				continue;
			}

			addTrans(instNS, filteredInstance, handler, pref);
		}
		catch(OW_Exception& e)
		{
			m_env->logError(format("Error occurred while exporting indications:"
				" %1", e).c_str());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::addTrans(
	const OW_String& ns,
	const OW_CIMInstance& indication,
	const OW_CIMInstance& handler, OW_IndicationExportProviderIFCRef provider)
{
	OW_MutexLock ml(m_guard);

	OW_NotifyTrans trans(ns, indication, handler, provider);
	if(getRunCount() < MAX_NOTIFIERS)
	{
		OW_Notifier* pnotifier = new OW_Notifier(this, trans);
		incRunCount();
		pnotifier->start();
	}
	else
	{
		m_trans.append(trans);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_IndicationServerImpl::notifyDone(OW_NotifyTrans*& outTrans)
{
	OW_MutexLock ml(m_guard);

	OW_Bool rv = false;
	if(m_trans.size() > 0)
	{
		outTrans = new OW_NotifyTrans(m_trans[0]);
		m_trans.remove(0);
		rv = true;
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRef
OW_IndicationServerImpl::getProvider(const OW_String& className)
{
	OW_IndicationExportProviderIFCRef pref(0);
	OW_String lowerClassName(className);
	lowerClassName.toLowerCase();
	OW_Map<OW_String, OW_IndicationExportProviderIFCRef>::iterator it =
		m_providers.find(lowerClassName);

	if(it != m_providers.end())
	{
		pref = it->second;
	}

	return pref;
}


//////////////////////////////////////////////////////////////////////////////
extern "C" OW_IndicationServer*
createIndicationServer()
{
	return new OW_IndicationServerImpl();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" const char*
getOWVersion()
{
	return OW_VERSION;
}
/*
#include <stdio.h>
extern "C"
void _fini(void)
{
	printf("_fini of indication server lib");
}
*/

