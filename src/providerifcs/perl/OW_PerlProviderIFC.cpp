/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_PerlProviderIFC.hpp"
#include "blocxx/SharedLibraryException.hpp"
#include "blocxx/SharedLibraryLoader.hpp"
#include "blocxx/Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/FileSystem.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_PerlInstanceProviderProxy.hpp"
#include "OW_PerlMethodProviderProxy.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_PerlAssociatorProviderProxy.hpp"
#endif
#include "OW_PerlIndicationProviderProxy.hpp"
// BMMU
#include <iostream>

namespace OW_NAMESPACE
{

//typedef PerlProviderBaseIFC* (*ProviderCreationFunc)();
// the closest approximation of PerlProviderBaseIFCRef is ::PerlFTABLE
//  as defined in FTABLERef.hpp
typedef FTABLERef* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();
const char* const PerlProviderIFC::CREATIONFUNC = "createProvider";
namespace
{
	const String COMPONENT_NAME("ow.provider.perlnpi.ifc");
}

//////////////////////////////////////////////////////////////////////////////
PerlProviderIFC::PerlProviderIFC()
	: ProviderIFCBaseIFC()
	, m_provs()
	, m_guard()
	, m_noidProviders()
	, m_loadDone(false)
{
}
//////////////////////////////////////////////////////////////////////////////
PerlProviderIFC::~PerlProviderIFC()
{
	try
	{
		ProviderMap::iterator it = m_provs.begin();
		//Reference<Perlenv> npiHandle(); /// @todo  createEnv(...);

		while (it != m_provs.end())
		{
			::NPIHandle _npiHandle = { 0, 0, 0, 0,
				it->second->npicontext};
			it->second->fp_cleanup(&_npiHandle);
			it->second.setNull();
			it++;
		}

		m_provs.clear();

		for (size_t i = 0; i < m_noidProviders.size(); i++)
		{
			::NPIHandle _npiHandle = { 0, 0, 0, 0,
				 m_noidProviders[i]->npicontext};
			m_noidProviders[i]->fp_cleanup(&_npiHandle);
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
PerlProviderIFC::doInit(const ProviderEnvironmentIFCRef& env,
	InstanceProviderInfoArray& i,
	SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	AssociatorProviderInfoArray& a,
#endif
	MethodProviderInfoArray& m,
	IndicationProviderInfoArray& ind)
{
	loadProviders(env, i, a, m,
		ind);
}
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
PerlProviderIFC::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	FTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		// if the createInstance pointer is set, then assume it's an instance
		// provider
		if (pProv->fp_createInstance)
		{
			BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("PerlProviderIFC found instance"
				" provider %1", provIdString));
			return InstanceProviderIFCRef(new PerlInstanceProviderProxy(
				pProv));
		}
		BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
PerlProviderIFC::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef& env)
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
PerlProviderIFC::doGetPolledProviders(const ProviderEnvironmentIFCRef& env)
{
	//loadNoIdProviders(env);
	PolledProviderIFCRefArray rvra;
	//for (size_t i = 0; i < m_noidProviders.size(); i++)
	//{
	//	FTABLERef pProv = m_noidProviders[i];
	//
	//	if (pProv->fp_activateFilter)
	//	{
	//		rvra.append(
	//			PolledProviderIFCRef(new
	//				PerlPolledProviderProxy(pProv)));
	//	}
	//}
	return rvra;
}
//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
PerlProviderIFC::doGetMethodProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	FTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		// it's a method provider if the invokeMethod function pointer is not
		// NULL
		if (pProv->fp_invokeMethod)
		{
			BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("PerlProviderIFC found method provider %1",
				provIdString));
			return MethodProviderIFCRef(
				new PerlMethodProviderProxy(pProv));
		}
		BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not a method provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
PerlProviderIFC::doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	FTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		// if the associatorNames function pointer is not 0, we know it's an
		// associator provider
		if (pProv->fp_associatorNames)
		{
			BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("PerlProviderIFC found associator provider %1",
				provIdString));
			return AssociatorProviderIFCRef(new
				PerlAssociatorProviderProxy(pProv));
		}
		BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an associator provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#endif
