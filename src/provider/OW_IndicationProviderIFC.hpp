/*******************************************************************************
* Copyright (C) 2002 Center 7, Inc All rights reserved.
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

#ifndef OW_INDICATION_PROVIDER_IFC_HPP_INCLUDE_GUARD_
#define OW_INDICATION_PROVIDER_IFC_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ProviderBaseIFC.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"

class OW_WQLSelectStatement;

/**
 * This is the interface implemented by indication providers.
 */
class OW_IndicationProviderIFC: public OW_ProviderBaseIFC
{
public:
	virtual ~OW_IndicationProviderIFC();

    // TODO: Add docs for these functions.
	virtual void activateFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes
		) = 0;

	virtual void authorizeFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes, 
		const OW_String& owner
		) = 0;

	virtual void deActivateFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes
		) = 0;

	// if a provider wishes to be polled, it must return a positive number.
	// The int returned will be the number of seconds between polls.
	// The cimom performs polling by calling enumInstances() each polling
	// cycle and comparing the results with the previous cycle.  It will
	// generate CIM_Inst{Creation,Modification,Deletion} indications based
	// on the difference in the instances.
	// The namespace/classname that will be passed into enumInstances is
	// the same one that is passed as the classPath parameter to mustPoll().
	// If an event provider does not want to be polled, it should return 0.
	// If the provider is going to start a thread that will wait for some
	// external event, it should do it the first time activateFilter is called
	// firstActivation will == true.
	// If a provider may take a long time to generate all instances in 
	// enumInstances, it should not be polled and have a short poll interval.
	// Also, a provider that is polled can only do lifecycle indications.
	// If the provider doesn't do lifecycle indications, then it must return
	// 0 from mustPoll, and has to generate indications by another means.
	virtual int mustPoll(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes
		) = 0;


};

//typedef OW_IntrusiveReference< OW_IndicationProviderIFC > OW_IndicationProviderIFCRef;
typedef OW_Reference< OW_IndicationProviderIFC > OW_IndicationProviderIFCRef;
typedef OW_Array<OW_IndicationProviderIFCRef>
		OW_IndicationProviderIFCRefArray;

#endif

