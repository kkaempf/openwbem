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

#ifndef OW_CPPPROVIDERIFC_HPP_
#define OW_CPPPROVIDERIFC_HPP_

#include "OW_config.h"
#include "OW_ProviderIFCBaseIFCBaseIFC.hpp"
#include "OW_Map.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_MutexLock.hpp"

/**
 * This class implements a bridge from the CIMOM's OW_ProviderManager to the
 * C++ providers.  It's main function is location and creation of providers.
 */
class OW_CppProviderIFC : public OW_ProviderIFCBaseIFC
{
public:
	static const char* const CREATIONFUNC;

	OW_CppProviderIFC();
	~OW_CppProviderIFC();

protected:

	virtual const char* getName() const { return "c++"; }

	/**
	 * The derived classes must override these functions to implement the
	 * desired functionality.
	 */
	virtual OW_InstanceProviderIFCRef doGetInstanceProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);

	virtual OW_MethodProviderIFCRef doGetMethodProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);

	virtual OW_PropertyProviderIFCRef doGetPropertyProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);

	virtual OW_AssociatorProviderIFCRef doGetAssociatorProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);

	virtual OW_IndicationExportProviderIFCRefArray doGetIndicationExportProviders(
		const OW_ProviderEnvironmentIFCRef& env
		);

	virtual OW_PolledProviderIFCRefArray doGetPolledProviders(
		const OW_ProviderEnvironmentIFCRef& env
		);

private:

	struct LoadedProvider
	{
		LoadedProvider() : m_lib(0), m_pProv(NULL) {}
		OW_SharedLibraryRef m_lib;
		OW_CppProviderBaseIFCRef m_pProv;
	};
	typedef OW_Map<OW_String, LoadedProvider> ProviderMap;
	typedef OW_Array<LoadedProvider> LoadedProviderArray;

	OW_CppProviderBaseIFCRef getProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	void loadNoIdProviders(const OW_ProviderEnvironmentIFCRef& env);


	ProviderMap m_provs;
	OW_Mutex m_guard;
	LoadedProviderArray m_noidProviders;
	OW_Bool m_loadDone;
};

typedef OW_Reference<OW_CppProviderIFC> OW_CppProviderIFCRef;
#endif

