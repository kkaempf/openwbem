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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_UnnamedPipe.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_String.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_PosixUnnamedPipe.hpp"

#include <unistd.h> // for dup()
#include <sys/types.h>
#include <sys/socket.h> // for socketpair()

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(UnnamedPipe);

//////////////////////////////////////////////////////////////////////////////
UnnamedPipe::~UnnamedPipe()
{
}
//////////////////////////////////////////////////////////////////////////////
int
UnnamedPipe::writeInt(int value)
{
	return this->write(&value, sizeof(int));
}
//////////////////////////////////////////////////////////////////////////////
int
UnnamedPipe::writeString(const String& strData)
{
	int rc;
	int len = static_cast<int>(strData.length()+1);
	if ((rc = this->writeInt(len)) != -1)
	{
		rc = this->write(strData.c_str(), len);
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
UnnamedPipe::readInt(int* value)
{
	return this->read(value, sizeof(int));
}
//////////////////////////////////////////////////////////////////////////////
int
UnnamedPipe::readString(String& strData)
{
	int len;
	int rc;
	
	if ((rc = this->readInt(&len)) != -1)
	{
		AutoPtrVec<char> p(new char[len+1]);

		// writeString() writes the '\0' terminator, so we don't worry about it here.
		if ((rc = this->read(p.get(), len)) != -1)
		{	
			strData = String(String::E_TAKE_OWNERSHIP, p.release(), len);
		}
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
String
UnnamedPipe::readAll()
{
	char buf[1024];
	int readbytes;
	StringBuffer retval;
	do
	{
		readbytes = this->read(buf, sizeof(buf)-1, E_THROW_ON_ERROR);
		buf[readbytes] = 0; // null-terminate the buffer
		retval += buf;
	} while (readbytes > 0); // keep going until we don't fill up the buffer.
	return retval.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
UnnamedPipeRef
UnnamedPipe::createUnnamedPipe(EOpen doOpen)
{
	return UnnamedPipeRef(new PosixUnnamedPipe(doOpen));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
UnnamedPipeRef
UnnamedPipe::createStdin()
{
	AutoDescriptor duped(::dup(0));
	if (duped.get() == -1)
	{
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "UnnamedPipe::createStdin(): dup():");
	}
	return UnnamedPipeRef(new PosixUnnamedPipe(duped, AutoDescriptor()));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
UnnamedPipeRef
UnnamedPipe::createStdout()
{
	AutoDescriptor duped(::dup(1));
	if (duped.get() == -1)
	{
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "UnnamedPipe::createStdout(): dup():");
	}
	return UnnamedPipeRef(new PosixUnnamedPipe(AutoDescriptor(), duped));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
UnnamedPipeRef
UnnamedPipe::createStdinStdout()
{
	AutoDescriptor dupedIn(::dup(0));
	if (dupedIn.get() == -1)
	{
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "UnnamedPipe::createStdout(): dup():");
	}
	AutoDescriptor dupedOut(::dup(1));
	if (dupedOut.get() == -1)
	{
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "UnnamedPipe::createStdout(): dup():");
	}
	return UnnamedPipeRef(new PosixUnnamedPipe(dupedIn, dupedOut));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
UnnamedPipeRef
UnnamedPipe::createStderr()
{
	AutoDescriptor duped(::dup(2));
	if (duped.get() == -1)
	{
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "UnnamedPipe::createStderr(): dup():");
	}
	return UnnamedPipeRef(new PosixUnnamedPipe(AutoDescriptor(), duped));
}

//////////////////////////////////////////////////////////////////////////////
// static 
UnnamedPipeRef 
UnnamedPipe::createUnnamedPipeFromDescriptor(AutoDescriptor inputAndOutput)
{
	AutoDescriptor duped(::dup(inputAndOutput.get()));
	if (duped.get() == -1)
	{
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "UnnamedPipe::createUnnamedPipeFromDescriptor(): dup():");
	}
	return UnnamedPipeRef(new PosixUnnamedPipe(inputAndOutput, duped));
}

//////////////////////////////////////////////////////////////////////////////
// static 
UnnamedPipeRef 
UnnamedPipe::createUnnamedPipeFromDescriptor(AutoDescriptor input, AutoDescriptor output)
{
	return UnnamedPipeRef(new PosixUnnamedPipe(input, output));
}

//////////////////////////////////////////////////////////////////////////////
// static 
void
UnnamedPipe::createConnectedPipes(UnnamedPipeRef& first, UnnamedPipeRef& second)
{
	int fds[2];
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1)
	{
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "PosixUnamedPipe::open(): soketpair()");
	}
	first = createUnnamedPipeFromDescriptor(AutoDescriptor(fds[0]));
	second = createUnnamedPipeFromDescriptor(AutoDescriptor(fds[1]));
}

} // end namespace OW_NAMESPACE

