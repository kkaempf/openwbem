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
#include "OW_CIMServer.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Format.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_CIM.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMParamValue.hpp"

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
	void checkAccess(int op, const OW_String& ns, const OW_ACLInfo& aclInfo);

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
	const OW_ACLInfo& aclInfo)
{
	if (aclInfo.m_internal)
	{
		return;
	}
	OW_ACLInfo intACLInfo;

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
			OW_CIMInstance ci;
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
					if (capability.indexOf(opType) < 0)
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
		OW_CIMInstance ci;
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
				if (capability.indexOf(opType) < 0)
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
		int idx = lns.lastIndexOf('/');
		if (idx <= 0)
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

//////////////////////////////////////////////////////////////////////////////
OW_CIMServer::OW_CIMServer(OW_CIMOMEnvironmentRef env,
	const OW_ProviderManagerRef& provManager,
	const OW_RepositoryIFCRef& cimRepository)
	: OW_RepositoryIFC()
	, m_provManager(provManager)
	, m_rwSchemaLocker()
	, m_rwInstanceLocker()
	, m_accessMgr(new OW_AccessMgr(this, env))
	, m_nsClass__Namespace()
	, m_nsClassCIM_Namespace()
	, m_env(env)
	, m_cimRepository(cimRepository)
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

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::createNameSpace(const OW_String& ns,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::CREATENAMESPACE, ns, aclInfo);

	m_cimRepository->createNameSpace(ns,aclInfo);

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::deleteNameSpace(const OW_String& ns,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::DELETENAMESPACE, ns, aclInfo);

	m_cimRepository->deleteNameSpace(ns,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMServer::getQualifierType(const OW_String& ns,
	const OW_String& qualifierName,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::GETQUALIFIER, ns, aclInfo);

	return m_cimRepository->getQualifierType(ns,qualifierName,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEQUALIFIERS, ns, aclInfo);
	m_cimRepository->enumQualifierTypes(ns,result,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumNameSpace(const OW_String& nsName,
	OW_StringResultHandlerIFC& result, OW_Bool deep,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATENAMESPACE, nsName,
		aclInfo);
	m_cimRepository->enumNameSpace(nsName,result,deep,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::deleteQualifierType(const OW_String& ns, const OW_String& qualName,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::DELETEQUALIFIER, ns, aclInfo);
	m_cimRepository->deleteQualifierType(ns,qualName,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::setQualifierType(
	const OW_String& ns,
	const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::SETQUALIFIER, ns, aclInfo);
	m_cimRepository->setQualifierType(ns,qt,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::getClass(
	const OW_String& ns, const OW_String& className, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// we don't check for __Namespace, so that clients can get it before they
	// create one.
	if (!className.equalsIgnoreCase("__Namespace"))
	{
		m_accessMgr->checkAccess(OW_AccessMgr::GETCLASS, ns, aclInfo);
	}
	return _getClass(ns,className, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::_getClass(const OW_String& ns, const OW_String& className, 
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
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

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::DELETECLASS, ns, aclInfo);
	return m_cimRepository->deleteClass(ns,className,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::createClass(const OW_String& ns, const OW_CIMClass& cimClass,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::CREATECLASS, ns, aclInfo);

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
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::MODIFYCLASS, ns, aclInfo);
	return m_cimRepository->modifyClass(ns,cc,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATECLASSES, ns, aclInfo);
	m_cimRepository->enumClasses(ns,className,result,deep,localOnly,
		includeQualifiers,includeClassOrigin,aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumClassNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep, const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATECLASSNAMES, ns, aclInfo);
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
			const OW_ACLInfo& aclInfo_,
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
		const OW_ACLInfo& aclInfo;
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
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEINSTANCENAMES, ns,
		aclInfo);

	InstNameEnumerator ie(ns, result, aclInfo, m_env, this);

	OW_ACLInfo intAclInfo;
	// If this is the namespace class then just return now
	if(className.equalsIgnoreCase(OW_CIMClass::NAMESPACECLASS)
		|| !deep)
	{
		OW_CIMClass theClass = _instGetClass(ns, className,false,true,true,0,intAclInfo);
		ie.handle(theClass);
	}
	else
	{
		m_cimRepository->enumClasses(ns,className,ie,deep,false,true,true,intAclInfo);
	}

}

/////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMServerProviderEnvironment : public OW_ProviderEnvironmentIFC
	{
	public:

		CIMServerProviderEnvironment(const OW_LocalCIMOMHandle& ch)
			: m_ch(new OW_LocalCIMOMHandle(ch))
		{}

		virtual OW_String getConfigItem(const OW_String &name) const
		{
			return m_ch->getEnvironment()->getConfigItem(name);
		}

		virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_ch;
		}
		
		virtual OW_LoggerRef getLogger() const
		{
			return m_ch->getEnvironment()->getLogger();
		}

	private:
		mutable OW_Reference<OW_LocalCIMOMHandle> m_ch;
	};

	OW_ProviderEnvironmentIFCRef createProvEnvRef(const OW_LocalCIMOMHandle& ch)
	{
		return OW_ProviderEnvironmentIFCRef(new CIMServerProviderEnvironment(ch));
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMServer::_getCIMInstanceNames(const OW_String& ns, const OW_String& className,
	const OW_CIMClass& theClass, OW_CIMObjectPathResultHandlerIFC& result,
	const OW_ACLInfo& aclInfo)
{
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);

	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);
	if (instancep)
	{
		instancep->enumInstanceNames(createProvEnvRef(real_ch),
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
			const OW_ACLInfo& aclInfo_,
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
				result, deep, localOnly, includeQualifiers, 
				includeClassOrigin, propertyList, aclInfo);
		}
	private:
		OW_String ns;
		OW_CIMInstanceResultHandlerIFC& result;
		const OW_ACLInfo& aclInfo;
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
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEINSTANCES, ns, aclInfo);

	OW_ACLInfo intAclInfo;
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
		// do subclasses
		m_cimRepository->enumClasses(ns,className,ie,true,false,true,true,intAclInfo);
	}
}

namespace
{
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
	const OW_CIMClass& theClass, OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{

	OW_InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass));
	if (instancep)
	{
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("OW_CIMServer calling provider to enumerate instances: %1:%2", ns,
				className));
		}

		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo, true);
		HandleLocalOnlyAndDeep handler1(result,theClass,localOnly,deep);
		HandleProviderInstance handler2(includeQualifiers, includeClassOrigin, propertyList, handler1);
		instancep->enumInstances(
			createProvEnvRef(real_ch), ns, className, handler2, theClass);
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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
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
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::GETINSTANCE, ns, aclInfo);

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

	OW_CIMInstance ci;
	if(instancep)
	{
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
			true);

		ci = instancep->getInstance(
			createProvEnvRef(real_ch),
				ns, instanceName, cc);
		if (!ci)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Provider erroneously returned a NULL CIMInstance");
		}

		ci.syncWithClass(cc, true);

		ci = ci.clone(localOnly, includeQualifiers, includeClassOrigin,
			propertyList);

	}
	else
	{
		ci = m_cimRepository->getInstance(ns, instanceName, localOnly, 
			includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	}

	OW_ASSERT(ci);
	
	return ci;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& cop_,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::DELETEINSTANCE, ns, aclInfo);
	
	OW_CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMServer::deleteInstance.  cop = %1",
			cop.toString()));
	}

	OW_ACLInfo intAclInfo;
	OW_CIMClass theClass;
	OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
		&theClass, intAclInfo);

	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);

	if(instancep)	// If there is an instance provider, let it do the delete.
	{
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
			true);

		instancep->deleteInstance(
			createProvEnvRef(real_ch), ns, cop);
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
	const OW_ACLInfo& aclInfo)
{
	OW_CIMObjectPath rval(ci);

	// Check to see if user has rights to create the instance
	m_accessMgr->checkAccess(OW_AccessMgr::CREATEINSTANCE, ns, aclInfo);

	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	OW_String className = ci.getClassName();

	OW_ACLInfo intAclInfo;

	OW_CIMClass theClass = _instGetClass(ns, className, false, true, true, 0, 
		intAclInfo);

	OW_CIMQualifier acq = theClass.getQualifier(
			OW_CIMQualifier::CIM_QUAL_ABSTRACT);
	if(acq)
	{
		if (acq.getValue() == OW_CIMValue(true))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
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

	if(theClass.isAssociation())
	{
		OW_ACLInfo intAclInfo;
		OW_CIMPropertyArray pra = ci.getProperties(
			OW_CIMDataType::REFERENCE);

		for(size_t j = 0; j < pra.size(); j++)
		{
			OW_CIMValue cv = pra[j].getValue();
			if(!cv)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					"Association has a NULL reference");
			}

			OW_CIMObjectPath op;
			cv.get(op);

			if(!op)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					"Association has a NULL reference");
			}

			OW_CIMClass rcc;
			OW_ACLInfo intAclInfo;
			try
			{
				getInstance(ns,op,false,true,true,0,&rcc,intAclInfo);
			}
			catch (OW_CIMException&)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("Association references an invalid instance:"
						" %1", op.toString()).c_str());
			}

		}

	}


	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass);
	if (instancep)
	{
		rval = instancep->createInstance(createProvEnvRef(real_ch), ns, ci);
	}
	else
	{
		_validatePropagatedKeys(ns, lci, theClass); // TODO: Should we do this for providers too?
		rval = m_cimRepository->createInstance(ns, ci, aclInfo);
	}

	OW_ASSERT(rval);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	OW_StringArray* propertyList,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::MODIFYINSTANCE, ns, aclInfo);

	OW_ACLInfo intAclInfo;
	OW_CIMClass theClass;
	OW_CIMObjectPath cop(modifiedInstance);
	OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
		&theClass, intAclInfo);

	OW_CIMInstance newInst(modifiedInstance);

	if (!includeQualifiers)
	{
		newInst.setQualifiers(oldInst.getQualifiers());
	}

	if (propertyList)
	{
		newInst.setProperties(oldInst.getProperties());
		for (OW_StringArray::const_iterator i = propertyList->begin();
			 i != propertyList->end(); ++i)
		{
			OW_CIMProperty p = modifiedInstance.getProperty(*i);
			if (p)
			{
				if (!includeQualifiers)
				{
					OW_CIMProperty cp = theClass.getProperty(*i);
					if (cp)
					{
						p.setQualifiers(cp.getQualifiers());
					}
				}
				newInst.setProperty(p);
			}
			else
			{
				OW_CIMProperty cp = theClass.getProperty(*i);
				if (cp)
				{
					newInst.setProperty(cp);
				}
				else
				{
					newInst.removeProperty(*i);
				}
			}
		}
	}

	OW_InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass));

	if(!instancep)
	{
		// No instance provider qualifier found
		m_cimRepository->modifyInstance(ns, newInst, 
			includeQualifiers, propertyList, aclInfo);
	}
	else
	{
		// Look for dynamic instances
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
			aclInfo, true);
		instancep->modifyInstance(createProvEnvRef(real_ch), ns,
			newInst);
	}

	OW_ASSERT(oldInst);
	return oldInst;

}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMServer::_instanceExists(const OW_String& ns, const OW_CIMObjectPath& icop,
	const OW_ACLInfo& aclInfo)
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
	const OW_String& propertyName, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::GETPROPERTY, ns, aclInfo);

	OW_ACLInfo intAclInfo;
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

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& valueArg,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::SETPROPERTY, ns, aclInfo);

	OW_ACLInfo intAclInfo;
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

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMServer::invokeMethod(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	const OW_String& methodName, const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::INVOKEMETHOD, ns, aclInfo);

	// Not creating any read/write locks here because the provider
	// could call back into the CIM Server for something and then we
	// could end up with a deadlock condition.

	OW_ACLInfo intAclInfo;
	OW_CIMMethod method;
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
	method = cc.getMethod(methodName);
	if(!method)
	{
		OW_THROWCIMMSG(OW_CIMException::METHOD_NOT_FOUND, methodName.c_str());
	}

	OW_CIMValue cv;
	OW_MethodProviderIFCRef methodp;

	try
	{
		// these are fully locking cimomhandles because we don't have any locks.
		OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
			intAclInfo, false);
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
			aclInfo, false);
		
		OW_CIMClass cctemp(cc);

		// we need to query for providers for this class and all base classes.
		// One provider may instrument an entire hierarchy of classes.
		// loop until we've got a provider or hit a root class
		while (true)
		{
			try
			{
				methodp = m_provManager->getMethodProvider(
					createProvEnvRef(internal_ch), ns, cctemp, method);
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
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
					"Parameter %1 was not specified.", parameterName).c_str());
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
			if (methodInParams[i].getQualifier(OW_CIMQualifier::CIM_QUAL_OUT))
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
			createProvEnvRef(real_ch),
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
			methodStr += ns;
			methodStr += ':';
			methodStr += path.toString();
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
	}
	catch(OW_CIMException& e)
	{
		throw;
	}
	catch(OW_Exception& e)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, format("Caught exception: %1 "
			"while calling provider for method %2", e, methodName).c_str());
	}
	catch(...)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, format("Caught unknown "
			"exception while calling provider for method %1",
			methodName).c_str());
	}

	return cv;
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_CIMServer::_getInstanceProvider(const OW_String& ns, const OW_CIMClass& cc_)
{
	OW_ACLInfo intAclInfo;
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);
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
				m_provManager->getInstanceProvider(createProvEnvRef(internal_ch), ns, cc);
		}
		catch (const OW_NoSuchProviderException&)
		{
			// if it's not an instance or associator provider, then ERROR!
			try
			{
				m_provManager->getAssociatorProvider(
					createProvEnvRef(internal_ch), ns, cc);
			}
			catch (const OW_NoSuchProviderException& e)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					format("Invalid provider: %1", e.getMessage()).c_str());
			}
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

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_CIMServer::_getAssociatorProvider(const OW_String& ns, const OW_CIMClass& cc_)
{
	OW_ACLInfo intAclInfo;
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);
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
				createProvEnvRef(internal_ch), ns, cc);
		}
		catch (const OW_NoSuchProviderException&)
		{
			// if it's not an instance or associator provider, then ERROR!
			try
			{
				m_provManager->getInstanceProvider(
					createProvEnvRef(internal_ch), ns, cc);
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

//////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::_getNameSpaceClass(const OW_String& className)
{

	if (className.equalsIgnoreCase("__Namespace"))
	{
		if(!m_nsClass__Namespace)
		{
			m_nsClass__Namespace = OW_CIMClass("__Namespace");

			OW_CIMQualifier cimQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
			cimQualifier.setValue(OW_CIMValue(OW_String(NAMESPACE_PROVIDER)));
			OW_CIMProperty cimProp(OW_CIMProperty::NAME_PROPERTY);
			cimProp.setDataType(OW_CIMDataType::STRING);
			cimProp.addQualifier(OW_CIMQualifier::createKeyQualifier());
			m_nsClass__Namespace.addQualifier(cimQualifier);
			m_nsClass__Namespace.addProperty(cimProp);
		}

		return m_nsClass__Namespace;
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
		return OW_CIMClass();
	}
}

//////////////////////////////////////////////////////////////////////
void
OW_CIMServer::execQuery(
	const OW_String& ns,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &query,
	const OW_String& queryLanguage, const OW_ACLInfo& aclInfo)
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

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::associators(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORS, ns, aclInfo);

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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORS, ns, aclInfo);

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
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORNAMES, ns, aclInfo);

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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCES, ns, aclInfo);

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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCES, ns, aclInfo);

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
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCENAMES, ns, aclInfo);

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
			OW_CIMClassArray& staticAssocs_,
			OW_CIMClassArray& dynamicAssocs_,
			OW_CIMServer& server_,
			const OW_ACLInfo& aclInfo_,
			const OW_String& ns_)
		: staticAssocs(staticAssocs_)
		, dynamicAssocs(dynamicAssocs_)
		, server(server_)
		, aclInfo(aclInfo_)
		, ns(ns_)
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
			OW_CIMClassArray* pra = (server._isDynamicAssoc(ns, cc)) ? &dynamicAssocs
				: &staticAssocs;
			pra->append(cc);
		}
	private:
		OW_CIMClassArray& staticAssocs;
		OW_CIMClassArray& dynamicAssocs;
		OW_CIMServer& server;
		const OW_ACLInfo& aclInfo;
		OW_String ns;
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
	const OW_ACLInfo& aclInfo)
{
	OW_CIMClass assocClass;
	OW_CIMObjectPath path(path_);
	path.setNameSpace(ns);

	// Get all association classes from the repository
	// If the result class was specified, only children of it will be
	// returned.

	OW_CIMClassArray staticAssocs;
	OW_CIMClassArray dynamicAssocs;

	assocClassSeparator assocClassResult(staticAssocs, dynamicAssocs, *this, aclInfo, ns);
	_getAssociationClasses(ns, resultClass, path.getObjectName(), assocClassResult, role);
	//OW_StringArray resultClassNames;
	//for(size_t i = 0; i < staticAssocs.size(); i++)
	//{
		//resultClassNames.append(staticAssocs[i].getName());
	//}
	//OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());

	if (path.getKeys().size() == 0)
	{
		// it's a class path
		// Process all of the association classes without providers
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
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			m_cimRepository->references(ns,path,*piresult,resultClass,role,includeQualifiers,includeClassOrigin,propertyList,aclInfo);
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			m_cimRepository->referenceNames(ns,path,*popresult,resultClass,role,aclInfo);
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
	OW_CIMObjectPathResultHandlerIFC* popresult, const OW_ACLInfo& aclInfo)
{
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

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

		OW_CIMObjectPath assocClassPath(assocClasses[i].getName(),
			path.getNameSpace());

		// If the object path enumeration pointer is null, then assume we
		// are doing references and not referenceNames
		if(piresult != 0)
		{
			assocP->references(
				createProvEnvRef(real_ch),
				path.getNameSpace(), assocClassPath, path, *piresult, role, includeQualifiers,
				includeClassOrigin, propertyList);
		}
		else if (popresult != 0)
		{
			assocP->referenceNames(
				createProvEnvRef(real_ch),
				path.getNameSpace(), assocClassPath, path, *popresult, role);
		}
		else
		{
			OW_ASSERT(0);
		}
	}
}

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
	const OW_ACLInfo& aclInfo)
{
	OW_CIMObjectPath path(path_);
	path.setNameSpace(ns);

	// Get association classes from the association repository
	OW_CIMClassArray staticAssocs;
	OW_CIMClassArray dynamicAssocs;
	assocClassSeparator assocClassResult(staticAssocs, dynamicAssocs, *this, aclInfo, ns);
	_getAssociationClasses(ns, assocClassName, path.getObjectName(), assocClassResult, role);

	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	//OW_StringArray resultClassNames;
	//if(!resultClass.empty())
	//{
	//	resultClassNames = m_mStore.getClassChildren(ns, resultClass);
	//	resultClassNames.append(resultClass);
	//}

	//OW_StringArray assocClassNames;
	//for(size_t i = 0; i < staticAssocs.size(); i++)
	//{
	//	assocClassNames.append(staticAssocs[i].getName());
	//}
	//OW_SortedVectorSet<OW_String> assocClassNamesSet(assocClassNames.begin(),
	//		assocClassNames.end());
	//OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(),
	//		resultClassNames.end());

	if (path.getKeys().size() == 0)
	{
		// it's a class path
		// Process all of the association classes without providers
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
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			m_cimRepository->associators(ns,path,*piresult,assocClassName,resultClass,role,resultRole,includeQualifiers,includeClassOrigin,propertyList,aclInfo);
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			m_cimRepository->associatorNames(ns,path,*popresult,assocClassName,resultClass,role,resultRole,aclInfo);
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
	OW_CIMObjectPathResultHandlerIFC* popresult, const OW_ACLInfo& aclInfo)
{
	// AssocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}

	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		OW_CIMClass cc = assocClasses[i];

		OW_AssociatorProviderIFCRef assocP = _getAssociatorProvider(path.getNameSpace(), cc);
		if(!assocP)
		{
			continue;
		}

		OW_CIMObjectPath assocClassPath(assocClasses[i].getName(),
			path.getNameSpace());

		if(piresult != 0)
		{
			assocP->associators(createProvEnvRef(real_ch), path.getNameSpace(),
				assocClassPath, path, *piresult, resultClass, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList);
		}
		else if (popresult != 0)
		{
			assocP->associatorNames(createProvEnvRef(real_ch),
				path.getNameSpace(),
				assocClassPath, path, *popresult, resultClass, role, resultRole);
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
	OW_ACLInfo intAclInfo;
	if(!assocClassName.empty())
	{
		// they gave us a class name so we can use the class association index
		// to only look at the ones that could provide associations
		OW_CIMClass cc = getClass(ns,assocClassName,false,true,true,0,intAclInfo);
		result.handle(cc);
		enumClasses(ns,assocClassName,result,true,false,true,true, intAclInfo);
	}
	else
	{
		// need to get all the assoc classes with dynamic providers
		OW_CIMObjectPath cop(className, ns);
		// this was faster: _staticReferencesClass(cop,0,role,true,false,0,0,&result);
		m_cimRepository->referencesClasses(ns,cop,result,assocClassName,role,true,false,0,intAclInfo);

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

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_validatePropagatedKeys(const OW_String& ns,
	const OW_CIMInstance& ci, const OW_CIMClass& theClass)
{
	OW_CIMObjectPathArray rv;
	OW_CIMPropertyArray kprops = theClass.getKeys();
	if(kprops.size() == 0)
	{
		return;
	}

	OW_Map<OW_String, OW_CIMPropertyArray> theMap;
	OW_Bool hasPropagatedKeys = false;

	// Look at all propagated key properties
	for(size_t i = 0; i < kprops.size(); i++)
	{
		OW_CIMQualifier cq = kprops[i].getQualifier(
			OW_CIMQualifier::CIM_QUAL_PROPAGATED);
		if(!cq)
		{
			continue;
		}

		hasPropagatedKeys = true;
		OW_CIMValue cv = cq.getValue();
		if(!cv)
		{
			continue;
		}

		OW_String cls;
		cv.get(cls);
		if(cls.empty())
		{
			continue;
		}
		int idx = cls.indexOf('.');
		OW_String ppropName;
		if (idx != -1)
		{
			ppropName = cls.substring(idx+1);
			cls = cls.substring(0,idx);
		}

		OW_CIMProperty cp = ci.getProperty(kprops[i].getName());
		if(!cp || !cp.getValue())
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Cannot create instance. Propagated key field missing:"
					" %1", kprops[i].getName()).c_str());
		}
		if (!ppropName.empty())
		{
			// We need to use the propagated property name, not the property
			// name on ci.  e.g. Given
			// [Propagated("fooClass.fooPropName")] string myPropName;
			// we need to check for fooPropName as the key to the propagated
			// instance, not myPropName.
			cp.setName(ppropName);
		}

		theMap[cls].append(cp);
	}

	if(!hasPropagatedKeys)
	{
		return;
	}

	if(theMap.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Cannot create instance. Propagated key properties missing");
	}

	OW_CIMObjectPath op(ci);
	OW_Map<OW_String, OW_CIMPropertyArray>::iterator it = theMap.begin();
	while(it != theMap.end())
	{
		OW_CIMClass cc;
		OW_String clsname = it->first;
		
		op.setObjectName(clsname);
		op.setKeys(it->second);

		try
		{
			OW_ACLInfo intAclInfo;
			getInstance(ns,op,false,true,true,0,0,intAclInfo);
		}
		catch (const OW_CIMException&)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Propagated keys refer to non-existent object: %1",
					op.toString()).c_str());
		}

		it++;
	}
}


const char* const OW_CIMServer::INST_REPOS_NAME = "instances";
const char* const OW_CIMServer::META_REPOS_NAME = "schema";
const char* const OW_CIMServer::NS_REPOS_NAME = "namespaces";
const char* const OW_CIMServer::CLASS_ASSOC_REPOS_NAME = "classassociation";
const char* const OW_CIMServer::INST_ASSOC_REPOS_NAME = "instassociation";
const char* const OW_CIMServer::NAMESPACE_PROVIDER = "owcimomd::namespace";
