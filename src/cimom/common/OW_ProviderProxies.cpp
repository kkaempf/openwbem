/*******************************************************************************
* Copyright (C) 2004 Novell, Inc., Vintela, Inc. All rights reserved.
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
*  - Neither the name of Novell nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Jon Carey
 */

#include "OW_config.h"
#ifdef OW_THREADS_RUN_AS_USER

#include "OW_ProviderProxies.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_CIMValue.hpp"
#include "OW_Logger.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_OperationContext.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMFeatures.hpp"

#ifdef OW_GNU_LINUX
	#ifdef OW_HAVE_PWD_H
	#include <pwd.h>
	#endif
	#ifdef OW_HAVE_UNISTD_H
	#include <unistd.h>
	#endif
	#ifdef OW_HAVE_SYS_TYPES_H
	#include <sys/types.h>
	#endif
#endif

namespace OpenWBEM
{

using namespace WBEMFlags;

#if defined(OW_GNU_LINUX)
#define RUID_MANAGER_FACTORY RUIDManager um(m_cimomuid, m_useruid)
#define UID_MANAGER_FACTORY  UIDManager um(m_useruid, m_cimomuid)
#define PROXY_ENV_FACTORY    proxyEnv(env, m_cimomuid, m_useruid)
#elif defined(OW_NETWARE)
#define RUID_MANAGER_FACTORY RUIDManager um(m_nwi)
#define UID_MANAGER_FACTORY  UIDManager um(m_nwi)
#define PROXY_ENV_FACTORY    proxyEnv(env, m_nwi)
#endif

namespace
{
#if defined (OW_GNU_LINUX)
	class UIDManager
	{
	public:
		UIDManager(uid_t tempUid, uid_t resetUid)
			: m_resetUid(resetUid)
			, m_uidsDiffer(tempUid != resetUid)
		{
			if (m_uidsDiffer)
			{
				::seteuid(tempUid);
			}
		}

		~UIDManager()
		{
			if (m_uidsDiffer)
			{
				::setuid(m_resetUid);
			}
		}
		
	private:
		uid_t m_resetUid;
		bool m_uidsDiffer;
	};

	class RUIDManager
	{
	public:
		RUIDManager(uid_t tempUid, uid_t resetUid)
			: m_resetUid(resetUid)
			, m_uidsDiffer(tempUid != resetUid)
		{
			if (m_uidsDiffer)
			{
				::setuid(tempUid);
			}
		}

		~RUIDManager()
		{
			if (m_uidsDiffer)
			{
				::seteuid(m_resetUid);
			}
		}
		
	private:
		uid_t m_resetUid;
		bool m_uidsDiffer;
	};
#elif defined (OW_NETWARE)
	class UIDManager
	{
	public:
		UIDManager(NetwareIdentityRef nwi)
			: m_nwi(nwi)
		{
			if(m_nwi)
			{
				m_nwi->setContextToUser(); 
			}
		}

		~UIDManager()
		{
			if(m_nwi)
			{
				m_nwi->setContextToAdmin(); 
			}
		}
		
	private:
		NetwareIdentityRef m_nwi; 
	};

	class RUIDManager
	{
	public:
		RUIDManager(NetwareIdentityRef nwi)
			: m_nwi(nwi)
		{
			if (m_nwi)
			{
				m_nwi->setContextToAdmin(); 
			}
		}

		~RUIDManager()
		{
			if (m_nwi)
			{
				m_nwi->setContextToUser(); 
			}
		}
		
	private:
		NetwareIdentityRef m_nwi; 
	};
#endif


	class ProxyCIMOMHandle : public CIMOMHandleIFC
	{
	public:

#if defined(OW_GNU_LINUX)
		ProxyCIMOMHandle(CIMOMHandleIFCRef ch, uid_t cimomuid, uid_t useruid)
			: CIMOMHandleIFC()
			, m_ch(ch)
			, m_cimomuid(cimomuid)
			, m_useruid(useruid)
#elif defined (OW_NETWARE)
		ProxyCIMOMHandle(CIMOMHandleIFCRef ch, NetwareIdentityRef nwi)
			: CIMOMHandleIFC()
			, m_ch(ch)
			, m_nwi(nwi)
#endif
		{
		}

