/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*	this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*	this list of conditions and the following disclaimer in the documentation
*	and/or other materials provided with the distribution.
*
*  - Neither the name of Center 7 nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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
#include "OW_DateTime.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_UserInfo.hpp"
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

//////////////////////////////////////////////////////////////////////////////
OW_IndicationServer::~OW_IndicationServer() 
{
}

//////////////////////////////////////////////////////////////////////////////
struct OW_NotifyTrans
{
	OW_NotifyTrans() : m_indication(OW_CIMNULL), m_handler(OW_CIMNULL), m_provider(0) {}

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
	OW_Notifier(OW_IndicationServerImpl* pmgr, OW_NotifyTrans& ntrans, OW_UserInfo const& aclInfo) :
		m_pmgr(pmgr), m_trans(ntrans), m_aclInfo(aclInfo) {}

	void start();

	virtual void run();

private:
	OW_IndicationServerImpl* m_pmgr;
	OW_NotifyTrans m_trans;
	OW_UserInfo m_aclInfo;
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

	virtual OW_RepositoryIFCRef getRepository() const
	{
		return m_env->getRepository();
	}

	virtual OW_String getConfigItem(const OW_String& name, const OW_String& defRetVal="") const
	{
		return m_env->getConfigItem(name, defRetVal);
	}
	
	virtual OW_LoggerRef getLogger() const
	{
		return m_env->getLogger();
	}

	virtual OW_String getUserName() const
	{
		return OW_Platform::getCurrentUserName();
	}

private:
	OW_CIMOMHandleIFCRef m_ch;
	OW_CIMOMEnvironmentRef m_env;
	OW_CIMOMHandleIFCRef m_repch;
};

