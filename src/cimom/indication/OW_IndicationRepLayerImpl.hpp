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

#ifndef OW_INDICATION_REP_LAYER_IMPL_HPP_
#define OW_INDICATION_REP_LAYER_IMPL_HPP_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_RepositoryIFC.hpp"
#include "OW_IndicationRepLayer.hpp"
#include "OW_CIMServer.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifierType.hpp"

class OW_IndicationRepLayerImpl : public OW_IndicationRepLayer
{
public:
	OW_IndicationRepLayerImpl() : OW_IndicationRepLayer(), m_pServer(0) {}

	virtual OW_CIMClass deleteClass(const OW_String& ns, const OW_String& className,
		const OW_ACLInfo& aclInfo);

	virtual void open(const OW_String&) {}

	virtual void close() {}

	virtual OW_CIMOMEnvironmentRef getEnvironment() const
	{
		return m_pEnv;
	}

	virtual void deleteQualifierType(const OW_String& ns, const OW_String& qualName,
		const OW_ACLInfo& aclInfo)
	{
		m_pServer->deleteQualifierType(ns, qualName, aclInfo);
	}


	virtual void enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_ACLInfo& aclInfo)
	{
		m_pServer->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
			includeClassOrigin, aclInfo);
	}


	virtual void enumClassNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo& aclInfo)
	{
		m_pServer->enumClassNames(ns, className, result, deep, aclInfo);
	}


	virtual OW_CIMInstance deleteInstance(const OW_String& ns, const OW_CIMObjectPath &path,
		const OW_ACLInfo& aclInfo);

	virtual void enumQualifierTypes(
		const OW_String& ns,
		OW_CIMQualifierTypeResultHandlerIFC& result, const OW_ACLInfo& aclInfo)
	{
		m_pServer->enumQualifierTypes(ns, result, aclInfo);
	}


	virtual OW_CIMInstance getInstance(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	virtual void enumInstances(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, OW_Bool enumSubClasses, 
		const OW_ACLInfo& aclInfo)
	{
		m_pServer->enumInstances(ns, className, result, deep, localOnly, includeQualifiers,
			includeClassOrigin, propertyList, enumSubClasses, aclInfo);
	}


	virtual void enumInstanceNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo& aclInfo)
	{
		return m_pServer->enumInstanceNames(ns, className, result, deep, aclInfo);
	}


	virtual OW_CIMQualifierType getQualifierType(
		const OW_String& ns,
		const OW_String& qualifierName, const OW_ACLInfo& aclInfo)
	{
		return m_pServer->getQualifierType(ns, qualifierName, aclInfo);
	}


	virtual OW_CIMClass getClass(
		const OW_String& ns,
		const OW_String& className,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo)
	{
		return m_pServer->getClass(ns, className, localOnly, includeQualifiers,
			includeClassOrigin, propertyList, aclInfo);
	}


	virtual OW_CIMValue invokeMethod(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String &methodName, const OW_CIMParamValueArray &inParams,
		OW_CIMParamValueArray &outParams, const OW_ACLInfo& aclInfo);

	virtual void createClass(const OW_String& ns,
		const OW_CIMClass &cc, const OW_ACLInfo& aclInfo);

	virtual void setQualifierType(const OW_String& ns,
		const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo)
	{
		m_pServer->setQualifierType(ns, qt, aclInfo);
	}


	virtual OW_CIMObjectPath createInstance(const OW_String& ns,
		const OW_CIMInstance &ci, const OW_ACLInfo& aclInfo);

	virtual OW_CIMClass modifyClass(const OW_String &ns,
		const OW_CIMClass& cc, const OW_ACLInfo& aclInfo);

	virtual void setProperty(
		const OW_String& ns,
		const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_CIMValue &cv,
		const OW_ACLInfo& aclInfo)
	{
		m_pServer->setProperty(ns, name, propertyName, cv, aclInfo);
	}


	virtual OW_CIMInstance modifyInstance(
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		OW_Bool includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	virtual void associators(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
	{
		m_pServer->associators(ns, path, result, assocClass, resultClass, role,
			resultRole, includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	}


	virtual void associatorsClasses(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
	{
		m_pServer->associatorsClasses(ns, path, result, assocClass, resultClass, role,
			resultRole, includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	}


	virtual OW_CIMValue getProperty(
		const OW_String& ns,
		const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_ACLInfo& aclInfo)
	{
		return m_pServer->getProperty(ns, name, propertyName, aclInfo);
	}


	virtual void references(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
	{
		m_pServer->references(ns, path, result, resultClass, role,
			includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	}


	virtual void referencesClasses(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
	{
		m_pServer->referencesClasses(ns, path, result, resultClass, role,
			includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	}


	virtual void associatorNames(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &assocClass,
		const OW_String &resultClass, const OW_String &role,
		const OW_String &resultRole, const OW_ACLInfo& aclInfo)
	{
		m_pServer->associatorNames(ns, path, result, assocClass, resultClass, role,
			resultRole, aclInfo);
	}


	virtual void referenceNames(
		const OW_String& ns,
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &resultClass,
		const OW_String &role, const OW_ACLInfo& aclInfo)
	{
		m_pServer->referenceNames(ns, path, result, resultClass, role, aclInfo);
	}

	
	virtual void execQuery(
		const OW_String& ns,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &query, const OW_String& queryLanguage,
		const OW_ACLInfo& aclInfo)
	{
		m_pServer->execQuery(ns, result, query, queryLanguage, aclInfo);
	}


	/**
	 * Delete a specified namespace.
	 * @param ns	The namespace to delete.
	 * @exception OW_CIMException If the namespace does not exist.
	 */
	void deleteNameSpace(const OW_String &ns, const OW_ACLInfo& aclInfo)
	{
		m_pServer->deleteNameSpace(ns, aclInfo);
	}


	/**
	 * Create a cim namespace.
	 * @param ns 	The namespace to be created.
	 * @exception OW_CIMException If the namespace already exists.
	 */
	void createNameSpace(const OW_String& ns, const OW_ACLInfo& aclInfo)
	{
		m_pServer->createNameSpace(ns, aclInfo);
	}


	/**
	 * Gets a list of the namespaces.
	 * @return An Array of namespace names as strings.
	 * @exception 	OW_CIMException If the namespace does not exist or the object
	 *					cannot be found in the specified namespace.
	 */
	void enumNameSpace(OW_StringResultHandlerIFC& result,
		const OW_ACLInfo& aclInfo)
	{
		m_pServer->enumNameSpace(result, aclInfo);
	}


	virtual void getSchemaReadLock()
	{
		m_pServer->getSchemaReadLock();
	}

	virtual void getSchemaWriteLock()
	{
		m_pServer->getSchemaWriteLock();
	}

	virtual void releaseSchemaReadLock()
	{
		m_pServer->releaseSchemaReadLock();
	}

	virtual void releaseSchemaWriteLock()
	{
		m_pServer->releaseSchemaWriteLock();
	}

	virtual void getInstanceReadLock()
	{
		m_pServer->getInstanceReadLock();
	}

	virtual void getInstanceWriteLock()
	{
		m_pServer->getInstanceWriteLock();
	}

	virtual void releaseInstanceReadLock()
	{
		m_pServer->releaseInstanceReadLock();
	}

	virtual void releaseInstanceWriteLock()
	{
		m_pServer->releaseInstanceWriteLock();
	}


private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (m_pServer) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (m_pServer) ? 0: &dummy::nonnull; }

	virtual void setCIMServer(const OW_RepositoryIFCRef& src)
	{
		m_pServer = src;
		m_pEnv = m_pServer->getEnvironment();
	}



private:

	void exportIndication(const OW_CIMInstance& instance,
		const OW_String& instNS)
	{
		m_pEnv->exportIndication(instance, instNS);
	}


	OW_RepositoryIFCRef m_pServer;
	OW_CIMOMEnvironmentRef m_pEnv;
};

#endif


