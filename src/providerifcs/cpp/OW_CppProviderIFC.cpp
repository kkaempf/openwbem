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
#include "OW_CppProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_CppProxyProvider.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_Assertion.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Condition.hpp"

namespace OpenWBEM
{

//#include <setjmp.h>
typedef CppProviderBaseIFC* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();
namespace
{
	const String COMPONENT_NAME("ow.provider.cpp.ifc");
}
const char* const CppProviderIFC::CREATIONFUNC = "createProvider";

//////////////////////////////////////////////////////////////////////////////
class CppProviderIFC::CppProviderInitializationHelper : public IntrusiveCountableBase
{
public:

	explicit CppProviderInitializationHelper(const CppProviderBaseIFCRef& provider)
		: m_initialized(false)
		, m_provider(provider)
	{

	}

	void waitUntilInitialized() const
	{
		NonRecursiveMutexLock l(m_initializedGuard);
		while (!m_initialized)
		{
			m_initializedCond.wait(l);
		}
	}
	
	void initialize(const ProviderEnvironmentIFCRef& env)
	{
		m_provider->initialize(env);
		NonRecursiveMutexLock l(m_initializedGuard);
		m_initialized = true;
		m_initializedCond.notifyAll();
	}

	CppProviderBaseIFCRef getProvider() const
	{
		return m_provider;
	}
private:
	bool m_initialized;
	mutable NonRecursiveMutex m_initializedGuard;
	mutable Condition m_initializedCond;
	CppProviderBaseIFCRef m_provider;
};

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
		while (it != m_provs.end())
		{
			it->second = 0;
			it++;
		}
	
		m_provs.clear();
	
