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

#if !defined(OW_DISABLE_ACLS)
class OW_AccessMgr
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

	OW_AccessMgr(OW_CIMServer* pServer, OW_CIMOMEnvironmentRef env);

	/**
	 * checkAccess will check that access is granted through the ACL. If
	 * Access is not granted, an OW_CIMException will be thrown.
	 * @param op	The operation that access is being checked for.
	 * @param ns	The name space that access is being check on.
	 * @param aclInfo The ACL info for the user request.
	 */
	void checkAccess(int op, const OW_String& ns, const OW_UserInfo& aclInfo);

private:

	OW_String getMethodType(int op);
	OW_CIMServer* m_pServer;
	OW_CIMOMEnvironmentRef m_env;
};

//////////////////////////////////////////////////////////////////////////////
OW_AccessMgr::OW_AccessMgr(OW_CIMServer* pServer,
	OW_CIMOMEnvironmentRef env)
	: m_pServer(pServer)
	, m_env(env)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_AccessMgr::getMethodType(int op)
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
			return OW_String("r");
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
			return OW_String("w");
		case INVOKEMETHOD:
			return OW_String("rw");
		default:
			OW_THROW(OW_Exception, "Unknown operation type passed to "
				"AccessMgr.  This shouldn't happen");
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AccessMgr::checkAccess(int op, const OW_String& ns,
	const OW_UserInfo& aclInfo)
{
	if (aclInfo.m_internal)
	{
		return;
	}
	OW_UserInfo intACLInfo;

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("Checking access to namespace: \"%1\"", ns));
		m_env->logDebug(format("UserName is: \"%1\" Operation is : %2",
			aclInfo.getUserName(), op));
	}

	OW_String lns(ns);
	while (!lns.empty() && lns[0] == '/')
	{
		lns = lns.substring(1);
	}
	lns.toLowerCase();

	for(;;)
	{
		if (!aclInfo.getUserName().empty())
		{
			OW_String superUser =
				m_env->getConfigItem(OW_ConfigOpts::ACL_SUPERUSER_opt);

			if (superUser.equalsIgnoreCase(aclInfo.getUserName()))
			{
				m_env->logDebug("User is SuperUser: checkAccess returning.");
				return;
			}

			try
			{
				OW_CIMClass cc = m_pServer->getClass("root/security",
					"OpenWBEM_UserACL", false, true, true, NULL,
					intACLInfo);
			}
			catch(OW_CIMException&)
			{
				m_env->logDebug("OpenWBEM_UserACL class non-existent in"
					" /root/security. ACLs disabled");
				return;
			}
			
			OW_CIMObjectPath cop("OpenWBEM_UserACL");
			cop.addKey("username", OW_CIMValue(aclInfo.getUserName()));
			cop.addKey("nspace", OW_CIMValue(lns));
			OW_CIMInstance ci(OW_CIMNULL);
			try
			{
				ci = m_pServer->getInstance("root/security", cop, false, true, true, NULL,
					NULL, intACLInfo);
			}
			catch(const OW_CIMException&)
			{
				ci.setNull();
			}
			if (ci)
			{
				OW_String capability;
				OW_CIMProperty capabilityProp = ci.getProperty("capability");
				if (capabilityProp)
				{
					OW_CIMValue cv = capabilityProp.getValue();
					if (cv)
					{
						capability = cv.toString();
					}
				}
				OW_String opType = getMethodType(op);
				capability.toLowerCase();
				if (opType.length() == 1)
				{
					if (capability.indexOf(opType) == OW_String::npos)
					{
						m_env->logError(format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							aclInfo.m_userName, lns));
						OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
					}
				}
				else
				{
					if (!capability.equals("rw") && !capability.equals("wr"))
					{
						m_env->logError(format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							aclInfo.m_userName, lns));
						OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
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
			OW_CIMClass cc = m_pServer->getClass("root/security",
				"OpenWBEM_NamespaceACL", false, true, true, NULL,
				intACLInfo);
		}
		catch(OW_CIMException&)
		{
			m_env->logDebug("OpenWBEM_NamespaceACL class non-existent in"
				" /root/security. namespace ACLs disabled");
			return;
		}
		OW_CIMObjectPath cop("OpenWBEM_NamespaceACL");
		cop.addKey("nspace", OW_CIMValue(lns));
		OW_CIMInstance ci(OW_CIMNULL);
		try
		{
			ci = m_pServer->getInstance("root/security", cop, false, true, true, NULL,
				NULL, intACLInfo);
		}
		catch(const OW_CIMException& ce)
		{
			if (m_env->getLogger()->getLogLevel() == DebugLevel)
			{
				m_env->logDebug(format("Caught exception: %1 in"
					" OW_AccessMgr::checkAccess", ce));
			}
			ci.setNull();
		}
	
		if (ci)
		{
			OW_String capability;
			OW_CIMProperty capabilityProp = ci.getProperty("capability");
			if (capabilityProp)
			{
				OW_CIMValue v = capabilityProp.getValue();
				if (v)
				{
					capability = v.toString();
				}
			}
			capability.toLowerCase();

			OW_String opType = getMethodType(op);

			if (opType.length() == 1)
			{
				if (capability.indexOf(opType) == OW_String::npos)
				{
					m_env->logError(format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						aclInfo.m_userName, lns));
					OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
				}
			}
			else
			{
				if (!capability.equals("rw") && !capability.equals("wr"))
				{
					m_env->logError(format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						aclInfo.m_userName, lns));
					OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
				}
			}

			m_env->logCustInfo(format(
				"ACCESS GRANTED to user \"%1\" for namespace \"%2\"",
				aclInfo.m_userName, lns));
			return;

		}
		size_t idx = lns.lastIndexOf('/');
		if (idx == 0 || idx == OW_String::npos)
		{
			break;
		}
		lns = lns.substring(0, idx);
	}
	m_env->logError(format(
		"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
		aclInfo.m_userName, lns));
	OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
}
#endif

