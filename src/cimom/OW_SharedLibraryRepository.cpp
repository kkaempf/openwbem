/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
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
#include "OW_SharedLibraryRepository.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"

using namespace OW_WBEMFlags;

OW_SharedLibraryRepository::OW_SharedLibraryRepository(OW_SharedLibraryRepositoryIFCRef ref)
	: OW_RepositoryIFC()
	, m_ref(ref) {}

OW_SharedLibraryRepository::~OW_SharedLibraryRepository()
{}

void OW_SharedLibraryRepository::close() { m_ref->close(); }

void OW_SharedLibraryRepository::open(const OW_String &path) { m_ref->open(path); }

OW_ServiceEnvironmentIFCRef OW_SharedLibraryRepository::getEnvironment() const
{
	return m_ref->getEnvironment();
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
void OW_SharedLibraryRepository::createNameSpace(const OW_String& ns,
	const OW_UserInfo &aclInfo)
{
	m_ref->createNameSpace(ns, aclInfo);
}

void OW_SharedLibraryRepository::deleteNameSpace(const OW_String& ns,
	const OW_UserInfo &aclInfo)
{
	m_ref->deleteNameSpace(ns, aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

void OW_SharedLibraryRepository::enumNameSpace(OW_StringResultHandlerIFC& result,
	const OW_UserInfo &aclInfo)
{
	return m_ref->enumNameSpace(result, aclInfo);
}

OW_CIMQualifierType OW_SharedLibraryRepository::getQualifierType(
	const OW_String& ns,
	const OW_String& qualifierName, const OW_UserInfo &aclInfo)
{
	return m_ref->getQualifierType(ns, qualifierName, aclInfo);
}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
void OW_SharedLibraryRepository::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result, const OW_UserInfo &aclInfo)
{
	return m_ref->enumQualifierTypes(ns, result, aclInfo);
}

void OW_SharedLibraryRepository::deleteQualifierType(const OW_String& ns, const OW_String& qualName,
	const OW_UserInfo &aclInfo)
{
	m_ref->deleteQualifierType(ns, qualName, aclInfo);
}

void OW_SharedLibraryRepository::setQualifierType(const OW_String& ns,
	const OW_CIMQualifierType &qt, const OW_UserInfo &aclInfo)
{
	m_ref->setQualifierType(ns, qt, aclInfo);
}

#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

OW_CIMClass OW_SharedLibraryRepository::getClass(
	const OW_String& ns,
	const OW_String& className,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const OW_StringArray *propertyList,
	const OW_UserInfo &aclInfo)
{
	return m_ref->getClass(ns, className, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
OW_CIMObjectPath OW_SharedLibraryRepository::createInstance(const OW_String& ns, const OW_CIMInstance &ci,
	const OW_UserInfo &aclInfo)
{
	return m_ref->createInstance(ns, ci, aclInfo);
}

OW_CIMInstance OW_SharedLibraryRepository::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const OW_StringArray* propertyList,
	const OW_UserInfo &aclInfo)
{
	return m_ref->modifyInstance(ns, modifiedInstance, includeQualifiers,
		propertyList, aclInfo);
}

void OW_SharedLibraryRepository::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath &name,
	const OW_String &propertyName, const OW_CIMValue &cv,
	const OW_UserInfo &aclInfo)
{
	m_ref->setProperty(ns, name, propertyName, cv, aclInfo);
}

OW_CIMInstance OW_SharedLibraryRepository::deleteInstance(const OW_String& ns, const OW_CIMObjectPath &cop,
	const OW_UserInfo &aclInfo)
{
	return m_ref->deleteInstance(ns, cop, aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION


void OW_SharedLibraryRepository::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep, 
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, 
	EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, 
	EEnumSubclassesFlag enumSubclasses, 
	const OW_UserInfo &aclInfo)
{
	return m_ref->enumInstances(ns, className, result, deep, localOnly,includeQualifiers,
		includeClassOrigin, propertyList, enumSubclasses, aclInfo);
}

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
void OW_SharedLibraryRepository::createClass(const OW_String& ns,
	const OW_CIMClass &cimClass, const OW_UserInfo &aclInfo)
{
	m_ref->createClass(ns, cimClass, aclInfo);
}

OW_CIMClass OW_SharedLibraryRepository::modifyClass(const OW_String &ns,
	const OW_CIMClass &cc, const OW_UserInfo &aclInfo)
{
	return m_ref->modifyClass(ns, cc, aclInfo);
}

OW_CIMClass OW_SharedLibraryRepository::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_UserInfo &aclInfo)
{
	return m_ref->deleteClass(ns, className, aclInfo);
}
#endif

void OW_SharedLibraryRepository::enumClasses(const OW_String& ns,
	const OW_String& className,
	OW_CIMClassResultHandlerIFC& result,
	EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const OW_UserInfo &aclInfo)
{
	return m_ref->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, aclInfo);
}

void OW_SharedLibraryRepository::enumClassNames(
	const OW_String& ns,
	const OW_String& className,
	OW_StringResultHandlerIFC& result,
	EDeepFlag deep, const OW_UserInfo &aclInfo)
{
	m_ref->enumClassNames(ns, className, result, deep, aclInfo);
}

OW_CIMValue OW_SharedLibraryRepository::invokeMethod(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	const OW_String &methodName, const OW_CIMParamValueArray &inParams,
	OW_CIMParamValueArray &outParams, const OW_UserInfo &aclInfo)
{
	return m_ref->invokeMethod(ns, path, methodName, inParams,
		outParams, aclInfo);
}

void OW_SharedLibraryRepository::enumInstanceNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep, const OW_UserInfo &aclInfo)
{
	return m_ref->enumInstanceNames(ns, className, result, deep, aclInfo);
}