		for (size_t i = 0; i < m_noUnloadProviders.size(); i++)
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
	if (pProv)
	{
		CppInstanceProviderIFC* pIP = pProv->getInstanceProvider();
		if (pIP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("CPPProviderIFC found instance"
				" provider %1", provIdString));
			CppInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
//			ipRef.useRefCountOf(pProv);
			return InstanceProviderIFCRef(new CppInstanceProviderProxy(ipRef));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an instance provider",
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
	if (pProv)
	{
		CppSecondaryInstanceProviderIFC* pIP = pProv->getSecondaryInstanceProvider();
		if (pIP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("CPPProviderIFC found secondary instance"
				" provider %1", provIdString));
			CppSecondaryInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
//			ipRef.useRefCountOf(pProv);
			return SecondaryInstanceProviderIFCRef(new CppSecondaryInstanceProviderProxy(ipRef));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not a secondary instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
CppProviderIFC::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef&)
{
	IndicationExportProviderIFCRefArray rvra;
	for (size_t i = 0; i < m_noUnloadProviders.size(); i++)
	{
		CppProviderBaseIFCRef pProv = m_noUnloadProviders[i];
		CppIndicationExportProviderIFC* pIEP =
			pProv->getIndicationExportProvider();
		if (pIEP)
		{
			CppIndicationExportProviderIFCRef iepRef(pProv.getLibRef(), pIEP);
//			iepRef.useRefCountOf(pProv);
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
	for (size_t i = 0; i < m_noUnloadProviders.size(); i++)
	{
		CppProviderBaseIFCRef pProv = m_noUnloadProviders[i];
		CppPolledProviderIFC* pPP = pProv->getPolledProvider();
		if (pPP)
		{
			CppPolledProviderIFCRef ppRef(pProv.getLibRef(), pPP);
//			ppRef.useRefCountOf(pProv);
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
	if (pProv)
	{
		CppMethodProviderIFC* pMP = pProv->getMethodProvider();
		if (pMP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("CPPProviderIFC found method provider %1",
				provIdString));
			CppMethodProviderIFCRef mpRef(pProv.getLibRef(), pMP);
//			mpRef.useRefCountOf(pProv);
			return MethodProviderIFCRef(
				new CppMethodProviderProxy(mpRef));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not a method provider",
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
	if (pProv)
	{
		CppAssociatorProviderIFC* pAP = pProv->getAssociatorProvider();
		if (pAP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("CPPProviderIFC found associator provider %1",
				provIdString));
			CppAssociatorProviderIFCRef apRef(pProv.getLibRef(), pAP);
//			apRef.useRefCountOf(pProv);
			return AssociatorProviderIFCRef(new
				CppAssociatorProviderProxy(apRef));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an associator provider",
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
	if (pProv)
	{
		CppIndicationProviderIFC* pAP = pProv->getIndicationProvider();
		if (pAP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("CPPProviderIFC found indication provider %1",
				provIdString));

			IndicationProviderMap::const_iterator ci = m_indicationProviders.find(provIdString);
			if (ci != m_indicationProviders.end())
			{
				return ci->second;
			}

			CppIndicationProviderIFCRef apRef(pProv.getLibRef(), pAP);
//			apRef.useRefCountOf(pProv);
			IndicationProviderIFCRef rv(new
				CppIndicationProviderProxy(apRef));
			
			m_indicationProviders.insert(IndicationProviderMap::value_type(provIdString, rv));

			return rv;
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an indication provider",
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
	if (m_loadDone)
	{
		return;
	}
	m_loadDone = true;

	// Create the Shared library loader

	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "C++ provider ifc failed to get shared lib"
			" loader");
		return;
	}

	String libPathsStr = env->getConfigItem(
		ConfigOpts::CPPIFC_PROV_LOC_opt, OW_DEFAULT_CPP_PROVIDER_LOCATION);
	StringArray paths = libPathsStr.tokenize(";:");
	for (StringArray::size_type i1 = 0; i1 < paths.size(); i1++)
	{
		StringArray dirEntries;
		if (!FileSystem::getDirectoryContents(paths[i1], dirEntries))
		{
			OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc failed getting"
				" contents of directory: %1", paths[i1]));
			continue;
		}
		for (size_t i = 0; i < dirEntries.size(); i++)
		{
			if (!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
			{
				continue;
			}
			String libName = paths[i1];
			libName += OW_FILENAME_SEPARATOR;
			libName += dirEntries[i];

	#ifdef OW_DARWIN
			if (!FileSystem::isLink(libName))
			{
				continue;
			}
	#endif // OW_DARWIN
	
			SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
				env->getLogger(COMPONENT_NAME));
			if (!theLib)
			{
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc failed to load library: %1", libName));
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				continue;
			}
			versionFunc_t versFunc;
			if (!theLib->getFunctionPointer("getOWVersion",
				versFunc))
			{
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc failed getting function pointer to \"getOWVersion\" from"
					" library: %1", libName));
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				continue;
			}
			const char* strVer = (*versFunc)();
			if (strcmp(strVer, OW_VERSION))
			{
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc got invalid version from provider: %1", strVer));
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				continue;
			}
			ProviderCreationFunc createProvider;
			String creationFuncName = String(CREATIONFUNC) + "NO_ID";
			if (!theLib->getFunctionPointer(creationFuncName,
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
					OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "****************************************");
					OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc: Libary %1 does not load", libName));
					OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "****************************************");
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
				if (p_polledProv || p_indExpProv)
				{
					if (p_indExpProv)
					{
						OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc"
							" loaded indication export provider from lib: %1 -"
							" initializing", libName));
					}

					if (p_polledProv)
					{
						OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc"
							" loaded polled provider from lib: %1 -"
							" initializing", libName));
					}
					CppProviderInitializationHelperRef p2(new CppProviderInitializationHelper(p));
					p2->initialize(env);
					p->setPersist(true);
					m_noUnloadProviders.append(p);
					m_provs[providerid] = p2;
				}

				CppInstanceProviderIFC* p_ip = p->getInstanceProvider();
				if (p_ip)
				{
					InstanceProviderInfo info;
					info.setProviderName(providerid);
					p_ip->getInstanceProviderInfo(info);
					instanceProviderInfo.push_back(info);
				}

				CppSecondaryInstanceProviderIFC* p_sip =
					p->getSecondaryInstanceProvider();

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
			AutoPtr<CppProviderBaseIFC> pProv((*createProvider)());
			if (!pProv.get())
			{
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc: Libary %1 - %2 returned null provider", libName, creationFuncName));
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				continue;
			}
			CppPolledProviderIFC* p_itp = pProv->getPolledProvider();
			CppIndicationExportProviderIFC* p_iep =
				pProv->getIndicationExportProvider();
			if (p_itp || p_iep)
			{
				if (p_iep)
				{
					OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc loaded"
						" indication export provider from lib: %1 -"
						" initializing", libName));
				}
				if (p_itp)
				{
					OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc loaded"
						" polled provider from lib: %1 - initializing",
						libName));
				}

				pProv->initialize(env);
				m_noUnloadProviders.append(CppProviderBaseIFCRef(theLib,
					pProv.release()));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//static
