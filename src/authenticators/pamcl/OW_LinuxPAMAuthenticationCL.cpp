/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_Exec.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_String.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include <string.h>

namespace OpenWBEM
{

class LinuxPAMAuthenticationCL : public AuthenticatorIFC
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
	virtual bool doAuthenticate(String &userName, const String &info, String &details);
	
	virtual void doInit(ServiceEnvironmentIFCRef env);
	String m_allowedUsers;
	String m_libexecdir;
};
//////////////////////////////////////////////////////////////////////////////
bool
LinuxPAMAuthenticationCL::doAuthenticate(String &userName,
	const String &info, String &details)
{
	if (info.empty())
	{
		details = "You must authenticate to access this resource";
		return false;
	}
	Array<String> allowedUsers = m_allowedUsers.tokenize();
	bool nameFound = false;
	for (size_t i = 0; i < allowedUsers.size(); i++)
	{
		if (allowedUsers[i].equals(userName))
		{
			nameFound = true;
			break;
		}
	}
	if (!nameFound)
	{
		return false;
	}
	String pathToPamAuth = m_libexecdir + "/PAMAuth";
	Array<String> commandLine;
	commandLine.push_back(pathToPamAuth);
	bool rval;
	PopenStreams ps = Exec::safePopen(commandLine,
		userName + " " + info + "\n");
	if (ps.getExitStatus() == 0)
	{
		rval = true;
	}
	else
	{
		rval = false;
	}
	return rval;
}
void
LinuxPAMAuthenticationCL::doInit(ServiceEnvironmentIFCRef env)
{
	m_allowedUsers = env->getConfigItem(ConfigOpts::PAM_ALLOWED_USERS_opt);
	m_libexecdir = env->getConfigItem(ConfigOpts::LIBEXEC_DIR_opt);
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
OW_AUTHENTICATOR_FACTORY(OpenWBEM::LinuxPAMAuthenticationCL);

