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
 * @author Dan Nuffer
 */

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
void SharedLibraryRepository::shutdown() { m_ref->shutdown(); }
void SharedLibraryRepository::open(const String &path) { m_ref->open(path); }
ServiceEnvironmentIFCRef SharedLibraryRepository::getEnvironment() const
{
	return m_ref->getEnvironment();
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
void SharedLibraryRepository::createNameSpace(const String& ns,
	OperationContext& context)
{
	m_ref->createNameSpace(ns, context);
}
void SharedLibraryRepository::deleteNameSpace(const String& ns,
	OperationContext& context)
{
	m_ref->deleteNameSpace(ns, context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
void SharedLibraryRepository::enumNameSpace(StringResultHandlerIFC& result,
	OperationContext& context)
{
	return m_ref->enumNameSpace(result, context);
}
CIMQualifierType SharedLibraryRepository::getQualifierType(
	const String& ns,
	const String& qualifierName, OperationContext& context)
{
	return m_ref->getQualifierType(ns, qualifierName, context);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
void SharedLibraryRepository::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result, OperationContext& context)
{
	return m_ref->enumQualifierTypes(ns, result, context);
}
void SharedLibraryRepository::deleteQualifierType(const String& ns, const String& qualName,
	OperationContext& context)
{
	m_ref->deleteQualifierType(ns, qualName, context);
}
void SharedLibraryRepository::setQualifierType(const String& ns,
	const CIMQualifierType &qt, OperationContext& context)
{
	m_ref->setQualifierType(ns, qt, context);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
CIMClass SharedLibraryRepository::getClass(
	const String& ns,
	const String& className,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList,
	OperationContext& context)
{
	return m_ref->getClass(ns, className, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, context);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
CIMObjectPath SharedLibraryRepository::createInstance(const String& ns, const CIMInstance &ci,
	OperationContext& context)
{
	return m_ref->createInstance(ns, ci, context);
}
CIMInstance SharedLibraryRepository::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	OperationContext& context)
{
	return m_ref->modifyInstance(ns, modifiedInstance, includeQualifiers,
		propertyList, context);
}
void SharedLibraryRepository::setProperty(
	const String& ns,
	const CIMObjectPath &name,
	const String &propertyName, const CIMValue &cv,
	OperationContext& context)
{
	m_ref->setProperty(ns, name, propertyName, cv, context);
}
CIMInstance SharedLibraryRepository::deleteInstance(const String& ns, const CIMObjectPath &cop,
	OperationContext& context)
{
	return m_ref->deleteInstance(ns, cop, context);
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
	OperationContext& context)
{
	return m_ref->enumInstances(ns, className, result, deep, localOnly,includeQualifiers,
		includeClassOrigin, propertyList, enumSubclasses, context);
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
void SharedLibraryRepository::createClass(const String& ns,
	const CIMClass &cimClass, OperationContext& context)
{
	m_ref->createClass(ns, cimClass, context);
}
CIMClass SharedLibraryRepository::modifyClass(const String &ns,
	const CIMClass &cc, OperationContext& context)
{
	return m_ref->modifyClass(ns, cc, context);
}
CIMClass SharedLibraryRepository::deleteClass(const String& ns, const String& className,
	OperationContext& context)
{
	return m_ref->deleteClass(ns, className, context);
}
#endif
void SharedLibraryRepository::enumClasses(const String& ns,
	const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, OperationContext& context)
{
	return m_ref->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, context);
}
void SharedLibraryRepository::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, OperationContext& context)
{
	m_ref->enumClassNames(ns, className, result, deep, context);
}
CIMValue SharedLibraryRepository::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String &methodName, const CIMParamValueArray &inParams,
	CIMParamValueArray &outParams, OperationContext& context)
{
	return m_ref->invokeMethod(ns, path, methodName, inParams,
		outParams, context);
}
void SharedLibraryRepository::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep, OperationContext& context)
{
	return m_ref->enumInstanceNames(ns, className, result, deep, context);
}
CIMInstance SharedLibraryRepository::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList,
	OperationContext& context)
{
	return m_ref->getInstance(ns, instanceName, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, context);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void SharedLibraryRepository::references(
	const String& ns,
	const CIMObjectPath &path,
	CIMInstanceResultHandlerIFC& result,
	const String &resultClass, const String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, OperationContext& context)
{
	m_ref->references(ns, path, result, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, context);
}
void SharedLibraryRepository::referencesClasses(
	const String& ns,
	const CIMObjectPath &path,
	CIMClassResultHandlerIFC& result,
	const String &resultClass, const String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, OperationContext& context)
{
	m_ref->referencesClasses(ns, path, result, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, context);
}
#endif
CIMValue SharedLibraryRepository::getProperty(
	const String& ns,
	const CIMObjectPath &name,
	const String &propertyName, OperationContext& context)
{
	return m_ref->getProperty(ns, name, propertyName, context);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void SharedLibraryRepository::associatorNames(
	const String& ns,
	const CIMObjectPath &path,
	CIMObjectPathResultHandlerIFC& result,
	const String &assocClass,
	const String &resultClass, const String &role,
	const String &resultRole, OperationContext& context)
{
	m_ref->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, context);
}
void SharedLibraryRepository::associators(
	const String& ns,
	const CIMObjectPath &path,
	CIMInstanceResultHandlerIFC& result,
	const String &assocClass, const String &resultClass,
	const String &role, const String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, OperationContext& context)
{
	m_ref->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		context);
}
void SharedLibraryRepository::associatorsClasses(
	const String& ns,
	const CIMObjectPath &path,
	CIMClassResultHandlerIFC& result,
	const String &assocClass, const String &resultClass,
	const String &role, const String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, OperationContext& context)
{
	m_ref->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		context);
}
void SharedLibraryRepository::referenceNames(
	const String& ns,
	const CIMObjectPath &path,
	CIMObjectPathResultHandlerIFC& result,
	const String &resultClass,
	const String &role, OperationContext& context)
{
	m_ref->referenceNames(ns, path, result, resultClass, role, context);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void SharedLibraryRepository::execQuery(
	const String& ns,
	CIMInstanceResultHandlerIFC& result,
	const String &query, const String &queryLanguage,
	OperationContext& context)
{
	m_ref->execQuery(ns, result, query, queryLanguage, context);
}
void SharedLibraryRepository::beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context)
{
	m_ref->beginOperation(op, context);
}
void SharedLibraryRepository::endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result)
{
	m_ref->endOperation(op, context, result);
}

} // end namespace OpenWBEM

