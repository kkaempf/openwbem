/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OWBI1_config.h"
#include "OWBI1_SimpleInstanceProviderIFC.hpp"
#include "OWBI1_CIMObjectPath.hpp"
#include "OWBI1_CIMInstance.hpp"
#include "OWBI1_CIMException.hpp"
#include "OWBI1_ResultHandlerIFC.hpp"

namespace OWBI1
{

using namespace WBEMFlags;
namespace {
OWBI1_DECLARE_EXCEPTION(FoundTheInst);
OWBI1_DEFINE_EXCEPTION(FoundTheInst);
//////////////////////////////////////////////////////////////////////////////
class GetInstanceHandler : public CIMInstanceResultHandlerIFC
{
public:
	GetInstanceHandler(const CIMObjectPath& instanceName, CIMInstance& theInst)
		: m_instanceName(instanceName)
		, m_theInst(theInst)
	{
	}
	void doHandle(const CIMInstance& inst)
	{
		if (CIMObjectPath(m_instanceName.getNameSpace(), inst) == m_instanceName)
		{
			m_theInst = inst;
			OWBI1_THROW(FoundTheInstException, "");
		}
	}
private:
	CIMObjectPath m_instanceName;
	CIMInstance& m_theInst;
};
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
CIMInstance
BI1SimpleInstanceProviderIFC::getInstance(
	const ProviderEnvironmentIFCRef &env,
	const String &ns,
	const CIMObjectPath &instanceName,
	const CIMPropertyList& propertyList,
	const CIMClass &cimClass)
{
	CIMInstance theInst(CIMNULL);
	GetInstanceHandler handler(instanceName, theInst);
	// this usage of exceptions is slightly abnormal, we use it to terminate
	// enumInstances once we've found the instance we want. The exception
	// is thrown in GetInstanceHandler::doHandle()
	try
	{
		this->doSimpleEnumInstances(env,ns,cimClass,handler, E_ALL_PROPERTIES);
	}
	catch (const FoundTheInstException&)
	{
		return theInst.clone(propertyList);
	}
	OWBI1_THROWCIMMSG(CIMException::NOT_FOUND, instanceName.toString().c_str());
}
namespace {
//////////////////////////////////////////////////////////////////////////////
class EnumInstanceNamesHandler : public CIMInstanceResultHandlerIFC
{
public:
	EnumInstanceNamesHandler(CIMObjectPathResultHandlerIFC &result,
		const String& ns)
		: m_result(result)
		, m_ns(ns)
	{
	}
	void doHandle(const CIMInstance& inst)
	{
		m_result.handle(CIMObjectPath(m_ns, inst));
	}
private:
	CIMObjectPathResultHandlerIFC& m_result;
	String m_ns;
};
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void
BI1SimpleInstanceProviderIFC::enumInstanceNames(
	const ProviderEnvironmentIFCRef &env,
	const String &ns,
	const CIMName& className,
	CIMObjectPathResultHandlerIFC &result,
	const CIMClass &cimClass)
{
	EnumInstanceNamesHandler handler(result, ns);
	this->doSimpleEnumInstances(env,ns,cimClass,handler, E_KEY_PROPERTIES_ONLY);
}
namespace {
//////////////////////////////////////////////////////////////////////////////
class EnumInstancesHandler : public CIMInstanceResultHandlerIFC
{
public:
	EnumInstancesHandler(CIMInstanceResultHandlerIFC &result,
		const CIMPropertyList& propertyList_)
		: m_result(result)
		, propertyList(propertyList_)
	{
	}
	void doHandle(const CIMInstance& inst)
	{
		m_result.handle(inst.clone(propertyList));
	}
private:
	CIMInstanceResultHandlerIFC& m_result;
	const CIMPropertyList& propertyList;
};
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void
BI1SimpleInstanceProviderIFC::enumInstances(
	const ProviderEnvironmentIFCRef &env,
	const String &ns,
	const CIMName& className,
	CIMInstanceResultHandlerIFC &result,
	const CIMPropertyList& propertyList,
	const CIMClass &requestedClass,
	const CIMClass &cimClass)
{
	EnumInstancesHandler handler(result, propertyList);
	this->doSimpleEnumInstances(env,ns,cimClass,handler, E_ALL_PROPERTIES);
}

} // end namespace OWBI1

