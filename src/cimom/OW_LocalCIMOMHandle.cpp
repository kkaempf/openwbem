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
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"

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
class OW_CIMServerSchemaReadLocker
{
public:
	OW_CIMServerSchemaReadLocker(OW_LocalCIMOMHandle* pHdl)
	: m_pHdl(pHdl) 
	{
		m_pHdl->getSchemaReadLock();
	}
	~OW_CIMServerSchemaReadLocker()
	{
		m_pHdl->releaseSchemaReadLock();
	}
private:
	OW_LocalCIMOMHandle* m_pHdl;
};

//////////////////////////////////////////////////////////////////////////////
class OW_CIMServerSchemaWriteLocker
{
public:
	OW_CIMServerSchemaWriteLocker(OW_LocalCIMOMHandle* pHdl)
	: m_pHdl(pHdl) 
	{
		m_pHdl->getSchemaWriteLock();
	}
	~OW_CIMServerSchemaWriteLocker()
	{
		m_pHdl->releaseSchemaWriteLock();
	}
private:
	OW_LocalCIMOMHandle* m_pHdl;
};

//////////////////////////////////////////////////////////////////////////////
class OW_CIMServerInstanceReadLocker
{
public:
	OW_CIMServerInstanceReadLocker(OW_LocalCIMOMHandle* pHdl)
	: m_pHdl(pHdl) 
	{
		m_pHdl->getInstanceReadLock();
	}
	~OW_CIMServerInstanceReadLocker()
	{
		m_pHdl->releaseInstanceReadLock();
	}
private:
	OW_LocalCIMOMHandle* m_pHdl;
};

