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

#include "OW_config.h"
#include "OW_UnnamedPipe.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_String.hpp"

DEFINE_EXCEPTION(UnnamedPipe);

//////////////////////////////////////////////////////////////////////////////
OW_UnnamedPipe::~OW_UnnamedPipe()
{
}

//////////////////////////////////////////////////////////////////////////////
int
OW_UnnamedPipe::writeInt(int value)
{
	return this->write(&value, sizeof(int));
}

//////////////////////////////////////////////////////////////////////////////
int
OW_UnnamedPipe::writeString(const OW_String& strData)
{
	int rc;
	int len = strData.length()+1;

	if((rc = this->writeInt(len)) != -1)
	{
		rc = this->write(strData.c_str(), len);
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_UnnamedPipe::readInt(int* value)
{
	return this->read(value, sizeof(int));
}

//////////////////////////////////////////////////////////////////////////////
int
OW_UnnamedPipe::readString(OW_String& strData)
{
	int len;
	int rc;
	if((rc = this->readInt(&len)) != -1)
	{
		OW_AutoPtrVec<char> p(new char[len+1]);
		if((rc = this->read(p.get(), len)) != -1)
		{	
			strData = OW_String(p.get(), len);
		}
	}

	return rc;
}


//////////////////////////////////////////////////////////////////////////////
OW_String
OW_UnnamedPipe::readAll()
{
	char buf[1024];
	int readbytes;
	OW_String retval;
	do
	{
		readbytes = this->read(buf, sizeof(buf)-1);
		buf[readbytes] = 0; // null-terminate the buffer
		retval += buf;
	} while (readbytes > 0); // keep going until we don't fill up the buffer.
	return retval;
}
