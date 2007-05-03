/*******************************************************************************
* Copyright (C) 2001-2005 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_BasicAuthorizer.hpp"
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
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_ServiceIFCNames.hpp"

namespace OW_NAMESPACE
{

using namespace WBEMFlags;

class BasicAccessMgr : public IntrusiveCountableBase
{
public:
	static const String COMPONENT_NAME;
	BasicAccessMgr(const RepositoryIFCRef& pServer);

	enum ECheckOperationContextFlag
	{
		E_SKIP_OPERATION_CONTEXT_CHECK,
		E_CHECK_OPERATION_CONTEXT
	};
	/**
	 * checkAccess will check that access is granted through the ACL. If
	 * Access is not granted, a CIMException will be thrown.
	 * @param required  The permissions required for the operation
	 * @param ns	The name space that access is being check on.
	 * @param context The context from which the ACL info for the user request
	 * 		will be retrieved.
	 */
	void checkAccess(
		char const * op, char const * required,
		String const * pns, OperationContext & context, ECheckOperationContextFlag = E_CHECK_OPERATION_CONTEXT);

	void setEnv(ServiceEnvironmentIFCRef const & env)
	{
		m_env = env;
	}

private:
	String userPermissions(String const *, String const &, OperationContext &);

	RepositoryIFCRef m_pServer;
	ServiceEnvironmentIFCRef m_env;
};
	
String const BasicAccessMgr::COMPONENT_NAME("ow.authorizer.basic");

BasicAccessMgr::BasicAccessMgr(RepositoryIFCRef const & pServer)
: m_pServer(pServer)
{
}

namespace
{
	struct OperationContextDataRestorer
	{
		OperationContextDataRestorer(OperationContext& oc, const String& key)
		: m_oc(oc)
		, m_key(key)
		, m_prevValue(oc.getData(key))
		{
		}
		~OperationContextDataRestorer()
		{
			if (!m_prevValue)
			{
				m_oc.removeData(m_key);
			}
			else
			{
				m_oc.setData(m_key, m_prevValue);
			}
		}
		OperationContext& m_oc;
		String m_key;
		OperationContext::DataRef m_prevValue;
	};

	char const DISABLE_AUTHENTICATION_FLAG[] = "BasicAuthorizer::disableAuthenticationFlag";

	CIMInstance ACLInstance(
		CIMObjectPath const & cop, RepositoryIFC & server,
		OperationContext & context
	)
	{
		try
		{
			return server.getInstance(
				"root/security", cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS,
				E_INCLUDE_CLASS_ORIGIN, NULL, context
			);
		}
		catch (CIMException &)
		{
			return CIMInstance(CIMNULL);
		}
	}

	String capabilityProp(CIMInstance const & ci)
	{
		CIMValue cv = ci.getPropertyValue("capability");
		return cv ? cv.toString() : String();
	}

	bool subset(char const * x, String const & y)
	{
		for ( ; *x; ++x)
		{
			if (y.indexOf(*x) == String::npos)
			{
				return false;
			}
		}
		return true;
	}

}

String BasicAccessMgr::userPermissions(
	String const * pns, String const & username, OperationContext & context
)
{
	if (!pns)
	{
		if (username.empty())
		{
			return "";
		}
		CIMObjectPath cop("OpenWBEM_CreateDeleteNamespaceACL");
		cop.setKeyValue("username", CIMValue(username));
		CIMInstance ci = ACLInstance(cop, *m_pServer, context);
		return ci ? "N" : "";
	}

	// Try to find user permissions
	if (!username.empty())
	{
		CIMObjectPath cop("OpenWBEM_UserACL");
		cop.setKeyValue("username", CIMValue(username));
		cop.setKeyValue("nspace", CIMValue(*pns));
		CIMInstance ci = ACLInstance(cop, *m_pServer, context);
		if (ci)
		{
			return capabilityProp(ci);
		}
	}

	// Now try default permissions
	CIMObjectPath cop("OpenWBEM_NamespaceACL");
	cop.setKeyValue("nspace", CIMValue(*pns));
	CIMInstance ci = ACLInstance(cop, *m_pServer, context);
	return ci ? capabilityProp(ci) : String();
}

