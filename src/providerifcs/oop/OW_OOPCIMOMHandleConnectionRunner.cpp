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
#include "OW_OOPCIMOMHandleConnectionRunner.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include "OW_OOPCallbackServiceEnv.hpp"
#include "blocxx/Logger.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "blocxx/TempFileStream.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_HTTPUtils.hpp"
#include "blocxx/NonRecursiveMutexLock.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

using namespace blocxx;

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");
}

OOPCIMOMHandleConnectionRunner::OOPCIMOMHandleConnectionRunner(const UnnamedPipeRef& conn, UInt8 op, UInt8 lockTypeArg, const ProviderEnvironmentIFCRef& provEnv)
	: m_conn(conn)
	, m_binaryRH(new BinaryRequestHandler(BinaryRequestHandler::E_SUPPORT_EXPORT_INDICATION))
	, m_inbuf(m_conn.getPtr())
	, m_instr(&m_inbuf)
	, m_outbuf(m_conn.getPtr())
	, m_outstr(&m_outbuf)
	, m_env(provEnv)
	, m_cancelled(false)
{
	m_binaryRH->init(ServiceEnvironmentIFCRef(new OOPCallbackServiceEnv(op, lockTypeArg, provEnv)));
	m_instr.tie(&m_outstr);
}


void
OOPCIMOMHandleConnectionRunner::run()
{
	Logger logger(COMPONENT_NAME);
	while (m_instr.good() && !cancelled())
	{
		UInt8 op;
		BinarySerialization::read(*m_instr.rdbuf(), op);
		if (op == BinarySerialization::BIN_END)
		{
			BLOCXX_LOG_DEBUG3(logger, "CloneCIMOMHandleConnectionRunner::run() received BIN_END request. shutting down cimom handle");
			return;
		}
		else if (op != BinarySerialization::BIN_OK)
		{
			//BLOCXX_LOG_ERROR(logger, "CloneCIMOMHandleConnectionRunner::run() "
			//	"invalid byte received from client");
			return;
		}

		HTTPChunkedIStream operationIstr(m_instr);
		HTTPChunkedOStream operationOstr(m_outstr);
		TempFileStream operationErrstr;
		BLOCXX_LOG_DEBUG3(logger, "CIMOMHandleConnectionRunner::run() calling m_binaryRH->process");
		m_binaryRH->process(&operationIstr, &operationOstr, &operationErrstr, m_env->getOperationContext());
		HTTPUtils::eatEntity(operationIstr);
		if (m_binaryRH->hasError())
		{
			BLOCXX_LOG_DEBUG3(logger, "CIMOMHandleConnectionRunner::run() m_binaryRH->hasError()");
			operationOstr.termOutput(HTTPChunkedOStream::E_DISCARD_LAST_CHUNK);
			operationErrstr.rewind();
			m_outstr << operationErrstr.rdbuf();
		}
		else
		{
			BLOCXX_LOG_DEBUG3(logger, "CIMOMHandleConnectionRunner::run() sending result and BIN_OK");
			operationOstr.termOutput(HTTPChunkedOStream::E_SEND_LAST_CHUNK);
			BinarySerialization::write(m_outbuf, BinarySerialization::BIN_OK);
		}
		if (m_outbuf.pubsync() == -1)
		{
			BLOCXX_LOG_ERROR(logger, "CIMOMHandleConnectionRunner::run() failed to flush output");
		}
	}
	BLOCXX_LOG_DEBUG3(logger, "CIMOMHandleConnectionRunner::run() finished");
}

bool
OOPCIMOMHandleConnectionRunner::cancelled()
{
	NonRecursiveMutexLock lock(m_guard);
	return m_cancelled;
}

void
OOPCIMOMHandleConnectionRunner::doShutdown()
{
	NonRecursiveMutexLock lock(m_guard);
	m_cancelled = true;
}




} // end namespace OW_NAMESPACE




