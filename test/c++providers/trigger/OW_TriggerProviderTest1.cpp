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

#include "OW_config.h"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMObjectPath.hpp"

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded

static const OW_Int32 INITIAL_POLLING_INTERVAL = 11;
static const OW_Int32 POLLING_INTERVAL = 2;


class OW_TriggerProviderTest1 : public OW_CppPolledProviderIFC
{
	// Testing polling interval
public:

	~OW_TriggerProviderTest1();

	/**
	 * @return The amount of seconds before the first call to the poll method.
	 * If this method returns zero, then the poll method is never called.
	 */
	virtual OW_Int32 getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef& env);

	/**
	 * Called by the CIMOM to give this OW_CppPolledProvider to
	 * opportunity to export indications if needed.
	 * @param lch	A local CIMOM handle the provider can use to export
	 *					indications if needed.
	 * @return How many seconds before the next call to the poll method. If this
	 * method returns -1 then the last polling interval will be used. If it
	 * returns 0 then the poll method will never be called again.
	 */
	virtual OW_Int32 poll(const OW_ProviderEnvironmentIFCRef& env);

	/**
	 * Called by the CIMOM when the provider is initialized
	 *
	 * @param hdl The handle to the cimom
	 *
	 * @throws OW_CIMException
	 */
	virtual void initialize(const OW_ProviderEnvironmentIFCRef& env);

	/**
	 * Called by the CIMOM when the provider is removed.
	 *
	 * @throws OW_CIMException
	 */
	virtual void cleanup();
};

//////////////////////////////////////////////////////////////////////////////
OW_TriggerProviderTest1::~OW_TriggerProviderTest1()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TriggerProviderTest1::initialize(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug("OW_TriggerProviderTest1::initialize called");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TriggerProviderTest1::cleanup()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_TriggerProviderTest1::getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug(
		format("OW_TriggerProviderTest1::getInitialPollingInterval() called."
		" return %1 seconds", INITIAL_POLLING_INTERVAL));

	return INITIAL_POLLING_INTERVAL;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_TriggerProviderTest1::poll(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug(
		format("OW_TriggerProviderTest1::poll() called."
		" return %1 seconds", POLLING_INTERVAL));
	
	static int count = 1;
	
	OW_CIMOMHandleIFCRef lch = env->getCIMOMHandle();

	OW_CIMClass cc = lch->getClass("root/testsuite", "OW_TestIndication1");
	if (cc)
	{
		OW_CIMInstance ci = cc.newInstance();
		ci.setProperty("indicationNumber", OW_CIMValue(count));
		OW_CIMClass embeddedClass = lch->getClass("root/testsuite", "TestClass1");
		ci.setProperty("TheClass", OW_CIMValue(embeddedClass));
		env->getLogger()->logDebug("OW_TriggerProviderTest1: about to export indication");
		try
		{
			lch->exportIndication(ci, OW_CIMNameSpace("root/testsuite"));
		}
		catch (const OW_Exception& e)
		{
			env->getLogger()->logDebug("Caught exception" + OW_String(e.type()) + " " + e.getFile() + " " + OW_String(e.getLine())
				+ " " + e.getMessage());
		}
		env->getLogger()->logDebug("OW_TriggerProviderTest1: exported indication");
	}
	else
	{
		env->getLogger()->logDebug("OW_TestIndication1 class does not exist!");
	}

	if(count++ == 8)
	{
		return 10;
	}

	/*
	if (count++ <= 7)
	{
		return POLLING_INTERVAL;
	}
	else
	{
		return 0;
	}
	*/
	return POLLING_INTERVAL;
}

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
OW_NOIDPROVIDERFACTORY(OW_TriggerProviderTest1);

