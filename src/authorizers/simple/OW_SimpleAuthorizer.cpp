/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_SimpleAuthorizer.hpp"
#include "OW_Assertion.hpp"
#include "OW_UserInfo.hpp"
#include "OW_OperationContext.hpp"
#include "OW_Logger.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifierType.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;

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
	AccessMgr(const RepositoryIFCRef& pServer);
	/**
	 * checkAccess will check that access is granted through the ACL. If
	 * Access is not granted, an CIMException will be thrown.
	 * @param op	The operation that access is being checked for.
	 * @param ns	The name space that access is being check on.
	 * @param context The context from which the ACL info for the user request will be retrieved.
	 */
	void checkAccess(int op, const String& ns, OperationContext& context);

	void setEnv(const ServiceEnvironmentIFCRef& env) { m_env = env; }

private:
	String getMethodType(int op);
	RepositoryIFCRef m_pServer;
	ServiceEnvironmentIFCRef m_env;
};
//////////////////////////////////////////////////////////////////////////////
AccessMgr::AccessMgr(const RepositoryIFCRef& pServer)
	: m_pServer(pServer)
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

namespace
{
	struct InternalDataRemover
	{
		OperationContext& m_context;
		String m_key;
		InternalDataRemover(OperationContext& context, const char* key) 
			: m_context(context) 
			, m_key(key)
		{}
		~InternalDataRemover() { m_context.removeData(m_key); }
	};
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

	const char* const ACCESS_MSG_INTERNAL_CALL = "ACCESS_MSG_INTERNAL_CALL";
	try
	{
		// Avoid an infinite recursion loop
		String val = context.getStringData(ACCESS_MSG_INTERNAL_CALL);
		if (val == "1")
		{
			return;
		}
	}
	catch (ContextDataNotFoundException& e)
	{
		// this means we're good to go.
	}

	// now set the value so that we won't cause an infinite recursion loop
	context.setStringData(ACCESS_MSG_INTERNAL_CALL, "1");
	// and set up an object to remove it when this function returns
	InternalDataRemover internalDataRemover(context, ACCESS_MSG_INTERNAL_CALL);


