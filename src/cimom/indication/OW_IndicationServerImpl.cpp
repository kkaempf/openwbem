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
#include "OW_IndicationServerImpl.hpp"
#include "OW_DateTime.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_NULLValueException.hpp"
#include "OW_PollingManager.hpp"
#include "OW_CppProxyProvider.hpp"
#include "OW_Platform.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMClass.hpp"
#include "OW_WQLInstancePropertySource.hpp"
#include "OW_OperationContext.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMDateTime.hpp"

#include <iterator>

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(IndicationServer);
OW_DEFINE_EXCEPTION_WITH_ID(IndicationServer);

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
IndicationServer::~IndicationServer()
{
}
//////////////////////////////////////////////////////////////////////////////
struct NotifyTrans
{
	NotifyTrans(
		const String& ns,
		const CIMInstance& indication,
		const CIMInstance& handler,
		const CIMInstance& subscription,
		const IndicationExportProviderIFCRef provider) :
			m_ns(ns), m_indication(indication), m_handler(handler), m_subscription(subscription), m_provider(provider) {}
	String m_ns;
	CIMInstance m_indication;
	CIMInstance m_handler;
	CIMInstance m_subscription;
	IndicationExportProviderIFCRef m_provider;
};
//////////////////////////////////////////////////////////////////////////////
namespace
{
//////////////////////////////////////////////////////////////////////////////
class Notifier : public Runnable
{
public:
	Notifier(IndicationServerImpl* pmgr, NotifyTrans& ntrans) :
		m_pmgr(pmgr), m_trans(ntrans) {}
	virtual void run();
private:
	virtual void doCooperativeCancel();
	virtual void doDefinitiveCancel();
	IndicationServerImpl* m_pmgr;
	NotifyTrans m_trans;
};
class IndicationServerProviderEnvironment : public ProviderEnvironmentIFC
{
public:
	IndicationServerProviderEnvironment(
		CIMOMEnvironmentRef env)
		: ProviderEnvironmentIFC()
		, m_ch()
		, m_env(env)
		, m_repch()
		, m_opctx()
	{
		m_ch = m_env->getCIMOMHandle(m_opctx);
		m_repch = m_env->getRepositoryCIMOMHandle(m_opctx);
	}
	virtual CIMOMHandleIFCRef getCIMOMHandle() const
	{
		return m_ch;
	}
	
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
	{
		return m_repch;
	}
	virtual RepositoryIFCRef getRepository() const
	{
		return m_env->getRepository();
	}
	virtual String getConfigItem(const String& name, const String& defRetVal="") const
	{
		return m_env->getConfigItem(name, defRetVal);
	}
	
