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

#ifndef OW_CLIENTAUTHCBIFC_HPP_
#define OW_CLIENTAUTHCBIFC_HPP_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"

namespace OpenWBEM
{

/**
 * An abstract base class used to provide a callback for retrieving a
 * username and password.  If no username/password is provided in the
 * URL, and the cimom returns a Unauthorized, this will be called
 * so the client can repeat the request with authentication credentials.
 */
class ClientAuthCBIFC : public IntrusiveCountableBase
{
public:
	/**
	 * The callback that is executed to retrieve the username and password.
	 *
	 * @param realm A string describing the server or area within the
	 * 	server for which the client needs access (typically a URL).
	 * @param name A out param String&.  The function should assign a valid
	 * 	user name here. "" is not a valid user name.
	 * @param passwd Similar to "name". The callback assigns the password
	 * 	here. 
	 * @return true means the http client should retry the request with
	 *	the provided name/password.  false means a valid name/password
	 *	were not obtained by the callback, and to http client has no
	 *	reason to retry the request. 
	 */
	virtual bool getCredentials(const String& realm, String& name,
			String& passwd, const String& details) = 0;
	virtual ~ClientAuthCBIFC();
};
typedef IntrusiveReference<ClientAuthCBIFC> ClientAuthCBIFCRef;

} // end namespace OpenWBEM

#endif