void BasicAccessMgr::checkAccess(
	char const * op, char const * required,
	String const * pns, OperationContext & context, ECheckOperationContextFlag checkOperationContext)
{
	// Access levels:
	// rw = read, write (instances)
	// RW = read, write (schema)
	// N = namespace manipulation (create, delete)

	UserInfo userInfo = context.getUserInfo();
	if (userInfo.getInternal())
	{
		return;
	}
	if (checkOperationContext == E_CHECK_OPERATION_CONTEXT && context.getStringDataWithDefault(DISABLE_AUTHENTICATION_FLAG) == "1")
	{
		return;
	}

	// Set up an object to remove it when this function returns
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	// Now set the value so that we won't cause an infinite recursion loop
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");

	String const & ns = pns ? *pns : String("<none>");
	String username = userInfo.getUserName();
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG2(lgr, Format("Checking access to namespace: \"%1\"", ns));
	OW_LOG_DEBUG2(lgr, Format("UserName is: \"%1\" Operation is : %2", username, op));

	String permissions = this->userPermissions(pns, username, context);
	OW_LOG_DEBUG3(lgr, Format("User has permissions: \"%1\"  Required permissions: \"%2\"", permissions, required));
	if (subset(required, permissions))
	{
		Format fmt(
			"ACCESS GRANTED to user \"%1\" for namespace \"%2\"", username, ns
		);
		OW_LOG_INFO(lgr, fmt);
		return;
	}
	else
	{
		Format fmt(
			"ACCESS DENIED to user \"%1\" for namespace \"%2\"", username, ns
		);
		OW_LOG_INFO(lgr, fmt);
		OW_THROWCIM(CIMException::ACCESS_DENIED);
	}
}

