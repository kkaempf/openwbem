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

#ifndef OW_POSIX_UNNAMED_PIPE_HPP_
#define OW_POSIX_UNNAMED_PIPE_HPP_
#include "OW_config.h"
#include "OW_UnnamedPipe.hpp"
#include "OW_IntrusiveReference.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OW_COMMON_API PosixUnnamedPipe : public UnnamedPipe
{
public:
	PosixUnnamedPipe(EOpen doOpen=E_OPEN);
	/// If @a fd_in == -1, then you cannot read from this object.
	/// If @a fd_out == -1, then you cannot write to this object
	//
	PosixUnnamedPipe(AutoDescriptor inputfd, AutoDescriptor outputfd);
	virtual ~PosixUnnamedPipe();
	virtual int write(const void* data, int dataLen, ErrorAction errorAsException = E_RETURN_ON_ERROR);
	virtual int read(void* buffer, int bufferLen, ErrorAction errorAsException = E_RETURN_ON_ERROR);
	int getInputHandle() const { return m_fds[0]; }
	int getOutputHandle() const { return m_fds[1]; }
	virtual void open();
	virtual int close();
	virtual bool isOpen() const;
	int closeInputHandle();
	int closeOutputHandle();
	virtual void setBlocking(EBlockingMode outputIsBlocking=E_BLOCKING);
	virtual void setWriteBlocking(EBlockingMode isBlocking=E_BLOCKING);
	virtual void setReadBlocking(EBlockingMode isBlocking=E_BLOCKING);
	virtual Select_t getReadSelectObj() const;
	virtual Select_t getWriteSelectObj() const;
	virtual Descriptor getInputDescriptor() const;
	virtual Descriptor getOutputDescriptor() const;
	virtual void passDescriptor(Descriptor h);
	virtual AutoDescriptor receiveDescriptor();

private:
	int m_fds[2];
#ifdef OW_WIN32
	int m_events[2];
#endif
	EBlockingMode m_blocking[2];
};
typedef IntrusiveReference<PosixUnnamedPipe> PosixUnnamedPipeRef;

} // end namespace OW_NAMESPACE

#endif
