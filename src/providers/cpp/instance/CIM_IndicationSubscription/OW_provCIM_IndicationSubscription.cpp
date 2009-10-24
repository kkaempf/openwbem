/*******************************************************************************
* Copyright (C) 2002-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_Logger.hpp"

namespace OW_NAMESPACE
{

using namespace WBEMFlags;
namespace
{

String const COMPONENT_NAME("ow.provider.CIM_IndicationSubscription");

// This is a pass through provider.  The instances are really stored in the repository.  All CIM_IndicationSubscription actions are intercepted and passed to the indication
// server so it can keep track of subscriptions and also deny the creation of any subscriptions for which no providers exist.
class provCIM_IndicationSubscription : public CppInstanceProviderIFC
{
public:
	virtual void initialize(const ProviderEnvironmentIFCRef& env)
	{
		indicationsEnabled = !(env->getConfigItem(ConfigOpts::DISABLE_INDICATIONS_opt, OW_DEFAULT_DISABLE_INDICATIONS).equalsIgnoreCase("true"));
		// get the indication server and save it.
		if (indicationsEnabled)
		{
			CIMOMEnvironment::instance()->getIndicationServer();
			if (!CIMOMEnvironment::instance()->getIndicationServer())
			{
				indicationsEnabled = false;
			}
		}
	}
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_IndicationSubscription");
	}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual void deleteInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &cop)
	{
		// delete it from the repository
		CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->deleteInstance(ns, cop);
		// tell the indication server it's being deleted.
		CIMOMEnvironment::instance()->getIndicationServer()->startDeleteSubscription(ns, cop);
	}
	virtual CIMObjectPath createInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &cimInstance_)
	{
		// make a copy so we can add the __Subscription_UserName property to it.
		CIMInstance cimInstance(cimInstance_);
		// we add this property so that if the cimom restarts, we can recover the username of whoever created the subscription.
		String username = env->getUserName();
		cimInstance.setProperty("__Subscription_UserName", CIMValue(username));
		if (!indicationsEnabled)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Indication are disabled.  Subscription creation is not allowed.");
		}
		CIMObjectPath rv = env->getRepositoryCIMOMHandle()->createInstance(ns, cimInstance);
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG3(logger, "Created instance in repository; now about to call startCreateSubscription");
		// Tell the indication server about the new subscription.
		CIMOMEnvironment::instance()->getIndicationServer()->startCreateSubscription(ns, cimInstance, username);
		OW_LOG_DEBUG3(logger, "Returned from startCreateSubscription");
		return rv;
	}
	virtual void modifyInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &modifiedInstance, const CIMInstance &previousInstance,
		EIncludeQualifiersFlag includeQualifiers, const StringArray *propertyList, const CIMClass &theClass)
	{
		if (!indicationsEnabled)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Indication are disabled.  Subscription creation is not allowed.");
		}

		CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->modifyInstance(ns, modifiedInstance, includeQualifiers, propertyList);
		// Tell the indication server about the modified subscription.
		CIMOMEnvironment::instance()->getIndicationServer()->startModifySubscription(ns, modifiedInstance.createModifiedInstance(previousInstance,includeQualifiers,propertyList,theClass));
	}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual CIMInstance getInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &instanceName, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList, const CIMClass &cimClass)
	{
		return env->getRepositoryCIMOMHandle()->getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList);
	}

	virtual void enumInstances(const ProviderEnvironmentIFCRef &env, const String &ns, const String &className, CIMInstanceResultHandlerIFC &result, ELocalOnlyFlag localOnly,
		EDeepFlag deep, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList, const CIMClass &requestedClass, const CIMClass &cimClass)
	{
		RepositoryIFCRef rephdl = env->getRepository();
		rephdl->enumInstances(ns,className,result,deep,localOnly,includeQualifiers,includeClassOrigin,propertyList, E_DONT_ENUM_SUBCLASSES, env->getOperationContext());
	}

	virtual void enumInstanceNames(const ProviderEnvironmentIFCRef &env, const String &ns, const String &className, CIMObjectPathResultHandlerIFC &result,
		const CIMClass &cimClass)
	{
		RepositoryIFCRef rephdl = env->getRepository();
		rephdl->enumInstanceNames(ns,className,result, E_SHALLOW, env->getOperationContext());
	}
private:
	bool indicationsEnabled;
};
} // end anonymous namespace
} // end namespace OW_NAMESPACE

OW_PROVIDERFACTORY(OpenWBEM::provCIM_IndicationSubscription, owprovCIM_IndicationSubscription);

