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
#include "OW_CppSimpleInstanceProviderIFC.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"

using namespace OW_WBEMFlags;

namespace {

DECLARE_EXCEPTION(FoundTheInst);
DEFINE_EXCEPTION(FoundTheInst);
//////////////////////////////////////////////////////////////////////////////
class GetInstanceHandler : public OW_CIMInstanceResultHandlerIFC
{
public:
	GetInstanceHandler(const OW_CIMObjectPath& instanceName, OW_CIMInstance& theInst)
		: m_instanceName(instanceName)
		, m_theInst(theInst)
	{
	}

	void doHandle(const OW_CIMInstance& inst)
	{
		if (OW_CIMObjectPath(m_instanceName.getNameSpace(), inst) == m_instanceName)
		{
			m_theInst = inst;
			OW_THROW(OW_FoundTheInstException, "");
		}
	}

private:
	OW_CIMObjectPath m_instanceName;
	OW_CIMInstance& m_theInst;
};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance 
OW_CppSimpleInstanceProviderIFC::getInstance(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_String &ns, 
	const OW_CIMObjectPath &instanceName, 
	ELocalOnlyFlag localOnly, 
	EIncludeQualifiersFlag includeQualifiers, 
	EIncludeClassOriginFlag includeClassOrigin, 
	const OW_StringArray *propertyList, 
	const OW_CIMClass &cimClass)
{
	OW_CIMInstance theInst(OW_CIMNULL);
	GetInstanceHandler handler(instanceName, theInst);
	// this usage of exceptions is slightly abnormal, we use it to terminate
	// enumInstances once we've found the instance we want. The exception
	// is thrown in GetInstanceHandler::doHandle()
	try
	{
		this->doSimpleEnumInstances(env,ns,cimClass,handler, E_ALL_PROPERTIES);
	}
	catch (const OW_FoundTheInstException&)
	{
		return theInst.clone(localOnly, includeQualifiers, includeClassOrigin, propertyList);
	}
	OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, instanceName.toString().c_str());
}

namespace {

//////////////////////////////////////////////////////////////////////////////
class EnumInstanceNamesHandler : public OW_CIMInstanceResultHandlerIFC
{
public:
	EnumInstanceNamesHandler(OW_CIMObjectPathResultHandlerIFC &result,
		const OW_String& ns)
		: m_result(result)
		, m_ns(ns)
	{
	}

	void doHandle(const OW_CIMInstance& inst)
	{
		m_result.handle(OW_CIMObjectPath(m_ns, inst));
	}

private:
	OW_CIMObjectPathResultHandlerIFC& m_result;
	OW_String m_ns;
};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
void 
OW_CppSimpleInstanceProviderIFC::enumInstanceNames(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_String &ns, 
	const OW_String &className, 
	OW_CIMObjectPathResultHandlerIFC &result, 
	const OW_CIMClass &cimClass)
{
	(void)className;
	EnumInstanceNamesHandler handler(result, ns);
	this->doSimpleEnumInstances(env,ns,cimClass,handler, E_KEY_PROPERTIES_ONLY);
}


namespace {

//////////////////////////////////////////////////////////////////////////////
class EnumInstancesHandler : public OW_CIMInstanceResultHandlerIFC
{
public:
	EnumInstancesHandler(OW_CIMInstanceResultHandlerIFC &result,
		ELocalOnlyFlag localOnly_,
		EDeepFlag deep_, 
		EIncludeQualifiersFlag includeQualifiers_, 
		EIncludeClassOriginFlag includeClassOrigin_, 
		const OW_StringArray *propertyList_, 
		const OW_CIMClass &requestedClass_, 
		const OW_CIMClass &cimClass_)
		: m_result(result)
		, localOnly(localOnly_)
		, deep(deep_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		, requestedClass(requestedClass_)
		, cimClass(cimClass_)
	{
	}

	void doHandle(const OW_CIMInstance& inst)
	{
		m_result.handle(inst.clone(localOnly, deep, includeQualifiers,
			includeClassOrigin, propertyList, requestedClass,
			cimClass));
	}

private:
	OW_CIMInstanceResultHandlerIFC& m_result;
	ELocalOnlyFlag localOnly;
	EDeepFlag deep;
	EIncludeQualifiersFlag includeQualifiers;
	EIncludeClassOriginFlag includeClassOrigin;
	const OW_StringArray *propertyList;
	const OW_CIMClass &requestedClass;
	const OW_CIMClass &cimClass;
};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
void 
OW_CppSimpleInstanceProviderIFC::enumInstances(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_String &ns, 
	const OW_String &className, 
	OW_CIMInstanceResultHandlerIFC &result, 
	ELocalOnlyFlag localOnly,
	EDeepFlag deep, 
	EIncludeQualifiersFlag includeQualifiers, 
	EIncludeClassOriginFlag includeClassOrigin, 
	const OW_StringArray *propertyList, 
	const OW_CIMClass &requestedClass, 
	const OW_CIMClass &cimClass)
{
	(void)className;
	EnumInstancesHandler handler(result, localOnly, deep, includeQualifiers,
		includeClassOrigin, propertyList, requestedClass, cimClass);
	this->doSimpleEnumInstances(env,ns,cimClass,handler, E_ALL_PROPERTIES);
}