//////////////////////////////////////////////////////////////////////////////
OW_CIMServer::OW_CIMServer(OW_CIMOMEnvironmentRef env,
	const OW_ProviderManagerRef& provManager,
	const OW_RepositoryIFCRef& cimRepository)
	: OW_RepositoryIFC()
	, m_provManager(provManager)
	, m_rwSchemaLocker()
	, m_rwInstanceLocker()
#if !defined(OW_DISABLE_ACLS)
	, m_accessMgr(new OW_AccessMgr(this, env))
#endif
	, m_nsClass_Namespace(OW_CIMNULL)
	, m_env(env)
	, m_cimRepository(cimRepository)
	, m_realRepository(m_cimRepository.cast_to<OW_CIMRepository>())
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMServer::~OW_CIMServer()
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
OW_CIMServer::open(const OW_String& path)
{
	(void)path;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::close()
{
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::createNameSpace(const OW_String& ns,
	const OW_UserInfo& aclInfo)
{
	// Don't need to check ACLs, since this is a result of calling createInstance.
	m_cimRepository->createNameSpace(ns,aclInfo);

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::deleteNameSpace(const OW_String& ns,
	const OW_UserInfo& aclInfo)
{
	// Don't need to check ACLs, since this is a result of calling deleteInstance.
	m_cimRepository->deleteNameSpace(ns,aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumNameSpace(OW_StringResultHandlerIFC& result,
	const OW_UserInfo& aclInfo)
{
	// Don't need to check ACLs, since this is a result of calling enumInstances.
	m_cimRepository->enumNameSpace(result,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMServer::getQualifierType(const OW_String& ns,
	const OW_String& qualifierName,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::GETQUALIFIER, ns, aclInfo);
#endif

	return m_cimRepository->getQualifierType(ns,qualifierName,aclInfo);
}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEQUALIFIERS, ns, aclInfo);
#endif
	m_cimRepository->enumQualifierTypes(ns,result,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::deleteQualifierType(const OW_String& ns, const OW_String& qualName,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::DELETEQUALIFIER, ns, aclInfo);
#endif

	m_cimRepository->deleteQualifierType(ns,qualName,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::setQualifierType(
	const OW_String& ns,
	const OW_CIMQualifierType& qt, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::SETQUALIFIER, ns, aclInfo);
#endif

	m_cimRepository->setQualifierType(ns,qt,aclInfo);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION


//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::getClass(
	const OW_String& ns, const OW_String& className, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// we don't check for __Namespace, so that clients can get it before they
	// create one.
	if (!className.equalsIgnoreCase("__Namespace"))
	{
		m_accessMgr->checkAccess(OW_AccessMgr::GETCLASS, ns, aclInfo);
	}
#endif

	return _getClass(ns,className, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::_getClass(const OW_String& ns, const OW_String& className, 
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	OW_CIMClass theClass = _getNameSpaceClass(className);
	if(!theClass)
	{
		theClass = m_cimRepository->getClass(ns,className,localOnly,
			includeQualifiers,includeClassOrigin,propertyList,aclInfo);
	}

	return theClass;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::_instGetClass(const OW_String& ns, const OW_String& className,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	OW_CIMClass theClass = _getNameSpaceClass(className);
	if(!theClass)
	{
		try
		{
			theClass = m_cimRepository->getClass(ns,className,localOnly,
				includeQualifiers,includeClassOrigin,propertyList,aclInfo);
		}
		catch (OW_CIMException& e)
		{
			if (e.getErrNo() == OW_CIMException::NOT_FOUND)
			{
				e.setErrNo(OW_CIMException::INVALID_CLASS);
			}
			throw e;
		}
	}

	return theClass;
}

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::DELETECLASS, ns, aclInfo);
#endif

	return m_cimRepository->deleteClass(ns,className,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::createClass(const OW_String& ns, const OW_CIMClass& cimClass,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::CREATECLASS, ns, aclInfo);
#endif


	if(cimClass.getName().equals(OW_CIMClass::NAMESPACECLASS))
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS,
			format("Creation of class %1 is not allowed",
				cimClass.getName()).c_str());
	}
	m_cimRepository->createClass(ns,cimClass,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::modifyClass(
	const OW_String& ns,
	const OW_CIMClass& cc,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::MODIFYCLASS, ns, aclInfo);
#endif

	return m_cimRepository->modifyClass(ns,cc,aclInfo);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATECLASSES, ns, aclInfo);
#endif

	m_cimRepository->enumClasses(ns,className,result,deep,localOnly,
		includeQualifiers,includeClassOrigin,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumClassNames(
	const OW_String& ns,
	const OW_String& className,
	OW_StringResultHandlerIFC& result,
	OW_Bool deep, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATECLASSNAMES, ns, aclInfo);
#endif

	m_cimRepository->enumClassNames(ns,className,result,deep,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class InstNameEnumerator : public OW_CIMClassResultHandlerIFC
	{
	public:
		InstNameEnumerator(
			const OW_String& ns_,
			OW_CIMObjectPathResultHandlerIFC& result_,
			const OW_UserInfo& aclInfo_,
			const OW_CIMOMEnvironmentRef& env_,
			OW_CIMServer* server_)
			: ns(ns_)
			, result(result_)
			, aclInfo(aclInfo_)
			, m_env(env_)
			, server(server_)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &cc)
		{
			if (m_env->getLogger()->getLogLevel() == DebugLevel)
			{
				m_env->logDebug(format("OW_CIMServer InstNameEnumerator enumerated derived instance names: %1:%2", ns,
					cc.getName()));
			}
			server->_getCIMInstanceNames(ns, cc.getName(), cc, result, aclInfo);
		}
	private:
		OW_String ns;
		OW_CIMObjectPathResultHandlerIFC& result;
		const OW_UserInfo& aclInfo;
		const OW_CIMOMEnvironmentRef& m_env;
		OW_CIMServer* server;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumInstanceNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEINSTANCENAMES, ns,
		aclInfo);
#endif


	InstNameEnumerator ie(ns, result, aclInfo, m_env, this);

	OW_UserInfo intAclInfo;

	OW_CIMClass theClass = _instGetClass(ns, className,false,true,true,0,intAclInfo);
	ie.handle(theClass);

	// If this is the namespace class then just return now
	if(className.equalsIgnoreCase(OW_CIMClass::NAMESPACECLASS)
		|| !deep)
	{
		return;
	}
	else
	{
		// TODO: measure whether it would be faster to use
		// enumClassNames + getClass() here.
		m_cimRepository->enumClasses(ns,className,ie,deep,false,true,true,intAclInfo);
	}

}

/////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMServerProviderEnvironment : public OW_ProviderEnvironmentIFC
	{
	public:

		CIMServerProviderEnvironment(const OW_UserInfo& acl,
			const OW_CIMOMEnvironmentRef& env)
			: m_acl(acl)
			, m_env(env)
		{}

		virtual OW_String getConfigItem(const OW_String &name, const OW_String& defRetVal="") const
		{
			return m_env->getConfigItem(name, defRetVal);
		}

		virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_acl, false, false, true);
		}
		
		virtual OW_CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			return m_env->getCIMOMHandle(m_acl, false, true, true);
		}
		
		virtual OW_RepositoryIFCRef getRepository() const
		{
			return m_env->getRepository();
		}

		virtual OW_LoggerRef getLogger() const
		{
			return m_env->getLogger();
		}

		virtual OW_String getUserName() const
		{
			return m_acl.getUserName();
		}

	private:
		OW_UserInfo m_acl;
		OW_CIMOMEnvironmentRef m_env;
	};

	inline OW_ProviderEnvironmentIFCRef createProvEnvRef(const OW_UserInfo& acl, const OW_CIMOMEnvironmentRef& env)
	{
		return OW_ProviderEnvironmentIFCRef(new CIMServerProviderEnvironment(acl, env));
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMServer::_getCIMInstanceNames(const OW_String& ns, const OW_String& className,
	const OW_CIMClass& theClass, OW_CIMObjectPathResultHandlerIFC& result,
	const OW_UserInfo& aclInfo)
{
	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);
	if (instancep)
	{
		instancep->enumInstanceNames(createProvEnvRef(aclInfo, m_env),
			ns, className, result, theClass);
	}
	else
	{
		m_cimRepository->enumInstanceNames(ns,className,result,false,aclInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class InstEnumerator : public OW_CIMClassResultHandlerIFC
	{
	public:
		InstEnumerator(
			const OW_String& ns_,
			OW_CIMInstanceResultHandlerIFC& result_,
			const OW_UserInfo& aclInfo_,
			const OW_CIMOMEnvironmentRef& env_,
			OW_CIMServer* server_, 
			OW_Bool deep_,
			OW_Bool localOnly_, 
			OW_Bool includeQualifiers_, 
			OW_Bool includeClassOrigin_,
			const OW_StringArray* propertyList_,
			const OW_CIMClass& theTopClass_)
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
		virtual void doHandle(const OW_CIMClass &cc)
		{
			if (m_env->getLogger()->getLogLevel() == DebugLevel)
			{
				m_env->logDebug(format("OW_CIMServer InstEnumerator Enumerating derived instance names: %1:%2", ns,
					cc.getName()));
			}
			server->_getCIMInstances(ns, cc.getName(), theTopClass, cc, 
				result, localOnly, deep, includeQualifiers, 
				includeClassOrigin, propertyList, aclInfo);
		}
	private:
		OW_String ns;
		OW_CIMInstanceResultHandlerIFC& result;
		const OW_UserInfo& aclInfo;
		const OW_CIMOMEnvironmentRef& m_env;
		OW_CIMServer* server;
		OW_Bool deep;
		OW_Bool localOnly;
		OW_Bool includeQualifiers;
		OW_Bool includeClassOrigin;
		const OW_StringArray* propertyList;
		const OW_CIMClass& theTopClass;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_Bool enumSubClasses, 
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEINSTANCES, ns, aclInfo);
#endif

	OW_UserInfo intAclInfo;
	OW_CIMClass theTopClass = _instGetClass(ns, className, false, true, true, 0, intAclInfo);

	InstEnumerator ie(ns, result, aclInfo, m_env, this, deep, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList, theTopClass);

	ie.handle(theTopClass);

	// If this is the namespace class then only do one class
	if(theTopClass.getName().equals(OW_CIMClass::NAMESPACECLASS) || enumSubClasses == false)
	{
		return;
	}
	else
	{
		// TODO: measure whether it would be faster to use
		// enumClassNames + getClass() here.
		// do subclasses
		m_cimRepository->enumClasses(ns,className,ie,true,false,true,true,intAclInfo);
	}
}

namespace
{
	/* not going to use this -- the providers are now responsible for their own behavior
	class HandleProviderInstance : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		HandleProviderInstance(
			bool includeQualifiers_, bool includeClassOrigin_,
			const OW_StringArray* propList_,
			OW_CIMInstanceResultHandlerIFC& result_)
		: includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, result(result_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &ci)
		{
			result.handle(ci.clone(false, includeQualifiers,
				includeClassOrigin, propList));
		}
	private:
		bool includeQualifiers, includeClassOrigin;
		const OW_StringArray* propList;
		OW_CIMInstanceResultHandlerIFC& result;
	};
	*/

	class HandleLocalOnlyAndDeep : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		HandleLocalOnlyAndDeep(
			OW_CIMInstanceResultHandlerIFC& result_,
			const OW_CIMClass& requestedClass_,
			bool localOnly_,
			bool deep_)
		: result(result_)
		, requestedClass(requestedClass_)
		, localOnly(localOnly_)
		, deep(deep_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &inst)
		{
			if (deep == true && localOnly == false) // don't filter anything
			{
				result.handle(inst);
				return;
			}

			OW_CIMPropertyArray props = inst.getProperties();
			OW_CIMPropertyArray newprops;
			OW_CIMInstance newInst(inst);
			OW_String requestedClassName = requestedClass.getName();
			for (size_t i = 0; i < props.size(); ++i)
			{
				OW_CIMProperty p = props[i];
				OW_CIMProperty clsp = requestedClass.getProperty(p.getName());
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
		OW_CIMInstanceResultHandlerIFC& result;
		const OW_CIMClass& requestedClass;
		bool localOnly;
		bool deep;
	};
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMServer::_getCIMInstances(
	const OW_String& ns,
	const OW_String& className,
	const OW_CIMClass& theTopClass,
	const OW_CIMClass& theClass, OW_CIMInstanceResultHandlerIFC& result, 
	OW_Bool localOnly, OW_Bool deep, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{

	OW_InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass));
	if (instancep)
	{
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("OW_CIMServer calling provider to enumerate instances: %1:%2", ns,
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
		m_cimRepository->enumInstances(ns, className, handler, true, false,
			includeQualifiers, includeClassOrigin, propertyList, false, aclInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	return getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, NULL, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMClass* pOutClass,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::GETINSTANCE, ns, aclInfo);
#endif

	OW_CIMClass cc = _instGetClass(ns, instanceName.getObjectName(),
		OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
		OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS,
		OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN,
		0, aclInfo);

	if(pOutClass)
	{
		*pOutClass = cc;
	}

	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(ns, cc);

	OW_CIMInstance ci(OW_CIMNULL);
	if(instancep)
	{
		ci = instancep->getInstance(
			createProvEnvRef(aclInfo, m_env),
				ns, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList, cc);
		if (!ci)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
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
OW_CIMInstance
OW_CIMServer::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& cop_,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::DELETEINSTANCE, ns, aclInfo);
#endif
	
	OW_CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMServer::deleteInstance.  cop = %1",
			cop.toString()));
	}

	OW_UserInfo intAclInfo;
	OW_CIMClass theClass(OW_CIMNULL);
	OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
		&theClass, intAclInfo);

	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);

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
OW_CIMObjectPath
OW_CIMServer::createInstance(
	const OW_String& ns,
	const OW_CIMInstance& ci,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to create the instance
	m_accessMgr->checkAccess(OW_AccessMgr::CREATEINSTANCE, ns, aclInfo);
#endif

	OW_String className = ci.getClassName();

	OW_UserInfo intAclInfo;

	OW_CIMClass theClass = _instGetClass(ns, className, false, true, true, 0, 
		intAclInfo);

	OW_CIMQualifier acq = theClass.getQualifier(
			OW_CIMQualifier::CIM_QUAL_ABSTRACT);
	if(acq)
	{
		if (acq.getValue() == OW_CIMValue(true))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("Unable to create instance because class (%1)"
						" is abstract", theClass.getName()).c_str());
		}
	}

	// Make sure instance jives with class definition
	OW_CIMInstance lci(ci);
	lci.syncWithClass(theClass, false);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMServer::createInstance.  ns = %1, "
			"instance = %2", ns, lci.toMOF()));
	}

	OW_CIMObjectPath rval(OW_CIMNULL);
	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);
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
		rval = OW_CIMObjectPath(lci);
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	const OW_StringArray* propertyList,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::MODIFYINSTANCE, ns, aclInfo);
#endif

	OW_CIMInstance oldInst(OW_CIMNULL);

	OW_UserInfo intAclInfo;
	OW_CIMClass theClass = _instGetClass(ns, modifiedInstance.getClassName(), 
		false, true, true, 0, intAclInfo);

	OW_InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass));
	if(!instancep)
	{
		// No instance provider qualifier found
		oldInst = m_cimRepository->modifyInstance(ns, modifiedInstance, 
			includeQualifiers, propertyList, aclInfo);
	}
	else
	{
		// Look for dynamic instances
		OW_CIMObjectPath cop(modifiedInstance);
		oldInst = getInstance(ns, cop, false, true, true, NULL,
			intAclInfo);

		instancep->modifyInstance(createProvEnvRef(aclInfo, m_env), ns,
			modifiedInstance, oldInst, includeQualifiers, propertyList, theClass);
	}

	OW_ASSERT(oldInst);
	return oldInst;

}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMServer::_instanceExists(const OW_String& ns, const OW_CIMObjectPath& icop,
	const OW_UserInfo& aclInfo)
{
	try
	{
		getInstance(ns,icop,false,true,true,0,0,aclInfo);
		return true;
	}
	catch(OW_CIMException&)
	{
		return false;
	}

}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMServer::getProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::GETPROPERTY, ns, aclInfo);
#endif

	OW_UserInfo intAclInfo;
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	OW_CIMClass theClass = _instGetClass(ns,name.getObjectName(),false,true,true,0,intAclInfo);

	OW_CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}

	OW_CIMInstance ci = getInstance(ns, name, false, true, true, NULL,
		NULL, aclInfo);
	OW_CIMProperty prop = ci.getProperty(propertyName);
	if(!prop)
	{
		OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}

	return prop.getValue();
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& valueArg,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	m_accessMgr->checkAccess(OW_AccessMgr::SETPROPERTY, ns, aclInfo);
#endif

	OW_UserInfo intAclInfo;
	OW_CIMClass theClass = _instGetClass(ns, name.getObjectName(),false,true,true,0,intAclInfo);

	OW_CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}

	// Ensure value passed in is right data type
	OW_CIMValue cv(valueArg);
	if(cv && (cp.getDataType().getType() != cv.getType()))
	{
		try
		{
			// this throws a FAILED CIMException if it can't convert
			cv = OW_CIMValueCast::castValueToDataType(cv, cp.getDataType());
		}
		catch (OW_CIMException& ce)
		{
			// translate FAILED to TYPE_MISMATCH
			if (ce.getErrNo() == OW_CIMException::FAILED)
			{
				ce.setErrNo(OW_CIMException::TYPE_MISMATCH);
			}
			throw ce;
		}
	}

	OW_CIMInstance ci = getInstance(ns, name, false, true, true, NULL,
		NULL, intAclInfo);

	if(!ci)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, name.toString().c_str());
	}

	OW_CIMProperty tcp = ci.getProperty(propertyName);

	if(cp.isKey() && tcp.getValue() && !tcp.getValue().equal(cv))
	{
		OW_String msg("Cannot modify key property: ");
		msg += cp.getName();
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}

	cp.setValue(cv);
	ci.setProperty(cp);
	modifyInstance(ns, ci, true, 0, intAclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMServer::invokeMethod(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	const OW_String& methodName, const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::INVOKEMETHOD, ns, aclInfo);
#endif

	OW_UserInfo intAclInfo;

	OW_CIMClass cc = _getClass(ns, path.getObjectName(),false,true,true,0,intAclInfo);

	OW_CIMPropertyArray keys = path.getKeys();

	// If this is an instance, ensure it exists.
	if(keys.size() > 0)
	{
		if(!_instanceExists(ns, path, aclInfo))
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
				format("Instance not found: %1", path.toString()).c_str());
		}
	}

	// Get the method from the class definition
	OW_CIMMethod method = cc.getMethod(methodName);
	if(!method)
	{
		OW_THROWCIMMSG(OW_CIMException::METHOD_NOT_FOUND, methodName.c_str());
	}

	OW_CIMValue cv(OW_CIMNULL);
	OW_MethodProviderIFCRef methodp;

	OW_CIMClass cctemp(cc);

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
		catch (const OW_NoSuchProviderException&)
		{
		}
		OW_String parentClassName = cctemp.getSuperClass();
		if (parentClassName.empty() || methodp)
		{
			// loop until we've got a provider or hit a root class
			break;
		}
		cctemp = _getClass(ns, parentClassName,false,true,true,0,intAclInfo);
	}

	if(!methodp)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, format("No provider for method %1", methodName).c_str());
	}

	OW_CIMParameterArray methodInParams = method.getINParameters();
	if (inParams.size() != methodInParams.size())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Incorrect number of parameters");
	}

	OW_CIMParameterArray methodOutParams = method.getOUTParameters();
	outParams.resize(methodOutParams.size());
	// set the names on outParams
	for (size_t i = 0; i < methodOutParams.size(); ++i)
	{
		outParams[i].setName(methodOutParams[i].getName());
	}

	OW_CIMParamValueArray orderedParams;
	OW_CIMParamValueArray inParams2(inParams);
	for (size_t i = 0; i < methodInParams.size(); ++i)
	{
		OW_String parameterName = methodInParams[i].getName();
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
			if (methodInParams[i].hasTrueQualifier(OW_CIMQualifier::CIM_QUAL_REQUIRED))
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
					"Parameter %1 was not specified.", parameterName).c_str());
			}
			else
			{
				// put a param with a null value
				OW_CIMParamValue optionalParam(methodInParams[i].getName());
				inParams2.push_back(optionalParam);
			}
		}

		// move the param from inParams2 to orderedParams
		orderedParams.push_back(inParams2[paramIdx]);
		inParams2.erase(inParams2.begin() + paramIdx);

		// make sure the type is right
		OW_CIMValue v = orderedParams[i].getValue();
		if (v)
		{
			if (methodInParams[i].getType().getType() != v.getType())
			{
				try
				{
					orderedParams[i].setValue(OW_CIMValueCast::castValueToDataType(
						v, methodInParams[i].getType()));
				}
				catch (OW_CIMException& ce)
				{
					ce.setErrNo(OW_CIMException::INVALID_PARAMETER);
					throw;
				}
			}
		}

		// if the in param is also an out param, assign the value to the out
		// params array
		if (methodInParams[i].hasTrueQualifier(OW_CIMQualifier::CIM_QUAL_OUT))
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
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
			"Unknown or duplicate parameter: %1", inParams2[0].getName()).c_str());
	}

	OW_StringBuffer methodStr;
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		methodStr += "OW_CIMServer invoking extrinsic method provider: ";
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
		OW_CIMValue v = outParams[i].getValue();
		if (v)
		{
			if (methodOutParams[i].getType().getType() != v.getType())
			{
				outParams[i].setValue(OW_CIMValueCast::castValueToDataType(
					v, methodOutParams[i].getType()));
			}
		}
	}

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		methodStr.reset();
		methodStr += "OW_CIMServer finished invoking extrinsic method provider: ";
		OW_CIMObjectPath path2(path);
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
OW_InstanceProviderIFCRef
OW_CIMServer::_getInstanceProvider(const OW_String& ns, const OW_CIMClass& cc_)
{
	OW_UserInfo intAclInfo;
	OW_InstanceProviderIFCRef instancep;
	OW_CIMClass cc(cc_);

	// we need to query for providers for this class and all base classes.
	// One provider may instrument an entire hierarchy of classes.
	// loop until we've got a provider or hit a root class
	while (true)
	{
		try
		{
			instancep =
				m_provManager->getInstanceProvider(createProvEnvRef(intAclInfo, m_env), ns, cc);
		}
		catch (const OW_NoSuchProviderException& e)
		{
			// if it's not an instance or associator provider, then ERROR!
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			try
			{
				m_provManager->getAssociatorProvider(
					createProvEnvRef(intAclInfo, m_env), ns, cc);
			}
			catch (const OW_NoSuchProviderException& e)
			{
#endif
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					format("Invalid provider: %1", e.getMessage()).c_str());
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			}
#endif
		}
		OW_String parentClassName = cc.getSuperClass();
		if (parentClassName.empty() || instancep)
		{
			// loop until we've got a provider or hit a root class
			break;
		}
		cc = _getClass(ns, parentClassName,false,true,true,0,intAclInfo);
	}

	return instancep;
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_CIMServer::_getAssociatorProvider(const OW_String& ns, const OW_CIMClass& cc_)
{
	OW_UserInfo intAclInfo;
	OW_AssociatorProviderIFCRef ap;
	OW_CIMClass cc(cc_);

	// we need to query for providers for this class and all base classes.
	// One provider may instrument an entire hierarchy of classes.
	// loop until we've got a provider or hit a root class
	while (true)
	{
		try
		{
			ap =  m_provManager->getAssociatorProvider(
				createProvEnvRef(intAclInfo, m_env), ns, cc);
		}
		catch (const OW_NoSuchProviderException&)
		{
			// if it's not an instance or associator provider, then ERROR!
			try
			{
				m_provManager->getInstanceProvider(
					createProvEnvRef(intAclInfo, m_env), ns, cc);
			}
			catch (const OW_NoSuchProviderException& e)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					format("Invalid provider: %1", e.getMessage()).c_str());
			}
		}
		OW_String parentClassName = cc.getSuperClass();
		if (parentClassName.empty() || ap)
		{
			// loop until we've got a provider or hit a root class
			break;
		}
		cc = _getClass(ns, parentClassName,false,true,true,0,intAclInfo);
	}
	return ap;
}
#endif