	virtual LoggerRef getLogger() const
	{
		return m_env->getLogger();
	}
	virtual String getUserName() const
	{
		return Platform::getCurrentUserName();
	}
	virtual OperationContext& getOperationContext()
	{
		return m_opctx;
	}
private:
	CIMOMHandleIFCRef m_ch;
	CIMOMEnvironmentRef m_env;
	CIMOMHandleIFCRef m_repch;
	OperationContext m_opctx;
};
ProviderEnvironmentIFCRef createProvEnvRef(CIMOMEnvironmentRef env)
{
	return ProviderEnvironmentIFCRef(new IndicationServerProviderEnvironment(env));
}
//////////////////////////////////////////////////////////////////////////////
void
Notifier::run()
{
	// TODO: process the subscription error handling here
	CIMOMEnvironmentRef env = m_pmgr->getEnvironment();
	try
	{
		m_trans.m_provider->exportIndication(createProvEnvRef(env), 
			m_trans.m_ns, m_trans.m_handler, m_trans.m_indication);
	}
	catch(Exception& e)
	{
		env->logError(Format("Caught exception while exporting indication: %1", e));
	}
	catch(ThreadCancelledException&)
	{
		throw;
	}
	catch(...)
	{
		env->logError("Unknown exception caught while exporting indication");
	}
}
//////////////////////////////////////////////////////////////////////////////
void
Notifier::doCooperativeCancel()
{
	m_trans.m_provider->doCooperativeCancel();
}
//////////////////////////////////////////////////////////////////////////////
void
Notifier::doDefinitiveCancel()
{
	m_trans.m_provider->doDefinitiveCancel();
}

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
IndicationServerImpl::IndicationServerImpl()
	: IndicationServer()
	, m_shuttingDown(false)
	, m_startedBarrier(2)
{
}
namespace
{
//////////////////////////////////////////////////////////////////////////////
class instanceEnumerator : public CIMInstanceResultHandlerIFC
{
public:
	instanceEnumerator(IndicationServerImpl* is_,
		const String& ns_)
		: is(is_)
		, ns(ns_)
	{}
private:
	void doHandle(const CIMInstance& i)
	{
		// try and get the name of whoever first created the subscription
		String username;
		CIMProperty p = i.getProperty("__Subscription_UserName");
		if (p)
		{
			CIMValue v = p.getValue();
			if (v)
			{
				username = v.toString();
			}
		}
		// TODO: If the provider rejects the subscription, we need to disable it!
		is->createSubscription(ns, i, username);
	}
	IndicationServerImpl* is;
	String ns;
};
//////////////////////////////////////////////////////////////////////////////
class namespaceEnumerator : public StringResultHandlerIFC
{
public:
	namespaceEnumerator(
		const CIMOMHandleIFCRef& ch_,
		IndicationServerImpl* is_)
		: ch(ch_)
		, is(is_)
	{}
private:
	void doHandle(const String& ns)
	{
		instanceEnumerator ie(is, ns);
		try
		{
			ch->enumInstances(ns,"CIM_IndicationSubscription", ie);
		}
		catch (const CIMException& ce)
		{
			// do nothing, class probably doesn't exist in the namespace
		}
	}
	CIMOMHandleIFCRef ch;
	IndicationServerImpl* is;
};
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::init(CIMOMEnvironmentRef env)
{
	m_env = env;
	// set up the thread pool
	Int32 maxIndicationExportThreads;
	try
	{
		maxIndicationExportThreads = env->getConfigItem(ConfigOpts::MAX_INDICATION_EXPORT_THREADS_opt, OW_DEFAULT_MAX_INDICATION_EXPORT_THREADS).toInt32();
	}
	catch (const StringConversionException&)
	{
			maxIndicationExportThreads = String(OW_DEFAULT_MAX_INDICATION_EXPORT_THREADS).toInt32();
	}
	m_notifierThreadPool = ThreadPoolRef(new ThreadPool(ThreadPool::DYNAMIC_SIZE,
				maxIndicationExportThreads, maxIndicationExportThreads * 100, env->getLogger(), "Indication Server Notifiers"));
	// pool to handle threads modifying subscriptions
	m_subscriptionPool = ThreadPoolRef(new ThreadPool(ThreadPool::DYNAMIC_SIZE,
		1, // 1 thread because only 1 can run at a time because of mutex locking
		0, // unlimited size queue
		env->getLogger(), "Indication Server Subscriptions"));
	//-----------------
	// Load map with available indication export providers
	//-----------------
	ProviderManagerRef pProvMgr = m_env->getProviderManager();
	OperationContext context;
	CIMOMHandleIFCRef lch = m_env->getCIMOMHandle(context, ServiceEnvironmentIFC::E_DONT_SEND_INDICATIONS);
	IndicationExportProviderIFCRefArray pra =
		pProvMgr->getIndicationExportProviders(createProvEnvRef(m_env));
	m_env->logDebug(Format("IndicationServerImpl: %1 export providers found",
		pra.size()));
	for (size_t i = 0; i < pra.size(); i++)
	{
		StringArray clsNames = pra[i]->getHandlerClassNames();
		for (size_t j = 0; j < clsNames.size(); j++)
		{
			String lowerClsName = clsNames[j];
			lowerClsName.toLowerCase();
			m_providers[lowerClsName] = pra[i];
			m_env->logDebug(Format("IndicationServerImpl: Handling"
				" indication type %1", clsNames[j]));
		}
	}

	// get the wql lib
	m_wqlRef = m_env->getWQLRef();
	if (!m_wqlRef)
	{
		const char* const err = "Cannot process indications, because there is no "
			"WQL library.";
		m_env->logFatalError(err);
		OW_THROW(IndicationServerException, err);
	}

	// Now initialize for all the subscriptions that exist in the repository.
	// This calls createSubscription for every instance of
	// CIM_IndicationSubscription in all namespaces.
	// TODO: If the provider rejects the subscription, we need to disable it!
	namespaceEnumerator nsHandler(lch, this);
	env->getRepository()->enumNameSpace(nsHandler, context);
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::waitUntilReady()
{
	m_startedBarrier.wait();
}
//////////////////////////////////////////////////////////////////////////////
IndicationServerImpl::~IndicationServerImpl()
{
	try
	{
		m_providers.clear();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
Int32
IndicationServerImpl::run()
{
	// let CIMOMEnvironment know we're running and ready to go.
	m_startedBarrier.wait();
	{
		NonRecursiveMutexLock l(m_mainLoopGuard);
		while (!m_shuttingDown)
		{
			m_mainLoopCondition.wait(l);
			
			try
			{
				while (!m_procTrans.empty() && !m_shuttingDown)
				{
					ProcIndicationTrans trans = m_procTrans.front();
					m_procTrans.pop_front();
					l.release();
					_processIndication(trans.instance, trans.nameSpace);
					l.lock();
				}
			}
			catch (const Exception& e)
			{
				m_env->logError(Format("IndicationServerImpl::run caught "
					" exception %1", e));
			}
			catch(ThreadCancelledException&)
			{
				throw;
			}
			catch(...)
			{
				m_env->logError("IndicationServerImpl::run caught unknown"
					" exception");
				// Ignore?
			}
		}
	}
	m_env->logDebug("IndicationServerImpl::run shutting down");
	m_subscriptionPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, 5);
	m_notifierThreadPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, 60);
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::shutdown()
{
	{
		NonRecursiveMutexLock l(m_mainLoopGuard);
		m_shuttingDown = true;
		m_mainLoopCondition.notifyAll();
	}
	// wait until the main thread exits.
	this->join();
	
	// clear out variables to avoid circular reference counts.
	m_providers.clear();
	m_procTrans.clear();
	m_env = 0;
	m_subscriptions.clear();
	m_pollers.clear();
	m_notifierThreadPool = 0;
	m_subscriptionPool = 0;
	m_wqlRef.setNull();
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::processIndication(const CIMInstance& instanceArg,
	const String& instNS)
{
	NonRecursiveMutexLock ml(m_mainLoopGuard);
	if (m_shuttingDown)
	{
		return;
	}
	ProcIndicationTrans trans(instanceArg, instNS);
	m_procTrans.push_back(trans);
	m_mainLoopCondition.notifyOne();
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
void splitUpProps(const StringArray& props,
	HashMap<String, StringArray>& map)
{
	// This function may appear a little complicated...
	// It's handling the many cases needed to split up
	// the props so they can be quickly accessed in
	// filterInstance().
	// The props that are possible are:
	// *
	// PropertyName
	// ClassName.PropertyName
	// ClassName.*
	// PropertyName.*
	// PropertyName.EmbedName
	// ClassName.PropertyName.*
	// ClassName.PropertyName.EmbedName
	for (size_t i = 0; i < props.size(); ++i)
	{
		String prop = props[i];
		prop.toLowerCase();
		size_t idx = prop.indexOf('.');
		map[""].push_back(prop); // for no ClassName
		if (idx != String::npos)
		{
			String key = prop.substring(0, idx);
			String val = prop.substring(idx+1);
			map[""].push_back(key); // Store PropertyName for PropertyName.EmbedName
			map[key].push_back(val); // Store PropertyName for ClassName.PropertyName and EmbedName for PropertyName.EmbedName
			// now remove trailing periods.
			idx = val.indexOf('.');
			if (idx != String::npos)
			{
				val = val.substring(0, idx);
			}
			map[key].push_back(val); // Store PropertyName for ClassName.PropertyName.EmbedName
		}
	}
}
CIMInstance filterInstance(const CIMInstance& toFilter, const StringArray& props)
{
	CIMInstance rval(toFilter.clone(E_NOT_LOCAL_ONLY,
		E_EXCLUDE_QUALIFIERS,
		E_EXCLUDE_CLASS_ORIGIN));
	if (props.empty())
	{
		return rval;
	}
	HashMap<String, StringArray> propMap;
	splitUpProps(props, propMap);
	// find "" and toFilter.getClassName() and keep those properties.
	StringArray propsToKeepArray(propMap[""]);
	
	String lowerClassName(toFilter.getClassName());
	lowerClassName.toLowerCase();
	propsToKeepArray.appendArray(propMap[lowerClassName]);
	// create a sorted set to get faster look-up time.
	SortedVectorSet<String> propsToKeep(propsToKeepArray.begin(),
		propsToKeepArray.end());
	CIMPropertyArray propArray = toFilter.getProperties();
	CIMPropertyArray propArrayToKeep;
	for (size_t i = 0; i < propArray.size(); ++i)
	{
		String lowerPropName(propArray[i].getName());
		lowerPropName.toLowerCase();
		if (propsToKeep.count(lowerPropName) > 0 || propsToKeep.count("*") > 0)
		{
			CIMProperty thePropToKeep(propArray[i]);
			// if it's an embedded instance, we need to recurse on it.
			if (thePropToKeep.getDataType().getType() == CIMDataType::EMBEDDEDINSTANCE)
			{
				CIMValue v = thePropToKeep.getValue();
				if (v)
				{
					CIMInstance embed;
					v.get(embed);
					if (embed)
					{
						StringArray embeddedProps;
						for (size_t i = 0; i < propsToKeepArray.size(); ++i)
						{
							const String& curPropName = propsToKeepArray[i];
							if (curPropName.startsWith(lowerPropName))
							{
								size_t idx = curPropName.indexOf('.');
								if (idx != String::npos)
								{
									embeddedProps.push_back(curPropName.substring(idx));
								}
							}
						}
						thePropToKeep.setValue(CIMValue(
							filterInstance(embed, embeddedProps)));
					}
				}
			}
			propArrayToKeep.push_back(thePropToKeep);
		}
	}
	rval.setProperties(propArrayToKeep);
	return rval;
}
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::_processIndication(const CIMInstance& instanceArg_,
	const String& instNS)
{
	m_env->logDebug(Format("IndicationServerImpl::_processIndication "
		"instanceArg = %1 instNS = %2", instanceArg_.toString(), instNS));
	
	// If the provider didn't set the IndicationTime property, then we'll set it.
	CIMInstance instanceArg(instanceArg_);
	if (!instanceArg.getProperty("IndicationTime"))
	{
		DateTime dtm;
		dtm.setToCurrent();
		CIMDateTime cdt(dtm);
		instanceArg.setProperty("IndicationTime", CIMValue(cdt));
	}

	String curClassName = instanceArg.getClassName();
	if (curClassName.empty())
	{
		m_env->logError("Cannot process indication, because it has no "
			"class name.");
	}
	while (!curClassName.empty())
	{
		String key = curClassName;
		key.toLowerCase();
		{
			MutexLock lock(m_subGuard);
			m_env->logDebug(Format("searching for key %1", key));
			std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range =
				m_subscriptions.equal_range(key);
			m_env->logDebug(Format("found %1 items", distance(range.first, range.second)));
			
			// make a copy so we can free the lock, otherwise we may cause a deadlock.
			subscriptions_copy_t subs(range.first, range.second);
			lock.release();
			_processIndicationRange(instanceArg, instNS, subs.begin(), subs.end());
		}
		CIMProperty prop = instanceArg.getProperty("SourceInstance");
		if (prop)
		{
			CIMValue v = prop.getValue();
			if (v && v.getType() == CIMDataType::EMBEDDEDINSTANCE)
			{
				CIMInstance embed;
				v.get(embed);
				key += ":";
				key += embed.getClassName();
				key.toLowerCase();
				{
					MutexLock lock(m_subGuard);
					m_env->logDebug(Format("searching for key %1", key));
					std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range =
						m_subscriptions.equal_range(key);
					m_env->logDebug(Format("found %1 items", distance(range.first, range.second)));
					
					// make a copy so we can free the lock, otherwise we may cause a deadlock.
					subscriptions_copy_t subs(range.first, range.second);

					lock.release();
					_processIndicationRange(instanceArg, instNS, subs.begin(), subs.end());
				}
			}
		}
		CIMClass cc;
		try
		{
			OperationContext context;
			cc = m_env->getRepositoryCIMOMHandle(context)->getClass(instNS, curClassName);
			curClassName = cc.getSuperClass();
		}
		catch (const CIMException& e)
		{
			curClassName.erase();
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::_processIndicationRange(
	const CIMInstance& instanceArg, const String instNS,
	IndicationServerImpl::subscriptions_iterator first,
	IndicationServerImpl::subscriptions_iterator last)
{
	OperationContext context;
	CIMOMHandleIFCRef hdl = m_env->getCIMOMHandle(context, ServiceEnvironmentIFC::E_DONT_SEND_INDICATIONS);
	for ( ;first != last; ++first)
	{
		try
		{
			Subscription& sub = first->second;
			CIMInstance filterInst = sub.m_filter;
			String queryLanguage = sub.m_filter.getPropertyT("QueryLanguage").getValueT().toString();
// TODO: Fix this to handle namespace portability problems, such as initial /
//			if (!sub.m_filterSourceNameSpace.equalsIgnoreCase(instNS))
//			{
//				m_env->logDebug(Format("skipping sub because namespace doesn't match. Filter ns = %1, Sub ns = %2", sub.m_filterSourceNameSpace, instNS));
//				continue;
//			}
			//-----------------------------------------------------------------
			// Here we need to call into the WQL process with the query string
			// and the indication instance
			//-----------------------------------------------------------------
			WQLInstancePropertySource propSource(instanceArg, hdl, instNS);
			if (!sub.m_compiledStmt.evaluate(propSource))
			{
				m_env->logDebug("skipping sub because wql.evaluate doesn't match");
				continue;
			}
			CIMInstance filteredInstance(filterInstance(instanceArg,
				sub.m_selectStmt.getSelectPropertyNames()));
			// Now get the export handler for this indication subscription
			CIMObjectPath handlerCOP =
				sub.m_subPath.getKeyT("Handler").getValueT().toCIMObjectPath();

			String handlerNS = handlerCOP.getNameSpace();
			if (handlerNS.empty())
				handlerNS = instNS;

			CIMInstance handler = hdl->getInstance(handlerNS,
				handlerCOP);
			if (!handler)
			{
				m_env->logError(Format("Handler does not exist: %1",
					handlerCOP.toString()));
				continue;
			}
			// Get the appropriate export provider for the subscription
			IndicationExportProviderIFCRef pref = getProvider(
				handler.getClassName());
			
			if (!pref)
			{
				m_env->logError(Format("No indication handler for class name:"
					" %1", handler.getClassName()));
			
				continue;
			}
			addTrans(instNS, filteredInstance, handler, sub.m_sub, pref);
		}
		catch(Exception& e)
		{
			m_env->logError(Format("Error occurred while exporting indications:"
				" %1", e).c_str());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::addTrans(
	const String& ns,
	const CIMInstance& indication,
	const CIMInstance& handler,
	const CIMInstance& subscription,
	IndicationExportProviderIFCRef provider)
{
	NotifyTrans trans(ns, indication, handler, subscription, provider);
	if (!m_notifierThreadPool->tryAddWork(RunnableRef(new Notifier(this, trans))))
	{
		m_env->logError(Format("Indication export notifier pool overloaded.  Dropping indication: %1", indication.toMOF()));
	}
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRef
IndicationServerImpl::getProvider(const String& className)
{
	IndicationExportProviderIFCRef pref(0);
	String lowerClassName(className);
	lowerClassName.toLowerCase();
	provider_map_t::iterator it =
		m_providers.find(lowerClassName);
	if (it != m_providers.end())
	{
		pref = it->second;
	}
	return pref;
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::deleteSubscription(const String& ns, const CIMObjectPath& subPath)
{
	LoggerRef log = m_env->getLogger();
	log->logDebug(Format("IndicationServerImpl::deleteSubscription ns = %1, subPath = %2", ns, subPath.toString()));
	CIMObjectPath cop(subPath);
	cop.setNameSpace(ns);
	log->logDebug(Format("cop = %1", cop));
	
	MutexLock l(m_subGuard);
	for (subscriptions_t::iterator iter = m_subscriptions.begin(); iter != m_subscriptions.end();)
	{
		log->logDebug(Format("subPath = %1", iter->second.m_subPath));
		if (cop.equals(iter->second.m_subPath))
		{
			log->logDebug("found a match");
			Subscription& sub = iter->second;
			for (size_t i = 0; i < sub.m_providers.size(); ++i)
			{
				try
				{
					if (sub.m_isPolled[i])
					{
						// loop through all the classes in the subscription.
						// TODO: This is slightly less than optimal, since
						// m_classes may contain a class that isn't handled by
						// the provider
						for (size_t j = 0; j < sub.m_classes.size(); ++j)
						{
							String key = sub.m_classes[j];
							key.toLowerCase();
							poller_map_t::iterator iter = m_pollers.find(key);
							if (iter != m_pollers.end())
							{
								LifecycleIndicationPollerRef p = iter->second;
								String subClsName = sub.m_selectStmt.getClassName();
								bool removePoller = false;
								if (subClsName.equalsIgnoreCase("CIM_InstCreation"))
								{
									removePoller = p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
								}
								else if (subClsName.equalsIgnoreCase("CIM_InstModification"))
								{
									removePoller = p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
								}
								else if (subClsName.equalsIgnoreCase("CIM_InstDeletion"))
								{
									removePoller = p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
								}
								else if (subClsName.equalsIgnoreCase("CIM_InstIndication") || subClsName.equalsIgnoreCase("CIM_Indication"))
								{
									p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
									p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
									removePoller = p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
								}
								if (removePoller)
								{
									m_pollers.erase(iter);
								}
							}
						}
					}
					else
					{
						IndicationProviderIFCRef p = sub.m_providers[i];
						p->deActivateFilter(createProvEnvRef(m_env), sub.m_selectStmt, sub.m_selectStmt.getClassName(), ns, sub.m_classes);
					}
					
				}
				catch (const Exception& e)
				{
					m_env->getLogger()->logError(Format("Caught exception while calling deActivateFilter for provider: %1", e));
				}
				catch(ThreadCancelledException&)
				{
					throw;
				}
				catch (...)
				{
					m_env->getLogger()->logError("Caught unknown exception while calling deActivateFilter for provider");
				}
			}
			m_subscriptions.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace // unnamed
{
String getSourceNameSpace(const CIMInstance& inst)
{
	try
	{
		return inst.getPropertyT("SourceNamespace").getValueT().toString();
	}
	catch (const NoSuchPropertyException& e)
	{
		return "";
	}
	catch (const NULLValueException& e)
	{
		return "";
	}
}

class createSubscriptionRunnable : public Runnable
{
	String ns;
	CIMInstance subInst;
	String username;
	IndicationServerImpl* is;
public:
	createSubscriptionRunnable(const String& ns_, const CIMInstance& subInst_, const String& username_, IndicationServerImpl* is_)
	: ns(ns_)
	, subInst(subInst_)
	, username(username_)
	, is(is_)
	{}

	virtual void run()
	{
		is->createSubscription(ns, subInst, username);
	}
}; // end class createSubscriptionRunnable

class modifySubscriptionRunnable : public Runnable
{
	String ns;
	CIMInstance subInst;
	IndicationServerImpl* is;
public:
	modifySubscriptionRunnable(const String& ns_, const CIMInstance& subInst_, IndicationServerImpl* is_)
	: ns(ns_)
	, subInst(subInst_)
	, is(is_)
	{}

	virtual void run()
	{
		is->modifySubscription(ns, subInst);
	}
}; // end class modifySubscriptionRunnable

class deleteSubscriptionRunnable : public Runnable
{
	String ns;
	CIMObjectPath sub;
	IndicationServerImpl* is;
public:
	deleteSubscriptionRunnable(const String& ns_, const CIMObjectPath& sub_, IndicationServerImpl* is_)
	: ns(ns_)
	, sub(sub_)
	, is(is_)
	{}

	virtual void run()
	{
		is->deleteSubscription(ns, sub);
	}
}; // end class deleteSubscriptionRunnable

} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startCreateSubscription(const String& ns, const CIMInstance& subInst, const String& username)
{
	RunnableRef rr(new createSubscriptionRunnable(ns, subInst, username, this));
	m_subscriptionPool->addWork(rr);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startModifySubscription(const String& ns, const CIMInstance& subInst)
{
	RunnableRef rr(new modifySubscriptionRunnable(ns, subInst, this));
	m_subscriptionPool->addWork(rr);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startDeleteSubscription(const String& ns, const CIMObjectPath& sub)
{
	RunnableRef rr(new deleteSubscriptionRunnable(ns, sub, this));
	m_subscriptionPool->addWork(rr);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::createSubscription(const String& ns, const CIMInstance& subInst, const String& username)
{
	LoggerRef log = m_env->getLogger();
	log->logDebug(Format("IndicationServerImpl::createSubscription ns = %1, subInst = %2", ns, subInst.toString()));
	// get the filter
	OperationContext context;
	CIMOMHandleIFCRef hdl = m_env->getRepositoryCIMOMHandle(context);
	CIMObjectPath filterPath = subInst.getProperty("Filter").getValueT().toCIMObjectPath();
	String filterNS = filterPath.getNameSpace();
	if (filterNS.empty())
	{
		filterNS = ns;
	}
	CIMInstance filterInst = hdl->getInstance(filterNS, filterPath);
	String filterQuery = filterInst.getPropertyT("Query").getValueT().toString();
	// parse the filter
	// Get query language
	String queryLanguage = filterInst.getPropertyT("QueryLanguage").getValueT().toString();
	log->logDebug(Format("Got query statement (%1) in %2", filterQuery, queryLanguage));
	if (!m_wqlRef->supportsQueryLanguage(queryLanguage))
	{
		OW_THROWCIMMSG(CIMException::FAILED, Format("Filter uses queryLanguage %1, which is"
			" not supported", queryLanguage).c_str());
	}
	WQLSelectStatement selectStmt(m_wqlRef->createSelectStatement(filterQuery));
	WQLCompile compiledStmt(selectStmt);
	const WQLCompile::Tableau& tableau(compiledStmt.getTableau());
	String indicationClassName = selectStmt.getClassName();
	log->logDebug(Format("query is for indication class: %1", indicationClassName));
	// collect up all the class names
	StringArray isaClassNames;
	for (size_t i = 0; i < tableau.size(); ++i)
	{
		for (size_t j = 0; j < tableau[i].size(); ++j)
		{
			if (tableau[i][j].op == WQL_ISA)
			{
				const WQLOperand& opn1(tableau[i][j].opn1);
				const WQLOperand& opn2(tableau[i][j].opn2);
				if (opn1.getType() == WQLOperand::PROPERTY_NAME && opn1.getPropertyName().equalsIgnoreCase("SourceInstance"))
				{
					if (opn2.getType() == WQLOperand::PROPERTY_NAME)
					{
						isaClassNames.push_back(opn2.getPropertyName());
						log->logDebug(Format("Found ISA class name: %1", opn2.getPropertyName()));
					}
					else if (opn2.getType() == WQLOperand::STRING_VALUE)
					{
						isaClassNames.push_back(opn2.getStringValue());
						log->logDebug(Format("Found ISA class name: %1", opn2.getStringValue()));
					}
				}
			}
		}
	}

	// TODO: look up all the subclasses of the classes in isaClassNames.  This may also require a change to the providers so they only register for the class
	// they care about, and not all the base classes.

	// get rid of duplicates - unique() requires that the range be sorted
	std::sort(isaClassNames.begin(), isaClassNames.end());
	isaClassNames.erase(std::unique(isaClassNames.begin(), isaClassNames.end()), isaClassNames.end());

	OStringStream ss;
	std::copy(isaClassNames.begin(), isaClassNames.end(), std::ostream_iterator<String>(ss, ", "));
	log->logDebug(Format("isaClassNames = %1", ss.toString()));

	// find providers that support this query. If none are found, throw an exception.
	ProviderManagerRef pm (m_env->getProviderManager());
	IndicationProviderIFCRefArray providers =
		pm->getIndicationProviders(createProvEnvRef(m_env), ns,
			indicationClassName, "");
	if (!isaClassNames.empty())
	{
		for (size_t i = 0; i < isaClassNames.size(); ++i)
		{
			providers.appendArray(pm->getIndicationProviders(createProvEnvRef(m_env),
				ns, indicationClassName, isaClassNames[i]));
		}
	}
	// get rid of duplicate providers - unique() requires that the range be sorted
	std::sort(providers.begin(), providers.end());
	providers.erase(std::unique(providers.begin(), providers.end()), providers.end());
	log->logDebug(Format("Found %1 providers for the subscription", providers.size()));
	if (providers.empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "No indication provider found for this subscription");
	}
	// verify that there is an indication export provider that can handle the handler for the subscription
	CIMObjectPath handlerPath = subInst.getProperty("Handler").getValueT().toCIMObjectPath();
	String handlerClass = handlerPath.getClassName();
	if (!getProvider(handlerClass))
	{
		OW_THROWCIMMSG(CIMException::FAILED, "No indication export provider found for this subscription");
	}
	// call authorizeFilter on all the indication providers
	for (size_t i = 0; i < providers.size(); ++i)
	{
		log->logDebug(Format("Calling authorizeFilter for provider %1", i));
		providers[i]->authorizeFilter(createProvEnvRef(m_env),
			selectStmt, indicationClassName, ns, isaClassNames, username);
	}
	// Call mustPoll on all the providers
	Array<bool> isPolled(providers.size(), false);
	for (size_t i = 0; i < providers.size(); ++i)
	{
		try
		{
			log->logDebug(Format("Calling mustPoll for provider %1", i));
			int pollInterval = providers[i]->mustPoll(createProvEnvRef(m_env),
				selectStmt, indicationClassName, ns, isaClassNames);
			log->logDebug(Format("got pollInterval %1", pollInterval));
			if (pollInterval > 0)
			{
				isPolled[i] = true;
				for (size_t j = 0; j < isaClassNames.size(); ++j)
				{
					String key = isaClassNames[j];
					key.toLowerCase();
					log->logDebug(Format("searching on class key %1", isaClassNames[j]));
					poller_map_t::iterator iter = m_pollers.find(key);
					LifecycleIndicationPollerRef p;
					if (iter != m_pollers.end())
					{
						log->logDebug(Format("found on class key %1: %2", isaClassNames[j], iter->first));
						p = iter->second;
					}
					else
					{
						log->logDebug(Format("not found on class key %1", isaClassNames[j]));
						p = LifecycleIndicationPollerRef(SharedLibraryRef(0),
							LifecycleIndicationPollerRef::element_type(new LifecycleIndicationPoller(ns, key, pollInterval)));
					}
					String subClsName = selectStmt.getClassName();
					if (subClsName.equalsIgnoreCase("CIM_InstCreation"))
					{
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
					}
					else if (subClsName.equalsIgnoreCase("CIM_InstModification"))
					{
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
					}
					else if (subClsName.equalsIgnoreCase("CIM_InstDeletion"))
					{
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
					}
					else if (subClsName.equalsIgnoreCase("CIM_InstIndication") || subClsName.equalsIgnoreCase("CIM_Indication"))
					{
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
					}
					p->addPollInterval(pollInterval);
					if (iter == m_pollers.end())
					{
						log->logDebug(Format("Inserting %1 into m_pollers", key));
						m_pollers.insert(std::make_pair(key, p));
						m_env->getPollingManager()->addPolledProvider(
							PolledProviderIFCRef(
								new CppPolledProviderProxy(
									CppPolledProviderIFCRef(p))));
					}
				}
			}

		}
		catch (CIMException& ce)
		{
			m_env->getLogger()->logError(Format("Caught exception while calling mustPoll for provider: %1", ce));
		}
		catch(ThreadCancelledException&)
		{
			throw;
		}
		catch (...)
		{
			m_env->getLogger()->logError("Caught unknown exception while calling mustPoll for provider");
		}
	}
	// create a subscription (save the compiled filter and other info)
	Subscription sub;
	sub.m_subPath = CIMObjectPath(ns, subInst);
	sub.m_sub = subInst;
	sub.m_providers = providers;
	sub.m_isPolled = isPolled;
	sub.m_filter = filterInst;
	sub.m_selectStmt = selectStmt;
	sub.m_compiledStmt = compiledStmt;
	sub.m_classes = isaClassNames;

	// m_filterSourceNamespace is saved so _processIndication can do what the
	// schema says:
	//"The path to a local namespace where the Indications "
	//"originate. If NULL, the namespace of the Filter registration "
	//"is assumed."
	// first try to get it from the property
	String filterSourceNameSpace = getSourceNameSpace(filterInst);
	if (filterSourceNameSpace.empty())
	{
		filterSourceNameSpace = filterNS;
	}
	sub.m_filterSourceNameSpace = filterSourceNameSpace;
	// get the lock and put it in m_subscriptions
	{
		MutexLock l(m_subGuard);
		if (isaClassNames.empty())
		{
			String subKey = indicationClassName;
			subKey.toLowerCase();
			m_subscriptions.insert(std::make_pair(subKey, sub));
		}
		else
		{
			for (size_t i = 0; i < isaClassNames.size(); ++i)
			{
				String subKey = indicationClassName + ':' + isaClassNames[i];
				subKey.toLowerCase();
				m_subscriptions.insert(std::make_pair(subKey, sub));
			}
		}
	}
	// call activateFilter on all the providers
	// If activateFilter calls fail or throw, just ignore it and keep going.
	// If none succeed, we need to remove it from m_subscriptions and throw
	// to indicate that subscription creation failed.
	int successfulActivations = 0;
	for (size_t i = 0; i < providers.size(); ++i)
	{
		try
		{
			providers[i]->activateFilter(createProvEnvRef(m_env),
				selectStmt, indicationClassName, ns, isaClassNames);

			++successfulActivations;
		}
		catch (CIMException& ce)
		{
			m_env->getLogger()->logError(Format("Caught exception while calling activateFilter for provider: %1", ce));
		}
		catch(ThreadCancelledException&)
		{
			throw;
		}
		catch (...)
		{
			m_env->getLogger()->logError("Caught unknown exception while calling activateFilter for provider");
		}
	}
	if (successfulActivations == 0)
	{
		// Remove it and throw
		MutexLock l(m_subGuard);
		if (isaClassNames.empty())
		{
			String subKey = indicationClassName;
			subKey.toLowerCase();
			m_subscriptions.erase(subKey);
		}
		else
		{
			for (size_t i = 0; i < isaClassNames.size(); ++i)
			{
				String subKey = indicationClassName + ':' + isaClassNames[i];
				subKey.toLowerCase();
				m_subscriptions.erase(subKey);
			}
		}
		OW_THROWCIMMSG(CIMException::FAILED, "activateFilter failed for all providers");
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::modifySubscription(const String& ns, const CIMInstance& subInst)
{
	// since you can't modify an instance's path which includes the paths to
	// the filter and the handler, if a subscription was modified, it will
	// have only really changed the non-key, non-ref properties, so we can just
	// find it in the subscriptions map and update it.
	CIMObjectPath cop(ns, subInst);
	
	MutexLock l(m_subGuard);
	for (subscriptions_t::iterator iter = m_subscriptions.begin();
		 iter != m_subscriptions.end(); ++iter)
	{
		if (cop.equals(iter->second.m_subPath))
		{
			Subscription& sub = iter->second;
			for (size_t i = 0; i < sub.m_providers.size(); ++i)
			{
				sub.m_sub = subInst;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::modifyFilter(const String& ns, const CIMInstance& filterInst)
{
	// If this were to update the filters, it would be quite a bit of work.
	// Basically all the subscriptions that use the old filter would have to
	// be unregistered, and then re-registered with the new filter.  If any
	// of the providers doesn't support the new filter, what then?
	// So, it's just easiest to disallow filter modification.  If we wanted to
	// make this a little more friendly, we could allow modification as long
	// as there's not subscriptions associated to it.
	OW_THROWCIMMSG(CIMException::FAILED, "modifying a filter is not supported");
}

void
IndicationServerImpl::doCooperativeCancel()
{
	NonRecursiveMutexLock l(m_mainLoopGuard);
	m_shuttingDown = true;
	m_mainLoopCondition.notifyAll();
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
extern "C" OpenWBEM::IndicationServer*
createIndicationServer()
{
	return new OpenWBEM::IndicationServerImpl();
}
//////////////////////////////////////////////////////////////////////////////
#if !defined(OW_STATIC_SERVICES)
extern "C" const char*
getOWVersion()
{
	return OW_VERSION;
}
#endif /* !defined(OW_STATIC_SERVICES) */
