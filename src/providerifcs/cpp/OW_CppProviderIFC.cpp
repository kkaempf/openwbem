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

/**
 * @author Jon Carey
 * @author Dan Nuffer
 */

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
#include "OW_Assertion.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Condition.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_RepositoryIFC.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(CppProviderIFC);
OW_DEFINE_EXCEPTION_WITH_ID(CppProviderIFC);

//#include <setjmp.h>
typedef CppProviderBaseIFC* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();
namespace
{
const String COMPONENT_NAME("ow.provider.cpp.ifc");

class ProvRegEnv : public ProviderRegistrationEnvironmentIFC
{
public:
	ProvRegEnv(const ProviderEnvironmentIFCRef& env)
		: ProviderRegistrationEnvironmentIFC()
		, m_env(env)
	{
	}

	virtual ~ProvRegEnv()
	{
	}

	virtual RepositoryIFCRef getRepository() const
	{
		return m_env->getRepository();
	}

	virtual LoggerRef getLogger(const String& componentName) const
	{
		return m_env->getLogger(componentName);
	}

	virtual String getConfigItem(const String &name, const String& defRetVal="") const
	{
		return m_env->getConfigItem(name, defRetVal);
	}

	virtual StringArray getMultiConfigItem(const String &itemName, 
		const StringArray& defRetVal, const char* tokenizeSeparator = 0) const
	{
		return m_env->getMultiConfigItem(itemName, defRetVal, tokenizeSeparator);
	}

	virtual ProviderRegistrationEnvironmentIFCRef clone() const
	{
		return ProviderRegistrationEnvironmentIFCRef(new ProvRegEnv(m_env));
	}

private:
	ProviderEnvironmentIFCRef m_env;
};

ProviderRegistrationEnvironmentIFCRef
createProvRegEnv(const ProviderEnvironmentIFCRef& env)
{
	return ProviderRegistrationEnvironmentIFCRef(new ProvRegEnv(env));
}

}		// Enf of anonymous namespace

const char* const CppProviderIFC::CREATIONFUNC = "createProvider";

//////////////////////////////////////////////////////////////////////////////
class CppProviderIFC::CppProviderInitializationHelper : public IntrusiveCountableBase
{
public:

	explicit CppProviderInitializationHelper(const CppProviderBaseIFCRef& provider)
		: m_initialized(false)
		, m_initializeFailed(false)
		, m_provider(provider)
	{

	}

	// returns false if initialization failed
	bool waitUntilInitialized() const
	{
		NonRecursiveMutexLock l(m_initializedGuard);
		while (!m_initialized && !m_initializeFailed)
		{
			m_initializedCond.wait(l);
		}
		return !m_initializeFailed;
	}
	
	void initialize(const ProviderEnvironmentIFCRef& env)
	{
		try
		{
			m_provider->initialize(env);
		}
		catch (...)
		{
			NonRecursiveMutexLock l(m_initializedGuard);
			m_initializeFailed = true;
			m_initializedCond.notifyAll();
			throw;
		}

		NonRecursiveMutexLock l(m_initializedGuard);
		m_initialized = true;
		m_initializedCond.notifyAll();
	}

	CppProviderBaseIFCRef getProvider() const
	{
		return m_provider;
	}
private:
	bool m_initialized;
	bool m_initializeFailed;
	mutable NonRecursiveMutex m_initializedGuard;
	mutable Condition m_initializedCond;
	CppProviderBaseIFCRef m_provider;
};