//////////////////////////////////////////////////////////////////////////////
IndicationProviderIFCRef
PerlProviderIFC::doGetIndicationProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
// BMMU
	//std::cout << "Get IndicationProvider for Id " << provIdString << std::endl;
	FTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		// if the indicationNames function pointer is not 0, we know it's an
		// indication provider
		if (pProv->fp_activateFilter)
		{
			BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("PerlProviderIFC found indication provider %1",
				provIdString));
			return IndicationProviderIFCRef(new
				PerlIndicationProviderProxy(pProv));
		}
		BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an indication provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
void
PerlProviderIFC::loadProviders(const ProviderEnvironmentIFCRef& env,
	InstanceProviderInfoArray& instanceProviderInfo,
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

	SharedLibraryLoaderRef ldr =
		 SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), "Perl provider ifc failed to get shared lib loader");
		return;
	}

	const StringArray libPaths = env->getMultiConfigItem(
		ConfigOpts::PERLPROVIFC_PROV_LOCATION_opt,
		String(OW_DEFAULT_PERLPROVIFC_PROV_LOCATION).tokenize(OW_PATHNAME_SEPARATOR),
		OW_PATHNAME_SEPARATOR);

	for (size_t libIdx = 0; libIdx < libPaths.size(); ++libIdx)
	{
		String libPath(libPaths[libIdx]);
		StringArray dirEntries;
		if (!FileSystem::getDirectoryContents(libPath, dirEntries))
		{
			BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc "
				"failed getting contents of directory: %1", libPath));
			return;
		}
		for (size_t i = 0; i < dirEntries.size(); i++)
		{
			if (!dirEntries[i].endsWith(".pl"))
			{
				continue;
			}
			String libName = libPath;
			libName += OW_FILENAME_SEPARATOR;
			libName += String("libperlProvider"OW_SHAREDLIB_EXTENSION);
			SharedLibraryRef theLib = ldr->loadSharedLibrary(libName);
			String guessProvId = dirEntries[i];
			if (!theLib)
			{
				BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider %1 "
					"failed to load library: %2",
					guessProvId, libName));
				continue;
			}
			NPIFP_INIT_FT createProvider;
			String creationFuncName = "perlProvider_initFunctionTable";
			if (!theLib->getFunctionPointer(creationFuncName, createProvider))
			{
				BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc: "
					"Library %1 does not contain %2 function",
					libName, creationFuncName));
				continue;
			}
			NPIFTABLE fTable = (*createProvider)();
			fTable.npicontext = new NPIContext;
			fTable.npicontext->scriptName = guessProvId.allocateCString();
			if (!fTable.fp_initialize)
			{
				BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc: "
				"Library %1 - initialize returned null", libName));
				delete (fTable.npicontext->scriptName);
				delete ((NPIContext *)fTable.npicontext);
				continue;
			}

			::NPIHandle _npiHandle = { 0, 0, 0, 0, fTable.npicontext};
					//NPIHandleFreer nhf(_npiHandle);
			::CIMOMHandle ch;

			//char * classList = (* (fTable.fp_initialize))(
			//	&_npiHandle, ch);
			StringArray classList = String((* (fTable.fp_initialize))(
				&_npiHandle, ch)).tokenize(",");

			// now register the perl script for every type
			// without trying to call
			/// @todo  implement check for perl subroutines
			InstanceProviderInfo inst_info;
			inst_info.setProviderName(guessProvId);
			instanceProviderInfo.push_back(inst_info);
	#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			AssociatorProviderInfo assoc_info;
			assoc_info.setProviderName(guessProvId);
			associatorProviderInfo.push_back(assoc_info);
	#endif
			MethodProviderInfo meth_info;
			meth_info.setProviderName(guessProvId);
			methodProviderInfo.push_back(meth_info);

			IndicationProviderInfo ind_info;
			if (! classList.empty())
			{
				IndicationProviderInfoEntry e("CIM_InstCreation");
				for (unsigned int cnt = 0; cnt < classList.size();
					cnt++)
				{
					e.classes.push_back(classList[cnt]);
				}
				ind_info.addInstrumentedClass(e);
				e.indicationName = "CIM_InstModification";
				ind_info.addInstrumentedClass(e);
				e.indicationName = "CIM_InstDeletion";
				ind_info.addInstrumentedClass(e);
				ind_info.setProviderName(guessProvId);
				indicationProviderInfo.push_back(ind_info);
			}
			continue;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