		virtual void close()
		{
			RUID_MANAGER_FACTORY; 
			m_ch->close();
		}

		virtual void enumClass(const String& ns, const String& className,
			CIMClassResultHandlerIFC& result, EDeepFlag deep,
			ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->enumClass(ns, className, result, deep, localOnly,
				includeQualifiers, includeClassOrigin);
		}

		virtual void enumClassNames(const String& ns, const String& className,
			StringResultHandlerIFC& result, EDeepFlag deep)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->enumClassNames(ns, className, result, deep);
		}

		virtual void enumInstances(const String& ns,
			const String& className, CIMInstanceResultHandlerIFC& result,
			EDeepFlag deep, ELocalOnlyFlag localOnly,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->enumInstances(ns, className, result, deep, localOnly,
				includeQualifiers, includeClassOrigin, propertyList);
		}

		virtual void enumInstanceNames(const String& ns,
			const String& className, CIMObjectPathResultHandlerIFC& result)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->enumInstanceNames(ns, className, result);
		}

		virtual CIMClass getClass(const String& ns, const String& className,
			ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->getClass(ns, className, localOnly,
				includeQualifiers, includeClassOrigin, propertyList);
		}

		virtual CIMInstance getInstance(const String& ns,
			const CIMObjectPath& instanceName, ELocalOnlyFlag localOnly,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->getInstance(ns, instanceName, localOnly,
				includeQualifiers, includeClassOrigin, propertyList);
		}

		virtual CIMValue invokeMethod(const String& ns,
			const CIMObjectPath& path, const String& methodName,
			const CIMParamValueArray& inParams, CIMParamValueArray& outParams)
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->invokeMethod(ns, path, methodName, inParams,
				outParams);
		}

		virtual CIMQualifierType getQualifierType(const String& ns,
			const String& qualifierName)
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->getQualifierType(ns, qualifierName);
		}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION

		virtual void setQualifierType(const String& ns,
			const CIMQualifierType& qualifierType)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->setQualifierType(ns, qualifierType);
		}

		virtual void deleteQualifierType(const String& ns,
			const String& qualName)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->deleteQualifierType(ns, qualName);
		}

		virtual void enumQualifierTypes(const String& ns,
			CIMQualifierTypeResultHandlerIFC& result)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->enumQualifierTypes(ns, result);
		}

		virtual CIMQualifierTypeEnumeration enumQualifierTypesE(
			const String& ns)
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->enumQualifierTypesE(ns);
		}

		virtual CIMQualifierTypeArray enumQualifierTypesA(const String& ns)
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->enumQualifierTypesA(ns);
		}

#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

#ifndef OW_DISABLE_SCHEMA_MANIPULATION

		virtual void modifyClass(const String& ns, const CIMClass& cimClass)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->modifyClass(ns, cimClass);
		}

		virtual void createClass(const String& ns, const CIMClass& cimClass)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->createClass(ns, cimClass);
		}

		virtual void deleteClass(const String& ns, const String& className)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->deleteClass(ns, className);
		}

#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

#ifndef OW_DISABLE_INSTANCE_MANIPULATION

		virtual void modifyInstance(const String& ns,
			const CIMInstance& modifiedInstance,
			EIncludeQualifiersFlag includeQualifiers,
			const StringArray* propertyList)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->modifyInstance(ns, modifiedInstance, includeQualifiers,
				propertyList);
		}

		virtual CIMObjectPath createInstance(const String& ns,
			const CIMInstance& instance)
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->createInstance(ns, instance);
		}

		virtual void deleteInstance(const String& ns,
			const CIMObjectPath& path)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->deleteInstance(ns, path);
		}

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
		virtual void setProperty(const String& ns,
			const CIMObjectPath& instanceName, const String& propertyName,
			const CIMValue& newValue)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->setProperty(ns, instanceName, propertyName, newValue);
		}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)


