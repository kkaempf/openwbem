/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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

#ifndef OW_REMOTE_PROVIDER_UTILS_HPP_INCLUDE_GUARD_
#define OW_REMOTE_PROVIDER_UTILS_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/IntrusiveReference.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_ClientFwd.hpp"
#include "OW_ProviderFwd.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

namespace RemoteProviderUtils
{

/**
 * Get a ClientCIMOMHandleRef from pool and configure it according to the given parameters.
 *
 * @param remoteUrl Base url for the server. This is an out parameter which may be modified
 *  if useConnectionCredentials == true.
 * @param useConnectionCredentials If true, then modify the url to include the
 *  principal and credentials stored in env->getOperationContext().
 * @param env The provider environment used to get a logger and the OperationContext.
 * @param pool The new connection will be retrieved from pool.
 * @param alwaysSendCredentials If true, assumeBasicAuth() will be called on the
 *  HTTPClient of the new connection.  If the server supports Basic HTTP authentication,
 *  this is helpful to avoid an extra round trip for the client to discover what the
 *  server supports.
 */
ClientCIMOMHandleRef getRemoteClientCIMOMHandle(blocxx::String& remoteUrl,
	bool useConnectionCredentials, const ProviderEnvironmentIFCRef &env,
	const ClientCIMOMHandleConnectionPoolRef& pool, bool alwaysSendCredentials);

} // end namespace RemoteProviderUtils

} // end namespace OW_NAMESPACE

#endif




