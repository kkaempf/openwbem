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
#include "OW_CIMServer.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Format.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMRepository.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_OperationContext.hpp"
#include "OW_MutexLock.hpp"
#include "OW_UserInfo.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
#if !defined(OW_DISABLE_ACLS)
class AccessMgr
{
public:
	enum
	{
		GETCLASS,
		GETINSTANCE,
		DELETECLASS,
		DELETEINSTANCE,
		CREATECLASS,
		CREATEINSTANCE,
		MODIFYCLASS,
		MODIFYINSTANCE,
		ENUMERATECLASSES,
		ENUMERATECLASSNAMES,
		ENUMERATEINSTANCES,
		ENUMERATEINSTANCENAMES,
		ASSOCIATORS,
		ASSOCIATORNAMES,
		REFERENCES,
		REFERENCENAMES,
		GETPROPERTY,
		SETPROPERTY,
		GETQUALIFIER,
		SETQUALIFIER,
		DELETEQUALIFIER,
		ENUMERATEQUALIFIERS,
		CREATENAMESPACE,
		DELETENAMESPACE,
		ENUMERATENAMESPACE,
		INVOKEMETHOD
	};
	AccessMgr(CIMServer* pServer, CIMOMEnvironmentRef env);
	/**
	 * checkAccess will check that access is granted through the ACL. If
	 * Access is not granted, an CIMException will be thrown.
	 * @param op	The operation that access is being checked for.
	 * @param ns	The name space that access is being check on.
	 * @param context The context from which the ACL info for the user request will be retrieved.
	 */
	void checkAccess(int op, const String& ns, OperationContext& context);
private:
	String getMethodType(int op);
	CIMServer* m_pServer;
	CIMOMEnvironmentRef m_env;
};
//////////////////////////////////////////////////////////////////////////////
AccessMgr::AccessMgr(CIMServer* pServer,
	CIMOMEnvironmentRef env)
	: m_pServer(pServer)
	, m_env(env)
{
}
//////////////////////////////////////////////////////////////////////////////
String
AccessMgr::getMethodType(int op)
{
	switch(op)
	{
		case GETCLASS:
		case GETINSTANCE:
		case ENUMERATECLASSES:
		case ENUMERATECLASSNAMES:
		case ENUMERATEINSTANCES:
		case ENUMERATEINSTANCENAMES:
		case ENUMERATEQUALIFIERS:
		case GETPROPERTY:
		case GETQUALIFIER:
		case ENUMERATENAMESPACE:
		case ASSOCIATORS:
		case ASSOCIATORNAMES:
		case REFERENCES:
		case REFERENCENAMES:
			return String("r");
		case DELETECLASS:
		case DELETEINSTANCE:
		case CREATECLASS:
		case CREATEINSTANCE:
		case MODIFYCLASS:
		case MODIFYINSTANCE:
		case SETPROPERTY:
		case SETQUALIFIER:
		case DELETEQUALIFIER:
		case CREATENAMESPACE:
		case DELETENAMESPACE:
			return String("w");
		case INVOKEMETHOD:
			return String("rw");
		default:
			OW_ASSERT("Unknown operation type passed to "
				"AccessMgr.  This shouldn't happen" == 0);
	}
	return "";
}
//////////////////////////////////////////////////////////////////////////////
void
AccessMgr::checkAccess(int op, const String& ns,
	OperationContext& context)
{
	UserInfo userInfo = context.getUserInfo();
	if (userInfo.m_internal)
	{
		return;
	}
	if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
	{
		m_env->logDebug(format("Checking access to namespace: \"%1\"", ns));
		m_env->logDebug(format("UserName is: \"%1\" Operation is : %2",
			userInfo.getUserName(), op));
	}
	String lns(ns);
	while (!lns.empty() && lns[0] == '/')
	{
		lns = lns.substring(1);
	}
	lns.toLowerCase();
	for(;;)
	{
		if (!userInfo.getUserName().empty())
		{
			String superUser =
				m_env->getConfigItem(ConfigOpts::ACL_SUPERUSER_opt);
			if (superUser.equalsIgnoreCase(userInfo.getUserName()))
			{
				m_env->logDebug("User is SuperUser: checkAccess returning.");
				return;
			}
			try
			{
				CIMClass cc = m_pServer->_getClass("root/security",
					"OpenWBEM_UserACL", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
					context);
			}
			catch(CIMException&)
			{
				m_env->logDebug("OpenWBEM_UserACL class non-existent in"
					" /root/security. ACLs disabled");
				return;
			}
			
			CIMObjectPath cop("OpenWBEM_UserACL");
			cop.addKey("username", CIMValue(userInfo.getUserName()));
			cop.addKey("nspace", CIMValue(lns));
			CIMInstance ci(CIMNULL);
			try
			{
				ci = m_pServer->_getInstance("root/security", cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
					NULL, context);
			}
			catch(const CIMException&)
			{
				ci.setNull();
			}
			if (ci)
			{
				String capability;
				CIMProperty capabilityProp = ci.getProperty("capability");
				if (capabilityProp)
				{
					CIMValue cv = capabilityProp.getValue();
					if (cv)
					{
						capability = cv.toString();
					}
				}
				String opType = getMethodType(op);
				capability.toLowerCase();
				if (opType.length() == 1)
				{
					if (capability.indexOf(opType) == String::npos)
					{
						m_env->logInfo(format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							userInfo.m_userName, lns));
						OW_THROWCIM(CIMException::ACCESS_DENIED);
					}
				}
				else
				{
					if (!capability.equals("rw") && !capability.equals("wr"))
					{
						m_env->logInfo(format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							userInfo.m_userName, lns));
						OW_THROWCIM(CIMException::ACCESS_DENIED);
					}
				}
				m_env->logInfo(format(
					"ACCESS GRANTED to user \"%1\" for namespace \"%2\"",
					userInfo.m_userName, lns));
				return;
			}
		}
		// use default policy for namespace
		try
		{
			CIMClass cc = m_pServer->_getClass("root/security",
				"OpenWBEM_NamespaceACL", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
				context);
		}
		catch(CIMException&)
		{
			m_env->logDebug("OpenWBEM_NamespaceACL class non-existent in"
				" /root/security. namespace ACLs disabled");
			return;
		}
		CIMObjectPath cop("OpenWBEM_NamespaceACL");
		cop.addKey("nspace", CIMValue(lns));
		CIMInstance ci(CIMNULL);
		try
		{
			ci = m_pServer->_getInstance("root/security", cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
				NULL, context);
		}
		catch(const CIMException& ce)
		{
			if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
			{
				m_env->logDebug(format("Caught exception: %1 in"
					" AccessMgr::checkAccess", ce));
			}
			ci.setNull();
		}
	
		if (ci)
		{
			String capability;
			CIMProperty capabilityProp = ci.getProperty("capability");
			if (capabilityProp)
			{
				CIMValue v = capabilityProp.getValue();
				if (v)
				{
					capability = v.toString();
				}
			}
			capability.toLowerCase();
			String opType = getMethodType(op);
			if (opType.length() == 1)
			{
				if (capability.indexOf(opType) == String::npos)
				{
					m_env->logInfo(format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						userInfo.m_userName, lns));
					OW_THROWCIM(CIMException::ACCESS_DENIED);
				}
			}
			else
			{
				if (!capability.equals("rw") && !capability.equals("wr"))
				{
					m_env->logInfo(format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						userInfo.m_userName, lns));
					OW_THROWCIM(CIMException::ACCESS_DENIED);
				}
			}
			m_env->logInfo(format(
				"ACCESS GRANTED to user \"%1\" for namespace \"%2\"",
				userInfo.m_userName, lns));
			return;
		}
		size_t idx = lns.lastIndexOf('/');
		if (idx == 0 || idx == String::npos)
		{
			break;
		}
		lns = lns.substring(0, idx);
	}
	m_env->logInfo(format(
		"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
		userInfo.m_userName, lns));
	OW_THROWCIM(CIMException::ACCESS_DENIED);
}
#endif
//////////////////////////////////////////////////////////////////////////////
CIMServer::CIMServer(CIMOMEnvironmentRef env,
	const ProviderManagerRef& provManager,
	const RepositoryIFCRef& cimRepository)
	: RepositoryIFC()
	, m_provManager(provManager)
