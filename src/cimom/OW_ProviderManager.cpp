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
	OW_ProviderIFCBaseIFC(), m_IFCArray(), m_cimomProviders(),
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
		it->second.m_pProv.setNull();
		it++;
	}

	m_cimomProviders.clear();

	// Clean up providers with no identification
	for(size_t i = 0; i < m_noIdProviders.size(); i++)
	{
		m_noIdProviders[i].m_pProv->cleanup();
		m_noIdProviders[i].m_pProv.setNull();
	}

	m_noIdProviders.clear();

	m_IFCArray.clear();
}

//////////////////////////////////////////////////////////////////////////////
void OW_ProviderManager::init(const OW_ProviderIFCLoaderRef IFCLoader)
{
	IFCLoader->loadIFCs(m_IFCArray);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::addCIMOMProvider(OW_CppProviderBaseIFCRef pProv)
{
	OW_ASSERT(pProv->getPolledProvider() || pProv->getIndicationExportProvider());

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
		OW_CppIndicationExportProviderIFC* pIEP =
			m_noIdProviders[i].m_pProv->getIndicationExportProvider();
		if (pIEP)
		{
			OW_CppProviderBaseIFCRef pProv =
				m_noIdProviders[i].m_pProv;
			
			if(!m_noIdProviders[i].m_initDone)
			{
				pProv->initialize(env);
				m_noIdProviders[i].m_initDone = true;
			}

			OW_CppIndicationExportProviderIFCRef iepRef(pProv.getLibRef(), pIEP);
			iepRef.useRefCountOf(pProv);

			rv.append(OW_IndicationExportProviderIFCRef(new
				OW_IndicationExportProviderProxy(iepRef)));
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
		OW_CppPolledProviderIFC* pPP =
			m_noIdProviders[i].m_pProv->getPolledProvider();
		if(pPP)
		{
			OW_CppProviderBaseIFCRef pProv = m_noIdProviders[i].m_pProv;
			if(!m_noIdProviders[i].m_initDone)
			{
				pProv->initialize(env);
				m_noIdProviders[i].m_initDone = true;
			}
			OW_CppPolledProviderIFCRef ppRef(pProv.getLibRef(), pPP);
			ppRef.useRefCountOf(pProv);

			rv.append(OW_PolledProviderIFCRef(new
				OW_PolledProviderProxy(ppRef)));
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
    OW_IndicationExportProviderIFCRefArray pra =
            this->doGetIndicationExportProviders(env);
    if(pra.size() > 0)
    {
        rv.appendArray(pra);
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
    OW_PolledProviderIFCRefArray pra =
            this->doGetPolledProviders(env);

    if(pra.size() > 0)
    {
        rv.appendArray(pra);
    }

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::unloadProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		m_IFCArray[i]->unloadProviders(env);
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

	if(ifcStr.equalsIgnoreCase(OW_String(CIMOM_PROVIDER_IFC)))
	{
		return OW_ProviderIFCBaseIFCRef(OW_SharedLibraryRef(),
			OW_Reference<OW_ProviderIFCBaseIFC>(
				const_cast<OW_ProviderManager*>(this), true));
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

	OW_CppInstanceProviderIFC* pIP = it->second.m_pProv->getInstanceProvider();

	if(!pIP)
	{
		env->getLogger()->logError(format(
			"Provider Manager - not an instance provider: %1", provIdString));
		return OW_InstanceProviderIFCRef(0);
	}

	OW_CppInstanceProviderIFCRef ipRef(it->second.m_pProv.getLibRef(), pIP);
	ipRef.useRefCountOf(it->second.m_pProv);

	if(!it->second.m_initDone)
	{
		it->second.m_pProv->initialize(env);
		it->second.m_initDone = true;
	}



	return OW_InstanceProviderIFCRef(new OW_InstanceProviderProxy(ipRef));
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

	OW_CppMethodProviderIFC* pMP = it->second.m_pProv->getMethodProvider();

	if(!pMP)
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

	OW_CppMethodProviderIFCRef mpRef(it->second.m_pProv.getLibRef(), pMP);
	mpRef.useRefCountOf(it->second.m_pProv);

	return OW_MethodProviderIFCRef(new OW_MethodProviderProxy(mpRef));
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

	OW_CppPropertyProviderIFC* pPP = it->second.m_pProv->getPropertyProvider();
	if(!pPP)
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

	OW_CppPropertyProviderIFCRef ppRef(it->second.m_pProv.getLibRef(), pPP);
	ppRef.useRefCountOf(it->second.m_pProv);

	return OW_PropertyProviderIFCRef(new OW_PropertyProviderProxy(ppRef));
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

	OW_CppAssociatorProviderIFC* pAP = it->second.m_pProv->getAssociatorProvider();
	if(!pAP)
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

	OW_CppAssociatorProviderIFCRef apRef(it->second.m_pProv.getLibRef(), pAP);

	return OW_AssociatorProviderIFCRef(new OW_AssociatorProviderProxy(apRef));
}

const char* const OW_ProviderManager::CIMOM_PROVIDER_IFC = "cimom";
