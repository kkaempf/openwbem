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

#ifndef OW_INDICATION_REP_LAYER_IMPL_HPP_
#define OW_INDICATION_REP_LAYER_IMPL_HPP_
#include "OW_config.h"
#include "OW_IndicationRepLayer.hpp"
#include "OW_CimomCommonFwd.hpp"

namespace OW_NAMESPACE
{

class OW_INDICATIONREPLAYER_API IndicationRepLayerImpl : public IndicationRepLayer
{
public:
	IndicationRepLayerImpl();
	~IndicationRepLayerImpl();
	virtual void open(const blocxx::String&);
	virtual void close();
	virtual blocxx::String getName() const;
	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void shutdown();
	virtual ServiceEnvironmentIFCRef getEnvironment() const;
	virtual void enumClasses(const blocxx::String& ns,
		const blocxx::String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, OperationContext& context);
	virtual void enumClassNames(
		const blocxx::String& ns,
		const blocxx::String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, OperationContext& context);
	virtual CIMInstance getInstance(
		const blocxx::String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const blocxx::StringArray* propertyList,
		OperationContext& context);
	virtual void enumInstances(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList,
		WBEMFlags::EEnumSubclassesFlag enumSubclasses,
		OperationContext& context);
	virtual void enumInstanceNames(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMObjectPathResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, OperationContext& context);
	virtual CIMQualifierType getQualifierType(
		const blocxx::String& ns,
		const blocxx::String& qualifierName, OperationContext& context);
	virtual CIMClass getClass(
		const blocxx::String& ns,
		const blocxx::String& className,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const blocxx::StringArray* propertyList,
		OperationContext& context);
	virtual RepositoryIFC::ELockType getLockTypeForMethod(
		const blocxx::String& ns,
		const CIMObjectPath& path,
		const blocxx::String& methodName,
		const CIMParamValueArray& in,
		OperationContext& context);
	virtual CIMValue invokeMethod(
		const blocxx::String& ns,
		const CIMObjectPath& path,
		const blocxx::String &methodName, const CIMParamValueArray &inParams,
		CIMParamValueArray &outParams, OperationContext& context);
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	virtual void createClass(const blocxx::String& ns,
		const CIMClass &cc, OperationContext& context);
	virtual CIMClass modifyClass(const blocxx::String &ns,
		const CIMClass& cc, OperationContext& context);
	virtual CIMClass deleteClass(const blocxx::String& ns, const blocxx::String& className,
		OperationContext& context);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	virtual void enumQualifierTypes(
		const blocxx::String& ns,
		CIMQualifierTypeResultHandlerIFC& result, OperationContext& context);
	virtual void deleteQualifierType(const blocxx::String& ns, const blocxx::String& qualName,
		OperationContext& context);
	virtual void setQualifierType(const blocxx::String& ns,
		const CIMQualifierType& qt, OperationContext& context);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual CIMObjectPath createInstance(const blocxx::String& ns,
		const CIMInstance &ci, OperationContext& context);
	virtual CIMInstance deleteInstance(const blocxx::String& ns, const CIMObjectPath &path,
		OperationContext& context);
	virtual CIMInstance modifyInstance(
		const blocxx::String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const blocxx::StringArray* propertyList,
		OperationContext& context);
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	virtual void setProperty(
		const blocxx::String& ns,
		const CIMObjectPath &name,
		const blocxx::String &propertyName, const CIMValue &cv,
		OperationContext& context);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	virtual CIMValue getProperty(
		const blocxx::String& ns,
		const CIMObjectPath &name,
		const blocxx::String &propertyName, OperationContext& context);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void associators(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &assocClass, const blocxx::String &resultClass,
		const blocxx::String &role, const blocxx::String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList, OperationContext& context);
	virtual void associatorsClasses(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const blocxx::String &assocClass, const blocxx::String &resultClass,
		const blocxx::String &role, const blocxx::String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList, OperationContext& context);
	virtual void references(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &resultClass, const blocxx::String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList, OperationContext& context);
	virtual void referencesClasses(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const blocxx::String &resultClass, const blocxx::String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList, OperationContext& context);
	virtual void associatorNames(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String &assocClass,
		const blocxx::String &resultClass, const blocxx::String &role,
		const blocxx::String &resultRole, OperationContext& context);
	virtual void referenceNames(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String &resultClass,
		const blocxx::String &role, OperationContext& context);
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

	virtual void execQuery(
		const blocxx::String& ns,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &query, const blocxx::String& queryLanguage,
		OperationContext& context);
	virtual void enumInstancesWQL(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMInstanceResultHandlerIFC& result,
		const WQLSelectStatement& wss,
		const WQLCompile& wc,
		OperationContext& context);
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	void deleteNameSpace(const blocxx::String &ns, OperationContext& context);
	void createNameSpace(const blocxx::String& ns, OperationContext& context);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	void enumNameSpace(StringResultHandlerIFC& result,
		OperationContext& context);

	virtual void beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context);
	virtual void endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result);
	virtual void setCIMServer(const RepositoryIFCRef& src);
private:
	void exportIndication(const CIMInstance& instance,
		const blocxx::String& instNS);
	RepositoryIFCRef m_pServer;
	CIMOMEnvironmentRef m_pEnv;
};

} // end namespace OW_NAMESPACE

#endif
