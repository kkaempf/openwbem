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
#ifndef OW_SHAREDLIBRARYREPOSITORY_HPP_INCLUDE_GUARD_
#define OW_SHAREDLIBRARYREPOSITORY_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_RepositoryIFC.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMObjectPath.hpp"

class OW_SharedLibraryRepository : public OW_RepositoryIFC
{
public:

	OW_SharedLibraryRepository(OW_SharedLibraryRepositoryIFCRef ref)
		: OW_RepositoryIFC()
		, m_ref(ref) {}

	virtual void close() { m_ref->close(); }

	virtual void open(const OW_String &path) { m_ref->open(path); }

	virtual OW_CIMQualifierType getQualifierType(
		const OW_String& ns,
		const OW_String& qualifierName, const OW_UserInfo &aclInfo)
	{
		return m_ref->getQualifierType(ns, qualifierName, aclInfo);
	}

	virtual OW_ServiceEnvironmentIFCRef getEnvironment() const
	{
		return m_ref->getEnvironment();
	}

	virtual void createNameSpace(const OW_String& ns,
		const OW_UserInfo &aclInfo)
	{
		m_ref->createNameSpace(ns, aclInfo);
	}

	virtual void deleteNameSpace(const OW_String& ns,
		const OW_UserInfo &aclInfo)
	{
		m_ref->deleteNameSpace(ns, aclInfo);
	}

	virtual void enumNameSpace(OW_StringResultHandlerIFC& result,
		const OW_UserInfo &aclInfo)
	{
		return m_ref->enumNameSpace(result, aclInfo);
	}

	virtual void enumQualifierTypes(
		const OW_String& ns,
		OW_CIMQualifierTypeResultHandlerIFC& result, const OW_UserInfo &aclInfo)
	{
		return m_ref->enumQualifierTypes(ns, result, aclInfo);
	}

	virtual void deleteQualifierType(const OW_String& ns, const OW_String& qualName,
		const OW_UserInfo &aclInfo)
	{
		m_ref->deleteQualifierType(ns, qualName, aclInfo);
	}

	virtual void setQualifierType(const OW_String& ns,
		const OW_CIMQualifierType &qt, const OW_UserInfo &aclInfo)
	{
		m_ref->setQualifierType(ns, qt, aclInfo);
	}

