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

//////////////////////////////////////////////////////////////////////////////
OW_ProviderManager::OW_ProviderManager() :
	OW_ProviderIFCBaseIFC(), m_IFCArray(), m_shlibArray(), m_cimomProviders(),
	m_guard(), m_noIdProviders()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_ProviderManager::~OW_ProviderManager()
{
	// Clean up providers identified by qualifiers
	ProviderMap::iterator it = m_cimomProviders.begin();
	while(it != m_cimomProviders.end())
	{
		it->second.m_pProv->cleanup();
		it->second.m_pProv = 0;
		it++;
	}

	m_cimomProviders.clear();

	// Clean up providers with no identification
	for(size_t i = 0; i < m_noIdProviders.size(); i++)
	{
		m_noIdProviders[i].m_pProv->cleanup();
		m_noIdProviders[i].m_pProv = 0;
	}

	m_noIdProviders.clear();

	// make sure the muxes are destroyed before the corresponding shared
	// libraries are, otherwise we'll segfault.
	m_IFCArray.clear();
	m_shlibArray.clear();
}

//////////////////////////////////////////////////////////////////////////////
void OW_ProviderManager::init(const OW_ProviderIFCLoaderRef IFCLoader)
{
	IFCLoader->loadIFCs(m_IFCArray, m_shlibArray);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::addCIMOMProvider(OW_CppProviderBaseIFCRef pProv)
{
	OW_ASSERT(pProv->isPolledProvider() || pProv->isIndicationExportProvider());

	OW_MutexLock ml(m_guard);

	CimProv cprov;
	cprov.m_pProv = pProv;
	m_noIdProviders.append(cprov);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::addCIMOMProvider(const OW_String& providerNameArg,
	OW_CppProviderBaseIFCRef pProv)
{
	OW_MutexLock ml(m_guard);

	OW_String providerName;
	int ndx = providerNameArg.indexOf("::");
	if(ndx != -1)
	{
		providerName = providerNameArg.substring(ndx+2);
	}
	else
	{
		providerName = providerNameArg;
	}

	CimProv cprov;
	cprov.m_pProv = pProv;
	m_cimomProviders[providerName] = cprov;
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray
OW_ProviderManager::doGetIndicationExportProviders(
	const OW_ProviderEnvironmentIFCRef& env)
{
	OW_MutexLock ml(m_guard);
	OW_IndicationExportProviderIFCRefArray rv;

	for(size_t i = 0; i < m_noIdProviders.size(); i++)
	{
		if (m_noIdProviders[i].m_pProv->isIndicationExportProvider())
		{
			OW_CppProviderBaseIFCRef pProv =
				m_noIdProviders[i].m_pProv;
			
			if(!m_noIdProviders[i].m_initDone)
			{
				pProv->initialize(env);
				m_noIdProviders[i].m_initDone = true;
			}

			rv.append(OW_IndicationExportProviderIFCRef(new
				OW_IndicationExportProviderProxy(pProv.cast_to<OW_CppIndicationExportProviderIFC>())));
		}
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_PolledProviderIFCRefArray
OW_ProviderManager::doGetPolledProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	OW_MutexLock ml(m_guard);
	OW_PolledProviderIFCRefArray rv;

	for(size_t i = 0; i < m_noIdProviders.size(); i++)
	{
		if(m_noIdProviders[i].m_pProv->isPolledProvider())
		{
			OW_CppProviderBaseIFCRef pProv = m_noIdProviders[i].m_pProv;
			if(!m_noIdProviders[i].m_initDone)
			{
				pProv->initialize(env);
				m_noIdProviders[i].m_initDone = true;
			}

			rv.append(OW_PolledProviderIFCRef(new
				OW_PolledProviderProxy(pProv.cast_to<OW_CppPolledProviderIFC>())));
		}
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_ProviderManager::getInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC.isNull())
	{
		return OW_InstanceProviderIFCRef(0);
	}

	return theIFC->getInstanceProvider(env, provStr.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef
OW_ProviderManager::getMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC.isNull())
	{
		return OW_MethodProviderIFCRef(0);
	}

	return theIFC->getMethodProvider(env, provStr.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_ProviderManager::getPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC.isNull())
	{
		return OW_PropertyProviderIFCRef(0);
	}

	return theIFC->getPropertyProvider(env, provStr.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_ProviderManager::getAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC.isNull())
	{
		return OW_AssociatorProviderIFCRef(0);
	}

	return theIFC->getAssociatorProvider(env, provStr.c_str());
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
OW_ProviderIFCBaseIFCRef
OW_ProviderManager::getProviderIFC(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual,
	OW_String& provStr) const
{
	OW_ProviderIFCBaseIFCRef rref(0);

	provStr = "";
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

	if(ifcStr.equalsIgnoreCase(OW_String(CIMOM_PROVIDER_IFC)))
	{
		return OW_ProviderIFCBaseIFCRef((OW_ProviderIFCBaseIFC*)this, true);
	}

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

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_ProviderManager::doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		return OW_InstanceProviderIFCRef(0);
	}

	if(!it->second.m_pProv->isInstanceProvider())
	{
		env->getLogger()->logError(format(
			"Provider Manager - not an instance provider: %1", provIdString));
		return OW_InstanceProviderIFCRef(0);
	}

	if(!it->second.m_initDone)
	{
		it->second.m_pProv->initialize(env);
		it->second.m_initDone = true;
	}

	return OW_InstanceProviderIFCRef(new OW_InstanceProviderProxy(
		it->second.m_pProv.cast_to<OW_CppInstanceProviderIFC>()));
}

//////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef
OW_ProviderManager::doGetMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		return OW_MethodProviderIFCRef(0);
	}

	if(!it->second.m_pProv->isMethodProvider())
	{
		env->getLogger()->logError(format(
			"Provider Manager - not a method provider: %1", provIdString));
		return OW_MethodProviderIFCRef(0);
	}

	if(!it->second.m_initDone)
	{
		it->second.m_pProv->initialize(env);
		it->second.m_initDone = true;
	}

	return OW_MethodProviderIFCRef(new OW_MethodProviderProxy(
		it->second.m_pProv.cast_to<OW_CppMethodProviderIFC>()));
}

//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_ProviderManager::doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		return OW_PropertyProviderIFCRef(0);
	}

	if(!it->second.m_pProv->isPropertyProvider())
	{
		env->getLogger()->logError(format(
			"Provider Manager - not a property provider: %1", provIdString));
		return OW_PropertyProviderIFCRef(0);
	}

	if(!it->second.m_initDone)
	{
		it->second.m_pProv->initialize(env);
		it->second.m_initDone = true;
	}

	return OW_PropertyProviderIFCRef(new OW_PropertyProviderProxy(
		it->second.m_pProv.cast_to<OW_CppPropertyProviderIFC>()));
}

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_ProviderManager::doGetAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		return OW_AssociatorProviderIFCRef(0);
	}

	if(!it->second.m_pProv->isAssociatorProvider())
	{
		env->getLogger()->logError(format(
			"Provider Manager - not an associator provider: %1", provIdString));

		return OW_AssociatorProviderIFCRef(0);
	}

	if(!it->second.m_initDone)
	{
		it->second.m_pProv->initialize(env);
		it->second.m_initDone = true;
	}

	return OW_AssociatorProviderIFCRef(new OW_AssociatorProviderProxy(
		it->second.m_pProv.cast_to<OW_CppAssociatorProviderIFC>()));
}

const char* const OW_ProviderManager::CIMOM_PROVIDER_IFC = "cimom";