	LoggerRef lgr = m_env->getLogger();
	if (lgr->getLogLevel() == E_DEBUG_LEVEL)
	{
		lgr->logDebug(Format("Checking access to namespace: \"%1\"", ns));
		lgr->logDebug(Format("UserName is: \"%1\" Operation is : %2",
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
				lgr->logDebug("User is SuperUser: checkAccess returning.");
				return;
			}
			try
			{
				CIMClass cc = m_pServer->getClass("root/security",
					"OpenWBEM_UserACL", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
					context);
			}
			catch(CIMException&)
			{
				lgr->logDebug("OpenWBEM_UserACL class non-existent in"
					" /root/security. ACLs disabled");
				return;
			}
			
			CIMObjectPath cop("OpenWBEM_UserACL");
			cop.setKeyValue("username", CIMValue(userInfo.getUserName()));
			cop.setKeyValue("nspace", CIMValue(lns));
			CIMInstance ci(CIMNULL);
			try
			{
				ci = m_pServer->getInstance("root/security", cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
					context);
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
						lgr->logInfo(Format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							userInfo.m_userName, lns));
						OW_THROWCIM(CIMException::ACCESS_DENIED);
					}
				}
				else
				{
					if (!capability.equals("rw") && !capability.equals("wr"))
					{
						lgr->logInfo(Format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							userInfo.m_userName, lns));
						OW_THROWCIM(CIMException::ACCESS_DENIED);
					}
				}
				lgr->logInfo(Format(
					"ACCESS GRANTED to user \"%1\" for namespace \"%2\"",
					userInfo.m_userName, lns));
				return;
			}
		}
		// use default policy for namespace
		try
		{
			CIMClass cc = m_pServer->getClass("root/security",
				"OpenWBEM_NamespaceACL", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
				context);
		}
		catch(CIMException&)
		{
			lgr->logDebug("OpenWBEM_NamespaceACL class non-existent in"
				" /root/security. namespace ACLs disabled");
			return;
		}
		CIMObjectPath cop("OpenWBEM_NamespaceACL");
		cop.setKeyValue("nspace", CIMValue(lns));
		CIMInstance ci(CIMNULL);
		try
		{
			ci = m_pServer->getInstance("root/security", cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
				context);
		}
		catch(const CIMException& ce)
		{
			if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
			{
				lgr->logDebug(Format("Caught exception: %1 in"
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
					lgr->logInfo(Format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						userInfo.m_userName, lns));
					OW_THROWCIM(CIMException::ACCESS_DENIED);
				}
			}
			else
			{
				if (!capability.equals("rw") && !capability.equals("wr"))
				{
					lgr->logInfo(Format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						userInfo.m_userName, lns));
					OW_THROWCIM(CIMException::ACCESS_DENIED);
				}
			}
			lgr->logInfo(Format(
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
	lgr->logInfo(Format(
		"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
		userInfo.m_userName, lns));
	OW_THROWCIM(CIMException::ACCESS_DENIED);
}
//////////////////////////////////////////////////////////////////////////////
SimpleAuthorizer::SimpleAuthorizer()
	: AuthorizerIFC()
{
}
//////////////////////////////////////////////////////////////////////////////
SimpleAuthorizer::~SimpleAuthorizer()
{
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::setSubRepositoryIFC(const RepositoryIFCRef& src)
{
	m_cimRepository = src;
	ServiceEnvironmentIFCRef env = m_cimRepository->getEnvironment();
	m_accessMgr = Reference<AccessMgr>(new AccessMgr(src));
	m_accessMgr->setEnv(env);
}
//////////////////////////////////////////////////////////////////////////////
AuthorizerIFC *
SimpleAuthorizer::clone() const
{
	return new SimpleAuthorizer(*this);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::open(const String& path)
{
	(void)path;
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::close()
{
}
//////////////////////////////////////////////////////////////////////////////
ServiceEnvironmentIFCRef
SimpleAuthorizer::getEnvironment() const
{
	return m_cimRepository->getEnvironment();
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::createNameSpace(const String& ns,
	OperationContext& context)
{
	// Don't need to check ACLs, since this is a result of calling createInstance.
	m_cimRepository->createNameSpace(ns,context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::deleteNameSpace(const String& ns,
	OperationContext& context)
{
	// Don't need to check ACLs, since this is a result of calling deleteInstance.
	m_cimRepository->deleteNameSpace(ns,context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::enumNameSpace(StringResultHandlerIFC& result,
	OperationContext& context)
{
	// Don't need to check ACLs, since this is a result of calling enumInstances.
	m_cimRepository->enumNameSpace(result,context);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
SimpleAuthorizer::getQualifierType(const String& ns,
	const String& qualifierName,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::GETQUALIFIER, ns, context);
	return m_cimRepository->getQualifierType(ns,qualifierName,context);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEQUALIFIERS, ns, context);
	m_cimRepository->enumQualifierTypes(ns,result,context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::deleteQualifierType(const String& ns, const String& qualName,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::DELETEQUALIFIER, ns, context);
	m_cimRepository->deleteQualifierType(ns,qualName,context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::setQualifierType(
	const String& ns,
	const CIMQualifierType& qt, OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::SETQUALIFIER, ns, context);
	m_cimRepository->setQualifierType(ns,qt,context);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
SimpleAuthorizer::getClass(
	const String& ns, const String& className, ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// we don't check for __Namespace, so that clients can get it before they
	// create one.
	if (!className.equalsIgnoreCase("__Namespace"))
	{
		m_accessMgr->checkAccess(AccessMgr::GETCLASS, ns, context);
	}
	return m_cimRepository->getClass(ns, className, localOnly, includeQualifiers, includeClassOrigin, propertyList, context);
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
SimpleAuthorizer::deleteClass(const String& ns, const String& className,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::DELETECLASS, ns, context);
	return m_cimRepository->deleteClass(ns,className,context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::createClass(const String& ns, const CIMClass& cimClass,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::CREATECLASS, ns, context);
	m_cimRepository->createClass(ns,cimClass,context);
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
SimpleAuthorizer::modifyClass(
	const String& ns,
	const CIMClass& cc,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::MODIFYCLASS, ns, context);
	return m_cimRepository->modifyClass(ns,cc,context);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::ENUMERATECLASSES, ns, context);
	m_cimRepository->enumClasses(ns,className,result,deep,localOnly,
		includeQualifiers,includeClassOrigin,context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::ENUMERATECLASSNAMES, ns, context);
	m_cimRepository->enumClassNames(ns,className,result,deep,context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEINSTANCENAMES, ns,
		context);
	m_cimRepository->enumInstanceNames(ns, className, result, deep, context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result, EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, EEnumSubclassesFlag enumSubclasses,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::ENUMERATEINSTANCES, ns, context);
	m_cimRepository->enumInstances(ns, className, result, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList, enumSubclasses, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
SimpleAuthorizer::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::GETINSTANCE, ns, context);
	return m_cimRepository->getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, context);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMInstance
SimpleAuthorizer::deleteInstance(const String& ns, const CIMObjectPath& cop,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::DELETEINSTANCE, ns, context);
	return m_cimRepository->deleteInstance(ns, cop, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
SimpleAuthorizer::createInstance(
	const String& ns,
	const CIMInstance& ci,
	OperationContext& context)
{
	// Check to see if user has rights to create the instance
	m_accessMgr->checkAccess(AccessMgr::CREATEINSTANCE, ns, context);
	return m_cimRepository->createInstance(ns, ci, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
SimpleAuthorizer::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::MODIFYINSTANCE, ns, context);
	return m_cimRepository->modifyInstance(ns, modifiedInstance,
			includeQualifiers, propertyList, context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
SimpleAuthorizer::getProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, OperationContext& context)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(AccessMgr::GETPROPERTY, ns, context);
	return m_cimRepository->getProperty(ns, name, propertyName, context);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::setProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, const CIMValue& valueArg,
	OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::SETPROPERTY, ns, context);
	m_cimRepository->setProperty(ns, name, propertyName, valueArg, context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
SimpleAuthorizer::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName, const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams, OperationContext& context)
{
	m_accessMgr->checkAccess(AccessMgr::INVOKEMETHOD, ns, context);
	return m_cimRepository->invokeMethod(ns, path, methodName, inParams, outParams, context);
}
//////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::execQuery(
	const String& ns,
	CIMInstanceResultHandlerIFC& result,
	const String &query,
	const String& queryLanguage, OperationContext& context)
{
	// don't check ACLs here, they'll get checked depending on what the query processor does.
	m_cimRepository->execQuery(ns, result, query, queryLanguage, context);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::associators(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORS, ns, context);
	m_cimRepository->associators(ns, path, result, assocClass, resultClass, role, resultRole, includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::associatorsClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORS, ns, context);
	m_cimRepository->associatorsClasses(ns, path, result, assocClass, resultClass, role, resultRole, includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::associatorNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::ASSOCIATORNAMES, ns, context);
	m_cimRepository->associatorNames(ns, path, result, assocClass, resultClass, role, resultRole, context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::references(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCES, ns, context);
	m_cimRepository->references(ns, path, result, resultClass, role, includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::referencesClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCES, ns, context);
	m_cimRepository->referencesClasses(ns, path, result, resultClass, role, includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::referenceNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass, const String& role,
	OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(AccessMgr::REFERENCENAMES, ns, context);
	m_cimRepository->referenceNames(ns, path, result, resultClass, role, context);
}
#endif

//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context)
{
	m_cimRepository->beginOperation(op, context);
}

//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthorizer::endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result)
{
	m_cimRepository->endOperation(op, context, result);
}


} // end namespace OpenWBEM


OW_AUTHORIZER_FACTORY(OpenWBEM::SimpleAuthorizer, simple);