//////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::_getNameSpaceClass(const OW_String& className)
{

	if (className.equalsIgnoreCase("__Namespace"))
	{
		if(!m_nsClass_Namespace)
		{
			m_nsClass_Namespace = OW_CIMClass("__Namespace");

			OW_CIMProperty cimProp(OW_CIMProperty::NAME_PROPERTY);
			cimProp.setDataType(OW_CIMDataType::STRING);
			cimProp.addQualifier(OW_CIMQualifier::createKeyQualifier());
			m_nsClass_Namespace.addProperty(cimProp);
		}

		return m_nsClass_Namespace;
	}
	/*
	else if (className.equalsIgnoreCase("CIM_Namespace"))
	{
		if (!m_nsClassCIM_Namespace)
		{
			m_nsClassCIM_Namespace = OW_CIMClass("CIM_Namespace");

		}
		return m_nsClassCIM_Namespace;
	}
	*/
	else
	{
		return OW_CIMClass(OW_CIMNULL);
	}
}

//////////////////////////////////////////////////////////////////////
void
OW_CIMServer::execQuery(
	const OW_String& ns,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &query,
	const OW_String& queryLanguage, const OW_UserInfo& aclInfo)
{
	OW_WQLIFCRef wql = m_env->getWQLRef();
	if (wql && wql->supportsQueryLanguage(queryLanguage))
	{
		OW_CIMOMHandleIFCRef lch(new OW_LocalCIMOMHandle(m_env,
			OW_RepositoryIFCRef(this, true), aclInfo, true));

		try
		{
			wql->evaluate(ns, result, query, queryLanguage, lch);
		}
		catch (const OW_CIMException& ce)
		{
			// translate any error except INVALID_NAMESPACE, INVALID_QUERY,
			// ACCESS_DENIED or FAILED into an INVALID_QUERY
			if (ce.getErrNo() != OW_CIMException::FAILED
				&& ce.getErrNo() != OW_CIMException::INVALID_NAMESPACE
				&& ce.getErrNo() != OW_CIMException::INVALID_QUERY
				&& ce.getErrNo() != OW_CIMException::ACCESS_DENIED)
			{
				// the " " added to the beginning of the message is a little
				// trick to fool the OW_CIMException constructor from stripping
				// away the old "canned" CIMException message.
				throw OW_CIMException(ce.getFile(), ce.getLine(),
					OW_CIMException::INVALID_QUERY,
					OW_String(OW_String(" ") + ce.getMessage()).c_str());
			}
			else
			{
				throw ce;
			}
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::QUERY_LANGUAGE_NOT_SUPPORTED, queryLanguage.c_str());
	}
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::associators(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORS, ns, aclInfo);
#endif

	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, &result, 0, 0,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::associatorsClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORS, ns, aclInfo);
