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
	 * @param aclInfo The ACL info for the user request.
	 */
	void checkAccess(int op, const String& ns, const UserInfo& aclInfo);
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
	const UserInfo& aclInfo)
{
	if (aclInfo.m_internal)
	{
		return;
	}
	UserInfo intACLInfo;
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("Checking access to namespace: \"%1\"", ns));
		m_env->logDebug(format("UserName is: \"%1\" Operation is : %2",
			aclInfo.getUserName(), op));
	}
	String lns(ns);
	while (!lns.empty() && lns[0] == '/')
	{
		lns = lns.substring(1);
	}
	lns.toLowerCase();
	for(;;)
	{
		if (!aclInfo.getUserName().empty())
		{
			String superUser =
				m_env->getConfigItem(ConfigOpts::ACL_SUPERUSER_opt);
			if (superUser.equalsIgnoreCase(aclInfo.getUserName()))
			{
				m_env->logDebug("User is SuperUser: checkAccess returning.");
				return;
			}
			try
			{
				CIMClass cc = m_pServer->getClass("root/security",
					"OpenWBEM_UserACL", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
					intACLInfo);
			}
			catch(CIMException&)
			{
				m_env->logDebug("OpenWBEM_UserACL class non-existent in"
					" /root/security. ACLs disabled");
				return;
			}
			
			CIMObjectPath cop("OpenWBEM_UserACL");
			cop.addKey("username", CIMValue(aclInfo.getUserName()));
			cop.addKey("nspace", CIMValue(lns));
			CIMInstance ci(CIMNULL);
			try
			{
				ci = m_pServer->getInstance("root/security", cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
					NULL, intACLInfo);
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
						m_env->logError(format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							aclInfo.m_userName, lns));
						OW_THROWCIM(CIMException::ACCESS_DENIED);
					}
				}
				else
				{
					if (!capability.equals("rw") && !capability.equals("wr"))
					{
						m_env->logError(format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							aclInfo.m_userName, lns));
						OW_THROWCIM(CIMException::ACCESS_DENIED);
					}
				}
				m_env->logCustInfo(format(
					"ACCESS GRANTED to user \"%1\" for namespace \"%2\"",
					aclInfo.m_userName, lns));
				return;
			}
		}
		// use default policy for namespace
		try
		{
			CIMClass cc = m_pServer->getClass("root/security",
				"OpenWBEM_NamespaceACL", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
				intACLInfo);
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
			ci = m_pServer->getInstance("root/security", cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
				NULL, intACLInfo);
		}
		catch(const CIMException& ce)
		{
			if (m_env->getLogger()->getLogLevel() == DebugLevel)
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
					m_env->logError(format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						aclInfo.m_userName, lns));
					OW_THROWCIM(CIMException::ACCESS_DENIED);
				}
			}
			else
			{
				if (!capability.equals("rw") && !capability.equals("wr"))
				{
					m_env->logError(format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						aclInfo.m_userName, lns));
					OW_THROWCIM(CIMException::ACCESS_DENIED);
				}
			}
			m_env->logCustInfo(format(
				"ACCESS GRANTED to user \"%1\" for namespace \"%2\"",
				aclInfo.m_userName, lns));
			return;
		}
		size_t idx = lns.lastIndexOf('/');
		if (idx == 0 || idx == String::npos)
		{
			break;
		}
		lns = lns.substring(0, idx);
	}
	m_env->logError(format(
		"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
		aclInfo.m_userName, lns));
	OW_THROWCIM(CIMException::ACCESS_DENIED);
}
#endif
//////////////////////////////////////////////////////////////////////////////
CIMServer::CIMServer(CIMOMEnvironmentRef env,
	const ProviderManagerRef& provManager,
	const RepositoryIFCRef& cimRepository)
	: RepositoryIFC()
	, m_provManager(provManager)
	, m_rwSchemaLocker()
	, m_rwInstanceLocker()
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
	const UserInfo& aclInfo)
{
	// Don't need to check ACLs, since this is a result of calling createInstance.
	m_cimRepository->createNameSpace(ns,aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::deleteNameSpace(const String& ns,
	const UserInfo& aclInfo)
{
	// Don't need to check ACLs, since this is a result of calling deleteInstance.
	m_cimRepository->deleteNameSpace(ns,aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumNameSpace(StringResultHandlerIFC& result,
	const UserInfo& aclInfo)
{
	// Don't need to check ACLs, since this is a result of calling enumInstances.
	m_cimRepository->enumNameSpace(result,aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
CIMServer::getQualifierType(const String& ns,
	const String& qualifierName,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::GETQUALIFIER, ns, aclInfo);
#endif
	return m_cimRepository->getQualifierType(ns,qualifierName,aclInfo);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEQUALIFIERS, ns, aclInfo);
#endif
	m_cimRepository->enumQualifierTypes(ns,result,aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::deleteQualifierType(const String& ns, const String& qualName,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::DELETEQUALIFIER, ns, aclInfo);
#endif
	m_cimRepository->deleteQualifierType(ns,qualName,aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::setQualifierType(
	const String& ns,
	const CIMQualifierType& qt, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::SETQUALIFIER, ns, aclInfo);
#endif
	m_cimRepository->setQualifierType(ns,qt,aclInfo);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::getClass(
	const String& ns, const String& className, ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// we don't check for __Namespace, so that clients can get it before they
	// create one.
	if (!className.equalsIgnoreCase("__Namespace"))
	{
		m_accessMgr->checkAccess(AccessMgr::GETCLASS, ns, aclInfo);
	}
#endif
	return _getClass(ns,className, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::_getClass(const String& ns, const String& className, 
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, const UserInfo& aclInfo)
{
	CIMClass theClass = _getNameSpaceClass(className);
	if(!theClass)
	{
		theClass = m_cimRepository->getClass(ns,className,localOnly,
			includeQualifiers,includeClassOrigin,propertyList,aclInfo);
	}
	return theClass;
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::_instGetClass(const String& ns, const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, const UserInfo& aclInfo)
{
	CIMClass theClass = _getNameSpaceClass(className);
	if(!theClass)
	{
		try
		{
			theClass = m_cimRepository->getClass(ns,className,localOnly,
				includeQualifiers,includeClassOrigin,propertyList,aclInfo);
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
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::DELETECLASS, ns, aclInfo);
#endif
	return m_cimRepository->deleteClass(ns,className,aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::createClass(const String& ns, const CIMClass& cimClass,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::CREATECLASS, ns, aclInfo);
#endif
	if(cimClass.getName().equals(CIMClass::NAMESPACECLASS))
	{
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS,
			format("Creation of class %1 is not allowed",
				cimClass.getName()).c_str());
	}
	m_cimRepository->createClass(ns,cimClass,aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMServer::modifyClass(
	const String& ns,
	const CIMClass& cc,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::MODIFYCLASS, ns, aclInfo);
#endif
	return m_cimRepository->modifyClass(ns,cc,aclInfo);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATECLASSES, ns, aclInfo);
#endif
	m_cimRepository->enumClasses(ns,className,result,deep,localOnly,
		includeQualifiers,includeClassOrigin,aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATECLASSNAMES, ns, aclInfo);
#endif
	m_cimRepository->enumClassNames(ns,className,result,deep,aclInfo);
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
			const UserInfo& aclInfo_,
			const CIMOMEnvironmentRef& env_,
			CIMServer* server_)
			: ns(ns_)
			, result(result_)
			, aclInfo(aclInfo_)
			, m_env(env_)
			, server(server_)
		{}
	protected:
		virtual void doHandle(const CIMClass &cc)
		{
			if (m_env->getLogger()->getLogLevel() == DebugLevel)
			{
				m_env->logDebug(format("CIMServer InstNameEnumerator enumerated derived instance names: %1:%2", ns,
					cc.getName()));
			}
			server->_getCIMInstanceNames(ns, cc.getName(), cc, result, aclInfo);
		}
	private:
		String ns;
		CIMObjectPathResultHandlerIFC& result;
		const UserInfo& aclInfo;
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
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEINSTANCENAMES, ns,
		aclInfo);
#endif
	InstNameEnumerator ie(ns, result, aclInfo, m_env, this);
	UserInfo intAclInfo;
	CIMClass theClass = _instGetClass(ns, className,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,intAclInfo);
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
		m_cimRepository->enumClasses(ns,className,ie,deep,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,intAclInfo);
	}
}
/////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMServerProviderEnvironment : public ProviderEnvironmentIFC
	{
	public:
		CIMServerProviderEnvironment(const UserInfo& acl,
			const CIMOMEnvironmentRef& env)
			: m_acl(acl)
			, m_env(env)
		{}
		virtual String getConfigItem(const String &name, const String& defRetVal="") const
		{
			return m_env->getConfigItem(name, defRetVal);
		}
		virtual CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_acl, 
				ServiceEnvironmentIFC::E_DONT_SEND_INDICATIONS, 
				ServiceEnvironmentIFC::E_USE_PROVIDERS, 
				CIMOMEnvironment::E_NO_LOCKING);
		}
		
		virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_acl, 
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
			return m_acl.getUserName();
		}
	private:
		UserInfo m_acl;
		CIMOMEnvironmentRef m_env;
	};
	inline ProviderEnvironmentIFCRef createProvEnvRef(const UserInfo& acl, const CIMOMEnvironmentRef& env)
	{
		return ProviderEnvironmentIFCRef(new CIMServerProviderEnvironment(acl, env));
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
CIMServer::_getCIMInstanceNames(const String& ns, const String& className,
	const CIMClass& theClass, CIMObjectPathResultHandlerIFC& result,
	const UserInfo& aclInfo)
{
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);
	if (instancep)
	{
		instancep->enumInstanceNames(createProvEnvRef(aclInfo, m_env),
			ns, className, result, theClass);
	}
	else
	{
		m_cimRepository->enumInstanceNames(ns,className,result,E_SHALLOW,aclInfo);
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
			const UserInfo& aclInfo_,
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
			, aclInfo(aclInfo_)
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
			if (m_env->getLogger()->getLogLevel() == DebugLevel)
			{
				m_env->logDebug(format("CIMServer InstEnumerator Enumerating derived instance names: %1:%2", ns,
					cc.getName()));
			}
			server->_getCIMInstances(ns, cc.getName(), theTopClass, cc, 
				result, localOnly, deep, includeQualifiers, 
				includeClassOrigin, propertyList, aclInfo);
		}
	private:
		String ns;
		CIMInstanceResultHandlerIFC& result;
		const UserInfo& aclInfo;
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
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEINSTANCES, ns, aclInfo);
#endif
	UserInfo intAclInfo;
	CIMClass theTopClass = _instGetClass(ns, className, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, intAclInfo);
	InstEnumerator ie(ns, result, aclInfo, m_env, this, deep, localOnly, 
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
		m_cimRepository->enumClasses(ns,className,ie,E_DEEP,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,intAclInfo);
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
	const StringArray* propertyList, const UserInfo& aclInfo)
{
	InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass));
	if (instancep)
	{
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("CIMServer calling provider to enumerate instances: %1:%2", ns,
				className));
		}
		// not going to use these, the provider ifc/providers are now responsible for it.
		//HandleLocalOnlyAndDeep handler1(result,theTopClass,localOnly,deep);
		//HandleProviderInstance handler2(includeQualifiers, includeClassOrigin, propertyList, handler1);
		instancep->enumInstances(
			createProvEnvRef(aclInfo, m_env), ns, className, result, localOnly, 
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
			includeQualifiers, includeClassOrigin, propertyList, E_DONT_ENUM_SUBCLASSES, aclInfo);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, const UserInfo& aclInfo)
{
	return getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, NULL, aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMClass* pOutClass,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::GETINSTANCE, ns, aclInfo);
#endif
	CIMClass cc = _instGetClass(ns, instanceName.getObjectName(),
		E_NOT_LOCAL_ONLY,
		E_INCLUDE_QUALIFIERS,
		E_INCLUDE_CLASS_ORIGIN,
		0, aclInfo);
	if(pOutClass)
	{
		*pOutClass = cc;
	}
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, cc);
	CIMInstance ci(CIMNULL);
	if(instancep)
	{
		ci = instancep->getInstance(
			createProvEnvRef(aclInfo, m_env),
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
			includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	}
	OW_ASSERT(ci);
	
	return ci;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::deleteInstance(const String& ns, const CIMObjectPath& cop_,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::DELETEINSTANCE, ns, aclInfo);
#endif
	
	CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("CIMServer::deleteInstance.  cop = %1",
			cop.toString()));
	}
	UserInfo intAclInfo;
	CIMClass theClass(CIMNULL);
	CIMInstance oldInst = getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		&theClass, intAclInfo);
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);
	if(instancep)	// If there is an instance provider, let it do the delete.
	{
		instancep->deleteInstance(
			createProvEnvRef(aclInfo, m_env), ns, cop);
	}
	else
	{
		// Delete the instance from the instance repository
		m_cimRepository->deleteInstance(ns, cop, aclInfo);
	}
	OW_ASSERT(oldInst);
	return oldInst;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
