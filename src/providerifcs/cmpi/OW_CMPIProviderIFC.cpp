/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc., IBM Corp. All rights reserved.
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
*
* Author:        Markus Mueller <sedgewick_de@yahoo.de>
*
*******************************************************************************/
#include "OW_config.h"
#include "OW_CMPIProviderIFC.hpp"
#include "blocxx/SharedLibraryException.hpp"
#include "blocxx/SharedLibraryLoader.hpp"
#include "blocxx/Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/FileSystem.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_CMPIInstanceProviderProxy.hpp"
#include "OW_CMPIMethodProviderProxy.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_CMPIAssociatorProviderProxy.hpp"
#endif
#include "OW_CMPIIndicationProviderProxy.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"
#include "cmpisrv.h"
#include "OW_OperationContext.hpp"

#include <algorithm>
using std::fill_n;

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.provider.cmpi.ifc");
}

//////////////////////////////////////////////////////////////////////////////
void
CMPIPrepareContext(
	const ProviderEnvironmentIFCRef& env,
	CMPI_ContextOnStack& eCtx,
	ELocalOnlyFlag localOnly,
	EDeepFlag deep,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin)
{
	LoggerRef logger = env->getLogger(COMPONENT_NAME);

	CMPIFlags flgs = 0;
	if (localOnly == E_LOCAL_ONLY)
		flgs |= CMPI_FLAG_LocalOnly;
	if (deep == E_DEEP)
		flgs |= CMPI_FLAG_DeepInheritance;
	if (includeQualifiers == E_INCLUDE_QUALIFIERS)
		flgs |= CMPI_FLAG_IncludeQualifiers;
	if (includeClassOrigin == E_INCLUDE_CLASS_ORIGIN)
		flgs |= CMPI_FLAG_IncludeClassOrigin;

	CIMParamValueArray* args = (CIMParamValueArray *)eCtx.hdl;

	args->append(CIMParamValue(CMPIInvocationFlags, CIMValue(UInt32(flgs))));

	OperationContext &ctx = env->getOperationContext();

	String user = ctx.getStringDataWithDefault(OperationContext::USER_NAME);
	if (!user.empty())
	{
	    args->append(CIMParamValue("CMPIPrincipal", CIMValue(user)));
	}


	OW_LOG_DEBUG3(env->getLogger(COMPONENT_NAME),
		Format("CMPIPrepareContext. User: %1 Flgs: %2",
			user, flgs));
}

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
		while (it != m_provs.end())
		{
			//CMPIInstanceMI * mi = it->second->instMI;
			MIs miVector = it->second->miVector;

			// If instance provider, allow instance prov cleanup to run
			if (miVector.instMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				miVector.instMI->ft->cleanup(miVector.instMI, &eCtx, true);
			}

			// If associator provider, allow associator prov cleanup to run
			if (miVector.assocMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				miVector.assocMI->ft->cleanup(miVector.assocMI, &eCtx, true);
			}

			// If method provider, allow method prov cleanup to run
			if (miVector.methMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				miVector.methMI->ft->cleanup(miVector.methMI, &eCtx, true);
			}

			// If property provider, allow property prov cleanup to run
			if (miVector.propMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				miVector.propMI->ft->cleanup(miVector.propMI, &eCtx, true);
			}

			// If indication provider, allow indication prov cleanup to run
			if (miVector.indMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				miVector.indMI->ft->cleanup(miVector.indMI, &eCtx, true);
			}

			it->second.setNull();
			it++;
		}

		m_provs.clear();

