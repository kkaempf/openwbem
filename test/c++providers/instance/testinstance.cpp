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
#include <strstream>

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
	virtual ~OW_TestInstance()
	{
	}

	virtual OW_CIMObjectPathEnumeration enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_Bool deep, 
		OW_CIMClass cimClass );

	virtual OW_CIMInstanceEnumeration enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_Bool deep, 
		OW_CIMClass cimClass, 
		OW_Bool localOnly );

	virtual OW_CIMInstance getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_CIMClass cimClass, 
		OW_Bool localOnly );

	virtual OW_CIMObjectPath createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		OW_CIMInstance cimInstance );

	virtual void setInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_CIMInstance cimInstance);

	virtual void deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop);

};



//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration 
	OW_TestInstance::enumInstanceNames(
	const OW_ProviderEnvironmentIFCRef& env,
	OW_CIMObjectPath cop,
	OW_Bool deep, 
	OW_CIMClass cimClass )
{
	(void)env;
	(void)cimClass;
	(void)deep;
	OW_CIMObjectPathEnumeration rval;
	for (OW_Array<TestInstance>::const_iterator iter = g_saa.begin();
		iter != g_saa.end(); iter++)
	{
		OW_CIMObjectPath instCop = cop;
		instCop.addKey("Name", OW_CIMValue(iter->name));
		rval.addElement(instCop);
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration 
	OW_TestInstance::enumInstances(
	const OW_ProviderEnvironmentIFCRef& env,
	OW_CIMObjectPath cop,
	OW_Bool deep, 
	OW_CIMClass cimClass, 
	OW_Bool localOnly )
{
	(void)cop;
	(void)env;
	(void)localOnly;
	(void)deep;
	OW_CIMInstanceEnumeration rval;
	for (OW_Array<TestInstance>::const_iterator iter = g_saa.begin();
		iter != g_saa.end(); iter++)
	{
		OW_CIMInstance inst = cimClass.newInstance();
		inst.setProperty("Name", OW_CIMValue(iter->name));
		inst.setProperty("Params", OW_CIMValue(iter->params));
		rval.addElement(inst);
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////

OW_CIMInstance 
	OW_TestInstance::getInstance(
	const OW_ProviderEnvironmentIFCRef& env,
	OW_CIMObjectPath cop,
	OW_CIMClass cimClass, 
	OW_Bool localOnly )
{
	(void)cop;
	(void)env;
	(void)localOnly;
	OW_CIMInstance rval = cimClass.newInstance();
	OW_String name;
	cop.getKeys()[0].getValue().get(name);
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
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath 
	OW_TestInstance::createInstance(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMObjectPath& cop,
	OW_CIMInstance cimInstance )
{

	(void)env;
	(void)cop;
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
	return cop;
}

//////////////////////////////////////////////////////////////////////////////
void 
	OW_TestInstance::setInstance(
	const OW_ProviderEnvironmentIFCRef& env,
	OW_CIMObjectPath cop,
	OW_CIMInstance cimInstance)
{
	(void)env;
	(void)cop;
	OW_String name;
	OW_StringArray params;
	cimInstance.getProperty("Name").getValue().get(name);
	cimInstance.getProperty("Params").getValue().get(params);

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
	OW_TestInstance::deleteInstance(
	const OW_ProviderEnvironmentIFCRef& env,
	OW_CIMObjectPath cop)
{
	(void)env;
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


//////////////////////////////////////////////////////////////////////////////

OW_PROVIDERFACTORY(OW_TestInstance, testinstance)

	
