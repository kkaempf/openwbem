/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_NPIPROVIDERIFC_HPP_
#define OW_NPIPROVIDERIFC_HPP_
#include "OW_config.h"
#include "OW_SharedLibrary.hpp"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_Map.hpp"
#include "OW_MutexLock.hpp"
#include "OW_FTABLERef.hpp"

namespace OpenWBEM
{

/**
 * This class implements a bridge from the CIMOM's ProviderManager to the
 * C++ providers.  It's main function is location and creation of providers.
 */
class NPIProviderIFC : public ProviderIFCBaseIFC
{
public:
	static const char * const CREATIONFUNC;
	NPIProviderIFC();
	~NPIProviderIFC();
protected:
	virtual const char* getName() const { return "npi"; }
	virtual void doInit(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& i,
		SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray& a,
#endif
		MethodProviderInfoArray& m,
		IndicationProviderInfoArray& ind);
	virtual InstanceProviderIFCRef doGetInstanceProvider(
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
	virtual void doUnloadProviders(const ProviderEnvironmentIFCRef& env)
	{
		(void)env;
		// TODO
	}
private:
	typedef Map<String, FTABLERef> ProviderMap;
	//typedef Array<SharedLibraryObject<FTABLERef> > LoadedProviderArray;
	typedef Array<FTABLERef > LoadedProviderArray;
	FTABLERef getProvider(const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	void loadNoIdProviders(const ProviderEnvironmentIFCRef& env);
	ProviderMap m_provs;
	Mutex m_guard;
	LoadedProviderArray m_noidProviders;
	bool m_loadDone;
};
typedef SharedLibraryReference< Reference<NPIProviderIFC> > NPIProviderIFCRef;

} // end namespace OpenWBEM

#endif
