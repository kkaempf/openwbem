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

#ifndef OW_CPP_PROVIDERBASEIFC_HPP_
#define OW_CPP_PROVIDERBASEIFC_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_SharedLibraryReference.hpp"


class OW_CppInstanceProviderIFC;
class OW_CppMethodProviderIFC;
class OW_CppAssociatorProviderIFC;
class OW_CppPropertyProviderIFC;
class OW_CppIndicationExportProviderIFC;
class OW_CppPolledProviderIFC;

/**
 * This is the base class implemented by all providers that are loaded
 * by the C++ provider interface.
 *
 * It is recommended that all type and data declarations in the provider be
 * declared inside an anonymous namespace to prevent possible identifier
 * collisions between providers or the openwbem libraries.
 */
class OW_CppProviderBaseIFC
{
	public:
		virtual ~OW_CppProviderBaseIFC() {}

		/**
		 * Called by the CIMOM when the provider is initialized
		 * @param hdl The handle to the cimom
		 * @throws OW_CIMException
		 */
		virtual void initialize(const OW_ProviderEnvironmentIFCRef&) {}

		/**
		 * Called by the CIMOM when the provider is removed.
		 * This method will be called when the CIMOM is exiting.  The CIM server
		 * will have already been destroyed, so providers should not try to
		 * perform any CIM operations.
		 * @throws OW_CIMException
		 */
		virtual void cleanup() {}

		/**
		 * We do the following because gcc seems to have a problem with
		 * dynamic_cast.  If often fails, especially when compiling with
		 * optimizations.  It will return a (supposedly) valid pointer, 
		 * when it should return NULL.
		 */

		virtual OW_CppInstanceProviderIFC* getInstanceProvider() { return 0; }
		virtual OW_CppMethodProviderIFC* getMethodProvider() { return 0; }
		virtual OW_CppAssociatorProviderIFC* getAssociatorProvider() { return 0; }
		virtual OW_CppPropertyProviderIFC* getPropertyProvider() { return 0; }
		virtual OW_CppIndicationExportProviderIFC* getIndicationExportProvider() { return 0; }
		virtual OW_CppPolledProviderIFC* getPolledProvider() { return 0; }
};

typedef OW_SharedLibraryReference<OW_CppProviderBaseIFC> OW_CppProviderBaseIFCRef;

#define OW_NOIDPROVIDERFACTORY(prov) OW_PROVIDERFACTORY(prov, NO_ID)

#define OW_PROVIDERFACTORY(prov, name) \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
} \
extern "C" OW_CppProviderBaseIFC* \
createProvider##name() \
{ \
	return new prov; \
}

#endif

