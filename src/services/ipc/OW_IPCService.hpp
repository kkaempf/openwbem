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
#ifndef __OW_IPCSERVICE_HPP__
#define __OW_IPCSERVICE_HPP__

#include "OW_config.h"
#include "OW_Thread.hpp"
#include "OW_IPCServer.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Reference.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_UnnamedPipe.hpp"

//////////////////////////////////////////////////////////////////////////////
class OW_IPCService : public OW_ServiceIFC
{
public:
	OW_IPCService();
	~OW_IPCService();

	virtual void startService();
	virtual void setServiceEnvironment(OW_ServiceEnvironmentIFCRef env);
	virtual void shutdown();
	virtual void doSelected();
	void incConnCount();
	void decConnCount();
	OW_UnnamedPipeRef getUPipe() { return m_upipe; }
	OW_ServiceEnvironmentIFCRef getEnvironment() { return m_env; }

private:
	OW_IPCServer m_server;
	OW_ServiceEnvironmentIFCRef m_env;
	OW_UnnamedPipeRef m_upipe;
	OW_Int32 m_connCount;
	OW_Mutex m_countGuard;
};

//////////////////////////////////////////////////////////////////////////////
class OW_IPCConnectionHandler : public OW_Runnable
{
public:
	OW_IPCConnectionHandler(OW_IPCService* pservice, OW_IPCConnection conn);
	~OW_IPCConnectionHandler();

	virtual void run();

	OW_String authenticate(std::ostream& ostrm, std::istream& istrm,
		const OW_String& oldUser);

private:

	OW_IPCService* m_pservice;
	OW_IPCConnection m_conn;
};

#endif	// __OW_IPCSERVICE_HPP__
