/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_INTERNALPROVIDERIFC_HPP_
#define OW_INTERNALPROVIDERIFC_HPP_

#include "OW_config.h"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_Reference.hpp"
#include "OW_String.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_Mutex.hpp"

class OW_InternalProviderIFC : public OW_ProviderIFCBaseIFC
{
public:

	~OW_InternalProviderIFC();

	/**
	 * Make a cimom provider available to the provider manager.
	 * A cimom provider is simply a provider that is provided by the cimom
	 * itself (i.e. not loaded from a shared library).
	 * @param providerName	The name this provider will be identified as.
	 * @param pProv	A pointer to a dynamically allocated provider. This
	 * provider will be deleted by the provider manager when it is no longer
	 * needed.
	 */
	void addCIMOMProvider(const OW_String& providerName, const OW_CppProviderBaseIFCRef& pProv);

	void addCIMOMProvider(const OW_CppProviderBaseIFCRef& pProv);

private:
	virtual const char* getName() const { return "owcimomd"; }

	virtual void doInit(const OW_ProviderEnvironmentIFCRef& env,
		OW_InstanceProviderInfoArray& i,
		OW_AssociatorProviderInfoArray& a,
		OW_MethodProviderInfoArray& m);

	virtual OW_InstanceProviderIFCRef doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char *provIdString);

	virtual OW_MethodProviderIFCRef doGetMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char *provIdString);

	virtual OW_PropertyProviderIFCRef doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char *provIdString);

	virtual OW_AssociatorProviderIFCRef doGetAssociatorProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char *provIdString);

	virtual OW_IndicationExportProviderIFCRefArray
		doGetIndicationExportProviders(
		const OW_ProviderEnvironmentIFCRef& env
		);

	virtual OW_PolledProviderIFCRefArray
		doGetPolledProviders(
		const OW_ProviderEnvironmentIFCRef& env
		);

	void doUnloadProviders(const OW_ProviderEnvironmentIFCRef& env)
	{
		(void)env;
		// empty
	}

	struct CimProv
	{
		CimProv() : m_initDone(false), m_pProv() {}
		OW_Bool m_initDone;
		OW_CppProviderBaseIFCRef m_pProv;
	};

	typedef OW_SortedVectorMap<OW_String, CimProv> ProviderMap;
	ProviderMap m_cimomProviders;
	OW_Array<CimProv> m_noIdProviders;
	OW_Mutex m_guard;
};

typedef OW_Reference<OW_InternalProviderIFC> OW_InternalProviderIFCRef;

#endif


