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
 * @author Dan Nuffer
 */

#ifndef OWBI1_PROVIDERIFC_HPP_INCLUDE_GUARD_
#define OWBI1_PROVIDERIFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OWBI1_config.h"
#include "OWBI1_Fwd.hpp"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_Map.hpp"
#include "OW_SharedLibrary.hpp"
#include "OWBI1_ProviderBaseIFC.hpp"
#include "OW_MutexLock.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OWBI1_IntrusiveReference.hpp"
#include "OWBI1_Array.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OWBI1
{

using namespace OpenWBEM;

typedef SharedLibraryReference< IntrusiveReference<BI1ProviderBaseIFC> > BI1ProviderBaseIFCRef;

/**
 * This class implements a bridge from the CIMOM's ProviderManager to the
 * C++ providers.  It's main function is location and creation of providers.
 */
class OWBI1_OWBI1PROVIFC_API BI1ProviderIFC : public ProviderIFCBaseIFC
{
public:
	static const char* const CREATIONFUNC;
	BI1ProviderIFC();
	~BI1ProviderIFC();

	// Making this public so other code can re-use it.
	static BI1ProviderBaseIFCRef loadProvider(const OpenWBEM::String& libName, OpenWBEM::LoggerRef logger);

protected:
	virtual const char* getName() const { return "owbi1"; }
	/**
	 * The derived classes must override these functions to implement the
	 * desired functionality.
	 */
	virtual void doInit(const OpenWBEM::ProviderEnvironmentIFCRef& env,
		OpenWBEM::InstanceProviderInfoArray& i,
		OpenWBEM::SecondaryInstanceProviderInfoArray& si,
#ifndef OWBI1_DISABLE_ASSOCIATION_TRAVERSAL
		OpenWBEM::AssociatorProviderInfoArray& a,
#endif
		OpenWBEM::MethodProviderInfoArray& m,
		OpenWBEM::IndicationProviderInfoArray& ind);
	virtual OpenWBEM::InstanceProviderIFCRef doGetInstanceProvider(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	virtual OpenWBEM::SecondaryInstanceProviderIFCRef doGetSecondaryInstanceProvider(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	virtual OpenWBEM::MethodProviderIFCRef doGetMethodProvider(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const char* provIdString);
#ifndef OWBI1_DISABLE_ASSOCIATION_TRAVERSAL
	virtual OpenWBEM::AssociatorProviderIFCRef doGetAssociatorProvider(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const char* provIdString);
#endif
	virtual OpenWBEM::IndicationProviderIFCRef doGetIndicationProvider(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	virtual OpenWBEM::IndicationExportProviderIFCRefArray doGetIndicationExportProviders(
		const OpenWBEM::ProviderEnvironmentIFCRef& env
		);
	virtual OpenWBEM::PolledProviderIFCRefArray doGetPolledProviders(
		const OpenWBEM::ProviderEnvironmentIFCRef& env
		);
	virtual void doUnloadProviders(const OpenWBEM::ProviderEnvironmentIFCRef& env);
	virtual void doShuttingDown(const OpenWBEM::ProviderEnvironmentIFCRef& env);
private:
	// To prevent a potential deadlock situation, we can't call initialize() while m_guard is locked.
	// However, that presents a race condition for initializing providers, so each BI1ProviderBaseIFCRef
	// has an associated condition to address that. The details are handled by this class.
	class BI1ProviderInitializationHelper;
	typedef OpenWBEM::IntrusiveReference<BI1ProviderInitializationHelper> BI1ProviderInitializationHelperRef;

	typedef Map<OpenWBEM::String, BI1ProviderInitializationHelperRef> ProviderMap;
	typedef Map<OpenWBEM::String, OpenWBEM::IndicationProviderIFCRef> IndicationProviderMap;
	
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
	BI1ProviderBaseIFCRef getProvider(const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const char* provIdString, StoreProviderFlag = storeProvider,
		InitializeProviderFlag = initializeProvider);
	
	void loadProviders(const OpenWBEM::ProviderEnvironmentIFCRef& env,
		OpenWBEM::InstanceProviderInfoArray& i,
		OpenWBEM::SecondaryInstanceProviderInfoArray& si,
		OpenWBEM::AssociatorProviderInfoArray& a,
		OpenWBEM::MethodProviderInfoArray& m,
		OpenWBEM::IndicationProviderInfoArray& ind);
	
	ProviderMap m_provs;
	IndicationProviderMap m_indicationProviders;
	OpenWBEM::Mutex m_guard;
	OpenWBEM::IndicationExportProviderIFCRefArray m_indicationExportProviders;
	OpenWBEM::PolledProviderIFCRefArray m_polledProviders;
	bool m_loadDone;
};
//typedef SharedLibraryReference< IntrusiveReference<BI1ProviderIFC> > CppProviderIFCRef;

} // end namespace OWBI1

#endif
