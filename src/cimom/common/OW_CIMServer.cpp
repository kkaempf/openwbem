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

/**
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMServer.hpp"
#include "OW_CIMValueCast.hpp"
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
#include "OW_ResultHandlers.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;

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
		virtual String getConfigItem(const String &name,
			const String& defRetVal="") const
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

	inline ProviderEnvironmentIFCRef createProvEnvRef(OperationContext& context,
		const CIMOMEnvironmentRef& env)
	{
		return ProviderEnvironmentIFCRef(new CIMServerProviderEnvironment(context, env));
	}

	inline void logOperation(const LoggerRef& lgr, const OperationContext& context, const char* operation, const String& ns, const String& objectName = String())
	{
		// avoid the overhead of formatting the message if we're not going to log this.
		ELogLevel level = lgr->getLogLevel();
		if (level == E_DEBUG_LEVEL || level == E_INFO_LEVEL)
		{
			String userString;
			String user = context.getStringDataWithDefault(OperationContext::USER_NAME);
			if (!user.empty())
			{
				userString = " for user: " + user;
			}
			String optObjectName;
			if (!objectName.empty())
			{
				optObjectName = ':' + objectName;
			}
			lgr->logInfo(Format("CIMServer doing operation: %1 on %2%3%4", operation, ns, optObjectName, userString));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
CIMServer::CIMServer(CIMOMEnvironmentRef env,
	const ProviderManagerRef& provManager,
	const RepositoryIFCRef& cimRepository)
	: RepositoryIFC()
	, m_provManager(provManager)
	, m_nsClass_Namespace(CIMNULL)
	, m_env(env)
	, m_cimRepository(cimRepository)
	, m_realRepository(dynamic_pointer_cast<CIMRepository>(m_cimRepository))
	, m_authorizerMgr(env->getAuthorizerManager())
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
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::shutdown()
{
	m_provManager = 0;
	m_env = 0;
	m_cimRepository = 0;
	m_realRepository = 0;
	m_authorizerMgr = 0;
}
//////////////////////////////////////////////////////////////////////////////
void 
CIMServer::_checkNameSpaceAccess(OperationContext& context, const String& ns,
	Authorizer2IFC::EAccessType acType)
{
	if(!m_authorizerMgr->allowAccessToNameSpace(
		createProvEnvRef(context, m_env), ns, acType))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Access to namespace %1 is not allowed", ns).c_str());
	}
}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::createNameSpace(const String& ns,
	OperationContext& context)
{
	if(!m_authorizerMgr->allowCreateNameSpace(
		createProvEnvRef(context, m_env), ns))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Creation of namespace %1 is not allowed",
				ns).c_str());
	}

	logOperation(m_env->getLogger(), context, "createNameSpace", ns);
	m_cimRepository->createNameSpace(ns,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::deleteNameSpace(const String& ns,
	OperationContext& context)
{
	if(!m_authorizerMgr->allowDeleteNameSpace(
		createProvEnvRef(context, m_env), ns))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Deletion of namespace %1 is not allowed",
				ns).c_str());
	}

	logOperation(m_env->getLogger(), context, "deleteNameSpace", ns);
	m_cimRepository->deleteNameSpace(ns,context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::enumNameSpace(StringResultHandlerIFC& result,
	OperationContext& context)
{
	if(!m_authorizerMgr->allowEnumNameSpace(
		createProvEnvRef(context, m_env)))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			"Enumeration of namespaces is not allowed");
	}

	// Don't need to check ACLs, since this is a result of calling enumInstances.
	logOperation(m_env->getLogger(), context, "enumNameSpace", String());
	m_cimRepository->enumNameSpace(result,context);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
CIMServer::getQualifierType(const String& ns,
	const String& qualifierName,
	OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	if(!m_authorizerMgr->allowReadSchema(
		createProvEnvRef(context, m_env), ns))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Read of qualifier %1 is not allowed",
				qualifierName).c_str());
	}

	logOperation(m_env->getLogger(), context, "GetQualifier", ns, qualifierName);
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	if(!m_authorizerMgr->allowReadSchema(
		createProvEnvRef(context, m_env), ns))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			"Enumeration of qualifiers is not allowed");
	}

	logOperation(m_env->getLogger(), context, "EnumerateQualifiers", ns);
	m_cimRepository->enumQualifierTypes(ns,result,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::deleteQualifierType(const String& ns, const String& qualName,
	OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);

	if(!m_authorizerMgr->allowWriteSchema(
		createProvEnvRef(context, m_env), ns, Authorizer2IFC::E_DELETE))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Deletion of qualifier %1 is not allowed",
				qualName).c_str());
	}

	logOperation(m_env->getLogger(), context, "DeleteQualifier", ns, qualName);
	m_cimRepository->deleteQualifierType(ns,qualName,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::setQualifierType(
	const String& ns,
	const CIMQualifierType& qt, OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);

	if(!m_authorizerMgr->allowWriteSchema(
		createProvEnvRef(context, m_env), ns, Authorizer2IFC::E_MODIFY))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Modification of qualifier %1 is not allowed",
				qt.getName()).c_str());
	}

	logOperation(m_env->getLogger(), context, "SetQualifier", ns, qt.getName());
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	if(!m_authorizerMgr->allowReadSchema(
		createProvEnvRef(context, m_env), ns))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Read of class %1 is not allowed", className).c_str());
	}

	logOperation(m_env->getLogger(), context, "GetClass", ns, className);
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);

	if(!m_authorizerMgr->allowWriteSchema(
		createProvEnvRef(context, m_env), ns, Authorizer2IFC::E_DELETE))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Deletion of class %1 is not allowed",
				className).c_str());
	}

	logOperation(m_env->getLogger(), context, "DeleteClass", ns, className);
	return m_cimRepository->deleteClass(ns,className,context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMServer::createClass(const String& ns, const CIMClass& cimClass,
	OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);

	if(!m_authorizerMgr->allowWriteSchema(
		createProvEnvRef(context, m_env), ns, Authorizer2IFC::E_CREATE))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Creation of class %1 is not allowed",
				cimClass.getName()).c_str());
	}

	logOperation(m_env->getLogger(), context, "GetClass", ns, cimClass.getName());
	if(cimClass.getName().equals(CIMClass::NAMESPACECLASS))
	{
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS,
			Format("Creation of class %1 is not allowed",
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);

	if(!m_authorizerMgr->allowWriteSchema(
		createProvEnvRef(context, m_env), ns, Authorizer2IFC::E_MODIFY))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("Modification of class %1 is not allowed",
				cc.getName()).c_str());
	}

	logOperation(m_env->getLogger(), context, "ModifyClass", ns, cc.getName());
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	if(!m_authorizerMgr->allowReadSchema(
		createProvEnvRef(context, m_env), ns))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			"Enumeration of classes is not allowed");
	}

	logOperation(m_env->getLogger(), context, "EnumerateClasses", ns, className);
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);
	if(!m_authorizerMgr->allowReadSchema(
		createProvEnvRef(context, m_env), ns))
	{
		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			"Enumeration of class names is not allowed");
	}

	logOperation(m_env->getLogger(), context, "EnumerateClassNames", ns, className);
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
				m_env->logDebug(Format("CIMServer InstNameEnumerator enumerated derived instance names: %1:%2", ns,
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	logOperation(m_env->getLogger(), context, "EnumerateInstanceNames", ns, className);

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
				m_env->logDebug(Format("CIMServer InstEnumerator Enumerating"
					" derived instance names: %1:%2", ns, cc.getName()));
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	logOperation(m_env->getLogger(), context, "EnumerateInstances", ns, className);

	CIMClass theTopClass = _instGetClass(ns, className, E_NOT_LOCAL_ONLY, 
		E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, context);

	InstEnumerator ie(ns, result, context, m_env, this, deep, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, theTopClass);
	ie.handle(theTopClass);
	// If this is the namespace class then only do one class
	if(theTopClass.getName().equals(CIMClass::NAMESPACECLASS)
	   || enumSubclasses == E_DONT_ENUM_SUBCLASSES)
	{
		return;
	}
	else
	{
		// TODO: measure whether it would be faster to use
		// enumClassNames + getClass() here.
		// do subclasses
		m_cimRepository->enumClasses(ns, className, ie, E_DEEP,
			E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN,
			context);
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
		{
		}

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

	class SecondaryInstanceProviderHandler : public CIMInstanceResultHandlerIFC
	{
	public:
		SecondaryInstanceProviderHandler(
			OperationContext& context_,
			const CIMOMEnvironmentRef& env_,
			const String& ns_,
			const String& className_,
			ELocalOnlyFlag localOnly_,
			EDeepFlag deep_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propertyList_,
			const CIMClass& theTopClass_,
			const CIMClass& theClass_,
			const SecondaryInstanceProviderIFCRefArray& secProvs_,
			CIMInstanceResultHandlerIFC& result_)
		: context(context_)
		, env(env_)
		, ns(ns_)
		, className(className_)
		, localOnly(localOnly_)
		, deep(deep_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		, theTopClass(theTopClass_)
		, theClass(theClass_)
		, secProvs(secProvs_)
		, result(result_)
		{
		}
		void doHandle(const CIMInstance& i)
		{
			CIMInstanceArray savedInstances;
			savedInstances.push_back(i);
			// now let all the secondary providers have at the instance
			for (size_t i = 0; i < secProvs.size(); ++i)
			{
				secProvs[i]->filterInstances(createProvEnvRef(context, env), ns, 
					className, savedInstances, localOnly, deep, includeQualifiers, 
					includeClassOrigin, propertyList, theTopClass, theClass );
			}
			for (size_t i = 0; i < savedInstances.size(); ++i)
			{
				result.handle(savedInstances[i]);
			}
		}
	private:
		OperationContext& context;
		const CIMOMEnvironmentRef& env;
		const String& ns;
		const String& className;
		ELocalOnlyFlag localOnly;
		EDeepFlag deep;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propertyList;
		const CIMClass& theTopClass;
		const CIMClass& theClass;
		const SecondaryInstanceProviderIFCRefArray& secProvs;
		CIMInstanceResultHandlerIFC& result;
	};

}
//////////////////////////////////////////////////////////////////////////////
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

	// See if the authorizer allows reading of these instances and give it
	// an opportunity to modify the property list
	StringArray authorizedPropertyList;
	if(!m_authorizerMgr->allowReadInstance(
		createProvEnvRef(context, m_env), ns, className,
		propertyList, authorizedPropertyList))
	{
		m_env->logDebug(Format("Authorizer did NOT authorize reading of %1"
			" instances from namespace %2", className, ns));
		return;
	}

	// If the authorizer modified the property list, then set the
	// property list parm to point to it.
	if(authorizedPropertyList.size() > 0)
	{
		propertyList = &authorizedPropertyList;
		m_env->logDebug(Format("Authorizer modified property list for reading"
			" of %1 instances from namespace %2", className, ns));
	}

	// If we have secondary instance providers, we need to use a new result 
	// handler so we can pass instances to the seconday instance providers.
	// Otherwise, we want to just pass them on to result.  presult will point
	// to either result or a SecondaryInstanceProviderHandler.
	SecondaryInstanceProviderIFCRefArray secProvs = 
		_getSecondaryInstanceProviders(ns, className, context);

	SecondaryInstanceProviderHandler secondaryHandler(context, m_env, ns, 
		className, localOnly, deep, includeQualifiers, includeClassOrigin, 
		propertyList, theTopClass, theClass, secProvs, result);

	CIMInstanceResultHandlerIFC* presult = &result;
	if (!secProvs.empty())
	{
		presult = &secondaryHandler;
	}

	if (instancep)
	{
		if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
		{
			m_env->logDebug(Format("CIMServer calling provider to"
				" enumerate instances: %1:%2", ns, className));
		}

		// not going to use these, the provider ifc/providers are now
		// responsible for it.
		//HandleLocalOnlyAndDeep handler1(result,theTopClass,localOnly,deep);
		//HandleProviderInstance handler2(includeQualifiers, includeClassOrigin, propertyList, handler1);
		instancep->enumInstances(
			createProvEnvRef(context, m_env), ns, className, *presult,
			localOnly, deep, includeQualifiers, includeClassOrigin,
			propertyList, theTopClass, theClass);
	}
	else
	{
		HandleLocalOnlyAndDeep handler(*presult, theTopClass, localOnly, deep);
		// don't pass along deep and localOnly flags, because the handler has
		// to take care of it.  m_cimRepository can't do it right, because we
		// can't pass in theTopClass.
		m_cimRepository->enumInstances(ns, className, handler, E_DEEP,
			E_NOT_LOCAL_ONLY, includeQualifiers, includeClassOrigin,
			propertyList, E_DONT_ENUM_SUBCLASSES, context);
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
	const CIMObjectPath& instanceName_,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMClass* pOutClass,
	OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);
	logOperation(m_env->getLogger(), context, "GetInstance", ns, instanceName_.toString());

	CIMObjectPath instanceName(instanceName_);
	String className = instanceName.getClassName();
	CIMClass cc = _instGetClass(ns, className,
		E_NOT_LOCAL_ONLY,
		E_INCLUDE_QUALIFIERS,
		E_INCLUDE_CLASS_ORIGIN,
		0, context);
	if(pOutClass)
	{
		*pOutClass = cc;
	}
	instanceName.syncWithClass(cc);
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, cc, context);

	// See if the authorizer allows reading of these instances and give it
	// an opportunity to modify the property list
	StringArray authorizedPropertyList;
	if(!m_authorizerMgr->allowReadInstance(
		createProvEnvRef(context, m_env), ns, className,
		propertyList, authorizedPropertyList))
	{
		m_env->logDebug(Format("Authorizer did NOT authorize reading of %1"
			" instances from namespace %2", className, ns));

		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("You are not authorized to read %1 instances from"
				" namespace %2", className, ns).c_str());
	}

	// If the authorizer changed the property list, then use it.
	if(authorizedPropertyList.size() > 0)
	{
		propertyList = &authorizedPropertyList;
	}

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
	
	SecondaryInstanceProviderIFCRefArray secProvs = _getSecondaryInstanceProviders(ns, className, context);
	if (!secProvs.empty())
	{
		CIMInstanceArray cia; cia.push_back(ci);
		// now let all the secondary providers have at the instance
		for (size_t i = 0; i < secProvs.size(); ++i)
		{
			secProvs[i]->filterInstances(createProvEnvRef(context, m_env), ns, 
				className, cia, localOnly, E_DEEP, includeQualifiers, 
				includeClassOrigin, propertyList, cc, cc );
		}
		OW_ASSERT(cia.size() == 1); // providers shouldn't add/remove from the array.
		ci = cia[0];
	}
	
	return ci;
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMServer::deleteInstance(const String& ns, const CIMObjectPath& cop_,
	OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);
	logOperation(m_env->getLogger(), context, "DeleteInstance", ns, cop_.toString());

	CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);
	if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
	{
		m_env->logDebug(Format("CIMServer::deleteInstance.  cop = %1",
			cop.toString()));
	}
	CIMClass theClass(CIMNULL);
	CIMInstance oldInst = getInstance(ns, cop, E_NOT_LOCAL_ONLY,
		E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL, &theClass, context);
	cop.syncWithClass(theClass);
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass, context);

	// Allow authorizer a chance to determine if the deletion is allowed
	if(!m_authorizerMgr->allowWriteInstance(
		createProvEnvRef(context, m_env), ns, cop,
		(instancep) ? Authorizer2IFC::E_DYNAMIC : Authorizer2IFC::E_NOT_DYNAMIC,
		Authorizer2IFC::E_DELETE))
	{
		m_env->logDebug(Format("Authorizer did NOT authorize deletion of %1"
			" instances from namespace %2", theClass.getName(), ns));

		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("You are not authorized to delete %1 instances from"
				" namespace %2", theClass.getName(), ns).c_str());
	}

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
	
	SecondaryInstanceProviderIFCRefArray secProvs = _getSecondaryInstanceProviders(ns, cop.getClassName(), context);
	// now let all the secondary providers know about the delete
	for (size_t i = 0; i < secProvs.size(); ++i)
	{
		secProvs[i]->deleteInstance(createProvEnvRef(context, m_env), ns, cop);
	}
	
	return oldInst;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
CIMServer::createInstance(
	const String& ns,
	const CIMInstance& ci,
	OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);
	logOperation(m_env->getLogger(), context, "CreateInstance", ns, ci.getClassName());

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
					Format("Unable to create instance because class (%1)"
						" is abstract", theClass.getName()).c_str());
		}
	}
	// Make sure instance jives with class definition
	CIMInstance lci(ci);
	lci.syncWithClass(theClass, E_INCLUDE_QUALIFIERS);
	if (m_env->getLogger()->getLogLevel() == E_DEBUG_LEVEL)
	{
		m_env->logDebug(Format("CIMServer::createInstance.  ns = %1, "
			"instance = %2", ns, lci.toMOF()));
	}
	CIMObjectPath rval(CIMNULL);
	InstanceProviderIFCRef instancep = _getInstanceProvider(ns, theClass, context);

	// Allow authorizer a chance to determine if the creation is allowed
	CIMObjectPath cop(ns, lci);

	if(!m_authorizerMgr->allowWriteInstance(
		createProvEnvRef(context, m_env), ns, cop,
		(instancep) ? Authorizer2IFC::E_DYNAMIC : Authorizer2IFC::E_NOT_DYNAMIC,
		Authorizer2IFC::E_CREATE))
	{
		m_env->logDebug(Format("Authorizer did NOT authorize creation of %1"
			" instances	 in namespace %2", lci.getClassName(), ns));

		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("You are not authorized to create %1 instances in"
				" namespace %2", lci.getClassName(), ns).c_str());
	}

	if (instancep)
	{
		rval = instancep->createInstance(createProvEnvRef(context, m_env), ns, lci);
	}
	else
	{
		rval = m_cimRepository->createInstance(ns, lci, context);
	}

	SecondaryInstanceProviderIFCRefArray secProvs = _getSecondaryInstanceProviders(ns, className, context);
	// now let all the secondary providers know about the create
	for (size_t i = 0; i < secProvs.size(); ++i)
	{
		secProvs[i]->createInstance(createProvEnvRef(context, m_env), ns, lci);
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);
	logOperation(m_env->getLogger(), context, "ModifyInstance", ns, modifiedInstance.getClassName());

	CIMInstance oldInst(CIMNULL);
	CIMClass theClass = _instGetClass(ns, modifiedInstance.getClassName(),
		E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, context);
	InstanceProviderIFCRef instancep(_getInstanceProvider(ns, theClass, context));

	// Make sure instance jives with class definition
	CIMInstance lci(modifiedInstance);
	lci.syncWithClass(theClass, E_INCLUDE_QUALIFIERS);

	CIMObjectPath cop(ns, lci);

	// Allow authorizer a chance to determine if the mofify is allowed
	if(!m_authorizerMgr->allowWriteInstance(
		createProvEnvRef(context, m_env), ns, cop,
		(instancep) ? Authorizer2IFC::E_DYNAMIC : Authorizer2IFC::E_NOT_DYNAMIC,
		Authorizer2IFC::E_MODIFY))
	{
		m_env->logDebug(Format("Authorizer did NOT authorize modification of %1"
			" instances in namespace %2", lci.getClassName(), ns));

		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("You are not authorized to modify %1 instances in"
				" namespace %2", lci.getClassName(), ns).c_str());
	}
	logOperation(m_env->getLogger(), context, "ModifyInstance", ns, modifiedInstance.getClassName());

	if(!instancep)
	{
		// No instance provider qualifier found
		oldInst = m_cimRepository->modifyInstance(ns, lci,
			includeQualifiers, propertyList, context);
	}
	else
	{
		// Look for dynamic instances
		oldInst = getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL, NULL,
			context);
		instancep->modifyInstance(createProvEnvRef(context, m_env), ns,
			lci, oldInst, includeQualifiers, propertyList, theClass);
	}
	OW_ASSERT(oldInst);
	
	SecondaryInstanceProviderIFCRefArray secProvs = _getSecondaryInstanceProviders(ns, modifiedInstance.getClassName(), context);
	// now let all the secondary providers know about the modify
	for (size_t i = 0; i < secProvs.size(); ++i)
	{
		secProvs[i]->modifyInstance(createProvEnvRef(context, m_env), ns, lci, oldInst, includeQualifiers, propertyList, theClass);
	}

	
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
		getInstance(ns,icop,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,0,context);
		return true;
	}
	catch(CIMException&)
	{
		return false;
	}
}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMServer::getProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);
	
	logOperation(m_env->getLogger(), context, "GetProperty", ns, name.toString() + '.' + propertyName);

	CIMClass theClass = _instGetClass(ns,name.getClassName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
	CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}
	CIMInstance ci = getInstance(ns, name, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_WRITE);

	logOperation(m_env->getLogger(), context, "SetProperty", ns, name.toString());
	m_env->logInfo(Format("SetProperty: %1=%2", propertyName, valueArg));

	CIMClass theClass = _instGetClass(ns, name.getClassName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
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
	StringArray propertyList;
	propertyList.push_back(propertyName);
	modifyInstance(ns, ci, E_INCLUDE_QUALIFIERS, &propertyList, context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#endif //#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMServer::invokeMethod(
	const String& ns,
	const CIMObjectPath& path_,
	const String& methodName, const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams, OperationContext& context)
{
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READWRITE);

	logOperation(m_env->getLogger(), context, "invokeMethod", ns, path_.toString() + '.' + methodName);

	// Allow authorizer a chance to determine if the mofify is allowed
	if(!m_authorizerMgr->allowMethodInvocation(
		createProvEnvRef(context, m_env), ns, path_, methodName))
	{
		m_env->logDebug(Format("Authorizer did NOT authorize invocation of"
			" method %1 on object path %2", methodName, path_.toString()));

		OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
			Format("You are not authorized to invoke method %1 on object %2",
				methodName, path_.toString()).c_str());
	}

	CIMObjectPath path(path_);
	CIMClass cc = getClass(ns, path.getClassName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
	path.syncWithClass(cc);
	CIMPropertyArray keys = path.getKeys();
	// If this is an instance, ensure it exists.
	if(keys.size() > 0)
	{
		if(!_instanceExists(ns, path, context))
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND,
				Format("Instance not found: %1", path.toString()).c_str());
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
		OW_THROWCIMMSG(CIMException::NOT_FOUND, Format("No provider for method %1", methodName).c_str());
	}
	CIMParameterArray methodInParams = method.getINParameters();
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
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, Format(
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
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, Format(
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
			Format("Invalid provider: %1", e.getMessage()).c_str());
	}
	return instancep;
}
//////////////////////////////////////////////////////////////////////////////
SecondaryInstanceProviderIFCRefArray
CIMServer::_getSecondaryInstanceProviders(const String& ns, const String& className, OperationContext& context)
{
	SecondaryInstanceProviderIFCRefArray rval;
	try
	{
		rval = m_provManager->getSecondaryInstanceProviders(createProvEnvRef(context, m_env), ns, className);
	}
	catch (const NoSuchProviderException& e)
	{
		// This will only happen if the provider qualifier is incorrect
		OW_THROWCIMMSG(CIMException::FAILED,
			Format("Invalid provider: %1", e.getMessage()).c_str());
	}
	return rval;
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
				Format("Invalid provider: %1", e.getMessage()).c_str());
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);
	WQLIFCRef wql = m_env->getWQLRef();
	if (wql && wql->supportsQueryLanguage(queryLanguage))
	{
		logOperation(m_env->getLogger(), context, "ExecQuery", ns, query);

		CIMOMHandleIFCRef lch = m_env->getCIMOMHandle(context,
				ServiceEnvironmentIFC::E_DONT_SEND_INDICATIONS,
				ServiceEnvironmentIFC::E_USE_PROVIDERS,
				CIMOMEnvironment::E_NO_LOCKING);
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


namespace
{
class InstanceAuthorizer : public CIMInstanceResultHandlerIFC
{
public:
	InstanceAuthorizer(
		ProviderEnvironmentIFCRef env_,
		AuthorizerManagerRef authorizerManager_,
		CIMInstanceResultHandlerIFC& result_,
		const String& ns_,
		EIncludeQualifiersFlag includeQualifiers_,
		EIncludeClassOriginFlag includeClassOrigin_,
		const StringArray* propertyList_)
	: env(env_)
	, authorizerMgr(authorizerManager_)
	, result(result_)
	, ns(ns_)
	, includeQualifiers(includeQualifiers_)
	, includeClassOrigin(includeClassOrigin_)
	, clientPropertyList(propertyList_)
	, authorizedPropertyList()
	{
	}

protected:
	virtual void doHandle(const CIMInstance &inst)
	{
		// See if the authorizer allows reading of these instances and give it
		// an opportunity to modify the properties returned.
		if(authorizerMgr->allowReadInstance(
			env, ns, inst.getClassName(), clientPropertyList,
			authorizedPropertyList))
		{
			if(authorizedPropertyList.size() > 0)
			{
				result.handle(inst.filterProperties(
					authorizedPropertyList, includeQualifiers,
					includeClassOrigin, false));
				authorizedPropertyList.clear();
			}
			else
			{
				result.handle(inst);
			}
		}
	}
private:
	ProviderEnvironmentIFCRef env;
	AuthorizerManagerRef authorizerMgr;
	CIMInstanceResultHandlerIFC& result;
	String ns;
	EIncludeQualifiersFlag includeQualifiers;
	EIncludeClassOriginFlag includeClassOrigin;
	const StringArray* clientPropertyList;
	StringArray authorizedPropertyList;
};
}	// End of unnamed namespace

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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	logOperation(m_env->getLogger(), context, "Associators", ns, path.toString());

	// Allow authorizer to intercept instances that will be returned
	InstanceAuthorizer ia(createProvEnvRef(context, m_env),
		m_authorizerMgr, result, ns, includeQualifiers,
		includeClassOrigin, propertyList);

	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, &ia, 0, 0,
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	logOperation(m_env->getLogger(), context, "AssociatorsClasses", ns, path.toString());

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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);
	
	logOperation(m_env->getLogger(), context, "AssociatorNames", ns, path.toString());
	
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);

	logOperation(m_env->getLogger(), context, "References", ns, path.toString());
	
	// Allow authorizer to intercept instances that will be returned
	InstanceAuthorizer ia(createProvEnvRef(context, m_env),
		m_authorizerMgr, result, ns, includeQualifiers,
		includeClassOrigin, propertyList);

	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, &ia, 0, 0, context);
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);
	
	logOperation(m_env->getLogger(), context, "ReferencesClasses", ns, path.toString());
	
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
	_checkNameSpaceAccess(context, ns, Authorizer2IFC::E_READ);
	
	logOperation(m_env->getLogger(), context, "ReferenceNames", ns, path.toString());
	
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
					Format("class %1 is not an association", cc.getName()).c_str());
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
	path.syncWithClass(getClass(ns, path.getClassName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context));
	// Get all association classes from the repository
	// If the result class was specified, only children of it will be
	// returned.
	CIMClassArray dynamicAssocs;
	StringArray resultClassNames;
	assocClassSeparator assocClassResult(!m_realRepository || resultClass.empty() ? 0 : &resultClassNames, dynamicAssocs, *this, context, ns, m_env->getLogger());
	_getAssociationClasses(ns, resultClass, path.getClassName(), assocClassResult, role, context);
	if (path.isClassPath())
	{
		// Let authorizer determine if user is allowed to read schema items
		if(!m_authorizerMgr->allowReadSchema(
			createProvEnvRef(context, m_env), ns))
		{
			OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
				Format("You are not allowed to read classes from namespace %1",
					ns).c_str());
		}

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

		// we need to remove dups from dynamicAssocs
		std::sort(dynamicAssocs.begin(),  dynamicAssocs.end());
		dynamicAssocs.erase(std::unique(dynamicAssocs.begin(),  dynamicAssocs.end()), dynamicAssocs.end());
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
	path.syncWithClass(getClass(ns, path.getClassName(),E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context));
	// Get association classes from the association repository
	CIMClassArray dynamicAssocs;
	StringArray assocClassNames;
	assocClassSeparator assocClassResult(!m_realRepository || assocClassName.empty() ? 0 : &assocClassNames, dynamicAssocs, *this, context, ns, m_env->getLogger());
	_getAssociationClasses(ns, assocClassName, path.getClassName(), assocClassResult, role, context);
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
		// Let authorizer determine if user is allowed to read schema items
		if(!m_authorizerMgr->allowReadSchema(
			createProvEnvRef(context, m_env), ns))
		{
			OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
				Format("You are not allowed to read classes from namespace %1",
					ns).c_str());
		}

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
		// we need to remove dups from dynamicAssocs
		std::sort(dynamicAssocs.begin(),  dynamicAssocs.end());
		dynamicAssocs.erase(std::unique(dynamicAssocs.begin(),  dynamicAssocs.end()), dynamicAssocs.end());
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
		CIMClass cc = getClass(ns,assocClassName,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0,context);
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

