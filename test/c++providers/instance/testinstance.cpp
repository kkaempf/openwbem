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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "OW_CppProviderIncludes.hpp"
#include "OW_SessionLanguage.hpp"

#include <unistd.h>

using std::endl;
using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{
	const String COMPONENT_NAME("ow.test.TestInstance");
}


struct TestInstanceData
{
	String name;
	StringArray params;
};

static Array<TestInstanceData> g_saa;

class TestInstance: public CppInstanceProviderIFC
{
public:
	virtual ~TestInstance(){}

	void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("TestInstance");
	}

	// we store stuff in memory, if we get unloaded the test suite will fail.
	virtual bool canUnload()
	{
		return false;
	}

//////////////////////////////////////////////////////////////////////////////
	void
		enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			CIMObjectPath instCop(className, ns);
			instCop.setKeyValue("Name", CIMValue(iter->name));
			result.handle(instCop);
		}
	}

//////////////////////////////////////////////////////////////////////////////
	void
		enumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		ELocalOnlyFlag localOnly,
		EDeepFlag deep,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass )
	{
		String al = procAcceptLanguage(env);
		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			CIMInstance inst = cimClass.newInstance();
			inst.setProperty("Name", CIMValue(iter->name));
			inst.setProperty("Params", CIMValue(iter->params));
			inst.setProperty("AcceptLanguage", CIMValue(al));
			inst.setLanguage("x-owtest");
			result.handle(inst.clone(localOnly,deep,includeQualifiers,includeClassOrigin,propertyList,requestedClass,cimClass));
		}
	}

//////////////////////////////////////////////////////////////////////////////

	CIMInstance
		getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& cimClass )
	{
		String al = procAcceptLanguage(env);
		CIMInstance rval = cimClass.newInstance();
		String name;
		instanceName.getKeys()[0].getValue().get(name);
		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				rval.setProperty("Name", CIMValue(name));
				rval.setProperty("Params", CIMValue(iter->params));
				rval.setProperty("AcceptLanguage", CIMValue(al));
				rval.setLanguage("x-owtest");
				break;
			}
		}
		return rval.clone(localOnly, includeQualifiers, includeClassOrigin,
			propertyList);
	}

//////////////////////////////////////////////////////////////////////////////
	CIMObjectPath
		createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{

		String name;
		StringArray params;
		cimInstance.getProperty("Name").getValue().get(name);

		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				OW_THROWCIM(CIMException::ALREADY_EXISTS);
				break;
			}
		}

		cimInstance.getProperty("Params").getValue().get(params);
		TestInstanceData newInst;
		newInst.name = name;
		newInst.params = params;
		g_saa.push_back(newInst);
		return CIMObjectPath(ns, cimInstance);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "TestInstance::modifyInstance");
		String name;
		StringArray params;
		modifiedInstance.getProperty("Name").getValue().get(name);
		modifiedInstance.getProperty("Params").getValue().get(params);

		for (Array<TestInstanceData>::iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				iter->params = params;
				return;
			}
		}
		// new instance
		TestInstanceData newInst;
		newInst.name = name;
		newInst.params = params;
		g_saa.push_back(newInst);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		String name;
		cop.getKeys()[0].getValue().get(name);
		for (Array<TestInstanceData>::iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				g_saa.erase(iter);
				break;
			}
		}
	}

//////////////////////////////////////////////////////////////////////////////
	String
	procAcceptLanguage(const ProviderEnvironmentIFCRef& env)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "TestInstance::procAcceptLanguage");

		String al;
		OperationContext& oc(env->getOperationContext());
		SessionLanguageRef slref = oc.getDataAs<SessionLanguage>(OperationContext::SESSION_LANGUAGE_KEY);
		if (slref)
		{
			al = slref->getAcceptLanguageString();
			slref->addContentLanguage("x-testinst");
			OW_LOG_DEBUG(logger, "TestInstance::procAcceptLanguage"
				" setting content-language in SessionLanguage object");
			String cl = slref->getContentLanguage();
			OW_LOG_DEBUG(logger, Format(
				"TestInstance::procAcceptLanguage content-language"
				" now is %1", cl).c_str());
		}
		else
		{
			OW_LOG_DEBUG(logger, "TestInstance::procAcceptLanguage"
				" didn't find SessionLanguage object in opctx");
		}

		return al;
	}
};






//////////////////////////////////////////////////////////////////////////////

OW_PROVIDERFACTORY(TestInstance, testinstance)



