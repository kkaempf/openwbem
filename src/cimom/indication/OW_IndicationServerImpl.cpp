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
#include "OW_CIMInstanceEnumeration.hpp"

//////////////////////////////////////////////////////////////////////////////
struct OW_NotifyTrans
{
	OW_NotifyTrans() : m_provider(0) {}

	OW_NotifyTrans(
		const OW_String& ns,
		const OW_CIMInstance& indication,
		const OW_CIMInstance& handler,
		const OW_IndicationExportProviderIFCRef provider) :
			m_ns(ns), m_indication(indication), m_handler(handler), m_provider(provider) {}

	OW_String m_ns;
	OW_CIMInstance m_indication;
	OW_CIMInstance m_handler;
	OW_IndicationExportProviderIFCRef m_provider;
};

//////////////////////////////////////////////////////////////////////////////
namespace
{
//////////////////////////////////////////////////////////////////////////////
class OW_Notifier : public OW_Runnable
{
public:
	OW_Notifier(OW_IndicationServerImpl* pmgr, OW_NotifyTrans& ntrans, OW_ACLInfo const& aclInfo) :
		m_pmgr(pmgr), m_trans(ntrans), m_aclInfo(aclInfo) {}

	void start();

	virtual void run();

private:
	OW_IndicationServerImpl* m_pmgr;
	OW_NotifyTrans m_trans;
	OW_ACLInfo m_aclInfo;
};

class IndicationServerProviderEnvironment : public OW_ProviderEnvironmentIFC
{
public:

	IndicationServerProviderEnvironment(const OW_CIMOMHandleIFCRef& ch,
		OW_CIMOMEnvironmentRef env,
		const OW_CIMOMHandleIFCRef& repch)
		: OW_ProviderEnvironmentIFC()
		, m_ch(ch)
		, m_env(env)
		, m_repch(repch)
	{}

	virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const
	{
		return m_ch;
	}
	
