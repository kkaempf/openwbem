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

#ifndef OW_INDICATION_REP_LAYER_IMPL_HPP_
#define OW_INDICATION_REP_LAYER_IMPL_HPP_
#include "OW_config.h"
#include "OW_IndicationRepLayer.hpp"

namespace OpenWBEM
{

class CIMOMEnvironment;
typedef Reference<CIMOMEnvironment> CIMOMEnvironmentRef;
class IndicationRepLayerImpl : public IndicationRepLayer
{
public:
	IndicationRepLayerImpl();
	~IndicationRepLayerImpl();
	virtual void open(const String&);
	virtual void close();
	virtual ServiceEnvironmentIFCRef getEnvironment() const;
	virtual void enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, OperationContext& context);
	virtual void enumClassNames(
		const String& ns,
		const String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, OperationContext& context);
	virtual CIMInstance getInstance(
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		OperationContext& context);
	virtual void enumInstances(
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		WBEMFlags::EEnumSubclassesFlag enumSubclasses,
		OperationContext& context);
	virtual void enumInstanceNames(
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, OperationContext& context);
	virtual CIMQualifierType getQualifierType(
		const String& ns,
		const String& qualifierName, OperationContext& context);
	virtual CIMClass getClass(
		const String& ns,
		const String& className,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		OperationContext& context);
	virtual CIMValue invokeMethod(
		const String& ns,
		const CIMObjectPath& path,
		const String &methodName, const CIMParamValueArray &inParams,
		CIMParamValueArray &outParams, OperationContext& context);
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	virtual void createClass(const String& ns,
		const CIMClass &cc, OperationContext& context);
	virtual CIMClass modifyClass(const String &ns,
		const CIMClass& cc, OperationContext& context);
	virtual CIMClass deleteClass(const String& ns, const String& className,
		OperationContext& context);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	virtual void enumQualifierTypes(
		const String& ns,
		CIMQualifierTypeResultHandlerIFC& result, OperationContext& context);
	virtual void deleteQualifierType(const String& ns, const String& qualName,
		OperationContext& context);
	virtual void setQualifierType(const String& ns,
		const CIMQualifierType& qt, OperationContext& context);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual CIMObjectPath createInstance(const String& ns,
		const CIMInstance &ci, OperationContext& context);
	virtual CIMInstance deleteInstance(const String& ns, const CIMObjectPath &path,
		OperationContext& context);
	virtual CIMInstance modifyInstance(
		const String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		OperationContext& context);
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	virtual void setProperty(
		const String& ns,
		const CIMObjectPath &name,
		const String &propertyName, const CIMValue &cv,
		OperationContext& context);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	virtual CIMValue getProperty(
		const String& ns,
		const CIMObjectPath &name,
		const String &propertyName, OperationContext& context);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void associators(
		const String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const String &assocClass, const String &resultClass,
		const String &role, const String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, OperationContext& context);
	virtual void associatorsClasses(
		const String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const String &assocClass, const String &resultClass,
		const String &role, const String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, OperationContext& context);
	virtual void references(
		const String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const String &resultClass, const String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, OperationContext& context);
	virtual void referencesClasses(
		const String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const String &resultClass, const String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, OperationContext& context);
	virtual void associatorNames(
		const String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const String &assocClass,
		const String &resultClass, const String &role,
		const String &resultRole, OperationContext& context);
	virtual void referenceNames(
		const String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const String &resultClass,
		const String &role, OperationContext& context);
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	
	virtual void execQuery(
		const String& ns,
		CIMInstanceResultHandlerIFC& result,
		const String &query, const String& queryLanguage,
		OperationContext& context);
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	void deleteNameSpace(const String &ns, OperationContext& context);
	void createNameSpace(const String& ns, OperationContext& context);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	void enumNameSpace(StringResultHandlerIFC& result,
		OperationContext& context);
	
	virtual void beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context);
	virtual void endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result);
	virtual void setCIMServer(const RepositoryIFCRef& src);
private:
	void exportIndication(const CIMInstance& instance,
		const String& instNS);
	RepositoryIFCRef m_pServer;
	CIMOMEnvironmentRef m_pEnv;
};

} // end namespace OpenWBEM

#endif
