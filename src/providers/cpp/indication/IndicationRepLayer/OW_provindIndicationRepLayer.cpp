/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"

#include "OW_CppIndicationProviderIFC.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_IndicationRepLayerMediator.hpp"

#include <iostream>
using namespace std;

// anonymous namespace to prevent symbol conflicts
namespace
{

class OW_provindIndicationRepLayer : public OW_CppIndicationProviderIFC
{
public:
	// Indication provider methods
	virtual int mustPoll(const OW_ProviderEnvironmentIFCRef &, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&) 
	{
		return 0;
	}
	virtual void authorizeFilter(const OW_ProviderEnvironmentIFCRef &, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&, const OW_String &) 
	{
	}
	virtual void activateFilter(const OW_ProviderEnvironmentIFCRef &, const OW_WQLSelectStatement &, const OW_String & eventType, const OW_String&, const OW_StringArray&, bool) 
	{
		OW_CIMOMEnvironmentRef ce(OW_CIMOMEnvironment::g_cimomEnvironment);
		ce->getIndicationRepLayerMediator()->addSubscription(eventType);
		cout << "OW_provindIndicationRepLayer::activateFilter added subscription for " << eventType << endl;
	}
	virtual void deActivateFilter(const OW_ProviderEnvironmentIFCRef &, const OW_WQLSelectStatement &, const OW_String & eventType, const OW_String&, const OW_StringArray&, bool ) 
	{
		OW_CIMOMEnvironmentRef ce(OW_CIMOMEnvironment::g_cimomEnvironment);
		ce->getIndicationRepLayerMediator()->deleteSubscription(eventType);
		cout << "OW_provindIndicationRepLayer::deActivateFilter deleted subscription for " << eventType << endl;
	}

	virtual void getProviderInfo(OW_IndicationProviderInfo& info) 
	{
		// all the life-cycle indications that may be generated by 
		// OW_IndicationRepLayer
		info.addInstrumentedClass("CIM_InstCreation");
		info.addInstrumentedClass("CIM_InstModification");
		info.addInstrumentedClass("CIM_InstDeletion");
		info.addInstrumentedClass("CIM_InstRead");
		info.addInstrumentedClass("CIM_InstMethodCall");
		info.addInstrumentedClass("CIM_ClassCreation");
		info.addInstrumentedClass("CIM_ClassModification");
		info.addInstrumentedClass("CIM_ClassDeletion");

		// OW_IndicationRepLayer doesn't directly generate these, but they are
		// base classes of the ones it does generate.  We need to report these
		// so that if someone has a query like "SELECT * FROM CIM_Indication"
		// we will be called.
		info.addInstrumentedClass("CIM_ClassIndication");
		info.addInstrumentedClass("CIM_InstIndication");
		info.addInstrumentedClass("CIM_Indication");
	}
};


} // end anonymous namespace


OW_PROVIDERFACTORY(OW_provindIndicationRepLayer, owprovindIndicationRepLayer)


