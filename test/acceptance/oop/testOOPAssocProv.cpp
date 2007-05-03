/*******************************************************************************
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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
 * @author Jon Carey
 */


#include "OW_config.h"
#include "OW_CppProviderIncludes.hpp"
#include "OW_CIMValue.hpp"
//#include "OW_SessionLanguage.hpp"

using namespace std;
using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{

struct InstData
{
	String key;
	Int32 value;
};

InstData Inst1s[] =
{
	{ "key1", 1 },
	{ "key2", 2 },
	{ "key3", 3 },
	{ "key4", 4 },
	{ "key5", 5 },
	{ "key6", 6 },
	{ "key7", 7 },
	{ "key8", 8 },
	{ "key9", 9 },
	{ "key10", 10 }
};

InstData Inst100s[] =
{
	{ "key100", 100 },
	{ "key200", 200 },
	{ "key300", 300 },
	{ "key400", 400 },
	{ "key500", 500 },
	{ "key600", 600 },
	{ "key700", 700 },
	{ "key800", 800 },
	{ "key900", 900 },
	{ "key1000", 1000 }
};

InstData*
getInstData(const String& id, InstData* pInstData, int* pndx=0)
{
	for (int i = 0; i < 10; i++)
	{
		if (id.equalsIgnoreCase(pInstData[i].key))
		{
			if (pndx)
			{
				*pndx = i;
			}
			return &pInstData[i];
		}
	}
	return 0;
}

InstData*
getInstData1(const String& id, int* pndx=0)
{
	return getInstData(id, Inst1s, pndx);
}

InstData*
getInstData100(const String& id, int* pndx=0)
{
	return getInstData(id, Inst100s, pndx);
}


const String COMPONENT_NAME("ow.test.testOOPAssocProv");

class testOOPAssocProv: public CppSimpleAssociatorProviderIFC
{
public:
	testOOPAssocProv() {}

	virtual ~testOOPAssocProv() {}

//////////////////////////////////////////////////////////////////////////////
	void enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPAssocProv::enumInstanceNames() start");
		CIMObjectPath cop(className, ns);
		InstData* pInstData = (className.equalsIgnoreCase("ooptestInst1"))
			?  Inst1s : Inst100s;
		for (int i = 0; i < 10; i++)
		{
			cop.setKeyValue("id", CIMValue(pInstData[i].key));
			result.handle(cop);
		}
	}

//////////////////////////////////////////////////////////////////////////////
	void enumInstances(
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
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPAssocProv::enumInstances() start");
		OW_LOG_DEBUG(logger, Format("env->getUserName() = %1" , env->getUserName()));
		if (env->getUserName() != "test1")
		{
			OW_LOG_DEBUG(logger, "aborting!");
			abort();
		}

		CIMInstance ci = cimClass.newInstance();
		InstData* pInstData = (className.equalsIgnoreCase("ooptestInst1"))
			?  Inst1s : Inst100s;
		for (int i = 0; i < 10; i++)
		{
			ci.setProperty("id", CIMValue(pInstData[i].key));
			ci.setProperty("value", CIMValue(pInstData[i].value));
			result.handle(ci.clone(localOnly, includeQualifiers,
				includeClassOrigin, propertyList));
		}
	}

//////////////////////////////////////////////////////////////////////////////

	CIMInstance getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& cimClass )
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPAssocProv::getInstance() start");
		String id;
		CIMValue cv	= instanceName.getKeyValue("id");
		if (cv)
		{
			cv.get(id);
		}
		String className = cimClass.getName();
		InstData* pInstData = (className.equalsIgnoreCase("ooptestInst1"))
			?  getInstData1(id) : getInstData100(id);

		if (!pInstData)
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND, 
				Format("id = %1", id).c_str());
		}

		CIMInstance ci = cimClass.newInstance();
		ci.setProperty("id", CIMValue(pInstData->key));
		ci.setProperty("value", CIMValue(pInstData->value));
		return ci.clone(localOnly, includeQualifiers, includeClassOrigin,
			propertyList);
	}

//////////////////////////////////////////////////////////////////////////////
	CIMObjectPath createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPAssocProv::createInstance() start");
		OW_THROWCIM(CIMException::NOT_SUPPORTED);
	}

//////////////////////////////////////////////////////////////////////////////
	void modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPAssocProv::modifyInstance");
		OW_THROWCIM(CIMException::NOT_SUPPORTED);
	}

//////////////////////////////////////////////////////////////////////////////
	void deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPAssocProv::deleteInstance");
		OW_THROWCIM(CIMException::NOT_SUPPORTED);
	}

//////////////////////////////////////////////////////////////////////////////
	void doReferences(
		const ProviderEnvironmentIFCRef &env,
		CIMInstanceResultHandlerIFC &result,
		const String &ns,
		const CIMObjectPath &objectName,
		const CIMClass &assocClass,
		const String &resultClass,
		const String &role,
		const String &resultRole)
	{
		String objClassName = objectName.getClassName();
		String id;
		CIMValue cv = objectName.getKeyValue("id");
		if (cv)
		{
			cv.get(id);
		}

		if (id.empty())
		{
			return;
		}

		CIMInstance ci = assocClass.newInstance();
		InstData* pInstData = 0;
		if (objClassName.equalsIgnoreCase("ooptestInst1"))
		{
			int ndx;
			pInstData = getInstData1(id, &ndx);
			if (pInstData)
			{
				ci.setProperty("inst1", CIMValue(objectName));
				CIMObjectPath cop("ooptestInst100", ns);
				cop.setKeyValue("id", CIMValue(Inst100s[ndx].key));
				ci.setProperty("inst100", CIMValue(cop));
				result.handle(ci);
			}
		}
		else
		{
			int ndx;
			pInstData = getInstData100(id, &ndx);
			if (pInstData)
			{
				ci.setProperty("inst100", CIMValue(objectName));
				CIMObjectPath cop("ooptestInst1", ns);
				cop.setKeyValue("id", CIMValue(Inst1s[ndx].key));
				ci.setProperty("inst1", CIMValue(cop));
				result.handle(ci);
			}
		}
	}

};


} // end unnamed namespace



//////////////////////////////////////////////////////////////////////////////

OW_PROVIDERFACTORY(testOOPAssocProv, oopassoctest)



