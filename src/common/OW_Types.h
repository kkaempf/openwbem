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

/**
 * @author Dan Nuffer
 */

#ifndef OW_TYPES_H_
#define OW_TYPES_H_

#include "OW_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>

#ifdef __cplusplus
}
#endif

typedef unsigned char      			OW_UInt8;
typedef signed char        			OW_Int8;

#if OW_SIZEOF_SHORT_INT == 2
typedef unsigned short     			OW_UInt16;
typedef short       				OW_Int16;
#elif OW_SIZEOF_INT == 2
typedef unsigned int     			OW_UInt16;
typedef int       					OW_Int16;
#endif

#if OW_SIZEOF_INT == 4
typedef unsigned int       			OW_UInt32;
typedef int         				OW_Int32;
#elif OW_SIZEOF_LONG_INT == 4
typedef unsigned long       		OW_UInt32;
typedef long         				OW_Int32;
#endif

#if OW_SIZEOF_LONG_INT == 8
typedef unsigned long   OW_UInt64;
typedef long    OW_Int64;
#elif OW_SIZEOF_LONG_LONG_INT == 8
typedef unsigned long long 			OW_UInt64;
typedef long long   				OW_Int64;
#else
#error "Compiler must support 64 bit long"
#endif

#if OW_SIZEOF_DOUBLE == 8
typedef double						OW_Real64;
#elif OW_SIZEOF_LONG_DOUBLE == 8
typedef long double					OW_Real64;
#endif

#if OW_SIZEOF_FLOAT == 4
typedef float						OW_Real32;
#elif OW_SIZEOF_DOUBLE == 4
typedef double						OW_Real32;
#endif

typedef off_t	OW_off_t;

#ifdef OW_WIN32
#define OW_SHAREDLIB_EXTENSION ".dll"
#define OW_FILENAME_SEPARATOR "\\"
#define OW_PATHNAME_SEPARATOR ";"
/* OW_Select_t is the type of object that can be used in
 * synchronous I/O multiplexing (i.e. select). On Win32
 * this is a HANDLE. The intention is to call
 * WaitForMultipleObjects on arrays of this type.
 */
struct OW_Select_t
{
	OW_Select_t() : event(NULL), sockfd(INVALID_SOCKET) {}
	OW_Select_t(const OW_Select_t& arg)
		: event(arg.event)
		, sockfd(arg.sockfd)
	{
	}
	
	HANDLE event;
	SOCKET sockfd;
};

//typedef HANDLE OW_Select_t;
#else
/* OW_Select_t is the type of object that can be used in
 * synchronous I/O multiplexing (i.e. select).
 */
typedef int OW_Select_t;

#if defined OW_DARWIN
#define OW_SHAREDLIB_EXTENSION ".dylib"
#elif defined OW_HPUX
#define OW_SHAREDLIB_EXTENSION ".sl"
#elif defined OW_NETWARE
#define OW_SHAREDLIB_EXTENSION ".nlm"
#else
#define OW_SHAREDLIB_EXTENSION ".so"
#endif
#define OW_FILENAME_SEPARATOR "/"
#define OW_PATHNAME_SEPARATOR ":"
#endif

#ifdef OW_WIN32
typedef HANDLE OW_FileHandle;
typedef int OW_UserId;
typedef DWORD OW_ProcId;
#else
typedef int OW_FileHandle;
typedef uid_t OW_UserId;
typedef pid_t OW_ProcId;
#endif

#endif


