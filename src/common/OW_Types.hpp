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
#ifdef __cplusplus
extern "C"
{
#endif
#include <sys/types.h>
#ifdef __cplusplus
}
#endif
#ifdef OW_WIN32
#include <windows.h>
#endif

namespace OpenWBEM
{

typedef unsigned char      			UInt8;
typedef signed char        			Int8;
#if OW_SIZEOF_SHORT_INT == 2
typedef unsigned short     			UInt16;
typedef short       				Int16;
#elif OW_SIZEOF_INT == 2
typedef unsigned int     			UInt16;
typedef int       					Int16;
#endif
#if OW_SIZEOF_INT == 4
typedef unsigned int       			UInt32;
typedef int         				Int32;
#elif OW_SIZEOF_LONG_INT == 4
typedef unsigned long       		UInt32;
typedef long         				Int32;
#endif
#if OW_SIZEOF_LONG_INT == 8
typedef unsigned long   UInt64;
typedef long    Int64;
#elif OW_SIZEOF_LONG_LONG_INT == 8
typedef unsigned long long 			UInt64;
typedef long long   				Int64;
#else
#error "Compiler must support 64 bit long"
#endif
#if OW_SIZEOF_DOUBLE == 8
typedef double						Real64;
#elif OW_SIZEOF_LONG_DOUBLE == 8
typedef long double					Real64;
#endif
#if OW_SIZEOF_FLOAT == 4
typedef float						Real32;
#elif OW_SIZEOF_DOUBLE == 4
typedef double						Real32;
#endif
typedef off_t	off_t;

#ifdef OW_WIN32
#define OW_SHAREDLIB_EXTENSION ".dll"
#define OW_FILENAME_SEPARATOR "\\"
#define OW_PATHNAME_SEPARATOR ";"
typedef HANDLE Select_t;
#else
// Select_t is the type of object that can be used in
// synchronous I/O multiplexing (i.e. select). There is a
// possibility this can be something other than an int on
// a platform we don't yet support.
typedef int Select_t;

#if defined OW_DARWIN
#define OW_SHAREDLIB_EXTENSION ".dylib"
#elif defined OW_HPUX
#define OW_SHAREDLIB_EXTENSION ".sl"
#else
#define OW_SHAREDLIB_EXTENSION ".so"
#endif
#define OW_FILENAME_SEPARATOR "/"
#define OW_PATHNAME_SEPARATOR ":"
#endif

typedef int		FileHandle;
#ifdef OW_WIN32
typedef int UserId;
typedef DWORD ProcId;
#else
typedef uid_t UserId;
typedef pid_t ProcId;
#endif

} // end namespace OpenWBEM

#endif
