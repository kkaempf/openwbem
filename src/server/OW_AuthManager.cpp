/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_AuthManager.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_SafeLibCreate.hpp"

///////////////////////////////////////////////////////////////////////////////
OW_AuthManager::OW_AuthManager()
	: m_authenticator()
{
}

///////////////////////////////////////////////////////////////////////////////
OW_AuthManager::~OW_AuthManager()
{
}

///////////////////////////////////////////////////////////////////////////////
void
OW_AuthManager::init(OW_ServiceEnvironmentIFCRef env)
{
	m_authenticator.setNull();

	OW_String authLib = env->getConfigItem(
		OW_ConfigOpts::AUTH_MOD_opt);

	if(authLib.length() == 0)
	{
		authLib = DEFAULT_AUTH_MOD;
	}

	env->getLogger()->logCustInfo(format("Authentication Manager: Loading"
		" authentication module %1", authLib));

	m_authenticator =
		OW_SafeLibCreate<OW_AuthenticatorIFC>::loadAndCreateObject(authLib,
			"createAuthenticator", env->getLogger());

	if(m_authenticator)
	{
		try
		{
			m_authenticator->init(env);

			env->getLogger()->logCustInfo(format("Authentication module %1"
				" is now being used for authentication to the CIMOM",
				authLib));
		}
		catch(OW_Exception& e)
		{
			env->getLogger()->logError(format("Authentication Module %1 failed"
				" to initialize: %2 - %3"
				" [No Authentication Mechanism Available!]", authLib, e.type(),
				e.getMessage()));

			m_authenticator.setNull();
		}
		catch(...)
		{
			env->getLogger()->logError(format("Authentication Module %1 failed"
				" to initialize: Unknown Exception Caught"
				" [No Authentication Mechanism Available!]", authLib));

			m_authenticator.setNull();
		}
	}
	else
	{
		env->getLogger()->logError(format("Authentication Module %1 failed"
			" to produce authentication module"
			" [No Authentication Mechanism Available!]", authLib));
	}
}

///////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_AuthManager::authenticate(OW_String& userName,
	const OW_String& info, OW_String& details)
{
	if(m_authenticator)
	{
		return m_authenticator->authenticate(userName, info, details);
	}

	details = "CIMOM has no available authentication mechanism";
	return false;
}