#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
		virtual CIMValue getProperty(const String& ns,
			const CIMObjectPath& instanceName, const String& propertyName)
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->getProperty(ns, instanceName, propertyName);
		}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

		virtual void associatorNames(const String& ns,
			const CIMObjectPath& objectName,
			CIMObjectPathResultHandlerIFC& result, const String& assocClass,
			const String& resultClass, const String& role,
			const String& resultRole)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->associatorNames(ns, objectName, result, assocClass,
				resultClass, role, resultRole);
		}

		virtual void associators(const String& ns,
			const CIMObjectPath& path, CIMInstanceResultHandlerIFC& result,
			const String& assocClass,
			const String& resultClass,
			const String& role,
			const String& resultRole,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->associators(ns, path, result, assocClass, resultClass,
				role, resultRole, includeQualifiers, includeClassOrigin,
				propertyList);
		}

		virtual void associatorsClasses(const String& ns,
			const CIMObjectPath& path, CIMClassResultHandlerIFC& result,
			const String& assocClass, const String& resultClass,
			const String& role, const String& resultRole,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->associatorsClasses(ns, path, result, assocClass, resultClass,
				role, resultRole, includeQualifiers, includeClassOrigin,
				propertyList);
		}

		virtual void referenceNames(const String& ns, const CIMObjectPath& path,
			CIMObjectPathResultHandlerIFC& result, const String& resultClass,
			const String& role)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->referenceNames(ns, path, result, resultClass, role);
		}

		virtual void references(const String& ns, const CIMObjectPath& path,
			CIMInstanceResultHandlerIFC& result, const String& resultClass,
			const String& role, EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->references(ns, path, result, resultClass, role,
				includeQualifiers, includeClassOrigin, propertyList);
		}

		virtual void referencesClasses(const String& ns,
			const CIMObjectPath& path, CIMClassResultHandlerIFC& result,
			const String& resultClass, const String& role,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->referencesClasses(ns, path, result, resultClass, role,
				includeQualifiers, includeClassOrigin, propertyList);
		}

#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

		virtual void execQuery(const String& ns,
			CIMInstanceResultHandlerIFC& result, const String& query,
			const String& queryLanguage)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->execQuery(ns, result, query, queryLanguage);
		}

		virtual CIMFeatures getServerFeatures()
		{
			RUID_MANAGER_FACTORY; 
			return m_ch->getServerFeatures();
		}

		virtual void exportIndication(const CIMInstance& instance,
			const String& instNS)
		{
			RUID_MANAGER_FACTORY; 
			m_ch->exportIndication(instance, instNS);
		}

	private:

		CIMOMHandleIFCRef m_ch;
#if defined (OW_GNU_LINUX)
		uid_t m_cimomuid;
		uid_t m_useruid;
#elif defined (OW_NETWARE)
		NetwareIdentityRef m_nwi; 
#endif
	};


	class ProxyRepository : public RepositoryIFC
	{
	public:
#if defined (OW_GNU_LINUX)
		ProxyRepository(RepositoryIFCRef prep, uid_t cimomuid, uid_t useruid)
			: RepositoryIFC()
			, m_prep(prep)
			, m_cimomuid(cimomuid)
			, m_useruid(useruid)
#elif defined (OW_NETWARE)
		ProxyRepository(RepositoryIFCRef prep, NetwareIdentityRef nwi)
			: RepositoryIFC()
			, m_prep(prep)
			, m_nwi(nwi)
#endif
		{
		}

		virtual void open(const String &path)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->open(path);
		}
		virtual void close()
		{
			RUID_MANAGER_FACTORY; 
			m_prep->close();
		}
		virtual void init(const ServiceEnvironmentIFCRef& env)
		{
			m_prep->init(env);
		}
		virtual void shutdown()
		{
			m_prep->shutdown();
		}
		virtual ServiceEnvironmentIFCRef getEnvironment() const
		{
			// Inst/Assoc/Meth provs don't need one of these
			return ServiceEnvironmentIFCRef(0);
		}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
		virtual void createNameSpace(const String &ns,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->createNameSpace(ns, context);
		}
		virtual void deleteNameSpace(const String &ns,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->deleteNameSpace(ns, context);
		}
