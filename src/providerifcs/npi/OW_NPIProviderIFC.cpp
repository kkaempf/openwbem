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
#include "OW_NPIProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_NPIInstanceProviderProxy.hpp"
#include "OW_NPIMethodProviderProxy.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_NPIAssociatorProviderProxy.hpp"
#endif
#include "OW_NPIPolledProviderProxy.hpp"
#include "OW_NPIIndicationProviderProxy.hpp"

namespace OW_NAMESPACE
{

//typedef NPIProviderBaseIFC* (*ProviderCreationFunc)();
// the closest approximation of NPIProviderBaseIFCRef is NPIFTABLE
typedef FTABLERef* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();
const char* const NPIProviderIFC::CREATIONFUNC = "createProvider";
namespace
{
	const String COMPONENT_NAME("ow.provider.npi.ifc");
}

//////////////////////////////////////////////////////////////////////////////
NPIProviderIFC::NPIProviderIFC()
	: ProviderIFCBaseIFC()
	, m_provs()
	, m_guard()
	, m_noidProviders()
	, m_loadDone(false)
{
}
//////////////////////////////////////////////////////////////////////////////
NPIProviderIFC::~NPIProviderIFC()
{
	try
	{
		ProviderMap::iterator it = m_provs.begin();
		//Reference<NPIenv> npiHandle(); // TODO: createEnv(...);
		while (it != m_provs.end())
		{
			it->second->fp_cleanup(0); // TODO: FIX this. m_npiHandle);
			it->second.setNull();
			it++;
		}
	
		m_provs.clear();
	
		for (size_t i = 0; i < m_noidProviders.size(); i++)
		{
			m_noidProviders[i]->fp_cleanup(0);
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
NPIProviderIFC::doInit(const ProviderEnvironmentIFCRef&,
	InstanceProviderInfoArray&,
	SecondaryInstanceProviderInfoArray&,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	AssociatorProviderInfoArray&,
#endif
	MethodProviderInfoArray&,
	IndicationProviderInfoArray&)
{
	return;
}
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
NPIProviderIFC::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	FTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		// if the createInstance pointer is set, then assume it's an instance
		// provider
		if (pProv->fp_createInstance)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPIProviderIFC found instance"
				" provider %1", provIdString));
			return InstanceProviderIFCRef(new NPIInstanceProviderProxy(
				pProv));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
NPIProviderIFC::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef& env)
{
	//loadNoIdProviders(env);
	IndicationExportProviderIFCRefArray rvra;
	//for (size_t i = 0; i < m_noidProviders.size(); i++)
	//{
	//	CppProviderBaseIFCRef pProv = m_noidProviders[i];
	//	if (pProv->isIndicationExportProvider())
	//	{
	//		rvra.append(
	//			IndicationExportProviderIFCRef(new
	//				CppIndicationExportProviderProxy(
	//					pProv.cast_to<CppIndicationExportProviderIFC>())));
	//	}
	//}
	return rvra;
}
//////////////////////////////////////////////////////////////////////////////
PolledProviderIFCRefArray
NPIProviderIFC::doGetPolledProviders(const ProviderEnvironmentIFCRef& env)
{
	// NPI doesn't support polled providers
	PolledProviderIFCRefArray rvra;
	return rvra;
}
//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
NPIProviderIFC::doGetMethodProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	FTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		// it's a method provider if the invokeMethod function pointer is not
		// NULL
		if (pProv->fp_invokeMethod)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPIProviderIFC found method provider %1",
				provIdString));
			return MethodProviderIFCRef(
				new NPIMethodProviderProxy(pProv));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not a method provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
NPIProviderIFC::doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	FTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		// if the associatorNames function pointer is not 0, we know it's an
		// associator provider
		if (pProv->fp_associatorNames)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPIProviderIFC found associator provider %1",
				provIdString));
			return AssociatorProviderIFCRef(new
				NPIAssociatorProviderProxy(pProv));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an associator provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#endif
//////////////////////////////////////////////////////////////////////////////
IndicationProviderIFCRef
NPIProviderIFC::doGetIndicationProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	FTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		// if the indicationNames function pointer is not 0, we know it's an
		// indication provider
		if (pProv->fp_activateFilter)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPIProviderIFC found indication provider %1",
				provIdString));
			return IndicationProviderIFCRef(new
				NPIIndicationProviderProxy(pProv));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an indication provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
