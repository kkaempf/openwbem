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

namespace OpenWBEM
{

using namespace WBEMFlags;
SharedLibraryRepository::SharedLibraryRepository(SharedLibraryRepositoryIFCRef ref)
	: RepositoryIFC()
	, m_ref(ref) {}
SharedLibraryRepository::~SharedLibraryRepository()
{}
void SharedLibraryRepository::close() { m_ref->close(); }
void SharedLibraryRepository::open(const String &path) { m_ref->open(path); }
ServiceEnvironmentIFCRef SharedLibraryRepository::getEnvironment() const
{
	return m_ref->getEnvironment();
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
void SharedLibraryRepository::createNameSpace(const String& ns,
	const UserInfo &aclInfo)
{
	m_ref->createNameSpace(ns, aclInfo);
}
void SharedLibraryRepository::deleteNameSpace(const String& ns,
	const UserInfo &aclInfo)
{
	m_ref->deleteNameSpace(ns, aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
void SharedLibraryRepository::enumNameSpace(StringResultHandlerIFC& result,
	const UserInfo &aclInfo)
{
	return m_ref->enumNameSpace(result, aclInfo);
}
CIMQualifierType SharedLibraryRepository::getQualifierType(
	const String& ns,
	const String& qualifierName, const UserInfo &aclInfo)
{
	return m_ref->getQualifierType(ns, qualifierName, aclInfo);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
void SharedLibraryRepository::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result, const UserInfo &aclInfo)
{
	return m_ref->enumQualifierTypes(ns, result, aclInfo);
}
void SharedLibraryRepository::deleteQualifierType(const String& ns, const String& qualName,
	const UserInfo &aclInfo)
{
	m_ref->deleteQualifierType(ns, qualName, aclInfo);
}
void SharedLibraryRepository::setQualifierType(const String& ns,
	const CIMQualifierType &qt, const UserInfo &aclInfo)
{
	m_ref->setQualifierType(ns, qt, aclInfo);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
CIMClass SharedLibraryRepository::getClass(
	const String& ns,
	const String& className,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList,
	const UserInfo &aclInfo)
{
	return m_ref->getClass(ns, className, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
CIMObjectPath SharedLibraryRepository::createInstance(const String& ns, const CIMInstance &ci,
	const UserInfo &aclInfo)
{
	return m_ref->createInstance(ns, ci, aclInfo);
}
CIMInstance SharedLibraryRepository::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	const UserInfo &aclInfo)
{
	return m_ref->modifyInstance(ns, modifiedInstance, includeQualifiers,
		propertyList, aclInfo);
}
void SharedLibraryRepository::setProperty(
	const String& ns,
	const CIMObjectPath &name,
	const String &propertyName, const CIMValue &cv,
	const UserInfo &aclInfo)
{
	m_ref->setProperty(ns, name, propertyName, cv, aclInfo);
}
CIMInstance SharedLibraryRepository::deleteInstance(const String& ns, const CIMObjectPath &cop,
	const UserInfo &aclInfo)
{
	return m_ref->deleteInstance(ns, cop, aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
void SharedLibraryRepository::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep, 
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, 
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, 
	EEnumSubclassesFlag enumSubclasses, 
	const UserInfo &aclInfo)
{
	return m_ref->enumInstances(ns, className, result, deep, localOnly,includeQualifiers,
		includeClassOrigin, propertyList, enumSubclasses, aclInfo);
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
void SharedLibraryRepository::createClass(const String& ns,
	const CIMClass &cimClass, const UserInfo &aclInfo)
{
	m_ref->createClass(ns, cimClass, aclInfo);
}
CIMClass SharedLibraryRepository::modifyClass(const String &ns,
	const CIMClass &cc, const UserInfo &aclInfo)
{
	return m_ref->modifyClass(ns, cc, aclInfo);
}
CIMClass SharedLibraryRepository::deleteClass(const String& ns, const String& className,
	const UserInfo &aclInfo)
{
	return m_ref->deleteClass(ns, className, aclInfo);
}
#endif
void SharedLibraryRepository::enumClasses(const String& ns,
	const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const UserInfo &aclInfo)
{
	return m_ref->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, aclInfo);
}
void SharedLibraryRepository::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, const UserInfo &aclInfo)
{
	m_ref->enumClassNames(ns, className, result, deep, aclInfo);
}
CIMValue SharedLibraryRepository::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String &methodName, const CIMParamValueArray &inParams,
	CIMParamValueArray &outParams, const UserInfo &aclInfo)
{
	return m_ref->invokeMethod(ns, path, methodName, inParams,
		outParams, aclInfo);
}
void SharedLibraryRepository::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep, const UserInfo &aclInfo)
{
	return m_ref->enumInstanceNames(ns, className, result, deep, aclInfo);
}
CIMInstance SharedLibraryRepository::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList,
	const UserInfo &aclInfo)
{
	return m_ref->getInstance(ns, instanceName, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void SharedLibraryRepository::references(
	const String& ns,
	const CIMObjectPath &path,
	CIMInstanceResultHandlerIFC& result,
	const String &resultClass, const String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const UserInfo &aclInfo)
{
	m_ref->references(ns, path, result, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}
void SharedLibraryRepository::referencesClasses(
	const String& ns,
	const CIMObjectPath &path,
	CIMClassResultHandlerIFC& result,
	const String &resultClass, const String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const UserInfo &aclInfo)
{
	m_ref->referencesClasses(ns, path, result, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}
#endif
CIMValue SharedLibraryRepository::getProperty(
	const String& ns,
	const CIMObjectPath &name,
	const String &propertyName, const UserInfo &aclInfo)
{
	return m_ref->getProperty(ns, name, propertyName, aclInfo);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void SharedLibraryRepository::associatorNames(
	const String& ns,
	const CIMObjectPath &path,
	CIMObjectPathResultHandlerIFC& result,
	const String &assocClass,
	const String &resultClass, const String &role,
	const String &resultRole, const UserInfo &aclInfo)
{
	m_ref->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, aclInfo);
}
void SharedLibraryRepository::associators(
	const String& ns,
	const CIMObjectPath &path,
	CIMInstanceResultHandlerIFC& result,
	const String &assocClass, const String &resultClass,
	const String &role, const String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const UserInfo &aclInfo)
{
	m_ref->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		aclInfo);
}
void SharedLibraryRepository::associatorsClasses(
	const String& ns,
	const CIMObjectPath &path,
	CIMClassResultHandlerIFC& result,
	const String &assocClass, const String &resultClass,
	const String &role, const String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const UserInfo &aclInfo)
{
	m_ref->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		aclInfo);
}
void SharedLibraryRepository::referenceNames(
	const String& ns,
	const CIMObjectPath &path,
	CIMObjectPathResultHandlerIFC& result,
	const String &resultClass,
	const String &role, const UserInfo &aclInfo)
{
	m_ref->referenceNames(ns, path, result, resultClass, role, aclInfo);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void SharedLibraryRepository::execQuery(
	const String& ns,
	CIMInstanceResultHandlerIFC& result,
	const String &query, const String &queryLanguage,
	const UserInfo &aclInfo)
{
	m_ref->execQuery(ns, result, query, queryLanguage, aclInfo);
}
void SharedLibraryRepository::getSchemaReadLock()
{
	m_ref->getSchemaReadLock();
}
void SharedLibraryRepository::getSchemaWriteLock()
{
	m_ref->getSchemaWriteLock();
}
void SharedLibraryRepository::releaseSchemaReadLock()
{
	m_ref->releaseSchemaReadLock();
}
void SharedLibraryRepository::releaseSchemaWriteLock()
{
	m_ref->releaseSchemaWriteLock();
}
void SharedLibraryRepository::getInstanceReadLock()
{
	m_ref->getInstanceReadLock();
}
void SharedLibraryRepository::getInstanceWriteLock()
{
	m_ref->getInstanceWriteLock();
}
void SharedLibraryRepository::releaseInstanceReadLock()
{
	m_ref->releaseInstanceReadLock();
}
void SharedLibraryRepository::releaseInstanceWriteLock()
{
	m_ref->releaseInstanceWriteLock();
}

} // end namespace OpenWBEM

