/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_RepositoryCIMOMHandle.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
RepositoryCIMOMHandle::RepositoryCIMOMHandle(RepositoryIFCRef pRepos, OperationContext& context, ELockingFlag lock)
	: CIMOMHandleIFC()
	, m_pServer(pRepos)
	, m_lock(lock)
	, m_context(context)
{
}
//////////////////////////////////////////////////////////////////////////////
class OperationScope
{
public:
	OperationScope(RepositoryCIMOMHandle* pHdl, EOperationFlag op, OperationContext& context)
	: m_pHdl(pHdl)
	, m_op(op)
	, m_context(context)
	, m_result(WBEMFlags::E_FAILED)
	{
		m_pHdl->beginOperation(m_op, m_context);
	}
	~OperationScope()
	{
		m_pHdl->endOperation(m_op, m_context, m_result);
	}
	void completedSuccessfully()
	{
		m_result = WBEMFlags::E_SUCCESS;
	}
private:
	RepositoryCIMOMHandle* m_pHdl;
	EOperationFlag m_op;
	OperationContext& m_context;
	WBEMFlags::EOperationResultFlag m_result;
};

//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::close()
{
}
//////////////////////////////////////////////////////////////////////////////
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
void
RepositoryCIMOMHandle::createNameSpace(const String& ns)
{
	OperationScope os(this, E_CREATE_NAMESPACE, m_context);
	m_pServer->createNameSpace(ns, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::deleteNameSpace(const String& ns)
{
	OperationScope os(this, E_DELETE_NAMESPACE, m_context);
	m_pServer->deleteNameSpace(ns, m_context);
	os.completedSuccessfully();
}
#endif
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::enumNameSpace(StringResultHandlerIFC& result)
{
	OperationScope os(this, E_ENUM_NAMESPACE, m_context);
	m_pServer->enumNameSpace(result, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::enumClass(const String& ns,
	const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin)
{
	OperationScope os(this, E_ENUM_CLASSES, m_context);
	m_pServer->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::enumClassNames(const String& ns,
		const String& className,
		StringResultHandlerIFC& result,
		EDeepFlag deep)
{
	OperationScope os(this, E_ENUM_CLASS_NAMES, m_context);
	m_pServer->enumClassNames(ns, className, result, deep, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	OperationScope os(this, E_ENUM_INSTANCES, m_context);
	m_pServer->enumInstances(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, E_ENUM_SUBCLASSES, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result)
{
	OperationScope os(this, E_ENUM_INSTANCE_NAMES, m_context);
	m_pServer->enumInstanceNames(ns, className, result, E_DEEP, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
RepositoryCIMOMHandle::getQualifierType(const String& ns,
		const String& qualifierName)
{
	OperationScope os(this, E_GET_QUALIFIER_TYPE, m_context);
	CIMQualifierType rval = m_pServer->getQualifierType(ns, qualifierName, m_context);
	os.completedSuccessfully();
	return rval;
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::deleteQualifierType(const String& ns, const String& qualName)
{
	OperationScope os(this, E_DELETE_QUALIFIER_TYPE, m_context);
	m_pServer->deleteQualifierType(ns, qualName, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result)
{
	OperationScope os(this, E_ENUM_QUALIFIER_TYPES, m_context);
	m_pServer->enumQualifierTypes(ns, result, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::setQualifierType(const String& ns,
	const CIMQualifierType& qt)
{
	OperationScope os(this, E_SET_QUALIFIER_TYPE, m_context);
	m_pServer->setQualifierType(ns, qt, m_context);
	os.completedSuccessfully();
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
RepositoryCIMOMHandle::getClass(
	const String& ns,
	const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	OperationScope os(this, E_GET_CLASS, m_context);
	CIMClass cls = m_pServer->getClass(ns, className, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, m_context);
	os.completedSuccessfully();
	return cls;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
RepositoryCIMOMHandle::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	OperationScope os(this, E_GET_INSTANCE, m_context);
	CIMInstance rval = m_pServer->getInstance(ns, instanceName, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, m_context);
	os.completedSuccessfully();
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
RepositoryCIMOMHandle::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName, const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams)
{
	OperationScope os(this, E_INVOKE_METHOD, m_context);
	CIMValue rval = m_pServer->invokeMethod(ns, path, methodName, inParams, outParams,
		m_context);
	os.completedSuccessfully();
	return rval;
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::modifyClass(
	const String& ns,
	const CIMClass& cc)
{
	OperationScope os(this, E_MODIFY_CLASS, m_context);
	m_pServer->modifyClass(ns, cc, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::createClass(const String& ns,
	const CIMClass& cc)
{
	OperationScope os(this, E_CREATE_CLASS, m_context);
	m_pServer->createClass(ns, cc, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::deleteClass(const String& ns, const String& className)
{
	OperationScope os(this, E_DELETE_CLASS, m_context);
	m_pServer->deleteClass(ns, className, m_context);
	os.completedSuccessfully();
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList)
{
	OperationScope os(this, E_MODIFY_INSTANCE, m_context);
	m_pServer->modifyInstance(ns, modifiedInstance, includeQualifiers,
		propertyList, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
RepositoryCIMOMHandle::createInstance(const String& ns,
	const CIMInstance& ci)
{
	OperationScope os(this, E_CREATE_INSTANCE, m_context);
	CIMObjectPath rval = m_pServer->createInstance(ns, ci, m_context);
	os.completedSuccessfully();
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::deleteInstance(const String& ns, const CIMObjectPath& path)
{
	OperationScope os(this, E_DELETE_INSTANCE, m_context);
	m_pServer->deleteInstance(ns, path, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::setProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, const CIMValue& cv)
{
	OperationScope os(this, E_SET_PROPERTY, m_context);
	m_pServer->setProperty(ns, name, propertyName, cv, m_context);
	os.completedSuccessfully();
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
RepositoryCIMOMHandle::getProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName)
{
	OperationScope os(this, E_GET_PROPERTY, m_context);
	CIMValue rval = m_pServer->getProperty(ns, name, propertyName, m_context);
	os.completedSuccessfully();
	return rval;
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::associatorNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole)
{
	OperationScope os(this, E_ASSOCIATOR_NAMES, m_context);
	m_pServer->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::associators(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	OperationScope os(this, E_ASSOCIATORS, m_context);
	m_pServer->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::associatorsClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	OperationScope os(this, E_ASSOCIATORS_CLASSES, m_context);
	m_pServer->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::referenceNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass, const String& role)
{
	OperationScope os(this, E_REFERENCE_NAMES, m_context);
	m_pServer->referenceNames(ns, path, result, resultClass, role, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::references(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	OperationScope os(this, E_REFERENCES, m_context);
	m_pServer->references(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::referencesClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	OperationScope os(this, E_REFERENCES_CLASSES, m_context);
	m_pServer->referencesClasses(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_context);
	os.completedSuccessfully();
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::execQuery(
	const String& ns,
	CIMInstanceResultHandlerIFC& result,
	const String& query,
	const String& queryLanguage)
{
	OperationScope os(this, E_EXEC_QUERY, m_context);
	m_pServer->execQuery(ns, result, query, queryLanguage, m_context);
	os.completedSuccessfully();
}
//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::beginOperation(WBEMFlags::EOperationFlag op, OperationContext& m_context)
{
	if (m_lock)
	{
		m_pServer->beginOperation(op, m_context);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
RepositoryCIMOMHandle::endOperation(WBEMFlags::EOperationFlag op, OperationContext& m_context, WBEMFlags::EOperationResultFlag result)
{
	if (m_lock)
	{
		m_pServer->endOperation(op, m_context, result);
	}
}

} // end namespace OpenWBEM

