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
#ifndef OW_UNNAMEDPIPE_HPP_
#define OW_UNNAMEDPIPE_HPP_
#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_Exception.hpp"
#include "OW_Reference.hpp"
#include "OW_IOIFC.hpp"

namespace OpenWBEM
{

// forward decls
class String;

class UnnamedPipe;
typedef Reference<UnnamedPipe> UnnamedPipeRef;

OW_DECLARE_EXCEPTION(UnnamedPipe);

/**
 * Abstract interface for an UnnamedPipe.
 * Blocking is enabled by default.
 */
class UnnamedPipe : public SelectableIFC, public IOIFC
{
public:
	virtual ~UnnamedPipe();
	
	/**
	 * Write an int (native binary representation) to the pipe.  If 
	 *  blocking is enabled, writeInt() will block for up to 
	 *  getWriteTimeout() seconds or forever if getWriteTimeout() == -1.
	 * Exception safety: No-throw
	 * @param value The value to write.
	 * @return The number of bytes written, -1 on error, ETIMEDOUT on timeout.
	 */
	int writeInt(int value);

	/**
	 * Writes a String to the pipe.  This should be read from the other side
	 *  by readString.  If blocking is enabled, writeString() 
	 *  will block for up to getWriteTimeout() seconds or forever if 
	 *  getWriteTimeout() == -1.
	 * Exception safety: No-throw
	 * @param strData The String to write.
	 * @return The number of bytes written, -1 on error, ETIMEDOUT on timeout.
	 */
	int writeString(const String& strData);

	/**
	 * Reads an int (native binary representation) from the pipe.  If 
	 *  blocking is enabled, readInt() will block for
	 *  up to getReadTimeout() seconds or forever if getReadTimeout() == -1.
	 * Exception safety: No-throw
	 * @param value Out parameter where the read int will be stored.
	 * @return The number of bytes read, -1 on error, ETIMEDOUT on timeout.
	 */
	int readInt(int* value);

	/**
	 * Reads a String from the pipe.  If blocking is enabled, 
	 *  readInt() will block for up to getReadTimeout() seconds or forever if
	 *  getReadTimeout() == -1.
	 * Exception safety: Strong for the C++ instance. Fubared for the pipe. 
	 *  If an exception is thrown, the pipe won't be reset to it's pre-call 
	 *  state.
	 * @param value Out parameter where the read int will be stored.
	 * @return The number of bytes read, -1 on error, ETIMEDOUT on timeout.
	 * @throws std::bad_alloc
	 */
	int readString(String& strData);

	/**
	 * Sets the read timeout value.  If blocking is enabled this is the number
	 * of seconds a read operation will block.
	 * Exception safety: No-throw
	 * @param seconds The new read timeout.
	 */
	void setReadTimeout(int seconds) { m_readTimeout = seconds; }
	/**
	 * Gets the read timeout value.  If blocking is enabled this is the number
	 * of seconds a read operation will block.
	 * Exception safety: No-throw
	 * @return The read timeout.
	 */
	int getReadTimeout() { return m_readTimeout; }
	/**
	 * Sets the write timeout value.  If blocking is enabled this is the number
	 * of seconds a write operation will block.
	 * Exception safety: No-throw
	 * @param seconds The new write timeout.
	 */
	void setWriteTimeout(int seconds) { m_writeTimeout = seconds; }
	/**
	 * Gets the write timeout value.  If blocking is enabled this is the number
	 * of seconds a write operation will block.
	 * Exception safety: No-throw
	 * @return seconds The write timeout.
	 */
	int getWriteTimeout() { return m_writeTimeout; }
	/**
	 * Sets the read & write timeout values.  If blocking is enabled this is 
	 * the number of seconds a read or write operation will block.
	 * Exception safety: No-throw
	 * @param seconds The new timeout.
	 */
	void setTimeouts(int seconds) { m_readTimeout = m_writeTimeout = seconds; }

	/**
	 * Read from the pipe and collect into a string, until the other end of the
	 * pipe is closed.
	 * Exception safety: Strong for the C++ instance. Fubared for the pipe. 
	 *  If an exception is thrown, the pipe won't be reset to it's pre-call 
	 *  state.
	 * @throws IOException on error
	 */
	String readAll();

	/**
	 * Open the pipe.
	 */
	virtual void open() = 0;

	/**
	 * Close the pipe.
	 * @return -1 on error, 0 on success.
	 */
	virtual int close() = 0;

	enum EBlockingMode
	{
		E_NONBLOCKING,
		E_BLOCKING
	};
	/**
	 * Set the pipe's blocking mode.
	 * Precondition: The pipe is open.
	 * @param outputIsBlocking new blocking mode.
	 */
	virtual void setBlocking(EBlockingMode outputIsBlocking=E_BLOCKING) = 0;
	virtual void setOutputBlocking(bool outputIsBlocking=true) OW_DEPRECATED = 0;

	
	enum EOpen
	{
		E_DONT_OPEN,
		E_OPEN
	};
	/**
	 * Create an instance of the concrete class that implements the UnnamedPipe 
	 *  interface.
	 * @param doOpen Open the pipe or not.
	 */
	static UnnamedPipeRef createUnnamedPipe(EOpen doOpen=E_OPEN);

protected:
	UnnamedPipe()
		: SelectableIFC()
	{}
	int m_readTimeout;
	int m_writeTimeout;
};

} // end namespace OpenWBEM

typedef OpenWBEM::UnnamedPipeException OW_UnnamedPipeException OW_DEPRECATED;
typedef OpenWBEM::UnnamedPipeRef OW_UnnamedPipeRef OW_DEPRECATED;
typedef OpenWBEM::UnnamedPipe OW_UnnamedPipe OW_DEPRECATED;

#endif	// OW_UNNAMEDPIPE_HPP_
