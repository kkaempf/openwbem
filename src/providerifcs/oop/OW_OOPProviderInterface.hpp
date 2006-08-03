/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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

#ifndef OW_OOP_PROVIDER_INTERFACE_HPP_INCLUDE_GUARD_
#define OW_OOP_PROVIDER_INTERFACE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_String.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_Timeout.hpp"
#include "OW_Mutex.hpp"
#include "OW_Reference.hpp"
#include "OW_OpenWBEM_OOPProviderRegistration.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OOPProviderInterface : public ProviderIFCBaseIFC
{
public:
	OOPProviderInterface();
	virtual ~OOPProviderInterface();

	struct ProvRegInfo : public IntrusiveCountableBase
	{
		ProvRegInfo()
			: timeout(Timeout::infinite)
			, isPersistent(false)
			, userContext(OpenWBEM::OOPProviderRegistration::E_USERCONTEXT_UNPRIVILEGED)
		{
		}
		String process;
		StringArray args;
		String protocol;
		Timeout timeout;
		bool isPersistent;
		UInt16 userContext;
		String monitorPrivilegesFile;
		StringArray indicationExportHandlerClassNames;
	};

	typedef IntrusiveReference<ProvRegInfo> ProvRegInfoRef;

private:
	virtual const char* getName() const;
	virtual void doInit(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& i,
		SecondaryInstanceProviderInfoArray& si,
		AssociatorProviderInfoArray& a,
		MethodProviderInfoArray& m,
		IndicationProviderInfoArray& ind);
	virtual InstanceProviderIFCRef doGetInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
	virtual SecondaryInstanceProviderIFCRef doGetSecondaryInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
	virtual MethodProviderIFCRef doGetMethodProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
	virtual AssociatorProviderIFCRef doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
	virtual IndicationExportProviderIFCRefArray doGetIndicationExportProviders(const ProviderEnvironmentIFCRef& env);
	virtual PolledProviderIFCRefArray doGetPolledProviders(const ProviderEnvironmentIFCRef& env);
	virtual IndicationProviderIFCRef doGetIndicationProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
	virtual void doUnloadProviders(const ProviderEnvironmentIFCRef& env);
	virtual void doShuttingDown(const ProviderEnvironmentIFCRef& env);

	// These variables are only modified in doInit() and are accessed read only by 
	// other functions, thus they aren't mutex protected.
	// key is the provider id. the value may be shared between multiple maps.
	typedef SortedVectorMap<String, ProvRegInfoRef> ProvRegMap_t;
	ProvRegMap_t m_instanceProvReg;
	ProvRegMap_t m_secondaryInstanceProvReg;
	ProvRegMap_t m_associatorProvReg;
	ProvRegMap_t m_methodProvReg;
	ProvRegMap_t m_indicationProvReg;
	ProvRegMap_t m_polledProvReg;
	ProvRegMap_t m_indicationExportProvReg;

	struct SavedProviders
	{
		SavedProviders()
			: guard(new Mutex)
			, process(new ProcessRef)
		{
		}

		SavedProviders(const IndicationProviderIFCRef& ipir)
			: indProv(ipir)
			, guard(new Mutex)
			, process(new ProcessRef)
		{
		}

		SavedProviders(const PolledProviderIFCRef& ppir)
			: polledProv(ppir)
			, guard(new Mutex)
			, process(new ProcessRef)
		{
		}

		SavedProviders(const IndicationExportProviderIFCRef& iepir)
			: indicationExportProv(iepir)
			, guard(new Mutex)
			, process(new ProcessRef)
		{
		}

		IndicationProviderIFCRef indProv;
		PolledProviderIFCRef polledProv;
		IndicationExportProviderIFCRef indicationExportProv;
		Reference<Mutex> guard;
		Reference<ProcessRef> process;

	};

	// key is the provider id
	typedef SortedVectorMap<String, SavedProviders> PersistentProvMap_t;
	PersistentProvMap_t m_persistentProvs;
	Mutex m_persistentProvsGuard;
};

}



#endif


