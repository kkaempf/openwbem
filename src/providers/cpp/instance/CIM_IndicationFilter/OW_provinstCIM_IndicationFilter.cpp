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

namespace OpenWBEM
{

using namespace WBEMFlags;
namespace
{
// This is a pass through provider.  The instances are really stored in the repository.  All CIM_IndicationFilter modifications are intercepted and passed to the indication
// server so it can keep track of subscriptions.
class provinstCIM_IndicationFilter : public CppInstanceProviderIFC
{
public:
	virtual void initialize(const ProviderEnvironmentIFCRef& env)
	{
		indicationsEnabled = !(env->getConfigItem(ConfigOpts::DISABLE_INDICATIONS_opt).equalsIgnoreCase("true"));
		// get the indication server and save it.
		if (indicationsEnabled)
		{
			indicationServer = CIMOMEnvironment::g_cimomEnvironment->getIndicationServer();
			if (!indicationServer)
			{
				indicationsEnabled = false;
			}
		}
	}
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("CIM_IndicationFilter");
	}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual void deleteInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &cop)
	{
		// delete it from the repository
		CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->deleteInstance(ns, cop);
	}
	virtual CIMObjectPath createInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &cimInstance)
	{
		if (!indicationsEnabled)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Indication are disabled.  Filter creation is not allowed.");
		}
		return env->getRepositoryCIMOMHandle()->createInstance(ns, cimInstance);
	}
	virtual void modifyInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &modifiedInstance, const CIMInstance &previousInstance,
			EIncludeQualifiersFlag includeQualifiers, const StringArray *propertyList, const CIMClass &theClass)
	{
		(void)previousInstance;
		(void)theClass;
		if (!indicationsEnabled)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Indication are disabled.  Filter creation is not allowed.");
		}
		CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->modifyInstance(ns, modifiedInstance, includeQualifiers, propertyList);
		// Tell the indication server about the modified subscription.
		indicationServer->modifyFilter(ns, modifiedInstance.createModifiedInstance(previousInstance,includeQualifiers,propertyList,theClass));
	}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual CIMInstance getInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &instanceName, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList, const CIMClass &cimClass)
	{
		(void)cimClass;
		return env->getRepositoryCIMOMHandle()->getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList);
	}
	virtual void enumInstances(const ProviderEnvironmentIFCRef &env, const String &ns, const String &className, CIMInstanceResultHandlerIFC &result, ELocalOnlyFlag localOnly,
			EDeepFlag deep, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList, const CIMClass &requestedClass, const CIMClass &cimClass)
	{
		(void)requestedClass; (void)cimClass;
		CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->enumInstances(ns,className,result,deep,localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}
	virtual void enumInstanceNames(const ProviderEnvironmentIFCRef &env, const String &ns, const String &className, CIMObjectPathResultHandlerIFC &result,
			const CIMClass &cimClass)
	{
		(void)cimClass;
		CIMOMHandleIFCRef rephdl = env->getRepositoryCIMOMHandle();
		rephdl->enumInstanceNames(ns,className,result);
	}
private:
	bool indicationsEnabled;
	IndicationServerRef indicationServer;
};
} // end anonymous namespace
} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::provinstCIM_IndicationFilter, owprovinstCIM_IndicationFilter);