//////////////////////////////////////////////////////////////////////////////
class OW_CIMServerInstanceWriteLocker
{
public:
	OW_CIMServerInstanceWriteLocker(OW_LocalCIMOMHandle* pHdl)
	: m_pHdl(pHdl) 
	{
		m_pHdl->getInstanceWriteLock();
	}
	~OW_CIMServerInstanceWriteLocker()
	{
		m_pHdl->releaseInstanceWriteLock();
	}
private:
	OW_LocalCIMOMHandle* m_pHdl;
};

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::createNameSpace(const OW_String& ns)
{
	OW_CIMServerSchemaWriteLocker swl(this);
	OW_CIMServerInstanceWriteLocker iwl(this);
	m_pServer->createNameSpace(ns, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::close()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::deleteNameSpace(const OW_String& ns)
{
	OW_CIMServerSchemaWriteLocker swl(this);
	OW_CIMServerInstanceWriteLocker iwl(this);
	m_pServer->deleteNameSpace(ns, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::deleteClass(const OW_String& ns, const OW_String& className)
{
	OW_CIMServerSchemaWriteLocker swl(this);
	OW_CIMServerInstanceWriteLocker iwl(this);
	m_pServer->deleteClass(ns, className, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& path)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceWriteLocker iwl(this);
	m_pServer->deleteInstance(ns, path, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::deleteQualifierType(const OW_String& ns, const OW_String& qualName)
{
	OW_CIMServerSchemaWriteLocker wl(this);
	m_pServer->deleteQualifierType(ns, qualName, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumNameSpace(const OW_String& ns,
	OW_StringResultHandlerIFC& result, OW_Bool deep)
{
	OW_CIMServerSchemaReadLocker rl(this);
	m_pServer->enumNameSpace(ns, result, deep, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumClass(const OW_String& ns,
	const OW_String& className,
	OW_CIMClassResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	OW_CIMServerSchemaReadLocker rl(this);
	m_pServer->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumClassNames(const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep)
{
	OW_CIMServerSchemaReadLocker rl(this);
	m_pServer->enumClassNames(ns, className, result, deep, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceReadLocker irl(this);
	m_pServer->enumInstances(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, true, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumInstanceNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceReadLocker irl(this);
	return m_pServer->enumInstanceNames(ns, className, result, true, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result)
{
	OW_CIMServerSchemaReadLocker srl(this);
	m_pServer->enumQualifierTypes(ns, result, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_LocalCIMOMHandle::getClass(
	const OW_String& ns,
	const OW_String& className,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMClass cls = m_pServer->getClass(ns, className, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, m_aclInfo);
	return cls;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_LocalCIMOMHandle::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceReadLocker irl(this);
	return m_pServer->getInstance(ns, instanceName, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_LocalCIMOMHandle::invokeMethod(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	const OW_String& methodName, const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams)
{
	// Don't know anything about locking for invokeMethod, so we have
	// to assume the worst.  OW_CIMServer will do a getClass and possibly
	// getInstance, we we need at least read locks.  But who knows what
	// the provider will do, so just get dual write locks.
	OW_CIMServerSchemaWriteLocker srl(this);
	OW_CIMServerInstanceWriteLocker irl(this);
	return m_pServer->invokeMethod(ns, path, methodName, inParams, outParams,
		m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_LocalCIMOMHandle::getQualifierType(const OW_String& ns,
		const OW_String& qualifierName)
{
	OW_CIMServerSchemaReadLocker rl(this);
	return m_pServer->getQualifierType(ns, qualifierName, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::setQualifierType(const OW_String& ns,
	const OW_CIMQualifierType& qt)
{
	OW_CIMServerSchemaWriteLocker wl(this);
	m_pServer->setQualifierType(ns, qt, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::modifyClass(
	const OW_String& ns,
	const OW_CIMClass& cc)
{
	// modify class will modify the class as well as any instances of that class
	OW_CIMServerSchemaWriteLocker swl(this);
	OW_CIMServerInstanceWriteLocker iwl(this);
	m_pServer->modifyClass(ns, cc, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::createClass(const OW_String& ns,
	const OW_CIMClass& cc)
{
	OW_CIMServerSchemaWriteLocker wl(this);
	m_pServer->createClass(ns, cc, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	const OW_StringArray* propertyList)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceWriteLocker iwl(this);
	m_pServer->modifyInstance(ns, modifiedInstance, includeQualifiers,
		propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_LocalCIMOMHandle::createInstance(const OW_String& ns,
	const OW_CIMInstance& ci)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceWriteLocker iwl(this);
	return m_pServer->createInstance(ns, ci, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_LocalCIMOMHandle::getProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceReadLocker irl(this);
	return m_pServer->getProperty(ns, name, propertyName, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& cv)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceWriteLocker iwl(this);
	m_pServer->setProperty(ns, name, propertyName, cv, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::associatorNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceReadLocker irl(this);
	m_pServer->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::associators(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceReadLocker irl(this);
	m_pServer->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::associatorsClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_CIMServerSchemaReadLocker srl(this);
	m_pServer->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList,
		m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::referenceNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceReadLocker irl(this);
	m_pServer->referenceNames(ns, path, result, resultClass, role, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::references(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceReadLocker irl(this);
	m_pServer->references(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::referencesClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_CIMServerSchemaReadLocker srl(this);
	m_pServer->referencesClasses(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, m_aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::execQuery(
	const OW_String& ns,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& query,
	const OW_String& queryLanguage)
{
	OW_CIMServerSchemaReadLocker srl(this);
	OW_CIMServerInstanceWriteLocker irl(this);
	m_pServer->execQuery(ns, result, query, queryLanguage, m_aclInfo);
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
	cf.validation = OW_String();
	return cf;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::exportIndication(const OW_CIMInstance& instance,
	const OW_String& instNS)
{
	m_env->exportIndication(instance, instNS);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::getSchemaReadLock()
{
	if(!m_noLock)
	{
		m_pServer->getSchemaReadLock();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::getSchemaWriteLock()
{
	if(!m_noLock)
	{
		m_pServer->getSchemaWriteLock();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::releaseSchemaReadLock()
{
	if(!m_noLock)
	{
		m_pServer->releaseSchemaReadLock();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::releaseSchemaWriteLock()
{
	if(!m_noLock)
	{
		m_pServer->releaseSchemaWriteLock();
	}
}
//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::getInstanceReadLock()
{
	if(!m_noLock)
	{
		m_pServer->getInstanceReadLock();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::getInstanceWriteLock()
{
	if(!m_noLock)
	{
		m_pServer->getInstanceWriteLock();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::releaseInstanceReadLock()
{
	if(!m_noLock)
	{
		m_pServer->releaseInstanceReadLock();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_LocalCIMOMHandle::releaseInstanceWriteLock()
{
	if(!m_noLock)
	{
		m_pServer->releaseInstanceWriteLock();
	}
}