#endif

	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, 0, 0, &result,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::associatorNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORNAMES, ns, aclInfo);
#endif

	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		false, false, 0, 0, &result, 0, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::references(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCES, ns, aclInfo);
#endif

	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, &result, 0, 0, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::referencesClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCES, ns, aclInfo);
#endif

	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, 0, 0, &result, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::referenceNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	const OW_UserInfo& aclInfo)
{
#if !defined(OW_DISABLE_ACLS)
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCENAMES, ns, aclInfo);
#endif

	_commonReferences(ns, path, resultClass, role, false, false, 0, 0, &result, 0,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class assocClassSeparator : public OW_CIMClassResultHandlerIFC
	{
	public:
		assocClassSeparator(
			OW_StringArray* staticAssocs_,
			OW_CIMClassArray& dynamicAssocs_,
			OW_CIMServer& server_,
			const OW_UserInfo& aclInfo_,
			const OW_String& ns_,
			const OW_LoggerRef& lgr)
		: staticAssocs(staticAssocs_)
		, dynamicAssocs(dynamicAssocs_)
		, server(server_)
		, aclInfo(aclInfo_)
		, ns(ns_)
		, logger(lgr)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &cc)
		{
			if (!cc.isAssociation())
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
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
		OW_StringArray* staticAssocs;
		OW_CIMClassArray& dynamicAssocs;
		OW_CIMServer& server;
		const OW_UserInfo& aclInfo;
		OW_String ns;
		OW_LoggerRef logger;
	};
}



