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
 * @author Bart Whiteley
 * @author Dan Nuffer
 * @author Kevin Harris
 */

#include "OW_config.h"
#include "blocxx/Array.hpp"
#include "blocxx/String.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include "OW_Assertion.hpp"

#include <string.h>

#if defined(OW_AIX)
extern "C"
{
#include <usersec.h>
}
#endif /* OW_AIX */

#include "blocxx/Format.hpp"

namespace OW_NAMESPACE
{

  /**
   * This is a very simple wrapper around the AIX authorize routines.
   * If you want details on the authenticate function, visit
   * http://post.doit.wisc.edu/library/aix51/usr/share/man/info/en_US/a_doc_lib/libs/basetrf1/authenticate.htm
   * or google for aix authenticate subroutine.
   */
class AIXAuthentication : public AuthenticatorIFC
{
	/**
	 * Authenticates a user
	 *
	 * @param userName
	 *   The name of the of the user being authenticated
	 * @param info
	 *   The authentication credentials
	 * @param details
	 *	An out parameter used to provide information as to why the
	 *   authentication failed.
	 * @return
	 *   True if user is authenticated
	 */
private:
	virtual bool doAuthenticate(String &userName, const String &info,
		String &details, OperationContext& context);
	virtual void doInit(ServiceEnvironmentIFCRef env);  
	String m_allowedUsers;
};
// See misc_conv.c in libpam for an example.
//////////////////////////////////////////////////////////////////////////////
bool AIXAuthentication::doAuthenticate(String &userName, const String &info,
	String &details, OperationContext& context)
{
	bool successful = false;
#if defined(OW_AIX)
	if (info.empty())
	{
		details = "You must authenticate to access this resource";
		return false;
	}
	Array<String> allowedUsers = m_allowedUsers.tokenize();
	bool nameFound = false;
	for (size_t i = 0; i < allowedUsers.size(); i++)
	{
		if (allowedUsers[i].equals(userName) 
                    || allowedUsers[i].equals("*"))
		{
			nameFound = true;
			break;
		}
	}
	if (!nameFound)
	{
		details = "You must authenticate to access this resource";
		return false;
	}

	// Variables used by authenticate.
	// Result is the return value (zero=success), reenter specifies if the password needs to be reentered (non-zero).
	int result, reenter;
	// FIXME! Allow variable attemps, or possibly find a way to re-request the password.
	int attempts_left = 3;
	char* message = NULL;

	char* pPasswd = strdup(info.c_str());
	char* pUserName = strdup(userName.c_str());
	// Just a test to make sure things won't go horribly wrong in the below loop.
	OW_ASSERT(pPasswd != NULL);
	OW_ASSERT(pUserName != NULL);

	do
	{
		result = ::authenticate(pUserName, pPasswd, &reenter, &message);
		--attempts_left;
	}
	while ( (attempts_left > 0) && reenter );

	free(pUserName);
	free(pPasswd);
	
	// Both should be 0 if the above loop was successful.
	if ( reenter || result )
	{
		if ( message )
		{
			details = message;
		}
		else if ( attempts_left <= 0 )
		{
			details = "Maximum authorization attempts made.";
		}
		else
		{
			details = "Unknown authentication failure.";
		}
	}
	if ( message )
	{
		free(message);
	}

	// FIXME! Potentially check to see if the password has expired, or that kind of stuff.
	
	successful = !reenter && !result;
#else
	details = "Authentication type (AIX) not supported on this platform.";
#endif /* OW_AIX */
	return successful;
}

void AIXAuthentication::doInit(ServiceEnvironmentIFCRef env)
{
	m_allowedUsers = env->getConfigItem(ConfigOpts::PAM_ALLOWED_USERS_opt);
}

} // end namespace OW_NAMESPACE

OW_AUTHENTICATOR_FACTORY(OpenWBEM::AIXAuthentication,aix);


