/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_RemoteMethodProvider.hpp"
#include "OW_RemoteProviderUtils.hpp"
#include "OW_CIMValue.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Logger.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ClientCIMOMHandle.hpp"

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.provider.remote.ifc");
}

//////////////////////////////////////////////////////////////////////////////
RemoteMethodProvider::RemoteMethodProvider(const ProviderEnvironmentIFCRef& env, const String& url, const ClientCIMOMHandleConnectionPoolRef& pool,
	bool alwaysSendCredentials, bool useConnectionCredentials)
	: m_pool(pool)
	, m_url(url)
	, m_alwaysSendCredentials(alwaysSendCredentials)
	, m_useConnectionCredentials(useConnectionCredentials)
{
}

//////////////////////////////////////////////////////////////////////////////
RemoteMethodProvider::~RemoteMethodProvider()
{
}

//////////////////////////////////////////////////////////////////////////////
CIMValue RemoteMethodProvider::invokeMethod(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& in,
		CIMParamValueArray& out )
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG3(lgr, Format("RemoteMethodProvider::invokeMethod ns = %1, path = %2, methodName = %3", ns, path, methodName));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG3(lgr, Format("RemoteMethodProvider::invokeMethod got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG3(lgr, "RemoteMethodProvider::invokeMethod calling remote WBEM server");

	CIMValue rval(CIMNULL);
	try
	{
		rval = hdl->invokeMethod(ns, path, methodName, in, out);
	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED from InvokeMethod
		}
		OW_LOG_INFO(lgr, Format("RemoteMethodProvider::invokeMethod remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteMethodProvider::invokeMethod failed calling remote WBEM server: %1", e);
		OW_LOG_ERROR(lgr, msg);
		OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
	}
	return rval;
}



} // end namespace OW_NAMESPACE


