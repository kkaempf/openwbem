/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
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
#include "OW_ProviderManager.hpp"
#include "OW_Format.hpp"
#include "OW_ProxyProvider.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CppProviderIFC.hpp"

//////////////////////////////////////////////////////////////////////////////
void OW_ProviderManager::load(const OW_ProviderIFCLoaderRef& IFCLoader)
{
	IFCLoader->loadIFCs(m_IFCArray);
	OW_Reference<OW_ProviderIFCBaseIFC> pi(new OW_InternalProviderIFC);
	m_internalIFC = pi.cast_to<OW_InternalProviderIFC>();
	// 0 because there is no shared library.
	m_IFCArray.push_back(OW_ProviderIFCBaseIFCRef(OW_SharedLibraryRef(0), pi));

	// now the CPP provider is linked to the cimom, not loaded dynamically, So
	// we have to create it here.
	OW_Reference<OW_ProviderIFCBaseIFC> cpppi(new OW_CppProviderIFC);
	// 0 because there is no shared library.
	m_IFCArray.push_back(OW_ProviderIFCBaseIFCRef(OW_SharedLibraryRef(0), cpppi));

}

namespace {

//////////////////////////////////////////////////////////////////////////////
void registerProviderInfo(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& name_,
	const OW_ProviderIFCBaseIFCRef& ifc,
	const OW_String& providerName,
	OW_ProviderManager::ProvRegMap_t& regMap)
{
	OW_String name(name_);
	name.toLowerCase();
	// search for duplicates
	OW_ProviderManager::ProvRegMap_t::const_iterator ci = regMap.find(name);
	if (ci != regMap.end())
	{
		env->getLogger()->logError(format("More than one provider is registered to instrument class (%1). %2::%3 and %4::%5",
			name, ci->second.ifc->getName(), ci->second.provName, ifc->getName(), providerName));
		return;
	}
	env->getLogger()->logDebug(format("Registering provider %1::%2 for %3", ifc->getName(), providerName, name));
	// now save it so we can look it up quickly when needed
	OW_ProviderManager::ProvReg reg;
	reg.ifc = ifc;
	reg.provName = providerName;
	regMap.insert(std::make_pair(name, reg));
}

//////////////////////////////////////////////////////////////////////////////
// Overloaded of the map type.  Indication registrations use a multi-map, since
// multiple indication providers can register for the same class.
void registerProviderInfo(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& name_,
	const OW_ProviderIFCBaseIFCRef& ifc,
	const OW_String& providerName,
	OW_ProviderManager::IndProvRegMap_t& regMap)
{
	OW_String name(name_);
	name.toLowerCase();
	env->getLogger()->logDebug(format("Registering provider %1::%2 for %3", ifc->getName(), providerName, name));
	// now save it so we can look it up quickly when needed
	OW_ProviderManager::ProvReg reg;
	reg.ifc = ifc;
	reg.provName = providerName;
	regMap.insert(std::make_pair(name, reg));
}

//////////////////////////////////////////////////////////////////////////////
// This handles method & property names
void processProviderClassExtraInfo(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& name,
	const OW_StringArray& extra,
	const OW_ProviderIFCBaseIFCRef& ifc,
	const OW_String& providerName,
	OW_ProviderManager::ProvRegMap_t& regMap)
{
	if (extra.empty())
	{
		registerProviderInfo(env, name, ifc, providerName, regMap);
	}
	else
	{
		for (size_t i = 0; i < extra.size(); ++i)
		{
			OW_String extraName = extra[i];
			if (extraName.empty())
			{
				env->getLogger()->logError(format("Provider sub-name is "
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
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& name,
	const OW_StringArray& extra,
	const OW_ProviderIFCBaseIFCRef& ifc,
	const OW_String& providerName,
	OW_ProviderManager::IndProvRegMap_t& regMap)
{
    if (extra.empty())
    {
        registerProviderInfo(env, name, ifc, providerName, regMap);
    }
    else
    {
        for (size_t i = 0; i < extra.size(); ++i)
        {
            OW_String extraName = extra[i];
            if (extraName.empty())
            {
                env->getLogger()->logError(format("Provider sub-name is "
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
inline OW_String getClassName(const ClassInfoT& classInfo)
{
	return classInfo.className;
}

inline OW_String getClassName(const OW_IndicationProviderInfoEntry& classInfo)
{
	return classInfo.indicationName;
}

//////////////////////////////////////////////////////////////////////////////
template <typename RegMapT, typename ClassInfoT>
void processProviderClassInfo(
	const OW_ProviderEnvironmentIFCRef& env,
	const ClassInfoT& classInfo,
	const OW_ProviderIFCBaseIFCRef& ifc,
	const OW_String& providerName,
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
			OW_String ns = classInfo.namespaces[l];
			if (ns.empty())
			{
				env->getLogger()->logError(format("Provider namespace is "
					"empty for class %1 by provider %2::%3",
					getClassName(classInfo), ifc->getName(), providerName));
				continue;
			}
			OW_String name = ns + ":" + getClassName(classInfo);
			registerProviderInfo(env, name, ifc, providerName, regMap);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void processProviderClassInfo(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_MethodProviderInfo::ClassInfo& classInfo,
	const OW_ProviderIFCBaseIFCRef& ifc,
	const OW_String& providerName,
	OW_ProviderManager::ProvRegMap_t& regMap)
{
	if (classInfo.namespaces.empty())
	{
		processProviderClassExtraInfo(env, classInfo.className, classInfo.methods, ifc, providerName, regMap);
	}
	else
	{
		for (size_t l = 0; l < classInfo.namespaces.size(); ++l)
		{
			OW_String ns = classInfo.namespaces[l];
			if (ns.empty())
			{
				env->getLogger()->logError(format("Provider namespace is "
					"empty for class %1 by provider %2::%3",
					classInfo.className, ifc->getName(), providerName));
				continue;
			}
			OW_String name = ns + ":" + classInfo.className;
			processProviderClassExtraInfo(env, name, classInfo.methods, ifc, providerName, regMap);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
void processProviderClassInfo(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_IndicationProviderInfo::ClassInfo& classInfo,
	const OW_ProviderIFCBaseIFCRef& ifc,
	const OW_String& providerName,
	OW_ProviderManager::IndProvRegMap_t& regMap)
{
	if (classInfo.namespaces.empty())
	{
		processProviderClassExtraInfo(env, classInfo.indicationName, classInfo.classes, ifc, providerName, regMap);
	}
	else
	{
		for (size_t l = 0; l < classInfo.namespaces.size(); ++l)
		{
			OW_String ns = classInfo.namespaces[l];
			if (ns.empty())
			{
				env->getLogger()->logError(format("Provider namespace is "
					"empty for class %1 by provider %2::%3",
					classInfo.indicationName, ifc->getName(), providerName));
				continue;
			}
			OW_String name = ns + ":" + classInfo.indicationName;
			processProviderClassExtraInfo(env, name, classInfo.classes, ifc, providerName, regMap);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
template <typename ProviderInfoT, typename RegMapT>
void processProviderInfo(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_Array<ProviderInfoT>& providerInfo,
	const OW_ProviderIFCBaseIFCRef& ifc,
	RegMapT& regMap)
{
	// process the provider info.  Each provider may have added an entry.
	for (size_t j = 0; j < providerInfo.size(); ++j)
	{
		// make sure the ifc or provider set a name.
		OW_String providerName = providerInfo[j].getProviderName();
		if (providerName.empty())
		{
			env->getLogger()->logError(format("Provider name not supplied for provider class registrations from IFC %1", ifc->getName()));
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
void OW_ProviderManager::init(const OW_ProviderEnvironmentIFCRef& env)
{
	for (size_t i = 0; i < m_IFCArray.size(); ++i)
	{
		OW_InstanceProviderInfoArray instanceProviderInfo;
		OW_AssociatorProviderInfoArray associatorProviderInfo;
		OW_MethodProviderInfoArray methodProviderInfo;
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
		OW_PropertyProviderInfoArray propertyProviderInfo;
#endif
		OW_IndicationProviderInfoArray indicationProviderInfo;
		m_IFCArray[i]->init(env, instanceProviderInfo, associatorProviderInfo, 
			methodProviderInfo, 
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
			propertyProviderInfo, 
#endif
			indicationProviderInfo);

		processProviderInfo(env, instanceProviderInfo, m_IFCArray[i], m_registeredInstProvs);
		processProviderInfo(env, associatorProviderInfo, m_IFCArray[i], m_registeredAssocProvs);
		processProviderInfo(env, methodProviderInfo, m_IFCArray[i], m_registeredMethProvs);
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
		processProviderInfo(env, propertyProviderInfo, m_IFCArray[i], m_registeredPropProvs);
#endif
		processProviderInfo(env, indicationProviderInfo, m_IFCArray[i], m_registeredIndProvs);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::addCIMOMProvider(const OW_CppProviderBaseIFCRef& pProv)
{
	m_internalIFC->addCIMOMProvider(pProv);
	// TODO: Need to add the provider's info the registry if init has already been run.
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::addCIMOMProvider(const OW_String& providerNameArg,
	const OW_CppProviderBaseIFCRef& pProv)
{
	m_internalIFC->addCIMOMProvider(providerNameArg, pProv);
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_ProviderManager::getInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& ns, const OW_CIMClass& cc) const
{
	// lookup just the class name to see if a provider registered for the
	// class in all namespaces.
	ProvRegMap_t::const_iterator ci = m_registeredInstProvs.find(cc.getName().toLowerCase());
	if (ci != m_registeredInstProvs.end())
	{
		return ci->second.ifc->getInstanceProvider(env, ci->second.provName.c_str());
	}

	// next lookup namespace:classname to see if we've got one for the
	// specific namespace
	OW_String nsAndClassName = ns + ':' + cc.getName();
	nsAndClassName.toLowerCase();
	ci = m_registeredInstProvs.find(nsAndClassName);
	if (ci != m_registeredInstProvs.end())
	{
		return ci->second.ifc->getInstanceProvider(env, ci->second.provName.c_str());
	}

	// if we don't have a new registration, try the old method
	OW_CIMQualifier qual = cc.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		OW_String provStr;
		OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if(theIFC)
		{
			return theIFC->getInstanceProvider(env, provStr.c_str());
		}
	}
	return OW_InstanceProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef
OW_ProviderManager::getMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& ns, const OW_CIMClass& cc, const OW_CIMMethod& method) const
{
	OW_String methodName = method.getName();

	// lookup just the class name to see if a provider registered for the
	// class in all namespaces.
	ProvRegMap_t::const_iterator ci = m_registeredMethProvs.find(cc.getName().toLowerCase());
	if (ci != m_registeredMethProvs.end())
	{
		return ci->second.ifc->getMethodProvider(env, ci->second.provName.c_str());
	}

	// next lookup classname/methodname to see if we've got one for the
	// specific class/method for any namespace
	OW_String classAndMethodName = cc.getName() + '/' + methodName;
	classAndMethodName.toLowerCase();
	ci = m_registeredMethProvs.find(classAndMethodName);
	if (ci != m_registeredMethProvs.end())
	{
		return ci->second.ifc->getMethodProvider(env, ci->second.provName.c_str());
	}

	// next lookup namespace:classname to see if we've got one for the
	// specific namespace/class & all methods
	OW_String nsAndClassName = ns + ':' + cc.getName();
	nsAndClassName.toLowerCase();
	ci = m_registeredMethProvs.find(nsAndClassName);
	if (ci != m_registeredMethProvs.end())
	{
		return ci->second.ifc->getMethodProvider(env, ci->second.provName.c_str());
	}

	// next lookup namespace:classname/methodname to see if we've got one for the
	// specific namespace/class/method
	OW_String name = ns + ':' + cc.getName() + '/' + methodName;
	name.toLowerCase();
	ci = m_registeredMethProvs.find(name);
	if (ci != m_registeredMethProvs.end())
	{
		return ci->second.ifc->getMethodProvider(env, ci->second.provName.c_str());
	}

	// didn't find it, so try the old way by looking at the provider qualifier.
	OW_CIMQualifier qual = method.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		OW_String provStr;
		OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if(theIFC)
		{
			return theIFC->getMethodProvider(env, provStr.c_str());
		}
	}

	// no method provider qualifier for the method, see if the class level
	// provider qualifier is a method provider
	qual = cc.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		OW_String provStr;
		OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if(theIFC)
		{
			return theIFC->getMethodProvider(env, provStr.c_str());
		}
	}

	return OW_MethodProviderIFCRef(0);
}

#ifdef OW_ENABLE_PROPERTY_PROVIDERS
//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_ProviderManager::getPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& ns, const OW_CIMClass& cc, const OW_CIMProperty& property) const
{
	OW_String propertyName = property.getName();

	// next lookup classname/propertyname to see if we've got one for the
	// specific class/property for any namespace
	OW_String classAndPropertyName = cc.getName() + '/' + propertyName;
	classAndPropertyName.toLowerCase();
	ProvRegMap_t::const_iterator ci = m_registeredPropProvs.find(classAndPropertyName);
	if (ci != m_registeredPropProvs.end())
	{
		return ci->second.ifc->getPropertyProvider(env, ci->second.provName.c_str());
	}

	// next lookup namespace:classname/propertyname to see if we've got one for the
	// specific namespace/class/property
	OW_String name = ns + ':' + cc.getName() + '/' + propertyName;
	name.toLowerCase();
	ci = m_registeredPropProvs.find(name);
	if (ci != m_registeredPropProvs.end())
	{
		return ci->second.ifc->getPropertyProvider(env, ci->second.provName.c_str());
	}

	// didn't find it, so try the old way by looking at the provider qualifier.
	OW_CIMQualifier qual = property.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		OW_String provStr;
		OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if(theIFC)
		{
			return theIFC->getPropertyProvider(env, provStr.c_str());
		}
	}

	return OW_PropertyProviderIFCRef(0);
}
#endif

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_ProviderManager::getAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& ns, const OW_CIMClass& cc) const
{
	// lookup just the class name to see if a provider registered for the
	// class in all namespaces.
	ProvRegMap_t::const_iterator ci = m_registeredAssocProvs.find(cc.getName().toLowerCase());
	if (ci != m_registeredAssocProvs.end())
	{
		return ci->second.ifc->getAssociatorProvider(env, ci->second.provName.c_str());
	}

	// next lookup namespace:classname to see if we've got one for the
	// specific namespace
	OW_String nsAndClassName = ns + ':' + cc.getName();
	nsAndClassName.toLowerCase();
	ci = m_registeredAssocProvs.find(nsAndClassName);
	if (ci != m_registeredAssocProvs.end())
	{
		return ci->second.ifc->getAssociatorProvider(env, ci->second.provName.c_str());
	}

	// if we don't have a new registration, try the old method
	OW_CIMQualifier qual = cc.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);
	if (qual)
	{
		OW_String provStr;
		OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
		if(theIFC)
		{
			return theIFC->getAssociatorProvider(env, provStr.c_str());
		}
	}
	return OW_AssociatorProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray
OW_ProviderManager::getIndicationExportProviders(
	const OW_ProviderEnvironmentIFCRef& env) const
{
	OW_IndicationExportProviderIFCRefArray rv;

	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		OW_IndicationExportProviderIFCRefArray pra =
				m_IFCArray[i]->getIndicationExportProviders(env);
		if(pra.size() > 0)
		{
			rv.appendArray(pra);
		}
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_PolledProviderIFCRefArray
OW_ProviderManager::getPolledProviders(
	const OW_ProviderEnvironmentIFCRef& env) const
{
	OW_PolledProviderIFCRefArray rv;

	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		OW_PolledProviderIFCRefArray pra =
				m_IFCArray[i]->getPolledProviders(env);

		if(pra.size() > 0)
		{
			rv.appendArray(pra);
		}
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationProviderIFCRefArray
OW_ProviderManager::getIndicationProviders(const OW_ProviderEnvironmentIFCRef& env, 
	const OW_String& ns, const OW_String& indicationClassName, 
	const OW_String& monitoredClassName) const
{
	OW_String lowerName = indicationClassName;
	lowerName.toLowerCase();
	IndProvRegMap_t::const_iterator lci;
	IndProvRegMap_t::const_iterator uci;
	if (monitoredClassName.empty())
	{
		// lookup just the class name to see if a provider registered for the
		// class in all namespaces.
		std::pair<IndProvRegMap_t::const_iterator, IndProvRegMap_t::const_iterator>
			range = m_registeredIndProvs.equal_range(lowerName);
		lci = range.first;
		uci = range.second;
		if (lci == m_registeredIndProvs.end())
		{
			// didn't find any, so
			// next lookup namespace:classname to see if we've got one for the
			// specific namespace
			OW_String nsAndClassName = ns + ':' + lowerName;
			nsAndClassName.toLowerCase();
			range = m_registeredIndProvs.equal_range(nsAndClassName);
			lci = range.first;
			uci = range.second;
		}

	}
	else
	{
		// lookup indicationClassName/monitoredClassName
		OW_String nsAndClassName = lowerName + '/' + monitoredClassName;
		nsAndClassName.toLowerCase();
		std::pair<IndProvRegMap_t::const_iterator, IndProvRegMap_t::const_iterator>
			range = m_registeredIndProvs.equal_range(nsAndClassName);
		lci = range.first;
		uci = range.second;

		if (lci == m_registeredIndProvs.end())
		{
			// didn't find any, so
			// next lookup namespace:indicationClassName/monitoredClassName to see if we've got one for the
			// specific namespace
			OW_String nsAndClassName = ns + ':' + lowerName + '/' + monitoredClassName;
			nsAndClassName.toLowerCase();
			range = m_registeredIndProvs.equal_range(nsAndClassName);
			lci = range.first;
			uci = range.second;
		}
	}

	OW_IndicationProviderIFCRefArray rval;
	if (lci != m_registeredIndProvs.end())
	{
		// loop through the matching range and put them in rval
		for (IndProvRegMap_t::const_iterator tci = lci; tci != uci; ++tci)
		{
			rval.push_back(tci->second.ifc->getIndicationProvider(env, tci->second.provName.c_str()));
		}
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::unloadProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		try
		{
			m_IFCArray[i]->unloadProviders(env);
		}
		catch (const OW_Exception& e)
		{
			env->getLogger()->logError(format("Caught exception while calling unloadProviders for provider interface %1: %2", m_IFCArray[i]->getName(), e));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_ProviderIFCBaseIFCRef
OW_ProviderManager::getProviderIFC(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual,
	OW_String& provStr) const
{
	OW_ProviderIFCBaseIFCRef rref;

	provStr = OW_String();
	if(!qual.getName().equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_PROVIDER))
	{
		env->getLogger()->logError(format("Provider Manager - invalid provider qualifier: %1",
			qual.getName()));
		return rref;
	}

	OW_CIMValue cv = qual.getValue();
	if(cv.getType() != OW_CIMDataType::STRING || cv.isArray())
	{
		OW_CIMDataType dt(cv.getType());
		if(cv.isArray())
		{
			dt.setToArrayType(cv.getArraySize());
		}

		env->getLogger()->logError(format(
			"Provider Manager - qualifier has incompatible data type: %1",
			dt.toString()));

		return rref;
	}

	OW_String qvstr;
	cv.get(qvstr);
	int ndx = qvstr.indexOf("::");
	if(ndx == -1)
	{
		env->getLogger()->logError(format(
			"Provider Manager - Invalid format for provider string: %1", qvstr));
		return rref;
	}

	OW_String ifcStr = qvstr.substring(0, ndx);
	provStr = qvstr.substring(ndx+2);

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
		env->getLogger()->logError(format(
			"Provider Manager - Invalid provider interface identifier: %1",
			ifcStr));
	}

	return rref;
}



