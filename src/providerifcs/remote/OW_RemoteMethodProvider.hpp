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

#ifndef OW_REMOTE_METHOD_PROVIDER_HPP_INCLUDE_GUARD_
#define OW_REMOTE_METHOD_PROVIDER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_MethodProviderIFC.hpp"
#include "OW_ClientCIMOMHandleConnectionPool.hpp"
#include "blocxx/String.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class RemoteMethodProvider : public MethodProviderIFC
{
public:
	RemoteMethodProvider(const ProviderEnvironmentIFCRef& env, const String& url, const ClientCIMOMHandleConnectionPoolRef& pool,
		bool alwaysSendCredentials, bool useConnectionCredentials);
	virtual ~RemoteMethodProvider();
	virtual CIMValue invokeMethod(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMObjectPath& path,
			const String& methodName,
			const CIMParamValueArray& in,
			CIMParamValueArray& out );
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env)
	{
	}


private:
	ClientCIMOMHandleConnectionPoolRef m_pool;
	String m_url;
	bool m_alwaysSendCredentials;
	bool m_useConnectionCredentials;
};


} // end namespace OW_NAMESPACE

#endif



