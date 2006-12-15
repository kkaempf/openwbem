/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_OOPInstanceProvider.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_Logger.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_IOIFCStreamBuffer.hpp"
#include "OW_IOException.hpp"
#include "OW_DateTime.hpp"

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");
}

//////////////////////////////////////////////////////////////////////////////
OOPInstanceProvider::OOPInstanceProvider(const OOPProviderInterface::ProvRegInfo& info,
	const Reference<Mutex>& guardRef,
	const Reference<ProcessRef>& persistentProcessRef)
	: OOPProviderBase(info, guardRef, persistentProcessRef)
{
}

//////////////////////////////////////////////////////////////////////////////
OOPInstanceProvider::~OOPInstanceProvider()
{
}
	
namespace
{
	class EnumInstanceNamesCallback : public OOPProviderBase::MethodCallback
	{
	public:
		EnumInstanceNamesCallback(
			const String& ns,
			const String& className,
			CIMObjectPathResultHandlerIFC& result,
			const CIMClass& cimClass )
			: m_ns(ns)
			, m_className(className)
			, m_result(result)
			, m_cimClass(cimClass)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->enumInstanceNames(out, in, timeout, env, m_ns, m_className, m_result, m_cimClass);
		}

	private:
		const String& m_ns;
		const String& m_className;
		CIMObjectPathResultHandlerIFC& m_result;
		const CIMClass& m_cimClass;

	};
}

//////////////////////////////////////////////////////////////////////////////
void
OOPInstanceProvider::enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
{
	EnumInstanceNamesCallback enumInstanceNamesCallback(ns, className, result, cimClass);
	startProcessAndCallFunction(env, enumInstanceNamesCallback, "OOPInstanceProvider::enumInstanceNames", E_SPAWN_NEW_PROCESS);
}

namespace
{
	class EnumInstancesCallback : public OOPProviderBase::MethodCallback
	{
	public:
		EnumInstancesCallback(
			const String& ns,
			const String& className,
			CIMInstanceResultHandlerIFC& result,
			WBEMFlags::ELocalOnlyFlag localOnly,
			WBEMFlags::EDeepFlag deep,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList,
			const CIMClass& requestedClass,
			const CIMClass& cimClass )
			: m_ns(ns)
			, m_className(className)
			, m_result(result)
			, m_localOnly(localOnly)
			, m_deep(deep)
			, m_includeQualifiers(includeQualifiers)
			, m_includeClassOrigin(includeClassOrigin)
			, m_propertyList(propertyList)
			, m_requestedClass(requestedClass)
			, m_cimClass(cimClass)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->enumInstances(out, in, timeout, env, m_ns, m_className, m_result, m_localOnly, m_deep, 
				m_includeQualifiers, m_includeClassOrigin, m_propertyList, m_requestedClass, m_cimClass);
		}

	private:
		const String& m_ns;
		const String& m_className;
		CIMInstanceResultHandlerIFC& m_result;
		WBEMFlags::ELocalOnlyFlag m_localOnly;
		WBEMFlags::EDeepFlag m_deep;
		WBEMFlags::EIncludeQualifiersFlag m_includeQualifiers;
		WBEMFlags::EIncludeClassOriginFlag m_includeClassOrigin;
		const StringArray* m_propertyList;
		const CIMClass& m_requestedClass;
		const CIMClass& m_cimClass;

	};
}

//////////////////////////////////////////////////////////////////////////////
void
OOPInstanceProvider::enumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EDeepFlag deep,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass )
{
	EnumInstancesCallback enumInstancesCallback(ns, className, result, localOnly, deep, includeQualifiers, includeClassOrigin, propertyList, requestedClass, cimClass);
	startProcessAndCallFunction(env, enumInstancesCallback, "OOPInstanceProvider::enumInstances", E_SPAWN_NEW_PROCESS);
}

namespace
{
	class GetInstanceCallback : public OOPProviderBase::MethodCallback
	{
	public:
		GetInstanceCallback(
			CIMInstance& rval,
			const String& ns,
			const CIMObjectPath& instanceName,
			WBEMFlags::ELocalOnlyFlag localOnly,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList,
			const CIMClass& cimClass )
			: m_rval(rval)
			, m_ns(ns)
			, m_instanceName(instanceName)
			, m_localOnly(localOnly)
			, m_includeQualifiers(includeQualifiers)
			, m_includeClassOrigin(includeClassOrigin)
			, m_propertyList(propertyList)
			, m_cimClass(cimClass)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			m_rval = protocol->getInstance(out, in, timeout, env, m_ns, m_instanceName, m_localOnly, 
				m_includeQualifiers, m_includeClassOrigin, m_propertyList, m_cimClass);
		}

