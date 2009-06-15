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
#include "blocxx/Mutex.hpp"
#include "blocxx/HashMap.hpp"
#include "blocxx/HashMultiMap.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_CimomCommonFwd.hpp"
#include "blocxx/SortedVectorSet.hpp"

namespace OW_NAMESPACE
{

/**
 * This class will be used by the CIMOM as a way of finding providers.
 * It will keep the list of provider interfaces and query them when searching
 * for providers.
 */
class OW_CIMOMCOMMON_API ProviderManager : public ServiceIFC
{
public:
	ProviderManager();
	/**
	 * Load and instantiate the ProviderIFCBaseIFC classes using the
	 * ifcLoader to do the work.
	 *
	 * @param ifcLoader the class that will actually load and instantiate the
	 * 	ProviderIFCBaseIFC classes.
	 */
	void load(const ProviderIFCLoaderRef& ifcLoader, const ServiceEnvironmentIFCRef& env);

	virtual String getName() const;
	/**
	 * Initialize the provider interfaces and providers.  This is called after
	 * the CIM Server is up and running, so the providers can access the
	 * repository.  None of the services have been started yet however.
	 */
	virtual void init(const ServiceEnvironmentIFCRef& env);
	
	/**
	 * Called when the cimom is shutting down.  All the provider IFCs will be
	 * unloaded.
	 */
	virtual void shutdown();

	/**
	 * Called just prior to shutting down the CIMOM, to allow shutdown
	 * activities that still require access to the CIMOM.  See
	 * description in base class ServiceIFC.
	 */
	virtual void shuttingDown();

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
	 * @param context The OperationContext.
	 *
	 * @return A ref counted InstanceProvider. If no provider is found then
	 * 	null is returned.
	 */
	InstanceProviderIFCRef getInstanceProvider(
		const String& ns, const CIMClass& cc, OperationContext& context) const;

	/**
	 * Locate secondary Instance providers.
	 *
	 * @param ns The namespace of the class.
     * @param className The name of the class.
     * @param context The OperationContext.
	 *
	 * @return An array of secondary instance providers which have registered
	 * for the class identified by the className argument.
	 */
	SecondaryInstanceProviderIFCRefArray getSecondaryInstanceProviders(
		const String& ns, const CIMName& className, OperationContext& context) const;
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
	 * @param context The OperationContext.
	 *
	 * @returns A ref counted MethodProvider. If no provider is found then
	 *	null is returned.
	 */
	MethodProviderIFCRef getMethodProvider(
		const String& ns, const CIMClass& cc, const CIMMethod& method, OperationContext& context) const;
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
	 * @param context The OperationContext.
	 *
	 * @returns A ref counted AssociatorProvider. If no provider is found then
	 * 	null is returned.
	 */
	AssociatorProviderIFCRef getAssociatorProvider(
		const String& ns, const CIMClass& cc, OperationContext& context) const;
#endif
	/**
	 * @return all available indication export providers from the available
	 * provider interfaces.
	 */
	IndicationExportProviderIFCRefArray	getIndicationExportProviders() const;
	/**
	 * @return all available polled providers from the available
	 * provider interfaces.
	 */
	PolledProviderIFCRefArray getPolledProviders() const;
	/**
	 * @return all available indication providers from the available
	 * provider interfaces, which are interested in exporting indications of
	 * indicationClassName in namespace ns.
	 * @param monitoredClassNames If lifecycle indications are being requested,
	 *  they will be passed in this parameter.
	 * @param context The OperationContext.
	 */
	IndicationProviderIFCRefArray
		getIndicationProviders(
			const String& ns, const CIMName& indicationClassName,
			const CIMNameArray& monitoredClassNames, OperationContext& context) const;

	/**
	 * Locate a Query provider.
	 *
	 * @param ns The namespace of the class.
	 * @param cc The class
	 * @param context The OperationContext.
	 *
	 * @return A QueryProviderRef. If no provider is found then null is returned.
	 */
	QueryProviderIFCRef getQueryProvider(const String& ns, const CIMClass& cc, OperationContext& context) const;

	/**
	 * Call into each ProviderIFC to unload providers which haven't been
	 * used for a while
	 */
	void unloadProviders(const ProviderEnvironmentIFCRef& env);
private:
	ProviderIFCBaseIFCRef getProviderIFC(const ProviderEnvironmentIFCRef& env,
		const CIMQualifier& qual,
		String& provStr) const;
	bool isRestrictedNamespace(const String& ns) const;
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
	typedef SortedVectorSet<String> NameSpaceSet_t;
	
	static String COMPONENT_NAME;

private:

	void findIndicationProviders(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMName& className,
		const ProviderManager::MultiProvRegMap_t& indProvs,
		IndicationProviderIFCRefArray& rval) const;

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
	// namespace:classname for a specific namespace.
	// For a lifecycle provider, /* and /classname entries are made for each lifecycle class.
	MultiProvRegMap_t m_registeredIndProvs;

	// The key must be: a classname if the provider supports any namespace,
	// or namespace:classname for a specific namespace.
	ProvRegMap_t m_registeredQueryProvs;

	Logger m_logger;
	ServiceEnvironmentIFCRef m_env;
	NameSpaceSet_t m_restrictedNamespaces;
};

} // end namespace OW_NAMESPACE

#endif
