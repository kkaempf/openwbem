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

#include "OW_config.h"
#include "OW_RemoteProviderUtils.hpp"
#include "OW_URL.hpp"
#include "OW_OperationContext.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ClientCIMOMHandleConnectionPool.hpp"
#include "blocxx/Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_HTTPClient.hpp"
#include "blocxx/Assertion.hpp"
#include "blocxx/Logger.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ClientCIMOMHandle.hpp"

namespace OW_NAMESPACE
{

using namespace blocxx;

namespace RemoteProviderUtils
{

namespace
{
	const String COMPONENT_NAME("ow.provider.remote.ifc");
}

/////////////////////////////////////////////////////////////////////////////
ClientCIMOMHandleRef getRemoteClientCIMOMHandle(String& remoteUrl,
	bool useConnectionCredentials, const ProviderEnvironmentIFCRef &env,
	const ClientCIMOMHandleConnectionPoolRef& pool, bool alwaysSendCredentials)
{
	ClientCIMOMHandleRef hdl;
	try
	{
		if (useConnectionCredentials)
		{
			URL url(remoteUrl);
			try
			{
				url.principal = env->getOperationContext().getStringData(OperationContext::USER_NAME);
				url.credential = env->getOperationContext().getStringData("remote_provider.user_credentials");
			}
			catch (ContextDataNotFoundException& e)
			{
			}
			remoteUrl = url.toString();
		}
		hdl = pool->getConnection(remoteUrl);
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteProviderUtils::getRemoteClientCIMOMHandle() failed to get a connection: %1", e);
		BLOCXX_LOG_ERROR(Logger(COMPONENT_NAME), msg);
		OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
	}

	if (alwaysSendCredentials)
	{
		CIMProtocolIFCRef tmp = hdl->getWBEMProtocolHandler();
		if (tmp)
		{
			IntrusiveReference<HTTPClient> httpClient = tmp.cast_to<HTTPClient>();
			if (httpClient)
			{
				httpClient->assumeBasicAuth();
			}
		}
	}
	BLOCXX_ASSERT(hdl);
	return hdl;
}


} // end namespace RemoteProviderUtils

} // end namespace OW_NAMESPACE






