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
#include "OW_CppProxyProvider.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"

using namespace OW_WBEMFlags;

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////		
OW_CppAssociatorProviderProxy::OW_CppAssociatorProviderProxy(
	OW_CppAssociatorProviderIFCRef pProv) :
	OW_AssociatorProviderIFC(), m_pProv(pProv)
{
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppAssociatorProviderProxy::associators(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		const OW_String& assocClass,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList)
{
	m_pProv->updateAccessTime();
	m_pProv->associators(env, result, ns, objectName, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppAssociatorProviderProxy::associatorNames(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		const OW_String& assocClass,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole)
{
	m_pProv->updateAccessTime();
	m_pProv->associatorNames(env, result, ns, objectName, assocClass, resultClass,
			role, resultRole);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppAssociatorProviderProxy::references(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		const OW_String& resultClass,
		const OW_String& role,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList)
{
	m_pProv->updateAccessTime();
	m_pProv->references(env, result, ns, objectName, resultClass, role,
			includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppAssociatorProviderProxy::referenceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		const OW_String& resultClass,
		const OW_String& role)
{
	m_pProv->updateAccessTime();
	m_pProv->referenceNames(env, result, ns, objectName, resultClass, role);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

//////////////////////////////////////////////////////////////////////////////		
OW_CppInstanceProviderProxy::OW_CppInstanceProviderProxy(
		OW_CppInstanceProviderIFCRef pProv) :
	OW_InstanceProviderIFC(), m_pProv(pProv)
{
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppInstanceProviderProxy::enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	return m_pProv->enumInstanceNames(env, ns, className, result, cimClass);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppInstanceProviderProxy::enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		ELocalOnlyFlag localOnly, 
		EDeepFlag deep, 
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList,
		const OW_CIMClass& requestedClass,
		const OW_CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	m_pProv->enumInstances(env, ns, className, result, localOnly, deep, 
		includeQualifiers, includeClassOrigin, propertyList, requestedClass, 
		cimClass);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMInstance
OW_CppInstanceProviderProxy::getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList, 
		const OW_CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	return m_pProv->getInstance(env, ns, instanceName, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList, cimClass);
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPath
OW_CppInstanceProviderProxy::createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& cimInstance)
{
	m_pProv->updateAccessTime();
	return m_pProv->createInstance(env, ns, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppInstanceProviderProxy::modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		const OW_CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_CIMClass& theClass)
{
	m_pProv->updateAccessTime();
	m_pProv->modifyInstance(env, ns, modifiedInstance, previousInstance, 
		includeQualifiers, propertyList, theClass);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppInstanceProviderProxy::deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
{
	m_pProv->updateAccessTime();
	m_pProv->deleteInstance(env, ns, cop);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////		
OW_CppMethodProviderProxy::OW_CppMethodProviderProxy(OW_CppMethodProviderIFCRef pProv) :
	OW_MethodProviderIFC(), m_pProv(pProv)
{
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMValue
OW_CppMethodProviderProxy::invokeMethod(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& methodName,
		const OW_CIMParamValueArray& in,
		OW_CIMParamValueArray& out)
{
	m_pProv->updateAccessTime();
	return m_pProv->invokeMethod(env, ns, path, methodName, in, out);
}


