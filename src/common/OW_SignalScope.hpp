/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_SIGNALSCOPE_HPP_INCLUDE_GUARD_
#define OW_SIGNALSCOPE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include <signal.h>

const int OW_SIGSEGV = SIGSEGV;
const int OW_SIGABRT = SIGABRT;
const int OW_SIGBUS = SIGBUS;
const int OW_SIGFPE = SIGFPE;


extern "C" {
typedef void (*sighandler_t)(int);
}

class OW_SignalScope
{
public:
	OW_SignalScope( int sig, sighandler_t handler )
			: m_sig( sig ), m_oldHandler( 0 ) 
	{
		m_oldHandler = signal( m_sig, handler );
	}

	~OW_SignalScope()
	{
		signal( m_sig, m_oldHandler );
	}

private:
	OW_SignalScope(const OW_SignalScope&);
	const OW_SignalScope& operator=(const OW_SignalScope&);
	int m_sig;
	sighandler_t m_oldHandler;
};

#endif

