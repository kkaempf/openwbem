/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_LocalCIMOMHandle.hpp"

#include <fstream>
#include <unistd.h>

using std::ifstream;
using std::ofstream;
using std::endl;
using namespace OpenWBEM;
using namespace WBEMFlags;


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

//////////////////////////////////////////////////////////////////////////////
	void
		enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		(void)env;
		(void)cimClass;
		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			CIMObjectPath instCop(className, ns);
			instCop.addKey("Name", CIMValue(iter->name));
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
		(void)ns;
		(void)className;
		(void)env;
		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			CIMInstance inst = cimClass.newInstance();
			inst.setProperty("Name", CIMValue(iter->name));
			inst.setProperty("Params", CIMValue(iter->params));
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
		(void)ns;
		(void)env;
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

		(void)env;
		(void)ns;
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
		env->getLogger()->logDebug("TestInstance::modifyInstance");
		(void)ns;
		(void)previousInstance;
		(void)includeQualifiers;
		(void)propertyList;
		(void)theClass;
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
		(void)env;
		(void)ns;
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


};




//////////////////////////////////////////////////////////////////////////////

OW_PROVIDERFACTORY(TestInstance, testinstance)


	