//////////////////////////////////////////////////////////////////////////////
BasicAuthorizer::BasicAuthorizer()
	: AuthorizerIFC()
{
}
//////////////////////////////////////////////////////////////////////////////
BasicAuthorizer::~BasicAuthorizer()
{
}
//////////////////////////////////////////////////////////////////////////////
String
BasicAuthorizer::getName() const
{
	return ServiceIFCNames::BasicAuthorizer;
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::setSubRepositoryIFC(const RepositoryIFCRef& src)
{
	m_cimRepository = src;
	ServiceEnvironmentIFCRef env = m_cimRepository->getEnvironment();
	m_accessMgr = IntrusiveReference<BasicAccessMgr>(new BasicAccessMgr(src));
	m_accessMgr->setEnv(env);
}
//////////////////////////////////////////////////////////////////////////////
AuthorizerIFC *
BasicAuthorizer::clone() const
{
	return new BasicAuthorizer(*this);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::open(const String& path)
{
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::close()
{
}
//////////////////////////////////////////////////////////////////////////////
ServiceEnvironmentIFCRef
BasicAuthorizer::getEnvironment() const
{
	return m_cimRepository->getEnvironment();
}

#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::createNameSpace(const String& ns,
	OperationContext& context)
{
	m_accessMgr->checkAccess("createNameSpace", "N", 0, context, BasicAccessMgr::E_SKIP_OPERATION_CONTEXT_CHECK);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->createNameSpace(ns,context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::deleteNameSpace(const String& ns,
	OperationContext& context)
{
	m_accessMgr->checkAccess("deleteNameSpace", "N", 0, context, BasicAccessMgr::E_SKIP_OPERATION_CONTEXT_CHECK);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->deleteNameSpace(ns,context);
}
#endif // #if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::enumNameSpace(StringResultHandlerIFC& result,
	OperationContext& context)
{
	m_cimRepository->enumNameSpace(result,context);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
BasicAuthorizer::getQualifierType(const String& ns,
	const String& qualifierName,
	OperationContext& context)
{
	m_accessMgr->checkAccess("getQualifierType", "R", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->getQualifierType(ns,qualifierName,context);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result,
	OperationContext& context)
{
	m_accessMgr->checkAccess("enumQualifierTypes", "R", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->enumQualifierTypes(ns,result,context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::deleteQualifierType(const String& ns, const String& qualName,
	OperationContext& context)
{
	m_accessMgr->checkAccess("deleteQualifierType", "W", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->deleteQualifierType(ns,qualName,context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::setQualifierType(
	const String& ns,
	const CIMQualifierType& qt, OperationContext& context)
{
	m_accessMgr->checkAccess("setQualifierType", "W", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->setQualifierType(ns,qt,context);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
BasicAuthorizer::getClass(
	const String& ns, const String& className, ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	m_accessMgr->checkAccess("getClass", "R", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->getClass(ns, className, localOnly, includeQualifiers, includeClassOrigin, propertyList, context);
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
BasicAuthorizer::deleteClass(const String& ns, const String& className,
	OperationContext& context)
{
	m_accessMgr->checkAccess("deleteClass", "W", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->deleteClass(ns,className,context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::createClass(const String& ns, const CIMClass& cimClass,
	OperationContext& context)
{
	m_accessMgr->checkAccess("createClass", "W", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->createClass(ns,cimClass,context);
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
BasicAuthorizer::modifyClass(
	const String& ns,
	const CIMClass& cc,
	OperationContext& context)
{
	m_accessMgr->checkAccess("modifyClass", "W", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->modifyClass(ns,cc,context);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, OperationContext& context)
{
	m_accessMgr->checkAccess("enumClasses", "R", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->enumClasses(ns,className,result,deep,localOnly,
		includeQualifiers,includeClassOrigin,context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, OperationContext& context)
{
	m_accessMgr->checkAccess("enumClassNames", "R", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->enumClassNames(ns,className,result,deep,context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep,
	OperationContext& context)
{
	m_accessMgr->checkAccess("enumInstanceNames", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->enumInstanceNames(ns, className, result, deep, context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result, EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, EEnumSubclassesFlag enumSubclasses,
	OperationContext& context)
{
	m_accessMgr->checkAccess("enumInstances", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->enumInstances(ns, className, result, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList, enumSubclasses, context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::enumInstancesWQL(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	const WQLSelectStatement& wss,
	const WQLCompile& wc,
	OperationContext& context)
{
	m_accessMgr->checkAccess("enumInstances", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->enumInstancesWQL(ns, className, result, wss, wc, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
BasicAuthorizer::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	m_accessMgr->checkAccess("getInstance", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, context);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMInstance
BasicAuthorizer::deleteInstance(const String& ns, const CIMObjectPath& cop,
	OperationContext& context)
{
	m_accessMgr->checkAccess("deleteInstance", "w", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->deleteInstance(ns, cop, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
BasicAuthorizer::createInstance(
	const String& ns,
	const CIMInstance& ci,
	OperationContext& context)
{
	// Check to see if user has rights to create the instance
	m_accessMgr->checkAccess("createInstance", "w", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->createInstance(ns, ci, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
BasicAuthorizer::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	OperationContext& context)
{
	m_accessMgr->checkAccess("modifyInstance", "w", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->modifyInstance(ns, modifiedInstance,
			includeQualifiers, propertyList, context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
CIMValue
BasicAuthorizer::getProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, OperationContext& context)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess("getProperty", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->getProperty(ns, name, propertyName, context);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::setProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, const CIMValue& valueArg,
	OperationContext& context)
{
	m_accessMgr->checkAccess("setProperty", "w", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->setProperty(ns, name, propertyName, valueArg, context);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
BasicAuthorizer::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName, const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams, OperationContext& context)
{
	m_accessMgr->checkAccess("invokeMethod", "rw", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	return m_cimRepository->invokeMethod(ns, path, methodName, inParams, outParams, context);
}
//////////////////////////////////////////////////////////////////////
RepositoryIFC::ELockType
BasicAuthorizer::getLockTypeForMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName,
	const CIMParamValueArray& in, 
	OperationContext& context)
{
	return m_cimRepository->getLockTypeForMethod(ns, path, methodName, in, context);
}
//////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::execQuery(
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
BasicAuthorizer::associators(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess("associators", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->associators(ns, path, result, assocClass, resultClass, role, resultRole, includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::associatorsClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess("associatorsClasses", "R", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->associatorsClasses(ns, path, result, assocClass, resultClass, role, resultRole, includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::associatorNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess("associatorNames", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->associatorNames(ns, path, result, assocClass, resultClass, role, resultRole, context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::references(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess("references", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->references(ns, path, result, resultClass, role, includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::referencesClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess("referencesClasses", "R", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->referencesClasses(ns, path, result, resultClass, role, includeQualifiers, includeClassOrigin, propertyList, context);
}
//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::referenceNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass, const String& role,
	OperationContext& context)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess("referenceNames", "r", &ns, context);
	OperationContextDataRestorer OperationContextDataRestorer(context, DISABLE_AUTHENTICATION_FLAG);
	context.setStringData(DISABLE_AUTHENTICATION_FLAG, "1");
	m_cimRepository->referenceNames(ns, path, result, resultClass, role, context);
}
#endif

//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context)
{
	m_cimRepository->beginOperation(op, context);
}

//////////////////////////////////////////////////////////////////////////////
void
BasicAuthorizer::endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result)
{
	m_cimRepository->endOperation(op, context, result);
}


} // end namespace OW_NAMESPACE


OW_AUTHORIZER_FACTORY(OpenWBEM::BasicAuthorizer, basic);