	virtual OW_CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
	{
		return m_repch;
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
	OW_CIMOMHandleIFCRef m_repch;
};

OW_ProviderEnvironmentIFCRef createProvEnvRef(OW_CIMOMEnvironmentRef env,
	const OW_CIMOMHandleIFCRef& ch, const OW_CIMOMHandleIFCRef& repch)
{
	return OW_ProviderEnvironmentIFCRef(
		new IndicationServerProviderEnvironment(ch, env, repch));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_Notifier::start()
{
	OW_CIMOMEnvironmentRef eref = m_pmgr->getEnvironment();

	OW_Bool singleThread = eref->getConfigItem(
		OW_ConfigOpts::SINGLE_THREAD_opt).equalsIgnoreCase("true");

	OW_Thread::run(OW_RunnableRef(this), !singleThread,
		OW_ThreadDoneCallbackRef(new OW_ThreadCountDecrementer(m_pmgr->m_threadCounter)));
}


//////////////////////////////////////////////////////////////////////////////
void
OW_Notifier::run()
{
	OW_CIMOMEnvironmentRef env = m_pmgr->getEnvironment();
	OW_CIMOMHandleIFCRef lch = env->getCIMOMHandle(m_aclInfo, false);
	OW_CIMOMHandleIFCRef repch = env->getCIMOMHandle(m_aclInfo, false, true);

	while (true)
	{
		try
		{
			m_trans.m_provider->exportIndication(createProvEnvRef(
				m_pmgr->getEnvironment(), lch, repch), m_trans.m_ns, m_trans.m_handler,
				m_trans.m_indication);
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
		if(!m_pmgr->getNewTrans(m_trans))
		{
			break;
		}
	}
}

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
OW_IndicationServerImpl::OW_IndicationServerImpl()
	: OW_IndicationServer()
	, m_threadCounter(new OW_ThreadCounter(MAX_NOTIFIERS))
	, m_shuttingDown(false)
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
	OW_CIMOMHandleIFCRef repch = env->getCIMOMHandle(aclInfo, false, true);

	OW_IndicationExportProviderIFCRefArray pra =
		pProvMgr->getIndicationExportProviders(createProvEnvRef(m_env, lch, repch));

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
void
OW_IndicationServerImpl::setStartedSemaphore(OW_Semaphore* sem)
{
	m_startedSem = sem;
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationServerImpl::~OW_IndicationServerImpl()
{
	try
	{
		m_trans.clear();
		m_providers.clear();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::run()
{
	// let OW_CIMOMEnvironment know we're running and ready to go.
	m_startedSem->signal();

	{
		OW_MutexLock l(m_mainLoopGuard);
		while(!m_shuttingDown)
		{
			m_mainLoopCondition.wait(l);
			
			try
			{
				while(!m_procTrans.empty() && !m_shuttingDown)
				{
					ProcIndicationTrans trans = m_procTrans.front();
					m_procTrans.pop_front();

					l.release();
					_processIndication(trans.instance, trans.nameSpace);
					l.lock();
				}
			}
			catch(...)
			{
				m_env->logError("OW_IndicationServerImpl::run caught unknown"
					" exception");
				// Ignore?
			}
		}
	}

	m_env->logDebug("OW_IndicationServerImpl::run shutting down");

	// Wait for OW_Notifier threads to complete any pending notifications
	m_threadCounter->waitForAll();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::shutdown()
{
	{
		OW_MutexLock l(m_mainLoopGuard);
		m_shuttingDown = true;
		m_mainLoopCondition.notifyAll();
	}
	// wait until the main thread exits.
	this->join();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::processIndication(const OW_CIMInstance& instanceArg,
	const OW_String& instNS)
{
	OW_MutexLock ml(m_mainLoopGuard);
	ProcIndicationTrans trans(instanceArg, instNS);
	m_procTrans.push_back(trans);
	m_mainLoopCondition.notifyOne();
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
	m_env->logDebug(format("OW_IndicationServerImpl::_processIndication "
		"instanceArg = %1 instNS = %2", instanceArg.toString(), instNS));
	OW_ACLInfo aclInfo;

	OW_CIMInstance instance(instanceArg);
	OW_DateTime dtm;
	dtm.setToCurrent();
	OW_CIMDateTime cdt(dtm);

	instance.setProperty("IndicationTime", OW_CIMValue(cdt));
	OW_CIMOMHandleIFCRef hdl = m_env->getCIMOMHandle(aclInfo, false);

	// TODO: Make this function much more efficient.
	//		1. Put the code into a callback so we don't have to build an enumeration.
	//		2. Don't start with the subscriptions.  Get the filters, and test
	//			each filter.  If the filter passes, call associators to get
	//			the handler instances and then deliver the indication.
	//		3. Cache the compiled filters to avoid redundant work.
	//	Gotcha: We'll need to make sure that we're re-entrant if the
	//		enumInstances callback calls associators.

	OW_CIMInstanceEnumeration subscriptions;
	try
	{
		subscriptions = hdl->enumInstancesE(instNS,
			OW_String("CIM_IndicationSubscription"), true);
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
			OW_CIMProperty filterProp = subscription.getProperty("Filter");
			if (!filterProp)
			{
				m_env->logError("Indication subscription has no Filter property");
				continue;
			}
			OW_CIMValue fpv = filterProp.getValue();
			if (!fpv)
			{
				m_env->logError("Indication subscription Filter property is NULL");
				continue;
			}
			fpv.get(cop);

			OW_CIMInstance filterInst = hdl->getInstance(cop.getNameSpace(), cop);

			// Get query string
			OW_String query;
			OW_CIMProperty queryProp = filterInst.getProperty("Query");
			if (!queryProp)
			{
				m_env->logError("Indication subscription has no Query property");
				continue;
			}
			OW_CIMValue qpv = queryProp.getValue();
			if (!qpv)
			{
				m_env->logError("Indication subscription Query property is NULL");
				continue;
			}
			qpv.get(query);

			// Get query language
			OW_String queryLanguage;
			OW_CIMProperty queryLangProp = filterInst.getProperty("QueryLanguage");
			if (!queryLangProp)
			{
				m_env->logDebug("Indication subscription has no Query property, assuming wql1");
				queryLanguage = "wql1";
			}
			else
			{
				OW_CIMValue qlpv = queryLangProp.getValue();
				if (!qlpv || qlpv.getType() != OW_CIMDataType::STRING)
				{
					m_env->logDebug("Indication subscription has NULL Query property, assuming wql1");
					queryLanguage = "wql1";
				}
				else
				{
					qlpv.get(queryLanguage);
				}
			}

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
	OW_MutexLock ml(m_transGuard);

	OW_NotifyTrans trans(ns, indication, handler, provider);
	if(m_threadCounter->getThreadCount() < MAX_NOTIFIERS)
	{
		OW_Notifier* pnotifier = new OW_Notifier(this, trans, OW_ACLInfo());
		m_threadCounter->incThreadCount();
		pnotifier->start();
	}
	else
	{
		m_trans.push_back(trans);
	}
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_IndicationServerImpl::getNewTrans(OW_NotifyTrans& outTrans)
{
	OW_MutexLock ml(m_transGuard);

	if(!m_trans.empty())
	{
		outTrans = m_trans.front();
		m_trans.pop_front();
		return true;
	}

	return false;
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


