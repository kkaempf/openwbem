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
#ifndef OW_SHAREDLIBRARYREPOSITORY_HPP_INCLUDE_GUARD_
#define OW_SHAREDLIBRARYREPOSITORY_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_RepositoryIFC.hpp"

class OW_SharedLibraryRepository : public OW_RepositoryIFC
{
public:

	OW_SharedLibraryRepository(OW_SharedLibraryRepositoryIFCRef ref);
	~OW_SharedLibraryRepository();

	virtual void close();

	virtual void open(const OW_String &path);

	virtual OW_ServiceEnvironmentIFCRef getEnvironment() const;

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual void createNameSpace(const OW_String& ns,
		const OW_UserInfo &aclInfo);

	virtual void deleteNameSpace(const OW_String& ns,
		const OW_UserInfo &aclInfo);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

	virtual void enumNameSpace(OW_StringResultHandlerIFC& result,
		const OW_UserInfo &aclInfo);

	virtual OW_CIMQualifierType getQualifierType(
		const OW_String& ns,
		const OW_String& qualifierName, const OW_UserInfo &aclInfo);

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	virtual void enumQualifierTypes(
		const OW_String& ns,
		OW_CIMQualifierTypeResultHandlerIFC& result, const OW_UserInfo &aclInfo);

	virtual void deleteQualifierType(const OW_String& ns, const OW_String& qualName,
		const OW_UserInfo &aclInfo);

	virtual void setQualifierType(const OW_String& ns,
		const OW_CIMQualifierType &qt, const OW_UserInfo &aclInfo);

#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

	virtual OW_CIMClass getClass(
		const OW_String& ns,
		const OW_String& className,
		OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OW_StringArray *propertyList,
		const OW_UserInfo &aclInfo);

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual OW_CIMObjectPath createInstance(const OW_String& ns, const OW_CIMInstance &ci,
		const OW_UserInfo &aclInfo);

	virtual OW_CIMInstance modifyInstance(
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_UserInfo &aclInfo);

	virtual void setProperty(
		const OW_String& ns,
		const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_CIMValue &cv,
		const OW_UserInfo &aclInfo);

	virtual OW_CIMInstance deleteInstance(const OW_String& ns, const OW_CIMObjectPath &cop,
		const OW_UserInfo &aclInfo);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION


	virtual void enumInstances(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_WBEMFlags::EDeepFlag deep, OW_WBEMFlags::ELocalOnlyFlag localOnly,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray *propertyList, 
		OW_WBEMFlags::EEnumSubclassesFlag enumSubclasses, 
		const OW_UserInfo &aclInfo);

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	virtual void createClass(const OW_String& ns,
		const OW_CIMClass &cimClass, const OW_UserInfo &aclInfo);

	virtual OW_CIMClass modifyClass(const OW_String &ns,
		const OW_CIMClass &cc, const OW_UserInfo &aclInfo);

	virtual OW_CIMClass deleteClass(const OW_String& ns, const OW_String& className,
		const OW_UserInfo &aclInfo);
#endif

	virtual void enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_WBEMFlags::EDeepFlag deep, OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OW_UserInfo &aclInfo);

	virtual void enumClassNames(
		const OW_String& ns,
		const OW_String& className,
		OW_StringResultHandlerIFC& result,
		OW_WBEMFlags::EDeepFlag deep, const OW_UserInfo &aclInfo);

	virtual OW_CIMValue invokeMethod(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String &methodName, const OW_CIMParamValueArray &inParams,
		OW_CIMParamValueArray &outParams, const OW_UserInfo &aclInfo);

	virtual void enumInstanceNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_WBEMFlags::EDeepFlag deep, const OW_UserInfo &aclInfo);

	virtual OW_CIMInstance getInstance(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OW_StringArray *propertyList,
		const OW_UserInfo &aclInfo);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void references(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray *propertyList, const OW_UserInfo &aclInfo);

	virtual void referencesClasses(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray *propertyList, const OW_UserInfo &aclInfo);
#endif

	virtual OW_CIMValue getProperty(
		const OW_String& ns,
		const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_UserInfo &aclInfo);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void associatorNames(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &assocClass,
		const OW_String &resultClass, const OW_String &role,
		const OW_String &resultRole, const OW_UserInfo &aclInfo);

	virtual void associators(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray *propertyList, const OW_UserInfo &aclInfo);

	virtual void associatorsClasses(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray *propertyList, const OW_UserInfo &aclInfo);

	virtual void referenceNames(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &resultClass,
		const OW_String &role, const OW_UserInfo &aclInfo);
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

	virtual void execQuery(
		const OW_String& ns,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &query, const OW_String &queryLanguage,
		const OW_UserInfo &aclInfo);

	virtual void getSchemaReadLock();

	virtual void getSchemaWriteLock();

	virtual void releaseSchemaReadLock();

	virtual void releaseSchemaWriteLock();

	virtual void getInstanceReadLock();

	virtual void getInstanceWriteLock();

	virtual void releaseInstanceReadLock();

	virtual void releaseInstanceWriteLock();

private:

	OW_SharedLibraryRepositoryIFCRef m_ref;
};

#endif