CppProviderBaseIFCRef
CppProviderIFC::loadProvider(const String& libName, LoggerRef logger)
{
	String provId = libName.substring(libName.lastIndexOf(OW_FILENAME_SEPARATOR)+1);
	// chop of lib and .so
	provId = provId.substring(3, provId.length() - (strlen(OW_SHAREDLIB_EXTENSION) + 3));

	SharedLibraryLoaderRef ldr = SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		OW_LOG_ERROR(logger, "C++ provider ifc FAILED to get shared lib loader");
		return CppProviderBaseIFCRef();
	}

	OW_LOG_DEBUG(logger, Format("CppProviderIFC::getProvider loading library: %1", libName));

	SharedLibraryRef theLib = ldr->loadSharedLibrary(libName, logger);

	versionFunc_t versFunc;
	if (!theLib->getFunctionPointer("getOWVersion", versFunc))
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc failed getting"
			" function pointer to \"getOWVersion\" from library %1.", libName));
		return CppProviderBaseIFCRef();
	}
	const char* strVer = (*versFunc)();
	if (strcmp(strVer, OW_VERSION))
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc got invalid version from provider: %1.", libName));
		return CppProviderBaseIFCRef();
	}

	ProviderCreationFunc createProvider;
	String creationFuncName = String(CREATIONFUNC) + provId;
	if (!theLib->getFunctionPointer(creationFuncName, createProvider))
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc: Libary %1 does not contain %2 function.",
			libName, creationFuncName));
		return CppProviderBaseIFCRef();
	}

	CppProviderBaseIFC* pProv = (*createProvider)();

	if (!pProv)
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc: Libary %1 -"
			" %2 returned null provider. Not loaded.", libName, creationFuncName));
		return CppProviderBaseIFCRef();
	}

	CppProviderBaseIFCRef rval(theLib, pProv);

	OW_LOG_DEBUG(logger, Format("C++ provider ifc successfully loaded"
		" library %1 for provider %2", libName, provId));

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
CppProviderBaseIFCRef
CppProviderIFC::getProvider(
	const ProviderEnvironmentIFCRef& env, const char* provIdString,
	StoreProviderFlag storeP, InitializeProviderFlag initP)
{
	OW_ASSERT((initP == initializeProvider && storeP == storeProvider) || (initP == dontInitializeProvider && storeP == dontStoreProvider));

	MutexLock ml(m_guard);

	String provId(provIdString);
	ProviderMap::iterator it = m_provs.find(provId);
	if (it != m_provs.end())
	{
		// make a copy in case the map gets modified when we unlock m_guard
		CppProviderInitializationHelperRef prov(it->second);
		// do this to prevent a deadlock
		ml.release();
		// another thread may be initializing the provider, wait until it's done.
		prov->waitUntilInitialized();
		return prov->getProvider();
	}

	String libName;
	CppProviderBaseIFCRef rval;

	String libPathsStr = env->getConfigItem(
		ConfigOpts::CPPIFC_PROV_LOC_opt, OW_DEFAULT_CPP_PROVIDER_LOCATION);
	StringArray paths = libPathsStr.tokenize(";:");
	for (StringArray::size_type i = 0; i < paths.size(); i++)
	{
		libName = paths[i];
		libName += OW_FILENAME_SEPARATOR;
		libName += "lib";
		libName += provId;
		libName += OW_SHAREDLIB_EXTENSION;

		if (!FileSystem::exists(libName))
		{
			continue;
		}

		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("CppProviderIFC::getProvider loading"
			" library: %1", libName));

		rval = loadProvider(libName, env->getLogger(COMPONENT_NAME));

		if (rval)
		{
			break;
		}

	}

	if (!rval)
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc failed to load"
			" library: %1 for provider id %2. Skipping.", libName, provId));
		return rval;
	}

	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc successfully loaded"
		" library %1 for provider %2", libName, provId));

	if (initP == initializeProvider && storeP == storeProvider)
	{
		CppProviderInitializationHelperRef provInitHelper(new CppProviderInitializationHelper(rval));

		m_provs[provId] = provInitHelper;
		
		// now it's in the map, we can unlock the mutex protecting the map
		ml.release();

		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc calling initialize"
			" for provider %1", provId));

		provInitHelper->initialize(env); // Let provider initialize itself

		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc: provider %1"
			" loaded and initialized", provId));

	}
	else
	{
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("C++ provider ifc: provider %1"
			" loaded but not initialized", provId));
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
		if (!iter->second->getProvider()->getPersist())
		{
			DateTime provDt = iter->second->getProvider()->getLastAccessTime();
			provDt.addMinutes(iTimeWindow);
			if (provDt < dt && iter->second->getProvider()->canUnload())
			{
				OW_LOG_INFO(env->getLogger(COMPONENT_NAME), Format("Unloading Provider %1",
					iter->first));
				iter->second = 0;
				m_provs.erase(iter);
			}
		}

		++iter;
	}
}

void CppProviderIFC::doShuttingDown()
{
	// Need to lock m_guard while accessing m_provs, but we can't leave the mutex locked
	// while calling shuttingDown(), since that might cause a deadlock, so make a copy.
	MutexLock l(m_guard);
	ProviderMap provsCopy(m_provs);
	l.release();

	ProviderMap::iterator it, itend = provsCopy.end();
	for (it = provsCopy.begin(); it != itend; ++it)
	{
		it->second->getProvider()->shuttingDown();
	}
}

} // end namespace OpenWBEM

