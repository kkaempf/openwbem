/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
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



// autoconf set variables here

@TOP@

/* The system has OpenSSL headers and libs */
#undef HAVE_OPENSSL

/* The system doesn't have OpenSSL */
#undef NO_SSL

#undef DEFAULT_SYSCONF_DIR
#undef DEFAULT_STATE_DIR
#undef DEFAULT_LIB_DIR
#undef DEFAULT_LIBEXEC_DIR
#undef PIDFILE_DIR
#undef ENABLE_STACK_TRACE_ON_EXCEPTIONS
#undef DEBUG
#undef PROFILE


/* Defined if we are building gnu pth version */
#undef USE_GNU_PTH

/* Defined if we are building gnu pthreads version */
#undef USE_PTHREAD

/* Defined if we want to use custom "new" operator to debug memory leaks */
#undef DEBUG_MEMORY

/* Define if we want to print "Entering"/"Leaving" <functionName> */
#undef PRINT_FUNC_DEBUG

/* Define if we want to check for NULL references and throw an exception */
#undef CHECK_NULL_REFERENCES

/* Define if we want to check for valid array indexing and throw an exception */
#undef CHECK_ARRAY_INDEXING

/* Define if we want to build the perlIFC */
#undef PERLIFC

/* Define which one is the current platform */
#undef GNU_LINUX
#undef OPENUNIX
#undef SOLARIS
#undef OPENSERVER
#undef DARWIN
#undef HPUX
#undef FREEBSD


#undef u_int8_t
#undef u_int16_t
#undef u_int32_t
#undef HAVE_SOCKLEN_T

#undef HAVE_PTHREAD_MUTEXATTR_SETTYPE
#undef HAVE_PTHREAD_SPIN_LOCK
#undef HAVE_PTHREAD_BARRIER
#undef HAVE_PAM

#undef DISABLE_ACLS
#undef DISABLE_DIGEST
#undef DISABLE_ASSOCIATION_TRAVERSAL
#undef DISABLE_QUALIFIER_DECLARATION
#undef DISABLE_SCHEMA_MANIPULATION
#undef DISABLE_INSTANCE_MANIPULATION

@BOTTOM@

// end of autoconf set vars


#ifdef __cplusplus
#ifdef OW_DEBUG_MEMORY
#include "OW_MemTracer.hpp"
#endif

/* For printing function names during debug */
#ifdef OW_PRINT_FUNC_DEBUG
#include "OW_FuncNamePrinter.hpp"

#define OW_PRINT_FUNC_NAME OW_FuncNamePrinter fnp##__LINE__ (__PRETTY_FUNCTION__)

#define OW_PRINT_FUNC_NAME_ARGS1(a) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a)
#define OW_PRINT_FUNC_NAME_ARGS2(a, b) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b)
#define OW_PRINT_FUNC_NAME_ARGS3(a, b, c) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b, c)
#define OW_PRINT_FUNC_NAME_ARGS4(a, b, c, d) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b, c, d)
#define OW_PRINT_FUNC_NAME_ARGS5(a, b, c, d, e) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b, c, d, e)
#define OW_PRINT_FUNC_NAME_ARGS6(a, b, c, d, e, f) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b, c, d, e, f)
#define OW_PRINT_FUNC_NAME_ARGS7(a, b, c, d, e, f, g) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b, c, d, e, f, g)
#define OW_PRINT_FUNC_NAME_ARGS8(a, b, c, d, e, f, g, h) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b, c, d, e, f, g, h)
#define OW_PRINT_FUNC_NAME_ARGS9(a, b, c, d, e, f, g, h, i) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b, c, d, e, f, g, h, i)
#define OW_PRINT_FUNC_NAME_ARGS10(a, b, c, d, e, f, g, h, i, j) OW_FuncNamePrinter fnp##__LINE__ ( __PRETTY_FUNCTION__ , a, b, c, d, e, f, g, h, i, j)
#else
#define OW_PRINT_FUNC_NAME_ARGS1(a)
#define OW_PRINT_FUNC_NAME_ARGS2(a, b)
#define OW_PRINT_FUNC_NAME_ARGS3(a, b, c)
#define OW_PRINT_FUNC_NAME_ARGS4(a, b, c, d)
#define OW_PRINT_FUNC_NAME_ARGS5(a, b, c, d, e)
#define OW_PRINT_FUNC_NAME_ARGS6(a, b, c, d, e, f)
#define OW_PRINT_FUNC_NAME_ARGS7(a, b, c, d, e, f, g)
#define OW_PRINT_FUNC_NAME_ARGS8(a, b, c, d, e, f, g, h)
#define OW_PRINT_FUNC_NAME_ARGS9(a, b, c, d, e, f, g, h, i)
#define OW_PRINT_FUNC_NAME_ARGS10(a, b, c, d, e, f, g, h, i, j)
#define OW_PRINT_FUNC_NAME
#endif // #ifdef OW_PRINT_FUNC_DEBUG

#endif // #ifdef __cplusplus


