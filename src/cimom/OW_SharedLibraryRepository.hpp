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
#ifndef __OW_SHAREDLIBRARYREPOSITORY_HPP__
#define __OW_SHAREDLIBRARYREPOSITORY_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_RepositoryIFC.hpp"

class OW_SharedLibraryRepository : public OW_RepositoryIFC
{
public:

	OW_SharedLibraryRepository(OW_SharedLibraryRepositoryIFCRef ref)
		: OW_RepositoryIFC()
		, m_ref(ref) {}

	virtual void close() { m_ref->close(); }

	virtual void open(const OW_String &path) { m_ref->open(path); }

	virtual OW_CIMQualifierType getQualifierType(
		const OW_CIMObjectPath &objPath, const OW_ACLInfo &aclInfo)
	{
		return m_ref->getQualifierType(objPath, aclInfo);
	}

	virtual OW_CIMOMEnvironmentRef getEnvironment() const
	{
		return m_ref->getEnvironment();
	}

	virtual void createNameSpace(const OW_String& ns,
		const OW_ACLInfo &aclInfo)
	{
		m_ref->createNameSpace(ns, aclInfo);
	}

	virtual void deleteNameSpace(const OW_String& ns,
		const OW_ACLInfo &aclInfo)
	{
		m_ref->deleteNameSpace(ns, aclInfo);
	}

	virtual void enumNameSpace(const OW_String& ns,
		OW_StringResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo &aclInfo)
	{
		return m_ref->enumNameSpace(ns, result, deep, aclInfo);
	}

	virtual void enumQualifierTypes(
		const OW_CIMObjectPath &path,
		OW_CIMQualifierTypeResultHandlerIFC& result, const OW_ACLInfo &aclInfo)
	{
		return m_ref->enumQualifierTypes(path, result, aclInfo);
	}

	virtual void deleteQualifierType(const OW_String& ns, const OW_String& qualName,
		const OW_ACLInfo &aclInfo)
	{
		m_ref->deleteQualifierType(ns, qualName, aclInfo);
	}

	virtual void setQualifierType(const OW_CIMObjectPath &name,
		const OW_CIMQualifierType &qt, const OW_ACLInfo &aclInfo)
	{
		m_ref->setQualifierType(name, qt, aclInfo);
	}

	virtual OW_CIMClass getClass(const OW_CIMObjectPath &path,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray *propertyList,
		const OW_ACLInfo &aclInfo)
	{
		return m_ref->getClass(path, localOnly, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}

	virtual OW_CIMClass deleteClass(const OW_String& ns, const OW_String& className,
		const OW_ACLInfo &aclInfo)
	{
		return m_ref->deleteClass(ns, className, aclInfo);
	}

	virtual OW_CIMObjectPath createInstance(const OW_CIMObjectPath &cop, OW_CIMInstance &ci,
		const OW_ACLInfo &aclInfo)
	{
		return m_ref->createInstance(cop, ci, aclInfo);
	}

	virtual void enumInstances(
		const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo &aclInfo)
	{
		return m_ref->enumInstances(path, result, deep, localOnly,includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}

	virtual void createClass(const OW_CIMObjectPath &path,
		OW_CIMClass &cimClass, const OW_ACLInfo &aclInfo)
	{
		m_ref->createClass(path, cimClass, aclInfo);
	}

	virtual OW_CIMClass modifyClass(const OW_CIMObjectPath &name,
		OW_CIMClass &cc, const OW_ACLInfo &aclInfo)
	{
		return m_ref->modifyClass(name, cc, aclInfo);
	}

	virtual void enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_ACLInfo &aclInfo)
	{
		return m_ref->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
			includeClassOrigin, aclInfo);
	}

	virtual void enumClassNames(
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo &aclInfo)
	{
		m_ref->enumClassNames(path, result, deep, aclInfo);
	}

	virtual OW_CIMValue invokeMethod(const OW_CIMObjectPath &name,
		const OW_String &methodName, const OW_CIMParamValueArray &inParams,
		OW_CIMParamValueArray &outParams, const OW_ACLInfo &aclInfo)
	{
		return m_ref->invokeMethod(name, methodName, inParams,
			outParams, aclInfo);
	}

	virtual void enumInstanceNames(
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo &aclInfo)
	{
		return m_ref->enumInstanceNames(path, result, deep, aclInfo);
	}

	virtual OW_CIMInstance getInstance(const OW_CIMObjectPath &cop,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray *propertyList,
		const OW_ACLInfo &aclInfo)
	{
		return m_ref->getInstance(cop, localOnly, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}

	virtual OW_CIMInstance deleteInstance(const OW_String& ns, const OW_CIMObjectPath &cop,
		const OW_ACLInfo &aclInfo)
	{
		return m_ref->deleteInstance(ns, cop, aclInfo);
	}

	virtual void references(const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo &aclInfo)
	{
		m_ref->references(path, result, resultClass, role, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}

	virtual void referencesClasses(const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo &aclInfo)
	{
		m_ref->referencesClasses(path, result, resultClass, role, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}

	virtual OW_CIMInstance modifyInstance(const OW_CIMObjectPath &cop,
		OW_CIMInstance &ci, const OW_ACLInfo &aclInfo)
	{
		return m_ref->modifyInstance(cop, ci, aclInfo);
	}

	virtual void setProperty(const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_CIMValue &cv,
		const OW_ACLInfo &aclInfo)
	{
		m_ref->setProperty(name, propertyName, cv, aclInfo);
	}

	virtual OW_CIMValue getProperty(const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_ACLInfo &aclInfo)
	{
		return m_ref->getProperty(name, propertyName, aclInfo);
	}

	virtual void associatorNames(
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &assocClass,
		const OW_String &resultClass, const OW_String &role,
		const OW_String &resultRole, const OW_ACLInfo &aclInfo)
	{
		m_ref->associatorNames(path, result, assocClass, resultClass, role,
			resultRole, aclInfo);
	}

	virtual void associators(const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo &aclInfo)
	{
		m_ref->associators(path, result, assocClass, resultClass, role,
			resultRole, includeQualifiers, includeClassOrigin, propertyList,
			aclInfo);
	}

	virtual void associatorsClasses(const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo &aclInfo)
	{
		m_ref->associatorsClasses(path, result, assocClass, resultClass, role,
			resultRole, includeQualifiers, includeClassOrigin, propertyList,
			aclInfo);
	}

	virtual void referenceNames(
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &resultClass,
		const OW_String &role, const OW_ACLInfo &aclInfo)
	{
		m_ref->referenceNames(path, result, resultClass, role, aclInfo);
	}

	virtual void execQuery(const OW_CIMNameSpace &ns,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &query, const OW_String &queryLanguage,
		const OW_ACLInfo &aclInfo)
	{
		m_ref->execQuery(ns, result, query, queryLanguage, aclInfo);
	}

	virtual OW_ReadLock getReadLock()
	{
		return m_ref->getReadLock();
	}

	virtual OW_WriteLock getWriteLock()
	{
		return m_ref->getWriteLock();
	}

private:

	OW_SharedLibraryRepositoryIFCRef m_ref;
};

#endif	// __OW_SHAREDLIBRARYREPOSITORY_HPP__


