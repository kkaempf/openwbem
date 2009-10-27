/*******************************************************************************
* Copyright (C) 2007 Quest Software All rights reserved.
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
*  - Neither the name of Quest Software nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software OR THE CONTRIBUTORS
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
#include "OW_OOPQueryProvider.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "blocxx/Format.hpp"
#include "OW_CIMException.hpp"
#include "blocxx/Logger.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ClientCIMOMHandle.hpp"
#include "blocxx/Exec.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include "blocxx/IOIFCStreamBuffer.hpp"
#include "blocxx/IOException.hpp"
#include "blocxx/DateTime.hpp"
#include "OW_OOPShuttingDownCallback.hpp"
#include "OW_OOPProtocolIFC.hpp"

namespace OW_NAMESPACE
{
using namespace blocxx;

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");
}

//////////////////////////////////////////////////////////////////////////////
OOPQueryProvider::OOPQueryProvider(const OOPProviderInterface::ProvRegInfo& info,
	const OOPProcessState& processState)
	: OOPProviderBase(info, processState)
{
}

//////////////////////////////////////////////////////////////////////////////
OOPQueryProvider::~OOPQueryProvider()
{
}

namespace
{
	class QueryInstancesCallback : public OOPProviderBase::MethodCallback
	{
	public:
		QueryInstancesCallback(
			const String& ns,
			const WQLSelectStatement& query,
			CIMInstanceResultHandlerIFC& result,
			const CIMClass& cimClass )
			: m_ns(ns)
			, m_query(query)
			, m_result(result)
			, m_cimClass(cimClass)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in,
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->queryInstances(out, in, timeout, env, m_ns, m_query, m_result, m_cimClass);
		}

	private:
		const String& m_ns;
		const WQLSelectStatement& m_query;
		CIMInstanceResultHandlerIFC& m_result;
		const CIMClass& m_cimClass;

	};
}

//////////////////////////////////////////////////////////////////////////////
void
OOPQueryProvider::queryInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const WQLSelectStatement& query,
		const WQLCompile& compiledWhereClause,
		CIMInstanceResultHandlerIFC& result,
		const CIMClass& cimClass )
{
	// note that the compiledWhereClause isn't sent because it can be recomputed in the provider.
	QueryInstancesCallback queryInstancesCallback(ns, query, result, cimClass);
	startProcessAndCallFunction(env, queryInstancesCallback, "OOPQueryProvider::queryInstances");
}

//////////////////////////////////////////////////////////////////////////////
void
OOPQueryProvider::shuttingDown(const ProviderEnvironmentIFCRef& env)
{
	OOPShuttingDownCallback shuttingDownCallback;
	startProcessAndCallFunction(env, shuttingDownCallback, "OOPQueryProvider::shuttingDown");
}

} // end namespace OW_NAMESPACE



