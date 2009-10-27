/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_PROVIDERAGENTPROVIDERENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_PROVIDERAGENTPROVIDERENVIRONMENT_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ClientCIMOMHandleConnectionPool.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_ProviderAgentEnvironment.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OW_PROVIDERAGENT_API ProviderAgentProviderEnvironment : public ProviderEnvironmentIFC
{
public:
	ProviderAgentProviderEnvironment(
		const ConfigFile::ConfigMap& configMap,
		OperationContext& operationContext,
		const blocxx::String& callbackURL,
		ClientCIMOMHandleConnectionPool& pool,
		ProviderAgentEnvironment::EConnectionCredentialsUsageFlag useConnectionCredentials);

	~ProviderAgentProviderEnvironment();
		// This function returns a regular cimom handle that does access checking and may call providers.
	virtual CIMOMHandleIFCRef getCIMOMHandle() const;
	virtual blocxx::String getConfigItem(const blocxx::String &name, const blocxx::String &defRetVal="") const;
	virtual blocxx::StringArray getMultiConfigItem(const blocxx::String &itemName,
		const blocxx::StringArray& defRetVal, const char* tokenizeSeparator = 0) const;
	// This function returns a cimom handle that directly accesses the repository (CIMServer is bypassed).
	// no providers will be called.  This function should only be called if getCIMOMHandle()
	// is insufficent.
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const;
	// This function returns a reference to the repository.  This function should only
	// be called if getCIMOMHandle() and getRepositoryCIMOMHandle() are insufficient.
	virtual RepositoryIFCRef getRepository() const;
	virtual RepositoryIFCRef getAuthorizingRepository() const;
	virtual blocxx::String getUserName() const;
	virtual OperationContext& getOperationContext();
	virtual ProviderEnvironmentIFCRef clone() const;
private:
	ConfigFile::ConfigMap m_configMap;
	OperationContext& m_operationContext;
	blocxx::String m_callbackURL;
	ClientCIMOMHandleConnectionPool& m_connectionPool;
	mutable blocxx::Array<ClientCIMOMHandleRef>  m_CIMOMHandleRA;
	ProviderAgentEnvironment::EConnectionCredentialsUsageFlag m_useConnectionCredentials;
};

} // end namespace OW_NAMESPACE

#endif // #ifndef OW_PROVIDERAGENTPROVIDERENVIRONMENT_HPP_INCLUDE_GUARD_
