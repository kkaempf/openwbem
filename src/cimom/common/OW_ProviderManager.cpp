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

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
void ProviderManager::load(const ProviderIFCLoaderRef& IFCLoader)
{
	IFCLoader->loadIFCs(m_IFCArray);
	// now the CPP provider is linked to the cimom, not loaded dynamically, So
	// we have to create it here.
	Reference<ProviderIFCBaseIFC> cpppi(new CppProviderIFC);
	// 0 because there is no shared library.
	m_IFCArray.push_back(ProviderIFCBaseIFCRef(SharedLibraryRef(0), cpppi));
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
		env->getLogger()->logError(Format("More than one provider is registered to instrument class (%1). %2::%3 and %4::%5",
			name, ci->second.ifc->getName(), ci->second.provName, ifc->getName(), providerName));
		return;
	}
	env->getLogger()->logDebug(Format("Registering provider %1::%2 for %3", ifc->getName(), providerName, name));
	// now save it so we can look it up quickly when needed
	ProviderManager::ProvReg reg;
	reg.ifc = ifc;
	reg.provName = providerName;
	regMap.insert(std::make_pair(name, reg));
}
//////////////////////////////////////////////////////////////////////////////
// Overloaded of the map type.  Indication registrations use a multi-map, since
// multiple indication providers can register for the same class.
void registerProviderInfo(
	const ProviderEnvironmentIFCRef& env,
	const String& name_,
	const ProviderIFCBaseIFCRef& ifc,
	const String& providerName,
	ProviderManager::MultiProvRegMap_t& regMap)
{
	String name(name_);
	name.toLowerCase();
	env->getLogger()->logDebug(Format("Registering provider %1::%2 for %3", ifc->getName(), providerName, name));
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
				env->getLogger()->logError(Format("Provider sub-name is "
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
	{
		registerProviderInfo(env, name, ifc, providerName, regMap);
	}
	if (!extra.empty())
	{
		for (size_t i = 0; i < extra.size(); ++i)
		{
			String extraName = extra[i];
			if (extraName.empty())
			{
				env->getLogger()->logError(Format("Provider sub-name is "
					"empty for %1 by provider %2::%3",
					name, ifc->getName(), providerName));
				continue;
			}
			registerProviderInfo(env, name + "/" + extraName, ifc, providerName, regMap);
		}
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
				env->getLogger()->logError(Format("Provider namespace is "
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
				env->getLogger()->logError(Format("Provider namespace is "
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
				env->getLogger()->logError(Format("Provider namespace is "
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
			env->getLogger()->logError(Format("Provider name not supplied for provider class registrations from IFC %1", ifc->getName()));
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
void ProviderManager::init(const ProviderEnvironmentIFCRef& env)
{
	for (size_t i = 0; i < m_IFCArray.size(); ++i)
	{
		InstanceProviderInfoArray instanceProviderInfo;
		SecondaryInstanceProviderInfoArray secondaryInstanceProviderInfo;

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray associatorProviderInfo;
#endif

		MethodProviderInfoArray methodProviderInfo;
		IndicationProviderInfoArray indicationProviderInfo;

		m_IFCArray[i]->init(env, 
			instanceProviderInfo,
			secondaryInstanceProviderInfo,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			associatorProviderInfo,
#endif
			methodProviderInfo,
			indicationProviderInfo);

		processProviderInfo(env, instanceProviderInfo, m_IFCArray[i], m_registeredInstProvs);
		processProviderInfo(env, secondaryInstanceProviderInfo, m_IFCArray[i], m_registeredSecInstProvs);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		processProviderInfo(env, associatorProviderInfo, m_IFCArray[i], m_registeredAssocProvs);
#endif

		processProviderInfo(env, methodProviderInfo, m_IFCArray[i], m_registeredMethProvs);
		processProviderInfo(env, indicationProviderInfo, m_IFCArray[i], m_registeredIndProvs);
	}
}

namespace
{

#ifdef OW_SETUID_PROVIDERS

inline InstanceProviderIFCRef
wrapProvider(InstanceProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
		return InstanceProviderIFCRef(new InstanceProviderProxy(pref, env));
}
inline SecondaryInstanceProviderIFCRef
wrapProvider(SecondaryInstanceProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
		return SecondaryInstanceProviderIFCRef(new SecondaryInstanceProviderProxy(pref, env));
}
inline MethodProviderIFCRef
wrapProvider(MethodProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	return MethodProviderIFCRef(new MethodProviderProxy(pref, env));
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
inline AssociatorProviderIFCRef
wrapProvider(AssociatorProviderIFCRef pref,
	const ProviderEnvironmentIFCRef& env)
{
	return AssociatorProviderIFCRef(new AssociatorProviderProxy(pref, env));
}
#endif

#else	// OW_SETUID_PROVIDERS

inline InstanceProviderIFCRef
wrapProvider(InstanceProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	(void)env;
	return pref;
}
inline SecondaryInstanceProviderIFCRef
wrapProvider(SecondaryInstanceProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	(void)env;
	return pref;
}
inline MethodProviderIFCRef
wrapProvider(MethodProviderIFCRef pref, const ProviderEnvironmentIFCRef& env)
{
	(void)env;
	return pref;
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
inline AssociatorProviderIFCRef
wrapProvider(AssociatorProviderIFCRef pref,
	const ProviderEnvironmentIFCRef& env)
{
	(void)env;
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
	// lookup just the class name to see if a provider registered for the
	// class in all namespaces.
	ProvRegMap_t::const_iterator ci = m_registeredInstProvs.find(cc.getName().toLowerCase());
	if (ci != m_registeredInstProvs.end())
	{
		return wrapProvider(ci->second.ifc->getInstanceProvider(env,
			ci->second.provName.c_str()), env);
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
		if(theIFC)
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
	const String& ns, const String& className) const
{
	String lowerName = className;
	lowerName.toLowerCase();
	MultiProvRegMap_t::const_iterator lci;
	MultiProvRegMap_t::const_iterator uci;
	// lookup just the class name to see if a provider registered for the
	// class in all namespaces.
	std::pair<MultiProvRegMap_t::const_iterator, MultiProvRegMap_t::const_iterator>
		range = m_registeredSecInstProvs.equal_range(lowerName);
	lci = range.first;
	uci = range.second;
	if (lci == m_registeredSecInstProvs.end())
	{
		// didn't find any, so
		// next lookup namespace:classname to see if we've got one for the
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
	String methodName = method.getName();
	// lookup just the class name to see if a provider registered for the
	// class in all namespaces.
	ProvRegMap_t::const_iterator ci = m_registeredMethProvs.find(cc.getName().toLowerCase());
	if (ci != m_registeredMethProvs.end())
	{
		return wrapProvider(ci->second.ifc->getMethodProvider(env,
				ci->second.provName.c_str()), env);
	}
	// next lookup classname/methodname to see if we've got one for the
	// specific class/method for any namespace
	String classAndMethodName = cc.getName() + '/' + methodName;
	classAndMethodName.toLowerCase();
	ci = m_registeredMethProvs.find(classAndMethodName);
	if (ci != m_registeredMethProvs.end())
	{
		return wrapProvider(ci->second.ifc->getMethodProvider(env,
				ci->second.provName.c_str()), env);
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
	String name = ns + ':' + cc.getName() + '/' + methodName;
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
		if(theIFC)
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
		if(theIFC)
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
	// lookup just the class name to see if a provider registered for the
	// class in all namespaces.
	ProvRegMap_t::const_iterator ci = m_registeredAssocProvs.find(cc.getName().toLowerCase());
	if (ci != m_registeredAssocProvs.end())
	{
		return wrapProvider(ci->second.ifc->getAssociatorProvider(env,
				ci->second.provName.c_str()), env);
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
		if(theIFC)
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
	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		IndicationExportProviderIFCRefArray pra =
				m_IFCArray[i]->getIndicationExportProviders(env);
		if(pra.size() > 0)
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
	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		PolledProviderIFCRefArray pra =
				m_IFCArray[i]->getPolledProviders(env);
		if(pra.size() > 0)
		{
			rv.appendArray(pra);
		}
	}
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
IndicationProviderIFCRefArray
ProviderManager::getIndicationProviders(const ProviderEnvironmentIFCRef& env,
	const String& ns, const String& indicationClassName,
	const String& monitoredClassName) const
{
	String lowerName = indicationClassName;
	lowerName.toLowerCase();
	MultiProvRegMap_t::const_iterator lci;
	MultiProvRegMap_t::const_iterator uci;
	if (monitoredClassName.empty())
	{
		// lookup just the class name to see if a provider registered for the
		// class in all namespaces.
		std::pair<MultiProvRegMap_t::const_iterator, MultiProvRegMap_t::const_iterator>
			range = m_registeredIndProvs.equal_range(lowerName);
		lci = range.first;
		uci = range.second;
		if (lci == m_registeredIndProvs.end())
		{
			// didn't find any, so
			// next lookup namespace:classname to see if we've got one for the
			// specific namespace
			String nsAndClassName = ns + ':' + lowerName;
			nsAndClassName.toLowerCase();
			range = m_registeredIndProvs.equal_range(nsAndClassName);
			lci = range.first;
			uci = range.second;
		}
	}
	else
	{
		// lookup indicationClassName/monitoredClassName
		String nsAndClassName = lowerName + '/' + monitoredClassName;
		nsAndClassName.toLowerCase();
		std::pair<MultiProvRegMap_t::const_iterator, MultiProvRegMap_t::const_iterator>
			range = m_registeredIndProvs.equal_range(nsAndClassName);
		lci = range.first;
		uci = range.second;
		if (lci == m_registeredIndProvs.end())
		{
			// didn't find any, so
			// next lookup namespace:indicationClassName/monitoredClassName to see if we've got one for the
			// specific namespace
			String nsAndClassName = ns + ':' + lowerName + '/' + monitoredClassName;
			nsAndClassName.toLowerCase();
			range = m_registeredIndProvs.equal_range(nsAndClassName);
			lci = range.first;
			uci = range.second;
		}
	}
	IndicationProviderIFCRefArray rval;
	if (lci != m_registeredIndProvs.end())
	{
		// loop through the matching range and put them in rval
		for (MultiProvRegMap_t::const_iterator tci = lci; tci != uci; ++tci)
		{
			rval.push_back(tci->second.ifc->getIndicationProvider(env, tci->second.provName.c_str()));
		}
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
ProviderManager::unloadProviders(const ProviderEnvironmentIFCRef& env)
{
	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		try
		{
			m_IFCArray[i]->unloadProviders(env);
		}
		catch (const Exception& e)
		{
			env->getLogger()->logError(Format("Caught exception while calling unloadProviders for provider interface %1: %2", m_IFCArray[i]->getName(), e));
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
	if(!qual.getName().equalsIgnoreCase(CIMQualifier::CIM_QUAL_PROVIDER))
	{
		env->getLogger()->logError(Format("Provider Manager - invalid provider qualifier: %1",
			qual.getName()));
		return rref;
	}
	CIMValue cv = qual.getValue();
	if(cv.getType() != CIMDataType::STRING || cv.isArray())
	{
		CIMDataType dt(cv.getType());
		if(cv.isArray())
		{
			dt.setToArrayType(cv.getArraySize());
		}
		env->getLogger()->logError(Format(
			"Provider Manager - qualifier has incompatible data type: %1",
			dt.toString()));
		return rref;
	}
	String qvstr;
	cv.get(qvstr);
	size_t ndxoffset = 2;
	size_t ndx = qvstr.indexOf("::");
	if(ndx == String::npos)
	{
		ndx = qvstr.indexOf(":");
		if (ndx == String::npos)
		{
			env->getLogger()->logError(Format(
				"Provider Manager - Invalid Format for provider string: %1", qvstr));
			return rref;
		}
		ndxoffset = 1;
	}
	String ifcStr = qvstr.substring(0, ndx);
	provStr = qvstr.substring(ndx+ndxoffset);
	for (size_t i = 0; i < m_IFCArray.size(); i++)
	{
		if(ifcStr.equalsIgnoreCase(m_IFCArray[i]->getName()))
		{
			rref = m_IFCArray[i];
			break;
		}
	}
	if(rref.isNull())
	{
		env->getLogger()->logError(Format(
			"Provider Manager - Invalid provider interface identifier: %1",
			ifcStr));
	}
	return rref;
}

} // end namespace OpenWBEM

