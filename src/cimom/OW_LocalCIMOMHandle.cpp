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

#include "OW_config.h"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_StringStream.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_Format.hpp"
#include "OW_DateTime.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMObjectPath.hpp"

#include <iostream>

//////////////////////////////////////////////////////////////////////////////
OW_LocalCIMOMHandle::OW_LocalCIMOMHandle(OW_CIMOMEnvironmentRef env,
	OW_RepositoryIFCRef pRepos, const OW_ACLInfo& aclInfo)
	: OW_CIMOMHandleIFC()
	, m_pServer(pRepos)
	, m_aclInfo(aclInfo)
	, m_noLock(false)
	, m_env(env)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_LocalCIMOMHandle::OW_LocalCIMOMHandle(const OW_LocalCIMOMHandle& arg)
	: OW_CIMOMHandleIFC()
	, m_pServer(arg.m_pServer)
	, m_aclInfo(arg.m_aclInfo)
	, m_noLock(arg.m_noLock)
	, m_env(arg.m_env)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_LocalCIMOMHandle::OW_LocalCIMOMHandle(OW_CIMOMEnvironmentRef env,
	OW_RepositoryIFCRef pRepos, const OW_ACLInfo& aclInfo, OW_Bool noLock)
	: OW_CIMOMHandleIFC()
	, m_pServer(pRepos)
	, m_aclInfo(aclInfo)
	, m_noLock(noLock)
	, m_env(env)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_LocalCIMOMHandle&
