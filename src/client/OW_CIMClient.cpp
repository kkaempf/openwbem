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
#include "OW_CIMClient.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_CIMException.hpp"

///////////////////////////////////////////////////////////////////////////////
OW_CIMClient::OW_CIMClient(const OW_String& url, const OW_String& ns,
	const OW_ClientAuthCBIFCRef& authCB)
{

	OW_URL owurl(url);
	OW_CIMProtocolIFCRef client(new OW_HTTPClient(url));


	/**********************************************************************
	 * Assign our callback to the HTTP Client.
	 **********************************************************************/

	client->setLoginCallBack(authCB);

	/**********************************************************************
	 * Here we create a OW_CIMXMLCIMOMHandle and have it use the
	 * OW_HTTPClient we've created.  OW_CIMXMLCIMOMHandle takes
	 * a OW_Reference<OW_CIMProtocol> it it's constructor, so
	 * we have to make a OW_Reference out of our HTTP Client first.
	 * By doing this, we don't have to worry about deleting our
	 * OW_HTTPClient.  OW_Reference will delete it for us when the
	 * last copy goes out of scope (reference count goes to zero).
	 **********************************************************************/

	if (owurl.path.equalsIgnoreCase("/owbinary"))
	{
		m_ch = new OW_BinaryCIMOMHandle(client);
	}
	else
	{
		m_ch = new OW_CIMXMLCIMOMHandle(client);
	}

	m_namespace = ns;

}

