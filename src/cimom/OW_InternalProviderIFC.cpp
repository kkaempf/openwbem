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
#include "OW_InternalProviderIFC.hpp"
#include "OW_ProxyProvider.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"


///////////////////////////////////////////////////////////////////////////////
OW_InternalProviderIFC::~OW_InternalProviderIFC()
{
	try
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
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

///////////////////////////////////////////////////////////////////////////////
void
OW_InternalProviderIFC::addCIMOMProvider(const OW_CppProviderBaseIFCRef& pProv)
{
	OW_ASSERT(pProv->getPolledProvider() || pProv->getIndicationExportProvider());

	OW_MutexLock ml(m_guard);

	CimProv cprov;
	cprov.m_pProv = pProv;
	m_noIdProviders.append(cprov);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InternalProviderIFC::addCIMOMProvider(const OW_String& providerNameArg,
	const OW_CppProviderBaseIFCRef& pProv)
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
void
OW_InternalProviderIFC::doInit(const OW_ProviderEnvironmentIFCRef& env,
	OW_InstanceProviderInfoArray& instInfos,
	OW_AssociatorProviderInfoArray& assocInfos,
	OW_MethodProviderInfoArray& methInfos,
	OW_PropertyProviderInfoArray& propInfos,
	OW_IndicationProviderInfoArray& indInfos)
{
	OW_MutexLock l(m_guard);
	for (ProviderMap::iterator i =  m_cimomProviders.begin(); i != m_cimomProviders.end(); ++i)
	{
		i->second.m_pProv->initialize(env);
		i->second.m_initDone = true;
	}
	for (OW_Array<CimProv>::iterator i = m_noIdProviders.begin(); i != m_noIdProviders.end(); ++i)
	{
		i->m_pProv->initialize(env);
		i->m_initDone = true;
	}

	for (ProviderMap::iterator it =  m_cimomProviders.begin(); it != m_cimomProviders.end(); ++it)
	{
		OW_CppInstanceProviderIFC* pIP = it->second.m_pProv->getInstanceProvider();

		if(pIP)
		{
			OW_InstanceProviderInfo	provInfo;
			provInfo.setProviderName(it->first);
			pIP->getProviderInfo(provInfo);
			instInfos.push_back(provInfo);
		}
		
		OW_CppAssociatorProviderIFC* pAP = it->second.m_pProv->getAssociatorProvider();
		if (pAP)
		{
			OW_AssociatorProviderInfo provInfo;
			provInfo.setProviderName(it->first);
			pAP->getProviderInfo(provInfo);
			assocInfos.push_back(provInfo);
		}

		OW_CppMethodProviderIFC* pMP = it->second.m_pProv->getMethodProvider();
		if (pMP)
		{
			OW_MethodProviderInfo provInfo;
			provInfo.setProviderName(it->first);
			pMP->getProviderInfo(provInfo);
			methInfos.push_back(provInfo);
		}

		OW_CppPropertyProviderIFC* pPP = it->second.m_pProv->getPropertyProvider();
		if (pPP)
		{
			OW_PropertyProviderInfo provInfo;
			provInfo.setProviderName(it->first);
			pPP->getProviderInfo(provInfo);
			propInfos.push_back(provInfo);
		}

		OW_CppIndicationProviderIFC* pIndP = it->second.m_pProv->getIndicationProvider();
		if (pIndP)
		{
			OW_IndicationProviderInfo provInfo;
			provInfo.setProviderName(it->first);
			pIndP->getProviderInfo(provInfo);
			indInfos.push_back(provInfo);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray
OW_InternalProviderIFC::doGetIndicationExportProviders(
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
OW_InternalProviderIFC::doGetPolledProviders(const OW_ProviderEnvironmentIFCRef& env)
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
OW_InternalProviderIFC::doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		OW_THROW(OW_NoSuchProviderException, provIdString);
	}

	OW_CppInstanceProviderIFC* pIP = it->second.m_pProv->getInstanceProvider();

	if(!pIP)
	{
		env->getLogger()->logError(format(
			"Provider Manager - not an instance provider: %1", provIdString));
		OW_THROW(OW_NoSuchProviderException, provIdString);
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
OW_InternalProviderIFC::doGetMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		OW_THROW(OW_NoSuchProviderException, provIdString);
	}

	OW_CppMethodProviderIFC* pMP = it->second.m_pProv->getMethodProvider();

	if(!pMP)
	{
		env->getLogger()->logError(format(
			"Provider Manager - not a method provider: %1", provIdString));
		OW_THROW(OW_NoSuchProviderException, provIdString);
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
OW_InternalProviderIFC::doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		OW_THROW(OW_NoSuchProviderException, provIdString);
	}

	OW_CppPropertyProviderIFC* pPP = it->second.m_pProv->getPropertyProvider();
	if(!pPP)
	{
		env->getLogger()->logError(format(
			"Provider Manager - not a property provider: %1", provIdString));
		OW_THROW(OW_NoSuchProviderException, provIdString);
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
OW_InternalProviderIFC::doGetAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		OW_THROW(OW_NoSuchProviderException, provIdString);
	}

	OW_CppAssociatorProviderIFC* pAP = it->second.m_pProv->getAssociatorProvider();
	if(!pAP)
	{
		env->getLogger()->logError(format(
			"Provider Manager - not an associator provider: %1", provIdString));

		OW_THROW(OW_NoSuchProviderException, provIdString);
	}

	if(!it->second.m_initDone)
	{
		it->second.m_pProv->initialize(env);
		it->second.m_initDone = true;
	}

	OW_CppAssociatorProviderIFCRef apRef(it->second.m_pProv.getLibRef(), pAP);

	return OW_AssociatorProviderIFCRef(new OW_AssociatorProviderProxy(apRef));
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationProviderIFCRef
OW_InternalProviderIFC::doGetIndicationProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char *provIdString)
{
	OW_MutexLock ml(m_guard);

	ProviderMap::iterator it = m_cimomProviders.find(OW_String(provIdString));

	if(it == m_cimomProviders.end())
	{
		OW_THROW(OW_NoSuchProviderException, provIdString);
	}

	OW_CppIndicationProviderIFC* pAP = it->second.m_pProv->getIndicationProvider();
	if(!pAP)
	{
		env->getLogger()->logError(format(
			"Provider Manager - not an indication provider: %1", provIdString));

		OW_THROW(OW_NoSuchProviderException, provIdString);
	}

	if(!it->second.m_initDone)
	{
		it->second.m_pProv->initialize(env);
		it->second.m_initDone = true;
	}

	OW_CppIndicationProviderIFCRef apRef(it->second.m_pProv.getLibRef(), pAP);

	return OW_IndicationProviderIFCRef(new OW_IndicationProviderProxy(apRef));
}



// TODO: Move these into their own files.
OW_CppProviderBaseIFC::~OW_CppProviderBaseIFC() {}
OW_CppIndicationProviderIFC::~OW_CppIndicationProviderIFC() {}
OW_CppInstanceProviderIFC::~OW_CppInstanceProviderIFC() {}
OW_CppAssociatorProviderIFC::~OW_CppAssociatorProviderIFC(){}
OW_CppIndicationExportProviderIFC::~OW_CppIndicationExportProviderIFC(){}
OW_CppMethodProviderIFC::~OW_CppMethodProviderIFC() {}
OW_CppPolledProviderIFC::~OW_CppPolledProviderIFC() {}
OW_CppPropertyProviderIFC::~OW_CppPropertyProviderIFC() {}
