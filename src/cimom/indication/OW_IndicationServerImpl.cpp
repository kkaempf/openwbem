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
#include "OW_CIMValueCast.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_NULLValueException.hpp"

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
		is->createSubscription(ns, i);
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
		ch->enumInstances(ns,"CIM_IndicationSubscription", ie);
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
	OW_ACLInfo aclInfo;

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
	lch->enumNameSpace("", nsHandler);
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
// TODO: Put this into it's own file?
class InstancePropertySource : public OW_WQLPropertySource
{
public:
	InstancePropertySource(const OW_CIMInstance& ci_,
		const OW_CIMOMHandleIFCRef& hdl,
		const OW_String& ns)
		: ci(ci_)
		, m_hdl(hdl)
		, m_ns(ns)
	{
	}

	virtual bool evaluateISA(const OW_String &propertyName, const OW_String &className) const 
	{
		OW_StringArray propNames = propertyName.tokenize(".");
		if (propNames.empty())
		{
			return false;
		}

		if (propNames[0] == ci.getClassName())
		{
			propNames.remove(0);
		}

		return evaluateISAAux(ci, propNames, className);
	}

	virtual bool getValue(const OW_String &propertyName, OW_WQLOperand &value) const 
	{
		OW_StringArray propNames = propertyName.tokenize(".");
		if (propNames.empty())
		{
			return false;
		}

		if (propNames[0] == ci.getClassName())
		{
			propNames.remove(0);
		}

		return getValueAux(ci, propNames, value);
	}

private:
	// This is for recursion on embedded instances
	bool evaluateISAAux(const OW_CIMInstance& ci, OW_StringArray propNames, const OW_String &className) const 
	{
		if (propNames.empty())
		{
			return classIsDerivedFrom(ci.getClassName(), className);
		}

		OW_CIMProperty p = ci.getProperty(propNames[0]);
		if (!p)
		{
			return false;
		}

		OW_CIMValue v = p.getValue();
		switch (v.getType())
		{
			case OW_CIMDataType::EMBEDDEDINSTANCE:
			{
				propNames.remove(0);
				OW_CIMInstance embed;
				v.get(embed);
				if (!embed)
				{
					return false;
				}
				return evaluateISAAux(embed, propNames, className);
			}
			default:
				return false;
		}
	}

	bool classIsDerivedFrom(const OW_String& cls, const OW_String& className) const
	{
		OW_String curClassName = cls;
		while (!curClassName.empty())
		{
			if (curClassName.equalsIgnoreCase(className))
			{
				return true;
			}
			// didn't match, so try the superclass of curClassName
			OW_CIMClass cls2 = m_hdl->getClass(m_ns, curClassName);
			curClassName = cls2.getSuperClass();
	
		}
		return false;
	}

	// This is for recursion on embedded instances
	static bool getValueAux(const OW_CIMInstance& ci, OW_StringArray propNames, OW_WQLOperand& value)
	{
		if (propNames.empty())
		{
			return false;
		}

		OW_CIMProperty p = ci.getProperty(propNames[0]);
		if (!p)
		{
			return false;
		}

		OW_CIMValue v = p.getValue();
		switch (v.getType())
		{
			case OW_CIMDataType::DATETIME:
			case OW_CIMDataType::CIMNULL:
				value = OW_WQLOperand();
				break;
			case OW_CIMDataType::UINT8:
			case OW_CIMDataType::SINT8:
			case OW_CIMDataType::UINT16:
			case OW_CIMDataType::SINT16:
			case OW_CIMDataType::UINT32:
			case OW_CIMDataType::SINT32:
			case OW_CIMDataType::UINT64:
			case OW_CIMDataType::SINT64:
			case OW_CIMDataType::CHAR16:
			{
				OW_Int64 x;
				OW_CIMValueCast::castValueToDataType(v, OW_CIMDataType::SINT64).get(x);
				value = OW_WQLOperand(x, WQL_INTEGER_VALUE_TAG);
				break;
			}
			case OW_CIMDataType::STRING:
				value = OW_WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
				break;
			case OW_CIMDataType::BOOLEAN:
			{
				OW_Bool b;
				v.get(b);
				value = OW_WQLOperand(b, WQL_BOOLEAN_VALUE_TAG);
				break;
			}
			case OW_CIMDataType::REAL32:
			case OW_CIMDataType::REAL64:
			{
				OW_Real64 x;
				OW_CIMValueCast::castValueToDataType(v, OW_CIMDataType::REAL64).get(x);
				value = OW_WQLOperand(x, WQL_DOUBLE_VALUE_TAG);
				break;
			}
			case OW_CIMDataType::REFERENCE:
				value = OW_WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
				break;
			case OW_CIMDataType::EMBEDDEDCLASS:
				value = OW_WQLOperand();
				break;
			case OW_CIMDataType::EMBEDDEDINSTANCE:
			{
				propNames.remove(0);
				OW_CIMInstance embed;
				v.get(embed);
				if (!embed)
				{
					return false;
				}
				return getValueAux(embed, propNames, value);
			}
			break;
			default:
				value = OW_WQLOperand();
				break;
		}

		return true;
	}

private:
	OW_CIMInstance ci;
	OW_CIMOMHandleIFCRef m_hdl;
	OW_String m_ns;
};