// BMMU: have to cleanup polled providers
		for (size_t i = 0; i < m_noidProviders.size(); i++)
		{
#if 0
			CMPIInstanceMI * mi = m_noidProviders[i]->instMI;
			::CMPIOperationContext context;
			CMPI_ContextOnStack eCtx(context);
			CMPI_ThreadContext thr(&_broker, &eCtx);
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
	SecondaryInstanceProviderInfoArray&,

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	AssociatorProviderInfoArray&,
#endif

	MethodProviderInfoArray&,
	IndicationProviderInfoArray&,
	QueryProviderInfoArray&)
{
	return;
}
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
CMPIProviderIFC::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CMPIFTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		// if the createInstance pointer is set, then assume it's an instance
		// provider
		if (pProv->miVector.instMI)
		{
			OW_LOG_DEBUG2(lgr, Format("CMPIProviderIFC found instance"
				" provider %1", provIdString));
			return InstanceProviderIFCRef(new CMPIInstanceProviderProxy(pProv));
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not an instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
CMPIProviderIFC::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef& env)
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
CMPIProviderIFC::doGetPolledProviders(const ProviderEnvironmentIFCRef& env)
{
	//loadNoIdProviders(env);
	PolledProviderIFCRefArray rvra;

#if 0
	for (size_t i = 0; i < m_noidProviders.size(); i++)
	{
		CMPIFTABLERef pProv = m_noidProviders[i];
		//  if (pProv->isPolledProvider())
		if (pProv->miVector.indMI)
		{
			rvra.append(PolledProviderIFCRef(
				new CMPIPolledProviderProxy(pProv)));
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
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		// it's a method provider if the invokeMethod function pointer is not
		// NULL
		if (pProv->miVector.methMI)
		{
			OW_LOG_DEBUG2(lgr, Format("CMPIProviderIFC found method provider %1",
				provIdString));
			return MethodProviderIFCRef(new CMPIMethodProviderProxy(pProv));
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not a method provider",
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
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		// if the associatorNames function pointer is not 0, we know it's an
		// associator provider
		if (pProv->miVector.assocMI)
		{
			OW_LOG_DEBUG2(lgr, Format("CMPIProviderIFC found associator provider %1",
				provIdString));
			return AssociatorProviderIFCRef(new
				CMPIAssociatorProviderProxy(pProv));
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not an associator provider",
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
#if 0
	CMPIFTABLERef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		// if the indicationNames function pointer is not 0, we know it's an
		// indication provider
		if (pProv->miVector.indMI)
		{
			OW_LOG_DEBUG2(lgr, Format("CMPIProviderIFC found indication provider %1",
				provIdString));
			return IndicationProviderIFCRef(new
				CMPIIndicationProviderProxy(pProv));
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not an indication provider",
			provIdString));
	}
#endif

	OW_THROW(NoSuchProviderException, provIdString);
}

//////////////////////////////////////////////////////////////////////////////
void
CMPIProviderIFC::loadNoIdProviders(const ProviderEnvironmentIFCRef& env)
{
	Logger lgr(COMPONENT_NAME);
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
	  OW_LOG_ERROR(lgr, "CMPI provider ifc failed to get shared lib loader");
	  return;
   }

   const StringArray libPaths = env->getMultiConfigItem(
	   ConfigOpts::CMPIPROVIFC_PROV_LOCATION_opt,
	   String(OW_DEFAULT_CMPIPROVIFC_PROV_LOCATION).tokenize(OW_PATHNAME_SEPARATOR),
	   OW_PATHNAME_SEPARATOR);

   for (size_t libIdx = 0; libIdx < libPaths.size(); ++libIdx)
   {
	   String libPath(libPaths[libIdx]);
	   StringArray dirEntries;
	   if (!FileSystem::getDirectoryContents(libPath, dirEntries))
	   {
		  OW_LOG_ERROR(lgr, Format("CMPI provider ifc failed getting contents of "
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
			SharedLibraryRef theLib = ldr->loadSharedLibrary(libName);
			String guessProvId = dirEntries[i].substring(3,dirEntries[i].length()-6);

			if (!theLib)
			{
				 OW_LOG_ERROR(lgr, Format("CMPI provider %1 ifc failed to load library: %2", guessProvId, libName));
				 continue;
			}
	   }
   }
}

//////////////////////////////////////////////////////////////////////////////
// Static
CMPIFTABLERef
CMPIProviderIFC::loadProvider(
	const ProviderEnvironmentIFCRef& env,
	const String& providerLib,
	::CMPI_Broker& broker)
{
	Logger lgr(COMPONENT_NAME);

	size_t ndx = providerLib.lastIndexOf(OW_FILENAME_SEPARATOR);
	String provId = (ndx != String::npos) ? providerLib.substring(ndx+1) : providerLib;

	if (provId.startsWith("lib"))
	{
		provId = provId.substring(3);
	}
	ndx = provId.indexOf('.');
	if (ndx != String::npos)
	{
		provId = provId.substring(0, ndx);
	}

	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();

	if (!ldr)
	{
		OW_LOG_ERROR(lgr, "CMPI: provider ifc failed to get shared lib loader");
		return CMPIFTABLERef();
	}

	SharedLibraryRef theLib = ldr->loadSharedLibrary(providerLib);
	if (!theLib)
	{
		OW_LOG_ERROR(lgr, Format("CMPI provider ifc failed to load library: %1 "
			"for provider id %2", providerLib, provId));
		return CMPIFTABLERef();
	}

	MIs miVector;
	fill_n((char*)&miVector, sizeof(MIs), 0);

	int specificMode = 0;

	///////////////////////////////////////////
	// find InstanceProvider entry points
	if (theLib->getFunctionPointer("_Generic_Create_InstanceMI",
		miVector.createGenInstMI))
	{
		miVector.miTypes |= CMPI_MIType_Instance;
			miVector.genericMode = 1;
	}

	String creationFuncName;
	if (provId.startsWith("cmpi"))
	{
		creationFuncName = provId.substring(4) + "_Create_InstanceMI";
	}
	else
	{
		creationFuncName = provId + "_Create_InstanceMI";
	}

	if (theLib->getFunctionPointer(creationFuncName, miVector.createInstMI))
	{
		miVector.miTypes |= CMPI_MIType_Instance;
		specificMode = 1;
	}

	///////////////////////////////////////////
	// find AssociationProvider entry points
	if (theLib->getFunctionPointer("_Generic_Create_AssociationMI",
		miVector.createGenAssocMI))
	{
		miVector.miTypes |= CMPI_MIType_Association;
			miVector.genericMode = 1;
	}

	if (provId.startsWith("cmpi"))
	{
		creationFuncName = provId.substring(4) + "_Create_AssociationMI";
	}
	else
	{
		creationFuncName = provId + "_Create_AssociationMI";
	}

	if (theLib->getFunctionPointer(creationFuncName, miVector.createAssocMI))
	{
		miVector.miTypes |= CMPI_MIType_Association;
		specificMode = 1;
	}

	///////////////////////////////////////////
	// find MethodProvider entry points
	if (theLib->getFunctionPointer("_Generic_Create_MethodMI",
		miVector.createGenMethMI))
	{
		miVector.miTypes |= CMPI_MIType_Method;
			miVector.genericMode = 1;
	}

	if (provId.startsWith("cmpi"))
	{
		creationFuncName = provId.substring(4) + "_Create_MethodMI";
	}
	else
	{
		creationFuncName = provId + "_Create_MethodMI";
	}

	if (theLib->getFunctionPointer(creationFuncName, miVector.createMethMI))
	{
		miVector.miTypes |= CMPI_MIType_Method;
		specificMode = 1;
	}

	///////////////////////////////////////////
	// find PropertyProvider entry points
	if (theLib->getFunctionPointer("_Generic_Create_PropertyMI",
		miVector.createGenPropMI))
	{
		miVector.miTypes |= CMPI_MIType_Property;
			miVector.genericMode = 1;
	}

	if (provId.startsWith("cmpi"))
	{
		creationFuncName = provId.substring(4) + "_Create_PropertyMI";
	}
	else
	{
		creationFuncName = provId + "_Create_PropertyMI";
	}

	if (theLib->getFunctionPointer(creationFuncName, miVector.createPropMI))
	{
		miVector.miTypes |= CMPI_MIType_Property;
		specificMode = 1;
	}

	///////////////////////////////////////////
	// find IndicationProvider entry points
	if (theLib->getFunctionPointer("_Generic_Create_IndicationMI",
		miVector.createGenIndMI))
	{
		miVector.miTypes |= CMPI_MIType_Indication;
			miVector.genericMode = 1;
	}

	if (provId.startsWith("cmpi"))
	{
		creationFuncName = provId.substring(4) + "_Create_IndicationMI";
	}
	else
	{
		creationFuncName = provId + "_Create_IndicationMI";
	}

	if (theLib->getFunctionPointer(creationFuncName, miVector.createIndMI))
	{
		miVector.miTypes |= CMPI_MIType_Indication;
		specificMode = 1;
	}

	if (miVector.miTypes == 0)
	{
		OW_LOG_ERROR(lgr, Format("CMPI provider ifc: Library %1 does not contain"
			" any CMPI function", providerLib));
		return CMPIFTABLERef();
	}

	if (miVector.genericMode && specificMode)
	{
		OW_LOG_ERROR(lgr, Format("CMPI provider ifc: Library %1 mixes generic/specific"
			" CMPI style provider functions", providerLib));
		return CMPIFTABLERef();
	}

	///////////////////////////////////////////
	// Now it's time to initialize the providers

	// This is a bad hack for the broken CMPI architecture.  Even though the
	// _broker.hdl pointer will outlive the lifetime of nonConstEnv (or env),
	// it won't (shouldn't) ever be used after initialization.
	ProviderEnvironmentIFCRef nonConstEnv(env);
	broker.hdl = &nonConstEnv;
	broker.bft = CMPI_Broker_Ftab;
	broker.eft = CMPI_BrokerEnc_Ftab;
	::CMPIOperationContext opc;
	CMPI_ContextOnStack eCtx(opc);
	CMPI_ThreadContext thr(&broker, &eCtx);

	if (miVector.genericMode)
	{
		const char *mName = provId.c_str();
		if (miVector.miTypes & CMPI_MIType_Instance)
		{
			miVector.instMI = miVector.createGenInstMI(&broker,&eCtx,mName);
		}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		if (miVector.miTypes & CMPI_MIType_Association)
		{
			miVector.assocMI = miVector.createGenAssocMI(&broker, &eCtx,
				mName);
		}
#endif

		if (miVector.miTypes & CMPI_MIType_Method)
		{
			miVector.methMI = miVector.createGenMethMI(&broker, &eCtx, mName);
		}

		if (miVector.miTypes & CMPI_MIType_Property)
		{
			miVector.propMI = miVector.createGenPropMI(&broker, &eCtx, mName);
		}

		if (miVector.miTypes & CMPI_MIType_Indication)
		{
			miVector.indMI = miVector.createGenIndMI(&broker, &eCtx, mName);
		}
	}
	else
	{
		if (miVector.miTypes & CMPI_MIType_Instance)
		{
			miVector.instMI = miVector.createInstMI(&broker,&eCtx);
		}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		if (miVector.miTypes & CMPI_MIType_Association)
		{
			miVector.assocMI = miVector.createAssocMI(&broker,&eCtx);
		}
#endif

		if (miVector.miTypes & CMPI_MIType_Method)
		{
			miVector.methMI = miVector.createMethMI(&broker,&eCtx);
		}
		if (miVector.miTypes & CMPI_MIType_Property)
		{
			miVector.propMI = miVector.createPropMI(&broker,&eCtx);
		}
		if (miVector.miTypes & CMPI_MIType_Indication)
		{
			miVector.indMI = miVector.createIndMI(&broker,&eCtx);
		}
	}

	OW_LOG_DEBUG2(lgr, Format("CMPI provider ifc: provider %1 loaded and initialized",
		provId));
	CMPIFTABLERef completeMI(theLib, new CompleteMI);
	completeMI->miVector = miVector;
	completeMI->broker = broker;
	//MIs * _miVector = new MIs(miVector);
	if (completeMI->miVector.instMI != miVector.instMI)
	{
		OW_LOG_DEBUG(lgr, "CMPI provider ifc: WARNING Vector mismatch");
	}
	return completeMI;
}

//////////////////////////////////////////////////////////////////////////////
CMPIFTABLERef
CMPIProviderIFC::getProvider(
	const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	Logger lgr(COMPONENT_NAME);
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
		OW_LOG_ERROR(lgr, "CMPI: provider ifc failed to get shared lib loader");
		return CMPIFTABLERef();
	}

	const StringArray libPaths = env->getMultiConfigItem(
		ConfigOpts::CMPIPROVIFC_PROV_LOCATION_opt,
		String(OW_DEFAULT_CMPIPROVIFC_PROV_LOCATION).tokenize(OW_PATHNAME_SEPARATOR),
		OW_PATHNAME_SEPARATOR);

	for (size_t libIdx = 0; libIdx < libPaths.size(); ++libIdx)
	{
		String libPath(libPaths[libIdx]);
		String libName(libPath);
		libName += OW_FILENAME_SEPARATOR;
		libName += "lib";
		libName += provId;
		libName += OW_SHAREDLIB_EXTENSION;
		OW_LOG_DEBUG3(lgr, Format("CMPIProviderIFC::getProvider loading library: %1",
			libName));

		if (!FileSystem::exists(libName))
		{
			continue;
		}

		CMPIFTABLERef prvtbl = loadProvider(env, libName, _broker);
		if (!prvtbl)
		{
			return CMPIFTABLERef();
		}

		m_provs[provId] = prvtbl;
		return m_provs[provId];
	}
	return CMPIFTABLERef();
}

//////////////////////////////////////////////////////////////////////////////
void
CMPIProviderIFC::doUnloadProviders(
	const ProviderEnvironmentIFCRef& env)
{
	String timeWindow = env->getConfigItem(ConfigOpts::CMPIPROVIFC_PROV_TTL_opt, OW_DEFAULT_CMPIPROVIFC_PROV_TTL);
	Int32 iTimeWindow;
	try
	{
		iTimeWindow = timeWindow.toInt32();
	}
	catch(const StringConversionException&)
	{
		iTimeWindow = String(OW_DEFAULT_CPPPROVIFC_PROV_TTL).toInt32();
	}

	if (iTimeWindow < 0)
	{
		return;
	}

	DateTime dt;
	dt.setToCurrent();
	CMPIStatus rc = {CMPI_RC_OK, NULL};
	MutexLock ml(m_guard);
	ProviderMap::iterator it = m_provs.begin();
	while (it != m_provs.end())
	{
		DateTime provDt = it->second->lastAccessTime;
		provDt.addMinutes(iTimeWindow);
		if(provDt < dt)
		{
			bool unload = true;
			//CMPIInstanceMI * mi = it->second->instMI;
			MIs miVector = it->second->miVector;

			// If instance provider, allow instance prov cleanup to run
			if (miVector.instMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				rc=miVector.instMI->ft->cleanup(miVector.instMI, &eCtx, false);
				if (rc.rc == CMPI_RC_ERR_NOT_SUPPORTED
					|| rc.rc == CMPI_RC_DO_NOT_UNLOAD
					|| rc.rc == CMPI_RC_NEVER_UNLOAD)
				{
					unload = false;
				}
			}

			// If associator provider, allow associator prov cleanup to run
			if (miVector.assocMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				rc=miVector.assocMI->ft->cleanup(miVector.assocMI, &eCtx, false);
				if (rc.rc == CMPI_RC_ERR_NOT_SUPPORTED
					|| rc.rc == CMPI_RC_DO_NOT_UNLOAD
					|| rc.rc == CMPI_RC_NEVER_UNLOAD)
				{
					unload = false;
				}
			}

			// If method provider, allow method prov cleanup to run
			if (miVector.methMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				rc=miVector.methMI->ft->cleanup(miVector.methMI, &eCtx, false);
				if (rc.rc == CMPI_RC_ERR_NOT_SUPPORTED
					|| rc.rc == CMPI_RC_DO_NOT_UNLOAD
					|| rc.rc == CMPI_RC_NEVER_UNLOAD)
				{
					unload = false;
				}
			}

			// If property provider, allow property prov cleanup to run
			if (miVector.propMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				rc=miVector.propMI->ft->cleanup(miVector.propMI, &eCtx, false);
				if (rc.rc == CMPI_RC_ERR_NOT_SUPPORTED
					|| rc.rc == CMPI_RC_DO_NOT_UNLOAD
					|| rc.rc == CMPI_RC_NEVER_UNLOAD)
				{
					unload = false;
				}
			}

			// If indication provider, allow indication prov cleanup to run
			if (miVector.indMI)
			{
				::CMPIOperationContext context;
				CMPI_ContextOnStack eCtx(context);
				CMPI_ThreadContext thr(&_broker, &eCtx);
				rc=miVector.indMI->ft->cleanup(miVector.indMI, &eCtx, false);
				if (rc.rc == CMPI_RC_ERR_NOT_SUPPORTED
					|| rc.rc == CMPI_RC_DO_NOT_UNLOAD
					|| rc.rc == CMPI_RC_NEVER_UNLOAD)
				{
					unload = false;
				}
			}

			if (unload)
			{
				OW_LOG_DEBUG2(env->getLogger(COMPONENT_NAME), Format(
						"Unloading CMPI Provider %1", it->first));
				it->second.setNull();
				m_provs.erase(it++);
				continue;
			}
		}
		it++;
	}
}


} // end namespace OW_NAMESPACE

OW_PROVIDERIFCFACTORY(OpenWBEM::CMPIProviderIFC, cmpi)