#endif

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
		virtual void enumQualifierTypes(const String &ns,
			CIMQualifierTypeResultHandlerIFC &result, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->enumQualifierTypes(ns, result, context);
		}
		virtual void deleteQualifierType(const String &ns,
			const String &qualName, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->deleteQualifierType(ns, qualName, context);
		}
		virtual void setQualifierType(const String &ns,
			const CIMQualifierType &qt, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->setQualifierType(ns, qt, context);
		}
#endif

		virtual void enumNameSpace(StringResultHandlerIFC &result,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->enumNameSpace(result, context);

		}
		virtual CIMQualifierType getQualifierType(const String &ns,
			const String &qualifierName, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->getQualifierType(ns, qualifierName, context);
		}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
		virtual CIMClass deleteClass(const String &ns, const String &className,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->deleteClass(ns, className, context);
		}
		virtual void createClass(const String &ns, const CIMClass &cimClass,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->createClass(ns, cimClass, context);
		}
		virtual CIMClass modifyClass(const String &ns, const CIMClass &cc,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->modifyClass(ns, cc, context);
		}
#endif

		virtual CIMClass getClass(const String &ns, const String &className,
			ELocalOnlyFlag localOnly,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->getClass(ns, className, localOnly,
				includeQualifiers, includeClassOrigin, propertyList, context);
		}
		virtual void enumClassNames(const String &ns, const String &className,
			StringResultHandlerIFC &result, EDeepFlag deep,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->enumClassNames(ns, className, result, deep, context);
		}
		virtual void enumInstances(const String &ns, const String &className,
			CIMInstanceResultHandlerIFC &result, EDeepFlag deep,
			ELocalOnlyFlag localOnly,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList,
			EEnumSubclassesFlag enumSubclasses,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->enumInstances(ns, className, result, deep, localOnly,
				includeQualifiers, includeClassOrigin, propertyList,
				enumSubclasses, context);
		}
		virtual void enumClasses(const String &ns, const String &className,
			CIMClassResultHandlerIFC &result, EDeepFlag deep,
			ELocalOnlyFlag localOnly,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->enumClasses(ns, className, result, deep, localOnly,
				includeQualifiers, includeClassOrigin, context);
		}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
		virtual CIMInstance deleteInstance(const String &ns,
			const CIMObjectPath &cop, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->deleteInstance(ns, cop, context);
		}
		virtual CIMObjectPath createInstance(const String &ns,
			const CIMInstance &ci, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->createInstance(ns, ci, context);
		}
		virtual CIMInstance modifyInstance(const String &ns,
			const CIMInstance &modifiedInstance,
			EIncludeQualifiersFlag includeQualifiers,
			const StringArray *propertyList, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->modifyInstance(ns, modifiedInstance,
				includeQualifiers, propertyList, context);
		}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
		virtual void setProperty(const String &ns, const CIMObjectPath &name,
			const String &propertyName, const CIMValue &cv,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->setProperty(ns, name, propertyName, cv, context);
		}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

