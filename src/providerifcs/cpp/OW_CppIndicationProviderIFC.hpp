/*******************************************************************************
* Copyright (C) 2002 Vintela, Inc. All rights reserved.
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
#ifndef OW_CPP_INDICATIONPROVIDERIFC_HPP_
#define OW_CPP_INDICATIONPROVIDERIFC_HPP_
#include "OW_config.h"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_IndicationProviderInfo.hpp"

namespace OpenWBEM
{

class WQLSelectStatement;
/**
 * This is the interface implemented by indication providers.
 */
class CppIndicationProviderIFC: public virtual CppProviderBaseIFC
{
public:
	virtual ~CppIndicationProviderIFC();
	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 * It should also insert one entry for each base class of the classes 
	 * of indications the provider may generate.  Note that this base class
	 * requirement is unique to indication providers and doesn't apply to
	 * other types of providers.
	 * The entry consists of the class name and an optional list of namespaces.
	 * If the namespace list is empty, all namespaces are implied.
	 * If the method does nothing, then the provider's class must have a
	 * provider qualifier that identifies the provider.  This old behavior is
	 * deprecated and will be removed sometime in the future.  This method
	 * has a default implementation that does nothing, to allow old providers
	 * to be migrated forward with little or no change, but once the old
	 * provider location method is removed, this member function will be pure
	 * virtual.
	 */
	virtual void getIndicationProviderInfo(IndicationProviderInfo& info);
	
	virtual void activateFilter(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes, 
		bool firstActivation
		);
	virtual void authorizeFilter(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes, 
		const String& owner
		);
	virtual void deActivateFilter(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes, 
		bool lastActivation
		);
	/**
	 * If a provider wishes to be polled, it must return a positive number.
	 * The int returned will be the number of seconds between polls.
	 * The cimom performs polling by calling enumInstances() each polling
	 * cycle and comparing the results with the previous cycle.  It will
	 * generate CIM_Inst{Creation,Modification,Deletion} indications based
	 * on the difference in the instances.
	 * The namespace/classname that will be passed into enumInstances is
	 * the same one that is passed as the classPath parameter to mustPoll().
	 * If an event provider does not want to be polled, it should return 0.
	 * If the provider is going to start a thread that will wait for some
	 * external event, it should do it the first time activateFilter is called
	 * firstActivation will == true.
	 * If a provider may take a long time to generate all instances in 
	 * enumInstances, it should either not be polled or it should have a large
	 * poll interval.
	 * Also, a provider that is polled can only do lifecycle indications.
	 * If the provider doesn't do lifecycle indications, then it must return
	 * 0 from mustPoll, and has to generate indications by another means.
	 * @param env 
	 */
	virtual int mustPoll(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes
		);
	virtual CppIndicationProviderIFC* getIndicationProvider();
};
typedef SharedLibraryReference< Reference<CppIndicationProviderIFC> > CppIndicationProviderIFCRef;

} // end namespace OpenWBEM

typedef OpenWBEM::CppIndicationProviderIFC OW_CppIndicationProviderIFC OW_DEPRECATED;

#endif
								
