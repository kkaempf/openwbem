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

#ifndef OW_OOP_CIMOM_HANDLE_CONNECTION_RUNNER_HPP_INCLUDE_GUARD_
#define OW_OOP_CIMOM_HANDLE_CONNECTION_RUNNER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/Runnable.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_BinaryRequestHandler.hpp"
#include "blocxx/IOIFCStreamBuffer.hpp"
#include "blocxx/NonRecursiveMutex.hpp"
#include <istream>
#include <ostream>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OOPCIMOMHandleConnectionRunner : public Runnable
{
public:
	OOPCIMOMHandleConnectionRunner(const UnnamedPipeRef& conn, UInt8 op, UInt8 lockTypeArg, const ProviderEnvironmentIFCRef& provEnv);

	virtual void run();
	bool cancelled();
	virtual void doShutdown();

private:
	UnnamedPipeRef m_conn;
	IntrusiveReference<BinaryRequestHandler> m_binaryRH;
	IOIFCStreamBuffer m_inbuf;
	std::istream m_instr;
	IOIFCStreamBuffer m_outbuf;
	std::ostream m_outstr;
	ProviderEnvironmentIFCRef m_env;
	bool m_cancelled;
	NonRecursiveMutex m_guard;
};


} // end namespace OW_NAMESPACE

#endif


