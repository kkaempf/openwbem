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

using namespace OW_WBEMFlags;
///////////////////////////////////////////////////////////////////////////////
OW_WQLFilterRep::OW_WQLFilterRep(const OW_CIMInstance& inst,
	const OW_RepositoryIFCRef& cimServer)
	: OW_RepositoryIFC()
	, m_inst(inst)
	, m_pCIMServer(cimServer)
{
}

///////////////////////////////////////////////////////////////////////////////
OW_ServiceEnvironmentIFCRef
OW_WQLFilterRep::getEnvironment() const
{
	return m_pCIMServer->getEnvironment();
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::enumNameSpace(
	OW_StringResultHandlerIFC&,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::createNameSpace(const OW_String& /*ns*/,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::deleteNameSpace(const OW_String& /*ns*/,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

///////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_WQLFilterRep::getQualifierType(const OW_String&,
	const OW_String&,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::setQualifierType(const OW_String &/*ns*/,
	const OW_CIMQualifierType &/*qt*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::enumQualifierTypes(
	const OW_String&,
	OW_CIMQualifierTypeResultHandlerIFC&, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::deleteQualifierType(const OW_String& /*ns*/, const OW_String& /*qualName*/,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::createClass(const OW_String &/*path*/,
	const OW_CIMClass &/*cimClass*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_WQLFilterRep::modifyClass(const OW_String &/*name*/,
	const OW_CIMClass &/*cc*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_WQLFilterRep::deleteClass(const OW_String& /*ns*/, const OW_String& /*className*/,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_WQLFilterRep::getClass(
	const OW_String& ns,
	const OW_String& className,
	OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OW_StringArray* propertyList,
	const OW_UserInfo& ai)
{
	return m_pCIMServer->getClass(ns, className, localOnly, includeQualifiers, includeClassOrigin, propertyList,
		ai);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_WQLFilterRep::getInstance(const OW_String&, const OW_CIMObjectPath&,
	OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OW_StringArray* propertyList,
	const OW_UserInfo&)
{
	(void)localOnly; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList;
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::enumClasses(const OW_String& ns,
	const OW_String& className,
	OW_CIMClassResultHandlerIFC& result,
	OW_WBEMFlags::EDeepFlag deep, 
	OW_WBEMFlags::ELocalOnlyFlag localOnly, 
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OW_UserInfo& aclInfo)
{
	(void)ns; (void)className; (void)result; (void)deep; (void)localOnly; (void)includeQualifiers; (void)includeClassOrigin; (void)aclInfo;
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::enumClassNames(
	const OW_String&,
	const OW_String&,
	OW_StringResultHandlerIFC&,
	OW_WBEMFlags::EDeepFlag /*deep*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result,
	OW_WBEMFlags::EDeepFlag deep,
	OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray* propertyList, OW_Bool enumSubClasses, const OW_UserInfo& aclInfo)
{
	OW_String superClassName = m_inst.getClassName();

	while (!superClassName.empty())
	{
		if (superClassName.equalsIgnoreCase(className))
		{
			// Don't need to do correct localOnly & deep processing.
			//result.handleInstance(m_inst.clone(localOnly, includeQualifiers,
			//	includeClassOrigin, propertyList));
			(void)deep; (void)localOnly; (void)includeQualifiers;
			(void)includeClassOrigin; (void)propertyList; (void)enumSubClasses;
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
OW_WQLFilterRep::enumInstanceNames(
	const OW_String&,
	const OW_String&,
	OW_CIMObjectPathResultHandlerIFC&,
	OW_WBEMFlags::EDeepFlag deep,
	const OW_UserInfo& aclInfo)
{
	(void)deep; (void)aclInfo;
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_WQLFilterRep::createInstance(const OW_String&,
	const OW_CIMInstance &/*ci*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_WQLFilterRep::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	const OW_StringArray* propertyList,
	const OW_UserInfo& aclInfo)
{
	(void)ns; (void)modifiedInstance; (void)includeQualifiers; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_WQLFilterRep::deleteInstance(const OW_String& /*ns*/, const OW_CIMObjectPath &/*cop*/,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::setProperty(
	const OW_String&,
	const OW_CIMObjectPath &/*name*/,
	const OW_String& /*propertyName*/, const OW_CIMValue &/*cv*/,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_WQLFilterRep::getProperty(
	const OW_String&,
	const OW_CIMObjectPath &/*name*/,
	const OW_String &/*propertyName*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_WQLFilterRep::invokeMethod(const OW_String&, const OW_CIMObjectPath &/*name*/,
	const OW_String &/*methodName*/, const OW_CIMParamValueArray &/*inParams*/,
	OW_CIMParamValueArray &/*outParams*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::associators(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &assocClass, const OW_String &resultClass,
	const OW_String &role, const OW_String &resultRole,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, const OW_UserInfo& aclInfo)
{
	(void)ns; (void)path; (void)result; (void)assocClass; (void)resultClass; (void)role; (void)resultRole; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::associatorsClasses(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String &assocClass, const OW_String &resultClass,
	const OW_String &role, const OW_String &resultRole,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, const OW_UserInfo& aclInfo)
{
	(void)ns; (void)path; (void)result; (void)assocClass; (void)resultClass; (void)role; (void)resultRole; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::referenceNames(
	const OW_String&,
	const OW_CIMObjectPath &/*path*/,
	OW_CIMObjectPathResultHandlerIFC&,
	const OW_String &/*resultClass*/,
	const OW_String &/*role*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::associatorNames(
	const OW_String&,
	const OW_CIMObjectPath &/*path*/,
	OW_CIMObjectPathResultHandlerIFC&,
	const OW_String &/*assocClass*/,
	const OW_String &/*resultClass*/, const OW_String &/*role*/,
	const OW_String &/*resultRole*/, const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::references(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &resultClass, const OW_String &role,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, const OW_UserInfo& aclInfo)
{
	(void)ns; (void)path; (void)result; (void)resultClass; (void)role; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::referencesClasses(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String &resultClass, const OW_String &role,
	OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const OW_StringArray *propertyList, const OW_UserInfo& aclInfo)
{
	(void)ns; (void)path; (void)result; (void)resultClass; (void)role; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)aclInfo;
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::execQuery(const OW_String& /*ns*/,
	OW_CIMInstanceResultHandlerIFC&,
	const OW_String &/*query*/, const OW_String &/*queryLanguage*/,
	const OW_UserInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::getSchemaReadLock()
{
	m_pCIMServer->getSchemaReadLock();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::getSchemaWriteLock()
{
	m_pCIMServer->getSchemaWriteLock();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::releaseSchemaReadLock()
{
	m_pCIMServer->releaseSchemaReadLock();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::releaseSchemaWriteLock()
{
	return m_pCIMServer->releaseSchemaWriteLock();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::getInstanceReadLock()
{
	m_pCIMServer->getInstanceReadLock();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::getInstanceWriteLock()
{
	m_pCIMServer->getInstanceWriteLock();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::releaseInstanceReadLock()
{
	m_pCIMServer->releaseInstanceReadLock();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::releaseInstanceWriteLock()
{
	return m_pCIMServer->releaseInstanceWriteLock();
}