#endif
		virtual void enumInstanceNames(const String &ns,
			const String &className, CIMObjectPathResultHandlerIFC &result,
			EDeepFlag deep, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->enumInstanceNames(ns, className, result, deep, context);
		}

		virtual CIMInstance getInstance(const String &ns,
			const CIMObjectPath &instanceName,
			ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->getInstance(ns, instanceName, localOnly,
				includeQualifiers, includeClassOrigin, propertyList, context);
		}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
		virtual CIMValue getProperty(const String &ns,
			const CIMObjectPath &name, const String &propertyName,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->getProperty(ns, name, propertyName, context);
		}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

		virtual CIMValue invokeMethod(const String &ns,
			const CIMObjectPath &path, const String &methodName,
			const CIMParamValueArray &inParams, CIMParamValueArray &outParams,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			return m_prep->invokeMethod(ns, path, methodName, inParams,
				outParams, context);
		}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		virtual void associatorsClasses(const String &ns,
			const CIMObjectPath &path, CIMClassResultHandlerIFC &result,
			const String &assocClass, const String &resultClass,
			const String &role, const String &resultRole,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->associatorsClasses(ns, path, result, assocClass,
				resultClass, role, resultRole, includeQualifiers,
				includeClassOrigin,	propertyList, context);
		}
		virtual void referenceNames(const String &ns,
			const CIMObjectPath &path, CIMObjectPathResultHandlerIFC &result,
			const String &resultClass, const String &role,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->referenceNames(ns, path, result, resultClass, role,
				context);
		}
		virtual void associatorNames(const String &ns,
			const CIMObjectPath &path, CIMObjectPathResultHandlerIFC &result,
			const String &assocClass, const String &resultClass,
			const String &role, const String &resultRole,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->associatorNames(ns, path, result, assocClass, resultClass,
				role, resultRole, context);
		}
		virtual void associators(const String &ns, const CIMObjectPath &path,
			CIMInstanceResultHandlerIFC &result, const String &assocClass,
			const String &resultClass, const String &role,
			const String &resultRole, EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->associators(ns, path, result, assocClass, resultClass,
				role, resultRole, includeQualifiers, includeClassOrigin,
				propertyList, context);
		}
		virtual void references(const String &ns, const CIMObjectPath &path,
			CIMInstanceResultHandlerIFC &result, const String &resultClass,
			const String &role, EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->references(ns, path, result, resultClass, role,
				includeQualifiers, includeClassOrigin, propertyList, context);
		}
		virtual void referencesClasses(const String &ns,
			const CIMObjectPath &path, CIMClassResultHandlerIFC &result,
			const String &resultClass, const String &role,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->referencesClasses(ns, path, result, resultClass, role,
				includeQualifiers, includeClassOrigin, propertyList, context);
		}
#endif
		virtual void execQuery(const String &ns,
			CIMInstanceResultHandlerIFC &result, const String &query,
			const String &queryLanguage, OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->execQuery(ns, result, query, queryLanguage, context);
		}
		virtual void beginOperation(EOperationFlag op,
			OperationContext &context)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->beginOperation(op, context);
		}
		virtual void endOperation(EOperationFlag op, OperationContext &context,
			EOperationResultFlag result)
		{
			RUID_MANAGER_FACTORY; 
			m_prep->endOperation(op, context, result);
		}

	private:

		RepositoryIFCRef m_prep;
#if defined(OW_GNU_LINUX)
		uid_t m_cimomuid;
		uid_t m_useruid;
#elif defined(OW_NETWARE)
		NetwareIdentityRef m_nwi; 
#endif
	};

	class ProxyEnvironment : public ProviderEnvironmentIFC
	{
	public:
#if defined(OW_GNU_LINUX)
		ProxyEnvironment(const ProviderEnvironmentIFCRef& env, uid_t cimomuid,
			uid_t useruid)
			: ProviderEnvironmentIFC()
			, m_cimomuid(cimomuid)
			, m_useruid(useruid)
#elif defined(OW_NETWARE)
		ProxyEnvironment(const ProviderEnvironmentIFCRef& env, NetwareIdentityRef nwi)
			: ProviderEnvironmentIFC()
			, m_nwi(nwi)
#endif
			, m_env(env)
		{}

		virtual String getConfigItem(const String &name,
			const String& defRetVal="") const
		{
			return m_env->getConfigItem(name, defRetVal);
		}

		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
#if defined(OW_GNU_LINUX)
			return CIMOMHandleIFCRef(new ProxyCIMOMHandle(
				m_env->getCIMOMHandle(), m_cimomuid, m_useruid));
#elif defined(OW_NETWARE)
			return CIMOMHandleIFCRef(new ProxyCIMOMHandle(
				m_env->getCIMOMHandle(), m_nwi)); 
#endif
		}
		
		virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