PerlProviderIFC::loadNoIdProviders(const ProviderEnvironmentIFCRef& env)
{

   MutexLock ml(m_guard);
   if (m_loadDone)
   {
	  return;
   }
   m_loadDone = true;
   SharedLibraryLoaderRef ldr =
	  SharedLibraryLoader::createSharedLibraryLoader();
   if (!ldr)
   {
	  BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), "Perl provider ifc failed to get shared lib loader");
	  return;
   }

   const StringArray libPaths = env->getMultiConfigItem(
	   ConfigOpts::PERLPROVIFC_PROV_LOCATION_opt,
	   String(OW_DEFAULT_PERLPROVIFC_PROV_LOCATION).tokenize(OW_PATHNAME_SEPARATOR),
	   OW_PATHNAME_SEPARATOR);

   for (size_t libIdx = 0; libIdx < libPaths.size(); ++libIdx)
   {
	   String libPath(libPaths[libIdx]);
	   StringArray dirEntries;
	   if (!FileSystem::getDirectoryContents(libPath, dirEntries))
	   {
		  BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc failed getting contents of "
			 "directory: %1", libPath));
		  return;
	   }
	   for (size_t i = 0; i < dirEntries.size(); i++)
	   {
		  if (!dirEntries[i].endsWith(".pl"))
		  {
			 continue;
		  }
		  String libName = libPath;
		  libName += OW_FILENAME_SEPARATOR;
		  //libName += dirEntries[i];
		  libName += String("libperlProvider"OW_SHAREDLIB_EXTENSION);
		  SharedLibraryRef theLib = ldr->loadSharedLibrary(libName);
		  String guessProvId = dirEntries[i];
		  if (!theLib)
		  {
			 BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider %1 ifc failed to load"
					   " library: %2", guessProvId, libName));
			 continue;
		  }
	#if 0
		  versionFunc_t versFunc;
		  if (!theLib->getFunctionPointer("getOWVersion",
				 versFunc))
		  {
			 BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc failed getting"
					 " function pointer to \"getOWVersion\" from library: %1",
					 libName));
			 continue;
		  }
		  const char* strVer = (*versFunc)();
		  if (strcmp(strVer, VERSION))
		  {
			 BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc got invalid version from "
					 "provider: %1", strVer));
			 continue;
		  }
	#endif
		NPIFP_INIT_FT createProvider;
		//String creationFuncName = guessProvId + "_initFunctionTable";
		String creationFuncName = "perlProvider_initFunctionTable";
		if (!theLib->getFunctionPointer(creationFuncName, createProvider))
		{
			BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc: Libary %1 does not contain"
				" %2 function", libName, creationFuncName));
			continue;
		}
		NPIFTABLE fTable = (*createProvider)();
		fTable.npicontext = new NPIContext;

		fTable.npicontext->scriptName = guessProvId.allocateCString();
		if ((!fTable.fp_initialize)||(!fTable.fp_activateFilter))
		{
			BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc: Libary %1 - %2 returned null"
				" initialize function pointer in function table", libName, creationFuncName));
			delete (fTable.npicontext->scriptName);
				delete ((NPIContext *)fTable.npicontext);
			continue;
		}
			// only initialize polled and indicationexport providers
		// since Perl doesn't support indicationexport providers ....
			// else it must be a polled provider - initialize it
		BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc loaded library %1. Calling initialize"
			" for provider %2", libName, guessProvId));
		::CIMOMHandle ch = {0}; // CIMOMHandle parameter is meaningless, there is
		// nothing the provider can do with it, so we'll just pass in 0
		::NPIHandle _npiHandle = { 0, 0, 0, 0, fTable.npicontext};
		fTable.fp_initialize(&_npiHandle, ch );	// Let provider initialize itself
			// take care of the errorOccurred field
			// that might indicate a buggy perl script
			if (_npiHandle.errorOccurred)
			{
				 BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc loaded library %1. Initialize failed"
			" for provider %2", libName, guessProvId));
				delete ((NPIContext *)fTable.npicontext);
			continue;
			}
		BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc: provider %1 loaded and initialized",
			guessProvId));
			//m_noidProviders.append(FTABLERef(theLib, new ::FTABLE(fTable)));
			m_noidProviders.append(FTABLERef(theLib, new NPIFTABLE(fTable)));
		}
   }
}
//////////////////////////////////////////////////////////////////////////////
FTABLERef
PerlProviderIFC::getProvider(
	const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	MutexLock ml(m_guard);
	String provId(provIdString);
	ProviderMap::iterator it = m_provs.find(provId);
	if (it != m_provs.end())
	{
		return it->second;
	}
	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), "Perl: provider ifc failed to get shared lib loader");
		return FTABLERef();
	}
	const StringArray libPaths = env->getMultiConfigItem(
		ConfigOpts::PERLPROVIFC_PROV_LOCATION_opt,
		String(OW_DEFAULT_PERLPROVIFC_PROV_LOCATION).tokenize(OW_PATHNAME_SEPARATOR),
		OW_PATHNAME_SEPARATOR);

	for (size_t libIdx = 0; libIdx < libPaths.size(); ++libIdx)
	{
		String libPath(libPaths[libIdx]);
		String libName(libPath);
		libName += OW_FILENAME_SEPARATOR;
		libName += "libperlProvider"OW_SHAREDLIB_EXTENSION;
		BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("PerlProviderIFC::getProvider loading library: %1",
			libName));

		if (!FileSystem::exists(libName))
		{
			continue;
		}

		SharedLibraryRef theLib = ldr->loadSharedLibrary(libName);
		if (!theLib)
		{
			BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc failed to load library: %1 "
				"for provider id %2", libName, provId));
			return FTABLERef();
		}
		NPIFP_INIT_FT createProvider;
		String creationFuncName = "perlProvider_initFunctionTable";
		if (!theLib->getFunctionPointer(creationFuncName, createProvider))
		{
			BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc: Libary %1 does not contain"
				" %2 function", libName, creationFuncName));
			return FTABLERef();
		}
		NPIFTABLE fTable = (*createProvider)();
		fTable.npicontext = new NPIContext;

		fTable.npicontext->scriptName = provId.allocateCString();
		if (!fTable.fp_initialize)
		{
			BLOCXX_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc: Libary %1 - %2 returned null"
				" initialize function pointer in function table", libName, creationFuncName));
				delete ((NPIContext *)fTable.npicontext);
			return FTABLERef();
		}
		BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc loaded library %1. Calling initialize"
			" for provider %2", libName, provId));
		::CIMOMHandle ch = {0}; // CIMOMHandle parameter is meaningless, there is
		// nothing the provider can do with it, so we'll just pass in 0
		//Reference<PerlEnv> npiHandle(); /// @todo  createEnv(...);
		::NPIHandle _npiHandle = { 0, 0, 0, 0, fTable.npicontext};
		fTable.fp_initialize(&_npiHandle, ch );	// Let provider initialize itself
			// take care of the errorOccurred field
			// that might indicate a buggy perl script
			if (_npiHandle.errorOccurred)
			{
				 BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc loaded library %1. Initialize failed"
			" for provider %2", libName, provId));
				delete ((NPIContext *)fTable.npicontext);
			return FTABLERef();
			}

		BLOCXX_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("Perl provider ifc: provider %1 loaded and initialized (script %2)",
			provId, fTable.npicontext->scriptName));
		m_provs[provId] = FTABLERef(theLib, new NPIFTABLE(fTable));
		return m_provs[provId];
	}
	return FTABLERef();
}

} // end namespace OW_NAMESPACE

OW_PROVIDERIFCFACTORY(OpenWBEM::PerlProviderIFC, perl)

