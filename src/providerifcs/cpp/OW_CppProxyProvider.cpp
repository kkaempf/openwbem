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
#include "OW_CppProxyProvider.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"

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
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole,
		const OW_Bool& includeQualifiers,
		const OW_Bool& includeClassOrigin,
		const OW_StringArray* propertyList)
{
	m_pProv->updateAccessTime();
	m_pProv->associators(env, assocName, objectName, result, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppAssociatorProviderProxy::associatorNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole)
{
	m_pProv->updateAccessTime();
	m_pProv->associatorNames(env, assocName, objectName, result, resultClass,
			role, resultRole);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppAssociatorProviderProxy::references(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& role,
		const OW_Bool& includeQualifiers,
		const OW_Bool& includeClassOrigin,
		const OW_StringArray* propertyList)
{
	m_pProv->updateAccessTime();
	m_pProv->references(env, assocName, objectName, result, role,
			includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppAssociatorProviderProxy::referenceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& role)
{
	m_pProv->updateAccessTime();
	m_pProv->referenceNames(env, assocName, objectName, result, role);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CppInstanceProviderProxy::OW_CppInstanceProviderProxy(
		OW_CppInstanceProviderIFCRef pProv) :
	OW_InstanceProviderIFC(), m_pProv(pProv)
{
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

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppInstanceProviderProxy::enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_Bool& deep,
		const OW_CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	return m_pProv->enumInstanceNames(env, ns, className, result, deep, cimClass);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppInstanceProviderProxy::enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_Bool& deep,
		const OW_CIMClass& cimClass,
		const OW_Bool& localOnly)
{
	m_pProv->updateAccessTime();
	m_pProv->enumInstances(env, ns, className, result, deep, cimClass, localOnly);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMInstance
OW_CppInstanceProviderProxy::getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		const OW_CIMClass& cimClass,
		const OW_Bool& localOnly)
{
	m_pProv->updateAccessTime();
	return m_pProv->getInstance(env, ns, instanceName, cimClass, localOnly);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPath
OW_CppInstanceProviderProxy::createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMInstance& cimInstance)
{
	m_pProv->updateAccessTime();
	return m_pProv->createInstance(env, cop, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppInstanceProviderProxy::modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMInstance& cimInstance)
{
	m_pProv->updateAccessTime();
	m_pProv->modifyInstance(env, cop, cimInstance);
}

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

//////////////////////////////////////////////////////////////////////////////		
OW_CppPropertyProviderProxy::OW_CppPropertyProviderProxy(
		OW_CppPropertyProviderIFCRef pProv) :
	OW_PropertyProviderIFC(), m_pProv(pProv)
{
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMValue
OW_CppPropertyProviderProxy::getPropertyValue(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_String& originClass,
		const OW_String& propertyName)
{
	m_pProv->updateAccessTime();
	return m_pProv->getPropertyValue(env, cop, originClass, propertyName);

}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppPropertyProviderProxy::setPropertyValue(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_String& originClass,
		const OW_String& propertyName,
		const OW_CIMValue& val)
{
	m_pProv->updateAccessTime();
	m_pProv->setPropertyValue(env, cop, originClass, propertyName, val);
}

