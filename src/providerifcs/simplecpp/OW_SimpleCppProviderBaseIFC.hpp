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

#ifndef OW_SIMPLECPP_PROVIDERBASEIFC_HPP_
#define OW_SIMPLECPP_PROVIDERBASEIFC_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_DateTime.hpp"


class OW_SimpleCppInstanceProviderIFC;
class OW_SimpleCppMethodProviderIFC;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
class OW_SimpleCppAssociatorProviderIFC;
#endif
class OW_SimpleCppIndicationExportProviderIFC;
class OW_SimpleCppPolledProviderIFC;

/**
 * This is the base class implemented by all providers that are loaded
 * by the simple C++ provider interface.
 *
 * It is recommended that all type and data declarations in the provider be
 * declared inside an anonymous namespace to prevent possible identifier
 * collisions between providers or the openwbem libraries.
 */
class OW_SimpleCppProviderBaseIFC
{
public:
	virtual ~OW_SimpleCppProviderBaseIFC();

	/**
	 * Called by the CIMOM when the provider is initialized
	 * @param hdl The handle to the cimom
	 * @throws OW_CIMException
	 */
	virtual void initialize(const OW_ProviderEnvironmentIFCRef&);

	/**
	 * We do the following because gcc seems to have a problem with
	 * dynamic_cast.  If often fails, especially when compiling with
	 * optimizations.  It will return a (supposedly) valid pointer,
	 * when it should return NULL.
	 */

	virtual OW_SimpleCppInstanceProviderIFC* getInstanceProvider();
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual OW_SimpleCppAssociatorProviderIFC* getAssociatorProvider();
#endif

	OW_DateTime getLastAccessTime() const;

	void updateAccessTime();
protected:

private:
	OW_DateTime m_dt;

};

typedef OW_SharedLibraryReference<OW_SimpleCppProviderBaseIFC> OW_SimpleCppProviderBaseIFCRef;

#define OW_PROVIDERFACTORY(prov, name) \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
} \
extern "C" OW_SimpleCppProviderBaseIFC* \
createProvider##name() \
{ \
	return new prov; \
}

#endif

