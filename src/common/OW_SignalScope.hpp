/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

#ifndef OW_SIGNALSCOPE_HPP_INCLUDE_GUARD_
#define OW_SIGNALSCOPE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include <signal.h>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

const int OW_SIGSEGV = SIGSEGV;
const int OW_SIGABRT = SIGABRT;
#ifdef SIGBUS // NetWare doesn't have this signal
const int OW_SIGBUS = SIGBUS;
#endif
const int OW_SIGFPE = SIGFPE;

extern "C" {
typedef void (*sighandler_t)(int);
}

class OW_COMMON_API SignalScope
{
public:
	SignalScope( int sig, sighandler_t handler )
			: m_sig( sig ), m_oldHandler( 0 ) 
	{
		m_oldHandler = signal( m_sig, handler );
	}
	~SignalScope()
	{
		signal( m_sig, m_oldHandler );
	}
private:
	SignalScope(const SignalScope&);
	const SignalScope& operator=(const SignalScope&);
	int m_sig;
	sighandler_t m_oldHandler;
};

} // end namespace OW_NAMESPACE

#endif
