/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
#include "OW_OOPMethodProvider.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_Logger.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_CIMValue.hpp"
#include "OW_DateTime.hpp"
#include "OW_OOPShuttingDownCallback.hpp"
#include "OW_OpenWBEM_OOPMethodProviderCapabilities.hpp"
#include "OW_OOPProtocolIFC.hpp"

namespace OW_NAMESPACE
{

//////////////////////////////////////////////////////////////////////////////
OOPMethodProvider::OOPMethodProvider(const OOPProviderInterface::ProvRegInfo& info,
	const OOPProcessState& processState)
	: OOPProviderBase(info, processState)
{
}

//////////////////////////////////////////////////////////////////////////////
OOPMethodProvider::~OOPMethodProvider()
{
}
	
namespace
{

	class InvokeMethodCallback : public OOPProviderBase::MethodCallback
	{
	public:
		InvokeMethodCallback(
			CIMValue& retval,
			const String& ns,
			const CIMObjectPath& path,
			const String& methodName,
			const CIMParamValueArray& in,
			CIMParamValueArray& out)
			: m_retval(retval)
			, m_ns(ns)
			, m_path(path)
			, m_methodName(methodName)
			, m_in(in)
			, m_out(out)
		{
		}
		
	private:
		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in, 
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			m_retval = protocol->invokeMethod(out, in, timeout, env, m_ns, m_path, m_methodName, m_in, m_out);
		}
	
		CIMValue& m_retval;
		const String& m_ns;
		const CIMObjectPath& m_path;
		const String& m_methodName;
		const CIMParamValueArray& m_in;
		CIMParamValueArray& m_out;
	
	};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
CIMValue
OOPMethodProvider::invokeMethod(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMObjectPath& path,
			const String& methodName,
			const CIMParamValueArray& in,
			CIMParamValueArray& out)
{
	CIMValue retval(CIMNULL);
	InvokeMethodCallback invokeMethodCallback(retval, ns, path, methodName, in, out);
	startProcessAndCallFunction(env, invokeMethodCallback, "OOPMethodProvider::invokeMethod");
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
void
OOPMethodProvider::shuttingDown(const ProviderEnvironmentIFCRef& env)
{
	OOPShuttingDownCallback shuttingDownCallback;
	startProcessAndCallFunction(env, shuttingDownCallback, "OOPMethodProvider::shuttingDown");
}

//////////////////////////////////////////////////////////////////////////////
MethodProviderIFC::ELockType
OOPMethodProvider::getLockTypeForMethod(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName,
	const CIMParamValueArray& in)
{
	UInt16 lt = getProvInfo().methodLockType;
	switch (lt)
	{
		case OpenWBEM::OOPMethodProviderCapabilities::E_LOCKTYPE_NO_LOCK:
			return MethodProviderIFC::E_NO_LOCK;
		case OpenWBEM::OOPMethodProviderCapabilities::E_LOCKTYPE_READ_LOCK:
			return MethodProviderIFC::E_READ_LOCK;
		case OpenWBEM::OOPMethodProviderCapabilities::E_LOCKTYPE_WRITE_LOCK:
			return MethodProviderIFC::E_WRITE_LOCK;
	}
	return MethodProviderIFC::E_WRITE_LOCK;
}

} // end namespace OW_NAMESPACE




