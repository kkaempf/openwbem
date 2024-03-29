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

#include "OW_config.h"
#include "OW_IndicationServerImpl.hpp"
#include "blocxx/DateTime.hpp"
#include "blocxx/Assertion.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/Logger.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_CIMValueCast.hpp"
#include "blocxx/SortedVectorSet.hpp"
#include "OW_NULLValueException.hpp"
#include "OW_PollingManager.hpp"
#include "OW_CppProxyProvider.hpp"
#include "OW_Platform.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "blocxx/MutexLock.hpp"
#include "OW_CIMClass.hpp"
#include "OW_WQLInstancePropertySource.hpp"
#include "OW_LocalOperationContext.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_LifecycleIndicationPoller.hpp"
#include "OW_ServiceIFCNames.hpp"
#include "OW_CIMNameSpaceUtils.hpp"

#include <iterator>
#include <set>

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(IndicationServer);
OW_DEFINE_EXCEPTION_WITH_ID(IndicationServer);

using namespace WBEMFlags;
using namespace blocxx;

namespace
{
const String COMPONENT_NAME("ow.owcimomd.indication.Server");
const String PROP_OnFatalErrorPolicy("OnFatalErrorPolicy");
const UInt16 PROP_OnFatalErrorPolicy_Remove = 4;
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
	BLOCXX_ASSERT(cimomEnv);
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
IndicationServerImpl::processIndication(const CIMInstance& indication,const String& indicationNS)
{
	m_indicationServerThread->processIndication(indication, indicationNS);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startDeleteSubscription(const String& subNS, const CIMObjectPath& subPath)
{
	m_indicationServerThread->startDeleteSubscription(subNS, subPath);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startCreateSubscription(const String& subNS, const CIMInstance& subInst, const String& username)
{
	m_indicationServerThread->startCreateSubscription(subNS, subInst, username);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::startModifySubscription(const String& subNS, const CIMInstance& subInst)
{
	m_indicationServerThread->startModifySubscription(subNS, subInst);
}

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImpl::modifyFilter(OperationContext& context, const String& filterNS, const CIMInstance& filterInst, const String& userName)
{
	m_indicationServerThread->modifyFilter(context, filterNS, filterInst, userName);
}

//////////////////////////////////////////////////////////////////////////////
struct NotifyTrans
{
	NotifyTrans(
		const String& indicationNS,
		const CIMInstance& indication,
		const CIMInstance& handler,
		const String& subNS,
		const CIMInstance& subscription,
		const IndicationExportProviderIFCRef provider) :
			m_indicationNS(indicationNS), m_indication(indication), m_handler(handler), m_subNS(subNS), m_subscription(subscription), m_provider(provider)
	{
	}

	String m_indicationNS;
	CIMInstance m_indication;
	CIMInstance m_handler;
	String m_subNS;
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
	virtual void doShutdown();
	virtual void doCooperativeCancel();
	virtual void doDefinitiveCancel();
	void handleDeliveryError();
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
	virtual RepositoryIFCRef getAuthorizingRepository() const
	{
		return m_env->getAuthorizingRepository();
	}
	virtual String getConfigItem(const String& name, const String& defRetVal="") const
	{
		return m_env->getConfigItem(name, defRetVal);
	}
	virtual StringArray getMultiConfigItem(const String &itemName,
		const StringArray& defRetVal, const char* tokenizeSeparator = 0) const
	{
		return m_env->getMultiConfigItem(itemName, defRetVal, tokenizeSeparator);
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
	mutable LocalOperationContext m_opctx;
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
	CIMOMEnvironmentRef env = m_pmgr->getEnvironment();
	try
	{
		m_trans.m_provider->exportIndication(createProvEnvRef(env),
			m_trans.m_indicationNS, m_trans.m_handler, m_trans.m_indication);
		return; // errors are handled after the catch blocks
	}
	catch(Exception& e)
	{
		Logger lgr(COMPONENT_NAME);
		BLOCXX_LOG_ERROR(lgr, Format("Caught exception while exporting indication: %1", e));
	}
	catch(ThreadCancelledException&)
	{
		throw;
	}
	catch(...)
	{
		Logger lgr(COMPONENT_NAME);
		BLOCXX_LOG_ERROR(lgr, "Unknown exception caught while exporting indication");
	}

	handleDeliveryError();
}
//////////////////////////////////////////////////////////////////////////////
void
Notifier::handleDeliveryError()
{
	Logger lgr(COMPONENT_NAME);
	CIMOMEnvironmentRef env = m_pmgr->getEnvironment();
	if (m_trans.m_subscription.propertyHasValue(PROP_OnFatalErrorPolicy))
	{
		/// @todo  handle all the cases
		UInt16 onFatalErrorPolicy = m_trans.m_subscription.getPropertyValue(PROP_OnFatalErrorPolicy).toUInt16();
		switch (onFatalErrorPolicy)
		{
			case PROP_OnFatalErrorPolicy_Remove:
				try
				{
					LocalOperationContext context;
					CIMOMHandleIFCRef lch = env->getCIMOMHandle(context);
					lch->deleteInstance(m_trans.m_subNS, CIMObjectPath(m_trans.m_subNS,m_trans.m_subscription));
				}
				catch (CIMException& e)
				{
					// NOT_FOUND means it was already deleted.
					// If the exception was something else, log it.
					if (e.getErrorCode() != CIMException::NOT_FOUND)
					{
						BLOCXX_LOG_ERROR(lgr, Format("Notifier::handleDeliveryError() failed to remove subscription. Error: %1", e));
					}
				}
				break;
			default:
				BLOCXX_LOG_INFO(lgr, Format("Unimplemented value for OnFatalErrorPolicy: %1", onFatalErrorPolicy));
				break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
Notifier::doShutdown()
{
	m_trans.m_provider->doShutdown();
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
	, m_logger(COMPONENT_NAME)
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
			/// @todo  If the provider rejects the subscription, we need to disable it!
			is->startCreateSubscription(ns, i, username);
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
	IndicationExportProviderIFCRefArray pra =
		pProvMgr->getIndicationExportProviders();
	BLOCXX_LOG_DEBUG(m_logger, Format("IndicationServerImplThread: %1 export providers found", pra.size()));
	for (size_t i = 0; i < pra.size(); i++)
	{
		StringArray clsNames = pra[i]->getHandlerClassNames();
		for (size_t j = 0; j < clsNames.size(); j++)
		{
			m_providers[clsNames[j]] = pra[i];
			BLOCXX_LOG_DEBUG(m_logger, Format("IndicationServerImplThread: Handling indication type %1", clsNames[j]));
		}
	}

	// get the wql lib
	m_wqlRef = m_env->getWQLRef();
	if (!m_wqlRef)
	{
		const char* const strerr = "Cannot process indications, because there is no WQL library.";
		BLOCXX_LOG_FATAL_ERROR(m_logger, strerr);
		OW_THROW(IndicationServerException, strerr);
	}

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
UInt32
IndicationServerImplThread::activateFilterOnProvider(
	IndicationProviderIFCRef& prov)
{
	MutexLock l(m_actCountGuard);
	UInt32 actcount = 0;
	activatecount_map_t::iterator it = m_activations.find(prov);
	if (it != m_activations.end())
	{
		actcount = it->second;
	}
	actcount++;
	m_activations[prov] = actcount;
	return actcount;
}

//////////////////////////////////////////////////////////////////////////////
UInt32
IndicationServerImplThread::deActivateFilterOnProvider(
	IndicationProviderIFCRef& prov)
{
	MutexLock l(m_actCountGuard);
	UInt32 actcount = 0;
	activatecount_map_t::iterator it = m_activations.find(prov);
	if (it != m_activations.end())
	{
		if (it->second != 0)
		{
			it->second--;
		}
		actcount = it->second;
	}
	return actcount;
}

//////////////////////////////////////////////////////////////////////////////
Int32
IndicationServerImplThread::run()
{
	// let CIMOMEnvironment know we're running and ready to go.
	m_startedBarrier.wait();

	// Now initialize for all the subscriptions that exist in the repository.
	// This calls createSubscription for every instance of
	// CIM_IndicationSubscription in all namespaces.
	/// @todo  If the provider rejects the subscription, we need to disable it!
	LocalOperationContext context;
	CIMOMHandleIFCRef lch = m_env->getCIMOMHandle(context);
	namespaceEnumerator nsHandler(lch, this);
	m_env->getRepository()->enumNameSpace(nsHandler, context);

	{
		NonRecursiveMutexLock l(m_mainLoopGuard);
		while (!m_shuttingDown)
		{
			m_mainLoopCondition.wait(l);

			while (!m_procTrans.empty() && !m_shuttingDown)
			{
				ProcIndicationTrans trans = m_procTrans.front();
				l.release();
				try
				{
					_processIndication(trans.indication, trans.indicationNS);
					l.lock();
					// sucessfully processed, so remove it from the queue.
					m_procTrans.pop_front();
				}
				catch (const Exception& e)
				{
					BLOCXX_LOG_ERROR(m_logger, Format("IndicationServerImplThread::run caught "
						" exception %1", e));
					l.lock();
				}
				catch(ThreadCancelledException&)
				{
					throw;
				}
				catch(...)
				{
					BLOCXX_LOG_ERROR(m_logger, "IndicationServerImplThread::run caught unknown"
						" exception");
					// Ignore?
					l.lock();
				}
			}
		}
	}
	BLOCXX_LOG_DEBUG3(m_logger, "IndicationServerImplThread::run shutting down");
	m_subscriptionPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, Timeout::relative(5));
	m_notifierThreadPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, Timeout::relative(45), Timeout::relative(55));
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
				BLOCXX_LOG_DEBUG(m_logger, Format("About to call deActivateFilter() for subscription %1, provider %2",
					sub.m_subPath.toString(), curProvider - providers.begin()));
				(*curProvider)->deActivateFilter(createProvEnvRef(m_env), sub.m_selectStmt, sub.m_selectStmt.getClassName(),
					sub.m_subPath.getNameSpace(), sub.m_classes, true);
				BLOCXX_LOG_DEBUG3(m_logger, "deActivateFilter() done");
			}
			catch (Exception& e)
			{
				BLOCXX_LOG_ERROR(m_logger, Format("Caught exception while calling deActivateFilter(): %1", e));
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
IndicationServerImplThread::processIndication(const CIMInstance& indication,
	const String& indicationNS)
{
	NonRecursiveMutexLock ml(m_mainLoopGuard);
	if (m_shuttingDown)
	{
		return;
	}
	ProcIndicationTrans trans(indication, indicationNS);
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

String makePollerMapKey(const String& nameSpace, const CIMName& className)
{
	return nameSpace + ':' + className.toString().toLowerCase();
}

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::_processIndication(const CIMInstance& instanceArg,
	const String& instNS)
{
	BLOCXX_LOG_DEBUG(m_logger, Format("IndicationServerImplThread::_processIndication instanceArg.getClassName() = %1 instNS = %2", instanceArg.getClassName(), instNS));
	BLOCXX_LOG_DEBUG3(m_logger, Format("IndicationServerImplThread::_processIndication instanceArg = %1", instanceArg.toString()));

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
		BLOCXX_LOG_ERROR(m_logger, "Cannot process indication, because it has no class name.");
	}
	while (curClassName != CIMName())
	{
		String key = curClassName.toString();
		key.toLowerCase();
		{
			MutexLock lock(m_subGuard);
			BLOCXX_LOG_DEBUG3(m_logger, Format("searching for key %1", key));
			std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range =
				m_subscriptions.equal_range(key);
			BLOCXX_LOG_DEBUG3(m_logger, Format("found %1 items", distance(range.first, range.second)));

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
					BLOCXX_LOG_DEBUG3(m_logger, Format("searching for key %1", key));
					std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range =
						m_subscriptions.equal_range(key);
					BLOCXX_LOG_DEBUG3(m_logger, Format("found %1 items", distance(range.first, range.second)));

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
			LocalOperationContext context;
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
	LocalOperationContext context;
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
				BLOCXX_LOG_DEBUG3(m_logger, Format("skipping sub because namespace doesn't match. Filter ns = %1, Sub ns = %2", sub.m_filterSourceNameSpace, instNS));
				continue;
			}
			//-----------------------------------------------------------------
			// Here we need to call into the WQL process with the query string
			// and the indication instance
			//-----------------------------------------------------------------
			WQLInstancePropertySource propSource(instanceArg, hdl, instNS);
			if (!sub.m_compiledStmt.evaluate(propSource))
			{
				BLOCXX_LOG_DEBUG3(m_logger, "skipping sub because wql.evaluate doesn't match");
				continue;
			}
			CIMInstance filteredInstance(filterInstance(instanceArg,
				sub.m_selectStmt.getSelectPropertyNames()));
			// Now get the export handler for this indication subscription
			/// @todo  get this when the subscription is created. No reason to keep fetching it whenever an indication is exported. We'll have to watch it for changes.
			CIMObjectPath handlerCOP =
				sub.m_subPath.getKeyT("Handler").getValueT().toCIMObjectPath();

			String handlerNS = handlerCOP.getNameSpace();
			if (handlerNS.empty())
				handlerNS = instNS;

			CIMInstance handler = hdl->getInstance(handlerNS,
				handlerCOP);
			if (!handler)
			{
				BLOCXX_LOG_ERROR(m_logger, Format("Handler does not exist: %1",
					handlerCOP.toString()));
				continue;
			}
			// Get the appropriate export provider for the subscription
			IndicationExportProviderIFCRef pref = getProvider(
				handler.getClassName());

			if (!pref)
			{
				BLOCXX_LOG_ERROR(m_logger, Format("No indication handler for class name:"
					" %1", handler.getClassName()));

				continue;
			}
			addTrans(instNS, filteredInstance, handler, sub.m_subPath.getNameSpace(), sub.m_sub, pref);
		}
		catch(Exception& e)
		{
			BLOCXX_LOG_ERROR(m_logger, Format("Error occurred while exporting indications:"
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
	const String& subscriptionNS,
	const CIMInstance& subscription,
	IndicationExportProviderIFCRef provider)
{
	NotifyTrans trans(ns, indication, handler, subscriptionNS, subscription, provider);
	if (!m_notifierThreadPool->tryAddWork(RunnableRef(new Notifier(this, trans))))
	{
		BLOCXX_LOG_ERROR(m_logger, "Indication export notifier pool overloaded.  Dropping indication.");
		BLOCXX_LOG_DEBUG3(m_logger, Format("Dropped indication: %1", indication.toMOF()));
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
	BLOCXX_LOG_DEBUG(m_logger, Format("IndicationServerImplThread::deleteSubscription ns = %1, subPath = %2", ns, subPath.toString()));
	CIMObjectPath cop(subPath);
	cop.setNameSpace(ns);
	BLOCXX_LOG_DEBUG3(m_logger, Format("cop = %1", cop));

	typedef std::set<SubscriptionRef> SubSet;
	SubSet uniqueSubscriptions;

	// The hash map m_subscriptions has duplicate entries for the same subscription, so we have to create a unique set, which
	// should end up containing only one entry for the subscription that is being deleted.
	{
		MutexLock l(m_subGuard);
		subscriptions_t::iterator curSubscription = m_subscriptions.begin();
		while (curSubscription != m_subscriptions.end())
		{
			BLOCXX_LOG_DEBUG3(m_logger, Format("subPath = %1", curSubscription->second->m_subPath));
			if (cop.equals(curSubscription->second->m_subPath))
			{
				BLOCXX_LOG_DEBUG3(m_logger, "found a match");
				uniqueSubscriptions.insert(curSubscription->second);
				m_subscriptions.erase(curSubscription++);
			}
			else
			{
				++curSubscription;
			}
		}
	}

	BLOCXX_ASSERT(uniqueSubscriptions.size() == 1);

	for (SubSet::iterator curSubscription = uniqueSubscriptions.begin(); curSubscription != uniqueSubscriptions.end(); ++curSubscription)
	{
		Subscription& sub(**curSubscription);
		CIMName indicationClassName = sub.m_selectStmt.getClassName();
		for (size_t i = 0; i < sub.m_providers.size(); ++i)
		{
			try
			{
				if (sub.m_isPolled[i])
				{
					// loop through all the classes in the subscription.
					/// @todo  This is slightly less than optimal, since
					// m_classes may contain a class that isn't handled by
					// the provider
					for (size_t j = 0; j < sub.m_classes.size(); ++j)
					{
						String key = makePollerMapKey(sub.m_filterSourceNameSpace, sub.m_classes[j]);
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
					bool lastActivation = (deActivateFilterOnProvider(p) == 0);
					p->deActivateFilter(createProvEnvRef(m_env), sub.m_selectStmt,
						indicationClassName.toString(), ns, sub.m_classes, lastActivation);
				}

			}
			catch (const Exception& e)
			{
				BLOCXX_LOG_ERROR(m_logger, Format("Caught exception while calling deActivateFilter for provider: %1", e));
			}
			catch(ThreadCancelledException&)
			{
				throw;
			}
			catch (...)
			{
				BLOCXX_LOG_ERROR(m_logger, "Caught unknown exception while calling deActivateFilter for provider");
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
		is->deleteSubscription(ns, sub);
	}
}; // end class deleteSubscriptionRunnable

} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
void
IndicationServerImplThread::startCreateSubscription(const String& ns, const CIMInstance& subInst, const String& username)
{
	BLOCXX_LOG_DEBUG3(m_logger, "Entering IndicationServerImplThread::startCreateSubscription");
	RunnableRef rr(new createSubscriptionRunnable(ns, subInst, username, this));
	BLOCXX_LOG_DEBUG3(m_logger, "Created RunnableRef in IndicationServerImplThread::startCreateSubscription");
	m_subscriptionPool->addWork(rr);
	BLOCXX_LOG_DEBUG3(m_logger, "Leaving IndicationServerImplThread::startCreateSubscription");
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
	BLOCXX_LOG_DEBUG(m_logger, Format("IndicationServerImplThread::createSubscription ns = %1, subInst.getClassName() = %2", ns, subInst.getClassName()));
	BLOCXX_LOG_DEBUG3(m_logger, Format("IndicationServerImplThread::createSubscription subInst = %1", ns, subInst.toString()));

	// get the filter
	LocalOperationContext context;
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
	BLOCXX_LOG_DEBUG3(m_logger, Format("Got query statement (%1) in %2", filterQuery, queryLanguage));
	if (!m_wqlRef->supportsQueryLanguage(queryLanguage))
	{
		OW_THROWCIMMSG(CIMException::FAILED, Format("Filter uses queryLanguage %1, which is"
			" not supported", queryLanguage).c_str());
	}

	WQLSelectStatement selectStmt(m_wqlRef->createSelectStatement(filterQuery));
	WQLCompile compiledStmt(selectStmt);
	const WQLCompile::Tableau& tableau(compiledStmt.getTableau());
	CIMName indicationClassName = selectStmt.getClassName();
	BLOCXX_LOG_DEBUG3(m_logger, Format("query is for indication class: %1", indicationClassName));

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
						BLOCXX_LOG_DEBUG3(m_logger, Format("Found ISA class name: %1", opn2.getPropertyName()));
					}
					else if (opn2.getType() == WQLOperand::STRING_VALUE)
					{
						isaClassNames.push_back(opn2.getStringValue());
						BLOCXX_LOG_DEBUG3(m_logger, Format("Found ISA class name: %1", opn2.getStringValue()));
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
			BLOCXX_LOG_DEBUG3(m_logger, Format("enumClassNamesA(%1, %2) returned %3 class names", filterSourceNameSpace, isaClassNames[i].toString(), tmp.size()));
			subClasses.insert(subClasses.end(), tmp.begin(), tmp.end());
		}
		catch (CIMException& e)
		{
			String msg = Format("Indication Server (subscription creation): failed to get subclass names of %1:%2 because: %3",
				filterSourceNameSpace, isaClassNames[i], e.getMessage());
			BLOCXX_LOG_ERROR(m_logger, msg);
			OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
		}
	}

	isaClassNames.appendArray(subClasses);

	// get rid of duplicates - unique() requires that the range be sorted
	std::sort(isaClassNames.begin(), isaClassNames.end());
	isaClassNames.erase(std::unique(isaClassNames.begin(), isaClassNames.end()), isaClassNames.end());

	OStringStream ss;
	std::copy(isaClassNames.begin(), isaClassNames.end(), std::ostream_iterator<CIMName>(ss, ", "));
	BLOCXX_LOG_DEBUG3(m_logger, Format("isaClassNames = %1", ss.toString()));

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
		BLOCXX_LOG_DEBUG3(m_logger, Format("Querying ProviderManager for indication providers. filterSourceNameSpace = %1, indicationClassName = %2, isaClassNames = {%3}",
			filterSourceNameSpace, indicationClassName, ss.toString()));
		providers = pm->getIndicationProviders(
			filterSourceNameSpace, indicationClassName, isaClassNames, context);
	}
	else
	{
		BLOCXX_LOG_DEBUG3(m_logger, Format("Querying ProviderManager for indication providers. filterSourceNameSpace = %1, indicationClassName = %2, isaClassNames = {}",
			filterSourceNameSpace, indicationClassName));
		providers = pm->getIndicationProviders(
			filterSourceNameSpace, indicationClassName, CIMNameArray(), context);
	}

	BLOCXX_LOG_DEBUG(m_logger, Format("Found %1 providers for the subscription", providers.size()));
	if (providers.empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "No indication provider found for this subscription");
	}

	// verify that there is an indication export provider that can handle the handler for the subscription
	CIMObjectPath handlerPath = subInst.getProperty("Handler").getValueT().toCIMObjectPath();
	CIMName handlerClass = handlerPath.getClassName();
	if (!getProvider(handlerClass))
	{
		OW_THROWCIMMSG(CIMException::FAILED, Format("No indication export provider found for the subscription's handler class: %1", handlerClass).c_str());
	}
	// call authorizeFilter on all the indication providers
	for (size_t i = 0; i < providers.size(); ++i)
	{
		BLOCXX_LOG_DEBUG2(m_logger, Format("Calling authorizeFilter for provider %1", i));
		providers[i]->authorizeFilter(createProvEnvRef(m_env),
			selectStmt, indicationClassName.toString(), ns, strIsaClassNames, username);
	}
	// Call mustPoll on all the providers
	Array<bool> isPolled(providers.size(), false);
	for (size_t i = 0; i < providers.size(); ++i)
	{
		try
		{
			BLOCXX_LOG_DEBUG2(m_logger, Format("Calling mustPoll for provider %1", i));
			int pollInterval = providers[i]->mustPoll(createProvEnvRef(m_env),
				selectStmt, indicationClassName.toString(), ns, strIsaClassNames);
			BLOCXX_LOG_DEBUG3(m_logger, Format("got pollInterval %1", pollInterval));
			if (pollInterval > 0)
			{
				isPolled[i] = true;
				for (size_t j = 0; j < isaClassNames.size(); ++j)
				{
					String key = makePollerMapKey(filterSourceNameSpace, isaClassNames[j]);
					BLOCXX_LOG_DEBUG3(m_logger, Format("searching m_pollers on key %1", key));
					poller_map_t::iterator iter = m_pollers.find(key);
					LifecycleIndicationPollerRef p;
					if (iter != m_pollers.end())
					{
						BLOCXX_LOG_DEBUG3(m_logger, Format("found poller for key %1: %2", key, iter->first));
						p = iter->second;
					}
					else
					{
						BLOCXX_LOG_DEBUG3(m_logger, Format("not found on key %1", key));
						p = LifecycleIndicationPollerRef(SharedLibraryRef(0),
							LifecycleIndicationPollerRef::element_type(new LifecycleIndicationPoller(filterSourceNameSpace, isaClassNames[j], pollInterval)));
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
						BLOCXX_LOG_DEBUG3(m_logger, Format("Inserting %1 into m_pollers", key));
						m_pollers.insert(std::make_pair(key, p));
						m_env->getPollingManager()->addPolledProvider(
							PolledProviderIFCRef(
								new CppPolledProviderProxy(
									CppPolledProviderIFCRef(p))));
					}
				}
			}

		}
		catch (Exception& e)
		{
			BLOCXX_LOG_ERROR(m_logger, Format("Caught exception while calling mustPoll for provider: %1", e));
		}
		catch(ThreadCancelledException&)
		{
			throw;
		}
		catch (...)
		{
			BLOCXX_LOG_ERROR(m_logger, "Caught unknown exception while calling mustPoll for provider");
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
			// Get the activation count for the indication class name
			bool firstActivation = (activateFilterOnProvider(providers[i]) == 1);
			providers[i]->activateFilter(createProvEnvRef(m_env),
				selectStmt, indicationClassName.toString(), ns, strIsaClassNames, firstActivation);

			++successfulActivations;
		}
		catch (Exception& e)
		{
			BLOCXX_LOG_ERROR(m_logger, Format("Caught exception while calling activateFilter for provider %1: %2", i, e));
		}
		catch(ThreadCancelledException&)
		{
			throw;
		}
		catch (...)
		{
			BLOCXX_LOG_ERROR(m_logger, Format("Caught unknown exception while calling activateFilter for provider %1", i));
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

	BLOCXX_LOG_DEBUG3(m_logger, "Successfully completed IndicationServerImplThread::createSubscription()");
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
IndicationServerImplThread::modifyFilter(OperationContext& context, const String& ns, const CIMInstance& filterInst, const String& userName)
{
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	// Implementation note: This depends on the fact that the indication subscription creation/deletion events are
	// processed sequentially (the thread pool only has 1 worker thread), so that the deletion is processed
	// before the creation.
	try
	{
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
IndicationServerImplThread::doShutdown()
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
