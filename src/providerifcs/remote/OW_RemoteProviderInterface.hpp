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

#ifndef OW_REMOTE_PROVIDER_INTERFACE_HPP_INCLUDE_GUARD_
#define OW_REMOTE_PROVIDER_INTERFACE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_ClientCIMOMHandleConnectionPool.hpp"
#include "OW_String.hpp"
#include "OW_SortedVectorMap.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class RemoteProviderInterface : public ProviderIFCBaseIFC
{
public:
	RemoteProviderInterface();
	virtual ~RemoteProviderInterface();

protected:
	virtual const char* getName() const;
	virtual void doInit(const ProviderEnvironmentIFCRef& env,
		InstanceProviderInfoArray& i,
		SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssociatorProviderInfoArray& a,
#endif
		MethodProviderInfoArray& m,
		IndicationProviderInfoArray& ind);
	virtual InstanceProviderIFCRef doGetInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
	virtual SecondaryInstanceProviderIFCRef doGetSecondaryInstanceProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
	virtual MethodProviderIFCRef doGetMethodProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual AssociatorProviderIFCRef doGetAssociatorProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString);
#endif

	ClientCIMOMHandleConnectionPoolRef m_connectionPool;

	struct ProvRegInfo
	{
		String namespaceName;
		String className;
		String url;
		bool alwaysSendCredentials;
		bool useConnectionCredentials;
	};
	enum EProviderTypes
	{
		E_INSTANCE = 1,
		E_SECONDARY_INSTANCE = 2,
		E_ASSOCIATION = 3,
		E_INDICATION = 4,
		E_METHOD = 5
	};

	typedef SortedVectorMap<String, ProvRegInfo> ProvRegMap_t;
	ProvRegMap_t m_instanceProvReg;
	ProvRegMap_t m_secondaryInstanceProvReg;
	ProvRegMap_t m_associatorProvReg;
	ProvRegMap_t m_methodProvReg;
};

}



#endif



