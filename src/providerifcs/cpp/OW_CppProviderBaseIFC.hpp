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
#ifndef OW_CPP_PROVIDERBASEIFC_HPP_
#define OW_CPP_PROVIDERBASEIFC_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_DateTime.hpp"

namespace OpenWBEM
{

class CppInstanceProviderIFC;
class CppSecondaryInstanceProviderIFC;
class CppMethodProviderIFC;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
class CppAssociatorProviderIFC;
#endif
class CppIndicationExportProviderIFC;
class CppPolledProviderIFC;
class CppIndicationProviderIFC;
/**
 * This is the base class implemented by all providers that are loaded
 * by the C++ provider interface.
 *
 * It is recommended that all type and data declarations in the provider be
 * declared inside an anonymous namespace to prevent possible identifier
 * collisions between providers or the openwbem libraries.
 */
class CppProviderBaseIFC
{
public:

	CppProviderBaseIFC() : m_dt(0), m_persist(false) {}

	CppProviderBaseIFC(const CppProviderBaseIFC& arg)
		: m_dt(arg.m_dt), m_persist(arg.m_persist) {}

	virtual ~CppProviderBaseIFC();
	/**
	 * Called by the CIMOM when the provider is initialized
	 * @param hdl The handle to the cimom
	 * @throws CIMException
	 */
	virtual void initialize(const ProviderEnvironmentIFCRef&) {}
	/**
	 * We do the following because gcc seems to have a problem with
	 * dynamic_cast.  If often fails, especially when compiling with
	 * optimizations.  It will return a (supposedly) valid pointer, 
	 * when it should return NULL.
	 */
	virtual CppInstanceProviderIFC* getInstanceProvider() { return 0; }
	virtual CppSecondaryInstanceProviderIFC* getSecondaryInstanceProvider() { return 0; }
	virtual CppMethodProviderIFC* getMethodProvider() { return 0; }
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual CppAssociatorProviderIFC* getAssociatorProvider() { return 0; }
#endif
	virtual CppIndicationExportProviderIFC* getIndicationExportProvider() { return 0; }
	virtual CppPolledProviderIFC* getPolledProvider() { return 0; }
	virtual CppIndicationProviderIFC* getIndicationProvider() { return 0; }
	DateTime getLastAccessTime() const  { return m_dt; }
	void updateAccessTime();
	
	virtual bool canUnload() { return true; }

	bool getPersist() const { return m_persist; }
	void setPersist(bool persist=true) { m_persist = persist; }

protected:
private:
	DateTime m_dt;
	bool m_persist;
};

typedef SharedLibraryReference< Reference<CppProviderBaseIFC> > CppProviderBaseIFCRef;

} // end namespace OpenWBEM


// This is here to prevent existing code from breaking.  New code should use OW_PROVIDERFACTORY_NOID.
#define OW_NOIDPROVIDERFACTORY(prov) OW_PROVIDERFACTORY(prov, NO_ID)

#if !defined(OW_STATIC_SERVICES)
#define OW_PROVIDERFACTORY_NOID(prov, name) OW_PROVIDERFACTORY(prov, NO_ID)
#define OW_PROVIDERFACTORY(prov, name) \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
} \
extern "C" OpenWBEM::CppProviderBaseIFC* \
createProvider##name() \
{ \
	return new prov; \
}
#else
#define OW_PROVIDERFACTORY_NOID(prov, name) OW_PROVIDERFACTORY(prov, NO_ID##name)
#define OW_PROVIDERFACTORY(prov, name) \
extern "C" OpenWBEM::CppProviderBaseIFC* \
createProvider##name() \
{ \
	return new prov; \
}
#endif /* !defined(OW_STATIC_SERVICES) */

#endif