#if !defined(OW_DISABLE_ACLS)
	, m_accessMgr(new AccessMgr(this, env))
#endif
	, m_nsClass_Namespace(CIMNULL)
	, m_env(env)
	, m_cimRepository(cimRepository)
	, m_realRepository(dynamic_pointer_cast<CIMRepository>(m_cimRepository))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMServer::~CIMServer()
{
	try
	{
		close();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::open(const String& path)
{
	(void)path;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::close()
{
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::createNameSpace(const String& ns,
	OperationContext& context)
{
	// Don't need to check ACLs, since this is a result of calling createInstance.
	m_cimRepository->createNameSpace(ns,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::deleteNameSpace(const String& ns,
	OperationContext& context)
{
	// Don't need to check ACLs, since this is a result of calling deleteInstance.
	m_cimRepository->deleteNameSpace(ns,context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumNameSpace(StringResultHandlerIFC& result,
	OperationContext& context)
{
	// Don't need to check ACLs, since this is a result of calling enumInstances.
	m_cimRepository->enumNameSpace(result,context);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
CIMServer::getQualifierType(const String& ns,
	const String& qualifierName,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::GETQUALIFIER, ns, context);
#endif
	return m_cimRepository->getQualifierType(ns,qualifierName,context);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEQUALIFIERS, ns, context);
#endif
	m_cimRepository->enumQualifierTypes(ns,result,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::deleteQualifierType(const String& ns, const String& qualName,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::DELETEQUALIFIER, ns, context);
#endif
	m_cimRepository->deleteQualifierType(ns,qualName,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::setQualifierType(
	const String& ns,
	const CIMQualifierType& qt, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::SETQUALIFIER, ns, context);
#endif
	m_cimRepository->setQualifierType(ns,qt,context);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::getClass(
	const String& ns, const String& className, ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// we don't check for __Namespace, so that clients can get it before they
	// create one.
	if (!className.equalsIgnoreCase("__Namespace"))
	{
		m_accessMgr->checkAccess(AccessMgr::GETCLASS, ns, context);
	}
#endif
	return _getClass(ns,className, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::_getClass(const String& ns, const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	CIMClass theClass = _getNameSpaceClass(className);
	if(!theClass)
	{
		theClass = m_cimRepository->getClass(ns,className,localOnly,
			includeQualifiers,includeClassOrigin,propertyList,context);
	}
	return theClass;
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::_instGetClass(const String& ns, const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	CIMClass theClass = _getNameSpaceClass(className);
	if(!theClass)
	{
		try
		{
			theClass = m_cimRepository->getClass(ns,className,localOnly,
				includeQualifiers,includeClassOrigin,propertyList,context);
		}
		catch (CIMException& e)
		{
			if (e.getErrNo() == CIMException::NOT_FOUND)
			{
				e.setErrNo(CIMException::INVALID_CLASS);
			}
			throw e;
		}
	}
	return theClass;
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::deleteClass(const String& ns, const String& className,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::DELETECLASS, ns, context);
#endif
	return m_cimRepository->deleteClass(ns,className,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::createClass(const String& ns, const CIMClass& cimClass,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::CREATECLASS, ns, context);
#endif
	if(cimClass.getName().equals(CIMClass::NAMESPACECLASS))
	{
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS,
			format("Creation of class %1 is not allowed",
				cimClass.getName()).c_str());
	}
	m_cimRepository->createClass(ns,cimClass,context);
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::modifyClass(
	const String& ns,
	const CIMClass& cc,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::MODIFYCLASS, ns, context);
#endif
	return m_cimRepository->modifyClass(ns,cc,context);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATECLASSES, ns, context);
#endif
	m_cimRepository->enumClasses(ns,className,result,deep,localOnly,
		includeQualifiers,includeClassOrigin,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATECLASSNAMES, ns, context);
#endif
	m_cimRepository->enumClassNames(ns,className,result,deep,context);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class InstNameEnumerator : public CIMClassResultHandlerIFC
	{
	public:
		InstNameEnumerator(
			const String& ns_,
			CIMObjectPathResultHandlerIFC& result_,
			OperationContext& context_,
			const CIMOMEnvironmentRef& env_,
			CIMServer* server_)
			: ns(ns_)
			, result(result_)
			, context(context_)
			, m_env(env_)
			, server(server_)
		{}
	protected:
		virtual void doHandle(const CIMClass &cc)
		{
			if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
			{
				m_env->logDebug(format("CIMServer InstNameEnumerator enumerated derived instance names: %1:%2", ns,
					cc.getName()));
			}
			server->_getCIMInstanceNames(ns, cc.getName(), cc, result, context);
		}
	private:
		String ns;
		CIMObjectPathResultHandlerIFC& result;
		OperationContext& context;
		const CIMOMEnvironmentRef& m_env;
		CIMServer* server;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEINSTANCENAMES, ns,
		context);
#endif
	InstNameEnumerator ie(ns, result, context, m_env, this);
	CIMClass theClass = _instGetClass(ns, className,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
	ie.handle(theClass);
	// If this is the namespace class then just return now
	if(className.equalsIgnoreCase(CIMClass::NAMESPACECLASS)
		|| !deep)
	{
		return;
	}
	else
	{
		// TODO: measure whether it would be faster to use
		// enumClassNames + getClass() here.
		m_cimRepository->enumClasses(ns,className,ie,deep,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,context);
	}
}
/////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMServerProviderEnvironment : public ProviderEnvironmentIFC
	{
	public:
		CIMServerProviderEnvironment(OperationContext& context,
			const CIMOMEnvironmentRef& env)
			: m_context(context)
			, m_env(env)
		{}
		virtual String getConfigItem(const String &name, const String& defRetVal="") const
		{
			return m_env->getConfigItem(name, defRetVal);
		}
		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_context,
				ServiceEnvironmentIFC::E_DONT_SEND_INDICATIONS,
				ServiceEnvironmentIFC::E_USE_PROVIDERS,
				CIMOMEnvironment::E_NO_LOCKING);
		}
		
		virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_context,
				ServiceEnvironmentIFC::E_DONT_SEND_INDICATIONS,
				ServiceEnvironmentIFC::E_BYPASS_PROVIDERS,
				CIMOMEnvironment::E_NO_LOCKING);
		}
		
		virtual RepositoryIFCRef getRepository() const
		{
			return m_env->getRepository();
		}
		virtual LoggerRef getLogger() const
		{
			return m_env->getLogger();
		}
		virtual String getUserName() const
		{
			return m_context.getUserInfo().getUserName();
		}
		virtual OperationContext& getOperationContext()
		{
			return m_context;
		}
	private:
		OperationContext& m_context;
		CIMOMEnvironmentRef m_env;
	};
	inline ProviderEnvironmentIFCRef createProvEnvRef(OperationContext& context, const CIMOMEnvironmentRef& env)
	{
		return ProviderEnvironmentIFCRef(new CIMServerProviderEnvironment(context, env));
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
CIMServer::_getCIMInstanceNames(const String& ns, const String& className,
	const CIMClass& theClass, CIMObjectPathResultHandlerIFC& result,
	OperationContext& context)
{
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass, context);
	if (instancep)
	{
		instancep->enumInstanceNames(createProvEnvRef(context, m_env),
			ns, className, result, theClass);
	}
	else
	{
		m_cimRepository->enumInstanceNames(ns,className,result,E_SHALLOW,context);
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class InstEnumerator : public CIMClassResultHandlerIFC
	{
	public:
		InstEnumerator(
			const String& ns_,
			CIMInstanceResultHandlerIFC& result_,
			OperationContext& context_,
			const CIMOMEnvironmentRef& env_,
			CIMServer* server_,
			EDeepFlag deep_,
			ELocalOnlyFlag localOnly_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propertyList_,
			const CIMClass& theTopClass_)
			: ns(ns_)
			, result(result_)
			, context(context_)
			, m_env(env_)
			, server(server_)
			, deep(deep_)
			, localOnly(localOnly_)
			, includeQualifiers(includeQualifiers_)
			, includeClassOrigin(includeClassOrigin_)
			, propertyList(propertyList_)
			, theTopClass(theTopClass_)
		{}
	protected:
		virtual void doHandle(const CIMClass &cc)
		{
			if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
			{
				m_env->logDebug(format("CIMServer InstEnumerator Enumerating derived instance names: %1:%2", ns,
					cc.getName()));
			}
			server->_getCIMInstances(ns, cc.getName(), theTopClass, cc,
				result, localOnly, deep, includeQualifiers,
				includeClassOrigin, propertyList, context);
		}
	private:
		String ns;
		CIMInstanceResultHandlerIFC& result;
		OperationContext& context;
		const CIMOMEnvironmentRef& m_env;
		CIMServer* server;
		EDeepFlag deep;
		ELocalOnlyFlag localOnly;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propertyList;
		const CIMClass& theTopClass;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result, EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, EEnumSubclassesFlag enumSubclasses,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEINSTANCES, ns, context);
#endif
	CIMClass theTopClass = _instGetClass(ns, className, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, context);
	InstEnumerator ie(ns, result, context, m_env, this, deep, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, theTopClass);
	ie.handle(theTopClass);
	// If this is the namespace class then only do one class
	if(theTopClass.getName().equals(CIMClass::NAMESPACECLASS) || enumSubclasses == E_DONT_ENUM_SUBCLASSES)
	{
		return;
	}
	else
	{
		// TODO: measure whether it would be faster to use
		// enumClassNames + getClass() here.
		// do subclasses
		m_cimRepository->enumClasses(ns,className,ie,E_DEEP,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,context);
	}
}
namespace
{
	/* not going to use this -- the providers are now responsible for their own behavior
	class HandleProviderInstance : public CIMInstanceResultHandlerIFC
	{
	public:
		HandleProviderInstance(
			bool includeQualifiers_, bool includeClassOrigin_,
			const StringArray* propList_,
			CIMInstanceResultHandlerIFC& result_)
		: includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, result(result_)
		{}
	protected:
		virtual void doHandle(const CIMInstance &ci)
		{
			result.handle(ci.clone(false, includeQualifiers,
				includeClassOrigin, propList));
		}
	private:
		bool includeQualifiers, includeClassOrigin;
		const StringArray* propList;
		CIMInstanceResultHandlerIFC& result;
	};
	*/
	class HandleLocalOnlyAndDeep : public CIMInstanceResultHandlerIFC
	{
	public:
		HandleLocalOnlyAndDeep(
			CIMInstanceResultHandlerIFC& result_,
			const CIMClass& requestedClass_,
			bool localOnly_,
			bool deep_)
		: result(result_)
		, requestedClass(requestedClass_)
		, localOnly(localOnly_)
		, deep(deep_)
		{}
	protected:
		virtual void doHandle(const CIMInstance &inst)
		{
			if (deep == true && localOnly == false) // don't filter anything
			{
				result.handle(inst);
				return;
			}
			CIMPropertyArray props = inst.getProperties();
			CIMPropertyArray newprops;
			CIMInstance newInst(inst);
			String requestedClassName = requestedClass.getName();
			for (size_t i = 0; i < props.size(); ++i)
			{
				CIMProperty p = props[i];
				CIMProperty clsp = requestedClass.getProperty(p.getName());
				if (clsp)
				{
					if (clsp.getOriginClass().equalsIgnoreCase(requestedClassName))
					{
						newprops.push_back(p);
						continue;
					}
				}
				if (deep == true)
				{
					if (!clsp
						|| !p.getOriginClass().equalsIgnoreCase(clsp.getOriginClass()))
					{
						// the property is from a derived class
						newprops.push_back(p);
						continue;
					}
				}
				if (localOnly == false)
				{
					if (clsp)
					{
						// the property has to be from a superclass
						newprops.push_back(p);
						continue;
					}
				}
			}
			newInst.setProperties(newprops);
			newInst.setKeys(inst.getKeyValuePairs());
			result.handle(newInst);
		}
	private:
		CIMInstanceResultHandlerIFC& result;
		const CIMClass& requestedClass;
		bool localOnly;
		bool deep;
	};
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
CIMServer::_getCIMInstances(
	const String& ns,
	const String& className,
	const CIMClass& theTopClass,
	const CIMClass& theClass, CIMInstanceResultHandlerIFC& result,
	ELocalOnlyFlag localOnly, EDeepFlag deep, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass, context));
	if (instancep)
	{
		if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
		{
			m_env->logDebug(format("CIMServer calling provider to enumerate instances: %1:%2", ns,
				className));
		}
		// not going to use these, the provider ifc/providers are now responsible for it.
		//HandleLocalOnlyAndDeep handler1(result,theTopClass,localOnly,deep);
		//HandleProviderInstance handler2(includeQualifiers, includeClassOrigin, propertyList, handler1);
		instancep->enumInstances(
			createProvEnvRef(context, m_env), ns, className, result, localOnly,
			deep, includeQualifiers, includeClassOrigin, propertyList,
			theTopClass, theClass);
	}
	else
	{
		HandleLocalOnlyAndDeep handler(result, theTopClass, localOnly, deep);
		// don't pass along deep and localOnly flags, because the handler has
		// to take care of it.  m_cimRepository can't do it right, because we
		// can't pass in theTopClass.  We pass false for enumSubClasses, to
		// only do one class.
		m_cimRepository->enumInstances(ns, className, handler, E_DEEP, E_NOT_LOCAL_ONLY,
			includeQualifiers, includeClassOrigin, propertyList, E_DONT_ENUM_SUBCLASSES, context);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	return getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, NULL, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMClass* pOutClass,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::GETINSTANCE, ns, context);
#endif
	return _getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList, pOutClass, context);
}

//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::_getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMClass* pOutClass,
	OperationContext& context)
{
	CIMClass cc = _instGetClass(ns, instanceName.getObjectName(),
		E_NOT_LOCAL_ONLY,
		E_INCLUDE_QUALIFIERS,
		E_INCLUDE_CLASS_ORIGIN,
		0, context);
	if(pOutClass)
	{
		*pOutClass = cc;
	}
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, cc, context);
	CIMInstance ci(CIMNULL);
	if(instancep)
	{
		ci = instancep->getInstance(
			createProvEnvRef(context, m_env),
				ns, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList, cc);
		if (!ci)
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				"Provider erroneously returned a NULL CIMInstance");
		}
		/* don't do this, it's up to the provider ifc/provider to do this correctly.
		ci.syncWithClass(cc, true);
		ci = ci.clone(localOnly, includeQualifiers, includeClassOrigin,
			propertyList);
			*/
	}
	else
	{
		ci = m_cimRepository->getInstance(ns, instanceName, localOnly,
			includeQualifiers, includeClassOrigin, propertyList, context);
	}
	OW_ASSERT(ci);
	
	return ci;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::deleteInstance(const String& ns, const CIMObjectPath& cop_,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::DELETEINSTANCE, ns, context);
#endif
	
	CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);
	if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
	{
		m_env->logDebug(format("CIMServer::deleteInstance.  cop = %1",
			cop.toString()));
	}
	CIMClass theClass(CIMNULL);
	CIMInstance oldInst = _getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		&theClass, context);
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass, context);
	if(instancep)	// If there is an instance provider, let it do the delete.
	{
		instancep->deleteInstance(
			createProvEnvRef(context, m_env), ns, cop);
	}
	else
	{
		// Delete the instance from the instance repository
		m_cimRepository->deleteInstance(ns, cop, context);
	}
	OW_ASSERT(oldInst);
	return oldInst;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
CIMServer::createInstance(
	const String& ns,
	const CIMInstance& ci,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to create the instance
	m_accessMgr->checkAccess(AccessMgr::CREATEINSTANCE, ns, context);
#endif
	String className = ci.getClassName();
	CIMClass theClass = _instGetClass(ns, className, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0,
		context);
	CIMQualifier acq = theClass.getQualifier(
			CIMQualifier::CIM_QUAL_ABSTRACT);
	if(acq)
	{
		if (acq.getValue() == CIMValue(true))
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					format("Unable to create instance because class (%1)"
						" is abstract", theClass.getName()).c_str());
		}
	}
	// Make sure instance jives with class definition
	CIMInstance lci(ci);
	lci.syncWithClass(theClass);
	if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
	{
		m_env->logDebug(format("CIMServer::createInstance.  ns = %1, "
			"instance = %2", ns, lci.toMOF()));
	}
	CIMObjectPath rval(CIMNULL);
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass, context);
	if (instancep)
	{
		rval = instancep->createInstance(createProvEnvRef(context, m_env), ns, lci);
	}
	else
	{
		rval = m_cimRepository->createInstance(ns, lci, context);
	}
	// Prevent lazy providers from causing a problem.
	if (!rval)
	{
		rval = CIMObjectPath(ns, lci);
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::MODIFYINSTANCE, ns, context);
#endif
	CIMInstance oldInst(CIMNULL);
	CIMClass theClass = _instGetClass(ns, modifiedInstance.getClassName(),
		E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, context);
	InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass, context));
	if(!instancep)
	{
		// No instance provider qualifier found
		oldInst = m_cimRepository->modifyInstance(ns, modifiedInstance,
			includeQualifiers, propertyList, context);
	}
	else
	{
		// Look for dynamic instances
		CIMObjectPath cop(ns, modifiedInstance);
		oldInst = _getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL, NULL,
			context);
		instancep->modifyInstance(createProvEnvRef(context, m_env), ns,
			modifiedInstance, oldInst, includeQualifiers, propertyList, theClass);
	}
	OW_ASSERT(oldInst);
	return oldInst;
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool
CIMServer::_instanceExists(const String& ns, const CIMObjectPath& icop,
	OperationContext& context)
{
	try
	{
		_getInstance(ns,icop,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,0,context);
		return true;
	}
	catch(CIMException&)
	{
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMServer::getProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(AccessMgr::GETPROPERTY, ns, context);
#endif
	LocalCIMOMHandle internal_ch(m_env, RepositoryIFCRef(this, true),
		context, LocalCIMOMHandle::E_NO_LOCKING);
	LocalCIMOMHandle real_ch(m_env, RepositoryIFCRef(this, true),
		context, LocalCIMOMHandle::E_NO_LOCKING);
	CIMClass theClass = _instGetClass(ns,name.getObjectName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
	CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}
	CIMInstance ci = _getInstance(ns, name, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		NULL, context);
	CIMProperty prop = ci.getProperty(propertyName);
	if(!prop)
	{
		OW_THROWCIMMSG(CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}
	return prop.getValue();
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::setProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, const CIMValue& valueArg,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::SETPROPERTY, ns, context);
#endif
	CIMClass theClass = _instGetClass(ns, name.getObjectName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
	CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}
	// Ensure value passed in is right data type
	CIMValue cv(valueArg);
	if(cv && (cp.getDataType().getType() != cv.getType()))
	{
		try
		{
			// this throws a FAILED CIMException if it can't convert
			cv = CIMValueCast::castValueToDataType(cv, cp.getDataType());
		}
		catch (CIMException& ce)
		{
			// translate FAILED to TYPE_MISMATCH
			if (ce.getErrNo() == CIMException::FAILED)
			{
				ce.setErrNo(CIMException::TYPE_MISMATCH);
			}
			throw ce;
		}
	}
	CIMInstance ci = _getInstance(ns, name, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		NULL, context);
	if(!ci)
	{
		OW_THROWCIMMSG(CIMException::NOT_FOUND, name.toString().c_str());
	}
	CIMProperty tcp = ci.getProperty(propertyName);
	if(cp.isKey() && tcp.getValue() && !tcp.getValue().equal(cv))
	{
		String msg("Cannot modify key property: ");
		msg += cp.getName();
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
	cp.setValue(cv);
	ci.setProperty(cp);
	modifyInstance(ns, ci, E_INCLUDE_QUALIFIERS, 0, context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMServer::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName, const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(AccessMgr::INVOKEMETHOD, ns, context);
#endif
	CIMClass cc = _getClass(ns, path.getObjectName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
	CIMPropertyArray keys = path.getKeys();
	// If this is an instance, ensure it exists.
	if(keys.size() > 0)
	{
		if(!_instanceExists(ns, path, context))
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND,
				format("Instance not found: %1", path.toString()).c_str());
		}
	}
	// Get the method from the class definition
	CIMMethod method = cc.getMethod(methodName);
	if(!method)
	{
		OW_THROWCIMMSG(CIMException::METHOD_NOT_FOUND, methodName.c_str());
	}
	CIMValue cv(CIMNULL);
	MethodProviderIFCRef methodp;
	CIMClass cctemp(cc);
	try
	{
		methodp = m_provManager->getMethodProvider(
			createProvEnvRef(context, m_env), ns, cctemp, method);
	}
	catch (const NoSuchProviderException&)
	{
	}
	if(!methodp)
	{
		OW_THROWCIMMSG(CIMException::NOT_FOUND, format("No provider for method %1", methodName).c_str());
	}
	CIMParameterArray methodInParams = method.getINParameters();
	if (inParams.size() != methodInParams.size())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"Incorrect number of parameters");
	}
	CIMParameterArray methodOutParams = method.getOUTParameters();
	outParams.resize(methodOutParams.size());
	// set the names on outParams
	for (size_t i = 0; i < methodOutParams.size(); ++i)
	{
		outParams[i].setName(methodOutParams[i].getName());
	}
	CIMParamValueArray orderedParams;
	CIMParamValueArray inParams2(inParams);
	for (size_t i = 0; i < methodInParams.size(); ++i)
	{
		String parameterName = methodInParams[i].getName();
		size_t paramIdx;
		for (paramIdx = 0; paramIdx < inParams2.size(); ++paramIdx)
		{
			if (inParams2[paramIdx].getName().equalsIgnoreCase(parameterName))
			{
				break;
			}
		}
		if (paramIdx == inParams2.size())
		{
			// The parameter wasn't specified.
			// Parameters are optional unless they have a Required(true)
			// qualifier
			if (methodInParams[i].hasTrueQualifier(CIMQualifier::CIM_QUAL_REQUIRED))
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, format(
					"Parameter %1 was not specified.", parameterName).c_str());
			}
			else
			{
				// put a param with a null value
				CIMParamValue optionalParam(methodInParams[i].getName());
				inParams2.push_back(optionalParam);
			}
		}
		// move the param from inParams2 to orderedParams
		orderedParams.push_back(inParams2[paramIdx]);
		inParams2.erase(inParams2.begin() + paramIdx);
		// make sure the type is right
		CIMValue v = orderedParams[i].getValue();
		if (v)
		{
			if (methodInParams[i].getType().getType() != v.getType())
			{
				try
				{
					orderedParams[i].setValue(CIMValueCast::castValueToDataType(
						v, methodInParams[i].getType()));
				}
				catch (CIMException& ce)
				{
					ce.setErrNo(CIMException::INVALID_PARAMETER);
					throw;
				}
			}
		}
		// if the in param is also an out param, assign the value to the out
		// params array
		if (methodInParams[i].hasTrueQualifier(CIMQualifier::CIM_QUAL_OUT))
		{
			size_t j;
			for (j = 0; j < outParams.size(); ++j)
			{
				if (outParams[j].getName() == parameterName)
				{
					outParams[j].setValue(orderedParams[i].getValue());
					break;
				}
			}
			if (j == outParams.size())
			{
				OW_ASSERT(0);
			}
		}
	}
	// all the params should have been moved to orderedParams.  If there are
	// some left, it means we have an unknown/invalid parameter.
	if (inParams2.size() > 0)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, format(
			"Unknown or duplicate parameter: %1", inParams2[0].getName()).c_str());
	}
	StringBuffer methodStr;
	if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
	{
		methodStr += "CIMServer invoking extrinsic method provider: ";
		methodStr += ns;
		methodStr += ':';
		methodStr += path.toString();
		methodStr += '.';
		methodStr += methodName;
		methodStr += '(';
		for (size_t i = 0; i < orderedParams.size(); ++i)
		{
			methodStr += orderedParams[i].getName();
			methodStr += '=';
			methodStr += orderedParams[i].getValue().toString();
			if (i != orderedParams.size() - 1)
			{
				methodStr += ", ";
			}
		}
		methodStr += ')';
		m_env->logDebug(methodStr.toString());
	}
	cv = methodp->invokeMethod(
		createProvEnvRef(context, m_env),
			ns, path, methodName, orderedParams, outParams);
	
	// make sure the type is right on the outParams
	for (size_t i = 0; i < methodOutParams.size(); ++i)
	{
		CIMValue v = outParams[i].getValue();
		if (v)
		{
			if (methodOutParams[i].getType().getType() != v.getType())
			{
				outParams[i].setValue(CIMValueCast::castValueToDataType(
					v, methodOutParams[i].getType()));
			}
		}
	}
	if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
	{
		methodStr.reset();
		methodStr += "CIMServer finished invoking extrinsic method provider: ";
		CIMObjectPath path2(path);
		path2.setNameSpace(ns);
		methodStr += path2.toString();
		methodStr += '.';
		methodStr += methodName;
		methodStr += " OUT Params(";
		for (size_t i = 0; i < outParams.size(); ++i)
		{
			methodStr += outParams[i].getName();
			methodStr += '=';
			methodStr += outParams[i].getValue().toString();
			if (i != outParams.size() - 1)
			{
				methodStr += ", ";
			}
		}
		methodStr += ") return value: ";
		methodStr += cv.toString();
		m_env->logDebug(methodStr.toString());
	}
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
CIMServer::_getInstanceProvider(const String& ns, const CIMClass& cc_, OperationContext& context)
{
	InstanceProviderIFCRef instancep;
	CIMClass cc(cc_);
	try
	{
		instancep =
			m_provManager->getInstanceProvider(createProvEnvRef(context, m_env), ns, cc);
	}
	catch (const NoSuchProviderException& e)
	{
		// This will only happen if the provider qualifier is incorrect
		OW_THROWCIMMSG(CIMException::FAILED,
			format("Invalid provider: %1", e.getMessage()).c_str());
	}
	return instancep;
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
CIMServer::_getAssociatorProvider(const String& ns, const CIMClass& cc_, OperationContext& context)
{
	AssociatorProviderIFCRef ap;
	CIMClass cc(cc_);
	try
	{
		ap =  m_provManager->getAssociatorProvider(
			createProvEnvRef(context, m_env), ns, cc);
	}
	catch (const NoSuchProviderException&)
	{
		// if it's not an instance or associator provider, then ERROR!
		try
		{
			m_provManager->getInstanceProvider(
				createProvEnvRef(context, m_env), ns, cc);
		}
		catch (const NoSuchProviderException& e)
		{
			// This will only happen if the provider qualifier is incorrect
			OW_THROWCIMMSG(CIMException::FAILED,
				format("Invalid provider: %1", e.getMessage()).c_str());
		}
	}
	return ap;
}
#endif
//////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::_getNameSpaceClass(const String& className)
{
	if (className.equalsIgnoreCase("__Namespace"))
	{
		MutexLock l(m_guard);
		if(!m_nsClass_Namespace)
		{
			m_nsClass_Namespace = CIMClass("__Namespace");
			CIMProperty cimProp(CIMProperty::NAME_PROPERTY);
			cimProp.setDataType(CIMDataType::STRING);
			cimProp.addQualifier(CIMQualifier::createKeyQualifier());
			m_nsClass_Namespace.addProperty(cimProp);
		}
		return m_nsClass_Namespace;
	}
	/*
	else if (className.equalsIgnoreCase("CIM_Namespace"))
	{
		if (!m_nsClassCIM_Namespace)
		{
			m_nsClassCIM_Namespace = CIMClass("CIM_Namespace");
		}
		return m_nsClassCIM_Namespace;
	}
	*/
	else
	{
		return CIMClass(CIMNULL);
	}
}
//////////////////////////////////////////////////////////////////////
void
CIMServer::execQuery(
	const String& ns,
	CIMInstanceResultHandlerIFC& result,
	const String &query,
	const String& queryLanguage, OperationContext& context)
{
	WQLIFCRef wql = m_env->getWQLRef();
	if (wql && wql->supportsQueryLanguage(queryLanguage))
	{
		CIMOMHandleIFCRef lch(new LocalCIMOMHandle(m_env,
			RepositoryIFCRef(this, true), context, LocalCIMOMHandle::E_NO_LOCKING));
		try
		{
			wql->evaluate(ns, result, query, queryLanguage, lch);
		}
		catch (const CIMException& ce)
		{
			// translate any error except INVALID_NAMESPACE, INVALID_QUERY,
			// ACCESS_DENIED or FAILED into an INVALID_QUERY
			if (ce.getErrNo() != CIMException::FAILED
				&& ce.getErrNo() != CIMException::INVALID_NAMESPACE
				&& ce.getErrNo() != CIMException::INVALID_QUERY
				&& ce.getErrNo() != CIMException::ACCESS_DENIED)
			{
				// the " " added to the beginning of the message is a little
				// trick to fool the CIMException constructor from stripping
				// away the old "canned" CIMException message.
				throw CIMException(ce.getFile(), ce.getLine(),
					CIMException::INVALID_QUERY,
					String(String(" ") + ce.getMessage()).c_str());
			}
			else
			{
				throw ce;
			}
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::QUERY_LANGUAGE_NOT_SUPPORTED, queryLanguage.c_str());
	}
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::associators(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORS, ns, context);
#endif
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, &result, 0, 0,
		context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::associatorsClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORS, ns, context);
#endif
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, 0, 0, &result,
		context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::associatorNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORNAMES, ns, context);
