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

#include "OW_config.h"

#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_IndicationServer.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"

namespace
{

// This is a pass through provider.  The instances are really stored in the repository.  All CIM_IndicationSubscription actions are intercepted and passed to the indication
// server so it can keep track of subscriptions and also deny the creation of any subscriptions for which no providers exist.
class OW_provCIM_IndicationSubscription : public OW_CppInstanceProviderIFC
{
public:
	virtual void initialize(const OW_ProviderEnvironmentIFCRef& env)
	{
		indicationsEnabled = !(env->getConfigItem(OW_ConfigOpts::DISABLE_INDICATIONS_opt).equalsIgnoreCase("true"));
		// get the indication server and save it.
		if (indicationsEnabled)
		{
			indicationServer = OW_CIMOMEnvironment::g_cimomEnvironment->getIndicationServer();
			if (!indicationServer)
			{
				indicationsEnabled = false;
			}
		}
	}

	virtual void getProviderInfo(OW_InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_IndicationSubscription");
	}

	virtual void deleteInstance(const OW_ProviderEnvironmentIFCRef &env, const OW_String &ns, const OW_CIMObjectPath &cop)
	{
		// delete it from the repository
		OW_CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->deleteInstance(ns, cop);

		// tell the indication server it's being deleted.
		indicationServer->deleteSubscription(ns, cop);

	}

	virtual OW_CIMObjectPath createInstance(const OW_ProviderEnvironmentIFCRef &env, const OW_String &ns, const OW_CIMInstance &cimInstance_)
	{
        // make a copy so we can add the __OW_Subscription_UserName property to it.
        OW_CIMInstance cimInstance(cimInstance_);

        // we add this property so that if the cimom restarts, we can recover the username of whoever created the subscription.
        OW_String username = env->getUserName();
        cimInstance.setProperty("__OW_Subscription_UserName", OW_CIMValue(username));

		if (!indicationsEnabled)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Indication are disabled.  Subscription creation is not allowed.");
		}

		// Tell the indication server about the new subscription.  This may throw if the subscription is not allowed.
		indicationServer->createSubscription(ns, cimInstance, username);

		// now create it in the repository.
		try
		{
			return env->getRepositoryCIMOMHandle()->createInstance(ns, cimInstance);
		}
		catch (...)
		{
			indicationServer->deleteSubscription(ns, OW_CIMObjectPath(cimInstance));
			throw;
		}
	}

	virtual OW_CIMInstance getInstance(const OW_ProviderEnvironmentIFCRef &env, const OW_String &ns, const OW_CIMObjectPath &instanceName, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray *propertyList, const OW_CIMClass &cimClass)
	{
		(void)cimClass;
		return env->getRepositoryCIMOMHandle()->getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList);
	}
	
	virtual void enumInstances(const OW_ProviderEnvironmentIFCRef &env, const OW_String &ns, const OW_String &className, OW_CIMInstanceResultHandlerIFC &result, OW_Bool localOnly,
		OW_Bool deep, OW_Bool includeQualifiers, OW_Bool includeClassOrigin, const OW_StringArray *propertyList, const OW_CIMClass &requestedClass, const OW_CIMClass &cimClass)
	{
		(void)requestedClass; (void)cimClass;
		OW_CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->enumInstances(ns,className,result,deep,localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}
	
	virtual void enumInstanceNames(const OW_ProviderEnvironmentIFCRef &env, const OW_String &ns, const OW_String &className, OW_CIMObjectPathResultHandlerIFC &result,
		const OW_CIMClass &cimClass)
	{
		(void)cimClass;
		OW_CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->enumInstanceNames(ns,className,result);
	}

	virtual void modifyInstance(const OW_ProviderEnvironmentIFCRef &env, const OW_String &ns, const OW_CIMInstance &modifiedInstance, const OW_CIMInstance &previousInstance,
		OW_Bool includeQualifiers, const OW_StringArray *propertyList, const OW_CIMClass &theClass)
	{
		(void)previousInstance;
		(void)theClass;
		if (!indicationsEnabled)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Indication are disabled.  Subscription creation is not allowed.");
		}
		
		// Tell the indication server about the modified subscription.  This may throw if the subscription is not allowed.
		indicationServer->modifySubscription(ns, modifiedInstance.createModifiedInstance(previousInstance,includeQualifiers,propertyList,theClass));

		OW_CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		try
		{
			rephdl->modifyInstance(ns, modifiedInstance, includeQualifiers, propertyList);
		}
		catch (...)
		{
			indicationServer->modifySubscription(ns, previousInstance);
		}
	}

private:
	bool indicationsEnabled;
	OW_IndicationServerRef indicationServer;
};

} // end anonymous namespace


OW_PROVIDERFACTORY(OW_provCIM_IndicationSubscription, owprovCIM_IndicationSubscription);


