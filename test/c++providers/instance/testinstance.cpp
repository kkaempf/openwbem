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

struct TestInstance
{
	OW_String name;
	OW_StringArray params;
};

static OW_Array<TestInstance> g_saa;

class OW_TestInstance: public OW_CppInstanceProviderIFC
{
public:
	virtual ~OW_TestInstance(){}

	void getProviderInfo(OW_InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("TestInstance");
	}

//////////////////////////////////////////////////////////////////////////////
	void
		enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_CIMClass& cimClass )
	{
		(void)env;
		(void)cimClass;
		for (OW_Array<TestInstance>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			OW_CIMObjectPath instCop(className, ns);
			instCop.addKey("Name", OW_CIMValue(iter->name));
			result.handle(instCop);
		}
	}

//////////////////////////////////////////////////////////////////////////////
	void
		enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool localOnly, 
		OW_Bool deep, 
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList,
		const OW_CIMClass& requestedClass,
		const OW_CIMClass& cimClass )
	{
		(void)ns;
		(void)className;
		(void)env;
		for (OW_Array<TestInstance>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			OW_CIMInstance inst = cimClass.newInstance();
			inst.setProperty("Name", OW_CIMValue(iter->name));
			inst.setProperty("Params", OW_CIMValue(iter->params));
			result.handle(inst.clone(localOnly,deep,includeQualifiers,includeClassOrigin,propertyList,requestedClass,cimClass));
		}
	}

//////////////////////////////////////////////////////////////////////////////

	OW_CIMInstance
		getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly,
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, 
		const OW_CIMClass& cimClass )
	{
		(void)ns;
		(void)env;
		OW_CIMInstance rval = cimClass.newInstance();
		OW_String name;
		instanceName.getKeys()[0].getValue().get(name);
		for (OW_Array<TestInstance>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				rval.setProperty("Name", OW_CIMValue(name));
				rval.setProperty("Params", OW_CIMValue(iter->params));
				break;
			}
		}
		return rval.clone(localOnly, includeQualifiers, includeClassOrigin, 
			propertyList);
	}

//////////////////////////////////////////////////////////////////////////////
	OW_CIMObjectPath
		createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& cimInstance )
	{

		(void)env;
		(void)ns;
		OW_String name;
		OW_StringArray params;
		cimInstance.getProperty("Name").getValue().get(name);

		for (OW_Array<TestInstance>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				OW_THROWCIM(OW_CIMException::ALREADY_EXISTS);
				break;
			}
		}

		cimInstance.getProperty("Params").getValue().get(params);
		TestInstance newInst;
		newInst.name = name;
		newInst.params = params;
		g_saa.push_back(newInst);
		return OW_CIMObjectPath(ns, cimInstance);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance)
	{
		(void)env;
		(void)ns;
		(void)modifiedInstance;
		OW_String name;
		OW_StringArray params;
		modifiedInstance.getProperty("Name").getValue().get(name);
		modifiedInstance.getProperty("Params").getValue().get(params);

		for (OW_Array<TestInstance>::iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				iter->params = params;
				return;
			}
		}
		// new instance
		TestInstance newInst;
		newInst.name = name;
		newInst.params = params;
		g_saa.push_back(newInst);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
	{
		(void)env;
		(void)ns;
		OW_String name;
		cop.getKeys()[0].getValue().get(name);
		for (OW_Array<TestInstance>::iterator iter = g_saa.begin();
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

OW_PROVIDERFACTORY(OW_TestInstance, testinstance)


	
