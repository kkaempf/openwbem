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

#ifndef OWBI1_TYPES_HPP_INCLUDE_GUARD_
#define OWBI1_TYPES_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"

#ifndef __cplusplus
#error "OWBI1_Types.hpp can only be included by c++ files"
#endif

extern "C"
{
#include <sys/types.h>
}

namespace OWBI1
{

typedef unsigned char      			UInt8;
typedef signed char        			Int8;
#if OWBI1_SIZEOF_SHORT_INT == 2
typedef unsigned short     			UInt16;
typedef short       				Int16;
#define OWBI1_INT16_IS_SHORT 1
#elif OWBI1_SIZEOF_INT == 2
typedef unsigned int     			UInt16;
typedef int       					Int16;
#define OWBI1_INT16_IS_INT 1
#endif
#if OWBI1_SIZEOF_INT == 4
typedef unsigned int       			UInt32;
typedef int         				Int32;
#define OWBI1_INT32_IS_INT 1
#elif OWBI1_SIZEOF_LONG_INT == 4
typedef unsigned long       		UInt32;
typedef long         				Int32;
#define OWBI1_INT32_IS_LONG 1
#endif
#if OWBI1_SIZEOF_LONG_INT == 8
typedef unsigned long   UInt64;
typedef long    Int64;
#define OWBI1_INT64_IS_LONG 1
#elif OWBI1_SIZEOF_LONG_LONG_INT == 8
typedef unsigned long long 			UInt64;
typedef long long   				Int64;
#define OWBI1_INT64_IS_LONG_LONG 1
#else
#error "Compiler must support 64 bit long"
#endif
#if OWBI1_SIZEOF_DOUBLE == 8
typedef double						Real64;
#define OWBI1_REAL64_IS_DOUBLE 1
#elif OWBI1_SIZEOF_LONG_DOUBLE == 8
typedef long double					Real64;
#define OWBI1_REAL64_IS_LONG_DOUBLE 1
#endif
#if OWBI1_SIZEOF_FLOAT == 4
typedef float						Real32;
#define OWBI1_REAL32_IS_FLOAT 1
#elif OWBI1_SIZEOF_DOUBLE == 4
typedef double						Real32;
#define OWBI1_REAL32_IS_DOUBLE 1
#endif
typedef off_t	off_t;

#ifdef OWBI1_WIN32

#define OWBI1_SHAREDLIB_EXTENSION ".dll"
#define OWBI1_FILENAME_SEPARATOR "\\"
#define OWBI1_FILENAME_SEPARATOR_C '\\'
#define OWBI1_PATHNAME_SEPARATOR ";"
// OWBI1_Select_t is the type of object that can be used in
// synchronous I/O multiplexing (i.e. select).
struct Select_t
{
	Select_t() 
		: event(NULL)
		, sockfd(INVALID_SOCKET)
		, networkevents(0)
		, doreset(false)
	{
	}

	Select_t(const Select_t& arg)
		: event(arg.event)
		, sockfd(arg.sockfd)
		, networkevents(arg.networkevents)
		, doreset(arg.doreset)
	{
	}
	
	HANDLE event;
	SOCKET sockfd;
	long networkevents;
	bool doreset;
};

//typedef HANDLE Select_t;
#else
// Select_t is the type of object that can be used in
// synchronous I/O multiplexing (i.e. select). There is a
// possibility this can be something other than an int on
// a platform we don't yet support.
typedef int Select_t;

#if defined OWBI1_DARWIN
#define OWBI1_SHAREDLIB_EXTENSION ".dylib"
#elif defined OWBI1_HPUX
#define OWBI1_SHAREDLIB_EXTENSION ".sl"
#elif defined OWBI1_NETWARE
#define OWBI1_SHAREDLIB_EXTENSION ".nlm"
#else
#define OWBI1_SHAREDLIB_EXTENSION ".so"
#endif
#define OWBI1_FILENAME_SEPARATOR "/"
#define OWBI1_FILENAME_SEPARATOR_C '/'
#define OWBI1_PATHNAME_SEPARATOR ":"
#endif

#ifdef OWBI1_WIN32
typedef HANDLE FileHandle;
#define OWBI1_INVALID_FILEHANDLE INVALID_HANDLE_VALUE
typedef int UserId;
typedef int uid_t;
typedef DWORD ProcId;
#else
typedef int FileHandle;
#define OWBI1_INVALID_FILEHANDLE -1
typedef uid_t UserId;
typedef pid_t ProcId;
#endif

} // end namespace OWBI1

#endif
