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
#include "OW_config.h"
#include "OW_CppProxyProvider.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////		
CppAssociatorProviderProxy::CppAssociatorProviderProxy(
	CppAssociatorProviderIFCRef pProv) :
	AssociatorProviderIFC(), m_pProv(pProv)
{
}
//////////////////////////////////////////////////////////////////////////////		
void
CppAssociatorProviderProxy::associators(
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	m_pProv->updateAccessTime();
	m_pProv->associators(env, result, ns, objectName, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList);
}
//////////////////////////////////////////////////////////////////////////////		
void
CppAssociatorProviderProxy::associatorNames(
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole)
{
	m_pProv->updateAccessTime();
	m_pProv->associatorNames(env, result, ns, objectName, assocClass, resultClass,
			role, resultRole);
}
//////////////////////////////////////////////////////////////////////////////		
void
CppAssociatorProviderProxy::references(
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	m_pProv->updateAccessTime();
	m_pProv->references(env, result, ns, objectName, resultClass, role,
			includeQualifiers, includeClassOrigin, propertyList);
}
//////////////////////////////////////////////////////////////////////////////		
void
CppAssociatorProviderProxy::referenceNames(
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role)
{
	m_pProv->updateAccessTime();
	m_pProv->referenceNames(env, result, ns, objectName, resultClass, role);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////		
CppInstanceProviderProxy::CppInstanceProviderProxy(
		CppInstanceProviderIFCRef pProv) :
	InstanceProviderIFC(), m_pProv(pProv)
{
}
//////////////////////////////////////////////////////////////////////////////		
void
CppInstanceProviderProxy::enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	return m_pProv->enumInstanceNames(env, ns, className, result, cimClass);
}
//////////////////////////////////////////////////////////////////////////////		
void
CppInstanceProviderProxy::enumInstances(
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
		const CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	m_pProv->enumInstances(env, ns, className, result, localOnly, deep, 
		includeQualifiers, includeClassOrigin, propertyList, requestedClass, 
		cimClass);
}
//////////////////////////////////////////////////////////////////////////////		
CIMInstance
CppInstanceProviderProxy::getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, 
		const CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	return m_pProv->getInstance(env, ns, instanceName, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList, cimClass);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////		
CIMObjectPath
CppInstanceProviderProxy::createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance)
{
	m_pProv->updateAccessTime();
	return m_pProv->createInstance(env, ns, cimInstance);
}
//////////////////////////////////////////////////////////////////////////////		
void
CppInstanceProviderProxy::modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
{
	m_pProv->updateAccessTime();
	m_pProv->modifyInstance(env, ns, modifiedInstance, previousInstance, 
		includeQualifiers, propertyList, theClass);
}
//////////////////////////////////////////////////////////////////////////////		
void
CppInstanceProviderProxy::deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
{
	m_pProv->updateAccessTime();
	m_pProv->deleteInstance(env, ns, cop);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////		
CppSecondaryInstanceProviderProxy::CppSecondaryInstanceProviderProxy(CppSecondaryInstanceProviderIFCRef pProv)
	: SecondaryInstanceProviderIFC()
	, m_pProv(pProv)
{
}
//////////////////////////////////////////////////////////////////////////////		
void
CppSecondaryInstanceProviderProxy::filterInstances(
	const ProviderEnvironmentIFCRef &env, const String &ns, 
	const String &className, CIMInstanceArray &instances, 
	ELocalOnlyFlag localOnly, EDeepFlag deep, 
	EIncludeQualifiersFlag includeQualifiers, 
	EIncludeClassOriginFlag includeClassOrigin, 
	const StringArray *propertyList, const CIMClass &requestedClass, 
	const CIMClass &cimClass)
{
	m_pProv->updateAccessTime();
	m_pProv->filterInstances(env, ns, className, instances, localOnly, deep, includeQualifiers, includeClassOrigin, propertyList, requestedClass, cimClass);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////		
void
CppSecondaryInstanceProviderProxy::createInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &cimInstance)
{
	m_pProv->updateAccessTime();
	m_pProv->createInstance(env, ns, cimInstance);
}
//////////////////////////////////////////////////////////////////////////////		
void
CppSecondaryInstanceProviderProxy::modifyInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &modifiedInstance, 
	const CIMInstance &previousInstance, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, const StringArray *propertyList, const CIMClass &theClass)
{
	m_pProv->updateAccessTime();
	m_pProv->modifyInstance(env, ns, modifiedInstance, previousInstance, includeQualifiers, propertyList, theClass);
}
//////////////////////////////////////////////////////////////////////////////		
void
CppSecondaryInstanceProviderProxy::deleteInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &cop)
{
	m_pProv->updateAccessTime();
	m_pProv->deleteInstance(env, ns, cop);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////		
CppMethodProviderProxy::CppMethodProviderProxy(CppMethodProviderIFCRef pProv) :
	MethodProviderIFC(), m_pProv(pProv)
{
}
//////////////////////////////////////////////////////////////////////////////		
CIMValue
CppMethodProviderProxy::invokeMethod(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& in,
		CIMParamValueArray& out)
{
	m_pProv->updateAccessTime();
	return m_pProv->invokeMethod(env, ns, path, methodName, in, out);
}

} // end namespace OpenWBEM

