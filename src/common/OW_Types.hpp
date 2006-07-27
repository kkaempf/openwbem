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

#ifndef OW_TYPES_HPP_INCLUDE_GUARD_
#define OW_TYPES_HPP_INCLUDE_GUARD_
#include "OW_config.h"

#ifndef __cplusplus
#error "OW_Types.hpp can only be included by c++ files"
#endif

extern "C"
{
#include <sys/types.h>
}

namespace OW_NAMESPACE
{

typedef unsigned char      			UInt8;
typedef signed char        			Int8;
#if OW_SIZEOF_SHORT_INT == 2
typedef unsigned short     			UInt16;
typedef short       				Int16;
#define OW_INT16_IS_SHORT 1
#elif OW_SIZEOF_INT == 2
typedef unsigned int     			UInt16;
typedef int       					Int16;
#define OW_INT16_IS_INT 1
#endif
#if OW_SIZEOF_INT == 4
typedef unsigned int       			UInt32;
typedef int         				Int32;
#define OW_INT32_IS_INT 1
#elif OW_SIZEOF_LONG_INT == 4
typedef unsigned long       		UInt32;
typedef long         				Int32;
#define OW_INT32_IS_LONG 1
#endif
#if OW_SIZEOF_LONG_INT == 8
typedef unsigned long   UInt64;
typedef long    Int64;
#define OW_INT64_IS_LONG 1
#elif OW_SIZEOF_LONG_LONG_INT == 8
typedef unsigned long long 			UInt64;
typedef long long   				Int64;
#define OW_INT64_IS_LONG_LONG 1
#else
#error "Compiler must support 64 bit long"
#endif
#if OW_SIZEOF_DOUBLE == 8
typedef double						Real64;
#define OW_REAL64_IS_DOUBLE 1
#elif OW_SIZEOF_LONG_DOUBLE == 8
typedef long double					Real64;
#define OW_REAL64_IS_LONG_DOUBLE 1
#endif
#if OW_SIZEOF_FLOAT == 4
typedef float						Real32;
#define OW_REAL32_IS_FLOAT 1
#elif OW_SIZEOF_DOUBLE == 4
typedef double						Real32;
#define OW_REAL32_IS_DOUBLE 1
#endif

#ifdef OW_WIN32

#define OW_SHAREDLIB_EXTENSION ".dll"
#define OW_FILENAME_SEPARATOR "\\"
#define OW_FILENAME_SEPARATOR_C '\\'
#define OW_PATHNAME_SEPARATOR ";"
// OW_Select_t is the type of object that can be used in
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

#if defined OW_DARWIN
#define OW_SHAREDLIB_EXTENSION ".dylib.bundle"
#elif defined OW_HPUX
#define OW_SHAREDLIB_EXTENSION ".sl"
#elif defined OW_NETWARE
#define OW_SHAREDLIB_EXTENSION ".nlm"
#else
#define OW_SHAREDLIB_EXTENSION ".so"
#endif
#define OW_FILENAME_SEPARATOR "/"
#define OW_FILENAME_SEPARATOR_C '/'
#define OW_PATHNAME_SEPARATOR ":"
#endif

#ifdef OW_WIN32
typedef HANDLE FileHandle;
typedef HANDLE Descriptor;
#define OW_INVALID_FILEHANDLE INVALID_HANDLE_VALUE
typedef int UserId;
typedef int uid_t;
typedef DWORD pid_t;
typedef DWORD ProcId;
typedef struct {} siginfo_t;
#else
typedef int FileHandle;
typedef int Descriptor;
#define OW_INVALID_FILEHANDLE -1
typedef uid_t UserId;
typedef pid_t ProcId;
#endif

} // end namespace OW_NAMESPACE

#endif
