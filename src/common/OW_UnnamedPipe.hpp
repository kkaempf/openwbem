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
#ifndef OW_UNNAMEDPIPE_HPP_
#define OW_UNNAMEDPIPE_HPP_
#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_Exception.hpp"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_Reference.hpp"
#include "OW_IOIFC.hpp"

namespace OpenWBEM
{

class String;
OW_DECLARE_EXCEPTION(UnnamedPipe);
class UnnamedPipe;
typedef Reference<UnnamedPipe> UnnamedPipeRef;
class UnnamedPipe : public SelectableIFC, public IOIFC
{
public:
	virtual ~UnnamedPipe();
	int writeInt(int value);
	int writeString(const String& strData);
	int readInt(int* value);
	int readString(String& strData);
	/**
	 * Read from the pipe and collect into a string, until the other end of the
	 * pipe is closed.
	 * @throws IOException on error
	 */
	String readAll();
	virtual void open() = 0;
	virtual int close() = 0;
	virtual void setOutputBlocking(bool outputIsBlocking=true) = 0;
	enum EOpen
	{
		E_DONT_OPEN,
		E_OPEN
	};
	static UnnamedPipeRef createUnnamedPipe(EOpen doOpen=E_OPEN);
protected:
	UnnamedPipe()
		: SelectableIFC()
	{}
};

} // end namespace OpenWBEM

typedef OpenWBEM::UnnamedPipeException OW_UnnamedPipeException;
typedef OpenWBEM::UnnamedPipeRef OW_UnnamedPipeRef;
typedef OpenWBEM::UnnamedPipe OW_UnnamedPipe;

#endif	// OW_UNNAMEDPIPE_HPP_
