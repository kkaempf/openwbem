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
#include "OWBI1_ProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OWBI1_ProxyProvider.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_Assertion.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Condition.hpp"
#include "OW_ExceptionIds.hpp"
#include "OWBI1_ProviderEnvironmentIFC.hpp"
#include "OWBI1_OperationContext.hpp"
#include "OW_OperationContext.hpp"
#include "OWBI1_InstanceProviderInfo.hpp"
#include "OWBI1_SecondaryInstanceProviderInfo.hpp"
#include "OWBI1_AssociatorProviderInfo.hpp"
#include "OWBI1_MethodProviderInfo.hpp"
#include "OWBI1_IndicationProviderInfo.hpp"
#include "OWBI1_CIMOMHandleIFC.hpp"
#include "OWBI1_Logger.hpp"
#include "OWBI1_SessionLanguage.hpp"

namespace OWBI1
{

//using namespace OpenWBEM;

OW_DECLARE_EXCEPTION(BI1ProviderIFC);
OW_DEFINE_EXCEPTION_WITH_ID(BI1ProviderIFC);

typedef BI1ProviderBaseIFC* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();
namespace
{
	const OpenWBEM::String COMPONENT_NAME("ow.provider.cpp.ifc");

	class OperationContextWrapper : public OperationContext
	{
	public:
		OperationContextWrapper(OpenWBEM::OperationContext& oc)
			: m_oc(oc)
		{
		}
		/**
		 * Remove the data identified by key.  It is not an error if key has not
		 * already been added to the context with setData().
		 * @param key Identifies the data to remove.
		 */
		virtual void removeData(const String& key)
		{
			m_oc.removeData(key.c_str());
		}

		/**
		 * Test whether there is data for the key.
		 * @param key The key to test.
		 * @return true if there is data for the key.
		 */
		virtual bool keyHasData(const String& key) const
		{
			return m_oc.keyHasData(key.c_str());
		}

		/**
		 * These are for convenience, and are implemented in terms of
		 * the first 2 functions.
		 */
		virtual void setStringData(const String& key, const String& str)
		{
			m_oc.setStringData(key.c_str(), str.c_str());
		}

		/**
		 * @throws ContextDataNotFoundException if key is not found
		 */
		virtual String getStringData(const String& key) const
		{
			return m_oc.getStringData(key.c_str()).c_str();
		}

		/**
		 * @returns def if key is not found
		 */
		virtual String getStringDataWithDefault(const String& key, const String& def = String() ) const
		{
			return m_oc.getStringDataWithDefault(key.c_str(), def.c_str()).c_str();
		}

	private:
		OpenWBEM::OperationContext& m_oc;
	};

	// hmm this needs it's own file, since the Proxy classes will use it too.
	// TODO: Write it.
	class ProvEnvWrapper : public OWBI1::ProviderEnvironmentIFC
	{
	public:
		ProvEnvWrapper(const OpenWBEM::ProviderEnvironmentIFCRef& env)
			: m_env(env)
			, m_context(new OperationContextWrapper(m_env->getOperationContext()))
		{
		}

		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return CIMOMHandleIFCRef();
		}
		virtual LoggerRef getLogger(const String& componentName) const
		{
			return LoggerRef();
		}
		virtual String getConfigItem(const String &name, const String& defRetVal="") const
		{
			return m_env->getConfigItem(name.c_str(), defRetVal.c_str()).c_str();
		}
		virtual String getUserName() const
		{
			return "";
		}
		virtual OperationContext& getOperationContext()
		{
			return *m_context;
		}
		virtual SessionLanguageRef getSessionLanguage() const
		{
			return SessionLanguageRef();
		}

	private:
		OpenWBEM::ProviderEnvironmentIFCRef m_env;
		IntrusiveReference<OperationContext> m_context;
	};

	OWBI1::ProviderEnvironmentIFCRef wrapProvEnv(const OpenWBEM::ProviderEnvironmentIFCRef& env)
	{
		return OWBI1::ProviderEnvironmentIFCRef(new ProvEnvWrapper(env));
	}

