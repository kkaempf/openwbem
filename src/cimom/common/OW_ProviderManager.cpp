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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ProviderManager.hpp"
#include "OW_ProviderProxies.hpp"
#include "OW_Format.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CppProviderIFC.hpp"
#include "OW_OperationContext.hpp"
#include "OW_Platform.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_ServiceIFCNames.hpp"
#include "OW_ConfigOpts.hpp"

namespace OW_NAMESPACE
{

String ProviderManager::COMPONENT_NAME("ow.owcimomd.ProviderManager");

//////////////////////////////////////////////////////////////////////////////
String
ProviderManager::getName() const
{
	return ServiceIFCNames::ProviderManager;
}
//////////////////////////////////////////////////////////////////////////////
void ProviderManager::load(const ProviderIFCLoaderRef& IFCLoader, const ServiceEnvironmentIFCRef& env)
{
	IFCLoader->loadIFCs(m_IFCArray);

	// the config can disable this.
	if (env->getConfigItem(ConfigOpts::DISABLE_CPP_PROVIDER_INTERFACE_opt, OW_DEFAULT_DISABLE_CPP_PROVIDER_INTERFACE) != "true")
	{
		// now the CPP provider is linked to the cimom, not loaded dynamically, So
		// we have to create it here.
		ProviderIFCBaseIFCRef::element_type cpppi(new CppProviderIFC);
		// 0 because there is no shared library.
		m_IFCArray.push_back(ProviderIFCBaseIFCRef(SharedLibraryRef(0), cpppi));
	}
}

//////////////////////////////////////////////////////////////////////////////
void ProviderManager::shutdown()
{
	MutexLock lock(m_guard);
	
	m_registeredInstProvs.clear();
	m_registeredSecInstProvs.clear();
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	m_registeredAssocProvs.clear();
#endif
	m_registeredMethProvs.clear();
	m_registeredPropProvs.clear();
	m_registeredIndProvs.clear();
	m_IFCArray.clear();

	m_env = 0;
}

namespace
{
//////////////////////////////////////////////////////////////////////////////
class ProviderEnvironmentServiceEnvironmentWrapper : public ProviderEnvironmentIFC
{
public:
	ProviderEnvironmentServiceEnvironmentWrapper(ServiceEnvironmentIFCRef env_)
		: env(env_)
		, m_context()
	{}
	virtual CIMOMHandleIFCRef getCIMOMHandle() const
	{
		return env->getCIMOMHandle(m_context);
	}
	
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
	{
		return env->getCIMOMHandle(m_context, ServiceEnvironmentIFC::E_BYPASS_PROVIDERS);
	}
	
