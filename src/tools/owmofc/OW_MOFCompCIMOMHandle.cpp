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
#include "OW_MOFCompCIMOMHandle.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"

//////////////////////////////////////////////////////////////////////////////
OW_MOFCompCIMOMHandle::OW_MOFCompCIMOMHandle(OW_RepositoryIFCRef pRepos, const OW_UserInfo& aclInfo)
	: OW_CIMOMHandleIFC()
	, m_pServer(pRepos)
	, m_aclInfo(aclInfo)
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::close()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::enumClass(const OW_String& ns,
	const OW_String& className,
	OW_CIMClassResultHandlerIFC& result, 
	OW_WBEMFlags::EDeepFlag deep,
	OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin)
{
	m_pServer->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::enumClassNames(const OW_String& ns,
		const OW_String& className,
		OW_StringResultHandlerIFC& result,
		OW_WBEMFlags::EDeepFlag deep)
{
	m_pServer->enumClassNames(ns, className, result, deep, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result, OW_WBEMFlags::EDeepFlag deep,
	OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_pServer->enumInstances(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, true, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::enumInstanceNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result)
{
	return m_pServer->enumInstanceNames(ns, className, result, OW_WBEMFlags::E_DEEP, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_MOFCompCIMOMHandle::getQualifierType(const OW_String& ns,
		const OW_String& qualifierName)
{
	return m_pServer->getQualifierType(ns, qualifierName, m_aclInfo);
}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::deleteQualifierType(const OW_String& ns, const OW_String& qualName)
{
	m_pServer->deleteQualifierType(ns, qualName, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result)
{
	m_pServer->enumQualifierTypes(ns, result, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::setQualifierType(const OW_String& ns,
	const OW_CIMQualifierType& qt)
{
	m_pServer->setQualifierType(ns, qt, m_aclInfo);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_MOFCompCIMOMHandle::getClass(
	const OW_String& ns,
	const OW_String& className,
	OW_WBEMFlags::ELocalOnlyFlag localOnly,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_CIMClass cls = m_pServer->getClass(ns, className, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, m_aclInfo);
	return cls;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_MOFCompCIMOMHandle::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_WBEMFlags::ELocalOnlyFlag localOnly,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_pServer->getInstance(ns, instanceName, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_MOFCompCIMOMHandle::invokeMethod(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	const OW_String& methodName, const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams)
{
	return m_pServer->invokeMethod(ns, path, methodName, inParams, outParams,
		m_aclInfo);
}

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::modifyClass(
	const OW_String& ns,
	const OW_CIMClass& cc)
{
	m_pServer->modifyClass(ns, cc, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::createClass(const OW_String& ns,
	const OW_CIMClass& cc)
{
	m_pServer->createClass(ns, cc, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::deleteClass(const OW_String& ns, const OW_String& className)
{
	m_pServer->deleteClass(ns, className, m_aclInfo);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	const OW_StringArray* propertyList)
{
	m_pServer->modifyInstance(ns, modifiedInstance, includeQualifiers,
		propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_MOFCompCIMOMHandle::createInstance(const OW_String& ns,
	const OW_CIMInstance& ci)
{
	return m_pServer->createInstance(ns, ci, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& path)
{
	m_pServer->deleteInstance(ns, path, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& cv)
{
	m_pServer->setProperty(ns, name, propertyName, cv, m_aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_MOFCompCIMOMHandle::getProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName)
{
	return m_pServer->getProperty(ns, name, propertyName, m_aclInfo);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::associatorNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole)
{
	m_pServer->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::associators(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_pServer->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::associatorsClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_pServer->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::referenceNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role)
{
	m_pServer->referenceNames(ns, path, result, resultClass, role, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::references(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_pServer->references(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::referencesClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_pServer->referencesClasses(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_aclInfo);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::execQuery(
	const OW_String& ns,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& query,
	const OW_String& queryLanguage)
{
	m_pServer->execQuery(ns, result, query, queryLanguage, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_MOFCompCIMOMHandle::getServerFeatures()
{
	OW_CIMFeatures cf;
	cf.protocolVersion = "1.0";
	cf.cimom = "/cimom";
	cf.cimProduct = OW_CIMFeatures::SERVER;
	cf.extURL = "http://www.dmtf.org/cim/mapping/http/v1.0";
	cf.supportedGroups.push_back("basic-read");
	cf.supportedGroups.push_back("basic-write");
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	cf.supportedGroups.push_back("schema-manipulation");
#endif
	cf.supportedGroups.push_back("instance-manipulation");
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	cf.supportedGroups.push_back("qualifier-declaration");
#endif
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	cf.supportedGroups.push_back("association-traversal");
#endif
	cf.supportedQueryLanguages.clear();
	cf.supportsBatch = true;
	cf.validation = OW_String();
	return cf;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MOFCompCIMOMHandle::exportIndication(const OW_CIMInstance&,
	const OW_String&)
{
}


