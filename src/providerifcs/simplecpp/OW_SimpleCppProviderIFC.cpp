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
#include "OW_SimpleCppProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_SimpleCppProxyProvider.hpp"
#include "OW_NoSuchProviderException.hpp"

typedef OW_SimpleCppProviderBaseIFC* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();

const char* const OW_SimpleCppProviderIFC::CREATIONFUNC = "createProvider";

//////////////////////////////////////////////////////////////////////////////
OW_SimpleCppProviderIFC::OW_SimpleCppProviderIFC()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_SimpleCppProviderIFC::~OW_SimpleCppProviderIFC()
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
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SimpleCppProviderIFC::doInit(const OW_ProviderEnvironmentIFCRef& env,
	OW_InstanceProviderInfoArray& i,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	OW_AssociatorProviderInfoArray& a,
#endif
	OW_MethodProviderInfoArray& m,
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
	OW_PropertyProviderInfoArray& p,
#endif
	OW_IndicationProviderInfoArray& ind)
{
	// TOOD: modify this and the providers to get the InstanceProviderInfo
	(void)env;
	(void)i;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	(void)a;
#endif
	(void)m;
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
	(void)p;
#endif
	(void)ind;
	return;
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_SimpleCppProviderIFC::doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_SimpleCppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		OW_SimpleCppInstanceProviderIFC* pIP = pProv->getInstanceProvider();
		if(pIP)
		{
			env->getLogger()->logDebug(format("OW_SimpleCppProviderIFC found instance"
				" provider %1", provIdString));

			OW_SimpleCppInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
			ipRef.useRefCountOf(pProv);

			return OW_InstanceProviderIFCRef(new OW_SimpleCppInstanceProviderProxy(ipRef));
		}

		env->getLogger()->logError(format("Provider %1 is not an instance provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray
OW_SimpleCppProviderIFC::doGetIndicationExportProviders(const OW_ProviderEnvironmentIFCRef&)
{
	OW_IndicationExportProviderIFCRefArray rvra;
	return rvra;
}

//////////////////////////////////////////////////////////////////////////////
OW_PolledProviderIFCRefArray
OW_SimpleCppProviderIFC::doGetPolledProviders(const OW_ProviderEnvironmentIFCRef&)
{
	OW_PolledProviderIFCRefArray rvra;
	return rvra;
}

//////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef
OW_SimpleCppProviderIFC::doGetMethodProvider(const OW_ProviderEnvironmentIFCRef&,
	const char* provIdString)
{
	OW_THROW(OW_NoSuchProviderException, provIdString);
}

#ifdef OW_ENABLE_PROPERTY_PROVIDERS
//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_SimpleCppProviderIFC::doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_SimpleCppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		OW_SimpleCppPropertyProviderIFC* pPP = pProv->getPropertyProvider();
		if(pPP)
		{
			env->getLogger()->logDebug(format("OW_CPPProviderIFC found property provider %1",
				provIdString));
			OW_SimpleCppPropertyProviderIFCRef ppRef(pProv.getLibRef(), pPP);
			ppRef.useRefCountOf(pProv);


			return OW_PropertyProviderIFCRef(new OW_SimpleCppPropertyProviderProxy(ppRef));
		}

		env->getLogger()->logError(format("Provider %1 is not a property provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}
#endif

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_SimpleCppProviderIFC::doGetAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_SimpleCppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		OW_SimpleCppAssociatorProviderIFC* pAP = pProv->getAssociatorProvider();
		if(pAP)
		{
			env->getLogger()->logDebug(format("OW_CPPProviderIFC found associator provider %1",
				provIdString));
			OW_SimpleCppAssociatorProviderIFCRef apRef(pProv.getLibRef(), pAP);
			apRef.useRefCountOf(pProv);

			return OW_AssociatorProviderIFCRef(new
				OW_SimpleCppAssociatorProviderProxy(apRef));
		}

		env->getLogger()->logError(format("Provider %1 is not an associator provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}
#endif

//////////////////////////////////////////////////////////////////////////////
OW_IndicationProviderIFCRef
OW_SimpleCppProviderIFC::doGetIndicationProvider(const OW_ProviderEnvironmentIFCRef&,
	const char* provIdString)
{
	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
OW_SimpleCppProviderBaseIFCRef
OW_SimpleCppProviderIFC::getProvider(
	const OW_ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	OW_MutexLock ml(m_guard);

	OW_String provId(provIdString);
	ProviderMap::iterator it = m_provs.find(provId);
	if(it != m_provs.end())
	{
		return it->second;
	}

	OW_String libPath = env->getConfigItem(
		OW_ConfigOpts::SIMPLECPPIFC_PROV_LOC_opt, OW_DEFAULT_SIMPLE_CPP_PROVIDER_LOCATION);

	OW_SharedLibraryLoaderRef ldr =
		OW_SharedLibraryLoader::createSharedLibraryLoader();

	if(ldr.isNull())
	{
		env->getLogger()->logError("simple C++ provider ifc failed to get shared lib loader");
		return OW_SimpleCppProviderBaseIFCRef();
	}

	OW_String libName(libPath);
	libName += OW_FILENAME_SEPARATOR;
	libName += "lib";
	libName += provId;
	libName += ".so";

	env->getLogger()->logDebug(format("OW_SimpleCppProviderIFC::getProvider loading library: %1",
		libName));

	OW_SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
		env->getLogger());

	if(theLib.isNull())
	{
		env->getLogger()->logError(format("simple C++ provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return OW_SimpleCppProviderBaseIFCRef();
	}

	versionFunc_t versFunc;
	if (!OW_SharedLibrary::getFunctionPointer(theLib, "getOWVersion", versFunc))
	{
		env->getLogger()->logError("simple C++ provider ifc failed getting"
			" function pointer to \"getOWVersion\" from library");

		return OW_SimpleCppProviderBaseIFCRef();
	}

	const char* strVer = (*versFunc)();
	if(strcmp(strVer, OW_VERSION))
	{
		env->getLogger()->logError(format("simple C++ provider ifc got invalid version from provider:"
			" %1", libName));
		return OW_SimpleCppProviderBaseIFCRef();
	}

	ProviderCreationFunc createProvider;
	OW_String creationFuncName = OW_String(CREATIONFUNC) + provId;
	if(!OW_SharedLibrary::getFunctionPointer(theLib, creationFuncName, createProvider))
	{
		env->getLogger()->logError(format("simple C++ provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		return OW_SimpleCppProviderBaseIFCRef();
	}

	OW_SimpleCppProviderBaseIFC* pProv = (*createProvider)();
	if(!pProv)
	{
		env->getLogger()->logError(format("simple C++ provider ifc: Libary %1 - %2 returned null"
			" provider", libName, creationFuncName));
		return OW_SimpleCppProviderBaseIFCRef();
	}

	env->getLogger()->logDebug(format("simple C++ provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, provId));

	pProv->initialize(env);	// Let provider initialize itself

	env->getLogger()->logDebug(format("simple C++ provider ifc: provider %1 loaded and initialized",
		provId));

	m_provs[provId] = OW_SimpleCppProviderBaseIFCRef(theLib, pProv);

	return m_provs[provId];
}


//////////////////////////////////////////////////////////////////////////////
void
OW_SimpleCppProviderIFC::doUnloadProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	OW_String timeWindow = env->getConfigItem(OW_ConfigOpts::CPPIFC_PROV_TTL_opt, OW_DEFAULT_CPPIFC_PROV_TTL);

	OW_Int32 iTimeWindow;
	try
	{
		iTimeWindow = timeWindow.toInt32();
	}
    catch (const OW_StringConversionException&)
	{
		iTimeWindow = OW_String(OW_DEFAULT_CPPIFC_PROV_TTL).toInt32();
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
		if (provDt < dt)
		{
			env->getLogger()->logCustInfo(format("Unloading Provider %1", iter->first));
			m_provs.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

OW_PROVIDERIFCFACTORY(OW_SimpleCppProviderIFC)

