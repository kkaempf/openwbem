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
#include "OW_RemoteProviderInterface.hpp"
#include "OW_RemoteInstanceProvider.hpp"
#include "OW_RemoteSecondaryInstanceProvider.hpp"
#include "OW_RemoteMethodProvider.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_RemoteAssociatorProvider.hpp"
#endif
#include "OW_ConfigOpts.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_NoSuchPropertyException.hpp"
#include "OW_NULLValueException.hpp"

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.remote.ifc");
}

//////////////////////////////////////////////////////////////////////////////
RemoteProviderInterface::RemoteProviderInterface()
{
}

//////////////////////////////////////////////////////////////////////////////
RemoteProviderInterface::~RemoteProviderInterface()
{
}

//////////////////////////////////////////////////////////////////////////////
const char*
RemoteProviderInterface::getName() const
{
	return "remote";
}
	
//////////////////////////////////////////////////////////////////////////////
void
RemoteProviderInterface::doInit(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& ipia,
		SecondaryInstanceProviderInfoArray& sipia,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray& apia,
#endif
		MethodProviderInfoArray& mpia,
		IndicationProviderInfoArray& indpia)
{
	unsigned maxConnectionsPerUrl = 5;
	try
	{
		maxConnectionsPerUrl = env->getConfigItem(ConfigOpts::REMOTEPROVIFC_MAX_CONNECTIONS_PER_URL_opt, OW_DEFAULT_REMOTEPROVIFC_MAX_CONNECTIONS_PER_URL).toUInt32();
	}
	catch (StringConversionException& e)
	{
	}
	m_connectionPool = ClientCIMOMHandleConnectionPoolRef(new ClientCIMOMHandleConnectionPool(maxConnectionsPerUrl));

	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	String interopNs = env->getConfigItem(ConfigOpts::INTEROP_SCHEMA_NAMESPACE_opt, OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
	CIMInstanceArray registrations;
	try
	{
		registrations = env->getCIMOMHandle()->enumInstancesA(interopNs, "OpenWBEM_RemoteProviderRegistration");
	}
	catch (CIMException& e)
	{
		lgr->logDebug(Format("RemoteProviderInterface::doInit() caught exception (%1) while enumerating instances of "
			"OpenWBEM_RemoteProviderRegistration in namespace %2", e, interopNs));
	}
	lgr->logDebug(Format("RemoteProviderInterface::doInit() found %1 registrations", registrations.size()));
	for (size_t i = 0; i < registrations.size(); ++i)
	{
		CIMInstance& curReg = registrations[i];
		lgr->logDebug(Format("RemoteProviderInterface::doInit() processing registration %1: %2", i, curReg.toString()));
		try
		{
			String instanceID = curReg.getPropertyT("InstanceID").getValueT().toString();
			ProvRegInfo info;
			info.namespaceName = curReg.propertyHasValue("NamespaceName") ? curReg.getPropertyT("NamespaceName").getValueT().toString() : String();
			info.className = curReg.getPropertyT("ClassName").getValueT().toString();
			UInt16Array providerTypes = curReg.getPropertyT("ProviderTypes").getValueT().toUInt16Array();
			info.url = curReg.getPropertyT("Url").getValueT().toString();
			info.alwaysSendCredentials = curReg.getPropertyT("AlwaysSendCredentials").getValueT().toBool();
			info.useConnectionCredentials = curReg.getPropertyT("UseConnectionCredentials").getValueT().toBool();

			if (providerTypes.empty())
			{
				lgr->logError("ProviderTypes property value has no entries");
			}
			for (size_t j = 0; j < providerTypes.size(); ++j)
			{
				switch (providerTypes[j])
				{
					case E_INSTANCE:
						{
							// keep it for ourselves
							m_instanceProvReg[instanceID] = info;
							// give the info back to the provider manager
							InstanceProviderInfo ipi;
							ipi.setProviderName(instanceID);
							StringArray namespaces;
							if (!info.namespaceName.empty())
							{
								namespaces.push_back(info.namespaceName);
							}
							InstanceProviderInfo::ClassInfo classInfo(info.className, namespaces);
							ipi.addInstrumentedClass(classInfo);
							ipia.push_back(ipi);
						}
						break;
					case E_SECONDARY_INSTANCE:
						{
							// keep it for ourselves
							m_secondaryInstanceProvReg[instanceID] = info;
							// give the info back to the provider manager
							SecondaryInstanceProviderInfo sipi;
							sipi.setProviderName(instanceID);
							StringArray namespaces;
							if (!info.namespaceName.empty())
							{
								namespaces.push_back(info.namespaceName);
							}
							SecondaryInstanceProviderInfo::ClassInfo classInfo(info.className, namespaces);
							sipi.addInstrumentedClass(classInfo);
							sipia.push_back(sipi);
						}
						break;
					case E_ASSOCIATION:
						{
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
							// keep it for ourselves
							m_associatorProvReg[instanceID] = info;
							// give the info back to the provider manager
							AssociatorProviderInfo api;
							api.setProviderName(instanceID);
							StringArray namespaces;
							if (!info.namespaceName.empty())
							{
								namespaces.push_back(info.namespaceName);
							}
							AssociatorProviderInfo::ClassInfo classInfo(info.className, namespaces);
							api.addInstrumentedClass(classInfo);
							apia.push_back(api);
#else
							lgr->logError("Remote associator providers not supported");
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
						}
						break;
					case E_INDICATION:
						{
							lgr->logError("Remote indication providers not supported");
						}
						break;
					case E_METHOD:
						{
							// keep it for ourselves
							m_methodProvReg[instanceID] = info;
							// give the info back to the provider manager
							MethodProviderInfo mpi;
							mpi.setProviderName(instanceID);
							StringArray namespaces;
							if (!info.namespaceName.empty())
							{
								namespaces.push_back(info.namespaceName);
							}
							StringArray methods; // leaving this empty means all methods
							MethodProviderInfo::ClassInfo classInfo(info.className, namespaces, methods);
							mpi.addInstrumentedClass(classInfo);
							mpia.push_back(mpi);
						}
						break;
					default:
						lgr->logError(Format("Invalid value (%1) in ProviderTypes", providerTypes[j]));
						break;
				}
			}
		}
		catch (NoSuchPropertyException& e)
		{
			lgr->logError(Format("Registration instance %1 has no property: %2", curReg.toString(), e));
		}
		catch (NULLValueException& e)
		{
			lgr->logError(Format("Registration instance %1 property has null value: %2", curReg.toString(), e));
		}
	}

}
	
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
RemoteProviderInterface::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	ProvRegMap_t::const_iterator iter = m_instanceProvReg.find(provIdString);
	if (iter == m_instanceProvReg.end())
	{
		// it wasn't registered, so it must be the provider qualifier.  In that case provIdString is the url.
		return InstanceProviderIFCRef(new RemoteInstanceProvider(env, provIdString, m_connectionPool, false, false));
	}
	else
	{
		return InstanceProviderIFCRef(new RemoteInstanceProvider(env, iter->second.url, m_connectionPool,
			iter->second.alwaysSendCredentials, iter->second.useConnectionCredentials));
	}
}

