/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_MOFCompCIMOMHandle.hpp"
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
MOFCompCIMOMHandle::MOFCompCIMOMHandle(RepositoryIFCRef pRepos, OperationContext& context)
	: CIMOMHandleIFC()
	, m_pServer(pRepos)
	, m_context(context)
{
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::close()
{
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::enumClass(const String& ns,
	const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin)
{
	m_pServer->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::enumClassNames(const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep)
{
	m_pServer->enumClassNames(ns, className, result, deep, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	m_pServer->enumInstances(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, E_ENUM_SUBCLASSES, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result)
{
	return m_pServer->enumInstanceNames(ns, className, result, E_DEEP, m_context);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
MOFCompCIMOMHandle::getQualifierType(const String& ns,
		const String& qualifierName)
{
	return m_pServer->getQualifierType(ns, qualifierName, m_context);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::deleteQualifierType(const String& ns, const String& qualName)
{
	m_pServer->deleteQualifierType(ns, qualName, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result)
{
	m_pServer->enumQualifierTypes(ns, result, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::setQualifierType(const String& ns,
	const CIMQualifierType& qt)
{
	m_pServer->setQualifierType(ns, qt, m_context);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
MOFCompCIMOMHandle::getClass(
	const String& ns,
	const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	CIMClass cls = m_pServer->getClass(ns, className, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, m_context);
	return cls;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
MOFCompCIMOMHandle::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	return m_pServer->getInstance(ns, instanceName, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, m_context);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
MOFCompCIMOMHandle::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName, const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams)
{
	return m_pServer->invokeMethod(ns, path, methodName, inParams, outParams,
		m_context);
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::modifyClass(
	const String& ns,
	const CIMClass& cc)
{
	m_pServer->modifyClass(ns, cc, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::createClass(const String& ns,
	const CIMClass& cc)
{
	m_pServer->createClass(ns, cc, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::deleteClass(const String& ns, const String& className)
{
	m_pServer->deleteClass(ns, className, m_context);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList)
{
	m_pServer->modifyInstance(ns, modifiedInstance, includeQualifiers,
		propertyList, m_context);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
MOFCompCIMOMHandle::createInstance(const String& ns,
	const CIMInstance& ci)
{
	return m_pServer->createInstance(ns, ci, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::deleteInstance(const String& ns, const CIMObjectPath& path)
{
	m_pServer->deleteInstance(ns, path, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::setProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, const CIMValue& cv)
{
	m_pServer->setProperty(ns, name, propertyName, cv, m_context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
MOFCompCIMOMHandle::getProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName)
{
	return m_pServer->getProperty(ns, name, propertyName, m_context);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::associatorNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole)
{
	m_pServer->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::associators(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	m_pServer->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::associatorsClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	m_pServer->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::referenceNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass, const String& role)
{
	m_pServer->referenceNames(ns, path, result, resultClass, role, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::references(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	m_pServer->references(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_context);
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::referencesClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	m_pServer->referencesClasses(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_context);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::execQuery(
	const String& ns,
	CIMInstanceResultHandlerIFC& result,
	const String& query,
	const String& queryLanguage)
{
	m_pServer->execQuery(ns, result, query, queryLanguage, m_context);
}
//////////////////////////////////////////////////////////////////////////////
CIMFeatures
MOFCompCIMOMHandle::getServerFeatures()
{
	CIMFeatures cf;
	cf.protocolVersion = "1.0";
	cf.cimom = "/cimom";
	cf.cimProduct = CIMFeatures::SERVER;
	cf.extURL = "http://www.dmtf.org/cim/mapping/http/v1.0";
	cf.supportedGroups.push_back("basic-read");
	cf.supportedGroups.push_back("basic-write");
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	cf.supportedGroups.push_back("schema-manipulation");
#endif
	cf.supportedGroups.push_back("instance-manipulation");
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	cf.supportedGroups.push_back("qualifier-declaration");
#endif
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	cf.supportedGroups.push_back("association-traversal");
#endif
	cf.supportedQueryLanguages.clear();
	cf.supportsBatch = true;
	cf.validation = String();
	return cf;
}
//////////////////////////////////////////////////////////////////////////////
void
MOFCompCIMOMHandle::exportIndication(const CIMInstance&,
	const String&)
{
}

} // end namespace OpenWBEM

