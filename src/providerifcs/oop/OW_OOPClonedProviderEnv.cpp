/*******************************************************************************
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_OOPClonedProviderEnv.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_IOException.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_Format.hpp"
#include "OW_LogMessage.hpp"
#include "OW_OperationContext.hpp"
#include "OW_BinaryRequestHandler.hpp"
#include "OW_Assertion.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_OOPProviderBase.hpp"
#include "OW_OOPDataOStreamBuf.hpp"
#include "OW_OOPCIMOMHandleConnectionRunner.hpp"
#include "OW_OOPProtocolCPP1Impl.hpp"

#include <deque>
#include <iosfwd>

namespace OW_NAMESPACE
{

using namespace OOPProtocolCPP1Impl;

OW_DECLARE_EXCEPTION(ClonedOOPProtocolCPP1);
OW_DEFINE_EXCEPTION(ClonedOOPProtocolCPP1);

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");
}	// End of unnamed namespace

//////////////////////////////////////////////////////////////////////////////
OOPClonedProviderEnv::OOPClonedProviderEnv(
	OOPProviderBase* pprov, 
	const UnnamedPipeRef& conn,
	const ProviderEnvironmentIFCRef& provEnv)
	: Runnable()
	, m_pprov(pprov)
	, m_conn(conn)
	, m_env(provEnv)
{
	m_conn->setTimeouts(Timeout::relative(0));
	m_conn->setWriteBlocking(UnnamedPipe::E_NONBLOCKING);
}

//////////////////////////////////////////////////////////////////////////////
void 
OOPClonedProviderEnv::run()
{
	Logger logger(COMPONENT_NAME);
	const int MAX_CALLBACK_THREADS = 10;
	ThreadPool threadPool(ThreadPool::DYNAMIC_SIZE_NO_QUEUE, 10, 10, logger, "ClonedOOPProtocolCPP1");
	ShutdownThreadPool stp(threadPool);
	SelectEngine selectEngine;
	Array<unsigned char> inputBuf;
	std::deque<OutputEntry> outputEntries;

	bool finishedSuccessfully = false;
	ShutdownThreadPool shutdownThreadPool(threadPool);

	NoResultHandler dummyResultHandler;
	SelectableCallbackIFCRef callback(new OOPSelectableCallback(
		inputBuf, outputEntries, m_conn, m_conn, m_env, dummyResultHandler, selectEngine,
		finishedSuccessfully, threadPool, m_pprov));

	selectEngine.addSelectableObject(m_conn->getReadSelectObj(), callback, 
		SelectableCallbackIFC::E_READ_EVENT);

	OW_LOG_DEBUG3(logger, "OOPClonedProviderEnv::run() about to run the select engine");
	try
	{
		selectEngine.go(Timeout::infinite);
	}
	catch (Exception& e)
	{
		OW_LOG_DEBUG(logger, Format("OOPClonedProviderEnv::run() caught select exception: %1", e));
	}
	catch(...)
	{
		OW_LOG_DEBUG(logger, "OOPClonedProviderEnv::run() caught UNKNOWN exception");
	}

	if (!finishedSuccessfully)
	{
		OW_LOG_DEBUG3(logger, "ClonedProvEnv::run pipe closed without sending a BIN_END");
	}

	OW_LOG_DEBUG3(logger, "OOPClonedProviderEnv::run returning");
}

//////////////////////////////////////////////////////////////////////////////
void
OOPClonedProviderEnv::doShutdown()
{
}

} // end namespace OW_NAMESPACE







