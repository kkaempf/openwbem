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

#ifndef OW_PROVIDERIFC_HPP_
#define OW_PROVIDERIFC_HPP_

#include "OW_config.h"
#include "OW_SharedLibraryReference.hpp"
#include "OW_String.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_PropertyProviderIFC.hpp"
#include "OW_InstanceProviderIFC.hpp"
#include "OW_AssociatorProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_Exception.hpp"
#include "OW_InstanceProviderInfo.hpp"

DEFINE_EXCEPTION(NoSuchProvider);

/**
 * This class implements a bridge from the CIMOM's OW_ProviderManager to the
 * providers.  It's main function is location and creation.  This is a base
 * class, and the derived classes each will implement a certain interface to
 * different providers, such as perl, java, python, etc.  The derived classes
 * have to be built into shared libraries that will then be loaded at runtime.
 *
 * Each Derived ProviderIFC must implement the following code:
 *
 * OW_PROVIDERIFCFACTORY(DerivedProviderIFC);
 *
 * Each provider interface must be compiled into it's own shared library.
 *
 * It is recommended that all type and data declarations in the provider be
 * declared inside an anonymous namespace to prevent possible identifier
 * collisions between providers or the openwbem libraries.
 */
class OW_ProviderIFCBaseIFC
{
public:
	OW_ProviderIFCBaseIFC()
	: signature(0xABCDEFA0)
	{
	}

	virtual ~OW_ProviderIFCBaseIFC() {}

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
	const OW_UInt32 signature;

	/**
	 * Called when the provider manager loads the interface
	 */
	void init(const OW_ProviderEnvironmentIFCRef& env,
		OW_InstanceProviderInfoArray& i,
		OW_AssociatorProviderInfoArray& a)
	{
		doInit(env, i, a);
	}
	/**
	 * Locate an Instance provider.
	 *
	 * @param provIdString	The provider interface specific string. The provider
	 *								interface will use this to identify the provider
	 *								being requested.
	 *
	 * @returns A ref counted OW_InstanceProvider. If the provider is not found,
	 * then an OW_NoSuchProviderException is thrown.
	 */
	OW_InstanceProviderIFCRef getInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString)
	{
		// ensure we don't return NULL
		OW_InstanceProviderIFCRef p = doGetInstanceProvider(env, provIdString);
		if (!p)
		{
			OW_THROW(OW_NoSuchProviderException, provIdString);
		}
		return p;
	}

	/**
	 * Locate a Method provider.
	 *
	 * @param provIdString	The provider interface specific string. The provider
	 *								interface will use this to identify the provider
	 *								being requested.
	 *
	 * @returns A ref counted OW_MethodProvider. If the provider is not found,
	 * then null is returned.
	 */
	OW_MethodProviderIFCRef getMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString)
	{
		return doGetMethodProvider(env, provIdString);
	}

	/**
	 * Locate a Property provider.
	 *
	 * @param provIdString	The provider interface specific string. The provider
	 *								interface will use this to identify the provider
	 *								being requested.
	 *
	 * @returns A ref counted OW_PropertyProvider. If the provider is not found,
	 * then null is returned.
	 */
	OW_PropertyProviderIFCRef getPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString)
	{
		return doGetPropertyProvider(env, provIdString);
	}

	/**
	 * Locate an Associator provider.
	 *
	 * @param provIdString	The provider interface specific string. The provider
	 *								interface will use this to identify the provider
	 *								being requested.
	 *
	 * @returns A ref counted OW_AssociatorProvider. If the provider is not
	 * found, then an OW_NoSuchProviderException is thrown.
	 */
	OW_AssociatorProviderIFCRef getAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString)
	{
		// ensure we don't return NULL
		OW_AssociatorProviderIFCRef p = doGetAssociatorProvider(env, provIdString);
		if (!p)
		{
			OW_THROW(OW_NoSuchProviderException, provIdString);
		}
		return p;
	}

	/**
	 * @return all available indication export providers from the available
	 * provider interfaces.
	 */
	OW_IndicationExportProviderIFCRefArray getIndicationExportProviders(
		const OW_ProviderEnvironmentIFCRef& env
		)
	{
		return doGetIndicationExportProviders(env);
	}

	/**
	 * @return all available indication trigger providers from the available
	 * provider interfaces.
	 */
	OW_PolledProviderIFCRefArray getPolledProviders(const OW_ProviderEnvironmentIFCRef& env
		)
	{
		return doGetPolledProviders(env);
	}

	/**
	 * Unload providers in memory that haven't been used for a while
	 */
	void unloadProviders(const OW_ProviderEnvironmentIFCRef& env)
	{
		doUnloadProviders(env);
	}

protected:
	/**
	 * The derived classes must override these functions to implement the
	 * desired functionality.
	 */
	virtual void doInit(const OW_ProviderEnvironmentIFCRef& env,
		OW_InstanceProviderInfoArray& i,
		OW_AssociatorProviderInfoArray& a) = 0;

	virtual OW_InstanceProviderIFCRef doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString) = 0;

	virtual OW_MethodProviderIFCRef doGetMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString) = 0;

	virtual OW_PropertyProviderIFCRef doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString) = 0;

	virtual OW_AssociatorProviderIFCRef doGetAssociatorProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char* provIdString) = 0;

	virtual OW_IndicationExportProviderIFCRefArray doGetIndicationExportProviders(
		const OW_ProviderEnvironmentIFCRef& env
		) = 0;

	virtual OW_PolledProviderIFCRefArray doGetPolledProviders(
		const OW_ProviderEnvironmentIFCRef& env
		) = 0;

	virtual void doUnloadProviders(const OW_ProviderEnvironmentIFCRef& env) = 0;
};

typedef OW_SharedLibraryReference<OW_ProviderIFCBaseIFC> OW_ProviderIFCBaseIFCRef;

#define OW_PROVIDERIFCFACTORY(prov) \
extern "C" OW_ProviderIFCBaseIFC* \
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