	OpenWBEM::InstanceProviderInfo convertInstanceProviderInfo(const OWBI1::InstanceProviderInfo& in)
	{
		//TODO:
		return OpenWBEM::InstanceProviderInfo();
	}
	OpenWBEM::SecondaryInstanceProviderInfo convertSecondaryInstanceProviderInfo(const OWBI1::SecondaryInstanceProviderInfo& in)
	{
		//TODO:
		return OpenWBEM::SecondaryInstanceProviderInfo();
	}
	OpenWBEM::AssociatorProviderInfo convertAssociatorProviderInfo(const OWBI1::AssociatorProviderInfo& in)
	{
		//TODO:
		return OpenWBEM::AssociatorProviderInfo();
	}
	OpenWBEM::MethodProviderInfo convertMethodProviderInfo(const OWBI1::MethodProviderInfo& in)
	{
		//TODO:
		return OpenWBEM::MethodProviderInfo();
	}
	OpenWBEM::IndicationProviderInfo convertIndicationProviderInfo(const OWBI1::IndicationProviderInfo& in)
	{
		//TODO:
		return OpenWBEM::IndicationProviderInfo();
	}

}
const char* const BI1ProviderIFC::CREATIONFUNC = "createProvider";

//////////////////////////////////////////////////////////////////////////////
class BI1ProviderIFC::BI1ProviderInitializationHelper : public IntrusiveCountableBase
{
public:

	explicit BI1ProviderInitializationHelper(const BI1ProviderBaseIFCRef& provider)
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
			OpenWBEM::NonRecursiveMutexLock l(m_initializedGuard);
			m_initializeFailed = true;
			m_initializedCond.notifyAll();
			throw;
		}

		OpenWBEM::NonRecursiveMutexLock l(m_initializedGuard);
		m_initialized = true;
		m_initializedCond.notifyAll();
	}

	BI1ProviderBaseIFCRef getProvider() const
	{
		return m_provider;
	}
private:
	bool m_initialized;
	bool m_initializeFailed;
	mutable OpenWBEM::NonRecursiveMutex m_initializedGuard;
	mutable OpenWBEM::Condition m_initializedCond;
	BI1ProviderBaseIFCRef m_provider;
};

//////////////////////////////////////////////////////////////////////////////
BI1ProviderIFC::BI1ProviderIFC()
	: m_loadDone(false)
{
}
//////////////////////////////////////////////////////////////////////////////
BI1ProviderIFC::~BI1ProviderIFC()
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
		m_indicationExportProviders.clear();
		m_polledProviders.clear();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
void
BI1ProviderIFC::doInit(const OpenWBEM::ProviderEnvironmentIFCRef& env,
	OpenWBEM::InstanceProviderInfoArray& i,
	OpenWBEM::SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	OpenWBEM::AssociatorProviderInfoArray& a,
#endif
	OpenWBEM::MethodProviderInfoArray& m,
	OpenWBEM::IndicationProviderInfoArray& ind)
{
#ifdef OW_DISABLE_ASSOCIATION_TRAVERSAL
	OpenWBEM::AssociatorProviderInfoArray a;
#endif

	loadProviders(env, i,
		si,
		a,
		m,
		ind);
}
//////////////////////////////////////////////////////////////////////////////
OpenWBEM::InstanceProviderIFCRef
BI1ProviderIFC::doGetInstanceProvider(const OpenWBEM::ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	BI1ProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		BI1InstanceProviderIFC* pIP = pProv->getInstanceProvider();
		if (pIP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), 
				Format("OWBI1ProviderIFC found instance provider %1", provIdString));
			BI1InstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
			return OpenWBEM::InstanceProviderIFCRef(new BI1InstanceProviderProxy(ipRef));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
OpenWBEM::SecondaryInstanceProviderIFCRef
BI1ProviderIFC::doGetSecondaryInstanceProvider(const OpenWBEM::ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	BI1ProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		BI1SecondaryInstanceProviderIFC* pIP = pProv->getSecondaryInstanceProvider();
		if (pIP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), 
				Format("OWBI1ProviderIFC found secondary instance provider %1", provIdString));
			BI1SecondaryInstanceProviderIFCRef ipRef(pProv.getLibRef(), pIP);
			return OpenWBEM::SecondaryInstanceProviderIFCRef(new BI1SecondaryInstanceProviderProxy(ipRef));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not a secondary instance provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
