/*******************************************************************************
* Copyright (C) 2001-2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_NAMESPACEPROVIDER_IFC_HPP_INCLUDE_GUARD_
#define OW_NAMESPACEPROVIDER_IFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ProviderBaseIFC.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_WBEMFlags.hpp"

namespace OW_NAMESPACE
{

class OW_PROVIDER_API NamespaceProviderIFC: public ProviderBaseIFC
{
public:

	virtual ~NamespaceProviderIFC();

	/**
	 * Called after construction
	 */
	void init(const ProviderEnvironmentIFCRef& env);

	/**
	 * Called during cimom shutdown.
	 */
	virtual void shutdown(const ProviderEnvironmentIFCRef& env);

	virtual CIMQualifierType getQualifierType(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& qualifierName) = 0;

#ifndef OW_DISABLE_QUALIFIER_DECLARATION

	virtual void enumQualifierTypes(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		CIMQualifierTypeResultHandlerIFC& result) = 0;

	virtual void deleteQualifierType(
		const ProviderEnvironmentIFCRef& env,
		const String& ns, 
		const String& qualName) = 0;

	virtual void setQualifierType(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMQualifierType& qt) = 0;

#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

	virtual CIMClass getClass(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		WBEMFlags::ELocalOnlyFlag localOnly, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray* propertyList) = 0;

#ifndef OW_DISABLE_SCHEMA_MANIPULATION

	virtual CIMClass deleteClass(
		const ProviderEnvironmentIFCRef& env,
		const String& ns, 
		const String& className) = 0;

	virtual void createClass(
		const ProviderEnvironmentIFCRef& env,
		const String& ns, 
		const CIMClass& cimClass) = 0;

	virtual CIMClass modifyClass(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMClass& cc) = 0;

#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
	
	virtual void enumClasses(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, 
		WBEMFlags::ELocalOnlyFlag localOnly, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin) = 0;

	virtual void enumClassNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep) = 0;

	virtual void enumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		WBEMFlags::EEnumSubclassesFlag enumSubclasses) = 0;

	virtual void enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep) = 0;

	virtual CIMInstance getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray* propertyList) = 0;

#ifndef OW_DISABLE_INSTANCE_MANIPULATION

	virtual CIMInstance deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns, 
		const CIMObjectPath& cop) = 0;

	virtual CIMObjectPath createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& ci) = 0;

	virtual CIMInstance modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList) = 0;

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

	virtual void setProperty(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& name, 
		const String& propertyName,
		const CIMValue& cv) = 0;

#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

	virtual CIMValue getProperty(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& name,
		const String& propertyName) = 0;

#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

	virtual CIMValue invokeMethod(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName, 
		const CIMParamValueArray& inParams,
		CIMParamValueArray& outParams) = 0;

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

	virtual void associatorNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		CIMObjectPathResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, 
		const String& role,
		const String& resultRole) = 0;

	virtual void associators(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		CIMInstanceResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, 
		const String& role,
		const String& resultRole,  
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray* propertyList) = 0;

	virtual void associatorsClasses(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, 
		const String& role,
		const String& resultRole,  
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray* propertyList) = 0;

	virtual void referenceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		CIMObjectPathResultHandlerIFC& result,
		const String& resultClass,
		const String& role) = 0;

	virtual void references(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		CIMInstanceResultHandlerIFC& result,
		const String& resultClass,
		const String& role, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray* propertyList) = 0;
	
	virtual void referencesClasses(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& resultClass,
		const String& role, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray* propertyList) = 0;

#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	
	virtual void execQuery(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		CIMInstanceResultHandlerIFC& result,
		const String &query, 
		const String& queryLanguage) = 0;
};

} // end namespace OW_NAMESPACE

#endif	// OW_NAMESPACEPROVIDER_IFC_HPP_INCLUDE_GUARD_

