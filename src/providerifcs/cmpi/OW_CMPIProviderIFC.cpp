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

namespace OpenWBEM
{

//typedef CMPIProviderBaseIFC* (*ProviderCreationFunc)();
// the closest approximation of CMPIProviderBaseIFCRef is ::FTABLE
typedef CMPIFTABLERef* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();
const char* const CMPIProviderIFC::CREATIONFUNC = "createProvider";
//////////////////////////////////////////////////////////////////////////////
CMPIProviderIFC::CMPIProviderIFC()
	: ProviderIFCBaseIFC()
	, m_provs()
	, m_guard()
	, m_noidProviders()
	, m_loadDone(false)
{
}
//////////////////////////////////////////////////////////////////////////////
CMPIProviderIFC::~CMPIProviderIFC()
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
CMPIProviderIFC::doInit(const ProviderEnvironmentIFCRef&,
	InstanceProviderInfoArray&,
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
CMPIProviderIFC::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the createInstance pointer is set, then assume it's an instance
		// provider
		if(pProv->miVector.instMI)
		{
			env->getLogger()->logDebug(format("CMPIProviderIFC found instance"
				" provider %1", provIdString));
			return InstanceProviderIFCRef(new CMPIInstanceProviderProxy(
				pProv));
		}
		env->getLogger()->logError(format("Provider %1 is not an instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
CMPIProviderIFC::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef& env)
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
CMPIProviderIFC::doGetPolledProviders(const ProviderEnvironmentIFCRef& env)
{
	(void)env;
	//loadNoIdProviders(env);
	PolledProviderIFCRefArray rvra;
#if 0
	for(size_t i = 0; i < m_noidProviders.size(); i++)
	{
		CMPIFTABLERef pProv = m_noidProviders[i];
		//  if (pProv->isPolledProvider())
		if(pProv->miVector.indMI)
		{
			rvra.append(
				PolledProviderIFCRef(new
					CMPIPolledProviderProxy(pProv)));
		}
	}
#endif
	return rvra;
}
//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
CMPIProviderIFC::doGetMethodProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// it's a method provider if the invokeMethod function pointer is not
		// NULL
		if(pProv->miVector.methMI)
		{
			env->getLogger()->logDebug(format("CMPIProviderIFC found method provider %1",
				provIdString));
			return MethodProviderIFCRef(
				new CMPIMethodProviderProxy(pProv));
		}
		env->getLogger()->logError(format("Provider %1 is not a method provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
CMPIProviderIFC::doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the associatorNames function pointer is not 0, we know it's an
		// associator provider
		if(pProv->miVector.assocMI)
		{
			env->getLogger()->logDebug(format("CMPIProviderIFC found associator provider %1",
				provIdString));
			return AssociatorProviderIFCRef(new
				CMPIAssociatorProviderProxy(pProv));
		}
		env->getLogger()->logError(format("Provider %1 is not an associator provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#endif
//////////////////////////////////////////////////////////////////////////////
IndicationProviderIFCRef
CMPIProviderIFC::doGetIndicationProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	(void) env;
	(void) provIdString;
#if 0
	CMPIFTABLERef pProv = getProvider(env, provIdString);
	if(pProv)
	{
		// if the indicationNames function pointer is not 0, we know it's an
		// indication provider
		if(pProv->miVector.indMI)
		{
			env->getLogger()->logDebug(format("CMPIProviderIFC found indication provider %1",
				provIdString));
			return IndicationProviderIFCRef(new
				CMPIIndicationProviderProxy(pProv));
		}
		env->getLogger()->logError(format("Provider %1 is not an indication provider",
			provIdString));
	}
#endif
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
void
CMPIProviderIFC::loadNoIdProviders(const ProviderEnvironmentIFCRef& env)
{
   
   env->getLogger()->logError("LoadNoIDproviders");
   MutexLock ml(m_guard);
   if(m_loadDone)
   {
	  return;
   }
   m_loadDone = true;
   String libPath = env->getConfigItem(ConfigOpts::CMPIIFC_PROV_LOC_opt, OW_DEFAULT_CMPI_PROVIDER_LOCATION);
   env->getLogger()->logError("LoadNoIDproviders 2");
   SharedLibraryLoaderRef ldr =
	  SharedLibraryLoader::createSharedLibraryLoader();
   if(ldr.isNull())
   {
	  env->getLogger()->logError("CMPI provider ifc failed to get shared lib loader");
	  return;
   }
   StringArray dirEntries;
   if(!FileSystem::getDirectoryContents(libPath, dirEntries))
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
	  String libName = libPath;
	  libName += OW_FILENAME_SEPARATOR;
	  libName += dirEntries[i];
	  SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
			env->getLogger());
	  String guessProvId = dirEntries[i].substring(3,dirEntries[i].length()-6);
	  if(theLib.isNull())
	  {
		 env->getLogger()->logError(format("CMPI provider %1 ifc failed to load"
				   " library: %2", guessProvId, libName));
		 continue;
	  }
	}
}
//////////////////////////////////////////////////////////////////////////////
CMPIFTABLERef
CMPIProviderIFC::getProvider(
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
		ConfigOpts::CMPIIFC_PROV_LOC_opt, OW_DEFAULT_CMPI_PROVIDER_LOCATION);
	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if(ldr.isNull())
	{
		env->getLogger()->logError("CMPI: provider ifc failed to get shared lib loader");
		return CMPIFTABLERef();
	}
	String libName(libPath);
	libName += OW_FILENAME_SEPARATOR;
	libName += "lib";
	libName += provId;
	libName += ".so";
	env->getLogger()->logDebug(format("CMPIProviderIFC::getProvider loading library: %1",
		libName));
	SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
		env->getLogger());
	if(theLib.isNull())
	{
		env->getLogger()->logError(format("CMPI provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return CMPIFTABLERef();
	}
	memset(&miVector, 0, sizeof(MIs));
		miVector.genericMode = 0;
	int specificMode = 0;
	// find InstanceProvider entry points
	if (theLib->getFunctionPointer(
		"_Generic_Create_InstanceMI", miVector.createGenInstMI))
	{
		miVector.miTypes |= CMPI_MIType_Instance;
			miVector.genericMode = 1;
	}
	String creationFuncName = provId.substring(4) + "_Create_InstanceMI";
	env->getLogger()->logError(format("CMPI provider ifc: Library %1 should contain %2", provId, creationFuncName));
	if (theLib->getFunctionPointer(
		creationFuncName, miVector.createInstMI))
	{
		miVector.miTypes |= CMPI_MIType_Instance;
		specificMode = 1;
	}
	// find AssociationProvider entry points
	if (theLib->getFunctionPointer(
		"_Generic_Create_AssociationMI", miVector.createGenAssocMI))
	{
		miVector.miTypes |= CMPI_MIType_Association;
			miVector.genericMode = 1;
	}
	creationFuncName = provId.substring(4) + "_Create_AssociationMI";
	if (theLib->getFunctionPointer(
		creationFuncName, miVector.createAssocMI))
	{
		miVector.miTypes |= CMPI_MIType_Association;
		specificMode = 1;
	}
	// find MethodProvider entry points
	if (theLib->getFunctionPointer(
		"_Generic_Create_MethodMI", miVector.createGenMethMI))
	{
		miVector.miTypes |= CMPI_MIType_Method;
			miVector.genericMode = 1;
	}
	creationFuncName = provId.substring(4) + "_Create_MethodMI";
	if (theLib->getFunctionPointer(
		creationFuncName, miVector.createMethMI))
	{
		miVector.miTypes |= CMPI_MIType_Method;
		specificMode = 1;
	}
	// find PropertyProvider entry points
	if (theLib->getFunctionPointer(
		"_Generic_Create_PropertyMI", miVector.createGenPropMI))
	{
		miVector.miTypes |= CMPI_MIType_Property;
			miVector.genericMode = 1;
	}
	creationFuncName = provId.substring(4) + "_Create_PropertyMI";
	if (theLib->getFunctionPointer(
		creationFuncName, miVector.createPropMI))
	{
		miVector.miTypes |= CMPI_MIType_Property;
		specificMode = 1;
	}
	// find IndicationProvider entry points
	if (theLib->getFunctionPointer(
		"_Generic_Create_IndicationMI", miVector.createGenIndMI))
	{
		miVector.miTypes |= CMPI_MIType_Indication;
			miVector.genericMode = 1;
	}
	creationFuncName = provId.substring(4) + "_Create_IndicationMI";
	if (theLib->getFunctionPointer(
		creationFuncName, miVector.createIndMI))
	{
		miVector.miTypes |= CMPI_MIType_Indication;
		specificMode = 1;
	}
				
	if (miVector.miTypes == 0)
	{
		env->getLogger()->logError(format("CMPI provider ifc: Library %1 does not contain"
		" any CMPI function", libName));
		return CMPIFTABLERef();
	}
	if (miVector.genericMode && specificMode)
	{
		env->getLogger()->logError(format("CMPI provider ifc: Library %1 mixes generic/specific"
		" CMPI style provider functions", libName));
		return CMPIFTABLERef();
	}
	// Now it's time to initialize the providers
		ProviderEnvironmentIFCRef * provenv =
		(ProviderEnvironmentIFCRef *)(&env);
	_broker.hdl=new ProviderEnvironmentIFCRef(* provenv);
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
	CompleteMI * completeMI = new  CompleteMI();
		completeMI->miVector = miVector;
	delete ((ProviderEnvironmentIFC *)completeMI->broker.hdl);
		completeMI->broker = _broker;
	//MIs * _miVector = new MIs(miVector);
	if (completeMI->miVector.instMI != miVector.instMI)
	{
		env->getLogger()->logDebug("CMPI provider ifc: WARNING Vector mismatch");
	}
	m_provs[provId] = CMPIFTABLERef(theLib, completeMI);
	return m_provs[provId];
}

} // end namespace OpenWBEM

OW_PROVIDERIFCFACTORY(OpenWBEM::CMPIProviderIFC)