OpenWBEM::IndicationExportProviderIFCRefArray
BI1ProviderIFC::doGetIndicationExportProviders(const OpenWBEM::ProviderEnvironmentIFCRef&)
{
	return m_indicationExportProviders;
	/*
	OpenWBEM::IndicationExportProviderIFCRefArray rvra;
	for (size_t i = 0; i < m_noUnloadProviders.size(); i++)
	{
		BI1ProviderBaseIFCRef pProv = m_noUnloadProviders[i];
		BI1IndicationExportProviderIFC* pIEP =
			pProv->getIndicationExportProvider();
		if (pIEP)
		{
			BI1IndicationExportProviderIFCRef iepRef(pProv.getLibRef(), pIEP);
			rvra.append(OpenWBEM::IndicationExportProviderIFCRef(new BI1IndicationExportProviderProxy(iepRef)));
		}
	}
	return rvra;
	*/
}
//////////////////////////////////////////////////////////////////////////////
OpenWBEM::PolledProviderIFCRefArray
BI1ProviderIFC::doGetPolledProviders(const OpenWBEM::ProviderEnvironmentIFCRef&)
{
	return m_polledProviders;
	/*
	OpenWBEM::PolledProviderIFCRefArray rvra;
	for (size_t i = 0; i < m_noUnloadProviders.size(); i++)
	{
		BI1ProviderBaseIFCRef pProv = m_noUnloadProviders[i];
		BI1PolledProviderIFC* pPP = pProv->getPolledProvider();
		if (pPP)
		{
			BI1PolledProviderIFCRef ppRef(pProv.getLibRef(), pPP);
			rvra.append(OpenWBEM::PolledProviderIFCRef(new BI1PolledProviderProxy(ppRef)));
		}
	}
	return rvra;
	*/
}
//////////////////////////////////////////////////////////////////////////////
OpenWBEM::MethodProviderIFCRef
BI1ProviderIFC::doGetMethodProvider(const OpenWBEM::ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	BI1ProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		BI1MethodProviderIFC* pMP = pProv->getMethodProvider();
		if (pMP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("OWBI1ProviderIFC found method provider %1",
				provIdString));
			BI1MethodProviderIFCRef mpRef(pProv.getLibRef(), pMP);
			return OpenWBEM::MethodProviderIFCRef(new BI1MethodProviderProxy(mpRef));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not a method provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
OpenWBEM::AssociatorProviderIFCRef
BI1ProviderIFC::doGetAssociatorProvider(const OpenWBEM::ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	BI1ProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		BI1AssociatorProviderIFC* pAP = pProv->getAssociatorProvider();
		if (pAP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("OWBI1ProviderIFC found associator provider %1",
				provIdString));
			BI1AssociatorProviderIFCRef apRef(pProv.getLibRef(), pAP);
			return OpenWBEM::AssociatorProviderIFCRef(new BI1AssociatorProviderProxy(apRef));
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an associator provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
#endif
//////////////////////////////////////////////////////////////////////////////
OpenWBEM::IndicationProviderIFCRef
BI1ProviderIFC::doGetIndicationProvider(const OpenWBEM::ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	BI1ProviderBaseIFCRef pProv = getProvider(env, provIdString);
	if (pProv)
	{
		BI1IndicationProviderIFC* pAP = pProv->getIndicationProvider();
		if (pAP)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("OWBI1ProviderIFC found indication provider %1",
				provIdString));

			IndicationProviderMap::const_iterator ci = m_indicationProviders.find(provIdString);
			if (ci != m_indicationProviders.end())
			{
				return ci->second;
			}

			BI1IndicationProviderIFCRef apRef(pProv.getLibRef(), pAP);
			OpenWBEM::IndicationProviderIFCRef rv(new BI1IndicationProviderProxy(apRef));
			
			m_indicationProviders.insert(IndicationProviderMap::value_type(provIdString, rv));

			return rv;
		}
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Provider %1 is not an indication provider",
			provIdString));
	}
	OW_THROW(NoSuchProviderException, provIdString);
}
//////////////////////////////////////////////////////////////////////////////
void
BI1ProviderIFC::loadProviders(const OpenWBEM::ProviderEnvironmentIFCRef& env,
	OpenWBEM::InstanceProviderInfoArray& instanceProviderInfo,
	OpenWBEM::SecondaryInstanceProviderInfoArray& secondaryInstanceProviderInfo,
	OpenWBEM::AssociatorProviderInfoArray& associatorProviderInfo,
	OpenWBEM::MethodProviderInfoArray& methodProviderInfo,
	OpenWBEM::IndicationProviderInfoArray& indicationProviderInfo)
{
	MutexLock ml(m_guard);
	if (m_loadDone)
	{
		return;
	}
	m_loadDone = true;

	// Create the Shared library loader

	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		const char* msg = "C++ provider ifc failed to get shared lib loader";
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), msg);
		OW_THROW(BI1ProviderIFCException, msg);
	}

	OpenWBEM::String libPathsStr = env->getConfigItem(
		ConfigOpts::OWBI1IFC_PROV_LOC_opt, OW_DEFAULT_OWBI1IFC_PROV_LOC);
