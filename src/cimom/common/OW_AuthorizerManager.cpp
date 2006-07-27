#include "OW_config.h"
#include "OW_AuthorizerManager.hpp"
#include "OW_LocalOperationContext.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Array.hpp"
#include "OW_ServiceIFCNames.hpp"

namespace OW_NAMESPACE
{

namespace
{

const String AUTH_ACTIVE_KEY("_aUtHoRiZeR@aCtIvE@kEy_");
const String DISABLED_KEY("__aUtH@mGr@DiSaBlEd__");
const String COMPONENT_NAME("ow.authorizer.AuthorizerManager");

class AuthorizerEnvironment : public ServiceEnvironmentIFC
{
public:

	AuthorizerEnvironment(const ServiceEnvironmentIFCRef& env,
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
	virtual StringArray getMultiConfigItem(const String &itemName, 
		const StringArray& defRetVal, const char* tokenizeSeparator) const
	{
		return m_env->getMultiConfigItem(itemName, defRetVal, tokenizeSeparator);
	}
	virtual void setConfigItem(const String &item, const String &value,
		ServiceEnvironmentIFC::EOverwritePreviousFlag overwritePrevious)
	{
		m_env->setConfigItem(item, value, overwritePrevious);
	}
	virtual RequestHandlerIFCRef getRequestHandler(const String &id) const
	{
		return m_env->getRequestHandler(id);
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
		EBypassProvidersFlag bypassProviders,
		ELockingFlag locking) const
	{
		return m_env->getCIMOMHandle(m_context, bypassProviders, locking);
	}

	virtual bool authenticate(String &userName, const String &info,
		String &details, OperationContext &context) const
	{
		return m_env->authenticate(userName, info, details, context);
	}

	virtual OperationContext& getOperationContext()
	{
		return m_context;
	}
private:
	ServiceEnvironmentIFCRef m_env;
	OperationContext& m_context;
};

inline ServiceEnvironmentIFCRef createAuthEnvRef(OperationContext& context,
	const ServiceEnvironmentIFCRef& env)
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
AuthorizerManager::AuthorizerManager(const Authorizer2IFCRef& authorizerRef)
	: m_authorizer(authorizerRef)
{
}
//////////////////////////////////////////////////////////////////////////////
AuthorizerManager::~AuthorizerManager()
{
}

//////////////////////////////////////////////////////////////////////////////
String
AuthorizerManager::getName() const
{
	return ServiceIFCNames::AuthorizerManager;
}

//////////////////////////////////////////////////////////////////////////////
void
AuthorizerManager::init(const ServiceEnvironmentIFCRef& env)
{
	if (!m_initialized)
	{
		if (m_authorizer)
		{
			LocalOperationContext oc;
			ServiceEnvironmentIFCRef envref = createAuthEnvRef(oc, env);
			m_authorizer->init(envref);

		}
		m_initialized = true;
	}
}

//////////////////////////////////////////////////////////////////////////////
bool
AuthorizerManager::allowReadInstance(
	const ServiceEnvironmentIFCRef& env,
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
	const ServiceEnvironmentIFCRef& env,
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
	const ServiceEnvironmentIFCRef& env,
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
	const ServiceEnvironmentIFCRef& env,
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
	const ServiceEnvironmentIFCRef& env,
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
	const ServiceEnvironmentIFCRef& env,
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
	const ServiceEnvironmentIFCRef& env,
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
	const ServiceEnvironmentIFCRef& env,
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
	const ServiceEnvironmentIFCRef& env,
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

//////////////////////////////////////////////////////////////////////////////
void
AuthorizerManager::shutdown()
{
	m_authorizer.setNull();
}

}	// end of OpenWBEM namespace
