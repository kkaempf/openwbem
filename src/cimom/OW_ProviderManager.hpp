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

#ifndef OW_PROVIDERMANAGER_HPP_
#define OW_PROVIDERMANAGER_HPP_

#include "OW_config.h"
#include "OW_InstanceProviderIFC.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_PropertyProviderIFC.hpp"
#include "OW_AssociatorProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppPropertyProviderIFC.hpp"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_Mutex.hpp"
#include "OW_InternalProviderIFC.hpp"
#include "OW_Map.hpp"

/**
 * This class will be used by the CIMOM as a way of finding providers.
 * It will keep the list of provider interfaces and query them when searching
 * for providers.
 */
class OW_ProviderManager
{
public:

	/**
	 * Load and instantiate the OW_ProviderIFCBaseIFC classes using the
	 * ifcLoader to do the work.
	 *
	 * @param ifcLoader the class that will actually load and instantiate the
	 * 	OW_ProviderIFCBaseIFC classes.
	 */
	void load(const OW_ProviderIFCLoaderRef& ifcLoader);

	/**
	 * Make a cimom provider available to the provider manager.
	 * A cimom provider is simply a provider that is provided by the cimom
	 * itself (i.e. not loaded from a shared library).
	 * @param providerName	The name this provider will be identified as.
	 * @param pProv	A pointer to a dynamically allocated provider. This
	 * provider will be deleted by the provider manager when it is no longer
	 * needed.
	 */
	void addCIMOMProvider(const OW_String& providerName, const OW_CppProviderBaseIFCRef& pProv);

	void addCIMOMProvider(const OW_CppProviderBaseIFCRef& pProv);

	/**
	 * Initialize the provider interfaces and providers.  This is called after
	 * the CIM Server is up and running, so the providers can access the
	 * repository.  None of the services have been started yet however.
	 */
	void init(const OW_ProviderEnvironmentIFCRef& env);

	/**
	 * Locate an Instance provider.
	 *
	 * @param ns The namespace of the class.
	 * @param cc The class may have a qualifier describing the instance provider. This qualifier
	 * must be a "provider" qualifer with a OW_CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.  This function will also return any providers registered for
	 * the class (without the provider qualifier).
	 *
	 * @returns A ref counted OW_InstanceProvider. If no provider is found then
	 * 	null is returned.
	 */
	OW_InstanceProviderIFCRef getInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns, const OW_CIMClass& cc) const;

	/**
	 * Locate a Method provider.
	 *
	 * @param qual A qualifier describing the method provider. This qualifier
	 * must be a "provider" qualifer with a OW_CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.
	 *
	 * @returns A ref counted OW_MethodProvider. If no provider is found then
	 *	null is returned.
	 */
	OW_MethodProviderIFCRef getMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMQualifier& qual) const;

	/**
	 * Locate a Property provider.
	 *
	 * @param qual A qualifier describing the property provider. This qualifier
	 * must be a "provider" qualifer with a OW_CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.
	 *
	 * @returns A ref counted OW_PropertyProvider. If no provider is found then
	 *	null is returned.
	 */
	OW_PropertyProviderIFCRef getPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMQualifier& qual) const;

	/**
	 * Locate an Associator provider.
	 *
	 * @param ns The namespace of the class.
	 * @param cc The class may have a qualifier describing the instance provider. This qualifier
	 * must be a "provider" qualifer with a OW_CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.  This function will also return any providers registered for
	 * the class (without the provider qualifier).
	 *
	 * @returns A ref counted OW_AssociatorProvider. If no provider is found then
	 * 	null is returned.
	 */
	OW_AssociatorProviderIFCRef getAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns, const OW_CIMClass& cc) const;

	/**
	 * @return all available indication export providers from the available
	 * provider interfaces.
	 */
	OW_IndicationExportProviderIFCRefArray
		getIndicationExportProviders(const OW_ProviderEnvironmentIFCRef& env);

	/**
	 * @return all available indication trigger providers from the available
	 * provider interfaces.
	 */
	OW_PolledProviderIFCRefArray
		getPolledProviders(const OW_ProviderEnvironmentIFCRef& env);

	/**
	 * Call into each ProviderIFC to unload providers which haven't been
	 * used for a while
	 */
	void unloadProviders(const OW_ProviderEnvironmentIFCRef& env);

private:


	OW_ProviderIFCBaseIFCRef getProviderIFC(const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMQualifier& qual,
		OW_String& provStr) const;

	OW_Array<OW_ProviderIFCBaseIFCRef> m_IFCArray;

	// also stored in m_IFCArray.  We keep this here so we can call
	// addCIMOMProvider()
	OW_InternalProviderIFCRef m_internalIFC;

	OW_Mutex m_guard;

	struct InstProvReg
	{
		OW_String provName;
		OW_ProviderIFCBaseIFCRef ifc;
	};

	typedef InstProvReg AssocProvReg; // same for now

	// The key must be: a classname if the provider supports any namespace,
	// or namespace:classname for a specific namespace.
	typedef OW_Map<OW_String, InstProvReg> InstProvRegMap_t;
	InstProvRegMap_t m_registeredInstProvs;

	// The key must be: a classname if the provider supports any namespace,
	// or namespace:classname for a specific namespace.
	typedef OW_Map<OW_String, AssocProvReg> AssocProvRegMap_t;
	AssocProvRegMap_t m_registeredAssocProvs;

};

typedef OW_Reference<OW_ProviderManager> OW_ProviderManagerRef;

#endif


