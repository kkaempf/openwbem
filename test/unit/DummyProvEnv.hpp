#ifndef DUMMY_PROV_ENV_HPP_INCLUDE_GUARD
#define DUMMY_PROV_ENV_HPP_INCLUDE_GUARD
/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_ProviderEnvironmentIFC.hpp"

struct DummyProvEnv : public OpenWBEM::ProviderEnvironmentIFC
{
	virtual ~DummyProvEnv();
	virtual OpenWBEM::CIMOMHandleIFCRef getCIMOMHandle() const;
	virtual OpenWBEM::CIMOMHandleIFCRef getRepositoryCIMOMHandle() const;
	virtual OpenWBEM::RepositoryIFCRef getRepository() const;
	virtual OpenWBEM::RepositoryIFCRef getAuthorizingRepository() const;
	virtual blocxx::String getConfigItem(
		const blocxx::String &name, const blocxx::String& defRetVal="")
		const;
	virtual blocxx::StringArray getMultiConfigItem(
		const blocxx::String &itemName,
		const blocxx::StringArray& defRetVal,
		const char* tokenizeSeparator = 0) const;
	virtual blocxx::String getUserName() const;
	virtual OpenWBEM::OperationContext& getOperationContext();
	virtual OpenWBEM::ProviderEnvironmentIFCRef clone() const;
};

#endif
