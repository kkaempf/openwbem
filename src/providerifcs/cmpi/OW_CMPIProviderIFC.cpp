/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc, IBM Corp. All rights reserved.
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
*
* Author:        Markus Mueller <sedgewick_de@yahoo.de>
*
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
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_CMPIAssociatorProviderProxy.hpp"
#endif
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

// BMMU: have to cleanup non instance providers, too

		while(it != m_provs.end())
		{
			//CMPIInstanceMI * mi = it->second->instMI;
			MIs miVector = it->second->miVector;
			if (miVector.instMI)
			{
				OperationContext context;
				CMPI_ContextOnStack eCtx(context);
				miVector.instMI->ft->cleanup(miVector.instMI,
						 &eCtx);
			}

			it->second.setNull();
			it++;
		}
	
		m_provs.clear();
	
// BMMU: have to cleanup polled providers 

		for(size_t i = 0; i < m_noidProviders.size(); i++)
		{
#if 0
			CMPIInstanceMI * mi = m_noidProviders[i]->instMI;
			OperationContext context;
			CMPI_ContextOnStack eCtx(context);
			mi->ft->cleanup(mi, &eCtx);
			m_noidProviders[i].setNull();
#endif
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
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	OW_AssociatorProviderInfoArray&,
#endif
	OW_MethodProviderInfoArray&,
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
		if(pProv->miVector.instMI)
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
		if(pProv->miVector.indMI)
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
	OW_CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// it's a method provider if the invokeMethod function pointer is not
		// NULL
		if(pProv->miVector.methMI)
		{
			env->getLogger()->logDebug(format("OW_CMPIProviderIFC found method provider %1",
				provIdString));

			return OW_MethodProviderIFCRef(
				new OW_CMPIMethodProviderProxy(pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not a method provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_CMPIProviderIFC::doGetAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	OW_CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the associatorNames function pointer is not 0, we know it's an
		// associator provider
		if(pProv->miVector.assocMI)
		{
			env->getLogger()->logDebug(format("OW_CMPIProviderIFC found associator provider %1",
				provIdString));

			return OW_AssociatorProviderIFCRef(new
				OW_CMPIAssociatorProviderProxy(pProv));
		}

		env->getLogger()->logError(format("Provider %1 is not an associator provider",
			provIdString));
	}

	OW_THROW(OW_NoSuchProviderException, provIdString);
}
#endif

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
		if(pProv->miVector.indMI)
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

   OW_String libPath = env->getConfigItem(OW_ConfigOpts::CMPIIFC_PROV_LOC_opt, OW_DEFAULT_CMPI_PROVIDER_LOCATION);

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
		OW_ConfigOpts::CMPIIFC_PROV_LOC_opt, OW_DEFAULT_CMPI_PROVIDER_LOCATION);

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

	memset(&miVector, 0, sizeof(MIs));
        miVector.genericMode = 0;

	int specificMode = 0;

	// find InstanceProvider entry points

	if (OW_SharedLibrary::getFunctionPointer(theLib,
		"_Generic_Create_InstanceMI", miVector.createGenInstMI))
	{
		miVector.miTypes |= CMPI_MIType_Instance;
        	miVector.genericMode = 1;
	}

	OW_String creationFuncName = provId.substring(4) + "_Create_InstanceMI";
	env->getLogger()->logError(format("CMPI provider ifc: Library %1 should contain %2", provId, creationFuncName));
	if (OW_SharedLibrary::getFunctionPointer(theLib,
		creationFuncName, miVector.createInstMI))
	{
		miVector.miTypes |= CMPI_MIType_Instance;
 		specificMode = 1;
	}

	// find AssociationProvider entry points

	if (OW_SharedLibrary::getFunctionPointer(theLib,
		"_Generic_Create_AssociationMI", miVector.createGenAssocMI))
	{
		miVector.miTypes |= CMPI_MIType_Association;
        	miVector.genericMode = 1;
	}

	creationFuncName = provId.substring(4) + "_Create_AssociationMI";
	if (OW_SharedLibrary::getFunctionPointer(theLib,
		creationFuncName, miVector.createAssocMI))
	{
		miVector.miTypes |= CMPI_MIType_Association;
 		specificMode = 1;
	}

	// find MethodProvider entry points

	if (OW_SharedLibrary::getFunctionPointer(theLib,
		"_Generic_Create_MethodMI", miVector.createGenMethMI))
	{
		miVector.miTypes |= CMPI_MIType_Method;
        	miVector.genericMode = 1;
	}

	creationFuncName = provId.substring(4) + "_Create_MethodMI";
	if (OW_SharedLibrary::getFunctionPointer(theLib,
		creationFuncName, miVector.createMethMI))
	{
		miVector.miTypes |= CMPI_MIType_Method;
 		specificMode = 1;
	}

	// find PropertyProvider entry points

	if (OW_SharedLibrary::getFunctionPointer(theLib,
		"_Generic_Create_PropertyMI", miVector.createGenPropMI))
	{
		miVector.miTypes |= CMPI_MIType_Property;
        	miVector.genericMode = 1;
	}

	creationFuncName = provId.substring(4) + "_Create_PropertyMI";
	if (OW_SharedLibrary::getFunctionPointer(theLib,
		creationFuncName, miVector.createPropMI))
	{
		miVector.miTypes |= CMPI_MIType_Property;
 		specificMode = 1;
	}

	// find IndicationProvider entry points

	if (OW_SharedLibrary::getFunctionPointer(theLib,
		"_Generic_Create_IndicationMI", miVector.createGenIndMI))
	{
		miVector.miTypes |= CMPI_MIType_Indication;
        	miVector.genericMode = 1;
	}

	creationFuncName = provId.substring(4) + "_Create_IndicationMI";
	if (OW_SharedLibrary::getFunctionPointer(theLib,
		creationFuncName, miVector.createIndMI))
	{
		miVector.miTypes |= CMPI_MIType_Indication;
 		specificMode = 1;
	}

                
#if 0
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
		}
	}
