/*******************************************************************************
* Copyright (C) 2005-2006 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL QUEST SOFTWARE, INC. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Kevin Van Horn
 * @author Dan Nuffer
 */

/**
 * If you make improvements to this file, consider making the same 
 * improvements in DescriptorUtils_noexcept.cpp in BloCxx
 */

// THIS CODE MUST NOT THROW EXCEPTIONS.  IT IS ALSO HIGHLY PREFERRED THAT
// IT NOT RELY ON ANY LIBRARY OTHER THAN STANDARD SYSTEM LIBRARIES AND THE
// STANDARD C++ LIBRARY, AS IT IS USED IN libowcprivman, AND WE WANT TO AVOID
// LINKING OTHER LIBRARIES IN WITH libowcprivman.

#include "OW_config.h"
#include "Cifc_DescriptorUtils_noexcept.hpp"
#include "Cifc_AutoDescriptor.hpp"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

namespace OW_NAMESPACE
{

namespace
{
	char const MAGIC_CHAR = '\xa5';

	AutoDescriptor copy_error(char * dst, size_t dstsz, char const * src)
	{
		std::strncpy(dst, src, dstsz);
		dst[dstsz - 1] = '\0';
		return AutoDescriptor();
	}
}

int passDescriptor(Descriptor streamPipe, Descriptor descriptor)
{
	struct msghdr msg;
	::memset(&msg, 0, sizeof(msg));
	struct iovec iov[1];
	::memset(iov, 0, sizeof(iov[0]));

#ifdef OW_HAVE_MSGHDR_MSG_CONTROL

// We need the newer CMSG_LEN() and CMSG_SPACE() macros, but few
// implementations support them today.  These two macros really need
// an ALIGN() macro, but each implementation does this differently.
#ifndef CMSG_LEN
#define CMSG_LEN(size)      (sizeof(struct cmsghdr) + (size))
#endif

#ifndef CMSG_SPACE
#define CMSG_SPACE(size)    (sizeof(struct cmsghdr) + (size))
#endif

	union {
	  struct cmsghdr cm;
	  char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr * cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*(reinterpret_cast<int *>(CMSG_DATA(cmptr))) = descriptor;
#else
	msg.msg_accrights = static_cast<caddr_t>(&descriptor);
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = 0;
	msg.msg_namelen = 0;

	char dummy[1] = { MAGIC_CHAR };
	iov[0].iov_base = dummy;
	iov[0].iov_len = 1;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return ::sendmsg(streamPipe, &msg, 0);
}

AutoDescriptor receiveDescriptor(Descriptor streamPipe, char * errbuf, size_t bufsz)
{
	struct msghdr msg;
	struct iovec iov[1];

	msg = msghdr(); // zero-init to make valgrind happy
#ifdef OW_HAVE_MSGHDR_MSG_CONTROL
	union {
	  struct cmsghdr cm;
	  char control[CMSG_SPACE(sizeof(int))];
	} control_un;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
#else
	int	newfd = -1;

	msg.msg_accrights = static_cast<caddr_t>(&newfd);
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = 0;
	msg.msg_namelen = 0;

	char dummy[1];
	iov[0].iov_base = dummy;
	iov[0].iov_len = 1;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	ssize_t	n = ::recvmsg(streamPipe, &msg, 0);
	if (n == 0)
	{
		return copy_error(errbuf, bufsz,
			"unexpected end of input when receiving handle");
	}
	if (n < 0)
	{
		return copy_error(errbuf, bufsz, "recvmsg() failed");
	}
	if (dummy[0] != MAGIC_CHAR)
	{
		return copy_error(errbuf, bufsz, "bad magic char when receiving handle");
	}


#ifdef OW_HAVE_MSGHDR_MSG_CONTROL
	struct cmsghdr * cmptr = CMSG_FIRSTHDR(&msg);
	if (!cmptr)
	{
		return copy_error(errbuf, bufsz,
			"missing control message when receiving handle");
	}
	// as far as I can tell, HP-UX is just broken and sets cmptr->cmsg_len to 12. Things work anyway.
#if !defined (OW_HPUX)
	if (cmptr->cmsg_len != CMSG_LEN(sizeof(int)))
	{
		return copy_error(errbuf, bufsz,
			"cmptr->cmsg_len != CMSG_LEN(sizeof(int)) when receiving handle");
	}
#endif
	if (cmptr->cmsg_level != SOL_SOCKET)
	{
		return copy_error(errbuf, bufsz,
			"control level != SOL_SOCKET when receiving handle");
	}
	if (cmptr->cmsg_type != SCM_RIGHTS)
	{
		return copy_error(errbuf, bufsz,
			"control type != SCM_RIGHTS when receiving handle");
	}
	return AutoDescriptor(*(reinterpret_cast<int *>(CMSG_DATA(cmptr))));
#else
	if (msg.msg_accrightslen != sizeof(int))
	{
		return copy_error(errbuf, bufsz,
			"bad control message when receiving handle");
	}
	return AutoDescriptor(newfd);
#endif

}

} // end namespace OW_NAMESPACE