#if defined(OW_GNU_LINUX)
			return CIMOMHandleIFCRef(new ProxyCIMOMHandle(
				m_env->getRepositoryCIMOMHandle(), m_cimomuid, m_useruid));
#elif defined(OW_NETWARE)
			return CIMOMHandleIFCRef(new ProxyCIMOMHandle(
				m_env->getRepositoryCIMOMHandle(), m_nwi)); 
#endif
		}
		
		virtual RepositoryIFCRef getRepository() const
		{
#if defined(OW_GNU_LINUX)
			return RepositoryIFCRef(new ProxyRepository(
				m_env->getRepository(), m_cimomuid, m_useruid));
#elif defined(OW_NETWARE)
			return RepositoryIFCRef(new ProxyRepository(
				m_env->getRepository(), m_nwi)); 
#endif
		}

		virtual LoggerRef getLogger() const
		{
			return m_env->getLogger();
		}

		virtual LoggerRef getLogger(const String& componentName) const
		{
			return m_env->getLogger(componentName);
		}

		virtual String getUserName() const
		{
			return m_env->getUserName();
		}

		virtual OperationContext& getOperationContext()
		{
			return m_env->getOperationContext();
		}

	private:

#if defined(OW_GNU_LINUX)
		uid_t m_cimomuid;
		uid_t m_useruid;
#elif defined(OW_NETWARE)
		NetwareIdentityRef m_nwi; 
#endif
		ProviderEnvironmentIFCRef m_env;
	};

#if defined (OW_GNU_LINUX)
	void getUIDS(const ProviderEnvironmentIFCRef& env,
		uid_t& cimomuid, uid_t& useruid)
	{
		cimomuid = ::getuid();	// Get CIMOM user id
		useruid = cimomuid;

		String wk = env->getOperationContext().getStringDataWithDefault(
			OperationContext::CURUSER_UIDKEY);
        if (!wk.empty())
        {
			useruid = wk.toUInt32();
			return;
		}
		String userName = env->getUserName();

		long pwnbufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
		char buf[pwnbufsize];
		struct passwd pw;
		struct passwd* ppw = 0;
	    int rv = ::getpwnam_r(userName.c_str(), &pw, buf, pwnbufsize, &ppw);
		if (rv == 0 && ppw)
		{
			useruid = pw.pw_uid;
		}
	}

	inline ProviderEnvironmentIFCRef
	proxyEnv(const ProviderEnvironmentIFCRef& env, uid_t cimomuid,
		uid_t useruid)
	{
		return ProviderEnvironmentIFCRef(new ProxyEnvironment(env, cimomuid,
			useruid));
	}

#elif defined(OW_NETWARE)
	NetwareIdentityRef getNWI(const ProviderEnvironmentIFCRef& env)
	{
		OperationContext::DataRef data = env->getOperationContext().getData(NOVELL_IDENTITY_KEY); 
		if (data)
		{
			NetwareIdentityRef nwi = data.cast_to<NetwareIdentity>(); 
			return nwi; 
		}
		return NetwareIdentityRef(0); 
	}

	inline ProviderEnvironmentIFCRef
	proxyEnv(const ProviderEnvironmentIFCRef& env, NetwareIdentityRef nwi)
	{
		return ProviderEnvironmentIFCRef(new ProxyEnvironment(env, nwi)); 
	}

#endif

}	// namespace

//////////////////////////////////////////////////////////////////////////////
InstanceProviderProxy::InstanceProviderProxy(InstanceProviderIFCRef pProv,
	const ProviderEnvironmentIFCRef& env)
	: InstanceProviderIFC()
	, m_pProv(pProv)
