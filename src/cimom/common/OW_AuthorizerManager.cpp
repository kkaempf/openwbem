#include "OW_config.h"
#include "OW_AuthorizerManager.hpp"
#include "OW_OperationContext.hpp"
#include "OW_CIMObjectPath.hpp"

#include <iostream>
using std::cerr;
using std::endl;



#define AUTH_ACTIVE_KEY "_aUtHoRiZeR@aCtIvE@kEy_"
#define DISABLED_KEY "__aUtH@mGr@DiSaBlEd__"

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
void 
AuthorizerManager::turnOff(const ProviderEnvironmentIFCRef& env)
{
	env->getOperationContext().setStringData(DISABLED_KEY, "1");

}
//////////////////////////////////////////////////////////////////////////////
void 
AuthorizerManager::turnOn(const ProviderEnvironmentIFCRef& env)
{
	try
	{
		env->getOperationContext().removeData(DISABLED_KEY);
	}
	catch(...)
	{
		// Ignore?
	}
}
//////////////////////////////////////////////////////////////////////////////
bool
AuthorizerManager::isOn(const ProviderEnvironmentIFCRef& env)
{
	bool cc = true;
	try
	{
		String val = env->getOperationContext().getStringData(
			DISABLED_KEY);
		cc = (val != "1");
	}
	catch (ContextDataNotFoundException& e)
	{
		// Ignore
	}

	return cc;
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
	   || !m_initialized
	   || !isOn(env))
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
	Authorizer2IFC::EDynamicFlag dynamic,
	Authorizer2IFC::EWriteFlag flag)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(env))
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
	   || !m_initialized
	   || !isOn(env))
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
	Authorizer2IFC::EWriteFlag flag)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(env))
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
	const String& ns,
	Authorizer2IFC::EAccessType accessType)
{
	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(env))
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowAccessToNameSpace(env, ns, accessType);
}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
bool 
AuthorizerManager::allowCreateNameSpace(
	const ProviderEnvironmentIFCRef& env,
	const String& ns)
{

	// TEST
	if(!isOn(env))
	{
		cerr << "!!!! AUTHORIZERMANAGER IS OFF !!!" << endl;
	}

	if(!m_initialized)
	{
		cerr << "!!! AUTHORIZERMANAGER IS NOT INITIALIZED !!!" << endl;
	}

	if(!m_authorizer)
	{
		cerr << "!!!! AUTHORIZERMANAGER HAS NO AUTHORIZER !!!" << endl;
	}

	// If the CIMServer is calling into the AuthorizerManager from the
	// loaded authorizer, don't do anything and authorize.
	// If there is no loaded authorizer, authorize everything.
	if(AuthorizerMarker::active(env)
	   || !m_authorizer
	   || !m_initialized
	   || !isOn(env))
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
	   || !m_initialized
       || !isOn(env))
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
	   || !m_initialized
	   || !isOn(env))
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
	   || !m_initialized
	   || !isOn(env))
	{
		return true;
	}

	AuthorizerMarker am(env);
	return m_authorizer->doAllowMethodInvocation(env, ns, path, methodName);
}


}	// wEnd of OpenWBEM namespace