	virtual RepositoryIFCRef getRepository() const
	{
		return env->getRepository();
	}
	virtual LoggerRef getLogger() const
	{
		return env->getLogger();
	}
	virtual LoggerRef getLogger(const String& componentName) const
	{
		return env->getLogger(componentName);
	}
	virtual String getConfigItem(const String &name, const String& defRetVal="") const
	{
		return env->getConfigItem(name, defRetVal);
	}
	virtual String getUserName() const
	{
		return Platform::getCurrentUserName();
	}
	virtual OperationContext& getOperationContext()
	{
		return m_context;
	}
	virtual ProviderEnvironmentIFCRef clone() const
	{
		return ProviderEnvironmentIFCRef(new ProviderEnvironmentServiceEnvironmentWrapper(env));
	}
private:
	ServiceEnvironmentIFCRef env;
	mutable OperationContext m_context;
};

} // end anonymous namespace

/////////////////////////////////////////////////////////////////////////////
void ProviderManager::shuttingDown()
{
	ProviderEnvironmentIFCRef penv = ProviderEnvironmentIFCRef(
		new ProviderEnvironmentServiceEnvironmentWrapper(m_env));

	size_t const n = m_IFCArray.size();
	for (size_t i = 0; i < n; ++i)
	{
		m_IFCArray[i]->shuttingDown(penv);
	}
}

namespace {
//////////////////////////////////////////////////////////////////////////////
void registerProviderInfo(
	const ProviderEnvironmentIFCRef& env,
	const String& name_,
	const ProviderIFCBaseIFCRef& ifc,
	const String& providerName,
	ProviderManager::ProvRegMap_t& regMap)
{
	String name(name_);
	name.toLowerCase();
	// search for duplicates
	ProviderManager::ProvRegMap_t::const_iterator ci = regMap.find(name);
	if (ci != regMap.end())
	{
		OW_LOG_ERROR(env->getLogger(ProviderManager::COMPONENT_NAME), Format("More than one provider is registered to instrument class (%1). %2::%3 and %4::%5",
			name, ci->second.ifc->getName(), ci->second.provName, ifc->getName(), providerName));
		return;
	}
	OW_LOG_DEBUG(env->getLogger(ProviderManager::COMPONENT_NAME), Format("Registering provider %1::%2 for %3", ifc->getName(), providerName, name));
	// now save it so we can look it up quickly when needed
	ProviderManager::ProvReg reg;
	reg.ifc = ifc;
	reg.provName = providerName;
	regMap.insert(std::make_pair(name, reg));
}

//////////////////////////////////////////////////////////////////////////////
// Overloaded of the map type.  registrations use a multi-map, since
// multiple providers can register for the same class.  This is applicable
// for indication and secondary instance providers.
void registerProviderInfo(
	const ProviderEnvironmentIFCRef& env,
	const String& name_,
	const ProviderIFCBaseIFCRef& ifc,
	const String& providerName,
	ProviderManager::MultiProvRegMap_t& regMap)
{
	String name(name_);
	name.toLowerCase();
	OW_LOG_DEBUG(env->getLogger(ProviderManager::COMPONENT_NAME), Format("Registering provider %1::%2 for %3", ifc->getName(), providerName, name));
	// now save it so we can look it up quickly when needed
	ProviderManager::ProvReg reg;
	reg.ifc = ifc;
	reg.provName = providerName;
	regMap.insert(std::make_pair(name, reg));
}

//////////////////////////////////////////////////////////////////////////////
// This handles method & property names
void processProviderClassExtraInfo(
	const ProviderEnvironmentIFCRef& env,
	const String& name,
	const StringArray& extra,
	const ProviderIFCBaseIFCRef& ifc,
	const String& providerName,
	ProviderManager::ProvRegMap_t& regMap)
{
	if (extra.empty())
	{
		registerProviderInfo(env, name, ifc, providerName, regMap);
	}
	else
	{
		for (size_t i = 0; i < extra.size(); ++i)
		{
			String extraName = extra[i];
			if (extraName.empty())
			{
				OW_LOG_ERROR(env->getLogger(ProviderManager::COMPONENT_NAME), Format("Provider sub-name is "
					"empty for %1 by provider %2::%3",
					name, ifc->getName(), providerName));
				continue;
			}
			registerProviderInfo(env, name + "/" + extraName, ifc, providerName, regMap);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
// This handles indication class names
void processProviderClassExtraInfo(
	const ProviderEnvironmentIFCRef& env,
	const String& name,
	const StringArray& extra,
	const ProviderIFCBaseIFCRef& ifc,
	const String& providerName,
	ProviderManager::MultiProvRegMap_t& regMap)
{
	if (!extra.empty())
	{
		// The "/*" identifies the registration as a lifecycle provider.
		registerProviderInfo(env, name + "/*", ifc, providerName, regMap);
		for (size_t i = 0; i < extra.size(); ++i)
		{
			String extraName = extra[i];
			if (extraName.empty())
			{
				OW_LOG_ERROR(env->getLogger(ProviderManager::COMPONENT_NAME), Format("Provider sub-name is "
					"empty for %1 by provider %2::%3",
					name, ifc->getName(), providerName));
				continue;
			}
			registerProviderInfo(env, name + '/' + extraName, ifc, providerName, regMap);
		}
	}
	else
	{
		// The lack of "/*" identifies the registration as a non-lifecycle provider.
		registerProviderInfo(env, name, ifc, providerName, regMap);
	}
}
//////////////////////////////////////////////////////////////////////////////
template <typename ClassInfoT>
inline String getClassName(const ClassInfoT& classInfo)
{
	return classInfo.className;
}
inline String getClassName(const IndicationProviderInfoEntry& classInfo)
{
	return classInfo.indicationName;
}
//////////////////////////////////////////////////////////////////////////////
template <typename RegMapT, typename ClassInfoT>
void processProviderClassInfo(
	const ProviderEnvironmentIFCRef& env,
	const ClassInfoT& classInfo,
	const ProviderIFCBaseIFCRef& ifc,
	const String& providerName,
	RegMapT& regMap)
{
	if (classInfo.namespaces.empty())
	{
		registerProviderInfo(env, getClassName(classInfo), ifc, providerName, regMap);
	}
	else
	{
		for (size_t l = 0; l < classInfo.namespaces.size(); ++l)
		{
			String ns = classInfo.namespaces[l];
			if (ns.empty())
			{
				OW_LOG_ERROR(env->getLogger(ProviderManager::COMPONENT_NAME), Format("Provider namespace is "
					"empty for class %1 by provider %2::%3",
					getClassName(classInfo), ifc->getName(), providerName));
				continue;
			}
			String name = ns + ":" + getClassName(classInfo);
			registerProviderInfo(env, name, ifc, providerName, regMap);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void processProviderClassInfo(
	const ProviderEnvironmentIFCRef& env,
	const MethodProviderInfo::ClassInfo& classInfo,
	const ProviderIFCBaseIFCRef& ifc,
	const String& providerName,
	ProviderManager::ProvRegMap_t& regMap)
{
	if (classInfo.namespaces.empty())
	{
		processProviderClassExtraInfo(env, classInfo.className, classInfo.methods, ifc, providerName, regMap);
	}
	else
	{
		for (size_t l = 0; l < classInfo.namespaces.size(); ++l)
		{
			String ns = classInfo.namespaces[l];
			if (ns.empty())
			{
				OW_LOG_ERROR(env->getLogger(ProviderManager::COMPONENT_NAME), Format("Provider namespace is "
					"empty for class %1 by provider %2::%3",
					classInfo.className, ifc->getName(), providerName));
				continue;
			}
			String name = ns + ":" + classInfo.className;
			processProviderClassExtraInfo(env, name, classInfo.methods, ifc, providerName, regMap);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void processProviderClassInfo(
	const ProviderEnvironmentIFCRef& env,
	const IndicationProviderInfo::ClassInfo& classInfo,
	const ProviderIFCBaseIFCRef& ifc,
	const String& providerName,
	ProviderManager::MultiProvRegMap_t& regMap)
{
	if (classInfo.namespaces.empty())
	{
		processProviderClassExtraInfo(env, classInfo.indicationName, classInfo.classes, ifc, providerName, regMap);
	}
	else
	{
		for (size_t l = 0; l < classInfo.namespaces.size(); ++l)
		{
			String ns = classInfo.namespaces[l];
			if (ns.empty())
			{
				OW_LOG_ERROR(env->getLogger(ProviderManager::COMPONENT_NAME), Format("Provider namespace is "
					"empty for class %1 by provider %2::%3",
					classInfo.indicationName, ifc->getName(), providerName));
				continue;
			}
			String name = ns + ":" + classInfo.indicationName;
			processProviderClassExtraInfo(env, name, classInfo.classes, ifc, providerName, regMap);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
template <typename ProviderInfoT, typename RegMapT>
void processProviderInfo(
	const ProviderEnvironmentIFCRef& env,
	const Array<ProviderInfoT>& providerInfo,
	const ProviderIFCBaseIFCRef& ifc,
	RegMapT& regMap)
{
	// process the provider info.  Each provider may have added an entry.
	for (size_t j = 0; j < providerInfo.size(); ++j)
	{
		// make sure the ifc or provider set a name.
		String providerName = providerInfo[j].getProviderName();
		if (providerName.empty())
		{
			OW_LOG_ERROR(env->getLogger(ProviderManager::COMPONENT_NAME), Format(
				"Provider name not supplied for provider class registrations from IFC %1", ifc->getName()));
			continue;
		}
		// now loop through all the classes the provider may support
		typedef typename ProviderInfoT::ClassInfo ClassInfo;
		typedef typename ProviderInfoT::ClassInfoArray ClassInfoArray;
		ClassInfoArray const& classInfos = providerInfo[j].getClassInfo();
		for (size_t k = 0; k < classInfos.size(); ++k)
		{
			ClassInfo classInfo(classInfos[k]);
			processProviderClassInfo(env, classInfo, ifc, providerName, regMap);
		}
	}
}

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void ProviderManager::init(const ServiceEnvironmentIFCRef& env)
{
	m_env = env;

	m_logger = env->getLogger(COMPONENT_NAME);

	ProviderEnvironmentIFCRef penv = ProviderEnvironmentIFCRef(
		new ProviderEnvironmentServiceEnvironmentWrapper(env));

	for (size_t i = 0; i < m_IFCArray.size(); ++i)
	{
		InstanceProviderInfoArray instanceProviderInfo;
		SecondaryInstanceProviderInfoArray secondaryInstanceProviderInfo;

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray associatorProviderInfo;
#endif

		MethodProviderInfoArray methodProviderInfo;
		IndicationProviderInfoArray indicationProviderInfo;

		m_IFCArray[i]->init(penv,
			instanceProviderInfo,
			secondaryInstanceProviderInfo,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			associatorProviderInfo,
#endif
			methodProviderInfo,
			indicationProviderInfo);

		processProviderInfo(penv, instanceProviderInfo, m_IFCArray[i], m_registeredInstProvs);
		processProviderInfo(penv, secondaryInstanceProviderInfo, m_IFCArray[i], m_registeredSecInstProvs);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		processProviderInfo(penv, associatorProviderInfo, m_IFCArray[i], m_registeredAssocProvs);
#endif

		processProviderInfo(penv, methodProviderInfo, m_IFCArray[i], m_registeredMethProvs);
		processProviderInfo(penv, indicationProviderInfo, m_IFCArray[i], m_registeredIndProvs);
	}

	m_restrictedNamespaces.clear();

	String wk;
	String exns = m_env->getConfigItem(ConfigOpts::EXPLICIT_REG_NAMESPACES_opt);
	if(exns.length())
	{
		StringArray exnsra = exns.tokenize();
		for(size_t i = 0; i < exnsra.size(); i++)
		{
			wk.erase();
			StringArray toks = exnsra[i].tokenize("/\\");
			for(size_t j = 0; j < toks.size(); j++)
			{
				if(wk.empty())
				{
					wk = toks[j];
				}
				else
				{
					wk += '/';
					wk += toks[j];
				}
			}

			m_restrictedNamespaces.insert(wk.toLowerCase());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
bool
ProviderManager::isRestrictedNamespace(const String& ns) const
{
	String lns(ns);
	lns.toLowerCase();
	return (m_restrictedNamespaces.find(lns) != m_restrictedNamespaces.end());
}

namespace
{

#ifdef OW_THREADS_RUN_AS_USER

inline InstanceProviderIFCRef
wrapProvider(InstanceProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	if (!pref)
	{
		return pref;
	}
	return InstanceProviderIFCRef(new InstanceProviderProxy(pref, env));
}
inline SecondaryInstanceProviderIFCRef
wrapProvider(SecondaryInstanceProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	if (!pref)
	{
		return pref;
	}
	return SecondaryInstanceProviderIFCRef(new SecondaryInstanceProviderProxy(pref, env));
}
inline MethodProviderIFCRef
wrapProvider(MethodProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	if (!pref)
	{
		return pref;
	}
	return MethodProviderIFCRef(new MethodProviderProxy(pref, env));
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
inline AssociatorProviderIFCRef
wrapProvider(AssociatorProviderIFCRef pref,
	const ProviderEnvironmentIFCRef& env)
{
	if (!pref)
	{
		return pref;
	}
	return AssociatorProviderIFCRef(new AssociatorProviderProxy(pref, env));
}
#endif

#else	// OW_SETUID_PROVIDERS

inline InstanceProviderIFCRef
wrapProvider(InstanceProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	return pref;
}
inline SecondaryInstanceProviderIFCRef
wrapProvider(SecondaryInstanceProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	return pref;
}
inline MethodProviderIFCRef
wrapProvider(MethodProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	return pref;
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
inline AssociatorProviderIFCRef
wrapProvider(AssociatorProviderIFCRef pref,
	const ProviderEnvironmentIFCRef& env)
{
	return pref;
}
#endif

#endif	// OW_SETUID_PROVIDERS

}

//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
ProviderManager::getInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const String& ns, const CIMClass& cc) const
{
	ProvRegMap_t::const_iterator ci;
	if(!isRestrictedNamespace(ns))
	{
		// lookup just the class name to see if a provider registered for the
		// class in all namespaces.
		ci = m_registeredInstProvs.find(cc.getName().toLowerCase());
		if (ci != m_registeredInstProvs.end())
		{
			return wrapProvider(ci->second.ifc->getInstanceProvider(env,
				ci->second.provName.c_str()), env);
		}
	}

	// next lookup namespace:classname to see if we've got one for the
	// specific namespace
	String nsAndClassName = ns + ':' + cc.getName();
	nsAndClassName.toLowerCase();
	ci = m_registeredInstProvs.find(nsAndClassName);
	if (ci != m_registeredInstProvs.end())
	{
		return wrapProvider(ci->second.ifc->getInstanceProvider(env,
				ci->second.provName.c_str()), env);
	}
	// if we don't have a new registration, try the old method
	CIMQualifier qual = cc.getQualifier(
		CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		String provStr;
		ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if (theIFC)
		{
			return wrapProvider(theIFC->getInstanceProvider(env,
				provStr.c_str()), env);
		}
	}
	return InstanceProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
SecondaryInstanceProviderIFCRefArray
ProviderManager::getSecondaryInstanceProviders(const ProviderEnvironmentIFCRef& env,
	const String& ns, const CIMName& className) const
{
	String lowerName = className.toString();
	lowerName.toLowerCase();
	MultiProvRegMap_t::const_iterator lci;
	MultiProvRegMap_t::const_iterator uci;

	std::pair<MultiProvRegMap_t::const_iterator, 
		MultiProvRegMap_t::const_iterator> range;

	lci =  m_registeredSecInstProvs.end();
	if(!isRestrictedNamespace(ns))
	{
		// lookup just the class name to see if a provider registered for the
		// class in all namespaces.
		range = m_registeredSecInstProvs.equal_range(lowerName);
		lci = range.first;
		uci = range.second;
	}

	if (lci == m_registeredSecInstProvs.end())
	{
		// lookup namespace:classname to see if we've got one for the
		// specific namespace
		String nsAndClassName = ns + ':' + lowerName;
		nsAndClassName.toLowerCase();
		range = m_registeredSecInstProvs.equal_range(nsAndClassName);
		lci = range.first;
		uci = range.second;
	}

	SecondaryInstanceProviderIFCRefArray rval;
	if (lci != m_registeredSecInstProvs.end())
	{
		// loop through the matching range and put them in rval
		for (MultiProvRegMap_t::const_iterator tci = lci; tci != uci; ++tci)
		{
			rval.push_back(wrapProvider(tci->second.ifc->getSecondaryInstanceProvider(env, tci->second.provName.c_str()), env));
		}
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
ProviderManager::getMethodProvider(const ProviderEnvironmentIFCRef& env,
	const String& ns, const CIMClass& cc, const CIMMethod& method) const
{
	ProvRegMap_t::const_iterator ci;
	CIMName methodName = method.getName();
	
	if(!isRestrictedNamespace(ns))
	{
		// lookup just the class name to see if a provider registered for the
		// class in all namespaces.
		ci = m_registeredMethProvs.find(cc.getName().toLowerCase());
		if (ci != m_registeredMethProvs.end())
		{
			return wrapProvider(ci->second.ifc->getMethodProvider(env,
					ci->second.provName.c_str()), env);
		}

		// next lookup classname/methodname to see if we've got one for the
		// specific class/method for any namespace
		String classAndMethodName = cc.getName() + '/' + methodName.toString();
		classAndMethodName.toLowerCase();
		ci = m_registeredMethProvs.find(classAndMethodName);
		if (ci != m_registeredMethProvs.end())
		{
			return wrapProvider(ci->second.ifc->getMethodProvider(env,
					ci->second.provName.c_str()), env);
		}
	}

	// next lookup namespace:classname to see if we've got one for the
	// specific namespace/class & all methods
	String nsAndClassName = ns + ':' + cc.getName();
	nsAndClassName.toLowerCase();
	ci = m_registeredMethProvs.find(nsAndClassName);
	if (ci != m_registeredMethProvs.end())
	{
		return wrapProvider(ci->second.ifc->getMethodProvider(env,
				ci->second.provName.c_str()), env);
	}
	// next lookup namespace:classname/methodname to see if we've got one for the
	// specific namespace/class/method
	String name = ns + ':' + cc.getName() + '/' + methodName.toString();
	name.toLowerCase();
	ci = m_registeredMethProvs.find(name);
	if (ci != m_registeredMethProvs.end())
	{
		return wrapProvider(ci->second.ifc->getMethodProvider(env,
			ci->second.provName.c_str()), env);
	}
	// didn't find it, so try the old way by looking at the provider qualifier.
	CIMQualifier qual = method.getQualifier(
		CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		String provStr;
		ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if (theIFC)
		{
			return wrapProvider(theIFC->getMethodProvider(env,
				provStr.c_str()), env);
		}
	}
	// no method provider qualifier for the method, see if the class level
	// provider qualifier is a method provider
	qual = cc.getQualifier(
		CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		String provStr;
		ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if (theIFC)
		{
			return wrapProvider(theIFC->getMethodProvider(env,
				provStr.c_str()), env);
		}
	}
	return MethodProviderIFCRef(0);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
ProviderManager::getAssociatorProvider(const ProviderEnvironmentIFCRef& env,
	const String& ns, const CIMClass& cc) const
{
	ProvRegMap_t::const_iterator ci;
	if(!isRestrictedNamespace(ns))
	{
		// lookup just the class name to see if a provider registered for the
		// class in all namespaces.
		ci = m_registeredAssocProvs.find(cc.getName().toLowerCase());
		if (ci != m_registeredAssocProvs.end())
		{
			return wrapProvider(ci->second.ifc->getAssociatorProvider(env,
					ci->second.provName.c_str()), env);
		}
	}

	// next lookup namespace:classname to see if we've got one for the
	// specific namespace
	String nsAndClassName = ns + ':' + cc.getName();
	nsAndClassName.toLowerCase();
	ci = m_registeredAssocProvs.find(nsAndClassName);
	if (ci != m_registeredAssocProvs.end())
	{
		return wrapProvider(ci->second.ifc->getAssociatorProvider(env,
				ci->second.provName.c_str()), env);
	}
	// if we don't have a new registration, try the old method
	CIMQualifier qual = cc.getQualifier(
		CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		String provStr;
		ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if (theIFC)
		{
			return wrapProvider(theIFC->getAssociatorProvider(env,
				provStr.c_str()), env);
		}
	}
	return AssociatorProviderIFCRef(0);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
ProviderManager::getIndicationExportProviders(
	const ProviderEnvironmentIFCRef& env) const
{
	IndicationExportProviderIFCRefArray rv;
	for (size_t i = 0; i < m_IFCArray.size(); i++)
	{
		IndicationExportProviderIFCRefArray pra =
				m_IFCArray[i]->getIndicationExportProviders(env);
		if (pra.size() > 0)
		{
			rv.appendArray(pra);
		}
	}
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
PolledProviderIFCRefArray
ProviderManager::getPolledProviders(
	const ProviderEnvironmentIFCRef& env) const
{
	PolledProviderIFCRefArray rv;
	for (size_t i = 0; i < m_IFCArray.size(); i++)
	{
		PolledProviderIFCRefArray pra =
				m_IFCArray[i]->getPolledProviders(env);
		if (pra.size() > 0)
		{
			rv.appendArray(pra);
		}
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderManager::findIndicationProviders(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMName& className,
	const ProviderManager::MultiProvRegMap_t& indProvs,
	IndicationProviderIFCRefArray& rval) const
{
	typedef ProviderManager::MultiProvRegMap_t::const_iterator citer_t;
	std::pair<citer_t, citer_t> range;

	range.first = indProvs.end();

	if(!isRestrictedNamespace(ns))
	{
		range = indProvs.equal_range(className.toString());
	}

	if (range.first == indProvs.end())
	{
		// didn't find any or restricted namespace, so
		// next lookup Namespace:className to see if we've got one for the
		// specific Namespace
		String nsAndClassName = ns + ':' + className.toString();
		nsAndClassName.toLowerCase();
		range = indProvs.equal_range(nsAndClassName);
	}

	if (range.first != indProvs.end())
	{
		// loop through the matching range and put them in rval
		for (citer_t tci = range.first; tci != range.second; ++tci)
		{
			rval.push_back(tci->second.ifc->getIndicationProvider(env, tci->second.provName.c_str()));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
IndicationProviderIFCRefArray
ProviderManager::getIndicationProviders(const ProviderEnvironmentIFCRef& env,
	const String& ns, const CIMName& indicationClassName,
	const CIMNameArray& monitoredClassNames) const
{
	IndicationProviderIFCRefArray providers;
	String lowerName = indicationClassName.toString();
	lowerName.toLowerCase();

	// first get all the non-lifecycle indication providers
	findIndicationProviders(env, ns, lowerName, m_registeredIndProvs, providers);

	if (monitoredClassNames.empty())
	{
		// find any lifecycle indication providers which may match indicationClassName
		findIndicationProviders(env, ns, lowerName + "/*", m_registeredIndProvs, providers);
	}
	else
	{
		// find all the desired lifecycle indication providers
		for (size_t i = 0; i < monitoredClassNames.size(); ++i)
		{
			// lookup indicationClassName/monitoredClassName
			String key = lowerName + '/' + monitoredClassNames[i].toString();
			key.toLowerCase();
			findIndicationProviders(env, ns, key, m_registeredIndProvs, providers);
		}

	}

	// get rid of duplicate providers - unique() requires that the range be sorted
	std::sort(providers.begin(), providers.end());
	providers.erase(std::unique(providers.begin(), providers.end()), providers.end());

	return providers;
}
//////////////////////////////////////////////////////////////////////////////
void
ProviderManager::unloadProviders(const ProviderEnvironmentIFCRef& env)
{
	for (size_t i = 0; i < m_IFCArray.size(); i++)
	{
		try
		{
			m_IFCArray[i]->unloadProviders(env);
		}
		catch (const Exception& e)
		{
			OW_LOG_ERROR(m_logger, Format("Caught exception while calling unloadProviders for provider interface %1: %2", m_IFCArray[i]->getName(), e));
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
ProviderIFCBaseIFCRef
ProviderManager::getProviderIFC(const ProviderEnvironmentIFCRef& env,
	const CIMQualifier& qual,
	String& provStr) const
{
	ProviderIFCBaseIFCRef rref;
	provStr = String();
	if (!qual.getName().equalsIgnoreCase(CIMQualifier::CIM_QUAL_PROVIDER))
	{
		OW_LOG_ERROR(m_logger, Format("Provider Manager - invalid provider qualifier: %1",
			qual.getName()));
		return rref;
	}
	CIMValue cv = qual.getValue();
	if (cv.getType() != CIMDataType::STRING || cv.isArray())
	{
		CIMDataType dt(cv.getType());
		if (cv.isArray())
		{
			dt.setToArrayType(cv.getArraySize());
		}
		OW_LOG_ERROR(m_logger, Format(
			"Provider Manager - qualifier has incompatible data type: %1",
			dt.toString()));
		return rref;
	}
	String qvstr;
	cv.get(qvstr);
	size_t ndxoffset = 2;
	size_t ndx = qvstr.indexOf("::");
	if (ndx == String::npos)
	{
		ndx = qvstr.indexOf(":");
		if (ndx == String::npos)
		{
			OW_LOG_ERROR(m_logger, Format(
				"Provider Manager - Invalid Format for provider string: %1", qvstr));
			return rref;
		}
		ndxoffset = 1;
	}
	String ifcStr = qvstr.substring(0, ndx);
	provStr = qvstr.substring(ndx+ndxoffset);
	for (size_t i = 0; i < m_IFCArray.size(); i++)
	{
		if (ifcStr.equalsIgnoreCase(m_IFCArray[i]->getName()))
		{
			rref = m_IFCArray[i];
			break;
		}
	}
	if (!rref)
	{
		OW_LOG_ERROR(m_logger, Format(
			"Provider Manager - Invalid provider interface identifier: %1",
			ifcStr));
	}
	return rref;
}

} // end namespace OW_NAMESPACE

