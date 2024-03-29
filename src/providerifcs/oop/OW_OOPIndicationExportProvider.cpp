/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
#include "OW_OOPIndicationExportProvider.hpp"
#include "OW_OOPShuttingDownCallback.hpp"
#include "OW_OOPProtocolIFC.hpp"

namespace OW_NAMESPACE
{

using namespace blocxx;

OOPIndicationExportProvider::OOPIndicationExportProvider(const OOPProviderInterface::ProvRegInfo& info,
	const OOPProcessState& processState)
	: OOPProviderBase(info, processState)
{

}

OOPIndicationExportProvider::~OOPIndicationExportProvider()
{

}

namespace
{
	class ExportIndicationCallback : public OOPProviderBase::MethodCallback
	{
	public:
		ExportIndicationCallback(
			const String& ns,
			const CIMInstance& indHandlerInst, const CIMInstance& indicationInst)
			: m_ns(ns)
			, m_indHandlerInst(indHandlerInst)
			, m_indicationInst(indicationInst)
		{
		}

	private:
		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, const UnnamedPipeRef& in,
			const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->exportIndication(out, in, timeout, env, m_ns, m_indHandlerInst, m_indicationInst);
		}

		const String& m_ns;
		const CIMInstance& m_indHandlerInst;
		const CIMInstance& m_indicationInst;
	};
}

StringArray
OOPIndicationExportProvider::getHandlerClassNames()
{
	return getProvInfo().indicationExportHandlerClassNames;
}

void
OOPIndicationExportProvider::exportIndication(const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance& indHandlerInst, const CIMInstance& indicationInst)
{
	ExportIndicationCallback exportIndicationCallback(ns, indHandlerInst, indicationInst);
	startProcessAndCallFunction(env, exportIndicationCallback, "OOPIndicationExportProvider::exportIndication");
}

void
OOPIndicationExportProvider::doShutdown()
{
	/// @todo  Figure out if doing anything here is necessary.
}

void
OOPIndicationExportProvider::doCooperativeCancel()
{
}

void
OOPIndicationExportProvider::doDefinitiveCancel()
{
}

void
OOPIndicationExportProvider::shuttingDown(const ProviderEnvironmentIFCRef& env)
{
	OOPShuttingDownCallback shuttingDownCallback;
	startProcessAndCallFunction(env, shuttingDownCallback, "OOPIndicationExportProvider::shuttingDown");
}

} // end namespace OW_NAMESPACE









