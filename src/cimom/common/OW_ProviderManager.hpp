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
#include "OW_SecondaryInstanceProviderIFC.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_Mutex.hpp"
#include "OW_HashMap.hpp"
#include "OW_HashMultiMap.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"

namespace OpenWBEM
{

/**
 * This class will be used by the CIMOM as a way of finding providers.
 * It will keep the list of provider interfaces and query them when searching
 * for providers.
 */
class ProviderManager : public IntrusiveCountableBase
{
public:
	/**
	 * Load and instantiate the ProviderIFCBaseIFC classes using the
	 * ifcLoader to do the work.
	 *
	 * @param ifcLoader the class that will actually load and instantiate the
	 * 	ProviderIFCBaseIFC classes.
	 */
	void load(const ProviderIFCLoaderRef& ifcLoader);
	/**
	 * Initialize the provider interfaces and providers.  This is called after
	 * the CIM Server is up and running, so the providers can access the
	 * repository.  None of the services have been started yet however.
	 */
	void init(const ProviderEnvironmentIFCRef& env);
	/**
	 * Called when the cimom is shutting down.  All the provider IFCs will be
	 * unloaded.
	 */
	void shutdown();
	/**
	 * Locate an Instance provider.
	 *
	 * @param ns The namespace of the class.
	 * @param cc The class may have a qualifier describing the instance provider. This qualifier
	 * must be a "provider" qualifer with a CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.  This function will also return any providers registered for
	 * the class (without the provider qualifier).
	 *
	 * @return A ref counted InstanceProvider. If no provider is found then
	 * 	null is returned.
	 */
	InstanceProviderIFCRef getInstanceProvider(const ProviderEnvironmentIFCRef& env,
		const String& ns, const CIMClass& cc) const;

	/**
	 * Locate secondary Instance providers.
	 *
	 * @param ns The namespace of the class.
	 * @param className The name of the class.
	 *
	 * @return An array of secondary instance providers which have registered
	 * for the class identified by the className argument.
	 */
	SecondaryInstanceProviderIFCRefArray getSecondaryInstanceProviders(const ProviderEnvironmentIFCRef& env,
		const String& ns, const String& className) const;
	/**
	 * Locate a Method provider.
	 *
	 * @param qual A qualifier describing the method provider. This qualifier
	 * must be a "provider" qualifer with a CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.
	 *
	 * @returns A ref counted MethodProvider. If no provider is found then
	 *	null is returned.
	 */
	MethodProviderIFCRef getMethodProvider(const ProviderEnvironmentIFCRef& env,
		const String& ns, const CIMClass& cc, const CIMMethod& method) const;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * Locate an Associator provider.
	 *
	 * @param ns The namespace of the class.
	 * @param cc The class may have a qualifier describing the instance provider. This qualifier
	 * must be a "provider" qualifer with a CIMValue that contains a
	 * string with the format: "provider interface id::provider string".
	 * The "provider interface string" is used to identify the provider
	 * interface. The "provider string" is provider interface specific. It is
	 * assumed that the provider interface will use this string to identify the
	 * provider.  This function will also return any providers registered for
	 * the class (without the provider qualifier).
	 *
	 * @returns A ref counted AssociatorProvider. If no provider is found then
	 * 	null is returned.
	 */
	AssociatorProviderIFCRef getAssociatorProvider(const ProviderEnvironmentIFCRef& env,
		const String& ns, const CIMClass& cc) const;
#endif
	/**
	 * @return all available indication export providers from the available
	 * provider interfaces.
	 */
	IndicationExportProviderIFCRefArray
		getIndicationExportProviders(const ProviderEnvironmentIFCRef& env) const;
	/**
	 * @return all available polled providers from the available
	 * provider interfaces.
	 */
	PolledProviderIFCRefArray
		getPolledProviders(const ProviderEnvironmentIFCRef& env) const;
	/**
	 * @return all available indication providers from the available
	 * provider interfaces, which are interested in exporting indications of
	 * indicationClassName in namespace ns.
	 */
	IndicationProviderIFCRefArray
		getIndicationProviders(const ProviderEnvironmentIFCRef& env, 
			const String& ns, const String& indicationClassName,
			const String& monitoredClassName) const;
	/**
	 * Call into each ProviderIFC to unload providers which haven't been
	 * used for a while
	 */
	void unloadProviders(const ProviderEnvironmentIFCRef& env);
private:
	ProviderIFCBaseIFCRef getProviderIFC(const ProviderEnvironmentIFCRef& env,
		const CIMQualifier& qual,
		String& provStr) const;
	Array<ProviderIFCBaseIFCRef> m_IFCArray;
	Mutex m_guard;
public:	// so free functions in cpp file can access them.
	struct ProvReg
	{
		String provName;
		ProviderIFCBaseIFCRef ifc;
	};
	typedef HashMap<String, ProvReg> ProvRegMap_t;
	typedef HashMultiMap<String, ProvReg> MultiProvRegMap_t;
	
private:
	// The key must be: a classname if the provider supports any namespace,
	// or namespace:classname for a specific namespace.
	ProvRegMap_t m_registeredInstProvs;

	// The key must be: a classname if the provider supports any namespace,
	// or namespace:classname for a specific namespace.
	MultiProvRegMap_t m_registeredSecInstProvs;

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	ProvRegMap_t m_registeredAssocProvs;
#endif
	// The key must be: [namespace:]className[:methodname]
	ProvRegMap_t m_registeredMethProvs;
	// The key must be: [namespace:]className[:propertyname]
	ProvRegMap_t m_registeredPropProvs;
	// The key must be: a classname if the provider supports any namespace,
	// or namespace:classname for a specific namespace.
	MultiProvRegMap_t m_registeredIndProvs;
};
typedef IntrusiveReference<ProviderManager> ProviderManagerRef;

} // end namespace OpenWBEM

#endif
