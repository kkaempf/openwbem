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
#include "OW_CppProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_CppProxyProvider.hpp"
#include "OW_NoSuchProviderException.hpp"

namespace OpenWBEM
{

//#include <setjmp.h>
typedef CppProviderBaseIFC* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();
const char* const CppProviderIFC::CREATIONFUNC = "createProvider";
//////////////////////////////////////////////////////////////////////////////
CppProviderIFC::CppProviderIFC()
	: ProviderIFCBaseIFC()
	, m_provs()
	, m_guard()
	, m_noUnloadProviders()
	, m_loadDone(false)
{
}
//////////////////////////////////////////////////////////////////////////////
CppProviderIFC::~CppProviderIFC()
{
	try
	{
		ProviderMap::iterator it = m_provs.begin();
		while(it != m_provs.end())
		{
			it->second.setNull();
			it++;
		}
	
		m_provs.clear();
	
		for(size_t i = 0; i < m_noUnloadProviders.size(); i++)
		{
			m_noUnloadProviders[i].setNull();
		}
	
		m_noUnloadProviders.clear();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CppProviderIFC::doInit(const ProviderEnvironmentIFCRef& env,
	InstanceProviderInfoArray& i,
	SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	AssociatorProviderInfoArray& a,
#endif
	MethodProviderInfoArray& m,
	IndicationProviderInfoArray& ind)
{
	loadProviders(env, i,
		si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		a,
#endif
		m,
		ind);
}
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
CppProviderIFC::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		CppInstanceProviderIFC* pIP = pProv->getInstanceProvider();
		if(pIP)
		{
			env->getLogger()->logDebug(Format("CPPProviderIFC found instance"
				" provider %1", provIdString));
			CppInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
			ipRef.useRefCountOf(pProv);
			return InstanceProviderIFCRef(new CppInstanceProviderProxy(ipRef));
		}
		env->getLogger()->logError(Format("Provider %1 is not an instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
SecondaryInstanceProviderIFCRef
CppProviderIFC::doGetSecondaryInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		CppSecondaryInstanceProviderIFC* pIP = pProv->getSecondaryInstanceProvider();
		if(pIP)
		{
			env->getLogger()->logDebug(Format("CPPProviderIFC found secondary instance"
				" provider %1", provIdString));
			CppSecondaryInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
			ipRef.useRefCountOf(pProv);
			return SecondaryInstanceProviderIFCRef(new CppSecondaryInstanceProviderProxy(ipRef));
		}
		env->getLogger()->logError(Format("Provider %1 is not a secondary instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
CppProviderIFC::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef&)
{
	IndicationExportProviderIFCRefArray rvra;
	for(size_t i = 0; i < m_noUnloadProviders.size(); i++)
	{
		CppProviderBaseIFCRef pProv = m_noUnloadProviders[i];
		CppIndicationExportProviderIFC* pIEP =
			pProv->getIndicationExportProvider();
		if(pIEP)
		{
			CppIndicationExportProviderIFCRef iepRef(pProv.getLibRef(), pIEP);
			iepRef.useRefCountOf(pProv);
			rvra.append(
				IndicationExportProviderIFCRef(new
					CppIndicationExportProviderProxy(iepRef)));
		}
	}
	return rvra;
}
//////////////////////////////////////////////////////////////////////////////
PolledProviderIFCRefArray
CppProviderIFC::doGetPolledProviders(const ProviderEnvironmentIFCRef&)
{
	PolledProviderIFCRefArray rvra;
	for(size_t i = 0; i < m_noUnloadProviders.size(); i++)
	{
		CppProviderBaseIFCRef pProv = m_noUnloadProviders[i];
		CppPolledProviderIFC* pPP = pProv->getPolledProvider();
		if(pPP)
		{
			CppPolledProviderIFCRef ppRef(pProv.getLibRef(), pPP);
			ppRef.useRefCountOf(pProv);
			rvra.append(
				PolledProviderIFCRef(new
					CppPolledProviderProxy(ppRef)));
		}
	}
	return rvra;
}
//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
CppProviderIFC::doGetMethodProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		CppMethodProviderIFC* pMP = pProv->getMethodProvider();
		if(pMP)
		{
			env->getLogger()->logDebug(Format("CPPProviderIFC found method provider %1",
				provIdString));
			CppMethodProviderIFCRef mpRef(pProv.getLibRef(), pMP);
			mpRef.useRefCountOf(pProv);
			return MethodProviderIFCRef(
				new CppMethodProviderProxy(mpRef));
		}
		env->getLogger()->logError(Format("Provider %1 is not a method provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
CppProviderIFC::doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		CppAssociatorProviderIFC* pAP = pProv->getAssociatorProvider();
		if(pAP)
		{
			env->getLogger()->logDebug(Format("CPPProviderIFC found associator provider %1",
				provIdString));
			CppAssociatorProviderIFCRef apRef(pProv.getLibRef(), pAP);
			apRef.useRefCountOf(pProv);
			return AssociatorProviderIFCRef(new
				CppAssociatorProviderProxy(apRef));
		}
		env->getLogger()->logError(Format("Provider %1 is not an associator provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#endif
//////////////////////////////////////////////////////////////////////////////
IndicationProviderIFCRef
CppProviderIFC::doGetIndicationProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		CppIndicationProviderIFC* pAP = pProv->getIndicationProvider();
		if(pAP)
		{
			env->getLogger()->logDebug(Format("CPPProviderIFC found indication provider %1",
				provIdString));
			CppIndicationProviderIFCRef apRef(pProv.getLibRef(), pAP);
			apRef.useRefCountOf(pProv);
			return IndicationProviderIFCRef(new
				CppIndicationProviderProxy(apRef));
		}
		env->getLogger()->logError(Format("Provider %1 is not an indication provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
void
CppProviderIFC::loadProviders(const ProviderEnvironmentIFCRef& env,
	InstanceProviderInfoArray& instanceProviderInfo,
	SecondaryInstanceProviderInfoArray& secondaryInstanceProviderInfo,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	AssociatorProviderInfoArray& associatorProviderInfo,
#endif
	MethodProviderInfoArray& methodProviderInfo,
	IndicationProviderInfoArray& indicationProviderInfo)
{
	MutexLock ml(m_guard);
	if(m_loadDone)
	{
		return;
	}
	m_loadDone = true;
	String libPath = env->getConfigItem(ConfigOpts::CPPIFC_PROV_LOC_opt, OW_DEFAULT_CPP_PROVIDER_LOCATION);
	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if(ldr.isNull())
	{
		env->getLogger()->logError("C++ provider ifc failed to get shared lib loader");
		return;
	}
	StringArray dirEntries;
	if(!FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		env->getLogger()->logError(Format("C++ provider ifc failed getting contents of "
			"directory: %1", libPath));
		return;
	}
	for(size_t i = 0; i < dirEntries.size(); i++)
	{
		if(!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
		{
			continue;
		}
		String libName = libPath;
		libName += OW_FILENAME_SEPARATOR;
		libName += dirEntries[i];
		SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
			env->getLogger());
		if(theLib.isNull())
		{
			env->getLogger()->logError(Format("C++ provider ifc failed to load"
				" library: %1", libName));
			continue;
		}
		versionFunc_t versFunc;
		if(!theLib->getFunctionPointer("getOWVersion",
			versFunc))
		{
			env->getLogger()->logError(Format("C++ provider ifc failed getting"
				" function pointer to \"getOWVersion\" from library: %1",
				libName));
			continue;
		}
		const char* strVer = (*versFunc)();
		if(strcmp(strVer, OW_VERSION))
		{
			env->getLogger()->logError(Format("C++ provider ifc got invalid version from "
				"provider: %1", strVer));
			continue;
		}
		ProviderCreationFunc createProvider;
		String creationFuncName = String(CREATIONFUNC) + "NO_ID";
		if(!theLib->getFunctionPointer(creationFuncName,
			createProvider))
		{
			// it's not a no-id provider
			// try and load it as an id provider
			String providerid = dirEntries[i];
			// chop off lib and .so
			providerid = providerid.substring(3,
				providerid.length() - (strlen(OW_SHAREDLIB_EXTENSION) + 3));

			CppProviderBaseIFCRef p = getProvider(env, providerid.c_str(),
				dontStoreProvider, dontInitializeProvider);

			if (!p)
			{
				env->getLogger()->logDebug(Format("C++ provider ifc: Libary %1"
					" does not load", libName));
				continue;
			}

			// The named provider may also be an indication export or a
			// polled provider. If so, we'll store a reference to it in
			// the m_noUnloadProviders and we'll skip unload processing on
			// it when doUnloadProviders is called. We'll skip this
			// processing by checking the persist flag on the provider.
			CppPolledProviderIFC* p_polledProv = p->getPolledProvider();
			CppIndicationExportProviderIFC* p_indExpProv =
				p->getIndicationExportProvider();
			if(p_polledProv || p_indExpProv)
			{
				if(p_indExpProv)
				{
					env->getLogger()->logDebug(Format("C++ provider ifc loaded"
						" indication export provider from lib: %1 -"
						" initializing", libName));
				}

				if(p_polledProv)
				{
					env->getLogger()->logDebug(Format("C++ provider ifc loaded"
						" polled provider from lib: %1 - initializing",
						libName));
				}
				
				p->initialize(env);
				p->setPersist(true);
				m_noUnloadProviders.append(p);
				m_provs[providerid] = p;
			}

			CppInstanceProviderIFC* p_ip = p->getInstanceProvider();
			if (p_ip)
			{
				InstanceProviderInfo info;
				info.setProviderName(providerid);
				p_ip->getInstanceProviderInfo(info);
				instanceProviderInfo.push_back(info);
			}
			CppSecondaryInstanceProviderIFC* p_sip = p->getSecondaryInstanceProvider();
			if (p_sip)
			{
				SecondaryInstanceProviderInfo info;
				info.setProviderName(providerid);
				p_sip->getSecondaryInstanceProviderInfo(info);
				secondaryInstanceProviderInfo.push_back(info);
			}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			CppAssociatorProviderIFC* p_ap = p->getAssociatorProvider();
			if (p_ap)
			{
				AssociatorProviderInfo info;
				info.setProviderName(providerid);
				p_ap->getAssociatorProviderInfo(info);
				associatorProviderInfo.push_back(info);
			}
#endif
			CppMethodProviderIFC* p_mp = p->getMethodProvider();
			if (p_mp)
			{
				MethodProviderInfo info;
				info.setProviderName(providerid);
				p_mp->getMethodProviderInfo(info);
				methodProviderInfo.push_back(info);
			}
			CppIndicationProviderIFC* p_indp = p->getIndicationProvider();
			if (p_indp)
			{
				IndicationProviderInfo info;
				info.setProviderName(providerid);
				p_indp->getIndicationProviderInfo(info);
				indicationProviderInfo.push_back(info);
			}

			continue;
		}

		// If we reach this point, the NO_ID provider factory was used. This
		// means that the provider doesn't have an identifier (i.e. doesn't
		// do instance, methods, associators and such.
		CppProviderBaseIFC* pProv = (*createProvider)();
		if(!pProv)
		{
			env->getLogger()->logError(Format("C++ provider ifc: Libary %1 - %2 returned null"
				" provider", libName, creationFuncName));
			continue;
		}
		// TODO: Make this exception safe, so pProv gets deleted if any of the
		// following throws.
		CppPolledProviderIFC* p_itp = pProv->getPolledProvider();
		CppIndicationExportProviderIFC* p_iep =
			pProv->getIndicationExportProvider();
		if (p_itp || p_iep)
		{
			if(p_iep)
			{
				env->getLogger()->logDebug(Format("C++ provider ifc loaded indication export"
					" provider from lib: %1 - initializing", libName));
			}
			if(p_itp)
			{
				env->getLogger()->logDebug(Format("C++ provider ifc loaded polled provider from "
					"lib: %1 - initializing", libName));
			}
			pProv->initialize(env);
			m_noUnloadProviders.append(CppProviderBaseIFCRef(theLib, pProv));
		}
		else
		{
			delete pProv;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
CppProviderBaseIFCRef
CppProviderIFC::getProvider(
	const ProviderEnvironmentIFCRef& env, const char* provIdString,
	StoreProviderFlag storeP, InitializeProviderFlag initP)
{
	MutexLock ml(m_guard);
	String provId(provIdString);
	ProviderMap::iterator it = m_provs.find(provId);
	if(it != m_provs.end())
	{
		return it->second;
	}
	String libPath = env->getConfigItem(
		ConfigOpts::CPPIFC_PROV_LOC_opt, OW_DEFAULT_CPP_PROVIDER_LOCATION);
	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if(ldr.isNull())
	{
		env->getLogger()->logError("C++ provider ifc failed to get shared lib loader");
		return CppProviderBaseIFCRef();
	}
	String libName(libPath);
	libName += OW_FILENAME_SEPARATOR;
	libName += "lib";
	libName += provId;
	libName += OW_SHAREDLIB_EXTENSION;
	env->getLogger()->logDebug(Format("CppProviderIFC::getProvider loading library: %1",
		libName));
	SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
		env->getLogger());
	if(theLib.isNull())
	{
		env->getLogger()->logError(Format("C++ provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return CppProviderBaseIFCRef();
	}
	versionFunc_t versFunc;
	if (!theLib->getFunctionPointer("getOWVersion", versFunc))
	{
		env->getLogger()->logError("C++ provider ifc failed getting"
			" function pointer to \"getOWVersion\" from library");
		return CppProviderBaseIFCRef();
	}
	const char* strVer = (*versFunc)();
	if(strcmp(strVer, OW_VERSION))
	{
		env->getLogger()->logError(Format("C++ provider ifc got invalid version from provider:"
			" %1", libName));
		return CppProviderBaseIFCRef();
	}
	ProviderCreationFunc createProvider;
	String creationFuncName = String(CREATIONFUNC) + provId;
	if(!theLib->getFunctionPointer(creationFuncName, createProvider))
	{
		env->getLogger()->logError(Format("C++ provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		return CppProviderBaseIFCRef();
	}
	CppProviderBaseIFC* pProv = (*createProvider)();
	if(!pProv)
	{
		env->getLogger()->logError(Format("C++ provider ifc: Libary %1 - %2 returned null"
			" provider", libName, creationFuncName));
		return CppProviderBaseIFCRef();
	}
	env->getLogger()->logDebug(Format("C++ provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, provId));
	if (initP == initializeProvider)
	{
		pProv->initialize(env);	// Let provider initialize itself
	}
	env->getLogger()->logDebug(Format("C++ provider ifc: provider %1 loaded and initialized",
		provId));
	CppProviderBaseIFCRef rval(theLib, pProv);
	if (storeP == storeProvider)
	{
		m_provs[provId] = rval;
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
CppProviderIFC::doUnloadProviders(const ProviderEnvironmentIFCRef& env)
{
	String timeWindow = env->getConfigItem(ConfigOpts::CPPIFC_PROV_TTL_opt, OW_DEFAULT_CPPIFC_PROV_TTL);
	Int32 iTimeWindow;
	try
	{
		iTimeWindow = timeWindow.toInt32();
	}
	catch (const StringConversionException&)
	{
		iTimeWindow = String(OW_DEFAULT_CPPIFC_PROV_TTL).toInt32();
	}
	if (iTimeWindow < 0)
	{
		return;
	}
	DateTime dt;
	dt.setToCurrent();
	MutexLock l(m_guard);
	for (ProviderMap::iterator iter = m_provs.begin();
		  iter != m_provs.end();)
	{
		// If this is not a persistent provider, see if we can unload it.
		if(!iter->second->getPersist())
		{
			DateTime provDt = iter->second->getLastAccessTime();
			provDt.addMinutes(iTimeWindow);
			if (provDt < dt && iter->second->canUnload())
			{
				env->getLogger()->logInfo(Format("Unloading Provider %1",
					iter->first));
				iter->second.setNull();
				m_provs.erase(iter);
			}
		}

		++iter;
	}
}
// TODO: Move these into their own files.
CppProviderBaseIFC::~CppProviderBaseIFC() {}
void CppProviderBaseIFC::updateAccessTime() { m_dt.setToCurrent(); }
CppInstanceProviderIFC::~CppInstanceProviderIFC() {}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
CppAssociatorProviderIFC::~CppAssociatorProviderIFC(){}
#endif
CppIndicationExportProviderIFC::~CppIndicationExportProviderIFC(){}
CppMethodProviderIFC::~CppMethodProviderIFC() {}
CppPolledProviderIFC::~CppPolledProviderIFC() {}
//OW_PROVIDERIFCFACTORY(OW_CppProviderIFC)

} // end namespace OpenWBEM

