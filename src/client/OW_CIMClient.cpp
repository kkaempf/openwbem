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

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMClient.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProtocolIFC.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
///////////////////////////////////////////////////////////////////////////////
CIMClient::CIMClient(const String& url, const String& ns,
	const ClientAuthCBIFCRef& authCB)
{
	//URL owurl(url);
	//CIMProtocolIFCRef client(new HTTPClient(url));
	/**********************************************************************
	 * Assign our callback to the HTTP Client.
	 **********************************************************************/
	//client->setLoginCallBack(authCB);
	/**********************************************************************
	 * Here we create a CIMXMLCIMOMHandle and have it use the
	 * HTTPClient we've created.  CIMXMLCIMOMHandle takes
	 * a Reference<CIMProtocol> it it's constructor, so
	 * we have to make a Reference out of our HTTP Client first.
	 * By doing this, we don't have to worry about deleting our
	 * HTTPClient.  Reference will delete it for us when the
	 * last copy goes out of scope (reference count goes to zero).
	 **********************************************************************/
	//if (owurl.scheme.startsWith(URL::OWBINARY) 
	//	|| owurl.namespaceName.equals(URL::OWBINARY)) // the /owbinary is deprecated and may be removed!
	//{
	//	m_ch = new BinaryCIMOMHandle(client);
	//}
	//else
	//{
	//	m_ch = new CIMXMLCIMOMHandle(client);
	//}
	
	m_ch = ClientCIMOMHandle::createFromURL(url);
	m_ch->getWBEMProtocolHandler()->setLoginCallBack(authCB);

	m_namespace = ns;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void CIMClient::createNameSpace(const String& ns)
{
	try
	{
		CIMNameSpaceUtils::createCIM_Namespace(*m_ch,ns);
	}
	catch (const CIMException& e)
	{
		// server doesn't support CIM_Namespace, try __Namespace
		CIMNameSpaceUtils::create__Namespace(*m_ch,ns);
	}
}
///////////////////////////////////////////////////////////////////////////////
void CIMClient::deleteNameSpace(const String& ns)
{
	try
	{
		CIMNameSpaceUtils::deleteCIM_Namespace(*m_ch,ns);
	}
	catch (const CIMException& e)
	{
		// server doesn't support CIM_Namespace, try __Namespace
		CIMNameSpaceUtils::delete__Namespace(*m_ch,ns);
	}
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
StringArray 
CIMClient::enumNameSpaceE(EDeepFlag deep)
{
	// TODO: try using CIM_Namespace first
	return CIMNameSpaceUtils::enum__Namespace(*m_ch, m_namespace, deep);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::enumNameSpace(StringResultHandlerIFC& result, 
	EDeepFlag deep)
{
	// TODO: try using CIM_Namespace first
	CIMNameSpaceUtils::enum__Namespace(*m_ch, m_namespace, result, deep);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::enumClass(const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin)
{
	m_ch->enumClass(m_namespace, className, result, deep, localOnly, 
		includeQualifiers, includeClassOrigin);
}
///////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration 
CIMClient::enumClassE(const String& className,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin)
{
	return m_ch->enumClassE(m_namespace, className, deep, localOnly, includeQualifiers, 
		includeClassOrigin);
}
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::enumClassNames(
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep)
{
	m_ch->enumClassNames(m_namespace, className, result, deep);
}
///////////////////////////////////////////////////////////////////////////////
StringEnumeration 
	CIMClient::enumClassNamesE(
	const String& className,
	EDeepFlag deep)
{
	return m_ch->enumClassNamesE(m_namespace, className, deep);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::enumInstances(
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	m_ch->enumInstances(m_namespace, className, result, deep, localOnly,
		includeQualifiers, includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration 
CIMClient::enumInstancesE(
	const String& className,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	return m_ch->enumInstancesE(m_namespace, className, deep, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::enumInstanceNames(
	const String& className,
	CIMObjectPathResultHandlerIFC& result)
{
	m_ch->enumInstanceNames(m_namespace, className, result);
}
///////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration 
CIMClient::enumInstanceNamesE(
	const String& className)
{
	return m_ch->enumInstanceNamesE(m_namespace, className);
}
///////////////////////////////////////////////////////////////////////////////
CIMClass 
	CIMClient::getClass(
	const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	return m_ch->getClass(m_namespace, className, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
CIMInstance 
	CIMClient::getInstance(
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList) 
{
	return m_ch->getInstance(m_namespace, instanceName, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
CIMValue 
	CIMClient::invokeMethod(
	const CIMObjectPath& path,
	const String& methodName,
	const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams)
{
	return m_ch->invokeMethod(m_namespace, path, methodName, inParams, outParams);
}
///////////////////////////////////////////////////////////////////////////////
CIMQualifierType 
	CIMClient::getQualifierType(const String& qualifierName)
{
	return m_ch->getQualifierType(m_namespace, qualifierName);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::setQualifierType(const CIMQualifierType& qualifierType)
{
	m_ch->setQualifierType(m_namespace, qualifierType);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::deleteQualifierType(const String& qualName)
{
	m_ch->deleteQualifierType(m_namespace, qualName);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::enumQualifierTypes(
	CIMQualifierTypeResultHandlerIFC& result)
{
	m_ch->enumQualifierTypes(m_namespace, result);
}
///////////////////////////////////////////////////////////////////////////////
CIMQualifierTypeEnumeration 
CIMClient::enumQualifierTypesE()
{
	return m_ch->enumQualifierTypesE(m_namespace);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::modifyClass(const CIMClass& cimClass) 
{
	m_ch->modifyClass(m_namespace, cimClass);
}
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::createClass(const CIMClass& cimClass) 
{
	m_ch->createClass(m_namespace, cimClass);
}
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::deleteClass(const String& className)
{
	m_ch->deleteClass(m_namespace, className);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::modifyInstance(const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	StringArray* propertyList)
{
	m_ch->modifyInstance(m_namespace, modifiedInstance, includeQualifiers,
		propertyList);
}
///////////////////////////////////////////////////////////////////////////////
CIMObjectPath 
	CIMClient::createInstance(const CIMInstance& instance) 
{
	return m_ch->createInstance(m_namespace, instance);
}
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::deleteInstance(const CIMObjectPath& path)
{
	m_ch->deleteInstance(m_namespace, path);
}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::setProperty(
	const CIMObjectPath& instanceName,
	const String& propertyName,
	const CIMValue& newValue) 
{
	m_ch->setProperty(m_namespace, instanceName, propertyName, newValue);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
///////////////////////////////////////////////////////////////////////////////
CIMValue 
	CIMClient::getProperty(
	const CIMObjectPath& instanceName,
	const String& propertyName)
{
	return m_ch->getProperty(m_namespace, instanceName, propertyName);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::associatorNames(
	const CIMObjectPath& objectName,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole)
{
	m_ch->associatorNames(m_namespace, objectName, result,
		assocClass, resultClass, role, resultRole);
}
///////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration 
	CIMClient::associatorNamesE(
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole)
{
	return m_ch->associatorNamesE(m_namespace, objectName, 
		assocClass, resultClass, role, resultRole);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::associators(
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	m_ch->associators(m_namespace, path, result,
		assocClass, resultClass,
		role, resultRole, includeQualifiers, includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration 
CIMClient::associatorsE(
	const CIMObjectPath& path,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	return m_ch->associatorsE(m_namespace, path, assocClass, 
		resultClass, role, resultRole, includeQualifiers, 
		includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::associatorsClasses(
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	m_ch->associatorsClasses(m_namespace, path, result, assocClass, 
		resultClass, role, resultRole, includeQualifiers, includeClassOrigin, 
		propertyList);
}
///////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration 
CIMClient::associatorsClassesE(
	const CIMObjectPath& path,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	return m_ch->associatorsClassesE(m_namespace, path, assocClass, 
		resultClass, role, resultRole, includeQualifiers, includeClassOrigin, 
		propertyList);
}
///////////////////////////////////////////////////////////////////////////////
void 
	CIMClient::referenceNames(
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass,
	const String& role)
{
	m_ch->referenceNames(m_namespace, path, result, resultClass, role);
}
///////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration 
	CIMClient::referenceNamesE(
	const CIMObjectPath& path,
	const String& resultClass,
	const String& role)
{
	return m_ch->referenceNamesE(m_namespace, path, resultClass, role);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::references(
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass,
	const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList) 
{
	m_ch->references(m_namespace, path, result, resultClass, 
		role, includeQualifiers, includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration 
CIMClient::referencesE(
	const CIMObjectPath& path,
	const String& resultClass,
	const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	return m_ch->referencesE(m_namespace, path, resultClass, role, 
		includeQualifiers, includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::referencesClasses(
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass,
	const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList) 
{
	m_ch->referencesClasses(m_namespace, path, result, resultClass, 
		role, includeQualifiers, includeClassOrigin, propertyList);
}
///////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration 
CIMClient::referencesClassesE(
	const CIMObjectPath& path,
	const String& resultClass,
	const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	return m_ch->referencesClassesE(m_namespace, path, resultClass, role, 
		includeQualifiers, includeClassOrigin, propertyList);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
///////////////////////////////////////////////////////////////////////////////
void 
CIMClient::execQuery(
	CIMInstanceResultHandlerIFC& result,
	const String& query,
	const String& queryLanguage) 
{
	m_ch->execQuery(m_namespace, result, query, queryLanguage);
}
///////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration 
CIMClient::execQueryE(
	const String& query,
	const String& queryLanguage)
{
	return m_ch->execQueryE(m_namespace, query, queryLanguage);
}

} // end namespace OpenWBEM

