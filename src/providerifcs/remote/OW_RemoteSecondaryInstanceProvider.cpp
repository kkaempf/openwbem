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

/**
 * Note that secondary instance providers are a little different, throwing an exception from create, modify or delete is not recommended.
 * Hence we simply eat Exceptions in those functions.
 */

#include "OW_config.h"
#include "OW_RemoteSecondaryInstanceProvider.hpp"
#include "OW_RemoteProviderUtils.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_Logger.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ClientCIMOMHandle.hpp"

namespace OW_NAMESPACE
{

using namespace WBEMFlags;

namespace
{
	const String COMPONENT_NAME("ow.provider.remote.ifc");
}

//////////////////////////////////////////////////////////////////////////////
RemoteSecondaryInstanceProvider::RemoteSecondaryInstanceProvider(const ProviderEnvironmentIFCRef& env, const String& url, const ClientCIMOMHandleConnectionPoolRef& pool,
	bool alwaysSendCredentials, bool useConnectionCredentials)
	: m_pool(pool)
	, m_url(url)
	, m_alwaysSendCredentials(alwaysSendCredentials)
	, m_useConnectionCredentials(useConnectionCredentials)
{
}

//////////////////////////////////////////////////////////////////////////////
RemoteSecondaryInstanceProvider::~RemoteSecondaryInstanceProvider()
{
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
RemoteSecondaryInstanceProvider::modifyInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &modifiedInstance,
	const CIMInstance &previousInstance, EIncludeQualifiersFlag includeQualifiers, const StringArray *propertyList, const CIMClass &theClass)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("RemoteSecondaryInstanceProvider::modifyInstance ns = %1, modifiedInstance = %2", ns, modifiedInstance));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG(lgr, Format("RemoteSecondaryInstanceProvider::modifyInstance got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG(lgr, "RemoteSecondaryInstanceProvider::modifyInstance calling remote WBEM server");

	try
	{
		hdl->modifyInstance(ns, modifiedInstance, includeQualifiers, propertyList);
	}
	catch (CIMException& e)
	{
		OW_LOG_INFO(lgr, Format("RemoteSecondaryInstanceProvider::modifyInstance remote WBEM server threw: %1", e));
		// dont: throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteSecondaryInstanceProvider::modifyInstance failed calling remote WBEM server: %1", e);
		OW_LOG_ERROR(lgr, msg);
		// dont: OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
void
RemoteSecondaryInstanceProvider::deleteInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &cop)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("RemoteSecondaryInstanceProvider::deleteInstance ns = %1, cop = %2", ns, cop));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG(lgr, Format("RemoteSecondaryInstanceProvider::deleteInstance got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG(lgr, "RemoteSecondaryInstanceProvider::deleteInstance calling remote WBEM server");

	try
	{
		hdl->deleteInstance(ns, cop);
	}
	catch (CIMException& e)
	{
		OW_LOG_INFO(lgr, Format("RemoteSecondaryInstanceProvider::deleteInstance remote WBEM server threw: %1", e));
		// dont: throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteSecondaryInstanceProvider::deleteInstance failed calling remote WBEM server: %1", e);
		OW_LOG_ERROR(lgr, msg);
		// dont: OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
void
RemoteSecondaryInstanceProvider::filterInstances(const ProviderEnvironmentIFCRef &env, const String &ns, const String &className, CIMInstanceArray &instances,
	ELocalOnlyFlag localOnly,
	EDeepFlag deep,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList, const CIMClass &requestedClass, const CIMClass &cimClass)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("RemoteSecondaryInstanceProvider::filterInstances ns = %1, className = %2", ns, className));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG(lgr, Format("RemoteSecondaryInstanceProvider::filterInstances got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG(lgr, "RemoteSecondaryInstanceProvider::filterInstances calling remote WBEM server");

	for (size_t i = 0; i < instances.size(); ++i)
	{
		CIMInstance& curInst = instances[i];
		try
		{
			CIMObjectPath instPath(ns, curInst);
			CIMInstance tmp = hdl->getInstance(ns, instPath, localOnly, includeQualifiers, includeClassOrigin, propertyList);
			// now go through the properties and set any that aren't keys or the default
			CIMPropertyArray classProps = cimClass.getAllProperties();
			for (size_t j = 0; j < classProps.size(); ++j)
			{
				CIMProperty& curProp = classProps[i];
				if (curProp.isKey())
				{
					continue;
				}
				CIMProperty p = tmp.getProperty(curProp.getName());
				if (!p)
				{
					continue;
				}
				CIMValue v = p.getValue();
				// if it's not the default value
				if (v != curProp.getValue())
				{
					curInst.setProperty(curProp.getName(), v);
				}
			}
		}
		catch (CIMException& e)
		{
			if (e.getErrNo() == CIMException::NOT_SUPPORTED)
			{
				e.setErrNo(CIMException::FAILED); // providers shouldn't ever throw NOT_SUPPORTED
			}
			OW_LOG_INFO(lgr, Format("RemoteSecondaryInstanceProvider::filterInstances remote WBEM server threw: %1", e));
			// we do want to throw here if something went wrong
			throw;
		}
		catch (const Exception& e)
		{
			String msg = Format("RemoteSecondaryInstanceProvider::filterInstances failed calling remote WBEM server: %1", e);
			OW_LOG_ERROR(lgr, msg);
			// we do want to throw here if something went wrong
			OW_THROWCIMMSG_SUBEX(CIMException::FAILED, msg.c_str(), e);
		}
	}
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
RemoteSecondaryInstanceProvider::createInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &cimInstance)
{
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, Format("RemoteSecondaryInstanceProvider::createInstance ns = %1, cimInstance = %2", ns, cimInstance));
	String lUrl(m_url);
	ClientCIMOMHandleRef hdl = RemoteProviderUtils::getRemoteClientCIMOMHandle(lUrl, m_useConnectionCredentials, env, m_pool, m_alwaysSendCredentials);
	OW_LOG_DEBUG(lgr, Format("RemoteSecondaryInstanceProvider::createInstance got ClientCIMOMHandleRef for url: %1", lUrl));

	ClientCIMOMHandleConnectionPool::HandleReturner returner(hdl, m_pool, lUrl);

	OW_LOG_DEBUG(lgr, "RemoteSecondaryInstanceProvider::createInstance calling remote WBEM server");

	try
	{
		hdl->createInstance(ns, cimInstance);
	}
	catch (CIMException& e)
	{
		OW_LOG_INFO(lgr, Format("RemoteSecondaryInstanceProvider::createInstance remote WBEM server threw: %1", e));
		// dont: throw;
	}
	catch (const Exception& e)
	{
		String msg = Format("RemoteSecondaryInstanceProvider::createInstance failed calling remote WBEM server: %1", e);
		OW_LOG_ERROR(lgr, msg);
		// dont: OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION


} // end namespace OW_NAMESPACE