	private:
		CIMInstance& m_rval;
		const String& m_ns;
		const CIMObjectPath& m_instanceName;
		WBEMFlags::ELocalOnlyFlag m_localOnly;
		WBEMFlags::EIncludeQualifiersFlag m_includeQualifiers;
		WBEMFlags::EIncludeClassOriginFlag m_includeClassOrigin;
		const StringArray* m_propertyList;
		const CIMClass& m_cimClass;

	};
}

//////////////////////////////////////////////////////////////////////////////
CIMInstance
OOPInstanceProvider::getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& cimClass )
{
	CIMInstance rval(CIMNULL);
	GetInstanceCallback getInstanceCallback(rval, ns, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList, cimClass);
	startProcessAndCallFunction(env, getInstanceCallback, "OOPInstanceProvider::getInstance", E_SPAWN_NEW_PROCESS);
	
	return rval;
}

namespace
{
	class CreateInstanceCallback : public OOPProviderBase::MethodCallback
	{
	public:
		CreateInstanceCallback(
			CIMObjectPath& rval,
			const String& ns,
			const CIMInstance& instance)
			: m_rval(rval)
			, m_ns(ns)
			, m_instance(instance)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			m_rval = protocol->createInstance(out, in, timeout, env, m_ns, m_instance);
		}

	private:
		CIMObjectPath& m_rval;
		const String& m_ns;
		const CIMInstance& m_instance;

	};
}

//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
OOPInstanceProvider::createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
{
	CIMObjectPath rval(CIMNULL);
	CreateInstanceCallback createInstanceCallback(rval, ns, cimInstance);
	startProcessAndCallFunction(env, createInstanceCallback, "OOPInstanceProvider::createInstance", E_SPAWN_NEW_PROCESS);
	
	return rval;
}

namespace
{
	class ModifyInstanceCallback : public OOPProviderBase::MethodCallback
	{
	public:
		ModifyInstanceCallback(
			const String& ns,
			const CIMInstance& modifiedInstance,
			const CIMInstance& previousInstance,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			const StringArray* propertyList,
			const CIMClass& theClass)
			: m_ns(ns)
			, m_modifiedInstance(modifiedInstance)
			, m_previousInstance(previousInstance)
			, m_includeQualifiers(includeQualifiers)
			, m_propertyList(propertyList)
			, m_theClass(theClass)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->modifyInstance(out, in, timeout, env, m_ns, m_modifiedInstance, m_previousInstance, m_includeQualifiers,
				m_propertyList, m_theClass);
		}

	private:
		const String& m_ns;
		const CIMInstance& m_modifiedInstance;
		const CIMInstance& m_previousInstance;
		WBEMFlags::EIncludeQualifiersFlag m_includeQualifiers;
		const StringArray* m_propertyList;
		const CIMClass& m_theClass;

	};
}

//////////////////////////////////////////////////////////////////////////////
void
OOPInstanceProvider::modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
{
	ModifyInstanceCallback modifyInstanceCallback(ns, modifiedInstance, previousInstance, includeQualifiers, propertyList, theClass);
	startProcessAndCallFunction(env, modifyInstanceCallback, "OOPInstanceProvider::modifyInstance", E_SPAWN_NEW_PROCESS);
}

namespace
{
	class DeleteInstanceCallback : public OOPProviderBase::MethodCallback
	{
	public:
		DeleteInstanceCallback(
			const String& ns,
			const CIMObjectPath& cop)
			: m_ns(ns)
			, m_cop(cop)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->deleteInstance(out, in, timeout, env, m_ns, m_cop);
		}

	private:
		const String& m_ns;
		const CIMObjectPath& m_cop;

	};
}

//////////////////////////////////////////////////////////////////////////////
void
OOPInstanceProvider::deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
{
	DeleteInstanceCallback deleteInstanceCallback(ns, cop);
	startProcessAndCallFunction(env, deleteInstanceCallback, "OOPInstanceProvider::deleteInstance", E_SPAWN_NEW_PROCESS);
}


} // end namespace OW_NAMESPACE



