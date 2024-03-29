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

/**
 * @author Dan Nuffer
 */

#ifndef OW_CPPPROVIDERIFC_HPP_
#define OW_CPPPROVIDERIFC_HPP_
#include "OW_config.h"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "blocxx/Map.hpp"
#include "blocxx/SharedLibrary.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "blocxx/MutexLock.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

/**
 * This class implements a bridge from the CIMOM's ProviderManager to the
 * C++ providers.  It's main function is location and creation of providers.
 */
class OW_CPPPROVIFC_API CppProviderIFC : public ProviderIFCBaseIFC
{
public:
	static const char* const CREATIONFUNC;
	CppProviderIFC();
	~CppProviderIFC();

	// Making this public so other code can re-use it.
	static CppProviderBaseIFCRef loadProvider(const blocxx::String& libName);

protected:
	virtual const char* getName() const { return "c++"; }
	/**
	 * The derived classes must override these functions to implement the
	 * desired functionality.
	 */
	virtual void doInit(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& i,
		SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray& a,
#endif
		MethodProviderInfoArray& m,
		IndicationProviderInfoArray& ind,
		QueryProviderInfoArray& q);
	virtual InstanceProviderIFCRef doGetInstanceProvider(
		const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	virtual SecondaryInstanceProviderIFCRef doGetSecondaryInstanceProvider(
		const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	virtual MethodProviderIFCRef doGetMethodProvider(
		const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual AssociatorProviderIFCRef doGetAssociatorProvider(
		const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
#endif
	virtual IndicationProviderIFCRef doGetIndicationProvider(
		const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	virtual IndicationExportProviderIFCRefArray doGetIndicationExportProviders(
		const ProviderEnvironmentIFCRef& env
		);
	virtual PolledProviderIFCRefArray doGetPolledProviders(
		const ProviderEnvironmentIFCRef& env
		);
	virtual QueryProviderIFCRef doGetQueryProvider(
		const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	virtual void doUnloadProviders(const ProviderEnvironmentIFCRef& env);
	virtual void doShuttingDown(const ProviderEnvironmentIFCRef& env);
private:
	// To prevent a potential deadlock situation, we can't call initialize() while m_guard is locked.
	// However, that presents a race condition for initializing providers, so each CppProviderBaseIFCRef
	// has an associated condition to address that. The details are handled by this class.
	class CppProviderInitializationHelper;
	typedef blocxx::IntrusiveReference<CppProviderInitializationHelper> CppProviderInitializationHelperRef;

	typedef blocxx::Map<blocxx::String, CppProviderInitializationHelperRef> ProviderMap;
	typedef blocxx::Map<blocxx::String, IndicationProviderIFCRef> IndicationProviderMap;
	typedef blocxx::Array<CppProviderBaseIFCRef> LoadedProviderArray;
	enum StoreProviderFlag
	{
		dontStoreProvider,
		storeProvider
	};
	enum InitializeProviderFlag
	{
		dontInitializeProvider,
		initializeProvider
	};
	// REQUIRE: (initP == initializeProvider && storeP == storeProvider) || (initP == dontInitializeProvider && storeP == dontStoreProvider)
	// i.e. if you initialize a provider, you must also store it.  If you don't initialize it, you can't store it.
	CppProviderBaseIFCRef getProvider(const ProviderEnvironmentIFCRef& env,
		const char* provIdString, StoreProviderFlag = storeProvider,
		InitializeProviderFlag = initializeProvider);
	void loadProviders(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& i,
		SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray& a,
#endif
		MethodProviderInfoArray& m,
		IndicationProviderInfoArray& ind,
		QueryProviderInfoArray& q);
	ProviderMap m_provs;
	IndicationProviderMap m_indicationProviders;
	blocxx::Mutex m_guard;
	LoadedProviderArray m_noUnloadProviders;
	bool m_loadDone;
};
typedef blocxx::SharedLibraryReference< blocxx::IntrusiveReference<CppProviderIFC> > CppProviderIFCRef;

} // end namespace OW_NAMESPACE

#endif
