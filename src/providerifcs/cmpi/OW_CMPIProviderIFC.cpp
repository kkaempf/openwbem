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
#include "OW_CMPIProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_CMPIInstanceProviderProxy.hpp"
#include "OW_CMPIMethodProviderProxy.hpp"
#include "OW_CMPIAssociatorProviderProxy.hpp"
#include "OW_CMPIPolledProviderProxy.hpp"
#include "OW_CMPIIndicationProviderProxy.hpp"

//typedef OW_CMPIProviderBaseIFC* (*ProviderCreationFunc)();
// the closest approximation of CMPIProviderBaseIFCRef is ::FTABLE

typedef OW_CMPIFTABLERef* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();

const char* const OW_CMPIProviderIFC::CREATIONFUNC = "createProvider";

//////////////////////////////////////////////////////////////////////////////
OW_CMPIProviderIFC::OW_CMPIProviderIFC()
	: OW_ProviderIFCBaseIFC()
	, m_provs()
	, m_guard()
	, m_noidProviders()
	, m_loadDone(false)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CMPIProviderIFC::~OW_CMPIProviderIFC()
{
	try
	{
		ProviderMap::iterator it = m_provs.begin();
		//OW_Reference<NPIenv> npiHandle(); // TODO: createEnv(...);
		while(it != m_provs.end())
		{
			CMPIInstanceMI * mi = it->second->instMI;
			OperationContext context;
			CMPI_ContextOnStack eCtx(context);
			mi->ft->cleanup(mi, &eCtx);

			it->second.setNull();
			it++;
		}
	
		m_provs.clear();
	
		for(size_t i = 0; i < m_noidProviders.size(); i++)
		{
			CMPIInstanceMI * mi = m_noidProviders[i]->instMI;
			OperationContext context;
			CMPI_ContextOnStack eCtx(context);
			mi->ft->cleanup(mi, &eCtx);
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
OW_CMPIProviderIFC::doInit(const OW_ProviderEnvironmentIFCRef&,
	OW_InstanceProviderInfoArray&,
	OW_AssociatorProviderInfoArray&,
	OW_MethodProviderInfoArray&,
	OW_PropertyProviderInfoArray&,
	OW_IndicationProviderInfoArray&)
{
	return;
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_CMPIProviderIFC::doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the createInstance pointer is set, then assume it's an instance
		// provider
		if(pProv->instMI)
		{
			env->getLogger()->logDebug(format("OW_CMPIProviderIFC found instance"
				" provider %1", provIdString));

			return OW_InstanceProviderIFCRef(new OW_CMPIInstanceProviderProxy(
				pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not an instance provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray
OW_CMPIProviderIFC::doGetIndicationExportProviders(const OW_ProviderEnvironmentIFCRef& env)
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
OW_CMPIProviderIFC::doGetPolledProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	(void)env;
	//loadNoIdProviders(env);
	OW_PolledProviderIFCRefArray rvra;
#if 0
	for(size_t i = 0; i < m_noidProviders.size(); i++)
	{
		OW_CMPIFTABLERef pProv = m_noidProviders[i];

		//  if (pProv->isPolledProvider())
		if(pProv->indMI)
		{
			rvra.append(
				OW_PolledProviderIFCRef(new
					OW_CMPIPolledProviderProxy(pProv)));
		}
	}
#endif

	return rvra;
}

//////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef
OW_CMPIProviderIFC::doGetMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	(void) env;
	(void) provIdString;
#if 0
	OW_CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// it's a method provider if the invokeMethod function pointer is not
		// NULL
		if(pProv->methMI)
		{
			env->getLogger()->logDebug(format("OW_CMPIProviderIFC found method provider %1",
				provIdString));

			return OW_MethodProviderIFCRef(
				new OW_CMPIMethodProviderProxy(pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not a method provider",
			provIdString));
	}
#endif

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_CMPIProviderIFC::doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	(void)env;
	(void)provIdString;
	/* not supported yet
	OW_CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if(pProv)
	{
#if 0
		if(pProv->isPropertyProvider())
		{
			env->getLogger()->logDebug(format("OW_CMPIProviderIFC found property provider %1",
				provIdString));

			return OW_PropertyProviderIFCRef(new OW_CMPIPropertyProviderProxy(
				pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not a property provider",
			provIdString));
#endif
	}
	*/

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_CMPIProviderIFC::doGetAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	(void) env;
	(void) provIdString;
#if 0
	OW_CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the associatorNames function pointer is not 0, we know it's an
		// associator provider
		if(pProv->assocMI)
		{
			env->getLogger()->logDebug(format("OW_CMPIProviderIFC found associator provider %1",
				provIdString));

			return OW_AssociatorProviderIFCRef(new
				OW_CMPIAssociatorProviderProxy(pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not an associator provider",
			provIdString));
	}
#endif

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationProviderIFCRef
OW_CMPIProviderIFC::doGetIndicationProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	(void) env;
	(void) provIdString;
#if 0
	OW_CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the indicationNames function pointer is not 0, we know it's an
		// indication provider
		if(pProv->indMI)
		{
			env->getLogger()->logDebug(format("OW_CMPIProviderIFC found indication provider %1",
				provIdString));

			return OW_IndicationProviderIFCRef(new
				OW_CMPIIndicationProviderProxy(pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not an indication provider",
			provIdString));
	}
#endif

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CMPIProviderIFC::loadNoIdProviders(const OW_ProviderEnvironmentIFCRef& env)
{
   
   env->getLogger()->logError("LoadNoIDproviders");
   OW_MutexLock ml(m_guard);

   if(m_loadDone)
   {
      return;
   }

   m_loadDone = true;

   OW_String libPath = env->getConfigItem(OW_ConfigOpts::CMPIIFC_PROV_LOC_opt);

   if(libPath.empty())
   {
      libPath = DEFAULT_CMPI_PROVIDER_LOCATION;
   }

   env->getLogger()->logError("LoadNoIDproviders 2");
   OW_SharedLibraryLoaderRef ldr =
      OW_SharedLibraryLoader::createSharedLibraryLoader();

   if(ldr.isNull())
   {
      env->getLogger()->logError("CMPI provider ifc failed to get shared lib loader");
      return;
   }

   OW_StringArray dirEntries;
   if(!OW_FileSystem::getDirectoryContents(libPath, dirEntries))
   {
      env->getLogger()->logError(format("CMPI provider ifc failed getting contents of "
         "directory: %1", libPath));
      return;
   }

   env->getLogger()->logError("LoadNoIDproviders 3");
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

      OW_String guessProvId = dirEntries[i].substring(3,dirEntries[i].length()-6);

      if(theLib.isNull())
      {
         env->getLogger()->logError(format("CMPI provider %1 ifc failed to load"
                   " library: %2", guessProvId, libName));
         continue;
      }

#if 0
      versionFunc_t versFunc;
      if(!OW_SharedLibrary::getFunctionPointer(theLib, "getOWVersion",
             versFunc))
      {
         env->getLogger()->logError(format("CMPI provider ifc failed getting"
                 " function pointer to \"getOWVersion\" from library: %1",
                 libName));
         continue;
      }

      const char* strVer = (*versFunc)();
      if(strcmp(strVer, OW_VERSION))
      {
         env->getLogger()->logError(format("CMPI provider ifc got invalid version from "
                 "provider: %1", strVer));
         continue;
      }
#endif

#if 0
	::FP_INIT_FT createProvider;
	OW_String creationFuncName = guessProvId + "_initFunctionTable";
   env->getLogger()->logError(format("LoadNoIDproviders 4b : %1", creationFuncName));

	if(!OW_SharedLibrary::getFunctionPointer(theLib, creationFuncName, createProvider))
	{
		env->getLogger()->logError(format("CMPI provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		continue;
	}

   env->getLogger()->logError("LoadNoIDproviders 5");
	::FTABLE fTable = (*createProvider)();

	if(!fTable.fp_initialize)
	{
		env->getLogger()->logError(format("CMPI provider ifc: Libary %1 - %2 returned null"
			" initialize function pointer in function table", libName, creationFuncName));
		continue;
	}

        // only initialize polled and indicationexport providers
	// since CMPI doesn't support indicationexport providers ....

   env->getLogger()->logError("LoadNoIDproviders 6");
        if (!fTable.fp_activateFilter) continue;

        // else it must be a polled provider - initialize it 

	env->getLogger()->logDebug(format("CMPI provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, guessProvId));

	::CIMOMHandle ch = {0}; // CIMOMHandle parameter is meaningless, there is
	// nothing the provider can do with it, so we'll just pass in 0

	//OW_Reference<CMPIEnv> npiHandle(); // TODO: createEnv(...);
	fTable.fp_initialize(0/*npiHandle.getPtr()*/, ch );	// Let provider initialize itself

	env->getLogger()->logDebug(format("CMPI provider ifc: provider %1 loaded and initialized",
		guessProvId));

        m_noidProviders.append(OW_FTABLERef(theLib, new ::FTABLE(fTable)));
#endif
    }
}

//////////////////////////////////////////////////////////////////////////////
OW_CMPIFTABLERef
OW_CMPIProviderIFC::getProvider(
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
		OW_ConfigOpts::CMPIIFC_PROV_LOC_opt);

	if(libPath.empty())
	{
		libPath = DEFAULT_CMPI_PROVIDER_LOCATION;
	}

	OW_SharedLibraryLoaderRef ldr =
		OW_SharedLibraryLoader::createSharedLibraryLoader();

	if(ldr.isNull())
	{
		env->getLogger()->logError("CMPI: provider ifc failed to get shared lib loader");
		return OW_CMPIFTABLERef();
	}

	OW_String libName(libPath);

	libName += OW_FILENAME_SEPARATOR;
	libName += "lib";
	libName += provId;
	libName += ".so";

	env->getLogger()->logDebug(format("OW_CMPIProviderIFC::getProvider loading library: %1",
		libName));

	OW_SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
		env->getLogger());

	if(theLib.isNull())
	{
		env->getLogger()->logError(format("CMPI provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return OW_CMPIFTABLERef();
	}

// BMMU
	//::FP_INIT_FT createProvider;
	//OW_String creationFuncName = provId + "_initFunctionTable";

        OW_ProviderEnvironmentIFCRef * provenv =
		(OW_ProviderEnvironmentIFCRef *)(&env);

	broker.hdl=new OW_ProviderEnvironmentIFCRef(* provenv);
	broker.bft=CMPI_Broker_Ftab;
	broker.eft=CMPI_BrokerEnc_Ftab;

	::CMPIInstanceMI * (*createCMPIInstanceMI) (void *, void *) = 0;
	OW_String creationFuncName = provId + "_Create_InstanceMI";

	if(!OW_SharedLibrary::getFunctionPointer(theLib, creationFuncName, createCMPIInstanceMI))
	{
		env->getLogger()->logError(format("CMPI provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		creationFuncName = provId.substring(4) + "_Create_InstanceMI";
		if(!OW_SharedLibrary::getFunctionPointer(theLib, creationFuncName, createCMPIInstanceMI))
		{
			env->getLogger()->logError(format("CMPI provider ifc: Libary %1 does not contain"
				" %2 function", libName, creationFuncName));
				return OW_CMPIFTABLERef();
		}
	}

	OperationContext opc;
	CMPI_ThreadContext thr;
	CMPI_ContextOnStack eCtx(opc);

	::CMPIInstanceMI * cProvInst = createCMPIInstanceMI(&broker, &eCtx);
//	::CMPIAssociationMI * cProvAssoc =
//		 createCMPIAssociationMI(&broker, &eCtx);
//	::CMPIIndicationMI * cProvInd = createCMPIIndicationMI(&broker, &eCtx);


	env->getLogger()->logDebug(format("CMPI provider ifc: provider %1 loaded and initialized",
		provId));

	OW_CompleteMI * cmi = new OW_CompleteMI();
	cmi->instMI = cProvInst;
	//m_provs[provId] = OW_CMPIFTABLERef(theLib, new ::CMPIInstanceMI(cProv));
	m_provs[provId] = OW_CMPIFTABLERef(theLib, cmi);

	return m_provs[provId];
}

OW_PROVIDERIFCFACTORY(OW_CMPIProviderIFC)