#if defined(OW_GNU_LINUX)
	, m_cimomuid(0)
	, m_useruid(0)
{
	getUIDS(env, m_cimomuid, m_useruid);
#elif defined(OW_NETWARE)
	, m_nwi(0)
{
	m_nwi = getNWI(env); 
#endif
}

//////////////////////////////////////////////////////////////////////////////
void
InstanceProviderProxy::enumInstanceNames(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	const CIMClass& cimClass )
{
	UID_MANAGER_FACTORY; 
	m_pProv->enumInstanceNames(PROXY_ENV_FACTORY, ns,
		className, result, cimClass);
}

//////////////////////////////////////////////////////////////////////////////
void
InstanceProviderProxy::enumInstances(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	ELocalOnlyFlag localOnly,
	EDeepFlag deep,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	const CIMClass& requestedClass,
	const CIMClass& cimClass )
{
	UID_MANAGER_FACTORY; 
	m_pProv->enumInstances(PROXY_ENV_FACTORY, ns, className,
		result, localOnly, deep, includeQualifiers, includeClassOrigin,
		propertyList, requestedClass, cimClass);
}

//////////////////////////////////////////////////////////////////////////////
CIMInstance
InstanceProviderProxy::getInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	const CIMClass& cimClass )
{
	UID_MANAGER_FACTORY; 
	return m_pProv->getInstance(PROXY_ENV_FACTORY, ns,
		instanceName, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, cimClass);
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
InstanceProviderProxy::createInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance& cimInstance )
{
	UID_MANAGER_FACTORY; 
	return m_pProv->createInstance(PROXY_ENV_FACTORY, ns,
		cimInstance);
}

//////////////////////////////////////////////////////////////////////////////
void
InstanceProviderProxy::modifyInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance& modifiedInstance,
	const CIMInstance& previousInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	const CIMClass& theClass)
{
	UID_MANAGER_FACTORY; 
	m_pProv->modifyInstance(PROXY_ENV_FACTORY, ns,
		modifiedInstance, previousInstance, includeQualifiers, propertyList,
		theClass);
}

//////////////////////////////////////////////////////////////////////////////
void
InstanceProviderProxy::deleteInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMObjectPath& cop)
{
	UID_MANAGER_FACTORY; 
	m_pProv->deleteInstance(PROXY_ENV_FACTORY, ns, cop);
}

#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	
//////////////////////////////////////////////////////////////////////////////
SecondaryInstanceProviderProxy::SecondaryInstanceProviderProxy(SecondaryInstanceProviderIFCRef pProv,
	const ProviderEnvironmentIFCRef& env)
	: SecondaryInstanceProviderIFC()
	, m_pProv(pProv)