OW_ProviderEnvironmentIFCRef createProvEnvRef(OW_CIMOMEnvironmentRef env)
{
	return OW_ProviderEnvironmentIFCRef(
		new IndicationServerProviderEnvironment(
			env->getCIMOMHandle(), env, env->getRepositoryCIMOMHandle()));
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

	while (true)
	{
		try
		{
			m_trans.m_provider->exportIndication(createProvEnvRef(
				m_pmgr->getEnvironment()), m_trans.m_ns, m_trans.m_handler,
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

namespace
{
//////////////////////////////////////////////////////////////////////////////
class instanceEnumerator : public OW_CIMInstanceResultHandlerIFC
{
public:
	instanceEnumerator(OW_IndicationServerImpl* is_,
		const OW_String& ns_)
		: is(is_)
		, ns(ns_)
	{}

private:
	void doHandle(const OW_CIMInstance& i)
	{
		// try and get the name of whoever first created the subscription
		OW_String username;
		OW_CIMProperty p = i.getProperty("__OW_Subscription_UserName");
		if (p)
		{
			OW_CIMValue v = p.getValue();
			if (v)
			{
				username = v.toString();
			}
		}

		is->createSubscription(ns, i, username);
	}
	OW_IndicationServerImpl* is;
	OW_String ns;
};

//////////////////////////////////////////////////////////////////////////////
class namespaceEnumerator : public OW_StringResultHandlerIFC
{
public:
	namespaceEnumerator(
		const OW_CIMOMHandleIFCRef& ch_,
		OW_IndicationServerImpl* is_)
		: ch(ch_)
		, is(is_)
	{}

private:
	void doHandle(const OW_String& ns)
	{
		instanceEnumerator ie(is, ns);
		try
		{
			ch->enumInstances(ns,"CIM_IndicationSubscription", ie);
		}
		catch (const OW_CIMException& ce)
		{
			// do nothing, class probably doesn't exist in the namespace
		}
	}

	OW_CIMOMHandleIFCRef ch;
	OW_IndicationServerImpl* is;
};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::init(OW_CIMOMEnvironmentRef env)
{
	m_env = env;
	OW_UserInfo aclInfo;

	//-----------------
	// Load map with available indication export providers
	//-----------------
	OW_ProviderManagerRef pProvMgr = m_env->getProviderManager();

	OW_CIMOMHandleIFCRef lch = m_env->getCIMOMHandle(aclInfo, false);

	OW_IndicationExportProviderIFCRefArray pra =
		pProvMgr->getIndicationExportProviders(createProvEnvRef(m_env));

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

	// Now initialize for all the subscriptions that exist in the repository.
	// This calls createSubscription for every instance of 
	// CIM_IndicationSubscription in all namespaces.
	namespaceEnumerator nsHandler(lch, this);
	OW_CIMNameSpaceUtils::enum__Namespace(lch, "root", nsHandler);
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
		OW_NonRecursiveMutexLock l(m_mainLoopGuard);
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
			catch (const OW_Exception& e)
			{
				m_env->logError(format("OW_IndicationServerImpl::run caught "
					" exception %1", e));
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

	// Wait for OW_Notifier threads to complete any pending notifications.
	// We use a large timeout (10 mins.) because if this does timeout, we'll
	// probably cause a segfault, since the library will be unloaded soon after
	// and if a thread is still running, then BOOM!
	m_threadCounter->waitForAll(600, 0);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::shutdown()
{
	{
		OW_NonRecursiveMutexLock l(m_mainLoopGuard);
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
	OW_NonRecursiveMutexLock ml(m_mainLoopGuard);
	ProcIndicationTrans trans(instanceArg, instNS);
	m_procTrans.push_back(trans);
	m_mainLoopCondition.notifyOne();
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
void splitUpProps(const OW_StringArray& props, 
	OW_HashMap<OW_String, OW_StringArray>& map)
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
		OW_String prop = props[i];
		prop.toLowerCase();
		size_t idx = prop.indexOf('.');
		map[""].push_back(prop); // for no ClassName
		if (idx != OW_String::npos)
		{
			OW_String key = prop.substring(0, idx);
			OW_String val = prop.substring(idx+1);

			map[""].push_back(key); // Store PropertyName for PropertyName.EmbedName
			map[key].push_back(val); // Store PropertyName for ClassName.PropertyName and EmbedName for PropertyName.EmbedName

			// now remove trailing periods.
			idx = val.indexOf('.');
			if (idx != OW_String::npos)
			{
				val = val.substring(0, idx);
			}
			map[key].push_back(val); // Store PropertyName for ClassName.PropertyName.EmbedName
		}
	}
}

OW_CIMInstance filterInstance(const OW_CIMInstance& toFilter, const OW_StringArray& props)
{
	OW_CIMInstance rval(toFilter.clone(OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
		OW_CIMOMHandleIFC::EXCLUDE_QUALIFIERS, 
		OW_CIMOMHandleIFC::EXCLUDE_CLASS_ORIGIN));

	if (props.empty())
	{
		return rval;
	}

	OW_HashMap<OW_String, OW_StringArray> propMap;
	splitUpProps(props, propMap);

	// find "" and toFilter.getClassName() and keep those properties.
	OW_StringArray propsToKeepArray(propMap[""]);
	
	OW_String lowerClassName(toFilter.getClassName());
	lowerClassName.toLowerCase();
	propsToKeepArray.appendArray(propMap[lowerClassName]);

	// create a sorted set to get faster look-up time.
	OW_SortedVectorSet<OW_String> propsToKeep(propsToKeepArray.begin(), 
		propsToKeepArray.end());

	OW_CIMPropertyArray propArray = toFilter.getProperties();
	OW_CIMPropertyArray propArrayToKeep;
	for (size_t i = 0; i < propArray.size(); ++i)
	{
		OW_String lowerPropName(propArray[i].getName());
		lowerPropName.toLowerCase();
		if (propsToKeep.count(lowerPropName) > 0 || propsToKeep.count("*") > 0)
		{
			OW_CIMProperty thePropToKeep(propArray[i]);
			// if it's an embedded instance, we need to recurse on it.
			if (thePropToKeep.getDataType().getType() == OW_CIMDataType::EMBEDDEDINSTANCE)
			{
				OW_CIMValue v = thePropToKeep.getValue();
				if (v)
				{
					OW_CIMInstance embed;
					v.get(embed);
					if (embed)
					{
						OW_StringArray embeddedProps;
						for (size_t i = 0; i < propsToKeepArray.size(); ++i)
						{
							const OW_String& curPropName = propsToKeepArray[i];
							if (curPropName.startsWith(lowerPropName))
							{
								size_t idx = curPropName.indexOf('.');
								if (idx != OW_String::npos)
								{
									embeddedProps.push_back(curPropName.substring(idx));
								}
							}
						}
						thePropToKeep.setValue(OW_CIMValue(
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
OW_IndicationServerImpl::_processIndication(const OW_CIMInstance& instanceArg_,
	const OW_String& instNS)
{
	m_env->logDebug(format("OW_IndicationServerImpl::_processIndication "
		"instanceArg = %1 instNS = %2", instanceArg_.toString(), instNS));

	// If the provider didn't set the IndicationTime property, then we'll set it.
	OW_CIMInstance instanceArg(instanceArg_);
	if (!instanceArg.getProperty("IndicationTime"))
	{
		OW_DateTime dtm;
		dtm.setToCurrent();
		OW_CIMDateTime cdt(dtm);
		instanceArg.setProperty("IndicationTime", OW_CIMValue(cdt));
	}

	OW_WQLIFCRef wqlRef = m_env->getWQLRef();

	if (!wqlRef)
	{
		m_env->logError("Cannot process indications, because there is no "
			"WQL library.");
		return;
	}


	OW_String curClassName = instanceArg.getClassName();
	if (curClassName.empty())
	{
		m_env->logError("Cannot process indication, because it has no "
			"class name.");
	}
	while (!curClassName.empty())
	{
		OW_String key = curClassName;

		key.toLowerCase();

		{
			OW_MutexLock lock(m_subGuard);
			m_env->logDebug(format("searching for key %1", key));
			std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range = 
				m_subscriptions.equal_range(key);
			m_env->logDebug(format("found %1 items", distance(range.first, range.second)));
			
			// make a copy so we can free the lock, otherwise we may cause a deadlock.
			std::vector<subscriptions_t::value_type> subs(range.first, range.second);
			lock.release();

			_processIndicationRange(instanceArg, instNS, subs.begin(), subs.end());
		}

		OW_CIMProperty prop = instanceArg.getProperty("SourceInstance");
		if (prop)
		{
			OW_CIMValue v = prop.getValue();
			if (v && v.getType() == OW_CIMDataType::EMBEDDEDINSTANCE)
			{
				OW_CIMInstance embed;
				v.get(embed);
				key += ":";
				key += embed.getClassName();
				key.toLowerCase();

				{
					OW_MutexLock lock(m_subGuard);
					m_env->logDebug(format("searching for key %1", key));
					std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range = 
						m_subscriptions.equal_range(key);
					m_env->logDebug(format("found %1 items", distance(range.first, range.second)));
					
					// make a copy so we can free the lock, otherwise we may cause a deadlock.
					std::vector<subscriptions_t::value_type> subs(range.first, range.second);
					lock.release();

					_processIndicationRange(instanceArg, instNS, subs.begin(), subs.end());
				}
			}
		}

		OW_CIMClass cc;
		try
		{
			cc = m_env->getRepositoryCIMOMHandle()->getClass(instNS, curClassName);
			curClassName = cc.getSuperClass();
		}
		catch (const OW_CIMException& e)
		{
			curClassName.erase();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::_processIndicationRange(
	const OW_CIMInstance& instanceArg, const OW_String instNS,
	std::vector<subscriptions_t::value_type>::iterator first, std::vector<subscriptions_t::value_type>::iterator last)
{
	OW_UserInfo aclInfo;
	OW_CIMOMHandleIFCRef hdl = m_env->getCIMOMHandle(aclInfo, false);

	for( ;first != last; ++first)
	{
		try
		{
			Subscription& sub = first->second;
			OW_CIMInstance filterInst = sub.m_filter;

			OW_String queryLanguage = sub.m_filter.getPropertyT("QueryLanguage").getValueT().toString();

			if (!sub.m_filterSourceNameSpace.equalsIgnoreCase(instNS))
			{
				continue;
			}

			//-----------------------------------------------------------------
			// Here we need to call into the WQL process with the query string
			// and the indication instance
			//-----------------------------------------------------------------
			OW_WQLInstancePropertySource propSource(instanceArg, hdl, instNS);
			if (!sub.m_compiledStmt.evaluate(propSource))
			{
				continue;
			}

			OW_CIMInstance filteredInstance(filterInstance(instanceArg,
				sub.m_selectStmt.getSelectPropertyNames()));

			// Now get the export handler for this indication subscription
			OW_CIMObjectPath handlerCOP = 
				sub.m_subPath.getKeyT("Handler").getValueT().toCIMObjectPath();

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
		// this may look like a memory leak, but the start method will end
		// up deleting the thread once it's done running.
		OW_Notifier* pnotifier = new OW_Notifier(this, trans, OW_UserInfo());
		m_threadCounter->incThreadCount(0, 0); // This should never timeout, since we already checked in the above if statement
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
	provider_map_t::iterator it =
		m_providers.find(lowerClassName);

	if(it != m_providers.end())
	{
		pref = it->second;
	}

	return pref;
}


//////////////////////////////////////////////////////////////////////////////
void 
OW_IndicationServerImpl::deleteSubscription(const OW_String& ns, const OW_CIMObjectPath& subPath)
{
	OW_LoggerRef log = m_env->getLogger();
	log->logDebug(format("OW_IndicationServerImpl::deleteSubscription ns = %1, subPath = %2", ns, subPath.toString()));

	OW_CIMObjectPath cop(subPath);
	cop.setNameSpace(ns);
	log->logDebug(format("cop = %1", cop));
	
	OW_MutexLock l(m_subGuard);
	for (subscriptions_t::iterator iter = m_subscriptions.begin(); iter != m_subscriptions.end();)
	{
		log->logDebug(format("subPath = %1", iter->second.m_subPath));
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
							OW_String key = sub.m_classes[j];
							key.toLowerCase();
							poller_map_t::iterator iter = m_pollers.find(key);
							if (iter != m_pollers.end())
							{
								OW_LifecycleIndicationPollerRef p = iter->second;
								OW_String subClsName = sub.m_selectStmt.getClassName();
								bool removePoller = false;
								if (subClsName.equalsIgnoreCase("CIM_InstCreation"))
								{
									removePoller = p->removePollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
								}
								else if (subClsName.equalsIgnoreCase("CIM_InstModification"))
								{
									removePoller = p->removePollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
								}
								else if (subClsName.equalsIgnoreCase("CIM_InstDeletion"))
								{
									removePoller = p->removePollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
								}
								else if (subClsName.equalsIgnoreCase("CIM_InstIndication") || subClsName.equalsIgnoreCase("CIM_Indication"))
								{
									p->removePollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
									p->removePollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
									removePoller = p->removePollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
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
						OW_IndicationProviderIFCRef p = sub.m_providers[i];
						p->deActivateFilter(createProvEnvRef(m_env), sub.m_selectStmt, sub.m_selectStmt.getClassName(), ns, sub.m_classes);
					}
					
				}
				catch (const OW_Exception& e)
				{
				}
				catch (...)
				{
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
namespace
{

OW_String getSourceNameSpace(const OW_CIMInstance& inst)
{
	try
	{
		return inst.getPropertyT("SourceNamespace").getValueT().toString();
	}
	catch (const OW_NoSuchPropertyException& e)
	{
		return "";
	}
	catch (const OW_NULLValueException& e)
	{
		return "";
	}
}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::createSubscription(const OW_String& ns, const OW_CIMInstance& subInst, const OW_String& username)
{
	OW_LoggerRef log = m_env->getLogger();
	log->logDebug(format("OW_IndicationServerImpl::createSubscription ns = %1, subInst = %2", ns, subInst.toString()));

	// get the filter
	OW_CIMOMHandleIFCRef hdl = m_env->getRepositoryCIMOMHandle();
	OW_CIMObjectPath filterPath = subInst.getProperty("Filter").getValueT().toCIMObjectPath();
	OW_String filterNS = filterPath.getNameSpace();
	if (filterNS.empty())
	{
		filterNS = ns;
	}
	OW_CIMInstance filterInst = hdl->getInstance(filterNS, filterPath);
	OW_String filterQuery = filterInst.getPropertyT("Query").getValueT().toString();

	// parse the filter
	// Get query language
	OW_String queryLanguage = filterInst.getPropertyT("QueryLanguage").getValueT().toString();

	// get the wql library
	OW_WQLIFCRef wqlRef = m_env->getWQLRef();

	if (!wqlRef)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Cannot process indications, because there is no "
			"WQL library.");
	}

	if (!wqlRef->supportsQueryLanguage(queryLanguage))
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, format("Filter uses queryLanguage %1, which is"
			" not supported", queryLanguage).c_str());
	}

	OW_WQLSelectStatement selectStmt(wqlRef->createSelectStatement(filterQuery));
	OW_WQLCompile compiledStmt(selectStmt);
	OW_WQLCompile::Tableau& tableau(compiledStmt.getTableau());
	OW_String indicationClassName = selectStmt.getClassName();

	// collect up all the class names
	OW_StringArray isaClassNames;
	for (size_t i = 0; i < tableau.size(); ++i)
	{
		for (size_t j = 0; j < tableau[i].size(); ++j)
		{
			if (tableau[i][j].op == WQL_ISA)
			{
				OW_WQLOperand& opn1(tableau[i][j].opn1);
				OW_WQLOperand& opn2(tableau[i][j].opn2);
				if (opn1.getType() == OW_WQLOperand::PROPERTY_NAME && opn1.getPropertyName() == "SourceInstance")
				{
					if (opn2.getType() == OW_WQLOperand::PROPERTY_NAME)
					{
						isaClassNames.push_back(opn2.getPropertyName());
					}
					else if (opn2.getType() == OW_WQLOperand::STRING_VALUE)
					{
						isaClassNames.push_back(opn2.getStringValue());
					}
				}

			}
		}
	}

	// get rid of duplicates - unique() requires that the range be sorted
	std::sort(isaClassNames.begin(), isaClassNames.end());
	isaClassNames.erase(std::unique(isaClassNames.begin(), isaClassNames.end()), isaClassNames.end());


	// find providers that support this query. If none are found, throw an exception.
	OW_ProviderManagerRef pm (m_env->getProviderManager());
	OW_IndicationProviderIFCRefArray providers = 
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

	log->logDebug(format("Found %1 providers for the subscription", providers.size()));
	if (providers.empty())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "No indication provider found for this subscription");
	}


	// verify that there is an indication export provider that can handle the handler for the subscription
	OW_CIMObjectPath handlerPath = subInst.getProperty("Handler").getValueT().toCIMObjectPath();
	OW_String handlerClass = handlerPath.getObjectName();
	if (!getProvider(handlerClass))
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "No indication export provider found for this subscription");
	}

	// call authorizeFilter on all the indication providers
	for (size_t i = 0; i < providers.size(); ++i)
	{
		log->logDebug(format("Calling authorizeFilter for provider %1", i));
		providers[i]->authorizeFilter(createProvEnvRef(m_env),
			selectStmt, indicationClassName, ns, isaClassNames, username);
	}

	// Call mustPoll on all the providers
	OW_Array<bool> isPolled(providers.size(), false);
	for (size_t i = 0; i < providers.size(); ++i)
	{
		try
		{
			log->logDebug(format("Calling mustPoll for provider %1", i));
			int pollInterval = providers[i]->mustPoll(createProvEnvRef(m_env),
				selectStmt, indicationClassName, ns, isaClassNames);
			log->logDebug(format("got pollInterval %1", pollInterval));
			if (pollInterval > 0)
			{
				isPolled[i] = true;
				for (size_t j = 0; j < isaClassNames.size(); ++j)
				{
					OW_String key = isaClassNames[j];
					key.toLowerCase();
					log->logDebug(format("searching on class key %1", isaClassNames[j]));
					poller_map_t::iterator iter = m_pollers.find(key);
					OW_LifecycleIndicationPollerRef p;
					if (iter != m_pollers.end())
					{
						log->logDebug(format("found on class key %1: %2", isaClassNames[j], iter->first));
						p = iter->second;
					}
					else
					{
						log->logDebug(format("not found on class key %1", isaClassNames[j]));
						p = OW_LifecycleIndicationPollerRef(OW_SharedLibraryRef(0), 
							OW_Reference<OW_LifecycleIndicationPoller>(new OW_LifecycleIndicationPoller(ns, key, pollInterval)));

					}

					OW_String subClsName = selectStmt.getClassName();
					if (subClsName.equalsIgnoreCase("CIM_InstCreation"))
					{
						p->addPollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
					}
					else if (subClsName.equalsIgnoreCase("CIM_InstModification"))
					{
						p->addPollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
					}
					else if (subClsName.equalsIgnoreCase("CIM_InstDeletion"))
					{
						p->addPollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
					}
					else if (subClsName.equalsIgnoreCase("CIM_InstIndication") || subClsName.equalsIgnoreCase("CIM_Indication"))
					{
						p->addPollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_CREATION);
						p->addPollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_MODIFICATION);
						p->addPollOp(OW_LifecycleIndicationPoller::POLL_FOR_INSTANCE_DELETION);
					}
					p->addPollInterval(pollInterval);

					if (iter == m_pollers.end())
					{
						log->logDebug(format("Inserting %1 into m_pollers", key));
						m_pollers.insert(std::make_pair(key, p));
						m_env->getPollingManager()->addPolledProvider(
							OW_PolledProviderIFCRef(
								new OW_CppPolledProviderProxy(
									OW_CppPolledProviderIFCRef(p))));
					}
				}
			}
			
		}
		catch (OW_CIMException& ce)
		{
			m_env->getLogger()->logError(format("Caught exception while calling mustPoll for provider: %1", ce));
		}
		catch (...)
		{
			m_env->getLogger()->logError("Caught unknown exception while calling mustPoll for provider");
		}
	}

	// create a subscription (save the compiled filter and other info)
	Subscription sub;
	sub.m_subPath = OW_CIMObjectPath(ns, subInst);
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
	OW_String filterSourceNameSpace = getSourceNameSpace(filterInst);
	if (filterSourceNameSpace.empty())
	{
		filterSourceNameSpace = filterNS;
	}
	sub.m_filterSourceNameSpace = filterSourceNameSpace;

	// get the lock and put it in m_subscriptions
	{
		OW_MutexLock l(m_subGuard);
		if (isaClassNames.empty())
		{
			OW_String subKey = indicationClassName;
			subKey.toLowerCase();
			m_subscriptions.insert(std::make_pair(subKey, sub));
		}
		else
		{
			for (size_t i = 0; i < isaClassNames.size(); ++i)
			{
				OW_String subKey = indicationClassName + ':' + isaClassNames[i];
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
		catch (OW_CIMException& ce)
		{
			m_env->getLogger()->logError(format("Caught exception while calling activateFilter for provider: %1", ce));
		}
		catch (...)
		{
			m_env->getLogger()->logError("Caught unknown exception while calling activateFilter for provider");
		}
	}

	if (successfulActivations == 0)
	{
		// Remove it and throw
		OW_MutexLock l(m_subGuard);
		if (isaClassNames.empty())
		{
			OW_String subKey = indicationClassName;
			subKey.toLowerCase();
			m_subscriptions.erase(subKey);
		}
		else
		{
			for (size_t i = 0; i < isaClassNames.size(); ++i)
			{
				OW_String subKey = indicationClassName + ':' + isaClassNames[i];
				subKey.toLowerCase();
				m_subscriptions.erase(subKey);
			}
		}
		OW_THROWCIMMSG(OW_CIMException::FAILED, "activateFilter failed for all providers");
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationServerImpl::modifySubscription(const OW_String& ns, const OW_CIMInstance& subInst)
{
	// since you can't modify an instance's path which includes the paths to
	// the filter and the handler, if a subscription was modified, it will
	// have only really changed the non-key, non-ref properties, so we can just
	// find it in the subscriptions map and update it.
	OW_CIMObjectPath cop(subInst);
	cop.setNameSpace(ns);
	
	OW_MutexLock l(m_subGuard);
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
OW_IndicationServerImpl::modifyFilter(const OW_String& ns, const OW_CIMInstance& filterInst)
{
	(void)ns;(void)filterInst;
	// If this were to update the filters, it would be quite a bit of work.
	// Basically all the subscriptions that use the old filter would have to
	// be unregistered, and then re-registered with the new filter.  If any
	// of the providers doesn't support the new filter, what then?
	// So, it's just easiest to disallow filter modification.  If we wanted to
	// make this a little more friendly, we could allow modification as long
	// as there's not subscriptions associated to it.
	OW_THROWCIMMSG(OW_CIMException::FAILED, "modifying a filter is not supported");
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


