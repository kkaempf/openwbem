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
#include "OW_ProviderManager.hpp"
#include "OW_Format.hpp"
#include "OW_ProxyProvider.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMClass.hpp"

//////////////////////////////////////////////////////////////////////////////
void OW_ProviderManager::load(const OW_ProviderIFCLoaderRef& IFCLoader)
{
	IFCLoader->loadIFCs(m_IFCArray);
	OW_Reference<OW_ProviderIFCBaseIFC> pi(new OW_InternalProviderIFC);
	m_internalIFC = pi.cast_to<OW_InternalProviderIFC>();
	// 0 because there is no shared library.
	m_IFCArray.push_back(OW_ProviderIFCBaseIFCRef(OW_SharedLibraryRef(0), pi));
}

//////////////////////////////////////////////////////////////////////////////
void OW_ProviderManager::init(const OW_ProviderEnvironmentIFCRef& env)
{
	for (size_t i = 0; i < m_IFCArray.size(); ++i)
	{
		OW_InstanceProviderInfoArray instanceProviderInfo;
		OW_AssociatorProviderInfoArray associatorProviderInfo;
		m_IFCArray[i]->init(env, instanceProviderInfo, associatorProviderInfo);
		for (size_t j = 0; j < instanceProviderInfo.size(); ++j)
		{
			OW_InstanceProviderInfo::ClassInfoArray classInfos = instanceProviderInfo[j].getClassInfo();
			OW_String providerName = instanceProviderInfo[j].getProviderName();
			if (providerName.empty())
			{
				env->getLogger()->logError(format("Provider name not supplied for instance provider class registrations from IFC %1", m_IFCArray[i]->getName()));
				continue;
			}
			for (size_t k = 0; k < classInfos.size(); ++k)
			{
				OW_String className = classInfos[k].className;
				// search for duplicates
				InstProvRegMap_t::const_iterator ci = m_registeredInstProvs.find(className);
				if (ci != m_registeredInstProvs.end())
				{
					env->getLogger()->logError(format("More than one instance provider is registered to instrument class (%1). %2::%3 and %4::%5",
						className, ci->second.ifc->getName(), ci->second.provName, m_IFCArray[i]->getName(), instanceProviderInfo[j].getProviderName()));
					continue;
				}
				InstProvReg reg;
				reg.ifc = m_IFCArray[i];
				reg.provName = providerName;
				m_registeredInstProvs.insert(std::make_pair(className, reg));
			}
		}
		for (size_t j = 0; j < associatorProviderInfo.size(); ++j)
		{
			OW_AssociatorProviderInfo::ClassInfoArray classInfos = associatorProviderInfo[j].getClassInfo();
			OW_String providerName = associatorProviderInfo[j].getProviderName();
			if (providerName.empty())
			{
				env->getLogger()->logError(format("Provider name not supplied for associator provider class registrations from IFC %1", m_IFCArray[i]->getName()));
				continue;
			}
			for (size_t k = 0; k < classInfos.size(); ++k)
			{
				OW_String className = classInfos[k].className;
				// search for duplicates
				AssocProvRegMap_t::const_iterator ci = m_registeredAssocProvs.find(className);
				if (ci != m_registeredAssocProvs.end())
				{
					env->getLogger()->logError(format("More than one associator provider is registered to instrument class (%1). %2::%3 and %4::%5",
						className, ci->second.ifc->getName(), ci->second.provName, m_IFCArray[i]->getName(), associatorProviderInfo[j].getProviderName()));
					continue;
				}
				AssocProvReg reg;
				reg.ifc = m_IFCArray[i];
				reg.provName = associatorProviderInfo[j].getProviderName();
				m_registeredAssocProvs.insert(std::make_pair(className, reg));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::addCIMOMProvider(const OW_CppProviderBaseIFCRef& pProv)
{
	m_internalIFC->addCIMOMProvider(pProv);
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
	InstProvRegMap_t::const_iterator ci = m_registeredInstProvs.find(cc.getName());
	if (ci != m_registeredInstProvs.end())
	{
		return ci->second.ifc->getInstanceProvider(env, ci->second.provName.c_str());
	}

	// next lookup namespace:classname to see if we've got one for the
	// specific namespace
	OW_String nsAndClassName = ns + ':' + cc.getName();
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
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC)
	{
		return theIFC->getMethodProvider(env, provStr.c_str());
	}
	return OW_MethodProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_ProviderManager::getPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC)
	{
		return theIFC->getPropertyProvider(env, provStr.c_str());
	}

	return OW_PropertyProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_ProviderManager::getAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& ns, const OW_CIMClass& cc) const
{
	// lookup just the class name to see if a provider registered for the 
	// class in all namespaces.
	AssocProvRegMap_t::const_iterator ci = m_registeredAssocProvs.find(cc.getName());
	if (ci != m_registeredAssocProvs.end())
	{
		return ci->second.ifc->getAssociatorProvider(env, ci->second.provName.c_str());
	}

	// next lookup namespace:classname to see if we've got one for the
	// specific namespace
	OW_String nsAndClassName = ns + ':' + cc.getName();
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
OW_ProviderManager::getIndicationExportProviders(const OW_ProviderEnvironmentIFCRef& env
	)
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
OW_ProviderManager::getPolledProviders(const OW_ProviderEnvironmentIFCRef& env
	)
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



