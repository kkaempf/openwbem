/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_POSIX_UNNAMED_PIPE_HPP_
#define OW_POSIX_UNNAMED_PIPE_HPP_
#include "OW_config.h"
#include "OW_UnnamedPipe.hpp"
#include "OW_Reference.hpp"

namespace OpenWBEM
{

class PosixUnnamedPipe : public UnnamedPipe
{
public:
	PosixUnnamedPipe(EOpen doOpen=E_OPEN);
	virtual ~PosixUnnamedPipe();
	virtual int write(const void* data, int dataLen, bool errorAsException=false);
	virtual int read(void* buffer, int bufferLen, bool errorAsException=false);
	int getInputHandle() { return m_fds[0]; }
	int getOutputHandle() { return m_fds[1]; }
	virtual void open();
	virtual int close();
	int closeInputHandle();
	int closeOutputHandle();
	virtual void setBlocking(bool outputIsBlocking);
	virtual void setOutputBlocking(bool outputIsBlocking=true) OW_DEPRECATED;
	Select_t getSelectObj() const;
private:
	int m_fds[2];
	bool m_blocking;
};
typedef Reference<PosixUnnamedPipe> PosixUnnamedPipeRef;

} // end namespace OpenWBEM

typedef OpenWBEM::PosixUnnamedPipe OW_PosixUnnamedPipe OW_DEPRECATED;

#endif
