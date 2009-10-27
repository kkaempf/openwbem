/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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

#ifndef OW_REPOSITORY_CIMOMHANDLE_HPP_INCLUDE_GUARD_
#define OW_REPOSITORY_CIMOMHANDLE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RepositoryIFC.hpp"
#include "blocxx/RWLocker.hpp"
#include "blocxx/IntrusiveReference.hpp"
#include "OW_CommonFwd.hpp"
#include "blocxx/SafeBool.hpp"

namespace OW_NAMESPACE
{

/**
 * The RepositoryCIMOMHandle class is a derivitive of the CIMOMHandleIFC that
 * is used by all components that need access to CIM Services.
 */
class OW_COMMON_API RepositoryCIMOMHandle : public CIMOMHandleIFC
{
public:

	/**
	 * Create a new RepositoryCIMOMHandle with a given repository interface
	 * and user access contol information.
	 * @param pRepos A reference to a Repository that will be used by this
	 *		RepositoryCIMOMHandle.
	 * @param context The operation context that will be associated with
	 *		this RepositoryCIMOMHandle.
	 */
	RepositoryCIMOMHandle(const RepositoryIFCRef& pRepos,
		OperationContext& context);

	virtual ~RepositoryCIMOMHandle();

	virtual void close();

#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	virtual void createNameSpace(const blocxx::String& ns);
	virtual void deleteNameSpace(const blocxx::String& ns);
#endif
	virtual void enumNameSpace(StringResultHandlerIFC& result);
	virtual void enumClass(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN);
	virtual void enumClassNames(
		const blocxx::String& ns,
		const blocxx::String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep=WBEMFlags::E_DEEP);
	virtual CIMQualifierType getQualifierType(const blocxx::String& ns,
		const blocxx::String& qualifierName);
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	virtual void enumQualifierTypes(
		const blocxx::String& ns,
		CIMQualifierTypeResultHandlerIFC& result);
	virtual void deleteQualifierType(const blocxx::String& ns, const blocxx::String& qualName);
	virtual void setQualifierType(const blocxx::String& ns,
		const CIMQualifierType& qt);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
	virtual CIMInstance getInstance(
		const blocxx::String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const blocxx::StringArray* propertyList=0);
	virtual void enumInstances(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const blocxx::StringArray* propertyList=0);
	virtual void enumInstanceNames(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMObjectPathResultHandlerIFC& result);
	virtual CIMClass getClass(
		const blocxx::String& ns,
		const blocxx::String& className,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN,
		const blocxx::StringArray* propertyList=NULL);
	virtual CIMValue invokeMethod(
		const blocxx::String& ns,
		const CIMObjectPath& path,
		const blocxx::String &methodName, const CIMParamValueArray &inParams,
		CIMParamValueArray &outParams);
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	virtual void createClass(const blocxx::String& ns,
		const CIMClass &cc);
	virtual void modifyClass(const blocxx::String &ns, const CIMClass &cc);
	virtual void deleteClass(const blocxx::String& ns, const blocxx::String& className);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual CIMObjectPath createInstance(const blocxx::String& ns,
		const CIMInstance &ci);
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	virtual void setProperty(
		const blocxx::String& ns,
		const CIMObjectPath &name,
		const blocxx::String &propertyName, const CIMValue &cv);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	virtual void modifyInstance(
		const blocxx::String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const blocxx::StringArray* propertyList);
	virtual void deleteInstance(const blocxx::String& ns, const CIMObjectPath& path);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	virtual CIMValue getProperty(
		const blocxx::String& ns,
		const CIMObjectPath &name,
		const blocxx::String &propertyName);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void associators(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &assocClass, const blocxx::String &resultClass,
		const blocxx::String &role, const blocxx::String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList);
	virtual void associatorsClasses(
		const blocxx::String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const blocxx::String& assocClass=blocxx::String(),
		const blocxx::String& resultClass=blocxx::String(),
		const blocxx::String& role=blocxx::String(),
		const blocxx::String& resultRole=blocxx::String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers=WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin=WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const blocxx::StringArray* propertyList=0);
virtual void references(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &resultClass, const blocxx::String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList);
	virtual void referencesClasses(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const blocxx::String &resultClass, const blocxx::String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList);
	virtual void associatorNames(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String &assocClass,
		const blocxx::String &resultClass, const blocxx::String &role,
		const blocxx::String &resultRole);
	virtual void referenceNames(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String &resultClass,
		const blocxx::String &role);
#endif

	virtual void execQuery(
		const blocxx::String& ns,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &query, const blocxx::String& queryLanguage);

	BLOCXX_SAFE_BOOL_IMPL(RepositoryCIMOMHandle, RepositoryIFCRef, RepositoryCIMOMHandle::m_pServer, m_pServer)

	void beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context);
	void endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result);

private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	/**
	 * A Reference to the Repository interface that this RepositoryCIMOMHandle
	 * will use.
	 */

	RepositoryIFCRef m_pServer;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

protected:
	OperationContext& m_context;
};
OW_EXPORT_TEMPLATE(OW_COMMON_API, IntrusiveReference, RepositoryCIMOMHandle);

} // end namespace OW_NAMESPACE

#endif
