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

#ifndef OWBI1_PROVIDERBASEIFC_HPP_
#define OWBI1_PROVIDERBASEIFC_HPP_
#include "OWBI1_ProviderFwd.hpp"
#include "OWBI1_DateTime.hpp"
#include "OWBI1_IntrusiveCountableBase.hpp"

namespace OWBI1
{

class BI1InstanceProviderIFC;
class BI1SecondaryInstanceProviderIFC;
class BI1MethodProviderIFC;
class BI1AssociatorProviderIFC;
class BI1IndicationExportProviderIFC;
class BI1PolledProviderIFC;
class BI1IndicationProviderIFC;
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
class OWBI1_OWBI1PROVIFC_API BI1ProviderBaseIFC : public virtual IntrusiveCountableBase
{
public:

	BI1ProviderBaseIFC();

	BI1ProviderBaseIFC(const BI1ProviderBaseIFC& arg);

	virtual ~BI1ProviderBaseIFC();
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
	 */
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env);

	/**
	 * We do the following because gcc seems to have a problem with
	 * dynamic_cast.  If often fails, especially when compiling with
	 * optimizations.  It will return a (supposedly) valid pointer,
	 * when it should return NULL.
	 */
	virtual BI1InstanceProviderIFC* getInstanceProvider();
	virtual BI1SecondaryInstanceProviderIFC* getSecondaryInstanceProvider();
	virtual BI1MethodProviderIFC* getMethodProvider();
	virtual BI1AssociatorProviderIFC* getAssociatorProvider();
	virtual BI1IndicationExportProviderIFC* getIndicationExportProvider();
	virtual BI1PolledProviderIFC* getPolledProvider();
	virtual BI1IndicationProviderIFC* getIndicationProvider();
	DateTime getLastAccessTime() const;
	void updateAccessTime();
	
	virtual bool canUnload();

private:
	DateTime m_dt;
};


} // end namespace OWBI1


#define OWBI1_PROVIDERFACTORY(prov, name) \
extern "C" OWBI1_EXPORT OWBI1::BI1ProviderBaseIFC* \
createProvider##name() \
{ \
	return new prov; \
}

#endif
