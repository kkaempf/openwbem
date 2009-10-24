/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OW_LISTENERAUTHENTICATOR_HPP_INCLUDE_GUARD_
#define OW_LISTENERAUTHENTICATOR_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_AuthenticatorIFC.hpp"
#include "blocxx/Map.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OW_LISTENER_API ListenerAuthenticator : public AuthenticatorIFC
{
public:
	/**
	 * Allocate a new ListenerAuthenticator (auth module
	 * used by a CIM Listener product)
	 */
	ListenerAuthenticator();
	virtual ~ListenerAuthenticator();
	/**
	 * generate a new random name/password in the form
	 * "<name>:<password>"
	 * @return a new set of credentials.
	 */
	String getNewCredentials();
	/**
	 * Remove a set of credentials from the name/password map.
	 * @param creds the credentials to be removed.  They have the
	 * 	form "<name>:<password>"
	 */
	void removeCredentials(const String& creds);
protected:
	virtual bool doAuthenticate(String& userName,
		const String& info, String& details, OperationContext& context);
	/**
	 * Called when authenticator is loaded
	 * Exception is thrown because other classes which derive
	 * from this may need to throw exceptions in init()
	 */
	virtual void doInit(ServiceEnvironmentIFCRef);
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	Map<String, String> m_passwdMap;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

};

} // end namespace OW_NAMESPACE

#endif
