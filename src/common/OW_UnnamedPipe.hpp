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

#ifndef OW_UNNAMEDPIPE_HPP_
#define OW_UNNAMEDPIPE_HPP_

#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_Exception.hpp"
#include "OW_Bool.hpp"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_Reference.hpp"
#include "OW_IOIFC.hpp"

class OW_String;


DECLARE_EXCEPTION(UnnamedPipe);

class OW_UnnamedPipe;
typedef OW_Reference<OW_UnnamedPipe> OW_UnnamedPipeRef;

class OW_UnnamedPipe : public OW_SelectableIFC, public OW_IOIFC
{
public:
	virtual ~OW_UnnamedPipe();

	int writeInt(int value);
	int writeString(const OW_String& strData);
	int readInt(int* value);
	int readString(OW_String& strData);
	OW_String readAll();

	virtual void open() = 0;
	virtual int close() = 0;
	virtual void setOutputBlocking(OW_Bool outputIsBlocking=true) = 0;

	static OW_UnnamedPipeRef createUnnamedPipe(OW_Bool doOpen=true);

protected:
	OW_UnnamedPipe()
		: OW_SelectableIFC()
	{}
};

#endif	// OW_UNNAMEDPIPE_HPP_



