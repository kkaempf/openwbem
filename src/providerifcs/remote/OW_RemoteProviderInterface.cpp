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

namespace OpenWBEM
{

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
		InstanceProviderInfoArray& i,
		SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray& a,
#endif
		MethodProviderInfoArray& m,
		IndicationProviderInfoArray& ind)
{
	unsigned maxConnectionsPerUrl = 5; // TODO: get this from a config item
	m_connectionPool = ClientCIMOMHandleConnectionPoolRef(new ClientCIMOMHandleConnectionPool(maxConnectionsPerUrl));
}
	
//////////////////////////////////////////////////////////////////////////////
InstanceProviderIFCRef
RemoteProviderInterface::doGetInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	return InstanceProviderIFCRef(new RemoteInstanceProvider(env, provIdString, m_connectionPool, false, false));
}

//////////////////////////////////////////////////////////////////////////////
SecondaryInstanceProviderIFCRef
RemoteProviderInterface::doGetSecondaryInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	return SecondaryInstanceProviderIFCRef(new RemoteSecondaryInstanceProvider(env, provIdString, m_connectionPool, false, false));
}

//////////////////////////////////////////////////////////////////////////////
MethodProviderIFCRef
RemoteProviderInterface::doGetMethodProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	return MethodProviderIFCRef(new RemoteMethodProvider(env, provIdString, m_connectionPool, false, false));
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
AssociatorProviderIFCRef
RemoteProviderInterface::doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString)
{
	return AssociatorProviderIFCRef(new RemoteAssociatorProvider(env, provIdString, m_connectionPool, false, false));
}
#endif


} // end namespace OpenWBEM

OW_PROVIDERIFCFACTORY(OpenWBEM::RemoteProviderInterface, remote)

