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
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_RemoteAssociatorProvider.hpp"
#include "OW_RemoteProviderUtils.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_ClientCIMOMHandle.hpp"

namespace OW_NAMESPACE
{

using namespace WBEMFlags;

namespace
{
	const String COMPONENT_NAME("ow.provider.remote.ifc");
}

//////////////////////////////////////////////////////////////////////////////
RemoteAssociatorProvider::RemoteAssociatorProvider(const ProviderEnvironmentIFCRef& env, const String& url, const ClientCIMOMHandleConnectionPoolRef& pool,
	bool alwaysSendCredentials, bool useConnectionCredentials)
	: m_pool(pool)
	, m_url(url)
	, m_alwaysSendCredentials(alwaysSendCredentials)
	, m_useConnectionCredentials(useConnectionCredentials)
{
}

//////////////////////////////////////////////////////////////////////////////
RemoteAssociatorProvider::~RemoteAssociatorProvider()
{
}

//////////////////////////////////////////////////////////////////////////////
void
RemoteAssociatorProvider::references(
	const ProviderEnvironmentIFCRef &env,
	CIMInstanceResultHandlerIFC &result,
	const String &ns,
	const CIMObjectPath &objectName,
	const String &resultClass,
	const String &role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("RemoteAssociatorProvider::references ns = %1, objectName = %2, resultClass = %3, role = %4", ns, objectName, resultClass, role));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG(lgr, Format("RemoteAssociatorProvider::references got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG(lgr, "RemoteAssociatorProvider::references calling remote WBEM server");

	try
	{
		hdl->references(ns, objectName, result, resultClass, role,
			includeQualifiers, includeClassOrigin, propertyList);

	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		OW_LOG_INFO(lgr, Format("RemoteAssociatorProvider::references remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteAssociatorProvider::references failed calling remote WBEM server: %1", e);
		OW_LOG_ERROR(lgr, msg);
		OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
	}

}

//////////////////////////////////////////////////////////////////////////////
void
RemoteAssociatorProvider::associators(const ProviderEnvironmentIFCRef &env, CIMInstanceResultHandlerIFC &result, const String &ns, const CIMObjectPath &objectName,
	const String &assocClass, const String &resultClass, const String &role, const String &resultRole, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
	WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("RemoteAssociatorProvider::associators ns = %1, objectName = %2, assocClass = %3, resultClass = %4, role = %5, resultRole = %6",
		ns, objectName, assocClass, resultClass, role, resultRole));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG(lgr, Format("RemoteAssociatorProvider::associators got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG(lgr, "RemoteAssociatorProvider::associators calling remote WBEM server");

	try
	{
		hdl->associators(ns, objectName, result, assocClass, resultClass, role, resultRole,
			includeQualifiers, includeClassOrigin, propertyList);

	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		OW_LOG_INFO(lgr, Format("RemoteAssociatorProvider::associators remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteAssociatorProvider::associators failed calling remote WBEM server: %1", e);
		OW_LOG_ERROR(lgr, msg);
		OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
RemoteAssociatorProvider::associatorNames(const ProviderEnvironmentIFCRef &env, CIMObjectPathResultHandlerIFC &result, const String &ns, const CIMObjectPath &objectName,
	const String &assocClass, const String &resultClass, const String &role, const String &resultRole)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("RemoteAssociatorProvider::associatorNames ns = %1, objectName = %2, assocClass = %3, resultClass = %4, role = %5, resultRole = %6",
		ns, objectName, assocClass, resultClass, role, resultRole));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG(lgr, Format("RemoteAssociatorProvider::associatorNames got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG(lgr, "RemoteAssociatorProvider::associatorNames calling remote WBEM server");

	try
	{
		hdl->associatorNames(ns, objectName, result, assocClass, resultClass, role, resultRole);

	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		OW_LOG_INFO(lgr, Format("RemoteAssociatorProvider::associatorNames remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteAssociatorProvider::associatorNames failed calling remote WBEM server: %1", e);
		OW_LOG_ERROR(lgr, msg);
		OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
RemoteAssociatorProvider::referenceNames(const ProviderEnvironmentIFCRef &env, CIMObjectPathResultHandlerIFC &result, const String &ns, const CIMObjectPath &objectName,
	const String &resultClass, const String &role)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("RemoteAssociatorProvider::referenceNames ns = %1, objectName = %2, resultClass = %3, role = %4", ns, objectName, resultClass, role));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG(lgr, Format("RemoteAssociatorProvider::referenceNames got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG(lgr, "RemoteAssociatorProvider::referenceNames calling remote WBEM server");

	try
	{
		hdl->referenceNames(ns, objectName, result, resultClass, role);

	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		OW_LOG_INFO(lgr, Format("RemoteAssociatorProvider::referenceNames remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteAssociatorProvider::referenceNames failed calling remote WBEM server: %1", e);
		OW_LOG_ERROR(lgr, msg);
		OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
	}

}


} // end namespace OW_NAMESPACE


#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
