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

#ifndef OW_NETWORK_TYPES_H_
#define OW_NETWORK_TYPES_H_

#include "OW_config.h"
#include "OW_Types.h"


extern "C"
{
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
}

#undef shutdown // On OpenUnix, sys/socket.h defines shutdown to be
				// _shutdown.  (which breaks OW_HTTPServer, etc.)

// Platform specific socket address type
typedef sockaddr		OW_SocketAddress_t;

// Platform specific inet socket address type
typedef sockaddr_in		OW_InetSocketAddress_t;

// Platform specific unix socket address type
typedef sockaddr_un		OW_UnixSocketAddress_t;

// Platform specific socket address type
typedef in_addr		OW_InetAddress_t;

// Platform specific socket fd type
typedef int 			OW_SocketHandle_t;

// Platform specific socket address family type
//typedef sa_family_t		OW_SAFamily_t;

#ifndef OW_HAVE_SOCKLEN_T
#ifdef OW_USE_GNU_PTH // pth.h typedefs socklen_t
#include <pth.h>
#else
typedef unsigned socklen_t;
#endif
#endif


#endif
