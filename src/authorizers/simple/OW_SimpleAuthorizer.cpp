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

#include <cstring>

#include <iostream>
using std::cerr;
using std::endl;

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
SimpleAuthorizer::SimpleAuthorizer()
	: Authorizer2IFC()
{
}
//////////////////////////////////////////////////////////////////////////////
SimpleAuthorizer::~SimpleAuthorizer()
{
}

//////////////////////////////////////////////////////////////////////////////
bool
SimpleAuthorizer::checkAccess(const String& opType, const String& ns,
	const ProviderEnvironmentIFCRef& env)
{
cerr << "<<< SimpleAuthorizer::checkAccess called..." << endl;


	OW_ASSERT(opType == ACCESS_READ || opType == ACCESS_WRITE
		|| opType == ACCESS_READWRITE);

	UserInfo userInfo = env->getOperationContext().getUserInfo();
	if (userInfo.getInternal())
	{
cerr << "<<< SimpleAuthorizer::checkAccess internal user returning" << endl;

		return true;
	}

	CIMOMHandleIFCRef lch = env->getCIMOMHandle();

	LoggerRef lgr = env->getLogger();

	if (!userInfo.getUserName().empty())
	{
		String superUser =
			env->getConfigItem(ConfigOpts::ACL_SUPERUSER_opt);
		if (superUser.equalsIgnoreCase(userInfo.getUserName()))
		{
cerr << "<<< SimpleAuthorizer::checkAccess is super user. returning" << endl;

			lgr->logDebug("User is SuperUser: checkAccess returning.");
			return true;
		}
	}

	String lns(ns);
	while(lns.startsWith('/'))
	{
		lns = lns.substring(1);
	}
	lns.toLowerCase();
	for(;;)
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
cerr << "<<< SimpleAuthorizer::checkAccess no OpenWBEM_UserACL class. returning" << endl;

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
					if(cv)
					{
						capability = cv.toString();
					}
				}

				capability.toLowerCase();
cerr << "<<< SimpleAuthorizer::checkAccess opType = " << opType << endl;
cerr << "<<< SimpleAuthorizer::checkAccess capability = " << opType << endl;

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
			else
			{
cerr << "<<< SimpleAuthorizer::checkAccess no capability prop" << endl;
			}

			capability.toLowerCase();
cerr << "<<< SimpleAuthorizer::checkAccess opType = " << opType << endl;
cerr << "<<< SimpleAuthorizer::checkAccess capability = " << opType << endl;

			if(opType.length() == 1)
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
cerr << "<<< SimpleAuthorizer::checkAccess returning true - Bottom" << endl;
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
SimpleAuthorizer::doAllowReadInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	const StringArray* clientPropertyList,
	StringArray& authorizedPropertyList)
{
	return checkAccess(ACCESS_READ, ns, env);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
	bool 
SimpleAuthorizer::doAllowWriteInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns, 
	const CIMObjectPath& instanceName, 
	Authorizer2IFC::EDynamicFlag dynamic,
	Authorizer2IFC::EWriteFlag flag)
{
	return checkAccess(ACCESS_WRITE, ns, env);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer::doAllowReadSchema(
	const ProviderEnvironmentIFCRef& env,
	const String& ns)
{
	return checkAccess(ACCESS_READ, ns, env);
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer::doAllowWriteSchema(
	const ProviderEnvironmentIFCRef& env,
	const String& ns, 
	Authorizer2IFC::EWriteFlag flag)
{
	return checkAccess(ACCESS_WRITE, ns, env);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer::doAllowAccessToNameSpace(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	Authorizer2IFC::EAccessType accessType)
{
	String actype;
	switch(accessType)
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

cerr << "<<< SimpleAuthorizer::doAllowAccessToNameSpace calling checkAccess ns"
	<< ns << " with access " << actype << endl;

	return checkAccess(actype, ns, env);
}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer::doAllowCreateNameSpace(
	const ProviderEnvironmentIFCRef& env,
	const String& ns_)
{
cerr << "<<< SimpleAuthorizer::doAllowCreateNameSpace called: ns = " << ns_ << endl;

	return doAllowAccessToNameSpace(env, ns_, Authorizer2IFC::E_WRITE);
}
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer::doAllowDeleteNameSpace(
	const ProviderEnvironmentIFCRef& env,
	const String& ns_)
{
	return doAllowAccessToNameSpace(env, ns_, Authorizer2IFC::E_WRITE);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer::doAllowEnumNameSpace(
		const ProviderEnvironmentIFCRef& env)
{
	return true; // ?
}
//////////////////////////////////////////////////////////////////////////////
bool 
SimpleAuthorizer::doAllowMethodInvocation(
	const ProviderEnvironmentIFCRef& env, 
	const String& ns, 
	const CIMObjectPath path, 
	const String& methodName)
{
	return checkAccess(ACCESS_READWRITE, ns, env);
}

} // end namespace OpenWBEM


OW_AUTHORIZER2_FACTORY(OpenWBEM::SimpleAuthorizer, simple);