#endif
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, 0, &result, 0, context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::references(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCES, ns, context);
#endif
	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, &result, 0, 0, context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::referencesClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCES, ns, context);
#endif
	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, 0, 0, &result, context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::referenceNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass, const String& role,
	OperationContext& context)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCENAMES, ns, context);
#endif
	_commonReferences(ns, path, resultClass, role, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, 0, &result, 0,
		context);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class assocClassSeparator : public CIMClassResultHandlerIFC
	{
	public:
		assocClassSeparator(
			StringArray* staticAssocs_,
			CIMClassArray& dynamicAssocs_,
			CIMServer& server_,
			OperationContext& context_,
			const String& ns_,
			const LoggerRef& lgr)
		: staticAssocs(staticAssocs_)
		, dynamicAssocs(dynamicAssocs_)
		, server(server_)
		, context(context_)
		, ns(ns_)
		, logger(lgr)
		{}
	protected:
		virtual void doHandle(const CIMClass &cc)
		{
			if (!cc.isAssociation())
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					format("class %1 is not an association", cc.getName()).c_str());
			}
			// Now separate the association classes that have associator provider from
			// the ones that don't
			if (server._isDynamicAssoc(ns, cc, context))
			{
				dynamicAssocs.push_back(cc);
				logger->logDebug("Found dynamic assoc class: " + cc.getName());
			}
			else if (staticAssocs)
			{
				staticAssocs->push_back(cc.getName());
				logger->logDebug("Found static assoc class: " + cc.getName());
			}
		}
	private:
		StringArray* staticAssocs;
		CIMClassArray& dynamicAssocs;
		CIMServer& server;
		OperationContext& context;
		String ns;
		LoggerRef logger;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::_commonReferences(
	const String& ns,
	const CIMObjectPath& path_,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult,
	OperationContext& context)
{
	CIMObjectPath path(path_);
	path.setNameSpace(ns);
	// Get all association classes from the repository
	// If the result class was specified, only children of it will be
	// returned.
	CIMClassArray dynamicAssocs;
	StringArray resultClassNames;
	assocClassSeparator assocClassResult(!m_realRepository || resultClass.empty() ? 0 : &resultClassNames, dynamicAssocs, *this, context, ns, m_env->getLogger());
	_getAssociationClasses(ns, resultClass, path.getObjectName(), assocClassResult, role, context);
	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		if (m_realRepository)
		{
			if (!resultClass.empty())
			{
				// providers don't do classes, so we'll add the dynamic ones into the list
				for(size_t i = 0; i < dynamicAssocs.size(); i++)
				{
					resultClassNames.append(dynamicAssocs[i].getName());
				}
				SortedVectorSet<String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());
				m_realRepository->_staticReferencesClass(path, &resultClassNamesSet,
					role, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, context);
			}
			else
			{
				m_realRepository->_staticReferencesClass(path, 0,
					role, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, context);
			}
		}
		else
		{
			if (popresult != 0)
			{
				m_cimRepository->referenceNames(ns,path,*popresult,resultClass,role,context);
			}
			else if (pcresult != 0)
			{
				m_cimRepository->referencesClasses(ns,path,*pcresult,resultClass,role,includeQualifiers,includeClassOrigin,propertyList,context);
			}
			else
			{
				OW_ASSERT(0);
			}
		}
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			if (m_realRepository)
			{
				if (!resultClass.empty())
				{
					SortedVectorSet<String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());
					m_realRepository->_staticReferences(path, &resultClassNamesSet, role,
						includeQualifiers, includeClassOrigin, propertyList, *piresult, context);
				}
				else
				{
					m_realRepository->_staticReferences(path, 0, role,
						includeQualifiers, includeClassOrigin, propertyList, *piresult, context);
				}
			}
			else
			{
				m_cimRepository->references(ns,path,*piresult,resultClass,role,includeQualifiers,includeClassOrigin,propertyList,context);
			}
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			if (m_realRepository)
			{
				if (!resultClass.empty())
				{
					SortedVectorSet<String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());
					m_realRepository->_staticReferenceNames(path,
						&resultClassNamesSet, role,
						*popresult);
				}
				else
				{
					m_realRepository->_staticReferenceNames(path,
						0, role,
						*popresult);
				}
			}
			else
			{
				m_cimRepository->referenceNames(ns,path,*popresult,resultClass,role,context);
			}
		}
		else
		{
			OW_ASSERT(0);
		}
		// Process all of the association classes with providers
		_dynamicReferences(path, dynamicAssocs, role, includeQualifiers,
			includeClassOrigin, propertyList, piresult, popresult, context);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::_dynamicReferences(const CIMObjectPath& path,
	const CIMClassArray& assocClasses, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult, OperationContext& context)
{
	// assocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}
	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		CIMClass cc = assocClasses[i];
		AssociatorProviderIFCRef assocP = _getAssociatorProvider(path.getNameSpace(), cc, context);
		if(!assocP)
		{
			continue;
		}
		String resultClass(assocClasses[i].getName());
		// If the object path enumeration pointer is null, then assume we
		// are doing references and not referenceNames
		if(piresult != 0)
		{
			assocP->references(
				createProvEnvRef(context, m_env), *piresult,
				path.getNameSpace(), path, resultClass, role, includeQualifiers,
				includeClassOrigin, propertyList);
		}
		else if (popresult != 0)
		{
			assocP->referenceNames(
				createProvEnvRef(context, m_env), *popresult,
				path.getNameSpace(), path, resultClass, role);
		}
		else
		{
			OW_ASSERT(0);
		}
	}
}
namespace
{
	class classNamesBuilder : public StringResultHandlerIFC
	{
	public:
		classNamesBuilder(StringArray& resultClassNames)
			: m_resultClassNames(resultClassNames)
		{}
		void doHandle(const String& op)
		{
			m_resultClassNames.push_back(op);
		}
	private:
		StringArray& m_resultClassNames;
	};
} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::_commonAssociators(
	const String& ns,
	const CIMObjectPath& path_,
	const String& assocClassName, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult,
	OperationContext& context)
{
	CIMObjectPath path(path_);
	path.setNameSpace(ns);
	// Get association classes from the association repository
	CIMClassArray dynamicAssocs;
	StringArray assocClassNames;
	assocClassSeparator assocClassResult(!m_realRepository || assocClassName.empty() ? 0 : &assocClassNames, dynamicAssocs, *this, context, ns, m_env->getLogger());
	_getAssociationClasses(ns, assocClassName, path.getObjectName(), assocClassResult, role, context);
	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	StringArray resultClassNames;
	if(m_realRepository && !resultClass.empty())
	{
		classNamesBuilder resultClassNamesResult(resultClassNames);
		m_cimRepository->enumClassNames(ns, resultClass, resultClassNamesResult, E_DEEP, context);
		resultClassNames.append(resultClass);
	}
	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		if (m_realRepository)
		{
			SortedVectorSet<String> assocClassNamesSet(assocClassNames.begin(),
					assocClassNames.end());
			SortedVectorSet<String> resultClassNamesSet(resultClassNames.begin(),
					resultClassNames.end());
			m_realRepository->_staticAssociatorsClass(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
				resultClass.empty() ? 0 : &resultClassNamesSet,
				role, resultRole, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, context);
		}
		else
		{
			if (popresult != 0)
			{
				m_cimRepository->associatorNames(ns,path,*popresult,assocClassName,resultClass,role,resultRole,context);
			}
			else if (pcresult != 0)
			{
				m_cimRepository->associatorsClasses(ns,path,*pcresult,assocClassName,resultClass,role,resultRole,includeQualifiers,includeClassOrigin,propertyList,context);
			}
			else
			{
				OW_ASSERT(0);
			}
		}
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			if (m_realRepository)
			{
				SortedVectorSet<String> assocClassNamesSet(assocClassNames.begin(),
						assocClassNames.end());
				SortedVectorSet<String> resultClassNamesSet(resultClassNames.begin(),
						resultClassNames.end());
				m_realRepository->_staticAssociators(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
					resultClass.empty() ? 0 : &resultClassNamesSet, role, resultRole,
					includeQualifiers, includeClassOrigin, propertyList, *piresult, context);
			}
			else
			{
				m_cimRepository->associators(ns,path,*piresult,assocClassName,resultClass,role,resultRole,includeQualifiers,includeClassOrigin,propertyList,context);
			}
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			if (m_realRepository)
			{
				SortedVectorSet<String> assocClassNamesSet(assocClassNames.begin(),
						assocClassNames.end());
				SortedVectorSet<String> resultClassNamesSet(resultClassNames.begin(),
						resultClassNames.end());
				m_realRepository->_staticAssociatorNames(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
					resultClass.empty() ? 0 : &resultClassNamesSet, role, resultRole,
					*popresult);
			}
			else
			{
				m_cimRepository->associatorNames(ns,path,*popresult,assocClassName,resultClass,role,resultRole,context);
			}
		}
		else
		{
			OW_ASSERT(0);
		}
		// Process all of the association classes with providers
		_dynamicAssociators(path, dynamicAssocs, resultClass, role, resultRole,
			includeQualifiers, includeClassOrigin, propertyList, piresult, popresult,
			context);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::_dynamicAssociators(const CIMObjectPath& path,
	const CIMClassArray& assocClasses, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult, OperationContext& context)
{
	// AssocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}
	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		CIMClass cc = assocClasses[i];
		AssociatorProviderIFCRef assocP = _getAssociatorProvider(path.getNameSpace(), cc, context);
		if(!assocP)
		{
			m_env->getLogger()->logError("Failed to get associator provider for class: " + cc.getName());
			continue;
		}
		String assocClass(assocClasses[i].getName());
		if(piresult != 0)
		{
			m_env->getLogger()->logDebug("Calling associators on associator provider for class: " + cc.getName());
			assocP->associators(createProvEnvRef(context, m_env), *piresult, path.getNameSpace(),
				path, assocClass, resultClass, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList);
		}
		else if (popresult != 0)
		{
			m_env->getLogger()->logDebug("Calling associatorNames on associator provider for class: " + cc.getName());
			assocP->associatorNames(createProvEnvRef(context, m_env), *popresult,
				path.getNameSpace(), path, assocClass, resultClass, role, resultRole);
		}
		else
		{
			OW_ASSERT(0);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::_getAssociationClasses(const String& ns,
		const String& assocClassName, const String& className,
		CIMClassResultHandlerIFC& result, const String& role, OperationContext& context)
{
	if(!assocClassName.empty())
	{
		// they gave us a class name so we can use the class association index
		// to only look at the ones that could provide associations
		CIMClass cc = _getClass(ns,assocClassName,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
		result.handle(cc);
		// TODO: measure whether it would be faster to use
		// enumClassNames + getClass() here.
		enumClasses(ns,assocClassName,result,E_DEEP,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN, context);
	}
	else
	{
		// need to get all the assoc classes with dynamic providers
		CIMObjectPath cop(className, ns);
		if (m_realRepository)
		{
			m_realRepository->_staticReferencesClass(cop,0,role,E_INCLUDE_QUALIFIERS,E_EXCLUDE_CLASS_ORIGIN,0,0,&result,context);
		}
		else
		{
			m_cimRepository->referencesClasses(ns,cop,result,assocClassName,role,E_INCLUDE_QUALIFIERS,E_EXCLUDE_CLASS_ORIGIN,0,context);
		}
		// TODO: test if this is faster
		//assocHelper helper(result, m_mStore, ns);
		//m_mStore.getTopLevelAssociations(ns, helper);
	}
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMServer::_isDynamicAssoc(const String& ns, const CIMClass& cc, OperationContext& context)
{
	return _getAssociatorProvider(ns, cc, context) ? true : false;
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

//////////////////////////////////////////////////////////////////////////////
void
CIMServer::beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context)
{
	m_cimRepository->beginOperation(op, context);
}

//////////////////////////////////////////////////////////////////////////////
void
CIMServer::endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result)
{
	m_cimRepository->endOperation(op, context, result);
}


//////////////////////////////////////////////////////////////////////////////
const char* const CIMServer::INST_REPOS_NAME = "instances";
const char* const CIMServer::META_REPOS_NAME = "schema";
const char* const CIMServer::NS_REPOS_NAME = "namespaces";
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
const char* const CIMServer::CLASS_ASSOC_REPOS_NAME = "classassociation";
const char* const CIMServer::INST_ASSOC_REPOS_NAME = "instassociation";
#endif



} // end namespace OpenWBEM

