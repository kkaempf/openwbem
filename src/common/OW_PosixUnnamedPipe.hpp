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
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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

class OW_PosixUnnamedPipe : public OW_UnnamedPipe
{
public:
	OW_PosixUnnamedPipe(OW_Bool doOpen=true);
	virtual ~OW_PosixUnnamedPipe();
	virtual int write(const void* data, int dataLen, bool errorAsException=false);
	virtual int read(void* buffer, int bufferLen, bool errorAsException=false);
	int getInputHandle() { return m_fds[0]; }
	int getOutputHandle() { return m_fds[1]; }
	virtual void open();
	virtual int close();
	int closeInputHandle();
	int closeOutputHandle();
	void setOutputBlocking(OW_Bool outputIsBlocking=true);
	OW_Select_t getSelectObj() const;

private:
	int m_fds[2];
};

typedef OW_Reference<OW_PosixUnnamedPipe> OW_PosixUnnamedPipeRef;

#endif
