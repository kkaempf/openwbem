#include "OW_config.h"
#include "OW_AuthorizerManager.hpp"
#include "OW_OperationContext.hpp"
#include "OW_CIMObjectPath.hpp"

#define AUTH_ACTIVE_KEY "_aUtHoRiZeR@aCtIvE@kEy_"

namespace OpenWBEM
{

namespace
{

//////////////////////////////////////////////////////////////////////////////
struct AuthorizerMarker
{
	const ProviderEnvironmentIFCRef& m_env;
	AuthorizerMarker(const ProviderEnvironmentIFCRef& env)
		: m_env(env)
	{
		m_env->getOperationContext().setStringData(AUTH_ACTIVE_KEY, "1");
	}
	~AuthorizerMarker()
	{
		m_env->getOperationContext().removeData(AUTH_ACTIVE_KEY);
	}
	
	static bool active(const ProviderEnvironmentIFCRef& env)
	{
		bool cc = false;
		try
		{
			String val = env->getOperationContext().getStringData(
				AUTH_ACTIVE_KEY);

			cc = (val == "1");
		}
		catch (ContextDataNotFoundException& e)
		{
			// Ignore
		}
	
		return cc;
	}
};

}	// End of unnamed namespace

//////////////////////////////////////////////////////////////////////////////
AuthorizerManager::AuthorizerManager()
	: m_authorizer()
	, m_initialized(false)
{
}

//////////////////////////////////////////////////////////////////////////////
AuthorizerManager::AuthorizerManager(AuthorizerIFCRef authorizerRef)
	: m_authorizer(authorizerRef)
{
}
//////////////////////////////////////////////////////////////////////////////
AuthorizerManager::~AuthorizerManager()
{
}

//////////////////////////////////////////////////////////////////////////////
void 
AuthorizerManager::init(ProviderEnvironmentIFCRef& env)
{
	if(!m_initialized)
	{
		if(m_authorizer)
		{
			m_authorizer->init(env);
		}
		m_initialized = true;
	}
}

//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowReadInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	const StringArray* clientPropertyList,
	StringArray& authorizedPropertyList)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowReadInstance(env, ns, className,
		clientPropertyList, authorizedPropertyList);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowWriteInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns, 
	const CIMObjectPath& op,
	AuthorizerIFC::EDynamicFlag dynamic,
	AuthorizerIFC::EWriteFlag flag)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowWriteInstance(env, ns, op, dynamic, flag);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowReadSchema(
	const ProviderEnvironmentIFCRef& env,
	const String& ns)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowReadSchema(env, ns);
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowWriteSchema(
	const ProviderEnvironmentIFCRef& env,
	const String& ns, 
	AuthorizerIFC::EWriteFlag flag)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowWriteSchema(env, ns, flag);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowAccessToNameSpace(
	const ProviderEnvironmentIFCRef& env,
	const String& ns)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowAccessToNameSpace(env, ns);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowCreateNameSpace(
	const ProviderEnvironmentIFCRef& env,
	const String& ns)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowCreateNameSpace(env, ns);
}
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowDeleteNameSpace(
	const ProviderEnvironmentIFCRef& env,
	const String& ns)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowDeleteNameSpace(env, ns);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool
AuthorizerManager::allowEnumNameSpace(const ProviderEnvironmentIFCRef& env)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowEnumNameSpace(env);
}

//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowMethodInvocation(
	const ProviderEnvironmentIFCRef& env, 
	const String& ns, 
	const CIMObjectPath& path,
	const String& methodName)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized)
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowMethodInvocation(env, ns, path, methodName);
}


}	// End of OpenWBEM namespace
