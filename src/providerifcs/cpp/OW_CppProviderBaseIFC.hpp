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

#ifndef OW_CPP_PROVIDERBASEIFC_HPP_
#define OW_CPP_PROVIDERBASEIFC_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ProviderRegistrationEnvironmentIFC.hpp"
#include "blocxx/SharedLibraryReference.hpp"
#include "blocxx/DateTime.hpp"
#include "blocxx/IntrusiveReference.hpp"
#include "blocxx/IntrusiveCountableBase.hpp"

namespace OW_NAMESPACE
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
class CppQueryProviderIFC;
/**
 * This is the base class implemented by all providers that are loaded
 * by the C++ provider interface.
 *
 * It is recommended that all type and data declarations in the provider be
 * declared inside an anonymous namespace to prevent possible identifier
 * collisions between providers or the openwbem libraries.
 *
 * If your provider spawns a separate thread that needs access to a
 * provider environment (for example, to get a CIMOM handle or logger),
 * see the comment for the initialize() function.
 *
 * DO NOT put inline functions in this class, they will be duplicated in
 * every provider and cause code bloat.
 */
class OW_CPPPROVIFC_API CppProviderBaseIFC : public virtual blocxx::IntrusiveCountableBase
{
public:

	CppProviderBaseIFC();

	CppProviderBaseIFC(const CppProviderBaseIFC& arg);

	virtual ~CppProviderBaseIFC();
	/**
	 * Called by the CIMOM when the provider is initialized
	 * @param env Gives the provider access to things such as a
	 *            CIMOM handle, logger, etc.  This provider environment
	 *            carries its own OperationContext and hence things like
	 *            the CIMOM handle remain valid for the lifetime of *env.
	 *            Thus the provider can store a copy of env for
	 *            later use, e.g., by a separate thread spawned by
	 *            the provider.
	 * @throws CIMException
	 */
	virtual void initialize(const ProviderEnvironmentIFCRef& env);

	/**
	 * Called by the CIMOM just before it starts shutting down services.
	 * Providers should override this to do any portion of their
	 * shutdown process that requires access to the CIMOM.
	 * A provider must still be functional after this call is done, as
	 * other providers or services may still access the provider.
	 * For instance, indication providers will get deActivateFilter()
	 * calls after shuttingDown() is called.
	 */
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env);

	/**
	 * We do the following because gcc seems to have a problem with
	 * dynamic_cast.  If often fails, especially when compiling with
	 * optimizations.  It will return a (supposedly) valid pointer,
	 * when it should return NULL.
	 */
	virtual CppInstanceProviderIFC* getInstanceProvider();
	virtual CppSecondaryInstanceProviderIFC* getSecondaryInstanceProvider();
	virtual CppMethodProviderIFC* getMethodProvider();
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual CppAssociatorProviderIFC* getAssociatorProvider();
#endif
	virtual CppIndicationExportProviderIFC* getIndicationExportProvider();
	virtual CppPolledProviderIFC* getPolledProvider();
	virtual CppIndicationProviderIFC* getIndicationProvider();
	virtual CppQueryProviderIFC* getQueryProvider();
	blocxx::DateTime getLastAccessTime() const;
	void updateAccessTime();

	virtual bool canUnload();

	bool getPersist() const;
	void setPersist(bool persist=true);

private:
	blocxx::DateTime m_dt;
	bool m_persist;
};

typedef blocxx::SharedLibraryReference< blocxx::IntrusiveReference<CppProviderBaseIFC> > CppProviderBaseIFCRef;

} // end namespace OW_NAMESPACE


#if !defined(OW_STATIC_SERVICES)
#define OW_PROVIDERFACTORY(prov, name) \
extern "C" OW_EXPORT const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
} \
extern "C" OW_EXPORT OW_NAMESPACE::CppProviderBaseIFC* \
createProvider##name() \
{ \
	return new prov; \
}
#else
#define OW_PROVIDERFACTORY(prov, name) \
extern "C" OW_NAMESPACE::CppProviderBaseIFC* \
createProvider##name() \
{ \
	return new prov; \
}
#endif /* !defined(OW_STATIC_SERVICES) */

#endif