#ifndef OW_WIN32
	OpenWBEM::StringArray paths = libPathsStr.tokenize(";:");
#else
	// On win32, can't use the : separator because that's the drive letter separator
	OpenWBEM::StringArray paths = libPathsStr.tokenize(";");
#endif
	for (OpenWBEM::StringArray::size_type i1 = 0; i1 < paths.size(); i1++)
	{
		OpenWBEM::StringArray dirEntries;
		if (!FileSystem::getDirectoryContents(paths[i1], dirEntries))
		{
			OpenWBEM::String msg(Format("C++ provider ifc failed getting contents of directory: %1", paths[i1]));
			OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), msg);
			OW_THROW(BI1ProviderIFCException, msg.c_str());
		}
		for (size_t i = 0; i < dirEntries.size(); i++)
		{
			if (!dirEntries[i].endsWith(OW_SHAREDLIB_EXTENSION))
			{
				continue;
			}
			OpenWBEM::String libName = paths[i1];
			libName += OW_FILENAME_SEPARATOR;
			libName += dirEntries[i];

#ifdef OW_DARWIN
			if (!FileSystem::isLink(libName))
			{
				continue;
			}
#endif // OW_DARWIN
	
			// try and load it

			SharedLibraryRef theLib = ldr->loadSharedLibrary(libName,
				env->getLogger(COMPONENT_NAME));
			if (!theLib)
			{
				OpenWBEM::String msg(Format("C++ provider ifc failed to load library: %1", libName));
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), msg);
				OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), "****************************************");
				OW_THROW(BI1ProviderIFCException, msg.c_str());
			}

			// chop off lib and .so
			OpenWBEM::String providerid = dirEntries[i].substring(3, providerid.length() - (strlen(OW_SHAREDLIB_EXTENSION) + 3));

			BI1ProviderBaseIFCRef p = getProvider(env, providerid.c_str(),
				dontStoreProvider, dontInitializeProvider);

			if (!p)
			{
				OpenWBEM::String msg(Format("C++ provider ifc: Libary %1 does not load", libName));
				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "****************************************");
				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), msg);
				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "****************************************");
				OW_THROW(BI1ProviderIFCException, msg.c_str());
			}

			BI1ProviderInitializationHelperRef p2;
			// The named provider may also be an indication export or a
			// polled provider. If so, we'll store a reference to it in
			// the m_polledProviders
			BI1PolledProviderIFC* p_polledProv = p->getPolledProvider();
			if (p_polledProv)
			{
				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), 
					Format("C++ provider ifc loaded polled provider from lib: %1 - initializing", libName));

				if (!p2)
				{
					p2 = BI1ProviderInitializationHelperRef(new BI1ProviderInitializationHelper(p));
					p2->initialize(wrapProvEnv(env));
					m_provs.insert(std::make_pair(providerid, p2));
				}
				BI1PolledProviderIFCRef ipRef(theLib, p_polledProv);
				m_polledProviders.push_back(OpenWBEM::PolledProviderIFCRef(new BI1PolledProviderProxy(ipRef)));
			}

			BI1IndicationExportProviderIFC* p_indExpProv =
				p->getIndicationExportProvider();
			if (p_indExpProv)
			{
				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), 
					Format("C++ provider ifc loaded indication export provider from lib: %1 - initializing", libName));

				if (!p2)
				{
					BI1ProviderInitializationHelperRef p2(new BI1ProviderInitializationHelper(p));
					p2->initialize(wrapProvEnv(env));
					m_provs.insert(std::make_pair(providerid, p2));
				}
				BI1IndicationExportProviderIFCRef iepRef(theLib, p_indExpProv);
				m_indicationExportProviders.push_back(OpenWBEM::IndicationExportProviderIFCRef(new BI1IndicationExportProviderProxy(iepRef)));
			}

			BI1InstanceProviderIFC* p_ip = p->getInstanceProvider();
			if (p_ip)
			{
				OWBI1::InstanceProviderInfo info;
				info.setProviderName(providerid.c_str());
				p_ip->getInstanceProviderInfo(info);
				instanceProviderInfo.push_back(convertInstanceProviderInfo(info));
			}

			BI1SecondaryInstanceProviderIFC* p_sip =
				p->getSecondaryInstanceProvider();

			if (p_sip)
			{
				OWBI1::SecondaryInstanceProviderInfo info;
				info.setProviderName(providerid.c_str());
				p_sip->getSecondaryInstanceProviderInfo(info);
				secondaryInstanceProviderInfo.push_back(convertSecondaryInstanceProviderInfo(info));
			}

			BI1AssociatorProviderIFC* p_ap = p->getAssociatorProvider();
			if (p_ap)
			{
				OWBI1::AssociatorProviderInfo info;
				info.setProviderName(providerid.c_str());
				p_ap->getAssociatorProviderInfo(info);
				associatorProviderInfo.push_back(convertAssociatorProviderInfo(info));
			}

			BI1MethodProviderIFC* p_mp = p->getMethodProvider();
			if (p_mp)
			{
				OWBI1::MethodProviderInfo info;
				info.setProviderName(providerid.c_str());
				p_mp->getMethodProviderInfo(info);
				methodProviderInfo.push_back(convertMethodProviderInfo(info));
			}
			BI1IndicationProviderIFC* p_indp = p->getIndicationProvider();
			if (p_indp)
			{
				OWBI1::IndicationProviderInfo info;
				info.setProviderName(providerid.c_str());
				p_indp->getIndicationProviderInfo(info);
				indicationProviderInfo.push_back(convertIndicationProviderInfo(info));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//static
BI1ProviderBaseIFCRef
BI1ProviderIFC::loadProvider(const OpenWBEM::String& libName, OpenWBEM::LoggerRef logger)
{
	OpenWBEM::String provId = libName.substring(libName.lastIndexOf(OW_FILENAME_SEPARATOR)+1);
	// chop of lib and .so
	provId = provId.substring(3, provId.length() - (strlen(OW_SHAREDLIB_EXTENSION) + 3));

	SharedLibraryLoaderRef ldr = SharedLibraryLoader::createSharedLibraryLoader();
	if (!ldr)
	{
		OW_LOG_ERROR(logger, "C++ provider ifc FAILED to get shared lib loader");
		return BI1ProviderBaseIFCRef();
	}

	OW_LOG_DEBUG(logger, Format("BI1ProviderIFC::loadProvider loading library: %1", libName));

	SharedLibraryRef theLib = ldr->loadSharedLibrary(libName, logger);

	versionFunc_t versFunc;
	if (!theLib->getFunctionPointer("getOWVersion", versFunc))
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc failed getting function pointer to \"getOWVersion\" from library %1.", libName));
		return BI1ProviderBaseIFCRef();
	}
	const char* strVer = (*versFunc)();
	if (strcmp(strVer, OW_VERSION))
	{
		OW_LOG_ERROR(logger, "C++ provider ifc got invalid version from provider");
		OW_LOG_ERROR(logger, Format("C++ provider ifc version: %1  provider version: %2  library: %3",
					OW_VERSION, strVer, libName));
		return BI1ProviderBaseIFCRef();
	}

	ProviderCreationFunc createProvider;
	OpenWBEM::String creationFuncName = OpenWBEM::String(CREATIONFUNC) + provId;
	if (!theLib->getFunctionPointer(creationFuncName, createProvider))
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc: Libary %1 does not contain %2 function.",
			libName, creationFuncName));
		return BI1ProviderBaseIFCRef();
	}

	BI1ProviderBaseIFC* pProv = (*createProvider)();

	if (!pProv)
	{
		OW_LOG_ERROR(logger, Format("C++ provider ifc: Libary %1 -"
			" %2 returned null provider. Not loaded.", libName, creationFuncName));
		return BI1ProviderBaseIFCRef();
	}

	BI1ProviderBaseIFCRef rval(theLib, pProv);

	OW_LOG_DEBUG(logger, Format("C++ provider ifc successfully loaded library %1 for provider %2", libName, provId));

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
BI1ProviderBaseIFCRef
BI1ProviderIFC::getProvider(
	const OpenWBEM::ProviderEnvironmentIFCRef& env, const char* provIdString,
	StoreProviderFlag storeP, InitializeProviderFlag initP)
{
	OW_ASSERT((initP == initializeProvider && storeP == storeProvider) || (initP == dontInitializeProvider && storeP == dontStoreProvider));

	MutexLock ml(m_guard);

	OpenWBEM::String provId(provIdString);
	ProviderMap::iterator it = m_provs.find(provId);
	if (it != m_provs.end())
	{
		// make a copy in case the map gets modified when we unlock m_guard
		BI1ProviderInitializationHelperRef prov(it->second);
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
			return BI1ProviderBaseIFCRef();
		}
	}

	OpenWBEM::String libName;
	BI1ProviderBaseIFCRef rval;

	OpenWBEM::String libPathsStr = env->getConfigItem(
		ConfigOpts::OWBI1IFC_PROV_LOC_opt, OW_DEFAULT_OWBI1IFC_PROV_LOC);
	OpenWBEM::StringArray paths = libPathsStr.tokenize(";:");
	for (OpenWBEM::StringArray::size_type i = 0; i < paths.size(); i++)
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

		rval = loadProvider(libName, env->getLogger(COMPONENT_NAME));

		if (rval)
		{
			break;
		}

	}

	if (!rval)
	{
		OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), 
			Format("C++ provider ifc failed to load library: %1 for provider id %2. Skipping.", libName, provId));
		return rval;
	}

	if (initP == initializeProvider && storeP == storeProvider)
	{
		BI1ProviderInitializationHelperRef provInitHelper(new BI1ProviderInitializationHelper(rval));

		m_provs[provId] = provInitHelper;
		
		// now it's in the map, we can unlock the mutex protecting the map
		ml.release();

		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), 
			Format("C++ provider ifc calling initialize for provider %1", provId));

		try
		{
			provInitHelper->initialize(wrapProvEnv(env)); // Let provider initialize itself
		}
		catch (...)
		{
			// provider initialization failed, we need to take it out of the map
			MutexLock lock(m_guard);
			m_provs.erase(provId);
			throw;
		}

		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), 
			Format("C++ provider ifc: provider %1 loaded and initialized", provId));

	}
	else
	{
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), 
			Format("C++ provider ifc: provider %1 loaded but not initialized", provId));
	}

	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