#if defined(OW_GNU_LINUX)
	, m_cimomuid(0)
	, m_useruid(0)
{
	getUIDS(PROXY_ENV_FACTORY, m_cimomuid, m_useruid);
#elif defined(OW_NETWARE)
	, m_nwi(0)
{
	m_nwi = getNWI(PROXY_ENV_FACTORY); 
#endif
}

//////////////////////////////////////////////////////////////////////////////
void
SecondaryInstanceProviderProxy::filterInstances(
	const ProviderEnvironmentIFCRef &env, const String &ns,
	const String &className, CIMInstanceArray &instances,
	ELocalOnlyFlag localOnly, EDeepFlag deep,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const CIMClass &requestedClass,
	const CIMClass &cimClass)
{
	UID_MANAGER_FACTORY; 
	m_pProv->filterInstances(env, ns, className, instances, localOnly, deep, includeQualifiers, includeClassOrigin, propertyList, requestedClass, cimClass);
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
SecondaryInstanceProviderProxy::createInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &cimInstance)
{
	UID_MANAGER_FACTORY; 
	m_pProv->createInstance(env, ns, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////
void
SecondaryInstanceProviderProxy::modifyInstance(
	const ProviderEnvironmentIFCRef &env, const String &ns,
	const CIMInstance &modifiedInstance, const CIMInstance &previousInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray *propertyList, const CIMClass &theClass)
{
	UID_MANAGER_FACTORY; 
	m_pProv->modifyInstance(env, ns, modifiedInstance, previousInstance, includeQualifiers, propertyList, theClass);
}

//////////////////////////////////////////////////////////////////////////////
void
SecondaryInstanceProviderProxy::deleteInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &cop)
{
	UID_MANAGER_FACTORY; 
	m_pProv->deleteInstance(env, ns, cop);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION


//////////////////////////////////////////////////////////////////////////////
MethodProviderProxy::MethodProviderProxy(MethodProviderIFCRef pProv,
	const ProviderEnvironmentIFCRef& env)
	: MethodProviderIFC()
	, m_pProv(pProv)
#if defined(OW_GNU_LINUX)
	, m_cimomuid(0)
	, m_useruid(0)
{
	getUIDS(PROXY_ENV_FACTORY, m_cimomuid, m_useruid);
#elif defined(OW_NETWARE)
	, m_nwi(0)
{
	m_nwi = getNWI(PROXY_ENV_FACTORY); 
#endif
}

//////////////////////////////////////////////////////////////////////////////
CIMValue
MethodProviderProxy::invokeMethod(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName,
	const CIMParamValueArray& in,
	CIMParamValueArray& out )
{
	UID_MANAGER_FACTORY; 
	return m_pProv->invokeMethod(PROXY_ENV_FACTORY, ns,
		path, methodName, in, out);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

//////////////////////////////////////////////////////////////////////////////
AssociatorProviderProxy::AssociatorProviderProxy(
	AssociatorProviderIFCRef pProv, const ProviderEnvironmentIFCRef& env)
	: AssociatorProviderIFC()
	, m_pProv(pProv)
#if defined(OW_GNU_LINUX)
	, m_cimomuid(0)
	, m_useruid(0)
{
	getUIDS(PROXY_ENV_FACTORY, m_cimomuid, m_useruid);
#elif defined(OW_NETWARE)
	, m_nwi(0)
{
	m_nwi = getNWI(PROXY_ENV_FACTORY); 
#endif
}

//////////////////////////////////////////////////////////////////////////////
void
AssociatorProviderProxy::associators(
	const ProviderEnvironmentIFCRef& env,
	CIMInstanceResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	UID_MANAGER_FACTORY; 
	m_pProv->associators(PROXY_ENV_FACTORY, result, ns,
		objectName, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList);
}

//////////////////////////////////////////////////////////////////////////////
void
AssociatorProviderProxy::associatorNames(
	const ProviderEnvironmentIFCRef& env,
	CIMObjectPathResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole )
{
	UID_MANAGER_FACTORY; 
	m_pProv->associatorNames(PROXY_ENV_FACTORY, result, ns,
		objectName, assocClass, resultClass, role, resultRole);
}

//////////////////////////////////////////////////////////////////////////////
void
AssociatorProviderProxy::references(
	const ProviderEnvironmentIFCRef& env,
	CIMInstanceResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& resultClass,
	const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	UID_MANAGER_FACTORY; 
	m_pProv->references(PROXY_ENV_FACTORY, result, ns,
		objectName, resultClass, role, includeQualifiers, includeClassOrigin,
		propertyList);
}

//////////////////////////////////////////////////////////////////////////////
void
AssociatorProviderProxy::referenceNames(
	const ProviderEnvironmentIFCRef& env,
	CIMObjectPathResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& resultClass,
	const String& role )
{
	UID_MANAGER_FACTORY; 
	m_pProv->referenceNames(PROXY_ENV_FACTORY, result, ns,
		objectName, resultClass, role);
}

#endif	// OW_DISABLE_ASSOCIATION_TRAVERSAL

}	// namepsace OpenWBEM

#endif	// OW_THREADS_RUN_AS_USER



