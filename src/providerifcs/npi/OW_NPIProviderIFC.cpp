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
#include "OW_NPIProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_NPIInstanceProviderProxy.hpp"
#include "OW_NPIMethodProviderProxy.hpp"
#include "OW_NPIAssociatorProviderProxy.hpp"
//#include <setjmp.h>

//////////////////////////////////////////////////////////////////////////////
OW_NPIProviderIFC::OW_NPIProviderIFC()
	: OW_ProviderIFCBaseIFC()
	, m_provs()
	, m_guard()
	//, m_noidProviders()
	, m_loadDone(false)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_NPIProviderIFC::~OW_NPIProviderIFC()
{
	ProviderMap::iterator it = m_provs.begin();
	//OW_Reference<NPIenv> npiHandle(); // TODO: createEnv(...);
	while(it != m_provs.end())
	{
		it->second->fp_cleanup(0); // TODO: FIX this. m_npiHandle);
		it->second.setNull();
		it++;
	}

	m_provs.clear();

	//for(size_t i = 0; i < m_noidProviders.size(); i++)
	//{
	//	m_noidProviders[i]->cleanup();
	//	m_noidProviders[i].setNull();
	//}

	//m_noidProviders.clear();
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_NPIProviderIFC::doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_FTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the createInstance pointer is set, then assume it's an instance
		// provider
		if(pProv->fp_createInstance)
		{
			env->getLogger()->logDebug(format("OW_NPIProviderIFC found instance"
				" provider %1", provIdString));

			return OW_InstanceProviderIFCRef(new OW_NPIInstanceProviderProxy(
				pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not an instance provider",
			provIdString));
	}

	return OW_InstanceProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray
OW_NPIProviderIFC::doGetIndicationExportProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	(void)env;
	//loadNoIdProviders(env);
	OW_IndicationExportProviderIFCRefArray rvra;
	//for(size_t i = 0; i < m_noidProviders.size(); i++)
	//{
	//	OW_CppProviderBaseIFCRef pProv = m_noidProviders[i];
	//	if(pProv->isIndicationExportProvider())
	//	{
	//		rvra.append(
	//			OW_IndicationExportProviderIFCRef(new
	//				OW_CppIndicationExportProviderProxy(
	//					pProv.cast_to<OW_CppIndicationExportProviderIFC>())));
	//	}
	//}

	return rvra;
}

//////////////////////////////////////////////////////////////////////////////
OW_PolledProviderIFCRefArray
OW_NPIProviderIFC::doGetPolledProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	(void)env;
	//loadNoIdProviders(env);
	OW_PolledProviderIFCRefArray rvra;
	//for(size_t i = 0; i < m_noidProviders.size(); i++)
	//{
	//	OW_CppProviderBaseIFCRef pProv = m_noidProviders[i];
	//	if(pProv->isPolledProvider())
	//	{
	//		rvra.append(
	//			OW_PolledProviderIFCRef(new
	//				OW_CppPolledProviderProxy(pProv.cast_to<OW_CppPolledProviderIFC>())));
	//	}
	//}

	return rvra;
}

//////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef
OW_NPIProviderIFC::doGetMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_FTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// it's a method provider if the invokeMethod function pointer is not
		// NULL
		if(pProv->fp_invokeMethod)
		{
			env->getLogger()->logDebug(format("OW_NPIProviderIFC found method provider %1",
				provIdString));

			return OW_MethodProviderIFCRef(
				new OW_NPIMethodProviderProxy(pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not a method provider",
			provIdString));
	}

	return OW_MethodProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_NPIProviderIFC::doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	(void)env;
	(void)provIdString;
	/* not supported yet
	OW_CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		if(pProv->isPropertyProvider())
		{
			env->getLogger()->logDebug(format("OW_NPIProviderIFC found property provider %1",
				provIdString));

			return OW_PropertyProviderIFCRef(new OW_NPIPropertyProviderProxy(
				pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not a property provider",
			provIdString));
	}
	*/

	return OW_PropertyProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_NPIProviderIFC::doGetAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_FTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the associatorNames function pointer is not 0, we know it's an
		// associator provider
		if(pProv->fp_associatorNames)
		{
			env->getLogger()->logDebug(format("OW_NPIProviderIFC found associator provider %1",
				provIdString));

			return OW_AssociatorProviderIFCRef(new
				OW_NPIAssociatorProviderProxy(pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not an associator provider",
			provIdString));
	}

	return OW_AssociatorProviderIFCRef(0);
}