BI1ProviderIFC::doUnloadProviders(const OpenWBEM::ProviderEnvironmentIFCRef& env)
{
	OpenWBEM::String timeWindow = env->getConfigItem(ConfigOpts::OWBI1IFC_PROV_TTL_opt, OW_DEFAULT_OWBI1IFC_PROV_TTL);
	Int32 iTimeWindow;
	try
	{
		iTimeWindow = timeWindow.toInt32();
	}
	catch (const StringConversionException&)
	{
		iTimeWindow = String(OW_DEFAULT_OWBI1IFC_PROV_TTL).toInt32();
	}
	if (iTimeWindow < 0)
	{
		return;
	}
	DateTime dt;
	dt.setToCurrent();
	OpenWBEM::MutexLock l(m_guard);
	for (ProviderMap::iterator iter = m_provs.begin();
		  iter != m_provs.end();)
	{
		DateTime provDt = iter->second->getProvider()->getLastAccessTime();
		provDt.addMinutes(iTimeWindow);
		if (provDt < dt && iter->second->getProvider()->canUnload())
		{
			OW_LOG_INFO(env->getLogger(COMPONENT_NAME), Format("Unloading Provider %1", iter->first));
			iter->second = 0;
			m_provs.erase(iter++);
			continue;
		}

		++iter;
	}
}

void BI1ProviderIFC::doShuttingDown(const OpenWBEM::ProviderEnvironmentIFCRef& env)
{
	// Need to lock m_guard while accessing m_provs, but we can't leave the mutex locked
	// while calling shuttingDown(), since that might cause a deadlock, so make a copy.
	OpenWBEM::MutexLock l(m_guard);
	ProviderMap provsCopy(m_provs);
	l.release();

	ProviderEnvironmentIFCRef bi1env = wrapProvEnv(env);
	ProviderMap::iterator it, itend = provsCopy.end();
	for (it = provsCopy.begin(); it != itend; ++it)
	{
		try
		{
			it->second->getProvider()->shuttingDown(bi1env);
		}
		catch (Exception& e)
		{
			OW_LOG_ERROR(env->getLogger(COMPONENT_NAME), Format("Caught exception while calling shuttingDown() on provider: %1", e));
		}
	}

}

} // end namespace OWBI1

