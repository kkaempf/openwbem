/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*******************************************************************************/

#ifndef OW_SIMPLECPPPROVIDERIFC_HPP_
#define OW_SIMPLECPPPROVIDERIFC_HPP_

#include "OW_config.h"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_Map.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_SimpleCppProviderBaseIFC.hpp"
#include "OW_MutexLock.hpp"

/**
 * This class implements a bridge from the CIMOM's OW_ProviderManager to the
 * C++ providers.  It's main function is location and creation of providers.
 */
class OW_SimpleCppProviderIFC : public OW_ProviderIFCBaseIFC
{
public:
	static const char* const CREATIONFUNC;

	OW_SimpleCppProviderIFC();
	~OW_SimpleCppProviderIFC();

protected:

	virtual const char* getName() const { return "simplec++"; }

	/**
	 * The derived classes must override these functions to implement the
	 * desired functionality.
	 */
	virtual void doInit(const OW_ProviderEnvironmentIFCRef& env,
		OW_InstanceProviderInfoArray& i,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		OW_AssociatorProviderInfoArray& a,
#endif
		OW_MethodProviderInfoArray& m,
		OW_IndicationProviderInfoArray& ind);

	virtual OW_InstanceProviderIFCRef doGetInstanceProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);

	virtual OW_MethodProviderIFCRef doGetMethodProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual OW_AssociatorProviderIFCRef doGetAssociatorProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);
#endif

	virtual OW_IndicationProviderIFCRef doGetIndicationProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);

	virtual OW_IndicationExportProviderIFCRefArray doGetIndicationExportProviders(
		const OW_ProviderEnvironmentIFCRef& env
		);

	virtual OW_PolledProviderIFCRefArray doGetPolledProviders(
		const OW_ProviderEnvironmentIFCRef& env
		);

	virtual void doUnloadProviders(const OW_ProviderEnvironmentIFCRef& env);

private:

	typedef OW_Map<OW_String, OW_SimpleCppProviderBaseIFCRef> ProviderMap;

	OW_SimpleCppProviderBaseIFCRef getProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString);

	ProviderMap m_provs;
	OW_Mutex m_guard;
};

typedef OW_SharedLibraryReference<OW_SimpleCppProviderIFC> OW_SimpleCppProviderIFCRef;
#endif