//////////////////////////////////////////////////////////////////////////////
// void
// OW_NPIProviderIFC::loadNoIdProviders(const OW_ProviderEnvironmentIFCRef& env)
// {
//     OW_MutexLock ml(m_guard);
//
//     if(m_loadDone)
//     {
//         return;
//     }
//
//     m_loadDone = true;
//
//     OW_String libPath = env->getConfigItem(OW_ConfigOpts::CPPIFC_PROV_LOC_opt);
//
//     if(libPath.length() == 0)
//     {
//         libPath = DEFAULT_CPP_PROVIDER_LOCATION;
//     }
//
//     OW_SharedLibraryLoaderRef ldr =
//         OW_SharedLibraryLoader::createSharedLibraryLoader();
//
//     if(ldr.isNull())
//     {
//         env->getLogger()->logError("C++ provider ifc failed to get shared lib loader");
//         return;
//     }
//
//     OW_StringArray dirEntries;
//     if(!OW_FileSystem::getDirectoryContents(libPath, dirEntries))
//     {
//         env->getLogger()->logError(format("C++ provider ifc failed getting contents of "
//             "directory: %1", libPath));
//         return;
//     }
//
//     for(size_t i = 0; i < dirEntries.size(); i++)
//     {
//         if(!dirEntries[i].endsWith(".so"))
//         {
//             continue;
//         }
//
//         OW_String libName = libPath;
//         libName += OW_FILENAME_SEPARATOR;
//         libName += dirEntries[i];
//         OW_SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
//             env->getLogger());
//
//         if(theLib.isNull())
//         {
//             env->getLogger()->logError(format("C++ provider ifc failed to load"
//                 " library: %1", libName));
//             continue;
//         }
//
//         versionFunc_t versFunc;
//         if(!OW_SharedLibrary::getFunctionPointer(theLib, "getOWVersion",
//             versFunc))
//         {
//             env->getLogger()->logError(format("C++ provider ifc failed getting"
//                 " function pointer to \"getOWVersion\" from library: %1",
//                 libName));
//             continue;
//         }
//
//         const char* strVer = (*versFunc)();
//         if(strcmp(strVer, OW_VERSION))
//         {
//             env->getLogger()->logError(format("C++ provider ifc got invalid version from "
//                 "provider: %1", strVer));
//             continue;
//         }
//
//         ProviderCreationFunc createProvider;
//         if(!OW_SharedLibrary::getFunctionPointer(theLib, OW_String(CREATIONFUNC),
//             createProvider))
//         {
//             env->getLogger()->logError(format("C++ provider ifc: Libary %1 does not contain"
//                 " %2 function", libName, CREATIONFUNC));
//             continue;
//         }
//
//         OW_CppProviderBaseIFC* pProv = (*createProvider)();
//         if(!pProv)
//         {
//             env->getLogger()->logError(format("C++ provider ifc: Libary %1 - %2 returned null"
//                 " provider", libName, CREATIONFUNC));
//             continue;
//         }
//
//
//         OW_CppPolledProviderIFC* p_itp = (pProv->isPolledProvider())
//             ? static_cast<OW_CppPolledProviderIFC*>(pProv) : 0;
//
//         OW_CppIndicationExportProviderIFC* p_iep = 0;
//
//         if (!p_itp)
//         {
//             p_iep = (pProv->isIndicationExportProvider())
//                 ? static_cast<OW_CppIndicationExportProviderIFC*>(pProv) : 0;
//         }
//
//         if (p_itp || p_iep)
//         {
//             if(p_iep)
//             {
//                 env->getLogger()->logDebug(format("C++ provider ifc loaded indication export"
//                     " provider from lib: %1 - initializing", libName));
//             }
//             else
//             {
//                 env->getLogger()->logDebug(format("C++ provider ifc loaded polled provider from "
//                     "lib: %1 - initializing", libName));
//             }
//
//             pProv->initialize(env);
//
//             m_noidProviders.append(OW_CppProviderBaseIFCRef(theLib, pProv));
//         }
//         else
//         {
//             delete pProv;
//         }
//     }
// }

//////////////////////////////////////////////////////////////////////////////
OW_FTABLERef
OW_NPIProviderIFC::getProvider(
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
		OW_ConfigOpts::CPPIFC_PROV_LOC_opt);

	if(libPath.length() == 0)
	{
		libPath = DEFAULT_CPP_PROVIDER_LOCATION;
	}

	OW_SharedLibraryLoaderRef ldr =
		OW_SharedLibraryLoader::createSharedLibraryLoader();

	if(ldr.isNull())
	{
		env->getLogger()->logError("NPI:C++ provider ifc failed to get shared lib loader");
		return OW_FTABLERef();
	}

	OW_String libName(libPath);
	libName += OW_FILENAME_SEPARATOR;
	libName += "lib";
	libName += provId;
	libName += ".so";

	env->getLogger()->logDebug(format("OW_NPIProviderIFC::getProvider loading library: %1",
		libName));

	OW_SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
		env->getLogger());

	if(theLib.isNull())
	{
		env->getLogger()->logError(format("C++ provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return OW_FTABLERef();
	}

	::FP_INIT_FT createProvider;
	OW_String creationFuncName = provId + "_initFunctionTable";

	if(!OW_SharedLibrary::getFunctionPointer(theLib, creationFuncName, createProvider))
	{
		env->getLogger()->logError(format("C++ provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		return OW_FTABLERef();
	}

	::FTABLE fTable = (*createProvider)();

	if(!fTable.fp_initialize)
	{
		env->getLogger()->logError(format("C++ provider ifc: Libary %1 - %2 returned null"
			" initialize function pointer in function table", libName, creationFuncName));
		return OW_FTABLERef();
	}

	env->getLogger()->logDebug(format("C++ provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, provId));

	::CIMOMHandle ch = {0}; // CIMOMHandle parameter is meaningless, there is
	// nothing the provider can do with it, so we'll just pass in 0

	//OW_Reference<NPIEnv> npiHandle(); // TODO: createEnv(...);
	fTable.fp_initialize(0/*npiHandle.getPtr()*/, ch );	// Let provider initialize itself

	env->getLogger()->logDebug(format("C++ provider ifc: provider %1 loaded and initialized",
		provId));

	m_provs[provId] = OW_FTABLERef(theLib, new ::FTABLE(fTable));

	return m_provs[provId];
}

OW_PROVIDERIFCFACTORY(OW_NPIProviderIFC)