//////////////////////////////////////////////////////////////////////////////
CppProviderIFC::CppProviderIFC()
	: ProviderIFCBaseIFC()
	, m_provs()
	, m_guard()
	, m_noUnloadProviders()
	, m_loadDone(false)
{
}
//////////////////////////////////////////////////////////////////////////////
CppProviderIFC::~CppProviderIFC()
{
	try
	{
		ProviderMap::iterator it = m_provs.begin();
		while (it != m_provs.end())
		{
			it->second = 0;
			it++;
		}
	
		m_provs.clear();
	
		for (size_t i = 0; i < m_noUnloadProviders.size(); i++)
		{
			m_noUnloadProviders[i].setNull();
		}
	
		m_noUnloadProviders.clear();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CppProviderIFC::doInit(const ProviderEnvironmentIFCRef& env,
	InstanceProviderInfoArray& i,
	SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	AssociatorProviderInfoArray& a,
#endif
	MethodProviderInfoArray& m,
	IndicationProviderInfoArray& ind)
{
	loadProviders(env, i,
		si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		a,
#endif
		m,
		ind);
}
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
CppProviderIFC::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		CppInstanceProviderIFC* pIP = pProv->getInstanceProvider();
		if (pIP)
		{
			OW_LOG_DEBUG(lgr, Format("CPPProviderIFC found instance provider %1", provIdString));
			CppInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
//			ipRef.useRefCountOf(pProv);
			return InstanceProviderIFCRef(new CppInstanceProviderProxy(ipRef));
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not an instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
SecondaryInstanceProviderIFCRef
CppProviderIFC::doGetSecondaryInstanceProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		CppSecondaryInstanceProviderIFC* pIP = pProv->getSecondaryInstanceProvider();
		if (pIP)
		{
			OW_LOG_DEBUG(lgr, 
				Format("CPPProviderIFC found secondary instance provider %1", provIdString));
			CppSecondaryInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
//			ipRef.useRefCountOf(pProv);
			return SecondaryInstanceProviderIFCRef(new CppSecondaryInstanceProviderProxy(ipRef));
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not a secondary instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
IndicationExportProviderIFCRefArray
CppProviderIFC::doGetIndicationExportProviders(const ProviderEnvironmentIFCRef&)
{
	IndicationExportProviderIFCRefArray rvra;
	for (size_t i = 0; i < m_noUnloadProviders.size(); i++)
	{
		CppProviderBaseIFCRef pProv = m_noUnloadProviders[i];
		CppIndicationExportProviderIFC* pIEP =
			pProv->getIndicationExportProvider();
		if (pIEP)
		{
			CppIndicationExportProviderIFCRef iepRef(pProv.getLibRef(), pIEP);
//			iepRef.useRefCountOf(pProv);
			rvra.append(
				IndicationExportProviderIFCRef(new
					CppIndicationExportProviderProxy(iepRef)));
		}
	}
	return rvra;
}
//////////////////////////////////////////////////////////////////////////////
PolledProviderIFCRefArray
CppProviderIFC::doGetPolledProviders(const ProviderEnvironmentIFCRef&)
{
	PolledProviderIFCRefArray rvra;
	for (size_t i = 0; i < m_noUnloadProviders.size(); i++)
	{
		CppProviderBaseIFCRef pProv = m_noUnloadProviders[i];
		CppPolledProviderIFC* pPP = pProv->getPolledProvider();
		if (pPP)
		{
			CppPolledProviderIFCRef ppRef(pProv.getLibRef(), pPP);
//			ppRef.useRefCountOf(pProv);
			rvra.append(
				PolledProviderIFCRef(new
					CppPolledProviderProxy(ppRef)));
		}
	}
	return rvra;
}
//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
CppProviderIFC::doGetMethodProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		CppMethodProviderIFC* pMP = pProv->getMethodProvider();
		if (pMP)
		{
			OW_LOG_DEBUG(lgr, Format("CPPProviderIFC found method provider %1",
				provIdString));
			CppMethodProviderIFCRef mpRef(pProv.getLibRef(), pMP);
//			mpRef.useRefCountOf(pProv);
			return MethodProviderIFCRef(
				new CppMethodProviderProxy(mpRef));
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not a method provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
CppProviderIFC::doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		CppAssociatorProviderIFC* pAP = pProv->getAssociatorProvider();
		if (pAP)
		{
			OW_LOG_DEBUG(lgr, Format("CPPProviderIFC found associator provider %1",
				provIdString));
			CppAssociatorProviderIFCRef apRef(pProv.getLibRef(), pAP);
//			apRef.useRefCountOf(pProv);
			return AssociatorProviderIFCRef(new
				CppAssociatorProviderProxy(apRef));
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not an associator provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#endif
//////////////////////////////////////////////////////////////////////////////
IndicationProviderIFCRef
CppProviderIFC::doGetIndicationProvider(const ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	CppProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		Logger lgr(COMPONENT_NAME);
		CppIndicationProviderIFC* pAP = pProv->getIndicationProvider();
		if (pAP)
		{
			OW_LOG_DEBUG(lgr, Format("CPPProviderIFC found indication provider %1",
				provIdString));

			IndicationProviderMap::const_iterator ci = m_indicationProviders.find(provIdString);
			if (ci != m_indicationProviders.end())
			{
				return ci->second;
			}

			CppIndicationProviderIFCRef apRef(pProv.getLibRef(), pAP);
//			apRef.useRefCountOf(pProv);
			IndicationProviderIFCRef rv(new
				CppIndicationProviderProxy(apRef));
			
			m_indicationProviders.insert(IndicationProviderMap::value_type(provIdString, rv));

			return rv;
		}
		OW_LOG_ERROR(lgr, Format("Provider %1 is not an indication provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
void
CppProviderIFC::loadProviders(const ProviderEnvironmentIFCRef& env,
	InstanceProviderInfoArray& instanceProviderInfo,
	SecondaryInstanceProviderInfoArray& secondaryInstanceProviderInfo,
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

	Logger lgr(COMPONENT_NAME);
	// Create the Shared library loader

	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		const char* msg = "C++ provider ifc failed to get shared lib loader";
		OW_LOG_ERROR(lgr, msg);
		OW_THROW(CppProviderIFCException, msg);
	}

	StringArray paths = env->getMultiConfigItem(
		ConfigOpts::CPPPROVIFC_PROV_LOCATION_opt, 
		String(OW_DEFAULT_CPPPROVIFC_PROV_LOCATION).tokenize(OW_PATHNAME_SEPARATOR), 
		OW_PATHNAME_SEPARATOR);
	for (StringArray::size_type i1 = 0; i1 < paths.size(); i1++)
	{
		StringArray dirEntries;
		if (!FileSystem::getDirectoryContents(paths[i1], dirEntries))
		{
			String msg(Format("C++ provider ifc failed getting contents of directory: %1", paths[i1]));
			OW_LOG_INFO(env->getLogger(COMPONENT_NAME), msg);
			continue; 
		}
		for (size_t i = 0; i < dirEntries.size(); i++)
		{
			if (!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
			{
				continue;
			}
			String libName = paths[i1];
			libName += OW_FILENAME_SEPARATOR;
			libName += dirEntries[i];

#ifdef OW_DARWIN
			if (!FileSystem::isLink(libName))
			{
				continue;
			}
#endif // OW_DARWIN
	
			SharedLibraryRef theLib = ldr->loadSharedLibrary(libName);
			if (!theLib)
			{
				String msg(Format("C++ provider ifc failed to load library: %1", libName));
				OW_LOG_ERROR(lgr, "****************************************");
				OW_LOG_ERROR(lgr, msg);
				OW_LOG_ERROR(lgr, "****************************************");
				OW_THROW(CppProviderIFCException, msg.c_str());
			}
			versionFunc_t versFunc;
			if (!theLib->getFunctionPointer("getOWVersion",
				versFunc))
			{
				String msg(Format("C++ provider ifc failed getting function pointer to \"getOWVersion\" from"
					" library: %1", libName));
				OW_LOG_INFO(lgr, msg);
				// This isn't treated as a fatal error, since the library isn't an OW library.
				// On Windows, providers that link to auxiliary dlls, must have those dlls in the same directory 
				// as the provider.
				continue;
			}
			const char* strVer = (*versFunc)();
			if (strcmp(strVer, OW_VERSION))
			{
				String msg(Format("C++ provider ifc got invalid version from provider: %1", strVer));
				OW_LOG_ERROR(lgr, "****************************************");
				OW_LOG_ERROR(lgr, msg);
				msg = Format("C++ provider ifc version: %1  provider version: %2  library: %3",
					OW_VERSION, strVer, libName);
				OW_LOG_ERROR(lgr, msg);
				OW_LOG_ERROR(lgr, "****************************************");
				OW_THROW(CppProviderIFCException, msg.c_str());
			}
			ProviderCreationFunc createProvider;
			String creationFuncName = String(CREATIONFUNC) + "NO_ID";
			if (!theLib->getFunctionPointer(creationFuncName,
				createProvider))
			{
				// it's not a no-id provider
				// try and load it as an id provider
				String providerid = dirEntries[i];
				// chop off lib and .so
				providerid = providerid.substring(3,
					providerid.length() - (strlen(OW_SHAREDLIB_EXTENSION) + 3));

				CppProviderBaseIFCRef p = getProvider(env, providerid.c_str(),
					dontStoreProvider, dontInitializeProvider);

				if (!p)
				{
					String msg(Format("C++ provider ifc: Libary %1 does not load", libName));
					OW_LOG_DEBUG(lgr, "****************************************");
					OW_LOG_DEBUG(lgr, msg);
					OW_LOG_DEBUG(lgr, "****************************************");
					OW_THROW(CppProviderIFCException, msg.c_str());
				}

				// The named provider may also be an indication export or a
				// polled provider. If so, we'll store a reference to it in
				// the m_noUnloadProviders and we'll skip unload processing on
				// it when doUnloadProviders is called. We'll skip this
				// processing by checking the persist flag on the provider.
				CppPolledProviderIFC* p_polledProv = p->getPolledProvider();
				CppIndicationExportProviderIFC* p_indExpProv =
					p->getIndicationExportProvider();
				if (p_polledProv || p_indExpProv)
				{
					if (p_indExpProv)
					{
						OW_LOG_DEBUG(lgr, Format("C++ provider ifc loaded indication export provider from lib: %1 - initializing", libName));
					}

					if (p_polledProv)
					{
						OW_LOG_DEBUG(lgr, Format("C++ provider ifc loaded polled provider from lib: %1 - initializing", libName));
					}
					CppProviderInitializationHelperRef p2(new CppProviderInitializationHelper(p));
					p2->initialize(env);
					p->setPersist(true);
					m_noUnloadProviders.append(p);
					m_provs[providerid] = p2;
				}

				CppInstanceProviderIFC* p_ip = p->getInstanceProvider();
				if (p_ip)
				{
					InstanceProviderInfo info;
					info.setProviderName(providerid);
					p_ip->getInstanceProviderInfoWithEnv(
						createProvRegEnv(env), info);
					instanceProviderInfo.push_back(info);
				}

				CppSecondaryInstanceProviderIFC* p_sip =
					p->getSecondaryInstanceProvider();

				if (p_sip)
				{
					SecondaryInstanceProviderInfo info;
					info.setProviderName(providerid);
					p_sip->getSecondaryInstanceProviderInfoWithEnv(
						createProvRegEnv(env), info);
					secondaryInstanceProviderInfo.push_back(info);
				}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
				CppAssociatorProviderIFC* p_ap = p->getAssociatorProvider();
				if (p_ap)
				{
					AssociatorProviderInfo info;
					info.setProviderName(providerid);
					p_ap->getAssociatorProviderInfoWithEnv(
						createProvRegEnv(env), info);
					associatorProviderInfo.push_back(info);
				}
#endif
				CppMethodProviderIFC* p_mp = p->getMethodProvider();
				if (p_mp)
				{
					MethodProviderInfo info;
					info.setProviderName(providerid);
					p_mp->getMethodProviderInfoWithEnv(
						createProvRegEnv(env), info);
					methodProviderInfo.push_back(info);
				}
				CppIndicationProviderIFC* p_indp = p->getIndicationProvider();
				if (p_indp)
				{
					IndicationProviderInfo info;
					info.setProviderName(providerid);
					p_indp->getIndicationProviderInfoWithEnv(
						createProvRegEnv(env), info);
					indicationProviderInfo.push_back(info);
				}

				continue;
			}

			// If we reach this point, the NO_ID provider factory was used. This
			// means that the provider doesn't have an identifier (i.e. doesn't
			// do instance, methods, associators and such.
			AutoPtr<CppProviderBaseIFC> pProv((*createProvider)());
			if (!pProv.get())
			{
				String msg(Format("C++ provider ifc: Libary %1 - %2 returned null provider", libName, creationFuncName));
				OW_LOG_ERROR(lgr, "****************************************");
				OW_LOG_ERROR(lgr, msg);
				OW_LOG_ERROR(lgr, "****************************************");
				OW_THROW(CppProviderIFCException, msg.c_str());
			}
			CppPolledProviderIFC* p_itp = pProv->getPolledProvider();
			CppIndicationExportProviderIFC* p_iep =
				pProv->getIndicationExportProvider();
			if (p_itp || p_iep)
			{
				if (p_iep)
				{
					OW_LOG_DEBUG(lgr, Format("C++ provider ifc loaded indication export provider from lib: %1 - initializing", libName));
				}
				if (p_itp)
				{
					OW_LOG_DEBUG(lgr, Format("C++ provider ifc loaded  polled provider from lib: %1 - initializing",
						libName));
				}

				pProv->initialize(env);
				m_noUnloadProviders.append(CppProviderBaseIFCRef(theLib,
					pProv.release()));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//static
CppProviderBaseIFCRef
CppProviderIFC::loadProvider(const String& libName, LoggerRef lgr)
{
	return loadProvider(libName);
}

//////////////////////////////////////////////////////////////////////////////
//static
CppProviderBaseIFCRef
CppProviderIFC::loadProvider(const String& libName)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG(logger, Format("CppProviderIFC::loadProvider() called for \"%1\"", libName));
	String provId = libName.substring(libName.lastIndexOf(OW_FILENAME_SEPARATOR)+1);
	// chop of lib and .so
	if( provId.startsWith("lib") )
	{
		provId = provId.substring(3);
	}
	if( provId.endsWith(OW_SHAREDLIB_EXTENSION) )
	{
		provId = provId.substring(0, provId.length() - strlen(OW_SHAREDLIB_EXTENSION));
	}
	// Trim it at the first '.' to handle cases where loadProvider was requested
	// on something that may have trailing garbage after (such as libraries on
	// MacOS X, where the version number is before the library extension.
	provId = provId.substring(0, provId.indexOf('.')); 

	OW_LOG_DEBUG(logger, Format("Provider ID is \"%1\"", provId));

	SharedLibraryLoaderRef ldr = SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		OW_LOG_ERROR(logger, "C++ provider ifc FAILED to get shared lib loader");
		return CppProviderBaseIFCRef();
	}

	OW_LOG_DEBUG(logger, Format("CppProviderIFC::loadProvider loading library: %1", libName));

	SharedLibraryRef theLib = ldr->loadSharedLibrary(libName);

	if (!theLib)
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc failed loading library %1.", libName));
		return CppProviderBaseIFCRef();
	}

	versionFunc_t versFunc;
	if (!theLib->getFunctionPointer("getOWVersion", versFunc))
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc failed getting function pointer to \"getOWVersion\" from library %1.", libName));
		return CppProviderBaseIFCRef();
	}
	const char* strVer = (*versFunc)();
	if (strcmp(strVer, OW_VERSION))
	{
		OW_LOG_ERROR(logger, "C++ provider ifc got invalid version from provider");
		OW_LOG_ERROR(logger, Format("C++ provider ifc version: %1  provider version: %2  library: %3",
					OW_VERSION, strVer, libName));
		return CppProviderBaseIFCRef();
	}

	ProviderCreationFunc createProvider;
	String creationFuncName = String(CREATIONFUNC) + provId;
	if (!theLib->getFunctionPointer(creationFuncName, createProvider))
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc: Libary %1 does not contain %2 function.",
			libName, creationFuncName));
		return CppProviderBaseIFCRef();
	}

	CppProviderBaseIFC* pProv = (*createProvider)();

	if (!pProv)
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc: Libary %1 -"
			" %2 returned null provider. Not loaded.", libName, creationFuncName));
		return CppProviderBaseIFCRef();
	}

	CppProviderBaseIFCRef rval(theLib, pProv);

	OW_LOG_DEBUG(logger, Format("C++ provider ifc successfully loaded library %1 for provider %2", libName, provId));

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
CppProviderBaseIFCRef
CppProviderIFC::getProvider(
	const ProviderEnvironmentIFCRef& env, const char* provIdString,
	StoreProviderFlag storeP, InitializeProviderFlag initP)
{
	OW_ASSERT((initP == initializeProvider && storeP == storeProvider) || (initP == dontInitializeProvider && storeP == dontStoreProvider));

	MutexLock ml(m_guard);

	String provId(provIdString);
	ProviderMap::iterator it = m_provs.find(provId);
	if (it != m_provs.end())
	{
		// make a copy in case the map gets modified when we unlock m_guard
		CppProviderInitializationHelperRef prov(it->second);
		// do this to prevent a deadlock
		ml.release();

		// another thread may be initializing the provider, wait until it's done.
		if (prov->waitUntilInitialized())
		{
			return prov->getProvider();
		}
		else
		{
			// initialization failed, it's now out of the map.
			return CppProviderBaseIFCRef();
		}
	}

	String libName;
	CppProviderBaseIFCRef rval;
	Logger lgr(COMPONENT_NAME);

	StringArray paths = env->getMultiConfigItem(
		ConfigOpts::CPPPROVIFC_PROV_LOCATION_opt, 
		String(OW_DEFAULT_CPPPROVIFC_PROV_LOCATION).tokenize(OW_PATHNAME_SEPARATOR), 
		OW_PATHNAME_SEPARATOR);
	for (StringArray::size_type i = 0; i < paths.size(); i++)
	{
		libName = paths[i];
		libName += OW_FILENAME_SEPARATOR;
		libName += "lib";
		libName += provId;
		libName += OW_SHAREDLIB_EXTENSION;

		if (!FileSystem::exists(libName))
		{
			continue;
		}

		rval = loadProvider(libName);

		if (rval)
		{
			break;
		}

	}

	if (!rval)
	{
		OW_LOG_ERROR(lgr, Format("C++ provider ifc failed to load library: %1 for provider id %2.", libName, provId));
		return rval;
	}

	if (initP == initializeProvider && storeP == storeProvider)
	{
		CppProviderInitializationHelperRef provInitHelper(new CppProviderInitializationHelper(rval));

		m_provs[provId] = provInitHelper;
		
		// now it's in the map, we can unlock the mutex protecting the map
		ml.release();

		OW_LOG_DEBUG(lgr, Format("C++ provider ifc calling initialize for provider %1", provId));

		try
		{
			provInitHelper->initialize(env); // Let provider initialize itself
		}
		catch (...)
		{
			// provider initialization failed, we need to take it out of the map
			MutexLock lock(m_guard);
			m_provs.erase(provId);
			throw;
		}

		OW_LOG_DEBUG(lgr, Format("C++ provider ifc: provider %1 loaded and initialized", provId));

	}
	else
	{
		OW_LOG_DEBUG(lgr, Format("C++ provider ifc: provider %1 loaded but not initialized", provId));
	}

	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
CppProviderIFC::doUnloadProviders(const ProviderEnvironmentIFCRef& env)
{
	String timeWindow = env->getConfigItem(ConfigOpts::CPPPROVIFC_PROV_TTL_opt, OW_DEFAULT_CPPPROVIFC_PROV_TTL);
	Int32 iTimeWindow;
	try
	{
		iTimeWindow = timeWindow.toInt32();
	}
	catch (const StringConversionException&)
	{
		iTimeWindow = String(OW_DEFAULT_CPPPROVIFC_PROV_TTL).toInt32();
	}
	if (iTimeWindow < 0)
	{
		return;
	}
	DateTime dt;
	dt.setToCurrent();
	MutexLock l(m_guard);
	for (ProviderMap::iterator iter = m_provs.begin();
		  iter != m_provs.end();)
	{
		// If this is not a persistent provider, see if we can unload it.
		if (!iter->second->getProvider()->getPersist())
		{
			DateTime provDt = iter->second->getProvider()->getLastAccessTime();
			provDt.addMinutes(iTimeWindow);
			if (provDt < dt && iter->second->getProvider()->canUnload())
			{
				Logger lgr(COMPONENT_NAME);
				OW_LOG_INFO(lgr, Format("Unloading Provider %1", iter->first));
				iter->second = 0;
				m_provs.erase(iter++);
				continue;
			}
		}

		++iter;
	}
}

void CppProviderIFC::doShuttingDown(const ProviderEnvironmentIFCRef& env)
{
	// Need to lock m_guard while accessing m_provs, but we can't leave the mutex locked
	// while calling shuttingDown(), since that might cause a deadlock, so make a copy.
	MutexLock l(m_guard);
	ProviderMap provsCopy(m_provs);
	LoadedProviderArray noUnloadProvidersCopy(m_noUnloadProviders);
	l.release();

	Logger lgr(COMPONENT_NAME);
	ProviderMap::iterator it, itend = provsCopy.end();
	for (it = provsCopy.begin(); it != itend; ++it)
	{
		try
		{
			it->second->getProvider()->shuttingDown(env);
		}
		catch (Exception& e)
		{
			OW_LOG_ERROR(lgr, Format("Caught exception while calling shuttingDown() on provider: %1", e));
		}
	}

	// call shuttingDown() for any providers in noUnloadProviders which *aren't* in provsCopy,
	// so we don't call it twice for one provider
	for (LoadedProviderArray::iterator curProv = noUnloadProvidersCopy.begin(); curProv != noUnloadProvidersCopy.end(); ++curProv)
	{
		bool found(false);

		for (it = provsCopy.begin(); it != itend; ++it)
		{
			if (it->second->getProvider() == *curProv)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			try
			{
				(*curProv)->shuttingDown(env);
			}
			catch (Exception& e)
			{
				OW_LOG_ERROR(lgr, Format("Caught exception while calling shuttingDown() on provider: %1", e));
			}
		}
	}
}

} // end namespace OW_NAMESPACE

