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
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Map.hpp"

/**
 * This class will be used by the CIMOM as a way of finding providers.
 * It will keep the list of provider interfaces and query them when searching
 * for providers.
 */
class OW_ProviderManager : public OW_ProviderIFCBaseIFC
{
public:

	static const char* const CIMOM_PROVIDER_IFC;

	/**
	 * Constructor
	 */
	OW_ProviderManager();

	/**
	 * Destructor
	 */
	~OW_ProviderManager();

	/**
	 * Load and instantiate the OW_ProviderIFCBaseIFC classes using the
	 * ifcLoader to do the work.
	 *
	 * @param ifcLoader the class that will actually load and instantiate the
	 * 	OW_ProviderIFCBaseIFC classes.
	 */
	void init(const OW_ProviderIFCBaseIFCLoaderRef ifcLoader);

	/**
	 * Make a cimom provider available to the provider manager.
	 * A cimom provider is simply a provider that is provided by the cimom
	 * itself (i.e. not loaded from a shared library).
	 * @param providerName	The name this provider will be identified as.
	 * @param pProv	A pointer to a dynamically allocated provider. This
	 * provider will be deleted by the provider manager when it is no longer
	 * needed.
	 */
	void addCIMOMProvider(const OW_String& providerName, OW_CppProviderBaseIFCRef pProv);

	void addCIMOMProvider(OW_CppProviderBaseIFCRef pProv);

	/**
	 * Locate an Instance provider.
	 *
	 * @param qual A qualifier describing the instance provider. This qualifier
	 * must be a "provider" qualifer with a OW_CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.
	 *
	 * @returns A ref counted OW_InstanceProvider. If no provider is found then
	 * 	null is returned.
	 */
	OW_InstanceProviderIFCRef getInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMQualifier& qual) const;

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
	 * @param qual A qualifier describing the associator provider. This qualifier
	 * must be a "provider" qualifer with a OW_CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.
	 *
	 * @returns A ref counted OW_AssociatorProvider. If no provider is found then
	 * 	null is returned.
	 */
	OW_AssociatorProviderIFCRef getAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMQualifier& qual) const;

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

protected:

	virtual const char* getName() const { return CIMOM_PROVIDER_IFC; }

	virtual OW_InstanceProviderIFCRef doGetInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char *provIdString);

	virtual OW_MethodProviderIFCRef doGetMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char *provIdString);

	virtual OW_PropertyProviderIFCRef doGetPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
		const char *provIdString);

	virtual OW_AssociatorProviderIFCRef doGetAssociatorProvider(
		const OW_ProviderEnvironmentIFCRef& env,
		const char *provIdString);

	virtual OW_IndicationExportProviderIFCRefArray
		doGetIndicationExportProviders(
		const OW_ProviderEnvironmentIFCRef& env
		);

	virtual OW_PolledProviderIFCRefArray
		doGetPolledProviders(
		const OW_ProviderEnvironmentIFCRef& env
		);

private:

	struct CimProv
	{
		CimProv() : m_initDone(false), m_pProv(NULL) {}
		OW_Bool m_initDone;
		OW_CppProviderBaseIFCRef m_pProv;
	};

	typedef OW_Map<OW_String, CimProv> ProviderMap;

	OW_ProviderIFCBaseIFCRef getProviderIFC(const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMQualifier& qual,
		OW_String& provStr) const;

	OW_Array<OW_ProviderIFCBaseIFCRef> m_IFCArray;
	OW_Array<OW_SharedLibraryRef> m_shlibArray;
	ProviderMap m_cimomProviders;
	OW_Mutex m_guard;
	OW_Array<CimProv> m_noIdProviders;
};

typedef OW_Reference<OW_ProviderManager> OW_ProviderManagerRef;

#endif


