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
#include "OW_CppProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_CppProxyProvider.hpp"
#include "OW_NoSuchProviderException.hpp"
//#include <setjmp.h>

typedef OW_CppProviderBaseIFC* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();

const char* const OW_CppProviderIFC::CREATIONFUNC = "createProvider";

//////////////////////////////////////////////////////////////////////////////
OW_CppProviderIFC::OW_CppProviderIFC()
	: OW_ProviderIFCBaseIFC()
	, m_provs()
	, m_guard()
	, m_noidProviders()
	, m_loadDone(false)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CppProviderIFC::~OW_CppProviderIFC()
{
	try
	{
		ProviderMap::iterator it = m_provs.begin();
		while(it != m_provs.end())
		{
			it->second->cleanup();
			it->second.setNull();
			it++;
		}
	
		m_provs.clear();
	
		for(size_t i = 0; i < m_noidProviders.size(); i++)
		{
			m_noidProviders[i]->cleanup();
			m_noidProviders[i].setNull();
		}
	
		m_noidProviders.clear();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CppProviderIFC::doInit(const OW_ProviderEnvironmentIFCRef& env,
	OW_InstanceProviderInfoArray& i,
	OW_AssociatorProviderInfoArray& a,
	OW_MethodProviderInfoArray& m,
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
	OW_PropertyProviderInfoArray& p,
#endif
	OW_IndicationProviderInfoArray& ind)
{
	loadProviders(env, i, a, m, 
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
		p, 
#endif
		ind);
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_CppProviderIFC::doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		OW_CppInstanceProviderIFC* pIP = pProv->getInstanceProvider();
		if(pIP)
		{
			env->getLogger()->logDebug(format("OW_CPPProviderIFC found instance"
				" provider %1", provIdString));

			OW_CppInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
			ipRef.useRefCountOf(pProv);

			return OW_InstanceProviderIFCRef(new OW_CppInstanceProviderProxy(ipRef));
		}

		env->getLogger()->logError(format("Provider %1 is not an instance provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray
OW_CppProviderIFC::doGetIndicationExportProviders(const OW_ProviderEnvironmentIFCRef&)
{
	OW_IndicationExportProviderIFCRefArray rvra;
	for(size_t i = 0; i < m_noidProviders.size(); i++)
	{
		OW_CppProviderBaseIFCRef pProv = m_noidProviders[i];
		OW_CppIndicationExportProviderIFC* pIEP =
			pProv->getIndicationExportProvider();
		if(pIEP)
		{
			OW_CppIndicationExportProviderIFCRef iepRef(pProv.getLibRef(), pIEP);
			iepRef.useRefCountOf(pProv);
			rvra.append(
				OW_IndicationExportProviderIFCRef(new
					OW_CppIndicationExportProviderProxy(iepRef)));
		}
	}

	return rvra;
}

//////////////////////////////////////////////////////////////////////////////
OW_PolledProviderIFCRefArray
OW_CppProviderIFC::doGetPolledProviders(const OW_ProviderEnvironmentIFCRef&)
{
	OW_PolledProviderIFCRefArray rvra;
	for(size_t i = 0; i < m_noidProviders.size(); i++)
	{
		OW_CppProviderBaseIFCRef pProv = m_noidProviders[i];
		OW_CppPolledProviderIFC* pPP = pProv->getPolledProvider();
		if(pPP)
		{
			OW_CppPolledProviderIFCRef ppRef(pProv.getLibRef(), pPP);
			ppRef.useRefCountOf(pProv);
			rvra.append(
				OW_PolledProviderIFCRef(new
					OW_CppPolledProviderProxy(ppRef)));
		}
	}

	return rvra;
}

//////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef
OW_CppProviderIFC::doGetMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		OW_CppMethodProviderIFC* pMP = pProv->getMethodProvider();
		if(pMP)
		{
			env->getLogger()->logDebug(format("OW_CPPProviderIFC found method provider %1",
				provIdString));

			OW_CppMethodProviderIFCRef mpRef(pProv.getLibRef(), pMP);
			mpRef.useRefCountOf(pProv);

			return OW_MethodProviderIFCRef(
				new OW_CppMethodProviderProxy(mpRef));
		}

		env->getLogger()->logError(format("Provider %1 is not a method provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

#ifdef OW_ENABLE_PROPERTY_PROVIDERS
//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_CppProviderIFC::doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		OW_CppPropertyProviderIFC* pPP = pProv->getPropertyProvider();
		if(pPP)
		{
			env->getLogger()->logDebug(format("OW_CPPProviderIFC found property provider %1",
				provIdString));
			OW_CppPropertyProviderIFCRef ppRef(pProv.getLibRef(), pPP);
			ppRef.useRefCountOf(pProv);


			return OW_PropertyProviderIFCRef(new OW_CppPropertyProviderProxy(ppRef));
		}

		env->getLogger()->logError(format("Provider %1 is not a property provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}
#endif

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_CppProviderIFC::doGetAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		OW_CppAssociatorProviderIFC* pAP = pProv->getAssociatorProvider();
		if(pAP)
		{
			env->getLogger()->logDebug(format("OW_CPPProviderIFC found associator provider %1",
				provIdString));
			OW_CppAssociatorProviderIFCRef apRef(pProv.getLibRef(), pAP);
			apRef.useRefCountOf(pProv);

			return OW_AssociatorProviderIFCRef(new
				OW_CppAssociatorProviderProxy(apRef));
		}

		env->getLogger()->logError(format("Provider %1 is not an associator provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationProviderIFCRef
OW_CppProviderIFC::doGetIndicationProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		OW_CppIndicationProviderIFC* pAP = pProv->getIndicationProvider();
		if(pAP)
		{
			env->getLogger()->logDebug(format("OW_CPPProviderIFC found indication provider %1",
				provIdString));
			OW_CppIndicationProviderIFCRef apRef(pProv.getLibRef(), pAP);
			apRef.useRefCountOf(pProv);

			return OW_IndicationProviderIFCRef(new
				OW_CppIndicationProviderProxy(apRef));
		}

		env->getLogger()->logError(format("Provider %1 is not an indication provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CppProviderIFC::loadProviders(const OW_ProviderEnvironmentIFCRef& env,
	OW_InstanceProviderInfoArray& instanceProviderInfo,
	OW_AssociatorProviderInfoArray& associatorProviderInfo,
	OW_MethodProviderInfoArray& methodProviderInfo,
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
	OW_PropertyProviderInfoArray& propertyProviderInfo,
#endif
	OW_IndicationProviderInfoArray& indicationProviderInfo)
{
	OW_MutexLock ml(m_guard);

	if(m_loadDone)
	{
		return;
	}

	m_loadDone = true;

	OW_String libPath = env->getConfigItem(OW_ConfigOpts::CPPIFC_PROV_LOC_opt);

	if(libPath.empty())
	{
		libPath = DEFAULT_CPP_PROVIDER_LOCATION;
	}

	OW_SharedLibraryLoaderRef ldr =
		OW_SharedLibraryLoader::createSharedLibraryLoader();

	if(ldr.isNull())
	{
		env->getLogger()->logError("C++ provider ifc failed to get shared lib loader");
		return;
	}

	OW_StringArray dirEntries;
	if(!OW_FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		env->getLogger()->logError(format("C++ provider ifc failed getting contents of "
			"directory: %1", libPath));
		return;
	}

	for(size_t i = 0; i < dirEntries.size(); i++)
	{
		if(!dirEntries[i].endsWith(".so"))
		{
			continue;
		}

		OW_String libName = libPath;
		libName += OW_FILENAME_SEPARATOR;
		libName += dirEntries[i];
		OW_SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
			env->getLogger());

		if(theLib.isNull())
		{
			env->getLogger()->logError(format("C++ provider ifc failed to load"
				" library: %1", libName));
			continue;
		}

		versionFunc_t versFunc;
		if(!OW_SharedLibrary::getFunctionPointer(theLib, "getOWVersion",
			versFunc))
		{
			env->getLogger()->logError(format("C++ provider ifc failed getting"
				" function pointer to \"getOWVersion\" from library: %1",
				libName));
			continue;
		}

		const char* strVer = (*versFunc)();
		if(strcmp(strVer, OW_VERSION))
		{
			env->getLogger()->logError(format("C++ provider ifc got invalid version from "
				"provider: %1", strVer));
			continue;
		}

		ProviderCreationFunc createProvider;
		OW_String creationFuncName = OW_String(CREATIONFUNC) + "NO_ID";
		if(!OW_SharedLibrary::getFunctionPointer(theLib, creationFuncName,
			createProvider))
		{
			// it's not a no-id provider
			// try and load it as an id provider
			OW_String providerid = dirEntries[i];
			// chop off lib and .so
			providerid = providerid.substring(3,providerid.length()-6);
			OW_CppProviderBaseIFCRef p = getProvider(env,providerid.c_str(), dontStoreProvider);
			if (!p)
			{
				env->getLogger()->logDebug(format("C++ provider ifc: Libary %1 does not load", libName));
				continue;
			}

			OW_CppInstanceProviderIFC* p_ip = p->getInstanceProvider();
			if (p_ip)
			{
				OW_InstanceProviderInfo info;
				info.setProviderName(providerid);
				p_ip->getProviderInfo(info);
				instanceProviderInfo.push_back(info);
			}
			OW_CppAssociatorProviderIFC* p_ap = p->getAssociatorProvider();
			if (p_ap)
			{
				OW_AssociatorProviderInfo info;
				info.setProviderName(providerid);
				p_ap->getProviderInfo(info);
				associatorProviderInfo.push_back(info);
			}
			OW_CppMethodProviderIFC* p_mp = p->getMethodProvider();
			if (p_mp)
			{
				OW_MethodProviderInfo info;
				info.setProviderName(providerid);
				p_mp->getProviderInfo(info);
				methodProviderInfo.push_back(info);
			}
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
			OW_CppPropertyProviderIFC* p_pp = p->getPropertyProvider();
			if (p_pp)
			{
				OW_PropertyProviderInfo info;
				info.setProviderName(providerid);
				p_pp->getProviderInfo(info);
				propertyProviderInfo.push_back(info);
			}
#endif
			OW_CppIndicationProviderIFC* p_indp = p->getIndicationProvider();
			if (p_indp)
			{
				OW_IndicationProviderInfo info;
				info.setProviderName(providerid);
				p_indp->getProviderInfo(info);
				indicationProviderInfo.push_back(info);
			}
			continue;
		}

		OW_CppProviderBaseIFC* pProv = (*createProvider)();
		if(!pProv)
		{
			env->getLogger()->logError(format("C++ provider ifc: Libary %1 - %2 returned null"
				" provider", libName, creationFuncName));
			continue;
		}

		// TODO: Make this exception safe, so pProv gets deleted if any of the
		// following throws.
		OW_CppPolledProviderIFC* p_itp = pProv->getPolledProvider();

		OW_CppIndicationExportProviderIFC* p_iep =
			pProv->getIndicationExportProvider();


		if (p_itp || p_iep)
		{
			if(p_iep)
			{
				env->getLogger()->logDebug(format("C++ provider ifc loaded indication export"
					" provider from lib: %1 - initializing", libName));
			}
			if(p_itp)
			{
				env->getLogger()->logDebug(format("C++ provider ifc loaded polled provider from "
					"lib: %1 - initializing", libName));
			}

			pProv->initialize(env);

			m_noidProviders.append(OW_CppProviderBaseIFCRef(theLib, pProv));
		}
		else
		{
			delete pProv;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CppProviderBaseIFCRef
OW_CppProviderIFC::getProvider(
	const OW_ProviderEnvironmentIFCRef& env, const char* provIdString,
	StoreProviderFlag storeP)
{
	OW_MutexLock ml(m_guard);

	OW_String provId(provIdString);
	ProviderMap::iterator it = m_provs.find(provId);
	if(it != m_provs.end())
	{
		return it->second;
	}

	OW_String libPath = env->getConfigItem(
		OW_ConfigOpts::CPPIFC_PROV_LOC_opt);

	if(libPath.empty())
	{
		libPath = DEFAULT_CPP_PROVIDER_LOCATION;
	}

	OW_SharedLibraryLoaderRef ldr =
		OW_SharedLibraryLoader::createSharedLibraryLoader();

	if(ldr.isNull())
	{
		env->getLogger()->logError("C++ provider ifc failed to get shared lib loader");
		return OW_CppProviderBaseIFCRef();
	}

	OW_String libName(libPath);
	libName += OW_FILENAME_SEPARATOR;
	libName += "lib";
	libName += provId;
	libName += ".so";

	env->getLogger()->logDebug(format("OW_CppProviderIFC::getProvider loading library: %1",
		libName));

	OW_SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
		env->getLogger());

	if(theLib.isNull())
	{
		env->getLogger()->logError(format("C++ provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return OW_CppProviderBaseIFCRef();
	}

	versionFunc_t versFunc;
	if (!OW_SharedLibrary::getFunctionPointer(theLib, "getOWVersion", versFunc))
	{
		env->getLogger()->logError("C++ provider ifc failed getting"
			" function pointer to \"getOWVersion\" from library");

		return OW_CppProviderBaseIFCRef();
	}

	const char* strVer = (*versFunc)();
	if(strcmp(strVer, OW_VERSION))
	{
		env->getLogger()->logError(format("C++ provider ifc got invalid version from provider:"
			" %1", libName));
		return OW_CppProviderBaseIFCRef();
	}

	ProviderCreationFunc createProvider;
	OW_String creationFuncName = OW_String(CREATIONFUNC) + provId;
	if(!OW_SharedLibrary::getFunctionPointer(theLib, creationFuncName, createProvider))
	{
		env->getLogger()->logError(format("C++ provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		return OW_CppProviderBaseIFCRef();
	}

	OW_CppProviderBaseIFC* pProv = (*createProvider)();
	if(!pProv)
	{
		env->getLogger()->logError(format("C++ provider ifc: Libary %1 - %2 returned null"
			" provider", libName, creationFuncName));
		return OW_CppProviderBaseIFCRef();
	}

	env->getLogger()->logDebug(format("C++ provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, provId));

	pProv->initialize(env);	// Let provider initialize itself

	env->getLogger()->logDebug(format("C++ provider ifc: provider %1 loaded and initialized",
		provId));

	OW_CppProviderBaseIFCRef rval(theLib, pProv);
	if (storeP == storeProvider)
	{
		m_provs[provId] = rval;
	}

	return rval;
}


//////////////////////////////////////////////////////////////////////////////
void
OW_CppProviderIFC::doUnloadProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	OW_String timeWindow = env->getConfigItem(OW_ConfigOpts::CPPIFC_PROV_TTL_opt);
	if (timeWindow.empty())
	{
		timeWindow = DEFAULT_CPPIFC_PROV_TTL;
	}

	OW_Int32 iTimeWindow;
	try
	{
		iTimeWindow = timeWindow.toInt32();
	}
    catch (const OW_StringConversionException&)
	{
		iTimeWindow = OW_String(DEFAULT_CPPIFC_PROV_TTL).toInt32();
	}
	if (iTimeWindow < 0)
	{
		return;
	}

	OW_DateTime dt;
	dt.setToCurrent();
	for (ProviderMap::iterator iter = m_provs.begin();
		  iter != m_provs.end();)
	{
		OW_DateTime provDt = iter->second->getLastAccessTime();
		provDt.addMinutes(iTimeWindow);
		if (provDt < dt && iter->second->canUnload())
		{
			env->getLogger()->logCustInfo(format("Unloading Provider %1", iter->first));
			// TODO: we can't call cleanup here since the provider may be in use.  Figure this out.
			//iter->second->cleanup();
			iter->second.setNull();
			m_provs.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}


// TODO: Move these into their own files.
OW_CppProviderBaseIFC::~OW_CppProviderBaseIFC() {}
void OW_CppProviderBaseIFC::updateAccessTime() { m_dt.setToCurrent(); }

OW_CppInstanceProviderIFC::~OW_CppInstanceProviderIFC() {}
OW_CppAssociatorProviderIFC::~OW_CppAssociatorProviderIFC(){}
OW_CppIndicationExportProviderIFC::~OW_CppIndicationExportProviderIFC(){}
OW_CppMethodProviderIFC::~OW_CppMethodProviderIFC() {}
OW_CppPolledProviderIFC::~OW_CppPolledProviderIFC() {}
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
OW_CppPropertyProviderIFC::~OW_CppPropertyProviderIFC() {}
#endif

//OW_PROVIDERIFCFACTORY(OW_CppProviderIFC)

