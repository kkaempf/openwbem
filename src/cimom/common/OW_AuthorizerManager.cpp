#include "OW_config.h"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_AuthorizerManager.hpp"
#include "OW_OperationContext.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"

#define AUTH_ACTIVE_KEY "_aUtHoRiZeR@aCtIvE@kEy_"
#define DISABLED_KEY "__aUtH@mGr@DiSaBlEd__"

namespace OpenWBEM
{

namespace
{
class AuthorizerEnvironment : public ServiceEnvironmentIFC
{
public:

	AuthorizerEnvironment(const CIMOMEnvironmentRef& env,
		OperationContext& context)
		: ServiceEnvironmentIFC()
		, m_env(env)
		, m_context(context)
	{}

	virtual String getConfigItem(const String &name,
		const String& defRetVal) const
	{
		return m_env->getConfigItem(name, defRetVal);
	}
	virtual void setConfigItem(const String &item, const String &value,
		ServiceEnvironmentIFC::EOverwritePreviousFlag overwritePrevious)
	{
		m_env->setConfigItem(item, value, overwritePrevious);
	}
	virtual RequestHandlerIFCRef getRequestHandler(const String &id)
	{
		return m_env->getRequestHandler(id);
	}

	virtual CIMInstanceArray getInteropInstances(const String &className) const
	{
		return m_env->getInteropInstances(className);
	}
	virtual void setInteropInstance(const CIMInstance &inst)
	{
		m_env->setInteropInstance(inst);
	}
	virtual void addSelectable(const SelectableIFCRef &obj,
		const SelectableCallbackIFCRef &cb)
	{
		m_env->addSelectable(obj, cb);
	}
	virtual void removeSelectable(const SelectableIFCRef &obj)
	{
		m_env->removeSelectable(obj);
	}

	virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext &context,
		ESendIndicationsFlag doIndications,
		EBypassProvidersFlag bypassProviders)
	{
		return m_env->getCIMOMHandle(m_context, doIndications, bypassProviders,
			CIMOMEnvironment::E_NO_LOCKING);
	}

	virtual LoggerRef getLogger() const
	{
		return m_env->getLogger();
	}

	virtual bool authenticate(String &userName, const String &info,
		String &details, OperationContext &context)
	{
		return m_env->authenticate(userName, info, details, context);
	}

	virtual OperationContext& getOperationContext()
	{
		return m_context;
	}
private:
	CIMOMEnvironmentRef m_env;
	OperationContext& m_context;
};

inline ServiceEnvironmentIFCRef createAuthEnvRef(OperationContext& context,
	const CIMOMEnvironmentRef& env)
{
	return ServiceEnvironmentIFCRef(new AuthorizerEnvironment(env, context));
}

//////////////////////////////////////////////////////////////////////////////
struct AuthorizerMarker
{
	OperationContext& m_context;
	AuthorizerMarker(OperationContext& context)
		: m_context(context)
	{
		m_context.setStringData(AUTH_ACTIVE_KEY, "1");
	}
	~AuthorizerMarker()
	{
		m_context.removeData(AUTH_ACTIVE_KEY);
	}
	
	static bool active(OperationContext& context)
	{
		return context.getStringDataWithDefault(AUTH_ACTIVE_KEY) == "1";
	}
};

}	// End of unnamed namespace


//////////////////////////////////////////////////////////////////////////////
void 
AuthorizerManager::turnOff(
	OperationContext& context)
{
	context.setStringData(DISABLED_KEY, "1");

}
//////////////////////////////////////////////////////////////////////////////
void 
AuthorizerManager::turnOn(
	OperationContext& context)
{
	try
	{
		context.removeData(DISABLED_KEY);
	}
	catch(...)
	{
		// Ignore?
	}
}
//////////////////////////////////////////////////////////////////////////////
bool
AuthorizerManager::isOn(
	OperationContext& context)
{
	return context.getStringDataWithDefault(DISABLED_KEY) != "1";
}
//////////////////////////////////////////////////////////////////////////////
AuthorizerManager::AuthorizerManager()
	: m_authorizer()
	, m_initialized(false)
{
}

//////////////////////////////////////////////////////////////////////////////
AuthorizerManager::AuthorizerManager(Authorizer2IFCRef authorizerRef)
	: m_authorizer(authorizerRef)
{
}
//////////////////////////////////////////////////////////////////////////////
AuthorizerManager::~AuthorizerManager()
{
}

//////////////////////////////////////////////////////////////////////////////
void 
AuthorizerManager::init(CIMOMEnvironmentRef& env)
{
	if (!m_initialized)
	{
		if (m_authorizer)
		{
			OperationContext oc;
			ServiceEnvironmentIFCRef envref = createAuthEnvRef(oc, env);
			m_authorizer->init(envref);

			//virtual void init(ServiceEnvironmentIFCRef&) {}

		}
		m_initialized = true;
	}
}

//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowReadInstance(
	const CIMOMEnvironmentRef& env,
	const String& ns,
	const String& className,
	const StringArray* clientPropertyList,
	StringArray& authorizedPropertyList,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowReadInstance(
		createAuthEnvRef(context, env), ns, className,
		clientPropertyList, authorizedPropertyList, context);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowWriteInstance(
	const CIMOMEnvironmentRef& env,
	const String& ns, 
	const CIMObjectPath& op,
	Authorizer2IFC::EDynamicFlag dynamic,
	Authorizer2IFC::EWriteFlag flag,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowWriteInstance(
		createAuthEnvRef(context, env), ns, op, dynamic, flag,
		context);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowReadSchema(
	const CIMOMEnvironmentRef& env,
	const String& ns,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowReadSchema(
		createAuthEnvRef(context, env), ns, context);
}
#if !defined(OW_DISABLE_SCHEMA_MANIPULATION) || !defined(OW_DISABLE_QUALIFIER_DECLARATION)
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowWriteSchema(
	const CIMOMEnvironmentRef& env,
	const String& ns, 
	Authorizer2IFC::EWriteFlag flag,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowWriteSchema(
		createAuthEnvRef(context, env), ns, flag, context);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowAccessToNameSpace(
	const CIMOMEnvironmentRef& env,
	const String& ns,
	Authorizer2IFC::EAccessType accessType,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowAccessToNameSpace(
		createAuthEnvRef(context, env), ns, accessType, context);
}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowCreateNameSpace(
	const CIMOMEnvironmentRef& env,
	const String& ns,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowCreateNameSpace(
		createAuthEnvRef(context, env), ns, context);
}
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowDeleteNameSpace(
	const CIMOMEnvironmentRef& env,
	const String& ns,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowDeleteNameSpace(
		createAuthEnvRef(context, env), ns, context);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool
AuthorizerManager::allowEnumNameSpace(
	const CIMOMEnvironmentRef& env,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowEnumNameSpace(
		createAuthEnvRef(context, env), context);
}

//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowMethodInvocation(
	const CIMOMEnvironmentRef& env, 
	const String& ns, 
	const CIMObjectPath& path,
	const String& methodName,
	OperationContext& context)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if (AuthorizerMarker::active(context)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(context))
	{
		return true;
	}

	AuthorizerMarker am(context);
	return m_authorizer->doAllowMethodInvocation(
		createAuthEnvRef(context, env), ns, path, methodName,
		context);
}


}	// wEnd of OpenWBEM namespace
