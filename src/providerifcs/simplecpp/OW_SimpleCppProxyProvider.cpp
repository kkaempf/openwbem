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
#include "OW_SimpleCppProxyProvider.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"

//////////////////////////////////////////////////////////////////////////////		
OW_SimpleCppAssociatorProviderProxy::OW_SimpleCppAssociatorProviderProxy(
	OW_SimpleCppAssociatorProviderIFCRef pProv) :
	OW_AssociatorProviderIFC(), m_pProv(pProv)
{
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class associatorResultFilter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		associatorResultFilter(
			OW_CIMInstanceResultHandlerIFC& result_,
			const OW_String& resultClass_,
			const OW_String& role_,
			const OW_String& resultRole_,
			bool includeQualifiers_,
			bool includeClassOrigin_,
			const OW_StringArray* propertyList_)
		: result(result_)
		, resultClass(resultClass_)
		, role(role_)
		, resultRole(resultRole_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &i)
		{
			// TODO: check resultClass, role and resultRole
			result.handle(i.clone(false, includeQualifiers,
				includeClassOrigin,*propertyList,propertyList == 0));
		}
	private:
		OW_CIMInstanceResultHandlerIFC& result;
		const OW_String& resultClass;
		const OW_String& role;
		const OW_String& resultRole;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propertyList;
	};
}
//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppAssociatorProviderProxy::associators(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
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
	(void)assocName;
	(void)ns;
	m_pProv->updateAccessTime();
	associatorResultFilter proxyresult(result, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList);
	m_pProv->associators(env,objectName,proxyresult);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppAssociatorProviderProxy::associatorNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole)
{
	m_pProv->updateAccessTime();
	// TODO: Create a handler to do something with assocName, resultClass, role and resultRole
	(void)assocName; (void)resultClass; (void)role; (void)resultRole; (void)ns;
	m_pProv->associatorNames(env, objectName, result);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppAssociatorProviderProxy::references(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& role,
		const OW_Bool& includeQualifiers,
		const OW_Bool& includeClassOrigin,
		const OW_StringArray* propertyList)
{
	(void)assocName; (void)ns;
	m_pProv->updateAccessTime();
	associatorResultFilter proxyresult(result, OW_String(), role, OW_String(),
		includeQualifiers, includeClassOrigin, propertyList);
	m_pProv->references(env, objectName, proxyresult);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppAssociatorProviderProxy::referenceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& role)
{
	m_pProv->updateAccessTime();
	// TODO: Create a handler to do something with assocName, role
	(void)assocName; (void)role; (void)ns;
	m_pProv->referenceNames(env, objectName, result);
}

//////////////////////////////////////////////////////////////////////////////		
OW_SimpleCppInstanceProviderProxy::OW_SimpleCppInstanceProviderProxy(
		OW_SimpleCppInstanceProviderIFCRef pProv) :
	OW_InstanceProviderIFC(), m_pProv(pProv)
{
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppInstanceProviderProxy::deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
{
	(void)ns;
	m_pProv->updateAccessTime();
	m_pProv->deleteInstance(env, cop);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppInstanceProviderProxy::enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	(void)ns;
	return m_pProv->enumInstanceNames(env, className, result, cimClass);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppInstanceProviderProxy::enumInstances(
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
		const OW_CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	(void)ns;
	// TODO: Write a handler that will filter the properties on the instances based on the parameters.
	(void)localOnly; (void)deep; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)requestedClass;
	m_pProv->enumInstances(env, className, result, cimClass);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMInstance
OW_SimpleCppInstanceProviderProxy::getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly,
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, 
		const OW_CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	(void)ns;
	OW_CIMInstance ci = m_pProv->getInstance(env, instanceName, cimClass);
	return ci.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPath
OW_SimpleCppInstanceProviderProxy::createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& cimInstance)
{
	m_pProv->updateAccessTime();
	(void)ns;
	return m_pProv->createInstance(env, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppInstanceProviderProxy::modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance)
{
	m_pProv->updateAccessTime();
	(void)ns;
	m_pProv->modifyInstance(env, modifiedInstance);
}

//////////////////////////////////////////////////////////////////////////////		
OW_SimpleCppPropertyProviderProxy::OW_SimpleCppPropertyProviderProxy(
		OW_SimpleCppPropertyProviderIFCRef pProv) :
	OW_PropertyProviderIFC(), m_pProv(pProv)
{
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMValue
OW_SimpleCppPropertyProviderProxy::getPropertyValue(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop,
		const OW_String& originClass,
		const OW_String& propertyName)
{
	m_pProv->updateAccessTime();
	(void)originClass;
	(void)ns;
	return m_pProv->getPropertyValue(env, cop, propertyName);

}

//////////////////////////////////////////////////////////////////////////////		
void
OW_SimpleCppPropertyProviderProxy::setPropertyValue(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop,
		const OW_String& originClass,
		const OW_String& propertyName,
		const OW_CIMValue& val)
{
	m_pProv->updateAccessTime();
	(void)originClass;
	(void)ns;
	m_pProv->setPropertyValue(env, cop, propertyName, val);
}

