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
#include "OW_LifecycleIndicationPoller.hpp"
#include "OW_ServiceIFCNames.hpp"
#include "OW_CIMNameSpaceUtils.hpp"

#include <iterator>
#include <set>

#if defined(OW_THREADS_RUN_AS_USER)
	#if defined(OW_NETWARE)
		#include <client.h>
		#include <fsio.h>
		#include <nks/dirio.h>
		#define USE_CIMOM_UID setcwd2(0)
	#elif defined(OW_GNU_LINUX)
		#ifdef OW_HAVE_UNISTD_H
			#include <unistd.h>
		#endif
		#ifdef OW_HAVE_SYS_TYPES_H
			#include <sys/types.h>
		#endif
		#define USE_CIMOM_UID seteuid(getuid())
	#else
		#define USE_CIMOM_UID
	#endif
#else
	#define USE_CIMOM_UID
#endif

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(IndicationServer);
OW_DEFINE_EXCEPTION_WITH_ID(IndicationServer);

using namespace WBEMFlags;

namespace
{
String COMPONENT_NAME("ow.owcimomd.indication.Server");
}

//////////////////////////////////////////////////////////////////////////////
IndicationServerImpl::IndicationServerImpl()
	: m_indicationServerThread(new IndicationServerImplThread)
{
}

//////////////////////////////////////////////////////////////////////////////
IndicationServerImpl::~IndicationServerImpl()
{
}

//////////////////////////////////////////////////////////////////////////////
String
IndicationServerImpl::getName() const
{
	return ServiceIFCNames::IndicationServer;
}

