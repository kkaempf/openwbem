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
OW_CIMInstanceEnumeration
OW_CppAssociatorProviderProxy::associators(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath assocName,
		OW_CIMObjectPath objectName,
		OW_String resultClass,
		OW_String role,
		OW_String resultRole,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	return m_pProv->associators(env, assocName, objectName, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPathEnumeration
OW_CppAssociatorProviderProxy::associatorNames(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath assocName,
		OW_CIMObjectPath objectName,
		OW_String resultClass,
		OW_String role,
		OW_String resultRole)
{
	return m_pProv->associatorNames(env, assocName, objectName, resultClass,
			role, resultRole);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMInstanceEnumeration
OW_CppAssociatorProviderProxy::references(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath assocName,
		OW_CIMObjectPath objectName,
		OW_String role,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	return m_pProv->references(env, assocName, objectName, role,
			includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPathEnumeration
OW_CppAssociatorProviderProxy::referenceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath assocName,
		OW_CIMObjectPath objectName,
		OW_String role)
{
	return m_pProv->referenceNames(env, assocName, objectName, role);
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
		OW_CIMObjectPath cop)
{
	m_pProv->deleteInstance(env, cop);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPathEnumeration
OW_CppInstanceProviderProxy::enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_Bool deep,
		OW_CIMClass cimClass)
{
	return m_pProv->enumInstanceNames(env, cop, deep, cimClass);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMInstanceEnumeration
OW_CppInstanceProviderProxy::enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_Bool deep,
		OW_CIMClass cimClass,
		OW_Bool localOnly)
{
	return m_pProv->enumInstances(env, cop, deep, cimClass, localOnly);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMInstance
OW_CppInstanceProviderProxy::getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_CIMClass cimClass,
		OW_Bool localOnly)
{
	return m_pProv->getInstance(env, cop, cimClass, localOnly);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPath
OW_CppInstanceProviderProxy::createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_CIMInstance cimInstance)
{
	return m_pProv->createInstance(env, cop, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppInstanceProviderProxy::setInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_CIMInstance cimInstance)
{
	m_pProv->setInstance(env, cop, cimInstance);
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
		const OW_CIMObjectPath& cop,
		const OW_String& methodName,
		const OW_CIMValueArray& in,
		OW_CIMValueArray& out)
{
	return m_pProv->invokeMethod(env, cop, methodName, in, out);
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
		OW_CIMObjectPath cop,
		OW_String originClass,
		OW_String propertyName)
{
	return m_pProv->getPropertyValue(env, cop, originClass, propertyName);

}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CppPropertyProviderProxy::setPropertyValue(
		const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMObjectPath cop,
		OW_String originClass,
		OW_String propertyName,
		OW_CIMValue val)
{
	m_pProv->setPropertyValue(env, cop, originClass, propertyName, val);
}