#endif

	if (miVector.miTypes == 0)
	{
		env->getLogger()->logError(format("CMPI provider ifc: Library %1 does not contain"
		" any CMPI function", libName));
		return OW_CMPIFTABLERef();
	}

	if (miVector.genericMode && specificMode)
	{
		env->getLogger()->logError(format("CMPI provider ifc: Library %1 mixes generic/specific"
		" CMPI style provider functions", libName));
		return OW_CMPIFTABLERef();
	}

	// Now it's time to initialize the providers

        OW_ProviderEnvironmentIFCRef * provenv =
		(OW_ProviderEnvironmentIFCRef *)(&env);

	_broker.hdl=new OW_ProviderEnvironmentIFCRef(* provenv);
	_broker.bft=CMPI_Broker_Ftab;
	_broker.eft=CMPI_BrokerEnc_Ftab;
	_broker.clsCache = NULL;


	OperationContext opc;
	CMPI_ContextOnStack eCtx(opc);

	if (miVector.genericMode)
	{
		char * mName = provId.allocateCString();
		if (miVector.miTypes & CMPI_MIType_Instance)
		{
			miVector.instMI =
				miVector.createGenInstMI(&_broker,&eCtx,mName);
		}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		if (miVector.miTypes & CMPI_MIType_Association)
		{
			miVector.assocMI =
				miVector.createGenAssocMI(&_broker,&eCtx,mName);
		}
#endif
		if (miVector.miTypes & CMPI_MIType_Method)
		{
			miVector.methMI =
				miVector.createGenMethMI(&_broker,&eCtx,mName);
		}
		if (miVector.miTypes & CMPI_MIType_Property)
		{
			miVector.propMI =
				miVector.createGenPropMI(&_broker,&eCtx,mName);
		}
		if (miVector.miTypes & CMPI_MIType_Indication)
		{
			miVector.indMI =
				miVector.createGenIndMI(&_broker,&eCtx,mName);
		}
		free(mName);
	}
	else
	{
		if (miVector.miTypes & CMPI_MIType_Instance)
		{
			miVector.instMI =
				miVector.createInstMI(&_broker,&eCtx);
		}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		if (miVector.miTypes & CMPI_MIType_Association)
		{
			miVector.assocMI =
				miVector.createAssocMI(&_broker,&eCtx);
		}
#endif
		if (miVector.miTypes & CMPI_MIType_Method)
		{
			miVector.methMI =
				miVector.createMethMI(&_broker,&eCtx);
		}
		if (miVector.miTypes & CMPI_MIType_Property)
		{
			miVector.propMI =
				miVector.createPropMI(&_broker,&eCtx);
		}
		if (miVector.miTypes & CMPI_MIType_Indication)
		{
			miVector.indMI =
				miVector.createIndMI(&_broker,&eCtx);
		}
	}

	env->getLogger()->logDebug(format("CMPI provider ifc: provider %1 loaded and initialized",
		provId));

	OW_CompleteMI * completeMI = new  OW_CompleteMI();
        completeMI->miVector = miVector;

	delete ((OW_ProviderEnvironmentIFC *)completeMI->broker.hdl);

        completeMI->broker = _broker;

	//MIs * _miVector = new MIs(miVector);
	if (completeMI->miVector.instMI != miVector.instMI)
	{
		env->getLogger()->logDebug("CMPI provider ifc: WARNING Vector mismatch");
	}
	m_provs[provId] = OW_CMPIFTABLERef(theLib, completeMI);

	return m_provs[provId];
}

OW_PROVIDERIFCFACTORY(OW_CMPIProviderIFC)

