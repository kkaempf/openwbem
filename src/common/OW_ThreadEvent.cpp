/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
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

#include "OW_config.h"
#include "OW_ThreadEvent.hpp"
#include "OW_Assertion.hpp"
#include "OW_Exception.hpp"
#include "OW_ThreadImpl.hpp"

//////////////////////////////////////////////////////////////////////////////	  
OW_ThreadEvent::ThreadEvent::ThreadEvent() :
	m_hdl(0)
{
	if(OW_ThreadEventImpl::createThreadEvent(m_hdl) != 0)
	{
		OW_THROW(OW_Assertion, "OW_ThreadEventImpl::createThreadEvent failed");
	}

	OW_ThreadEventImpl::resetThreadEvent(m_hdl);
}

//////////////////////////////////////////////////////////////////////////////	  
OW_ThreadEvent::ThreadEvent::~ThreadEvent()
{
	OW_ThreadEventImpl::destroyThreadEvent(m_hdl);
}

//////////////////////////////////////////////////////////////////////////////	  
bool
OW_ThreadEvent::ThreadEvent::waitForSignal(OW_UInt32 ms)
{
	return OW_ThreadEventImpl::waitForThreadEventSignal(m_hdl, ms);
}

//////////////////////////////////////////////////////////////////////////////	  
void 
OW_ThreadEvent::ThreadEvent::signal()
{
	OW_ThreadEventImpl::signalThreadEvent(m_hdl);
	OW_ThreadImpl::yield();
}

//////////////////////////////////////////////////////////////////////////////	  
void 
OW_ThreadEvent::ThreadEvent::pulse()
{
	OW_ThreadEventImpl::pulseThreadEvent(m_hdl);
	OW_ThreadImpl::yield();
}

//////////////////////////////////////////////////////////////////////////////	  
void 
OW_ThreadEvent::ThreadEvent::reset()
{
	OW_ThreadEventImpl::resetThreadEvent(m_hdl);
}

//////////////////////////////////////////////////////////////////////////////	  
bool
OW_ThreadEvent::ThreadEvent::isSignaled()
{
	return OW_ThreadEventImpl::isThreadEventSignaled(m_hdl);
}