OW_CIMInstance OW_SharedLibraryRepository::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const OW_StringArray *propertyList,
	const OW_UserInfo &aclInfo)
{
	return m_ref->getInstance(ns, instanceName, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void OW_SharedLibraryRepository::references(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &resultClass, const OW_String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, const OW_UserInfo &aclInfo)
{
	m_ref->references(ns, path, result, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}

void OW_SharedLibraryRepository::referencesClasses(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String &resultClass, const OW_String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, const OW_UserInfo &aclInfo)
{
	m_ref->referencesClasses(ns, path, result, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}
#endif

OW_CIMValue OW_SharedLibraryRepository::getProperty(
	const OW_String& ns,
	const OW_CIMObjectPath &name,
	const OW_String &propertyName, const OW_UserInfo &aclInfo)
{
	return m_ref->getProperty(ns, name, propertyName, aclInfo);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void OW_SharedLibraryRepository::associatorNames(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String &assocClass,
	const OW_String &resultClass, const OW_String &role,
	const OW_String &resultRole, const OW_UserInfo &aclInfo)
{
	m_ref->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, aclInfo);
}

void OW_SharedLibraryRepository::associators(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &assocClass, const OW_String &resultClass,
	const OW_String &role, const OW_String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, const OW_UserInfo &aclInfo)
{
	m_ref->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		aclInfo);
}

void OW_SharedLibraryRepository::associatorsClasses(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String &assocClass, const OW_String &resultClass,
	const OW_String &role, const OW_String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, const OW_UserInfo &aclInfo)
{
	m_ref->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		aclInfo);
}

void OW_SharedLibraryRepository::referenceNames(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String &resultClass,
	const OW_String &role, const OW_UserInfo &aclInfo)
{
	m_ref->referenceNames(ns, path, result, resultClass, role, aclInfo);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

void OW_SharedLibraryRepository::execQuery(
	const OW_String& ns,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &query, const OW_String &queryLanguage,
	const OW_UserInfo &aclInfo)
{
	m_ref->execQuery(ns, result, query, queryLanguage, aclInfo);
}

void OW_SharedLibraryRepository::getSchemaReadLock()
{
	m_ref->getSchemaReadLock();
}

void OW_SharedLibraryRepository::getSchemaWriteLock()
{
	m_ref->getSchemaWriteLock();
}

void OW_SharedLibraryRepository::releaseSchemaReadLock()
{
	m_ref->releaseSchemaReadLock();
}

void OW_SharedLibraryRepository::releaseSchemaWriteLock()
{
	m_ref->releaseSchemaWriteLock();
}

void OW_SharedLibraryRepository::getInstanceReadLock()
{
	m_ref->getInstanceReadLock();
}

void OW_SharedLibraryRepository::getInstanceWriteLock()
{
	m_ref->getInstanceWriteLock();
}

void OW_SharedLibraryRepository::releaseInstanceReadLock()
{
	m_ref->releaseInstanceReadLock();
}

void OW_SharedLibraryRepository::releaseInstanceWriteLock()
{
	m_ref->releaseInstanceWriteLock();
}




