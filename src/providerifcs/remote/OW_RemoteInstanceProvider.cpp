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
#include "OW_RemoteInstanceProvider.hpp"
#include "OW_RemoteProviderUtils.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.remote.ifc");
}

//////////////////////////////////////////////////////////////////////////////
RemoteInstanceProvider::RemoteInstanceProvider(const ProviderEnvironmentIFCRef& env, const String& url, const ClientCIMOMHandleConnectionPoolRef& pool,
	bool alwaysSendCredentials, bool useConnectionCredentials)
	: m_pool(pool)
	, m_url(url)
	, m_alwaysSendCredentials(alwaysSendCredentials)
	, m_useConnectionCredentials(useConnectionCredentials)
{
}

//////////////////////////////////////////////////////////////////////////////
RemoteInstanceProvider::~RemoteInstanceProvider()
{
}
	
//////////////////////////////////////////////////////////////////////////////
void
RemoteInstanceProvider::enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	lgr->logDebug(Format("RemoteInstanceProvider::enumInstanceNames ns = %1, className = %2", ns, className));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	lgr->logDebug(Format("RemoteInstanceProvider::enumInstanceNames got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	lgr->logDebug("RemoteInstanceProvider::enumInstanceNames calling remote WBEM server");

	try
	{
		hdl->enumInstanceNames(ns, className, result);
	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		lgr->logInfo(Format("RemoteInstanceProvider::enumInstanceNames remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteInstanceProvider::enumInstanceNames failed calling remote WBEM server: %1", e);
		lgr->logError(msg);
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
void
RemoteInstanceProvider::enumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EDeepFlag deep,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass )
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	lgr->logDebug(Format("RemoteInstanceProvider::enumInstances ns = %1, className = %2", ns, className));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	lgr->logDebug(Format("RemoteInstanceProvider::enumInstances got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	lgr->logDebug("RemoteInstanceProvider::enumInstances calling remote WBEM server");

	try
	{
		hdl->enumInstances(ns, className, result, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		lgr->logInfo(Format("RemoteInstanceProvider::enumInstances remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteInstanceProvider::enumInstances failed calling remote WBEM server: %1", e);
		lgr->logError(msg);
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
CIMInstance
RemoteInstanceProvider::getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& cimClass )
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	lgr->logDebug(Format("RemoteInstanceProvider::getInstance ns = %1, instanceName = %2", ns, instanceName));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	lgr->logDebug(Format("RemoteInstanceProvider::getInstance got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	lgr->logDebug("RemoteInstanceProvider::getInstance calling remote WBEM server");

	CIMInstance rval(CIMNULL);
	try
	{
		rval = hdl->getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList);
	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		lgr->logInfo(Format("RemoteInstanceProvider::getInstance remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteInstanceProvider::getInstance failed calling remote WBEM server: %1", e);
		lgr->logError(msg);
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
	return rval;
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
RemoteInstanceProvider::createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	lgr->logDebug(Format("RemoteInstanceProvider::createInstance ns = %1", ns));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	lgr->logDebug(Format("RemoteInstanceProvider::createInstance got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	lgr->logDebug("RemoteInstanceProvider::createInstance calling remote WBEM server");

	CIMObjectPath rval(CIMNULL);
	try
	{
		rval = hdl->createInstance(ns, cimInstance);
	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		lgr->logInfo(Format("RemoteInstanceProvider::createInstance remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteInstanceProvider::createInstance failed calling remote WBEM server: %1", e);
		lgr->logError(msg);
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
RemoteInstanceProvider::modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	lgr->logDebug(Format("RemoteInstanceProvider::modifyInstance ns = %1", ns));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	lgr->logDebug(Format("RemoteInstanceProvider::modifyInstance got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	lgr->logDebug("RemoteInstanceProvider::modifyInstance calling remote WBEM server");

	try
	{
		hdl->modifyInstance(ns, modifiedInstance, includeQualifiers, propertyList);
	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		lgr->logInfo(Format("RemoteInstanceProvider::modifyInstance remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteInstanceProvider::modifyInstance failed calling remote WBEM server: %1", e);
		lgr->logError(msg);
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
void
RemoteInstanceProvider::deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	lgr->logDebug(Format("RemoteInstanceProvider::deleteInstance ns = %1, cop = %2", ns, cop));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	lgr->logDebug(Format("RemoteInstanceProvider::deleteInstance got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	lgr->logDebug("RemoteInstanceProvider::deleteInstance calling remote WBEM server");

	try
	{
		hdl->deleteInstance(ns, cop);
	}
	catch (CIMException& e)
	{
		if (e.getErrNo() == CIMException::NOT_SUPPORTED)
		{
			e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
		}
		lgr->logInfo(Format("RemoteInstanceProvider::deleteInstance remote WBEM server threw: %1", e));
		throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteInstanceProvider::deleteInstance failed calling remote WBEM server: %1", e);
		lgr->logError(msg);
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
}

#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION



} // end namespace OpenWBEM



