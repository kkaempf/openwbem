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
#ifndef OW_PROVIDERIFC_HPP_
#define OW_PROVIDERIFC_HPP_
#include "OW_config.h"
#include "OW_SharedLibraryReference.hpp"
#include "OW_String.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_InstanceProviderIFC.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_AssociatorProviderIFC.hpp"
#endif
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_InstanceProviderInfo.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_AssociatorProviderInfo.hpp"
#endif
#include "OW_MethodProviderInfo.hpp"
#include "OW_IndicationProviderInfo.hpp"

namespace OpenWBEM
{

/**
 * This class implements a bridge from the CIMOM's ProviderManager to the
 * providers.  It's main function is location and creation.  This is a base
 * class, and the derived classes each will implement a certain interface to
 * different providers, such as perl, java, python, etc.  The derived classes
 * have to be built into shared libraries that will then be loaded at runtime.
 *
 * Each Derived ProviderIFC must implement the following code:
 *
 * PROVIDERIFCFACTORY(DerivedProviderIFC);
 *
 * Each provider interface must be compiled into it's own shared library.
 *
 * It is recommended that all type and data declarations in the provider be
 * declared inside an anonymous namespace to prevent possible identifier
 * collisions between providers or the openwbem libraries.
 */
class ProviderIFCBaseIFC
{
public:
	ProviderIFCBaseIFC();
	virtual ~ProviderIFCBaseIFC();
	/**
	 * Return the provider's name. The name will be used to identify this
	 * provider interface from other provider interfaces.
	 *
	 * @returns The name of the provider interface.
	 */
	virtual const char* getName() const = 0;
	/**
	 * This public data member is to allow openwbem to easily check to make
	 * sure that the provider interface is valid.  Since it will be compiled into
	 * a shared library, openwbem cannot trust the code it loads.
	 */
	const UInt32 signature;
	/**
	 * Called when the provider manager loads the interface
	 */
	void init(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& i,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray& a,
#endif
		MethodProviderInfoArray& m,
		IndicationProviderInfoArray& ind);
	/**
	 * Locate an Instance provider.
	 *
	 * @param provIdString	The provider interface specific string. The provider
	 *								interface will use this to identify the provider
	 *								being requested.
	 *
	 * @returns A ref counted InstanceProvider. If the provider is not found,
	 * then an NoSuchProviderException is thrown.
	 */
	InstanceProviderIFCRef getInstanceProvider(const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
	/**
	 * Locate a Method provider.
	 *
	 * @param provIdString	The provider interface specific string. The provider
	 *								interface will use this to identify the provider
	 *								being requested.
	 *
	 * @returns A ref counted MethodProvider. If the provider is not found,
	 * then an NoSuchProviderException is thrown.
	 */
	MethodProviderIFCRef getMethodProvider(const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * Locate an Associator provider.
	 *
	 * @param provIdString	The provider interface specific string. The provider
	 *								interface will use this to identify the provider
	 *								being requested.
	 *
	 * @returns A ref counted AssociatorProvider. If the provider is not
	 * found, then an NoSuchProviderException is thrown.
	 */
	AssociatorProviderIFCRef getAssociatorProvider(const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
#endif
	/**
	 * @return all available indication export providers from the available
	 * provider interfaces.
	 */
	IndicationExportProviderIFCRefArray getIndicationExportProviders(
		const ProviderEnvironmentIFCRef& env);
	/**
	 * @return all available indication trigger providers from the available
	 * provider interfaces.
	 */
	PolledProviderIFCRefArray getPolledProviders(const ProviderEnvironmentIFCRef& env);
	/**
	 * Unload providers in memory that haven't been used for a while
	 */
	void unloadProviders(const ProviderEnvironmentIFCRef& env);
	/**
	 * Locate an Indication provider.
	 *
	 * @param provIdString	The provider interface specific string. The provider
	 *								interface will use this to identify the provider
	 *								being requested.
	 *
	 * @returns A ref counted IndicationProvider. If the provider is not
	 * found, then an NoSuchProviderException is thrown.
	 */
	IndicationProviderIFCRef getIndicationProvider(const ProviderEnvironmentIFCRef& env,
		const char* provIdString);
protected:
	/**
	 * The derived classes must override these functions to implement the
	 * desired functionality.
	 */
	virtual void doInit(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& i,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray& a,
#endif
		MethodProviderInfoArray& m,
		IndicationProviderInfoArray& ind) = 0;
	virtual InstanceProviderIFCRef doGetInstanceProvider(const ProviderEnvironmentIFCRef& env,
		const char* provIdString) = 0;
	virtual MethodProviderIFCRef doGetMethodProvider(const ProviderEnvironmentIFCRef& env,
		const char* provIdString) = 0;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual AssociatorProviderIFCRef doGetAssociatorProvider(
		const ProviderEnvironmentIFCRef& env,
		const char* provIdString) = 0;
#endif
	virtual IndicationExportProviderIFCRefArray doGetIndicationExportProviders(
		const ProviderEnvironmentIFCRef& env
		) = 0;
	virtual PolledProviderIFCRefArray doGetPolledProviders(
		const ProviderEnvironmentIFCRef& env
		) = 0;
	virtual IndicationProviderIFCRef doGetIndicationProvider(
		const ProviderEnvironmentIFCRef& env,
		const char* provIdString) = 0;
	virtual void doUnloadProviders(const ProviderEnvironmentIFCRef& env) = 0;
};

typedef SharedLibraryReference<ProviderIFCBaseIFC> ProviderIFCBaseIFCRef;

} // end namespace OpenWBEM

#define OW_PROVIDERIFCFACTORY(prov) \
extern "C" OpenWBEM::ProviderIFCBaseIFC* \
createProviderIFC() \
{ \
	return new prov; \
} \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
}

#endif
