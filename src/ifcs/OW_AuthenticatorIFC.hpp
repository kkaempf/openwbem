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
#ifndef OW_AUTHENTICATORIFC_HPP_
#define OW_AUTHENTICATORIFC_HPP_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Exception.hpp"
#include "OW_MutexLock.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SharedLibraryReference.hpp"

DECLARE_EXCEPTION(Authentication);

class OW_AuthenticatorIFC
{
public:

	OW_AuthenticatorIFC()
		: signature(0xCA1DE8A1)
		, m_mutex() {}

	virtual ~OW_AuthenticatorIFC() {};

	/**
	 * Called when authenticator is loaded
	 * @param env A reference to an OW_ServiceEnvironment for the authenticator
	 *		to use.
	 * Exception is thrown because other classes which derive
	 * from this may need to throw exceptions in init()
	 */
	void init(OW_ServiceEnvironmentIFCRef env) { doInit(env); }

    /**
     * Authenticates a user
     *
	 * @param userName
	 *   The name of the of the user being authenticated
     * @param type
	 *   A string describing the type of authentication
	 * @param info
     *   The authentication credentials
     * @param details
	 *	An out parameter used to provide information as to why the
	 *   authentication failed.
     * @return
     *   True if user is authenticated
     */
	OW_Bool authenticate(OW_String& userName, const OW_String& info,
		OW_String& details)
	{
		OW_MutexLock lock(m_mutex);
		return doAuthenticate(userName, info, details);
	}

	OW_UInt32 signature;

protected:

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
	virtual OW_Bool doAuthenticate(OW_String& userName,
		const OW_String& info, OW_String& details) = 0;

	/**
	 * Called when authenticator is loaded
	 * Exception is thrown because other classes which derive
	 * from this may need to throw exceptions in init()
	 */
	virtual void doInit(OW_ServiceEnvironmentIFCRef) {}

	OW_Mutex m_mutex;
};

typedef OW_SharedLibraryReference<OW_AuthenticatorIFC> OW_AuthenticatorIFCRef;

#define OW_AUTHENTICATOR_FACTORY(derived) \
extern "C" OW_AuthenticatorIFC* \
createAuthenticator() \
{ \
	return new derived; \
} \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
}


#endif