	virtual OW_CIMClass getClass(
		const OW_String& ns,
		const OW_String& className,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray *propertyList,
		const OW_UserInfo &aclInfo)
	{
		return m_ref->getClass(ns, className, localOnly, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}

	virtual OW_CIMClass deleteClass(const OW_String& ns, const OW_String& className,
		const OW_UserInfo &aclInfo)
	{
		return m_ref->deleteClass(ns, className, aclInfo);
	}

	virtual OW_CIMObjectPath createInstance(const OW_String& ns, const OW_CIMInstance &ci,
		const OW_UserInfo &aclInfo)
	{
		return m_ref->createInstance(ns, ci, aclInfo);
	}

	virtual void enumInstances(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, OW_Bool enumSubClasses, 
		const OW_UserInfo &aclInfo)
	{
		return m_ref->enumInstances(ns, className, result, deep, localOnly,includeQualifiers,
			includeClassOrigin, propertyList, enumSubClasses, aclInfo);
	}

	virtual void createClass(const OW_String& ns,
		const OW_CIMClass &cimClass, const OW_UserInfo &aclInfo)
	{
		m_ref->createClass(ns, cimClass, aclInfo);
	}

	virtual OW_CIMClass modifyClass(const OW_String &ns,
		const OW_CIMClass &cc, const OW_UserInfo &aclInfo)
	{
		return m_ref->modifyClass(ns, cc, aclInfo);
	}

	virtual void enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_UserInfo &aclInfo)
	{
		return m_ref->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
			includeClassOrigin, aclInfo);
	}

	virtual void enumClassNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_UserInfo &aclInfo)
	{
		m_ref->enumClassNames(ns, className, result, deep, aclInfo);
	}

	virtual OW_CIMValue invokeMethod(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String &methodName, const OW_CIMParamValueArray &inParams,
		OW_CIMParamValueArray &outParams, const OW_UserInfo &aclInfo)
	{
		return m_ref->invokeMethod(ns, path, methodName, inParams,
			outParams, aclInfo);
	}

	virtual void enumInstanceNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_UserInfo &aclInfo)
	{
		return m_ref->enumInstanceNames(ns, className, result, deep, aclInfo);
	}

	virtual OW_CIMInstance getInstance(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray *propertyList,
		const OW_UserInfo &aclInfo)
	{
		return m_ref->getInstance(ns, instanceName, localOnly, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}

	virtual OW_CIMInstance deleteInstance(const OW_String& ns, const OW_CIMObjectPath &cop,
		const OW_UserInfo &aclInfo)
	{
		return m_ref->deleteInstance(ns, cop, aclInfo);
	}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void references(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_UserInfo &aclInfo)
	{
		m_ref->references(ns, path, result, resultClass, role, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}

	virtual void referencesClasses(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_UserInfo &aclInfo)
	{
		m_ref->referencesClasses(ns, path, result, resultClass, role, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}
#endif

	virtual OW_CIMInstance modifyInstance(
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		OW_Bool includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_UserInfo &aclInfo)
	{
		return m_ref->modifyInstance(ns, modifiedInstance, includeQualifiers,
			propertyList, aclInfo);
	}

	virtual void setProperty(
		const OW_String& ns,
		const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_CIMValue &cv,
		const OW_UserInfo &aclInfo)
	{
		m_ref->setProperty(ns, name, propertyName, cv, aclInfo);
	}

	virtual OW_CIMValue getProperty(
		const OW_String& ns,
		const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_UserInfo &aclInfo)
	{
		return m_ref->getProperty(ns, name, propertyName, aclInfo);
	}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void associatorNames(
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

	virtual void associators(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_UserInfo &aclInfo)
	{
		m_ref->associators(ns, path, result, assocClass, resultClass, role,
			resultRole, includeQualifiers, includeClassOrigin, propertyList,
			aclInfo);
	}

	virtual void associatorsClasses(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_UserInfo &aclInfo)
	{
		m_ref->associatorsClasses(ns, path, result, assocClass, resultClass, role,
			resultRole, includeQualifiers, includeClassOrigin, propertyList,
			aclInfo);
	}

	virtual void referenceNames(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &resultClass,
		const OW_String &role, const OW_UserInfo &aclInfo)
	{
		m_ref->referenceNames(ns, path, result, resultClass, role, aclInfo);
	}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

	virtual void execQuery(
		const OW_String& ns,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &query, const OW_String &queryLanguage,
		const OW_UserInfo &aclInfo)
	{
		m_ref->execQuery(ns, result, query, queryLanguage, aclInfo);
	}

	virtual void getSchemaReadLock()
	{
		m_ref->getSchemaReadLock();
	}

	virtual void getSchemaWriteLock()
	{
		m_ref->getSchemaWriteLock();
	}

	virtual void releaseSchemaReadLock()
	{
		m_ref->releaseSchemaReadLock();
	}

	virtual void releaseSchemaWriteLock()
	{
		m_ref->releaseSchemaWriteLock();
	}

	virtual void getInstanceReadLock()
	{
		m_ref->getInstanceReadLock();
	}

	virtual void getInstanceWriteLock()
	{
		m_ref->getInstanceWriteLock();
	}

	virtual void releaseInstanceReadLock()
	{
		m_ref->releaseInstanceReadLock();
	}

	virtual void releaseInstanceWriteLock()
	{
		m_ref->releaseInstanceWriteLock();
	}

private:

	OW_SharedLibraryRepositoryIFCRef m_ref;
};

#endif



