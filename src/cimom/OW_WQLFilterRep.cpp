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
#include "OW_config.h"
#include "OW_WQLFilterRep.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_Array.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMServer.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_UserInfo.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
///////////////////////////////////////////////////////////////////////////////
WQLFilterRep::WQLFilterRep(const CIMInstance& inst,
	const RepositoryIFCRef& cimServer)
	: RepositoryIFC()
	, m_inst(inst)
	, m_pCIMServer(cimServer)
{
}
///////////////////////////////////////////////////////////////////////////////
ServiceEnvironmentIFCRef
WQLFilterRep::getEnvironment() const
{
	return m_pCIMServer->getEnvironment();
}
///////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::enumNameSpace(
	StringResultHandlerIFC&,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::createNameSpace(const String& /*ns*/,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
///////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::deleteNameSpace(const String& /*ns*/,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
CIMQualifierType
WQLFilterRep::getQualifierType(const String&,
	const String&,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
///////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::setQualifierType(const String &/*ns*/,
	const CIMQualifierType &/*qt*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
///////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::enumQualifierTypes(
	const String&,
	CIMQualifierTypeResultHandlerIFC&, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
///////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::deleteQualifierType(const String& /*ns*/, const String& /*qualName*/,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::createClass(const String &/*path*/,
	const CIMClass &/*cimClass*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
WQLFilterRep::modifyClass(const String &/*name*/,
	const CIMClass &/*cc*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
WQLFilterRep::deleteClass(const String& /*ns*/, const String& /*className*/,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
WQLFilterRep::getClass(
	const String& ns,
	const String& className,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
	const UserInfo& ai)
{
	return m_pCIMServer->getClass(ns, className, localOnly, includeQualifiers, includeClassOrigin, propertyList,
		ai);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
WQLFilterRep::getInstance(const String&, const CIMObjectPath&,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
	const UserInfo&)
{
	(void)localOnly; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList;
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::enumClasses(const String& ns,
	const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep, 
	ELocalOnlyFlag localOnly, 
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const UserInfo& aclInfo)
{
	(void)ns; (void)className; (void)result; (void)deep; (void)localOnly; (void)includeQualifiers; (void)includeClassOrigin; (void)aclInfo;
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::enumClassNames(
	const String&,
	const String&,
	StringResultHandlerIFC&,
	EDeepFlag /*deep*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, EEnumSubclassesFlag enumSubclasses, const UserInfo& aclInfo)
{
	String superClassName = m_inst.getClassName();
	while (!superClassName.empty())
	{
		if (superClassName.equalsIgnoreCase(className))
		{
			// Don't need to do correct localOnly & deep processing.
			//result.handleInstance(m_inst.clone(localOnly, includeQualifiers,
			//	includeClassOrigin, propertyList));
			(void)deep; (void)localOnly; (void)includeQualifiers;
			(void)includeClassOrigin; (void)propertyList; (void)enumSubclasses;
			// This is more efficient
			result.handle(m_inst);
			break;
		}
		
		superClassName = m_pCIMServer->getClass(ns, superClassName,
			E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN,
			NULL, aclInfo).getSuperClass();
	}
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::enumInstanceNames(
	const String&,
	const String&,
	CIMObjectPathResultHandlerIFC&,
	EDeepFlag deep,
	const UserInfo& aclInfo)
{
	(void)deep; (void)aclInfo;
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
WQLFilterRep::createInstance(const String&,
	const CIMInstance &/*ci*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
WQLFilterRep::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	const UserInfo& aclInfo)
{
	(void)ns; (void)modifiedInstance; (void)includeQualifiers; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
WQLFilterRep::deleteInstance(const String& /*ns*/, const CIMObjectPath &/*cop*/,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::setProperty(
	const String&,
	const CIMObjectPath &/*name*/,
	const String& /*propertyName*/, const CIMValue &/*cv*/,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
WQLFilterRep::getProperty(
	const String&,
	const CIMObjectPath &/*name*/,
	const String &/*propertyName*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
WQLFilterRep::invokeMethod(const String&, const CIMObjectPath &/*name*/,
	const String &/*methodName*/, const CIMParamValueArray &/*inParams*/,
	CIMParamValueArray &/*outParams*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::associators(
	const String& ns,
	const CIMObjectPath &path,
	CIMInstanceResultHandlerIFC& result,
	const String &assocClass, const String &resultClass,
	const String &role, const String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const UserInfo& aclInfo)
{
	(void)ns; (void)path; (void)result; (void)assocClass; (void)resultClass; (void)role; (void)resultRole; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::associatorsClasses(
	const String& ns,
	const CIMObjectPath &path,
	CIMClassResultHandlerIFC& result,
	const String &assocClass, const String &resultClass,
	const String &role, const String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const UserInfo& aclInfo)
{
	(void)ns; (void)path; (void)result; (void)assocClass; (void)resultClass; (void)role; (void)resultRole; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::referenceNames(
	const String&,
	const CIMObjectPath &/*path*/,
	CIMObjectPathResultHandlerIFC&,
	const String &/*resultClass*/,
	const String &/*role*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::associatorNames(
	const String&,
	const CIMObjectPath &/*path*/,
	CIMObjectPathResultHandlerIFC&,
	const String &/*assocClass*/,
	const String &/*resultClass*/, const String &/*role*/,
	const String &/*resultRole*/, const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::references(
	const String& ns,
	const CIMObjectPath &path,
	CIMInstanceResultHandlerIFC& result,
	const String &resultClass, const String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const UserInfo& aclInfo)
{
	(void)ns; (void)path; (void)result; (void)resultClass; (void)role; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::referencesClasses(
	const String& ns,
	const CIMObjectPath &path,
	CIMClassResultHandlerIFC& result,
	const String &resultClass, const String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const UserInfo& aclInfo)
{
	(void)ns; (void)path; (void)result; (void)resultClass; (void)role; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::execQuery(const String& /*ns*/,
	CIMInstanceResultHandlerIFC&,
	const String &/*query*/, const String &/*queryLanguage*/,
	const UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::getSchemaReadLock()
{
	m_pCIMServer->getSchemaReadLock();
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::getSchemaWriteLock()
{
	m_pCIMServer->getSchemaWriteLock();
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::releaseSchemaReadLock()
{
	m_pCIMServer->releaseSchemaReadLock();
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::releaseSchemaWriteLock()
{
	return m_pCIMServer->releaseSchemaWriteLock();
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::getInstanceReadLock()
{
	m_pCIMServer->getInstanceReadLock();
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::getInstanceWriteLock()
{
	m_pCIMServer->getInstanceWriteLock();
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::releaseInstanceReadLock()
{
	m_pCIMServer->releaseInstanceReadLock();
}
//////////////////////////////////////////////////////////////////////////////
void
WQLFilterRep::releaseInstanceWriteLock()
{
	return m_pCIMServer->releaseInstanceWriteLock();
}

} // end namespace OpenWBEM

