/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 * @author Jon Carey
 */

#include "OW_config.h"
#include "blocxx/Exec.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/String.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include "blocxx/Secure.hpp"
#include "OW_PrivilegeManager.hpp"
#if defined(OW_HAVE_SYS_WAIT_H) && defined(OW_WIFEXITED_NEEDS_WAIT_H)
#include <sys/wait.h>
#endif

#include <string.h>

#include <iostream>
using std::cout;
using std::endl;

namespace OW_NAMESPACE
{

class LinuxPAMAuthenticationCL : public AuthenticatorIFC
{
public:
	LinuxPAMAuthenticationCL()
		: AuthenticatorIFC()
		, m_allowedUsers()
		, m_allUsersAllowed(false)
		, m_libexecdir()
		, m_authProc(0)
	{
	}

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
	virtual bool doAuthenticate(String &userName, const String &info, String &details, OperationContext& context);
	virtual void doInit(ServiceEnvironmentIFCRef env);
	bool initAuthProc(String& details);
	ProcessRef getAuthProc(String& details);
	StringArray m_allowedUsers;
	bool m_allUsersAllowed;
	String m_libexecdir;
	ProcessRef m_authProc;
};

//////////////////////////////////////////////////////////////////////////////
ProcessRef
LinuxPAMAuthenticationCL::getAuthProc(String& details)
{
	if (m_authProc)
	{
		Process::Status status = m_authProc->processStatus();
		if (status.running())
			return m_authProc;
	}
	initAuthProc(details);
	return m_authProc;
}

//////////////////////////////////////////////////////////////////////////////
bool
LinuxPAMAuthenticationCL::initAuthProc(String& details)
{
	m_authProc = 0;
	PrivilegeManager privmgr = PrivilegeManager::getPrivilegeManager();
	if (privmgr.isNull())
	{
		details = "Authenticator unable to get privilege manager";
		return false;
	}

	String pathToPamAuth = m_libexecdir + "/OW_PAMAuth";
	try
	{
		StringArray argv(1, pathToPamAuth);
		m_authProc = privmgr.userSpawn(pathToPamAuth, argv,
			Secure::minimalEnvironment(), "root");
		if (!m_authProc)
		{
			details = "Failed to start authentication process";
			return false;
		}

		Process::Status status = m_authProc->processStatus();
		if (!status.running())
		{
			details = "Unexpected termination of authentication process";
			return false;
		}
	}
	catch(Exception& e)
	{
		details = "Exception caught executing authentication process: ";
		details += e.getMessage();
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool
LinuxPAMAuthenticationCL::doAuthenticate(String &userName,
	const String &info, String &details, OperationContext& context)
{
	if (info.empty())
	{
		details = "You must authenticate to access this resource";
		return false;
	}

	bool nameFound = m_allUsersAllowed;
	for (size_t i = 0; i < m_allowedUsers.size() && !nameFound; i++)
	{
		if (m_allowedUsers[i].equals(userName))
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

	try
	{
		ProcessRef pproc = getAuthProc(details);
		if (!pproc)
		{
			return false;
		}

		UnnamedPipeRef authin = pproc->in();
		UnnamedPipeRef authout = pproc->out();

		if (authin->writeString(userName) == -1)
		{
			details = "Failed to communicate with authentication process";
			return false;
		}

		if (authin->writeString(info) == -1)
		{
			details = "Failed to communicate with authentication process";
			return false;
		}

		int authcc;
		if (authout->readInt(&authcc) == -1)
		{
			details = "Failed to get response from authentication process";
			return false;
		}

		return (authcc == 1);
	}
	catch(Exception& e)
	{
		details = "Exception caught while authenticating: ";
		details += e.getMessage();
	}

	return false;
}

void
LinuxPAMAuthenticationCL::doInit(ServiceEnvironmentIFCRef env)
{
	String allowedUsersLine = env->getConfigItem(ConfigOpts::PAM_ALLOWED_USERS_opt);
	m_allowedUsers = allowedUsersLine.tokenize();
	m_libexecdir = env->getConfigItem(ConfigOpts::LIBEXECDIR_opt, OW_DEFAULT_OWLIBEXECDIR);

	m_allUsersAllowed = false;
	for (StringArray::size_type i = 0; i < m_allowedUsers.size(); i++)
	{
		if (m_allowedUsers[i].equals("*"))
		{
			m_allUsersAllowed = true;
			break;
		}
	}
}

} // end namespace OW_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
OW_AUTHENTICATOR_FACTORY(OpenWBEM::LinuxPAMAuthenticationCL,pamcl);