OW_LocalCIMOMHandle::operator= (const OW_LocalCIMOMHandle& arg)
{
	m_pServer = arg.m_pServer;
	m_aclInfo = arg.m_aclInfo;
	m_noLock = arg.m_noLock;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::createNameSpace(const OW_CIMNameSpace& ns)
{
	OW_WriteLock wl = getWriteLock();
	m_pServer->createNameSpace(ns, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::close()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::deleteNameSpace(const OW_CIMNameSpace& ns)
{
	OW_WriteLock wl = getWriteLock();
	m_pServer->deleteNameSpace(ns, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::deleteClass(const OW_CIMObjectPath& path)
{
	OW_WriteLock wl = getWriteLock();
	m_pServer->deleteClass(path, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::deleteInstance(const OW_CIMObjectPath& path)
{
	OW_WriteLock wl = getWriteLock();
	m_pServer->deleteInstance(path, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::deleteQualifierType(const OW_CIMObjectPath& path)
{
	OW_WriteLock wl = getWriteLock();
	m_pServer->deleteQualifierType(path, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumNameSpace(const OW_CIMNameSpace& ns,
	OW_StringResultHandlerIFC& result, OW_Bool deep)
{
	OW_ReadLock rl = getReadLock();
	m_pServer->enumNameSpace(ns, result, deep, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumClass(const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	OW_ReadLock rl = getReadLock();
	m_pServer->enumClasses(path, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumClassNames(const OW_CIMObjectPath& path,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep)
{
	OW_ReadLock rl = getReadLock();
	m_pServer->enumClassNames(path, result, deep, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumInstances(const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_ReadLock rl = getReadLock();
	m_pServer->enumInstances(path, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumInstanceNames(const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep)
{
	OW_ReadLock rl = getReadLock();
	return m_pServer->enumInstanceNames(path, result, deep, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumQualifierTypes(const OW_CIMObjectPath& path,
	OW_CIMQualifierTypeResultHandlerIFC& result)
{
	OW_ReadLock rl = getReadLock();
	m_pServer->enumQualifierTypes(path, result, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_LocalCIMOMHandle::getClass(const OW_CIMObjectPath& name, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_ReadLock rl = getReadLock();
	OW_CIMClass cls = m_pServer->getClass(name, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, m_aclInfo);
	return cls;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_LocalCIMOMHandle::getInstance(const OW_CIMObjectPath& name,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_ReadLock rl = getReadLock();
	return m_pServer->getInstance(name, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_LocalCIMOMHandle::invokeMethod(const OW_CIMObjectPath& name,
	const OW_String& methodName, const OW_CIMValueArray& inParams,
	OW_CIMValueArray& outParams)
{
	OW_WriteLock wl = getWriteLock();
	return m_pServer->invokeMethod(name, methodName, inParams, outParams,
		m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_LocalCIMOMHandle::getQualifierType(const OW_CIMObjectPath& name)
{
	OW_ReadLock rl = getReadLock();
	return m_pServer->getQualifierType(name, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::setQualifierType(const OW_CIMObjectPath& name,
	const OW_CIMQualifierType& qt)
{
	OW_WriteLock wl = getWriteLock();
	m_pServer->setQualifierType(name, qt, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::modifyClass(const OW_CIMObjectPath& name,
	const OW_CIMClass& cc)
{
	OW_WriteLock wl = getWriteLock();
	OW_CIMClass lcc(cc);
	m_pServer->modifyClass(name, lcc, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::createClass(const OW_CIMObjectPath& name,
	const OW_CIMClass& cc)
{
	OW_WriteLock wl = getWriteLock();
	OW_CIMClass lcc(cc);
	m_pServer->createClass(name, lcc, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::modifyInstance(const OW_CIMObjectPath& name,
	const OW_CIMInstance& ci)
{
	OW_WriteLock wl = getWriteLock();
	OW_CIMInstance lci(ci);
	m_pServer->modifyInstance(name, lci, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_LocalCIMOMHandle::createInstance(const OW_CIMObjectPath& name,
	const OW_CIMInstance& ci)
{
	OW_WriteLock wl = getWriteLock();

	OW_CIMInstance lci(ci);
	return m_pServer->createInstance(name, lci, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_LocalCIMOMHandle::getProperty(const OW_CIMObjectPath& name,
	const OW_String& propertyName)
{
	OW_ReadLock rl = getReadLock();
	return m_pServer->getProperty(name, propertyName, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::setProperty(const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& cv)
{
	OW_WriteLock wl = getWriteLock();
	m_pServer->setProperty(name, propertyName, cv, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_LocalCIMOMHandle::associatorNames(const OW_CIMObjectPath& path,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole)
{
	OW_ReadLock rl = getReadLock();
	return m_pServer->associatorNames(path, assocClass, resultClass, role,
		resultRole, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_LocalCIMOMHandle::associators(const OW_CIMObjectPath& path,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_ReadLock rl = getReadLock();
	return m_pServer->associators(path, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_LocalCIMOMHandle::referenceNames(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role)
{
	OW_ReadLock rl = getReadLock();
	return m_pServer->referenceNames(path, resultClass, role, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_LocalCIMOMHandle::references(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_ReadLock rl = getReadLock();
	return m_pServer->references(path, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_LocalCIMOMHandle::execQuery(const OW_CIMNameSpace& ns, const OW_String& query,
	const OW_String& queryLanguage)
{
	OW_WriteLock wl = getWriteLock();
	return m_pServer->execQuery(ns, query, queryLanguage, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_LocalCIMOMHandle::getServerFeatures()
{
	OW_CIMFeatures cf;
	cf.protocolVersion = "1.0";
	cf.cimom = "/cimom";
	cf.cimProduct = OW_CIMFeatures::SERVER;
	cf.extURL = "http://www.dmtf.org/cim/mapping/http/v1.0";
	cf.supportedGroups.push_back("basic-read");
	cf.supportedGroups.push_back("basic-write");
	cf.supportedGroups.push_back("schema-manipulation");
	cf.supportedGroups.push_back("instance-manipulation");
	cf.supportedGroups.push_back("qualifier-declaration");
	cf.supportedGroups.push_back("association-traversal");
	cf.supportedQueryLanguages.clear();
	cf.supportsBatch = true;
	cf.validation = "";
	return cf;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::exportIndication(const OW_CIMInstance& instance,
	const OW_CIMNameSpace& instNS)
{
	m_env->exportIndication(instance, instNS);
}

//////////////////////////////////////////////////////////////////////////////
OW_ReadLock
OW_LocalCIMOMHandle::getReadLock()
{
	OW_ReadLock rl;
	if(!m_noLock)
	{
		rl = m_pServer->getReadLock();
	}

	return rl;
}

//////////////////////////////////////////////////////////////////////////////
OW_WriteLock
OW_LocalCIMOMHandle::getWriteLock()
{
	OW_WriteLock wl;
	if(!m_noLock)
	{
		wl = m_pServer->getWriteLock();
	}

	return wl;
}