void splitUpProps(const OW_StringArray& props, 
	OW_HashMap<OW_String, OW_StringArray>& map)
{
	for (size_t i = 0; i < props.size(); ++i)
	{
		OW_String prop = props[i];
		int idx = prop.indexOf('.');
		if (idx == -1)
		{
			map[""].push_back(prop);
		}
		else
		{
			OW_String key = prop.substring(0, idx);
			key.toLowerCase();
			OW_String val = prop.substring(idx+1);
			val.toLowerCase();
			map[key].push_back(val);
		}
	}
}

OW_CIMInstance filterInstance(const OW_CIMInstance& toFilter, const OW_StringArray& props)
{
	OW_CIMInstance rval(toFilter.clone(OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
		OW_CIMOMHandleIFC::EXCLUDE_QUALIFIERS, 
		OW_CIMOMHandleIFC::EXCLUDE_CLASS_ORIGIN));

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
		if (propsToKeep.count(lowerPropName) > 0)
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
						thePropToKeep.setValue(OW_CIMValue(
							filterInstance(embed, propsToKeepArray)));
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
OW_IndicationServerImpl::_processIndication(const OW_CIMInstance& instanceArg,
	const OW_String& instNS)
{
	m_env->logDebug(format("OW_IndicationServerImpl::_processIndication "
		"instanceArg = %1 instNS = %2", instanceArg.toString(), instNS));
	OW_ACLInfo aclInfo;

	// TODO: Figure out if we should really set the IndicationTime property
	// of if it's the provider's responsibility.
	//OW_CIMInstance instance(instanceArg);
	//OW_DateTime dtm;
	//dtm.setToCurrent();
	//OW_CIMDateTime cdt(dtm);
	//instance.setProperty("IndicationTime", OW_CIMValue(cdt));

	OW_CIMOMHandleIFCRef hdl = m_env->getCIMOMHandle(aclInfo, false);

	OW_WQLIFCRef wqlRef = m_env->getWQLRef();

	if (!wqlRef)
	{
		m_env->logError("Cannot process indications, because there is no "
			"WQL library.");
		return;
	}

	OW_String key = instanceArg.getClassName();
    OW_CIMProperty prop = instanceArg.getProperty("SourceInstance");
	if (prop)
	{
		OW_CIMValue v = prop.getValue();
		if (v)
		{
			if (v.getType() == OW_CIMDataType::EMBEDDEDINSTANCE)
			{
				OW_CIMInstance embed;
				v.get(embed);
				key += ":";
				key += embed.getClassName();
			}
		}
	}

	OW_MutexLock lock(m_subGuard);
	std::pair<subscriptions_t::iterator, subscriptions_t::iterator> range = 
		m_subscriptions.equal_range(key);
	subscriptions_t::iterator first = range.first;
	subscriptions_t::iterator last = range.second;

	for( ;first != last; ++first)
	{
		try
		{
			Subscription& sub = first->second;
			OW_CIMInstance filterInst = sub.m_filter;

			OW_String queryLanguage = sub.m_filter.getPropertyT("Filter").getValueT().toString();

			if (!sub.m_filterSourceNameSpace.equalsIgnoreCase(instNS))
			{
				continue;
			}

			//-----------------------------------------------------------------
			// Here we need to call into the WQL process with the query string
			// and the indication instance
			//-----------------------------------------------------------------
			InstancePropertySource propSource(instanceArg, hdl, instNS);
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
void 
OW_IndicationServerImpl::deleteSubscription(const OW_String& ns, const OW_CIMObjectPath& subPath)
{
	OW_CIMObjectPath cop(subPath);
	cop.setNameSpace(ns);
	
	OW_MutexLock l(m_subGuard);
	for (subscriptions_t::iterator iter = m_subscriptions.begin(); iter != m_subscriptions.end();)
	{
		if (cop.equals(iter->second.m_subPath))
		{
			Subscription& sub = iter->second;
			for (size_t i = 0; i < sub.m_providers.size(); ++i)
			{
				try
				{
					OW_IndicationProviderIFCRef p = sub.m_providers[i];
					bool lastActivation = false; // TODO: Figure this out
					p->deActivateFilter(createProvEnvRef(m_env), sub.m_selectStmt, sub.m_selectStmt.getClassName(), ns, sub.m_classes, lastActivation);
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
OW_IndicationServerImpl::createSubscription(const OW_String& ns, const OW_CIMInstance& subInst)
{
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
	OW_IndicationProviderIFCRefArray providers;
	if (isaClassNames.empty())
	{
		providers = pm->getIndicationProviders(createProvEnvRef(m_env), ns, 
			indicationClassName, "");
	}
	else
	{
		for (size_t i = 0; i < isaClassNames.size(); ++i)
		{
			providers.appendArray(pm->getIndicationProviders(createProvEnvRef(m_env), 
				ns, indicationClassName, isaClassNames[i]));
		}
	}

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
		providers[i]->authorizeFilter(createProvEnvRef(m_env),
			selectStmt, indicationClassName, ns, isaClassNames, ""); // TODO: figure out the user name
	}

	// create a subscription (save the compiled filter and other info)
	Subscription sub;
	sub.m_subPath = OW_CIMObjectPath(subInst);
	sub.m_sub = subInst;
	sub.m_providers = providers;
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
		bool firstActivation = true; // TODO: Figure out firstActivation
		try
		{
			providers[i]->activateFilter(createProvEnvRef(m_env),
				selectStmt, indicationClassName, ns, isaClassNames, firstActivation);
			
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


