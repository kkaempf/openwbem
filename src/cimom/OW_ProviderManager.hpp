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

#ifndef OW_PROVIDERMANAGER_HPP_
#define OW_PROVIDERMANAGER_HPP_

#include "OW_config.h"
#include "OW_InstanceProviderIFC.hpp"
#include "OW_MethodProviderIFC.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_AssociatorProviderIFC.hpp"
#endif
#include "OW_PolledProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_Mutex.hpp"
#include "OW_HashMap.hpp"
#include "OW_HashMultiMap.hpp"


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
		const OW_String& ns, const OW_CIMClass& cc, const OW_CIMMethod& method) const;

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
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
#endif

	/**
	 * @return all available indication export providers from the available
	 * provider interfaces.
	 */
	OW_IndicationExportProviderIFCRefArray
		getIndicationExportProviders(const OW_ProviderEnvironmentIFCRef& env) const;

	/**
	 * @return all available polled providers from the available
	 * provider interfaces.
	 */
	OW_PolledProviderIFCRefArray
		getPolledProviders(const OW_ProviderEnvironmentIFCRef& env) const;

	/**
	 * @return all available indication providers from the available
	 * provider interfaces, which are interested in exporting indications of
	 * indicationClassName in namespace ns.
	 */
	OW_IndicationProviderIFCRefArray
		getIndicationProviders(const OW_ProviderEnvironmentIFCRef& env, 
			const OW_String& ns, const OW_String& indicationClassName,
			const OW_String& monitoredClassName) const;

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

	OW_Mutex m_guard;

public:	// so free functions in cpp file can access them.
	struct ProvReg
	{
		OW_String provName;
		OW_ProviderIFCBaseIFCRef ifc;
	};

	typedef OW_HashMap<OW_String, ProvReg> ProvRegMap_t;
	typedef OW_HashMultiMap<OW_String, ProvReg> IndProvRegMap_t;
	
private:
	// The key must be: a classname if the provider supports any namespace,
	// or namespace:classname for a specific namespace.
	ProvRegMap_t m_registeredInstProvs;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	ProvRegMap_t m_registeredAssocProvs;
#endif

	// The key must be: [namespace:]className[:methodname]
	ProvRegMap_t m_registeredMethProvs;

	// The key must be: [namespace:]className[:propertyname]
	ProvRegMap_t m_registeredPropProvs;

	// The key must be: a classname if the provider supports any namespace,
	// or namespace:classname for a specific namespace.
	IndProvRegMap_t m_registeredIndProvs;
};

typedef OW_Reference<OW_ProviderManager> OW_ProviderManagerRef;

#endif


