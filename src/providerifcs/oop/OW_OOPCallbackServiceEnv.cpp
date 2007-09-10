/*******************************************************************************
* Copyright (C) 2007 Quest Software, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_OOPCallbackServiceEnv.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_Assertion.hpp"

namespace OW_NAMESPACE
{

OOPCallbackServiceEnv::OOPCallbackServiceEnv(UInt8 type, UInt8 lockTypeArg, const ProviderEnvironmentIFCRef& provEnv)
	: m_type(type)
	, m_provEnv(provEnv)
{
	if (m_type == BinarySerialization::LOCKED_CIMOM_HANDLE_REQUEST)
	{
		m_hdl = m_provEnv->getLockedCIMOMHandle(ProviderEnvironmentIFC::EInitialLockFlag(lockTypeArg));
	}
}

String 
OOPCallbackServiceEnv::getConfigItem(const String& item, const String& defRetVal) const
{
	return m_provEnv->getConfigItem(item, defRetVal);
}


CIMOMHandleIFCRef 
OOPCallbackServiceEnv::getCIMOMHandle(OperationContext& context,
	ServiceEnvironmentIFC::EBypassProvidersFlag bypassProviders) const
{
	if (m_type == BinarySerialization::CIMOM_HANDLE_REQUEST)
	{
		return m_provEnv->getCIMOMHandle();
	}
	else if (m_type == BinarySerialization::REPOSITORY_CIMOM_HANDLE_REQUEST)
	{
		return m_provEnv->getRepositoryCIMOMHandle();
	}
	else if (m_type == BinarySerialization::LOCKED_CIMOM_HANDLE_REQUEST)
	{
		return m_hdl;
	}
	else
	{
		OW_ASSERT(0);
	}
	return CIMOMHandleIFCRef();
}

		

} // end namespace OW_NAMESPACE



