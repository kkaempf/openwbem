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
#ifndef __OW_IPCSERVER_HPP__
#define __OW_IPCSERVER_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_String.hpp"
#include "OW_SelectableIFC.hpp"
#include "OW_Reference.hpp"
#include "OW_Exception.hpp"
#include "OW_IPCConnection.hpp"

class OW_IPCServerImpl;
typedef OW_Reference<OW_IPCServerImpl> OW_IPCServerImplRef;

class OW_IPCServerImpl : public OW_SelectableIFC
{
public:
	virtual ~OW_IPCServerImpl() {}
	virtual void initialize() = 0;
	virtual void cleanup() = 0;
	virtual OW_IPCConnection getClientConnection() = 0;

	static OW_IPCServerImplRef createAPIServer();
};

class OW_IPCServer : public OW_SelectableIFC
{
public:
	OW_IPCServer(OW_Bool hasImpl=false)
		: OW_SelectableIFC()
		, m_impl(hasImpl ? OW_IPCServerImpl::createAPIServer()
			: OW_IPCServerImplRef(0))
	{
	}

	OW_IPCServer(const OW_IPCServer& arg)
		: OW_SelectableIFC()
		, m_impl(arg.m_impl) {}

	OW_IPCServer& operator= (const OW_IPCServer& arg)
	{
		m_impl = arg.m_impl;
		return *this;
	}

	void initialize() { m_impl->initialize(); }
	void cleanup() { m_impl->cleanup(); }
	OW_IPCConnection getClientConnection()
	{
		return m_impl->getClientConnection();
	}

	OW_Select_t getSelectObj() const { return m_impl->getSelectObj(); }

	OW_IPCServerImplRef getImpl() { return m_impl; }

	operator void*() const { return (void*)(m_impl.getPtr()); }

private:
	OW_IPCServerImplRef m_impl;
};

#endif	// __OW_IPCSERVER_HPP__


