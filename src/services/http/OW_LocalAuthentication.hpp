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

#ifndef OW_LOCAL_AUTHENTICATION_HPP_INCLUDE_GUARD
#define OW_LOCAL_AUTHENTICATION_HPP_INCLUDE_GUARD
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_DateTime.hpp"

#include <vector>

namespace OpenWBEM
{

class HTTPSvrConnection;

class LocalAuthentication
{
public:
	LocalAuthentication() {}
	~LocalAuthentication();

	bool authenticate(String& userName,
		const String& info, HTTPSvrConnection* htcon);

private:
	struct AuthEntry
	{
		String fileName;
		String cookie;
		String nonce;
		DateTime creationTime;
		String userName;
	};

	String createNewChallenge(const String& uid, const String& userName);
	static String generateNewNonce();
	static void cleanupEntry(const AuthEntry& entry);
	static void generateNewCookieFile(const String& uid, String& cookieFileName, String& cookie);
	void cleanupStaleEntries();

	std::vector<AuthEntry> m_authEntries;

	// unimplemented
	LocalAuthentication(const LocalAuthentication&);
	LocalAuthentication& operator=(const LocalAuthentication&);
};

} // end namespace OpenWBEM

#endif
