/*******************************************************************************
* Copyright (C) 2001-2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
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
 */

#include "OW_config.h"
#include "OW_SimpleAuthorizer2.hpp"
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
#include "OW_CIMOMHandleIFC.hpp"

#include <cstring>

namespace OpenWBEM
{

using namespace WBEMFlags;

namespace
{
const String ACCESS_READ("r");
const String ACCESS_WRITE("w");
const String ACCESS_READWRITE("rw");
}
	
//////////////////////////////////////////////////////////////////////////////
SimpleAuthorizer2::SimpleAuthorizer2()
	: Authorizer2IFC()
{
}
//////////////////////////////////////////////////////////////////////////////
SimpleAuthorizer2::~SimpleAuthorizer2()
{
}

//////////////////////////////////////////////////////////////////////////////
bool
SimpleAuthorizer2::checkAccess(const String& opType, const String& ns,
	const ServiceEnvironmentIFCRef& env, OperationContext& context)
{
	OW_ASSERT(opType == ACCESS_READ || opType == ACCESS_WRITE
		|| opType == ACCESS_READWRITE);

	UserInfo userInfo = context.getUserInfo();
	if (userInfo.getInternal())
	{
		return true;
	}

	CIMOMHandleIFCRef lch = env->getCIMOMHandle(context,
		ServiceEnvironmentIFC::E_DONT_SEND_INDICATIONS, 
		ServiceEnvironmentIFC::E_USE_PROVIDERS);

	LoggerRef lgr = env->getLogger();

	if (!userInfo.getUserName().empty())
	{
		String superUser =
			env->getConfigItem(ConfigOpts::ACL_SUPERUSER_opt);
		if (superUser.equalsIgnoreCase(userInfo.getUserName()))
		{
			lgr->logDebug("User is SuperUser: checkAccess returning.");
			return true;
		}
	}

	String lns(ns);
	while (lns.startsWith('/'))
	{
		lns = lns.substring(1);
	}
	lns.toLowerCase();
	for (;;)
	{
		if (!userInfo.getUserName().empty())
		{
			try
			{
				CIMClass cc = lch->getClass("root/security",
					"OpenWBEM_UserACL", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS,
					E_INCLUDE_CLASS_ORIGIN, NULL);
			}
			catch(CIMException&)
			{
				lgr->logDebug("OpenWBEM_UserACL class non-existent in"
					" /root/security. ACLs disabled");
				return true;
			}
			
			CIMObjectPath cop("OpenWBEM_UserACL");
			cop.setKeyValue("username", CIMValue(userInfo.getUserName()));
			cop.setKeyValue("nspace", CIMValue(lns));
			CIMInstance ci(CIMNULL);
			try
			{
				ci = lch->getInstance("root/security", cop,
					E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS,
					E_INCLUDE_CLASS_ORIGIN, NULL);
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

				capability.toLowerCase();
				if (opType.length() == 1)
				{
					if (capability.indexOf(opType) == String::npos)
					{
					    // Access to namespace denied for user
						OW_THROWCIM(CIMException::ACCESS_DENIED);
					}
				}
				else
				{
					if (!capability.equals("rw") && !capability.equals("wr"))
					{
						// Access to namespace denied for user
						OW_THROWCIM(CIMException::ACCESS_DENIED);
					}
				}

				// Access to namespace granted for user
				return true;
			}
		}

		// use default policy for namespace
		try
		{
			CIMClass cc = lch->getClass("root/security",
				"OpenWBEM_NamespaceACL", E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS,
				E_INCLUDE_CLASS_ORIGIN, NULL);
		}
		catch(CIMException&)
		{
			// OpenWBEM_NamespaceACL class non-existent in /root/security.
			// namespace ACLs disabled
			return true;
		}
		CIMObjectPath cop("OpenWBEM_NamespaceACL");
		cop.setKeyValue("nspace", CIMValue(lns));
		CIMInstance ci(CIMNULL);
		try
		{
			ci = lch->getInstance("root/security", cop, E_NOT_LOCAL_ONLY,
				E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL);
		}
		catch(const CIMException& ce)
		{
			lgr->logDebug(Format("Caught exception: %1 in"
				" AccessMgr::checkAccess. line=%2", ce, __LINE__));
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
			if (opType.length() == 1)
			{
				if (capability.indexOf(opType) == String::npos)
				{
					// Access namespace denied for user
				   	OW_THROWCIM(CIMException::ACCESS_DENIED);
				}
			}
			else
			{
				if (!capability.equals("rw") && !capability.equals("wr"))
				{
					// Access to namespace denied for user
				   	OW_THROWCIM(CIMException::ACCESS_DENIED);
				}
			}

			// Access to namespace granted for user
			return true;
		}
		size_t idx = lns.lastIndexOf('/');
		if (idx == 0 || idx == String::npos)
		{
			break;
		}
		lns = lns.substring(0, idx);
	}

	// Access to namespace denied for user
   	OW_THROWCIM(CIMException::ACCESS_DENIED);
	return false;
}

//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowReadInstance(
	const ServiceEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	const StringArray* clientPropertyList,
	StringArray& authorizedPropertyList,
	OperationContext& context)
{
	return checkAccess(ACCESS_READ, ns, env, context);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowWriteInstance(
	const ServiceEnvironmentIFCRef& env,
	const String& ns, 
	const CIMObjectPath& instanceName, 
	Authorizer2IFC::EDynamicFlag dynamic,
	Authorizer2IFC::EWriteFlag flag,
	OperationContext& context)
{
	return checkAccess(ACCESS_WRITE, ns, env, context);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowReadSchema(
	const ServiceEnvironmentIFCRef& env,
	const String& ns,
	OperationContext& context)
{
	return checkAccess(ACCESS_READ, ns, env, context);
}
#if !defined(OW_DISABLE_SCHEMA_MANIPULATION) || !defined(OW_DISABLE_QUALIFIER_DECLARATION)
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowWriteSchema(
	const ServiceEnvironmentIFCRef& env,
	const String& ns, 
	Authorizer2IFC::EWriteFlag flag,
	OperationContext& context)
{
	return checkAccess(ACCESS_WRITE, ns, env, context);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowAccessToNameSpace(
	const ServiceEnvironmentIFCRef& env,
	const String& ns,
	Authorizer2IFC::EAccessType accessType,
	OperationContext& context)
{
	String actype;
	switch (accessType)
	{
		case Authorizer2IFC::E_READ:
			actype = ACCESS_READ;
			break;
		case Authorizer2IFC::E_WRITE:
			actype = ACCESS_WRITE;
			break;
		default:
			actype = ACCESS_READWRITE;
			break;
	}

	return checkAccess(actype, ns, env, context);
}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowCreateNameSpace(
	const ServiceEnvironmentIFCRef& env,
	const String& ns_,
	OperationContext& context)
{
	return doAllowAccessToNameSpace(env, ns_, Authorizer2IFC::E_WRITE,
		context);
}
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowDeleteNameSpace(
	const ServiceEnvironmentIFCRef& env,
	const String& ns_,
	OperationContext& context)
{
	return doAllowAccessToNameSpace(env, ns_, Authorizer2IFC::E_WRITE,
		context);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowEnumNameSpace(
	const ServiceEnvironmentIFCRef& env,
	OperationContext& context)
{
	return true; // ?
}
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer2::doAllowMethodInvocation(
	const ServiceEnvironmentIFCRef& env, 
	const String& ns, 
	const CIMObjectPath& path, 
	const String& methodName,
	OperationContext& context)
{
	return checkAccess(ACCESS_READWRITE, ns, env, context);
}

} // end namespace OpenWBEM


OW_AUTHORIZER2_FACTORY(OpenWBEM::SimpleAuthorizer2, simple);