CIMServer::createInstance(
	const String& ns,
	const CIMInstance& ci,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to create the instance
	m_accessMgr->checkAccess(AccessMgr::CREATEINSTANCE, ns, aclInfo);
#endif
	String className = ci.getClassName();
	UserInfo intAclInfo;
	CIMClass theClass = _instGetClass(ns, className, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, 
		intAclInfo);
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
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("CIMServer::createInstance.  ns = %1, "
			"instance = %2", ns, lci.toMOF()));
	}
	CIMObjectPath rval(CIMNULL);
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);
	if (instancep)
	{
		rval = instancep->createInstance(createProvEnvRef(aclInfo, m_env), ns, lci);
	}
	else
	{
		rval = m_cimRepository->createInstance(ns, lci, aclInfo);
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
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::MODIFYINSTANCE, ns, aclInfo);
#endif
	CIMInstance oldInst(CIMNULL);
	UserInfo intAclInfo;
	CIMClass theClass = _instGetClass(ns, modifiedInstance.getClassName(), 
		E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, intAclInfo);
	InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass));
	if(!instancep)
	{
		// No instance provider qualifier found
		oldInst = m_cimRepository->modifyInstance(ns, modifiedInstance, 
			includeQualifiers, propertyList, aclInfo);
	}
	else
	{
		// Look for dynamic instances
		CIMObjectPath cop(ns, modifiedInstance);
		oldInst = getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
			intAclInfo);
		instancep->modifyInstance(createProvEnvRef(aclInfo, m_env), ns,
			modifiedInstance, oldInst, includeQualifiers, propertyList, theClass);
	}
	OW_ASSERT(oldInst);
	return oldInst;
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool
CIMServer::_instanceExists(const String& ns, const CIMObjectPath& icop,
	const UserInfo& aclInfo)
{
	try
	{
		getInstance(ns,icop,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,0,aclInfo);
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
	const String& propertyName, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(AccessMgr::GETPROPERTY, ns, aclInfo);
#endif
	UserInfo intAclInfo;
	LocalCIMOMHandle internal_ch(m_env, RepositoryIFCRef(this, true),
		intAclInfo, LocalCIMOMHandle::E_NO_LOCKING);
	LocalCIMOMHandle real_ch(m_env, RepositoryIFCRef(this, true),
		aclInfo, LocalCIMOMHandle::E_NO_LOCKING);
	CIMClass theClass = _instGetClass(ns,name.getObjectName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,intAclInfo);
	CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}
	CIMInstance ci = getInstance(ns, name, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		NULL, aclInfo);
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
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(AccessMgr::SETPROPERTY, ns, aclInfo);
#endif
	UserInfo intAclInfo;
	CIMClass theClass = _instGetClass(ns, name.getObjectName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,intAclInfo);
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
	CIMInstance ci = getInstance(ns, name, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		NULL, intAclInfo);
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
	modifyInstance(ns, ci, E_INCLUDE_QUALIFIERS, 0, intAclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMServer::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName, const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(AccessMgr::INVOKEMETHOD, ns, aclInfo);
#endif
	UserInfo intAclInfo;
	CIMClass cc = _getClass(ns, path.getObjectName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,intAclInfo);
	CIMPropertyArray keys = path.getKeys();
	// If this is an instance, ensure it exists.
	if(keys.size() > 0)
	{
		if(!_instanceExists(ns, path, aclInfo))
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
	// we need to query for providers for this class and all base classes.
	// One provider may instrument an entire hierarchy of classes.
	// loop until we've got a provider or hit a root class
	while (true)
	{
		try
		{
			methodp = m_provManager->getMethodProvider(
				createProvEnvRef(intAclInfo, m_env), ns, cctemp, method);
		}
		catch (const NoSuchProviderException&)
		{
		}
		String parentClassName = cctemp.getSuperClass();
		if (parentClassName.empty() || methodp)
		{
			// loop until we've got a provider or hit a root class
			break;
		}
		cctemp = _getClass(ns, parentClassName,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,intAclInfo);
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
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
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
		createProvEnvRef(aclInfo, m_env),
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
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
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
CIMServer::_getInstanceProvider(const String& ns, const CIMClass& cc_)
{
	UserInfo intAclInfo;
	InstanceProviderIFCRef instancep;
	CIMClass cc(cc_);
	try
	{
		instancep =
			m_provManager->getInstanceProvider(createProvEnvRef(intAclInfo, m_env), ns, cc);
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
CIMServer::_getAssociatorProvider(const String& ns, const CIMClass& cc_)
{
	UserInfo intAclInfo;
	AssociatorProviderIFCRef ap;
	CIMClass cc(cc_);
	try
	{
		ap =  m_provManager->getAssociatorProvider(
			createProvEnvRef(intAclInfo, m_env), ns, cc);
	}
	catch (const NoSuchProviderException&)
	{
		// if it's not an instance or associator provider, then ERROR!
		try
		{
			m_provManager->getInstanceProvider(
				createProvEnvRef(intAclInfo, m_env), ns, cc);
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
	const String& queryLanguage, const UserInfo& aclInfo)
{
	WQLIFCRef wql = m_env->getWQLRef();
	if (wql && wql->supportsQueryLanguage(queryLanguage))
	{
		CIMOMHandleIFCRef lch(new LocalCIMOMHandle(m_env,
			RepositoryIFCRef(this, true), aclInfo, LocalCIMOMHandle::E_NO_LOCKING));
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
	const StringArray* propertyList, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORS, ns, aclInfo);
#endif
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, &result, 0, 0,
		aclInfo);
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
	const StringArray* propertyList, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORS, ns, aclInfo);
#endif
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, 0, 0, &result,
		aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::associatorNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORNAMES, ns, aclInfo);
#endif
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, 0, &result, 0, aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::references(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCES, ns, aclInfo);
#endif
	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, &result, 0, 0, aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::referencesClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCES, ns, aclInfo);
#endif
	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, 0, 0, &result, aclInfo);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::referenceNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass, const String& role,
	const UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCENAMES, ns, aclInfo);
#endif
	_commonReferences(ns, path, resultClass, role, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, 0, &result, 0,
		aclInfo);
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
			const UserInfo& aclInfo_,
			const String& ns_,
			const LoggerRef& lgr)
		: staticAssocs(staticAssocs_)
		, dynamicAssocs(dynamicAssocs_)
		, server(server_)
		, aclInfo(aclInfo_)
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
			if (server._isDynamicAssoc(ns, cc))
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
		const UserInfo& aclInfo;
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
	const UserInfo& aclInfo)
{
	CIMObjectPath path(path_);
	path.setNameSpace(ns);
	// Get all association classes from the repository
	// If the result class was specified, only children of it will be
	// returned.
	CIMClassArray dynamicAssocs;
	StringArray resultClassNames;
	assocClassSeparator assocClassResult(!m_realRepository || resultClass.empty() ? 0 : &resultClassNames, dynamicAssocs, *this, aclInfo, ns, m_env->getLogger());
	_getAssociationClasses(ns, resultClass, path.getObjectName(), assocClassResult, role);
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
					role, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, aclInfo);
			}
			else
			{
				m_realRepository->_staticReferencesClass(path, 0,
					role, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, aclInfo);
			}
		}
		else
		{
			if (popresult != 0)
			{
				m_cimRepository->referenceNames(ns,path,*popresult,resultClass,role,aclInfo);
			}
			else if (pcresult != 0)
			{
				m_cimRepository->referencesClasses(ns,path,*pcresult,resultClass,role,includeQualifiers,includeClassOrigin,propertyList,aclInfo);
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
						includeQualifiers, includeClassOrigin, propertyList, *piresult, aclInfo);
				}
				else
				{
					m_realRepository->_staticReferences(path, 0, role,
						includeQualifiers, includeClassOrigin, propertyList, *piresult, aclInfo);
				}
			}
			else
			{
				m_cimRepository->references(ns,path,*piresult,resultClass,role,includeQualifiers,includeClassOrigin,propertyList,aclInfo);
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
				m_cimRepository->referenceNames(ns,path,*popresult,resultClass,role,aclInfo);
			}
		}
		else
		{
			OW_ASSERT(0);
		}
		// Process all of the association classes with providers
		_dynamicReferences(path, dynamicAssocs, role, includeQualifiers,
			includeClassOrigin, propertyList, piresult, popresult, aclInfo);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::_dynamicReferences(const CIMObjectPath& path,
	const CIMClassArray& assocClasses, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult, const UserInfo& aclInfo)
{
	// assocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}
	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		CIMClass cc = assocClasses[i];
		AssociatorProviderIFCRef assocP = _getAssociatorProvider(path.getNameSpace(), cc);
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
				createProvEnvRef(aclInfo, m_env), *piresult,
				path.getNameSpace(), path, resultClass, role, includeQualifiers,
				includeClassOrigin, propertyList);
		}
		else if (popresult != 0)
		{
			assocP->referenceNames(
				createProvEnvRef(aclInfo, m_env), *popresult,
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
	const UserInfo& aclInfo)
{
	CIMObjectPath path(path_);
	path.setNameSpace(ns);
	// Get association classes from the association repository
	CIMClassArray dynamicAssocs;
	StringArray assocClassNames;
	assocClassSeparator assocClassResult(!m_realRepository || assocClassName.empty() ? 0 : &assocClassNames, dynamicAssocs, *this, aclInfo, ns, m_env->getLogger());
	_getAssociationClasses(ns, assocClassName, path.getObjectName(), assocClassResult, role);
	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	StringArray resultClassNames;
	if(m_realRepository && !resultClass.empty())
	{
		classNamesBuilder resultClassNamesResult(resultClassNames);
		m_cimRepository->enumClassNames(ns, resultClass, resultClassNamesResult, E_DEEP, aclInfo);
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
				role, resultRole, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, aclInfo);
		}
		else
		{
			if (popresult != 0)
			{
				m_cimRepository->associatorNames(ns,path,*popresult,assocClassName,resultClass,role,resultRole,aclInfo);
			}
			else if (pcresult != 0)
			{
				m_cimRepository->associatorsClasses(ns,path,*pcresult,assocClassName,resultClass,role,resultRole,includeQualifiers,includeClassOrigin,propertyList,aclInfo);
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
					includeQualifiers, includeClassOrigin, propertyList, *piresult, aclInfo);
			}
			else
			{
				m_cimRepository->associators(ns,path,*piresult,assocClassName,resultClass,role,resultRole,includeQualifiers,includeClassOrigin,propertyList,aclInfo);
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
				m_cimRepository->associatorNames(ns,path,*popresult,assocClassName,resultClass,role,resultRole,aclInfo);
			}
		}
		else
		{
			OW_ASSERT(0);
		}
		// Process all of the association classes with providers
		_dynamicAssociators(path, dynamicAssocs, resultClass, role, resultRole,
			includeQualifiers, includeClassOrigin, propertyList, piresult, popresult,
			aclInfo);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::_dynamicAssociators(const CIMObjectPath& path,
	const CIMClassArray& assocClasses, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult, const UserInfo& aclInfo)
{
	// AssocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}
	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		CIMClass cc = assocClasses[i];
		AssociatorProviderIFCRef assocP = _getAssociatorProvider(path.getNameSpace(), cc);
		if(!assocP)
		{
			m_env->getLogger()->logError("Failed to get associator provider for class: " + cc.getName());
			continue;
		}
		String assocClass(assocClasses[i].getName());
		if(piresult != 0)
		{
			m_env->getLogger()->logDebug("Calling associators on associator provider for class: " + cc.getName());
			assocP->associators(createProvEnvRef(aclInfo, m_env), *piresult, path.getNameSpace(),
				path, assocClass, resultClass, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList);
		}
		else if (popresult != 0)
		{
			m_env->getLogger()->logDebug("Calling associatorNames on associator provider for class: " + cc.getName());
			assocP->associatorNames(createProvEnvRef(aclInfo, m_env), *popresult,
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
		CIMClassResultHandlerIFC& result, const String& role)
{
	UserInfo intAclInfo;
	if(!assocClassName.empty())
	{
		// they gave us a class name so we can use the class association index
		// to only look at the ones that could provide associations
		CIMClass cc = getClass(ns,assocClassName,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,intAclInfo);
		result.handle(cc);
		// TODO: measure whether it would be faster to use
		// enumClassNames + getClass() here.
		enumClasses(ns,assocClassName,result,E_DEEP,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN, intAclInfo);
	}
	else
	{
		// need to get all the assoc classes with dynamic providers
		CIMObjectPath cop(className, ns);
		if (m_realRepository)
		{
			m_realRepository->_staticReferencesClass(cop,0,role,E_INCLUDE_QUALIFIERS,E_EXCLUDE_CLASS_ORIGIN,0,0,&result,intAclInfo);
		}
		else
		{
			m_cimRepository->referencesClasses(ns,cop,result,assocClassName,role,E_INCLUDE_QUALIFIERS,E_EXCLUDE_CLASS_ORIGIN,0,intAclInfo);
		}
		// TODO: test if this is faster
		//assocHelper helper(result, m_mStore, ns);
		//m_mStore.getTopLevelAssociations(ns, helper);
	}
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMServer::_isDynamicAssoc(const String& ns, const CIMClass& cc)
{
	return _getAssociatorProvider(ns, cc) ? true : false;
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
const char* const CIMServer::INST_REPOS_NAME = "instances";
const char* const CIMServer::META_REPOS_NAME = "schema";
const char* const CIMServer::NS_REPOS_NAME = "namespaces";
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
const char* const CIMServer::CLASS_ASSOC_REPOS_NAME = "classassociation";
const char* const CIMServer::INST_ASSOC_REPOS_NAME = "instassociation";
#endif

} // end namespace OpenWBEM

