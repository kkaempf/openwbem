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
#include "OW_CIMValue.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
RemoteMethodProvider::RemoteMethodProvider(const ProviderEnvironmentIFCRef& env, const String& url, const ClientCIMOMHandleConnectionPoolRef& pool)
	: m_pool(pool)
	, m_url(url)
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
	LoggerRef lgr = env->getLogger();
	lgr->logDebug(Format("RemoteMethodProvider::invokeMethod ns = %1, path = %2, methodName = %3", ns, path, methodName));
	lgr->logDebug(Format("RemoteMethodProvider::invokeMethod getting ClientCIMOMHandleRef for url: %1", m_url));
	ClientCIMOMHandleRef hdl;
	try
	{
		hdl = m_pool->getConnection(m_url);
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteMethodProvider::invokeMethod failed to get a connection: %1", e);
		lgr->logError(msg);
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, m_url);

	lgr->logDebug("RemoteMethodProvider::invokeMethod calling remote WBEM server");
	CIMValue rval(CIMNULL);

	try
	{
		rval = hdl->invokeMethod(ns, path, methodName, in, out);
	}
	catch (const CIMException& e)
	{
		lgr->logInfo(Format("RemoteMethodProvider::invokeMethod remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteMethodProvider::invokeMethod failed calling remote WBEM server: %1", e);
		lgr->logError(msg);
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}

	lgr->logDebug(Format("RemoteMethodProvider::invokeMethod success. Returning: %1", rval));
	return rval;
}



} // end namespace OpenWBEM


