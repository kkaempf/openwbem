/*******************************************************************************
* Copyright (C) 2006 Quest Software, Inc. All rights reserved.
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

#ifndef OW_CIM_SERVER_PROVIDER_ENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_CIM_SERVER_PROVIDER_ENVIRONMENT_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_LocalOperationContext.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"


namespace OW_NAMESPACE
{

class CIMServerProviderEnvironment : public ProviderEnvironmentIFC
{
public:
	CIMServerProviderEnvironment(OperationContext& context,
		const ServiceEnvironmentIFCRef& env);
	virtual blocxx::String getConfigItem(const blocxx::String &name,
		const blocxx::String& defRetVal="") const;
	virtual blocxx::StringArray getMultiConfigItem(const blocxx::String &itemName,
		const blocxx::StringArray& defRetVal, const char* tokenizeSeparator = 0) const;
	virtual CIMOMHandleIFCRef getCIMOMHandle() const;
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const;
	virtual RepositoryIFCRef getRepository() const;
	virtual RepositoryIFCRef getAuthorizingRepository() const;
	virtual CIMOMHandleIFCRef getLockedCIMOMHandle(EInitialLockFlag initialLock) const;
	virtual blocxx::String getUserName() const;
	virtual OperationContext& getOperationContext();
	virtual ProviderEnvironmentIFCRef clone() const;

private:
	OperationContext& m_context;
	ServiceEnvironmentIFCRef m_env;
};

class ClonedCIMServerProviderEnvironment : public CIMServerProviderEnvironment
{
public:
	ClonedCIMServerProviderEnvironment(
		const ServiceEnvironmentIFCRef& env)
		: CIMServerProviderEnvironment(m_context, env)
	{}
private:
	mutable LocalOperationContext m_context;
};


} // end namespace OW_NAMESPACE

#endif