//////////////////////////////////////////////////////////////////////////////
StringArray
IndicationServerImpl::getDependencies() const
{
	StringArray rv;
	rv.push_back(ServiceIFCNames::CIMServer);
	rv.push_back(ServiceIFCNames::ProviderManager);
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::init(const ServiceEnvironmentIFCRef& env)
{
	CIMOMEnvironmentRef cimomEnv(env.cast_to<CIMOMEnvironment>());
	OW_ASSERT(cimomEnv);
	m_indicationServerThread->init(cimomEnv);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::start()
{
	m_indicationServerThread->start();
	m_indicationServerThread->waitUntilReady();
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::shutdown()
{
	m_indicationServerThread->shutdown();
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::processIndication(const CIMInstance& instance,const String& instNS)
{
	m_indicationServerThread->processIndication(instance, instNS);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startDeleteSubscription(const String& ns, const CIMObjectPath& subPath)
{
	m_indicationServerThread->startDeleteSubscription(ns, subPath);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startCreateSubscription(const String& ns, const CIMInstance& subInst, const String& username)
{
	m_indicationServerThread->startCreateSubscription(ns, subInst, username);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startModifySubscription(const String& ns, const CIMInstance& subInst)
{
	m_indicationServerThread->startModifySubscription(ns, subInst);
}
	
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::modifyFilter(const String& ns, const CIMInstance& filterInst, const String& userName)
{
	m_indicationServerThread->modifyFilter(ns, filterInst, userName);
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
	Notifier(IndicationServerImplThread* pmgr, NotifyTrans& ntrans) :
		m_pmgr(pmgr), m_trans(ntrans) {}
	virtual void run();
private:
	virtual void doCooperativeCancel();
	virtual void doDefinitiveCancel();
	IndicationServerImplThread* m_pmgr;
	NotifyTrans m_trans;
};
class IndicationServerProviderEnvironment : public ProviderEnvironmentIFC
{
public:
	IndicationServerProviderEnvironment(
		const CIMOMEnvironmentRef& env)
		: ProviderEnvironmentIFC()
		, m_opctx()
		, m_env(env)
	{
	}
	virtual CIMOMHandleIFCRef getCIMOMHandle() const
	{
		return m_env->getCIMOMHandle(m_opctx);;
	}
	
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
	{
		return m_env->getCIMOMHandle(m_opctx, ServiceEnvironmentIFC::E_BYPASS_PROVIDERS);;
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
		return m_env->getLogger(COMPONENT_NAME);
	}
	virtual LoggerRef getLogger(const String& componentName) const
	{
		return m_env->getLogger(componentName);
	}
	virtual String getUserName() const
	{
		return Platform::getCurrentUserName();
	}
	virtual OperationContext& getOperationContext()
	{
		return m_opctx;
	}
	virtual ProviderEnvironmentIFCRef clone() const
	{
		return ProviderEnvironmentIFCRef(new IndicationServerProviderEnvironment(m_env));
	}
private:
	mutable OperationContext m_opctx;
	CIMOMEnvironmentRef m_env;
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
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Caught exception while exporting indication: %1", e));
	}
	catch(ThreadCancelledException&)
	{
		throw;
	}
	catch(...)
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "Unknown exception caught while exporting indication");
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
IndicationServerImplThread::IndicationServerImplThread()
	: m_shuttingDown(false)
	, m_startedBarrier(2)
{
}
namespace
{
//////////////////////////////////////////////////////////////////////////////
class instanceEnumerator : public CIMInstanceResultHandlerIFC
{
public:
	instanceEnumerator(IndicationServerImplThread* is_,
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

		try
		{
			// TODO: If the provider rejects the subscription, we need to disable it!
			is->createSubscription(ns, i, username);
		}
		catch(Exception& e)
		{
			// Something was wrong with the last subscription.
			// If we allow this exception to pass through, all subsequent
			// subscriptions will be ignored at init time.
			// We'll ignore it here to keep that from happening.
		}
	}
	IndicationServerImplThread* is;
	String ns;
};
//////////////////////////////////////////////////////////////////////////////
class namespaceEnumerator : public StringResultHandlerIFC
{
public:
	namespaceEnumerator(
		const CIMOMHandleIFCRef& ch_,
		IndicationServerImplThread* is_)
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
	IndicationServerImplThread* is;
};
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::init(const CIMOMEnvironmentRef& env)
{
	m_env = env;
	m_logger = env->getLogger(COMPONENT_NAME);
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
				maxIndicationExportThreads, maxIndicationExportThreads * 100, m_logger, "Indication Server Notifiers"));
	
	// pool to handle threads modifying subscriptions
	m_subscriptionPool = ThreadPoolRef(new ThreadPool(ThreadPool::DYNAMIC_SIZE,
		1, // 1 thread because only 1 can run at a time because of mutex locking.
		   // Also modifyFilter() takes advantage of this detail to make sure a delete/create are processed in order.
		0, // unlimited size queue
		m_logger, "Indication Server Subscriptions"));

	//-----------------
	// Load map with available indication export providers
	//-----------------
	ProviderManagerRef pProvMgr = m_env->getProviderManager();
	OperationContext context;
	IndicationExportProviderIFCRefArray pra =
		pProvMgr->getIndicationExportProviders(createProvEnvRef(m_env));
	OW_LOG_DEBUG(m_logger, Format("IndicationServerImplThread: %1 export providers found",
		pra.size()));
	for (size_t i = 0; i < pra.size(); i++)
	{
		StringArray clsNames = pra[i]->getHandlerClassNames();
		for (size_t j = 0; j < clsNames.size(); j++)
		{
			m_providers[clsNames[j]] = pra[i];
			OW_LOG_DEBUG(m_logger, Format("IndicationServerImplThread: Handling"
				" indication type %1", clsNames[j]));
		}
	}

	// get the wql lib
	m_wqlRef = m_env->getWQLRef();
	if (!m_wqlRef)
	{
		const char* const err = "Cannot process indications, because there is no "
			"WQL library.";
		OW_LOG_FATAL_ERROR(m_logger, err);
		OW_THROW(IndicationServerException, err);
	}

	// Now initialize for all the subscriptions that exist in the repository.
	// This calls createSubscription for every instance of
	// CIM_IndicationSubscription in all namespaces.
	// TODO: If the provider rejects the subscription, we need to disable it!
	CIMOMHandleIFCRef lch = m_env->getCIMOMHandle(context);
	namespaceEnumerator nsHandler(lch, this);
	env->getRepository()->enumNameSpace(nsHandler, context);
}

//////////////////////////////////////////////////////////////////////////////
CIMOMEnvironmentRef
IndicationServerImplThread::getEnvironment() const
{
	return m_env;
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::waitUntilReady()
{
	m_startedBarrier.wait();
}
//////////////////////////////////////////////////////////////////////////////
IndicationServerImplThread::~IndicationServerImplThread()
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
IndicationServerImplThread::run()
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
				OW_LOG_ERROR(m_logger, Format("IndicationServerImplThread::run caught "
					" exception %1", e));
			}
			catch(ThreadCancelledException&)
			{
				throw;
			}
			catch(...)
			{
				OW_LOG_ERROR(m_logger, "IndicationServerImplThread::run caught unknown"
					" exception");
				// Ignore?
			}
		}
	}
	OW_LOG_DEBUG(m_logger, "IndicationServerImplThread::run shutting down");
	m_subscriptionPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, 5);
	m_notifierThreadPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, 60);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::deactivateAllSubscriptions()
{
	typedef std::set<SubscriptionRef> SubSet;
	SubSet uniqueSubscriptions;

	for (subscriptions_t::iterator curSubscription = m_subscriptions.begin();
		  curSubscription != m_subscriptions.end(); ++curSubscription)
	{
		uniqueSubscriptions.insert(curSubscription->second);
	}

	for (SubSet::iterator curSubscription = uniqueSubscriptions.begin(); curSubscription != uniqueSubscriptions.end(); ++curSubscription)
	{
		Subscription& sub(**curSubscription);
		IndicationProviderIFCRefArray& providers(sub.m_providers);
		for (IndicationProviderIFCRefArray::iterator curProvider = providers.begin();
			  curProvider != providers.end(); ++curProvider)
		{
			try
			{
				OW_LOG_DEBUG(m_logger, Format("About to call deActivateFilter() for subscription %1, provider %2",
					sub.m_subPath.toString(), curProvider - providers.begin()));
				(*curProvider)->deActivateFilter(createProvEnvRef(m_env), sub.m_selectStmt, sub.m_selectStmt.getClassName(),
					sub.m_subPath.getNameSpace(), sub.m_classes);
				OW_LOG_DEBUG(m_logger, "deActivateFilter() done");
			}
			catch (Exception& e)
			{
				OW_LOG_ERROR(m_logger, Format("Caught exception while calling deActivateFilter(): %1", e));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::shutdown()
{
	{
		NonRecursiveMutexLock l(m_mainLoopGuard);
		m_shuttingDown = true;
		m_mainLoopCondition.notifyAll();
	}
	// wait until the main thread exits.
	this->join();
	
	deactivateAllSubscriptions();

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
IndicationServerImplThread::processIndication(const CIMInstance& instanceArg,
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
IndicationServerImplThread::_processIndication(const CIMInstance& instanceArg,
	const String& instNS)
{
	OW_LOG_DEBUG(m_logger, Format("IndicationServerImplThread::_processIndication "
		"instanceArg = %1 instNS = %2", instanceArg.toString(), instNS));
	
	// If the provider didn't set the IndicationTime property, then we'll set it.
	// DN 01/25/2005: removing this, since not all indications may have the IndicationTime property, and it's not required anyway.
	// The indication producers should set it if necessary.
	//CIMInstance instanceArg(instanceArg_);
	//if (!instanceArg.getProperty("IndicationTime"))
	//{
	//	DateTime dtm;
	//	dtm.setToCurrent();
	//	CIMDateTime cdt(dtm);
	//	instanceArg.setProperty("IndicationTime", CIMValue(cdt));
	//}

	CIMName curClassName = instanceArg.getClassName();
	if (curClassName == CIMName())
	{
		OW_LOG_ERROR(m_logger, "Cannot process indication, because it has no "
			"class name.");
	}
	while (curClassName != CIMName())
	{
		String key = curClassName.toString();
		key.toLowerCase();
		{
			MutexLock lock(m_subGuard);
			OW_LOG_DEBUG(m_logger, Format("searching for key %1", key));
			std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range =
				m_subscriptions.equal_range(key);
			OW_LOG_DEBUG(m_logger, Format("found %1 items", distance(range.first, range.second)));
			
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
					OW_LOG_DEBUG(m_logger, Format("searching for key %1", key));
					std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range =
						m_subscriptions.equal_range(key);
					OW_LOG_DEBUG(m_logger, Format("found %1 items", distance(range.first, range.second)));
					
					// make a copy of the subscriptions so we can free the lock, otherwise we may cause a deadlock.
					subscriptions_copy_t subs;
					for (subscriptions_t::iterator curSub = range.first; curSub != range.second; ++curSub)
					{
						subs.insert(subscriptions_copy_t::value_type(curSub->first, SubscriptionRef(new Subscription(*curSub->second))));
					}

					lock.release();
					_processIndicationRange(instanceArg, instNS, subs.begin(), subs.end());
				}
			}
		}
		CIMClass cc;
		try
		{
			OperationContext context;
			cc = m_env->getRepositoryCIMOMHandle(context)->getClass(instNS, curClassName.toString());
			curClassName = cc.getSuperClass();
		}
		catch (const CIMException& e)
		{
			curClassName = CIMName();
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::_processIndicationRange(
	const CIMInstance& instanceArg, const String instNS,
	IndicationServerImplThread::subscriptions_iterator first,
	IndicationServerImplThread::subscriptions_iterator last)
{
	OperationContext context;
	CIMOMHandleIFCRef hdl = m_env->getCIMOMHandle(context, CIMOMEnvironment::E_DONT_SEND_INDICATIONS);
	for ( ;first != last; ++first)
	{
		try
		{
			Subscription& sub = *(first->second);
			CIMInstance filterInst = sub.m_filter;
			String queryLanguage = sub.m_filter.getPropertyT("QueryLanguage").getValueT().toString();
			if (!sub.m_filterSourceNameSpace.equalsIgnoreCase(instNS))
			{
				OW_LOG_DEBUG(m_logger, Format("skipping sub because namespace doesn't match. Filter ns = %1, Sub ns = %2", sub.m_filterSourceNameSpace, instNS));
				continue;
			}
			//-----------------------------------------------------------------
			// Here we need to call into the WQL process with the query string
			// and the indication instance
			//-----------------------------------------------------------------
			WQLInstancePropertySource propSource(instanceArg, hdl, instNS);
			if (!sub.m_compiledStmt.evaluate(propSource))
			{
				OW_LOG_DEBUG(m_logger, "skipping sub because wql.evaluate doesn't match");
				continue;
			}
			CIMInstance filteredInstance(filterInstance(instanceArg,
				sub.m_selectStmt.getSelectPropertyNames()));
			// Now get the export handler for this indication subscription
			// TODO: get this when the subscription is created. No reason to keep fetching it whenever an indication is exported. We'll have to watch it for changes.
			CIMObjectPath handlerCOP =
				sub.m_subPath.getKeyT("Handler").getValueT().toCIMObjectPath();

			String handlerNS = handlerCOP.getNameSpace();
			if (handlerNS.empty())
				handlerNS = instNS;

			CIMInstance handler = hdl->getInstance(handlerNS,
				handlerCOP);
			if (!handler)
			{
				OW_LOG_ERROR(m_logger, Format("Handler does not exist: %1",
					handlerCOP.toString()));
				continue;
			}
			// Get the appropriate export provider for the subscription
			IndicationExportProviderIFCRef pref = getProvider(
				handler.getClassName());
			
			if (!pref)
			{
				OW_LOG_ERROR(m_logger, Format("No indication handler for class name:"
					" %1", handler.getClassName()));
			
				continue;
			}
			addTrans(instNS, filteredInstance, handler, sub.m_sub, pref);
		}
		catch(Exception& e)
		{
			OW_LOG_ERROR(m_logger, Format("Error occurred while exporting indications:"
				" %1", e).c_str());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::addTrans(
	const String& ns,
	const CIMInstance& indication,
	const CIMInstance& handler,
	const CIMInstance& subscription,
	IndicationExportProviderIFCRef provider)
{
	NotifyTrans trans(ns, indication, handler, subscription, provider);
	if (!m_notifierThreadPool->tryAddWork(RunnableRef(new Notifier(this, trans))))
	{
		OW_LOG_ERROR(m_logger, Format("Indication export notifier pool overloaded.  Dropping indication: %1", indication.toMOF()));
	}
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRef
IndicationServerImplThread::getProvider(const CIMName& className)
{
	IndicationExportProviderIFCRef pref(0);
	provider_map_t::iterator it =
		m_providers.find(className);
	if (it != m_providers.end())
	{
		pref = it->second;
	}
	return pref;
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::deleteSubscription(const String& ns, const CIMObjectPath& subPath)
{
	OW_LOG_DEBUG(m_logger, Format("IndicationServerImplThread::deleteSubscription ns = %1, subPath = %2", ns, subPath.toString()));
	CIMObjectPath cop(subPath);
	cop.setNameSpace(ns);
	OW_LOG_DEBUG(m_logger, Format("cop = %1", cop));
	
	typedef std::set<SubscriptionRef> SubSet;
	SubSet uniqueSubscriptions;

	// The hash map m_subscriptions has duplicate entries for the same subscription, so we have to create a unique set, which
	// should end up containing only one entry for the subscription that is being deleted.
	{
		MutexLock l(m_subGuard);
		subscriptions_t::iterator curSubscription = m_subscriptions.begin();
		while (curSubscription != m_subscriptions.end())
		{
			OW_LOG_DEBUG(m_logger, Format("subPath = %1", curSubscription->second->m_subPath));
			if (cop.equals(curSubscription->second->m_subPath))
			{
				OW_LOG_DEBUG(m_logger, "found a match");
				uniqueSubscriptions.insert(curSubscription->second);
				m_subscriptions.erase(curSubscription++);
			}
			else
			{
				++curSubscription;
			}
		}
	}

	OW_ASSERT(uniqueSubscriptions.size() == 1);

	for (SubSet::iterator curSubscription = uniqueSubscriptions.begin(); curSubscription != uniqueSubscriptions.end(); ++curSubscription)
	{
		Subscription& sub(**curSubscription);
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
						CIMName key = sub.m_classes[j];
						poller_map_t::iterator iter = m_pollers.find(key);
						if (iter != m_pollers.end())
						{
							LifecycleIndicationPollerRef p = iter->second;
							CIMName subClsName = sub.m_selectStmt.getClassName();
							bool removePoller = false;
							if (subClsName == "CIM_InstCreation")
							{
								removePoller = p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
							}
							else if (subClsName == "CIM_InstModification")
							{
								removePoller = p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
							}
							else if (subClsName == "CIM_InstDeletion")
							{
								removePoller = p->removePollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
							}
							else if (subClsName == "CIM_InstIndication" || subClsName == "CIM_Indication")
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
				OW_LOG_ERROR(m_logger, Format("Caught exception while calling deActivateFilter for provider: %1", e));
			}
			catch(ThreadCancelledException&)
			{
				throw;
			}
			catch (...)
			{
				OW_LOG_ERROR(m_logger, "Caught unknown exception while calling deActivateFilter for provider");
			}
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
		return CIMNameSpaceUtils::prepareNamespace(inst.getPropertyT("SourceNamespace").getValueT().toString());
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
	IndicationServerImplThread* is;
public:
	createSubscriptionRunnable(const String& ns_, const CIMInstance& subInst_, const String& username_, IndicationServerImplThread* is_)
	: ns(ns_)
	, subInst(subInst_)
	, username(username_)
	, is(is_)
	{}

	virtual void run()
	{
		USE_CIMOM_UID;

		is->createSubscription(ns, subInst, username);
	}
}; // end class createSubscriptionRunnable

class modifySubscriptionRunnable : public Runnable
{
	String ns;
	CIMInstance subInst;
	IndicationServerImplThread* is;
public:
	modifySubscriptionRunnable(const String& ns_, const CIMInstance& subInst_, IndicationServerImplThread* is_)
	: ns(ns_)
	, subInst(subInst_)
	, is(is_)
	{}

	virtual void run()
	{
		USE_CIMOM_UID;
		is->modifySubscription(ns, subInst);
	}
}; // end class modifySubscriptionRunnable

class deleteSubscriptionRunnable : public Runnable
{
	String ns;
	CIMObjectPath sub;
	IndicationServerImplThread* is;
public:
	deleteSubscriptionRunnable(const String& ns_, const CIMObjectPath& sub_, IndicationServerImplThread* is_)
	: ns(ns_)
	, sub(sub_)
	, is(is_)
	{}

	virtual void run()
	{
		USE_CIMOM_UID;
		is->deleteSubscription(ns, sub);
	}
}; // end class deleteSubscriptionRunnable

} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::startCreateSubscription(const String& ns, const CIMInstance& subInst, const String& username)
{
	RunnableRef rr(new createSubscriptionRunnable(ns, subInst, username, this));
	m_subscriptionPool->addWork(rr);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::startModifySubscription(const String& ns, const CIMInstance& subInst)
{
	RunnableRef rr(new modifySubscriptionRunnable(ns, subInst, this));
	m_subscriptionPool->addWork(rr);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::startDeleteSubscription(const String& ns, const CIMObjectPath& sub)
{
	RunnableRef rr(new deleteSubscriptionRunnable(ns, sub, this));
	m_subscriptionPool->addWork(rr);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::createSubscription(const String& ns, const CIMInstance& subInst, const String& username)
{
	OW_LOG_DEBUG(m_logger, Format("IndicationServerImplThread::createSubscription ns = %1, subInst = %2", ns, subInst.toString()));
	
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
	OW_LOG_DEBUG(m_logger, Format("Got query statement (%1) in %2", filterQuery, queryLanguage));
	if (!m_wqlRef->supportsQueryLanguage(queryLanguage))
	{
		OW_THROWCIMMSG(CIMException::FAILED, Format("Filter uses queryLanguage %1, which is"
			" not supported", queryLanguage).c_str());
	}

	WQLSelectStatement selectStmt(m_wqlRef->createSelectStatement(filterQuery));
	WQLCompile compiledStmt(selectStmt);
	const WQLCompile::Tableau& tableau(compiledStmt.getTableau());
	CIMName indicationClassName = selectStmt.getClassName();
	OW_LOG_DEBUG(m_logger, Format("query is for indication class: %1", indicationClassName));

	// collect up all the class names
	CIMNameArray isaClassNames;
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
						OW_LOG_DEBUG(m_logger, Format("Found ISA class name: %1", opn2.getPropertyName()));
					}
					else if (opn2.getType() == WQLOperand::STRING_VALUE)
					{
						isaClassNames.push_back(opn2.getStringValue());
						OW_LOG_DEBUG(m_logger, Format("Found ISA class name: %1", opn2.getStringValue()));
					}
				}
			}
		}
	}

	//"The path to a local namespace where the Indications "
	//"originate. If NULL, the namespace of the Filter registration "
	//"is assumed."
	// first try to get it from the property
	String filterSourceNameSpace = getSourceNameSpace(filterInst);
	if (filterSourceNameSpace.empty())
	{
		filterSourceNameSpace = filterNS;
	}

	// look up all the subclasses of the classes in isaClassNames.
	CIMNameArray subClasses;
	for (size_t i = 0; i < isaClassNames.size(); ++i)
	{
		try
		{
			StringArray tmp(hdl->enumClassNamesA(filterSourceNameSpace, isaClassNames[i].toString()));
			subClasses.insert(subClasses.end(), tmp.begin(), tmp.end());
		}
		catch (CIMException& e)
		{
			String msg = Format("Indication Server (subscription creation): failed to get subclass names of %1:%2 because: %3",
				filterSourceNameSpace, isaClassNames[i], e.getMessage());
			OW_LOG_ERROR(m_logger, msg);
			OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
		}
	}
	
	isaClassNames.appendArray(subClasses);

	// get rid of duplicates - unique() requires that the range be sorted
	std::sort(isaClassNames.begin(), isaClassNames.end());
	isaClassNames.erase(std::unique(isaClassNames.begin(), isaClassNames.end()), isaClassNames.end());

	OStringStream ss;
	std::copy(isaClassNames.begin(), isaClassNames.end(), std::ostream_iterator<CIMName>(ss, ", "));
	OW_LOG_DEBUG(m_logger, Format("isaClassNames = %1", ss.toString()));

	// we need to make a copy of this to pass to indication provider.  Darn backward compatibility :(
	StringArray strIsaClassNames;
	strIsaClassNames.reserve(isaClassNames.size());
	for (size_t i = 0; i < isaClassNames.size(); ++i)
	{
		strIsaClassNames.push_back(isaClassNames[i].toString());
	}

	// find providers that support this query. If none are found, throw an exception.
	ProviderManagerRef pm (m_env->getProviderManager());
	IndicationProviderIFCRefArray providers;

	if (!isaClassNames.empty())
	{
		providers = pm->getIndicationProviders(createProvEnvRef(m_env),
			ns, indicationClassName, isaClassNames);
	}
	else
	{
		providers = pm->getIndicationProviders(createProvEnvRef(m_env), ns,
			indicationClassName, CIMNameArray());
	}
	
	OW_LOG_DEBUG(m_logger, Format("Found %1 providers for the subscription", providers.size()));
	if (providers.empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "No indication provider found for this subscription");
	}

	// verify that there is an indication export provider that can handle the handler for the subscription
	CIMObjectPath handlerPath = subInst.getProperty("Handler").getValueT().toCIMObjectPath();
	CIMName handlerClass = handlerPath.getClassName();
	if (!getProvider(handlerClass))
	{
		OW_THROWCIMMSG(CIMException::FAILED, "No indication export provider found for this subscription");
	}
	// call authorizeFilter on all the indication providers
	for (size_t i = 0; i < providers.size(); ++i)
	{
		OW_LOG_DEBUG(m_logger, Format("Calling authorizeFilter for provider %1", i));
		providers[i]->authorizeFilter(createProvEnvRef(m_env),
			selectStmt, indicationClassName.toString(), ns, strIsaClassNames, username);
	}
	// Call mustPoll on all the providers
	Array<bool> isPolled(providers.size(), false);
	for (size_t i = 0; i < providers.size(); ++i)
	{
		try
		{
			OW_LOG_DEBUG(m_logger, Format("Calling mustPoll for provider %1", i));
			int pollInterval = providers[i]->mustPoll(createProvEnvRef(m_env),
				selectStmt, indicationClassName.toString(), ns, strIsaClassNames);
			OW_LOG_DEBUG(m_logger, Format("got pollInterval %1", pollInterval));
			if (pollInterval > 0)
			{
				isPolled[i] = true;
				for (size_t j = 0; j < isaClassNames.size(); ++j)
				{
					CIMName key = isaClassNames[j];
					OW_LOG_DEBUG(m_logger, Format("searching on class key %1", isaClassNames[j]));
					poller_map_t::iterator iter = m_pollers.find(key);
					LifecycleIndicationPollerRef p;
					if (iter != m_pollers.end())
					{
						OW_LOG_DEBUG(m_logger, Format("found on class key %1: %2", isaClassNames[j], iter->first));
						p = iter->second;
					}
					else
					{
						OW_LOG_DEBUG(m_logger, Format("not found on class key %1", isaClassNames[j]));
						p = LifecycleIndicationPollerRef(SharedLibraryRef(0),
							LifecycleIndicationPollerRef::element_type(new LifecycleIndicationPoller(ns, key, pollInterval)));
					}
					CIMName subClsName = selectStmt.getClassName();
					if (subClsName == "CIM_InstCreation")
					{
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
					}
					else if (subClsName == "CIM_InstModification")
					{
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
					}
					else if (subClsName == "CIM_InstDeletion")
					{
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
					}
					else if (subClsName == "CIM_InstIndication" || subClsName == "CIM_Indication")
					{
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
						p->addPollOp(LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
					}
					p->addPollInterval(pollInterval);
					if (iter == m_pollers.end())
					{
						OW_LOG_DEBUG(m_logger, Format("Inserting %1 into m_pollers", key));
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
			OW_LOG_ERROR(m_logger, Format("Caught exception while calling mustPoll for provider: %1", ce));
		}
		catch(ThreadCancelledException&)
		{
			throw;
		}
		catch (...)
		{
			OW_LOG_ERROR(m_logger, "Caught unknown exception while calling mustPoll for provider");
		}
	}

	// create a subscription (save the compiled filter and other info)
	SubscriptionRef sub(new Subscription);
	sub->m_subPath = CIMObjectPath(ns, subInst);
	sub->m_sub = subInst;
	sub->m_providers = providers;
	sub->m_isPolled = isPolled;
	sub->m_filter = filterInst;
	sub->m_selectStmt = selectStmt;
	sub->m_compiledStmt = compiledStmt;
	sub->m_classes = strIsaClassNames;

	// m_filterSourceNamespace is saved so _processIndication can do what the
	// schema says:
	//"The path to a local namespace where the Indications "
	//"originate. If NULL, the namespace of the Filter registration "
	//"is assumed."
	// first try to get it from the property
	sub->m_filterSourceNameSpace = filterSourceNameSpace;

	// get the lock and put it in m_subscriptions
	{
		MutexLock l(m_subGuard);
		if (isaClassNames.empty())
		{
			String subKey = indicationClassName.toString();
			subKey.toLowerCase();
			m_subscriptions.insert(std::make_pair(subKey, sub));
		}
		else
		{
			for (size_t i = 0; i < isaClassNames.size(); ++i)
			{
				String subKey = indicationClassName.toString() + ':' + isaClassNames[i].toString();
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
				selectStmt, indicationClassName.toString(), ns, strIsaClassNames);

			++successfulActivations;
		}
		catch (CIMException& ce)
		{
			OW_LOG_ERROR(m_logger, Format("Caught exception while calling activateFilter for provider: %1", ce));
		}
		catch(ThreadCancelledException&)
		{
			throw;
		}
		catch (...)
		{
			OW_LOG_ERROR(m_logger, "Caught unknown exception while calling activateFilter for provider");
		}
	}

	if (successfulActivations == 0)
	{
		// Remove it and throw
		MutexLock l(m_subGuard);
		if (isaClassNames.empty())
		{
			String subKey = indicationClassName.toString();
			subKey.toLowerCase();
			m_subscriptions.erase(subKey);
		}
		else
		{
			for (size_t i = 0; i < isaClassNames.size(); ++i)
			{
				String subKey = indicationClassName.toString() + ':' + isaClassNames[i].toString();
				subKey.toLowerCase();
				m_subscriptions.erase(subKey);
			}
		}
		OW_THROWCIMMSG(CIMException::FAILED, "activateFilter failed for all providers");
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::modifySubscription(const String& ns, const CIMInstance& subInst)
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
		Subscription& sub = *(iter->second);
		if (cop.equals(sub.m_subPath))
		{
			sub.m_sub = subInst;
			break; // should only be one subscription to update
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::modifyFilter(const String& ns, const CIMInstance& filterInst, const String& userName)
{
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	// Implementation note: This depends on the fact that the indication subscription creation/deletion events are
	// processed sequentially (the thread pool only has 1 worker thread), so that the deletion is processed
	// before the creation.
	try
	{
		OperationContext context;
		CIMOMHandleIFCRef hdl(m_env->getRepositoryCIMOMHandle(context));
		// get all the CIM_IndicationSubscription instances referencing the filter
		CIMObjectPath filterPath(ns, filterInst);
		CIMInstanceArray subscriptions(hdl->referencesA(ns, filterPath, "CIM_IndicationSubscription", "Filter"));

		// call startDeleteSubscription on the old instances
		for (size_t i = 0; i < subscriptions.size(); ++i)
		{
			startDeleteSubscription(ns, CIMObjectPath(ns, subscriptions[i]));
		}
		
		// call startCreateSubscription on the new instances
		for (size_t i = 0; i < subscriptions.size(); ++i)
		{
			startCreateSubscription(ns, subscriptions[i], userName);
		}

	}
	catch (CIMException& e)
	{
		OW_THROWCIMMSG_SUBEX(CIMException::FAILED, "modifying the filter failed", e);
	}

#else
	OW_THROWCIMMSG(CIMException::FAILED, "Modifying the filter not allowed because association traversal is disabled");
#endif
}

void
IndicationServerImplThread::doCooperativeCancel()
{
	NonRecursiveMutexLock l(m_mainLoopGuard);
	m_shuttingDown = true;
	m_mainLoopCondition.notifyAll();
}

} // end namespace OW_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
extern "C" OW_EXPORT OW_NAMESPACE::IndicationServer*
createIndicationServer()
{
	return new OW_NAMESPACE::IndicationServerImpl();
}
//////////////////////////////////////////////////////////////////////////////
#if !defined(OW_STATIC_SERVICES)
extern "C" OW_EXPORT const char*
getOWVersion()
{
	return OW_VERSION;
}
#endif /* !defined(OW_STATIC_SERVICES) */
