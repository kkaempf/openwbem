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
#include "OW_AuthenticatorIFC.hpp"
#include "OW_String.hpp"
#include "OW_Map.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Format.hpp"
#include <fstream>
#include <iosfwd>

namespace OpenWBEM
{

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded
class SimpleAuthenticator: public AuthenticatorIFC
{
public:
	SimpleAuthenticator();
	virtual ~SimpleAuthenticator() { };
   /**
    * Called when authenticator is loaded
    *
    */
protected:
	virtual void doInit(ServiceEnvironmentIFCRef);
	bool doAuthenticate(String& userName,
		const String& info, String& details);
private:
	String m_passwordPath;
	Map<String, String> m_passwords;
	/**
    * Loads the password file into a hash map so it only has to
    * take the hit of loading from a file once.
    */
	void loadPasswordFile(ServiceEnvironmentIFCRef env);
	bool doAuthenticate(const String& userName, const String& passwd);
};
SimpleAuthenticator::SimpleAuthenticator()
{
}
//////////////////////////////////////////////////////////////////////////////
void
SimpleAuthenticator::doInit(ServiceEnvironmentIFCRef env)
{
	loadPasswordFile(env);
}
//////////////////////////////////////////////////////////////////////////////
bool
SimpleAuthenticator::doAuthenticate(String& userName,
		const String& info, String& details)
{
	bool rval = false;
	if (info.empty())
	{
		details = "You must authenticate to access this resource";
		return rval;
	}
	if(!(rval = doAuthenticate(userName, info)))
	{
		details = "Invalid username or password";
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
bool
SimpleAuthenticator::doAuthenticate(const String& userName,
	const String& passwd)
{
	bool rval;
	if (m_passwords.count(userName) < 1) // user not found in password file
	{
		rval = false;
	}
	else
	{
		String truePass = m_passwords[userName];
		rval = passwd.equals(truePass);
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
// Private
void
SimpleAuthenticator::loadPasswordFile(ServiceEnvironmentIFCRef env)
{
	// get path to password file from config file
	String passwdFile = env->getConfigItem(
		ConfigOpts::SIMPLE_AUTH_FILE_opt);
	if (passwdFile.empty())
	{
		OW_THROW(AuthenticationException, "No password file given for "
			"simple authorization module");
	}
	std::ifstream infile(passwdFile.c_str(), std::ios::in);
	if (!infile)
	{
		OW_THROW(AuthenticationException, "Cannot open password file");
	}
	// read name/password pairs from file into password map.
	while (infile)
	{
		String line;
		String name;
		String passwd;
		int lineCount = 0;
		line = String::getLine(infile);
		lineCount++;
		line.trim();
		if (line.empty()) // skip blank lines
		{
			continue;
		}
		size_t index = line.indexOf(':');
		if (index != String::npos)
		{
			name = line.substring(0, index);
			passwd = line.substring(index + 1);
		}
		else
		{
			OW_THROW(AuthenticationException, format("Invalid syntax in "
				"%1 at line %2", passwdFile, lineCount).c_str());
		}
		m_passwords[name] = passwd;
	}
}
		
} // end anonymous namespace

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
OW_AUTHENTICATOR_FACTORY(OpenWBEM::SimpleAuthenticator)