///////////////////////////////////////////////////////////////////////////////
void OW_CIMClient::createNameSpace(const OW_String& ns)
{
    try
    {
        OW_CIMNameSpaceUtils::createCIM_Namespace(m_ch,ns);
    }
    catch (const OW_CIMException& e)
    {
        // server doesn't support CIM_Namespace, try __Namespace
        OW_CIMNameSpaceUtils::create__Namespace(m_ch,ns);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OW_CIMClient::deleteNameSpace(const OW_String& ns)
{
	try
    {
        OW_CIMNameSpaceUtils::deleteCIM_Namespace(m_ch,ns);
    }
    catch (const OW_CIMException& e)
    {
        // server doesn't support CIM_Namespace, try __Namespace
        OW_CIMNameSpaceUtils::delete__Namespace(m_ch,ns);
    }
}

///////////////////////////////////////////////////////////////////////////////
OW_StringArray 
OW_CIMClient::enumNameSpaceE(OW_Bool deep)
{
	return OW_CIMNameSpaceUtils::enum__Namespace(m_ch, m_namespace, deep);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::enumNameSpace(OW_StringResultHandlerIFC& result, 
	OW_Bool deep)
{
	OW_CIMNameSpaceUtils::enum__Namespace(m_ch, m_namespace, result, deep);
}

///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::deleteInstance(const OW_CIMObjectPath& path)
{
	m_ch->deleteInstance(m_namespace, path);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::enumClass(const OW_String& className,
	OW_CIMClassResultHandlerIFC& result,
	OW_Bool deep, OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin)
{
	m_ch->enumClass(m_namespace, className, result, deep, localOnly, 
		includeQualifiers, includeClassOrigin);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration 
OW_CIMClient::enumClassE(const OW_String& className,
	OW_Bool deep,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin)
{
	return m_ch->enumClassE(m_namespace, className, deep, localOnly, includeQualifiers, 
		includeClassOrigin);
}

///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::enumClassNames(
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep)
{
	m_ch->enumClassNames(m_namespace, className, result, deep);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration 
	OW_CIMClient::enumClassNamesE(
	const OW_String& className,
	OW_Bool deep)
{
	return m_ch->enumClassNamesE(m_namespace, className, deep);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::enumInstances(
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result,
	OW_Bool deep,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_ch->enumInstances(m_namespace, className, result, deep, localOnly,
		includeQualifiers, includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration 
OW_CIMClient::enumInstancesE(
	const OW_String& className,
	OW_Bool deep,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->enumInstancesE(m_namespace, className, deep, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::enumInstanceNames(
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result)
{
	m_ch->enumInstanceNames(m_namespace, className, result);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration 
OW_CIMClient::enumInstanceNamesE(
	const OW_String& className)
{
	return m_ch->enumInstanceNamesE(m_namespace, className);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMClass 
	OW_CIMClient::getClass(
	const OW_String& className,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->getClass(m_namespace, className, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMInstance 
	OW_CIMClient::getInstance(
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList) 
{
	return m_ch->getInstance(m_namespace, instanceName, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMValue 
	OW_CIMClient::invokeMethod(
	const OW_CIMObjectPath& path,
	const OW_String& methodName,
	const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams)
{
	return m_ch->invokeMethod(m_namespace, path, methodName, inParams, outParams);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType 
	OW_CIMClient::getQualifierType(const OW_String& qualifierName)
{
	return m_ch->getQualifierType(m_namespace, qualifierName);
}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::setQualifierType(const OW_CIMQualifierType& qualifierType)
{
	m_ch->setQualifierType(m_namespace, qualifierType);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::deleteQualifierType(const OW_String& qualName)
{
	m_ch->deleteQualifierType(m_namespace, qualName);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::enumQualifierTypes(
	OW_CIMQualifierTypeResultHandlerIFC& result)
{
	m_ch->enumQualifierTypes(m_namespace, result);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration 
OW_CIMClient::enumQualifierTypesE()
{
	return m_ch->enumQualifierTypesE(m_namespace);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::modifyClass(const OW_CIMClass& cimClass) 
{
	m_ch->modifyClass(m_namespace, cimClass);
}

///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::createClass(const OW_CIMClass& cimClass) 
{
	m_ch->createClass(m_namespace, cimClass);
}

///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::deleteClass(const OW_String& className)
{
	m_ch->deleteClass(m_namespace, className);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::modifyInstance(const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	OW_StringArray* propertyList)
{
	m_ch->modifyInstance(m_namespace, modifiedInstance, includeQualifiers,
		propertyList);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath 
	OW_CIMClient::createInstance(const OW_CIMInstance& instance) 
{
	return m_ch->createInstance(m_namespace, instance);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMValue 
	OW_CIMClient::getProperty(
	const OW_CIMObjectPath& instanceName,
	const OW_String& propertyName)
{
	return m_ch->getProperty(m_namespace, instanceName, propertyName);
}

///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::setProperty(
	const OW_CIMObjectPath& instanceName,
	const OW_String& propertyName,
	const OW_CIMValue& newValue) 
{
	m_ch->setProperty(m_namespace, instanceName, propertyName, newValue);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::associatorNames(
	const OW_CIMObjectPath& objectName,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole)
{
	m_ch->associatorNames(m_namespace, objectName, result,
		assocClass, resultClass, role, resultRole);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration 
	OW_CIMClient::associatorNamesE(
	const OW_CIMObjectPath& objectName,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole)
{
	return m_ch->associatorNamesE(m_namespace, objectName, 
		assocClass, resultClass, role, resultRole);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::associators(
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_ch->associators(m_namespace, path, result,
		assocClass, resultClass,
		role, resultRole, includeQualifiers, includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration 
OW_CIMClient::associatorsE(
	const OW_CIMObjectPath& path,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->associatorsE(m_namespace, path, assocClass, 
		resultClass, role, resultRole, includeQualifiers, 
		includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::associatorsClasses(
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_ch->associatorsClasses(m_namespace, path, result, assocClass, 
		resultClass, role, resultRole, includeQualifiers, includeClassOrigin, 
		propertyList);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration 
OW_CIMClient::associatorsClassesE(
	const OW_CIMObjectPath& path,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->associatorsClassesE(m_namespace, path, assocClass, 
		resultClass, role, resultRole, includeQualifiers, includeClassOrigin, 
		propertyList);
}

///////////////////////////////////////////////////////////////////////////////
void 
	OW_CIMClient::referenceNames(
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass,
	const OW_String& role)
{
	m_ch->referenceNames(m_namespace, path, result, resultClass, role);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration 
	OW_CIMClient::referenceNamesE(
	const OW_CIMObjectPath& path,
	const OW_String& resultClass,
	const OW_String& role)
{
	return m_ch->referenceNamesE(m_namespace, path, resultClass, role);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::references(
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& resultClass,
	const OW_String& role,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList) 
{
	m_ch->references(m_namespace, path, result, resultClass, 
		role, includeQualifiers, includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration 
OW_CIMClient::referencesE(
	const OW_CIMObjectPath& path,
	const OW_String& resultClass,
	const OW_String& role,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->referencesE(m_namespace, path, resultClass, role, 
		includeQualifiers, includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::referencesClasses(
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& resultClass,
	const OW_String& role,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList) 
{
	m_ch->referencesClasses(m_namespace, path, result, resultClass, 
		role, includeQualifiers, includeClassOrigin, propertyList);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration 
OW_CIMClient::referencesClassesE(
	const OW_CIMObjectPath& path,
	const OW_String& resultClass,
	const OW_String& role,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->referencesClassesE(m_namespace, path, resultClass, role, 
		includeQualifiers, includeClassOrigin, propertyList);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

///////////////////////////////////////////////////////////////////////////////
void 
OW_CIMClient::execQuery(
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& query,
	const OW_String& queryLanguage) 
{
	m_ch->execQuery(m_namespace, result, query, queryLanguage);
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration 
OW_CIMClient::execQueryE(
	const OW_String& query,
	const OW_String& queryLanguage)
{
	return m_ch->execQueryE(m_namespace, query, queryLanguage);
}