//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_commonReferences(
	const OW_String& ns,
	const OW_CIMObjectPath& path_,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult,
	const OW_UserInfo& aclInfo)
{
	OW_CIMObjectPath path(path_);
	path.setNameSpace(ns);

	// Get all association classes from the repository
	// If the result class was specified, only children of it will be
	// returned.

	OW_CIMClassArray dynamicAssocs;
	OW_StringArray resultClassNames;

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

				OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());
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
					OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());
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
					OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());
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
OW_CIMServer::_dynamicReferences(const OW_CIMObjectPath& path,
	const OW_CIMClassArray& assocClasses, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult, const OW_UserInfo& aclInfo)
{
	// assocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}

	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		OW_CIMClass cc = assocClasses[i];

		OW_AssociatorProviderIFCRef assocP = _getAssociatorProvider(path.getNameSpace(), cc);
		if(!assocP)
		{
			continue;
		}

		OW_String resultClass(assocClasses[i].getName());

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
	class classNamesBuilder : public OW_StringResultHandlerIFC
	{
	public:
		classNamesBuilder(OW_StringArray& resultClassNames) 
			: m_resultClassNames(resultClassNames) 
		{}

		void doHandle(const OW_String& op)
		{
			m_resultClassNames.push_back(op);
		}

	private:
		OW_StringArray& m_resultClassNames;
	};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_commonAssociators(
	const OW_String& ns,
	const OW_CIMObjectPath& path_,
	const OW_String& assocClassName, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList,
	OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult,
	const OW_UserInfo& aclInfo)
{
	OW_CIMObjectPath path(path_);
	path.setNameSpace(ns);

	// Get association classes from the association repository
	OW_CIMClassArray dynamicAssocs;
	OW_StringArray assocClassNames;
	assocClassSeparator assocClassResult(!m_realRepository || assocClassName.empty() ? 0 : &assocClassNames, dynamicAssocs, *this, aclInfo, ns, m_env->getLogger());
	_getAssociationClasses(ns, assocClassName, path.getObjectName(), assocClassResult, role);

	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	OW_StringArray resultClassNames;
	if(m_realRepository && !resultClass.empty())
	{
		classNamesBuilder resultClassNamesResult(resultClassNames);
		m_cimRepository->enumClassNames(ns, resultClass, resultClassNamesResult, true, aclInfo);
		resultClassNames.append(resultClass);
	}

	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		if (m_realRepository)
		{
			OW_SortedVectorSet<OW_String> assocClassNamesSet(assocClassNames.begin(),
					assocClassNames.end());
			OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(),
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
				OW_SortedVectorSet<OW_String> assocClassNamesSet(assocClassNames.begin(),
						assocClassNames.end());
				OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(),
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
				OW_SortedVectorSet<OW_String> assocClassNamesSet(assocClassNames.begin(),
						assocClassNames.end());
				OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(),
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
OW_CIMServer::_dynamicAssociators(const OW_CIMObjectPath& path,
	const OW_CIMClassArray& assocClasses, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult, const OW_UserInfo& aclInfo)
{
	// AssocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}

	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		OW_CIMClass cc = assocClasses[i];

		OW_AssociatorProviderIFCRef assocP = _getAssociatorProvider(path.getNameSpace(), cc);
		if(!assocP)
		{
			m_env->getLogger()->logError("Failed to get associator provider for class: " + cc.getName());
			continue;
		}

		OW_String assocClass(assocClasses[i].getName());

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
OW_CIMServer::_getAssociationClasses(const OW_String& ns,
		const OW_String& assocClassName, const OW_String& className,
		OW_CIMClassResultHandlerIFC& result, const OW_String& role)
{
	OW_UserInfo intAclInfo;
	if(!assocClassName.empty())
	{
		// they gave us a class name so we can use the class association index
		// to only look at the ones that could provide associations
		OW_CIMClass cc = getClass(ns,assocClassName,false,true,true,0,intAclInfo);
		result.handle(cc);
		// TODO: measure whether it would be faster to use
		// enumClassNames + getClass() here.
		enumClasses(ns,assocClassName,result,true,false,true,true, intAclInfo);
	}
	else
	{
		// need to get all the assoc classes with dynamic providers
		OW_CIMObjectPath cop(className, ns);
		if (m_realRepository)
		{
			m_realRepository->_staticReferencesClass(cop,0,role,true,false,0,0,&result,intAclInfo);
		}
		else
		{
			m_cimRepository->referencesClasses(ns,cop,result,assocClassName,role,true,false,0,intAclInfo);
		}

		// TODO: test if this is faster
		//assocHelper helper(result, m_mStore, ns);
		//m_mStore.getTopLevelAssociations(ns, helper);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMServer::_isDynamicAssoc(const OW_String& ns, const OW_CIMClass& cc)
{
	return _getAssociatorProvider(ns, cc) ? true : false;
}

#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL


const char* const OW_CIMServer::INST_REPOS_NAME = "instances";
const char* const OW_CIMServer::META_REPOS_NAME = "schema";
const char* const OW_CIMServer::NS_REPOS_NAME = "namespaces";
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
const char* const OW_CIMServer::CLASS_ASSOC_REPOS_NAME = "classassociation";
const char* const OW_CIMServer::INST_ASSOC_REPOS_NAME = "instassociation";
#endif