//////////////////////////////////////////////////////////////////////////////
SecondaryInstanceProviderIFCRef
RemoteProviderInterface::doGetSecondaryInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	ProvRegMap_t::const_iterator iter = m_secondaryInstanceProvReg.find(provIdString);
	if (iter == m_secondaryInstanceProvReg.end())
	{
		// it wasn't registered, so it must be the provider qualifier.  In that case provIdString is the url.
		return SecondaryInstanceProviderIFCRef(new RemoteSecondaryInstanceProvider(env, provIdString, m_connectionPool, false, false));
	}
	else
	{
		return SecondaryInstanceProviderIFCRef(new RemoteSecondaryInstanceProvider(env, iter->second.url, m_connectionPool,
			iter->second.alwaysSendCredentials, iter->second.useConnectionCredentials));
	}
}

//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
RemoteProviderInterface::doGetMethodProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	ProvRegMap_t::const_iterator iter = m_methodProvReg.find(provIdString);
	if (iter == m_methodProvReg.end())
	{
		// it wasn't registered, so it must be the provider qualifier.  In that case provIdString is the url.
		return MethodProviderIFCRef(new RemoteMethodProvider(env, provIdString, m_connectionPool, false, false));
	}
	else
	{
		return MethodProviderIFCRef(new RemoteMethodProvider(env, iter->second.url, m_connectionPool,
			iter->second.alwaysSendCredentials, iter->second.useConnectionCredentials));
	}
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
RemoteProviderInterface::doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	ProvRegMap_t::const_iterator iter = m_associatorProvReg.find(provIdString);
	if (iter == m_associatorProvReg.end())
	{
		// it wasn't registered, so it must be the provider qualifier.  In that case provIdString is the url.
		return AssociatorProviderIFCRef(new RemoteAssociatorProvider(env, provIdString, m_connectionPool, false, false));
	}
	else
	{
		return AssociatorProviderIFCRef(new RemoteAssociatorProvider(env, iter->second.url, m_connectionPool,
			iter->second.alwaysSendCredentials, iter->second.useConnectionCredentials));
	}
}
#endif


} // end namespace OpenWBEM

OW_PROVIDERIFCFACTORY(OpenWBEM::RemoteProviderInterface, remote)

