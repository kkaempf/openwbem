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
#include "OW_PerlProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_PerlInstanceProviderProxy.hpp"
#include "OW_PerlMethodProviderProxy.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_PerlAssociatorProviderProxy.hpp"
#endif
#include "OW_PerlPolledProviderProxy.hpp"
#include "OW_PerlIndicationProviderProxy.hpp"
// BMMU
#include <iostream>

namespace OpenWBEM
{

//typedef PerlProviderBaseIFC* (*ProviderCreationFunc)();
// the closest approximation of PerlProviderBaseIFCRef is ::PerlFTABLE
//  as defined in FTABLERef.hpp
typedef FTABLERef* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();
const char* const PerlProviderIFC::CREATIONFUNC = "createProvider";
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
		//Reference<Perlenv> npiHandle(); // TODO: createEnv(...);
		while(it != m_provs.end())
		{
			it->second->fp_cleanup(0); // TODO: FIX this. m_npiHandle);
			it->second.setNull();
			it++;
		}
	
		m_provs.clear();
	
		for(size_t i = 0; i < m_noidProviders.size(); i++)
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
PerlProviderIFC::doInit(const ProviderEnvironmentIFCRef& env,
	InstanceProviderInfoArray& i,
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
	if(pProv)
	{
		// if the createInstance pointer is set, then assume it's an instance
		// provider
		if(pProv->fp_createInstance)
		{
			env->getLogger()->logDebug(format("PerlProviderIFC found instance"
				" provider %1", provIdString));
			return InstanceProviderIFCRef(new PerlInstanceProviderProxy(
				pProv));
		}
		env->getLogger()->logError(format("Provider %1 is not an instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
PerlProviderIFC::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef& env)
{
	(void)env;
	//loadNoIdProviders(env);
	IndicationExportProviderIFCRefArray rvra;
	//for(size_t i = 0; i < m_noidProviders.size(); i++)
	//{
	//	CppProviderBaseIFCRef pProv = m_noidProviders[i];
	//	if(pProv->isIndicationExportProvider())
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
	(void)env;
	//loadNoIdProviders(env);
	PolledProviderIFCRefArray rvra;
	//for(size_t i = 0; i < m_noidProviders.size(); i++)
	//{
	//	FTABLERef pProv = m_noidProviders[i];
	//
	//	if(pProv->fp_activateFilter)
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
	if(pProv)
	{
		// it's a method provider if the invokeMethod function pointer is not
		// NULL
		if(pProv->fp_invokeMethod)
		{
			env->getLogger()->logDebug(format("PerlProviderIFC found method provider %1",
				provIdString));
			return MethodProviderIFCRef(
				new PerlMethodProviderProxy(pProv));
		}
		env->getLogger()->logError(format("Provider %1 is not a method provider",
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
	if(pProv)
	{
		// if the associatorNames function pointer is not 0, we know it's an
		// associator provider
		if(pProv->fp_associatorNames)
		{
			env->getLogger()->logDebug(format("PerlProviderIFC found associator provider %1",
				provIdString));
			return AssociatorProviderIFCRef(new
				PerlAssociatorProviderProxy(pProv));
		}
		env->getLogger()->logError(format("Provider %1 is not an associator provider",
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
	std::cout << "Get IndicationProvider for Id " << provIdString << std::endl;
	FTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the indicationNames function pointer is not 0, we know it's an
		// indication provider
		if(pProv->fp_activateFilter)
		{
			env->getLogger()->logDebug(format("PerlProviderIFC found indication provider %1",
				provIdString));
			return IndicationProviderIFCRef(new
				PerlIndicationProviderProxy(pProv));
		}
		env->getLogger()->logError(format("Provider %1 is not an indication provider",
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
	if(m_loadDone)
	{
		return;
	}
	m_loadDone = true;
	String libPath = env->getConfigItem(
		ConfigOpts::PERLIFC_PROV_LOC_opt, OW_DEFAULT_PERL_PROVIDER_LOCATION);
	SharedLibraryLoaderRef ldr =
		 SharedLibraryLoader::createSharedLibraryLoader();
	if(ldr.isNull())
	{
		env->getLogger()->logError("Perl provider ifc failed to get shared lib loader");
		return;
	}
	StringArray dirEntries;
   	if(!FileSystem::getDirectoryContents(libPath, dirEntries))
	{
		env->getLogger()->logError(format("Perl provider ifc "
			"failed getting contents of directory: %1", libPath));
		return;
	}
	for(size_t i = 0; i < dirEntries.size(); i++)
	{
		if(!dirEntries[i].endsWith(".pl"))
		{
			continue;
		}
		String libName = libPath;
		libName += OW_FILENAME_SEPARATOR;
		libName += String("libperlProvider.so");
		SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
			env->getLogger());
		String guessProvId = dirEntries[i];
		if(theLib.isNull())
		{
			env->getLogger()->logError(format("Perl provider %1 "
				"failed to load library: %2",
				guessProvId, libName));
			continue;
		}
		NPIFP_INIT_FT createProvider;
		String creationFuncName = "perlProvider_initFunctionTable";
		if(!SharedLibrary::getFunctionPointer(theLib, creationFuncName, createProvider))
		{
			env->getLogger()->logError(format("Perl provider ifc: "
				"Library %1 does not contain %2 function",
				libName, creationFuncName));
			continue;
		}
		NPIFTABLE fTable = (*createProvider)();
		fTable.npicontext = new NPIContext;
		fTable.npicontext->scriptName = guessProvId.allocateCString();
		if (!fTable.fp_initialize)
		{
			env->getLogger()->logError(format("Perl provider ifc: "
			"Library %1 - initialize returned null", libName));
			delete (fTable.npicontext->scriptName);
			delete ((NPIContext *)fTable.npicontext);
			continue;
		}
		// now register the perl script for every type 
		// without trying to call 
		// TODO: implement check for perl subroutines
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
		IndicationProviderInfoEntry e("CIM_InstCreation");
		// BMMU used for testing
		//e.classes.push_back("CIM_LocalFileSystem");
		//e.classes.push_back("Linux_Ext2FileSystem");
		ind_info.addInstrumentedClass(e);
		e.indicationName = "CIM_InstModification";
		ind_info.addInstrumentedClass(e);
		e.indicationName = "CIM_InstDeletion";
		ind_info.addInstrumentedClass(e);
		e.indicationName = "CIM_InstIndication";
		ind_info.addInstrumentedClass(e);
		e.indicationName = "CIM_Indication";
		ind_info.addInstrumentedClass(e);
		ind_info.setProviderName(guessProvId);
		indicationProviderInfo.push_back(ind_info);
		continue;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
PerlProviderIFC::loadNoIdProviders(const ProviderEnvironmentIFCRef& env)
{
   
   env->getLogger()->logError("LoadNoID Perl providers");
   MutexLock ml(m_guard);
   if(m_loadDone)
   {
      return;
   }
   m_loadDone = true;
   String libPath = env->getConfigItem(ConfigOpts::PERLIFC_PROV_LOC_opt, OW_DEFAULT_PERL_PROVIDER_LOCATION);
   env->getLogger()->logError("LoadNoIDproviders 2");
   SharedLibraryLoaderRef ldr =
      SharedLibraryLoader::createSharedLibraryLoader();
   if(ldr.isNull())
   {
      env->getLogger()->logError("Perl provider ifc failed to get shared lib loader");
      return;
   }
   StringArray dirEntries;
   if(!FileSystem::getDirectoryContents(libPath, dirEntries))
   {
      env->getLogger()->logError(format("Perl provider ifc failed getting contents of "
         "directory: %1", libPath));
      return;
   }
   env->getLogger()->logError("LoadNoIDproviders 3");
   for(size_t i = 0; i < dirEntries.size(); i++)
   {
      if(!dirEntries[i].endsWith(".pl"))
      {
         continue;
      }
      String libName = libPath;
      libName += OW_FILENAME_SEPARATOR;
      //libName += dirEntries[i];
      libName += String("libperlProvider.so");
      SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
            env->getLogger());
      String guessProvId = dirEntries[i];
      if(theLib.isNull())
      {
         env->getLogger()->logError(format("Perl provider %1 ifc failed to load"
                   " library: %2", guessProvId, libName));
         continue;
      }
#if 0
      versionFunc_t versFunc;
      if(!SharedLibrary::getFunctionPointer(theLib, "getOWVersion",
             versFunc))
      {
         env->getLogger()->logError(format("Perl provider ifc failed getting"
                 " function pointer to \"getOWVersion\" from library: %1",
                 libName));
         continue;
      }
      const char* strVer = (*versFunc)();
      if(strcmp(strVer, VERSION))
      {
         env->getLogger()->logError(format("Perl provider ifc got invalid version from "
                 "provider: %1", strVer));
         continue;
      }
#endif
	NPIFP_INIT_FT createProvider;
	//String creationFuncName = guessProvId + "_initFunctionTable";
	String creationFuncName = "perlProvider_initFunctionTable";
   env->getLogger()->logError(format("LoadNoIDproviders 4b : %1", creationFuncName));
	if(!SharedLibrary::getFunctionPointer(theLib, creationFuncName, createProvider))
	{
		env->getLogger()->logError(format("Perl provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		continue;
	}
   env->getLogger()->logError("LoadNoIDproviders 5");
	NPIFTABLE fTable = (*createProvider)();
	fTable.npicontext = new NPIContext;
	
	fTable.npicontext->scriptName = guessProvId.allocateCString();
	if ((!fTable.fp_initialize)||(!fTable.fp_activateFilter))
	{
		env->getLogger()->logError(format("Perl provider ifc: Libary %1 - %2 returned null"
			" initialize function pointer in function table", libName, creationFuncName));
		delete (fTable.npicontext->scriptName);
	        delete ((NPIContext *)fTable.npicontext);
		continue;
	}
        // only initialize polled and indicationexport providers
	// since Perl doesn't support indicationexport providers ....
   env->getLogger()->logError("LoadNoIDproviders 6");
        // else it must be a polled provider - initialize it 
	env->getLogger()->logDebug(format("Perl provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, guessProvId));
	::CIMOMHandle ch = {0}; // CIMOMHandle parameter is meaningless, there is
	// nothing the provider can do with it, so we'll just pass in 0
	::NPIHandle _npiHandle = { 0, 0, 0, 0, fTable.npicontext};
	fTable.fp_initialize(&_npiHandle, ch );	// Let provider initialize itself
        // take care of the errorOccurred field
        // that might indicate a buggy perl script
        if (_npiHandle.errorOccurred)
        {
	         env->getLogger()->logDebug(format("Perl provider ifc loaded library %1. Initialize failed"
		" for provider %2", libName, guessProvId));
	        delete ((NPIContext *)fTable.npicontext);
		continue;
        }
	env->getLogger()->logDebug(format("Perl provider ifc: provider %1 loaded and initialized",
		guessProvId));
        //m_noidProviders.append(FTABLERef(theLib, new ::FTABLE(fTable)));
        m_noidProviders.append(FTABLERef(theLib, new NPIFTABLE(fTable)));
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
	if(it != m_provs.end())
	{
		return it->second;
	}
	String libPath = env->getConfigItem(
		ConfigOpts::PERLIFC_PROV_LOC_opt, OW_DEFAULT_PERL_PROVIDER_LOCATION);
	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if(ldr.isNull())
	{
		env->getLogger()->logError("Perl: provider ifc failed to get shared lib loader");
		return FTABLERef();
	}
	String libName(libPath);
	libName += OW_FILENAME_SEPARATOR;
	libName += "libperlProvider.so";
	env->getLogger()->logDebug(format("PerlProviderIFC::getProvider loading library: %1",
		libName));
	SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
		env->getLogger());
	if(theLib.isNull())
	{
		env->getLogger()->logError(format("Perl provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return FTABLERef();
	}
	NPIFP_INIT_FT createProvider;
	String creationFuncName = "perlProvider_initFunctionTable";
	if(!SharedLibrary::getFunctionPointer(theLib, creationFuncName, createProvider))
	{
		env->getLogger()->logError(format("Perl provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		return FTABLERef();
	}
	NPIFTABLE fTable = (*createProvider)();
	fTable.npicontext = new NPIContext;
	
	fTable.npicontext->scriptName = provId.allocateCString();
	if(!fTable.fp_initialize)
	{
		env->getLogger()->logError(format("Perl provider ifc: Libary %1 - %2 returned null"
			" initialize function pointer in function table", libName, creationFuncName));
	        delete ((NPIContext *)fTable.npicontext);
		return FTABLERef();
	}
	env->getLogger()->logDebug(format("Perl provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, provId));
	::CIMOMHandle ch = {0}; // CIMOMHandle parameter is meaningless, there is
	// nothing the provider can do with it, so we'll just pass in 0
	//Reference<PerlEnv> npiHandle(); // TODO: createEnv(...);
	::NPIHandle _npiHandle = { 0, 0, 0, 0, fTable.npicontext};
	fTable.fp_initialize(&_npiHandle, ch );	// Let provider initialize itself
        // take care of the errorOccurred field
        // that might indicate a buggy perl script
        if (_npiHandle.errorOccurred)
        {
	         env->getLogger()->logDebug(format("Perl provider ifc loaded library %1. Initialize failed"
		" for provider %2", libName, provId));
	        delete ((NPIContext *)fTable.npicontext);
		return FTABLERef();
        }
       
	env->getLogger()->logDebug(format("Perl provider ifc: provider %1 loaded and initialized (script %2)",
		provId, fTable.npicontext->scriptName));
	m_provs[provId] = FTABLERef(theLib, new NPIFTABLE(fTable));
	return m_provs[provId];
}

} // end namespace OpenWBEM

OW_PROVIDERIFCFACTORY(OpenWBEM::PerlProviderIFC)

