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
#ifndef __OW_IPCCONNECTION_HPP__
#define __OW_IPCCONNECTION_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_SelectableIFC.hpp"
#include "OW_String.hpp"
#include "OW_Reference.hpp"
#include "OW_Exception.hpp"
#include "OW_IOIFC.hpp"
#include <iostream>

DEFINE_EXCEPTION(IPCConnection)

class OW_IPCConnectionImpl;
typedef OW_Reference<OW_IPCConnectionImpl> OW_IPCConnectionImplRef;

//////////////////////////////////////////////////////////////////////////////
class OW_IPCConnectionImpl : public OW_SelectableIFC, public OW_IOIFC
{
public:
	virtual ~OW_IPCConnectionImpl() {}

	/**
	 * Connect to the CIMOM running on the local machine. Used on client side
	 * only.
	 * @exception OW_IPCConnectionException
	 */
	virtual void connect() = 0;

	/**
	 * Close this OW_IPCConnectionImpl
	 * @exception OW_IPCConnectionException
	 */
	virtual void disconnect() = 0;

	/**
	 * @return An input stream that will allow reading from the ipc connection.
	 * @exception OW_IPCConnectionException
	 */
	virtual std::istream& getInputStream() = 0;

	/**
	 * @return An output stream that will allow writing to the ipc connection.
	 * @exception OW_IPCConnectionException
	 */
	virtual std::ostream& getOutputStream() = 0;

	/**
	 * @return An input/output stream that will allow reading/writing to the ipc connection.
	 * @exception OW_IPCConnectionException
	 */
	virtual std::iostream& getIOStream() = 0;

	/**
	 * Wait for input data to become available on this ipc connection.
	 * @param timeOutSecs The maximum number of seconds to wait before timing
	 *		out. If this value is negative then wait indefinately
	 * @return 0 on success. -1 on error. -2 if timed out.
	 */
	virtual int waitForInput(int timeOutSecs=-1) = 0;

	/**
	 * Wait until its ok to write to this ipc connection
	 * @param timeOutSecs The maximum number of seconds to wait before timing
	 *		out. If this value is negative then wait indefinately
	 * @return 0 on success. -1 on error. -2 if timed out.
	 */
	virtual int waitForOutput(int timeOutSecs=-1) = 0;

	/**
	 * @return the user name of the client connected to the server.
	 */
	virtual OW_String getUserName() = 0;

	/**
	 * Create a platform specific OW_IPCConnectionImpl object. This method
	 * must be implemented in the platform specific sections of the source tree.
	 * @return An OW_IPCConnectionImplRef that points to a platform specific
	 * implementation of OW_IPCConnectionImpl.
	 * @exception OW_IPCConnectionException
	 */
	static OW_IPCConnectionImplRef createIPCConnection();
};

//////////////////////////////////////////////////////////////////////////////
class OW_IPCConnection : public OW_SelectableIFC, public OW_IOIFC
{
public:

	OW_IPCConnection()
		: OW_SelectableIFC()
		, OW_IOIFC()
		, m_impl(OW_IPCConnectionImpl::createIPCConnection())
		, m_isSuperUser(false) {}

	OW_IPCConnection(const OW_IPCConnection& arg)
		: OW_SelectableIFC()
		, OW_IOIFC()
		, m_impl(arg.m_impl)
		, m_isSuperUser(arg.m_isSuperUser) {}

	OW_IPCConnection(OW_IPCConnectionImplRef impl)
		: OW_SelectableIFC()
		, OW_IOIFC()
		, m_impl(impl)
		, m_isSuperUser(false) {}

	OW_IPCConnection& operator= (const OW_IPCConnection& arg);

	void connect()
	{
		m_impl->connect();
	}

	void disconnect()
	{
		m_impl->disconnect();
	}

	int waitForInput(int timeOutSecs=-1)
	{
		return m_impl->waitForInput(timeOutSecs);
	}

	int waitForOutput(int timeOutSecs=-1)
	{
		return m_impl->waitForOutput(timeOutSecs);
	}

	std::istream& getInputStream() { return m_impl->getInputStream(); }
	std::ostream& getOutputStream() { return m_impl->getOutputStream(); }
	std::iostream& getIOStream() { return m_impl->getIOStream(); }

	OW_String getUserName() { return m_impl->getUserName(); }
	void setIsSuperUser(OW_Bool isSuperUserArg=true)
	{
		m_isSuperUser = isSuperUserArg;
	}

	OW_Bool isSuperUser() { return  m_isSuperUser; }


	OW_IPCConnectionImpl* getImpl() { return m_impl.getPtr(); }
	OW_Select_t getSelectObj() const { return m_impl->getSelectObj(); }

	int write(const void* dataOut, int dataOutLen,
		OW_Bool errorAsException=false);

	int read(void* dataIn, int dataInLen, OW_Bool errorAsException=false);

	operator void*() const { return (void*)(m_impl.getPtr()); }

private:

	OW_IPCConnectionImplRef m_impl;
	OW_Bool m_isSuperUser;
};

#endif	// __OW_IPCCONNECTION_HPP__