void
NPIProviderIFC::loadNoIdProviders(const ProviderEnvironmentIFCRef& env)
{

   MutexLock ml(m_guard);
   if (m_loadDone)
   {
	  return;
   }
   m_loadDone = true;
   String libPath = env->getConfigItem(ConfigOpts::NPIPROVIFC_PROV_LOCATION_opt, OW_DEFAULT_NPIPROVIFC_PROV_LOCATION);
   SharedLibraryLoaderRef ldr =
	  SharedLibraryLoader::createSharedLibraryLoader();
   if (!ldr)
   {
	  OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "NPI provider ifc failed to get shared lib loader");
	  return;
   }
   StringArray dirEntries;
   if (!FileSystem::getDirectoryContents(libPath, dirEntries))
   {
	  OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc failed getting contents of "
		 "directory: %1", libPath));
	  return;
   }
   for (size_t i = 0; i < dirEntries.size(); i++)
   {
	  if (!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
	  {
		 continue;
	  }
#ifdef OW_DARWIN
          if (dirEntries[i].indexOf(OW_VERSION) != String::npos)
          {
                 continue;
          }
#endif // OW_DARWIN
	  String libName = libPath;
	  libName += OW_FILENAME_SEPARATOR;
	  libName += dirEntries[i];
	  SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
			env->getLogger(COMPONENT_NAME));
	  String guessProvId = dirEntries[i].substring(3, dirEntries[i].length() - (strlen(OW_SHAREDLIB_EXTENSION) + 3));
	  if (!theLib)
	  {
		 OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("NPI provider %1 ifc failed to load"
				   " library: %2", guessProvId, libName));
		 continue;
	  }
	::FP_INIT_FT createProvider;
	String creationFuncName = guessProvId + "_initFunctionTable";
	if (!theLib->getFunctionPointer(creationFuncName, createProvider))
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		continue;
	}
	::FTABLE fTable_ = (*createProvider)();
	if (!fTable_.fp_initialize)
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc: Libary %1 - %2 returned null"
			" initialize function pointer in function table", libName, creationFuncName));
		continue;
	}
		// only initialize polled and indicationexport providers
	// since NPI doesn't support indicationexport providers ....
	if (!fTable_.fp_activateFilter) continue;
	//
		// else it must be a polled provider - initialize it
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, guessProvId));
	::CIMOMHandle ch = {0}; // CIMOMHandle parameter is meaningless, there is
	// nothing the provider can do with it, so we'll just pass in 0
	//Reference<NPIEnv> npiHandle(); // TODO: createEnv(...);
		// Garbage Collection support
	NPIFTABLE fTable;
		memcpy(&fTable, &fTable_, sizeof(::FTABLE));
		fTable.npicontext = new NPIContext;
		fTable.npicontext->scriptName = NULL;
	::NPIHandle _npiHandle = {0, 0, 0, 0, fTable.npicontext};
	fTable.fp_initialize(&_npiHandle, ch );	// Let provider initialize itself
	// take care of the errorOccurred field - buggy provider or perl script
	if (_npiHandle.errorOccurred)
	{
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc loaded library %1. Initialize failed"
		" for provider %2", libName, guessProvId));
		delete ((NPIContext *)fTable.npicontext);
		continue;
	}
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc: provider %1 loaded and initialized",
		guessProvId));
	//::NPIFTABLE * nf = new ::NPIFTABLE();
	//* nf = fTable;
		m_noidProviders.append(FTABLERef(theLib, new NPIFTABLE(fTable)));
		//m_noidProviders.append(FTABLERef(theLib, nf));
	}
}
//////////////////////////////////////////////////////////////////////////////
FTABLERef
NPIProviderIFC::getProvider(
	const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	MutexLock ml(m_guard);
	String provId(provIdString);
	ProviderMap::iterator it = m_provs.find(provId);
	if (it != m_provs.end())
	{
		return it->second;
	}
	String libPath = env->getConfigItem(
		ConfigOpts::NPIPROVIFC_PROV_LOCATION_opt, OW_DEFAULT_NPIPROVIFC_PROV_LOCATION);
	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "NPI: provider ifc failed to get shared lib loader");
		return FTABLERef();
	}
	String libName(libPath);
	libName += OW_FILENAME_SEPARATOR;
	libName += "lib";
	libName += provId;
	libName += OW_SHAREDLIB_EXTENSION;
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPIProviderIFC::getProvider loading library: %1",
		libName));
	SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
		env->getLogger(COMPONENT_NAME));
	if (!theLib)
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return FTABLERef();
	}
	::FP_INIT_FT createProvider;
	String creationFuncName = provId + "_initFunctionTable";
	if (!theLib->getFunctionPointer(creationFuncName, createProvider))
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		return FTABLERef();
	}
	::FTABLE fTable_ = (*createProvider)();
	//NPIFTABLE fTable = fTable_;
	NPIFTABLE fTable;
	memcpy(&fTable, &fTable_, sizeof(::FTABLE));
	fTable.npicontext = new NPIContext;
	fTable.npicontext->scriptName = NULL;
	if (!fTable.fp_initialize)
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc: Libary %1 - %2 returned null"
			" initialize function pointer in function table", libName, creationFuncName));
		delete ((NPIContext *)fTable.npicontext);
		return FTABLERef();
	}
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, provId));
	::CIMOMHandle ch = {0}; // CIMOMHandle parameter is meaningless, there is
	// nothing the provider can do with it, so we'll just pass in 0
	//Reference<NPIEnv> npiHandle(); // TODO: createEnv(...);
	::NPIHandle _npiHandle = { 0, 0, 0, 0, fTable.npicontext};
	fTable.fp_initialize(&_npiHandle, ch ); // Let provider initialize itself
	// take care of the errorOccurred field
	// that might indicate a buggy provider
	if (_npiHandle.errorOccurred)
	{
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc loaded library %1. Initialize failed"
			" for provider %2", libName, provId));
		delete ((NPIContext *)fTable.npicontext);
		return FTABLERef();
	}
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("NPI provider ifc: provider %1 loaded and initialized",
		provId));
	m_provs[provId] = FTABLERef(theLib, new NPIFTABLE(fTable));
	return m_provs[provId];
}
} // end namespace OW_NAMESPACE

OW_PROVIDERIFCFACTORY(OpenWBEM::NPIProviderIFC, npi)

