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
 */

#include "OW_config.h"
#include "OW_ListenerAuthenticator.hpp"
#include "OW_RandomNumber.hpp"

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
ListenerAuthenticator::ListenerAuthenticator()
	: AuthenticatorIFC(), m_passwdMap()
{}
//////////////////////////////////////////////////////////////////////////////
ListenerAuthenticator::~ListenerAuthenticator() 
{
}
//////////////////////////////////////////////////////////////////////////////
bool
ListenerAuthenticator::doAuthenticate(String& userName,
		const String& info, String& details, OperationContext& context)
{
	bool rval = false;
	if (info.empty()) // no "Authorization" header
	{
		details = "You must authenticate to access this resource";
		return rval;
	}
	String password = info;
	if (m_passwdMap.count(userName) < 1) // user not found in password file
	{
		rval = false;
	}
	else
	{
		String truePass = m_passwdMap[userName];
		rval = password.equals(truePass);
	}
	if (!rval)
	{
		details = "Invalid username or password";
	}
	return rval;
}
///////////////////////////////////////////////////////////////////////////////
void
ListenerAuthenticator::doInit(ServiceEnvironmentIFCRef)
{
}
///////////////////////////////////////////////////////////////////////////////
String
ListenerAuthenticator::getNewCredentials()
{
	String name, pass;
	RandomNumber rn('0', 'z');
	MutexLock lock(m_mutex);
	do
	{
		name.erase();
		for(size_t i = 0; i < 8;)
		{
			int x = rn.getNextNumber();
			if((x > '9' && x < 'A') || (x > 'Z' && x < 'a'))
			{ // only allow alpha-numeric
				continue;
			}
			name += String(static_cast<char>(x));
			++i;
		}
	} while (m_passwdMap.find(name) != m_passwdMap.end());
	for(size_t i = 0; i < 8;)
	{
		int x = rn.getNextNumber();
		if((x > '9' && x < 'A') || (x > 'Z' && x < 'a'))
		{ // only allow alpha-numeric
			continue;
		}
		pass += String(static_cast<char>(x));
		++i;
	}
	m_passwdMap[name] = pass;
	return name + ":" + pass;
}
///////////////////////////////////////////////////////////////////////////////
void
ListenerAuthenticator::removeCredentials(const String& creds)
{
	size_t idx = creds.indexOf(":");
	String name = creds.substring(0, idx);
	Map<String, String>::iterator iter;
	MutexLock lock(m_mutex);
	iter = m_passwdMap.find(name);
	if (iter != m_passwdMap.end())
	{
		m_passwdMap.erase(iter);
	}
}

} // end namespace OpenWBEM

