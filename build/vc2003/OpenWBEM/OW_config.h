/* NOT automatically generated for windows */
#ifndef __SRC_COMMON_OW_CONFIG_H
#define __SRC_COMMON_OW_CONFIG_H 1

#if !defined(_MT)
#error The multithreaded C run-time library is required!
#endif

/* The system has OpenSSL headers and libs */
#ifndef OW_HAVE_OPENSSL 
#define OW_HAVE_OPENSSL  1 
#endif

/* The system doesn't have OpenSSL */
/* #undef OW_NO_SSL */

#ifndef OW_DEFAULT_SYSCONF_DIR 
#define OW_DEFAULT_SYSCONF_DIR  "/usr/local/etc" 
#endif

#ifndef OW_DEFAULT_STATE_DIR 
#define OW_DEFAULT_STATE_DIR  "/usr/local/var" 
#endif

#ifndef OW_DEFAULT_LIB_DIR 
#define OW_DEFAULT_LIB_DIR  "/usr/local/lib" 
#endif

#ifndef OW_DEFAULT_LIBEXEC_DIR 
#define OW_DEFAULT_LIBEXEC_DIR  "/usr/local/libexec" 
#endif

#ifndef OW_PIDFILE_DIR 
#define OW_PIDFILE_DIR  "/var/run" 
#endif

/* #undef OW_ENABLE_STACK_TRACE_ON_EXCEPTIONS */
#ifdef _DEBUG
	#ifndef OW_DEBUG 
		#define OW_DEBUG  1 
	#endif
#else
	#undef OW_DEBUG
#endif

/* #undef OW_PROFILE */


/* Defined if we want to use custom "new" operator to debug memory leaks */
/* #undef OW_DEBUG_MEMORY */

/* Define if we want to print "Entering"/"Leaving" <functionName> */
/* #undef OW_PRINT_FUNC_DEBUG */

/* Define if we want to check for NULL references and throw an exception */
#ifndef OW_CHECK_NULL_REFERENCES 
#define OW_CHECK_NULL_REFERENCES  1 
#endif

/* Define if we want to check for valid array indexing and throw an exception */
#ifndef OW_CHECK_ARRAY_INDEXING 
#define OW_CHECK_ARRAY_INDEXING  1 
#endif

/* Define if we want to build the perlIFC */
/* #undef OW_PERLIFC  1 */

/* Define which one is the current platform */
#ifndef OW_WIN32 
#define OW_WIN32  1 
#endif

// Shouldn't be looking at this. OW_WIN32 is enough
#ifndef OW_USE_WIN32_THREADS
#define OW_USE_WIN32_THREADS  1
#endif

/* #undef u_int8_t */
/* #undef u_int16_t */
/* #undef u_int32_t */
#ifndef OW_HAVE_SOCKLEN_T 
#define OW_HAVE_SOCKLEN_T  1 
#endif

/* #undef OW_HAVE_PTHREAD_MUTEXATTR_SETTYPE */
/* #undef OW_HAVE_PTHREAD_SPIN_LOCK  1 */
/* #undef OW_HAVE_PAM  1 */

/* #undef OW_DISABLE_DIGEST */
/* #undef OW_DISABLE_ASSOCIATION_TRAVERSAL */
/* #undef OW_DISABLE_QUALIFIER_DECLARATION */
/* #undef OW_DISABLE_SCHEMA_MANIPULATION */
/* #undef OW_DISABLE_INSTANCE_MANIPULATION */


/* Define to 1 if you have the `backtrace' function. */
/* #undef OW_HAVE_BACKTRACE  1 */

/* Define to 1 if you have the <byteswap.h> header file. */
/* #undef OW_HAVE_BYTESWAP_H  1 */

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef OW_HAVE_DOPRNT */

/* Define to 1 if you have the <ext/hash_map> header file. */
/* #ifndef OW_HAVE_EXT_HASH_MAP 
#define OW_HAVE_EXT_HASH_MAP  1 
#endif */

/* Define to 1 if you have the <fcntl.h> header file. */
/* #ifndef OW_HAVE_FCNTL_H 
#define OW_HAVE_FCNTL_H  1 
#endif */

/* Define to 1 if you have the `gethostbyname_r' function. */
/* #ifndef OW_HAVE_GETHOSTBYNAME_R 
#define OW_HAVE_GETHOSTBYNAME_R  1 
#endif */

/* Define to 1 if you have the <getopt.h> header file. */
/* #ifndef OW_HAVE_GETOPT_H 
#define OW_HAVE_GETOPT_H  1 
#endif */

/* Define to 1 if you have the `getopt_long' function. */
/* #ifndef OW_HAVE_GETOPT_LONG 
#define OW_HAVE_GETOPT_LONG  1 
#endif */

/* Define to 1 if you have the `getpagesize' function. */
/* #ifndef OW_HAVE_GETPAGESIZE 
#define OW_HAVE_GETPAGESIZE  1 
#endif */

/* Define to 1 if you have the <hash_map> header file. */
/* #undef OW_HAVE_HASH_MAP */

/* Define to 1 if you have the <inttypes.h> header file. */
//#ifndef OW_HAVE_INTTYPES_H 
//#define OW_HAVE_INTTYPES_H  1 
//#endif

/* Define to 1 if you have the <istream> header file. */
#ifndef OW_HAVE_ISTREAM 
#define OW_HAVE_ISTREAM  1 
#endif

/* Define to 1 if you have the <limits.h> header file. */
#ifndef OW_HAVE_LIMITS_H 
#define OW_HAVE_LIMITS_H  1 
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef OW_HAVE_MEMORY_H 
#define OW_HAVE_MEMORY_H  1 
#endif

/* Define to 1 if you have a working `mmap' system call. */
/* #ifndef OW_HAVE_MMAP 
#define OW_HAVE_MMAP  1 
#endif */

/* Define to 1 if you have the `nanosleep' function. */
/* #ifndef OW_HAVE_NANOSLEEP 
#define OW_HAVE_NANOSLEEP  1 
#endif */

/* Define to 1 if you have the <ostream> header file. */
#ifndef OW_HAVE_OSTREAM 
#define OW_HAVE_OSTREAM  1 
#endif

/* Define to 1 if you have the <ostream.h> header file. */
/* #ifndef OW_HAVE_OSTREAM_H 
#define OW_HAVE_OSTREAM_H  1 
#endif */

/* Define to 1 if you have the <pthread.h> header file. */
/* #ifndef OW_HAVE_PTHREAD_H 
#define OW_HAVE_PTHREAD_H  1 
#endif */

/* Define to 1 if you have the `sched_yield' function. */
/* #ifndef OW_HAVE_SCHED_YIELD 
#define OW_HAVE_SCHED_YIELD  1 
#endif */

/* Define to 1 if you have the <security/pam_appl.h> header file. */
/* #ifndef OW_HAVE_SECURITY_PAM_APPL_H 
#define OW_HAVE_SECURITY_PAM_APPL_H  1 
#endif */

/* Define to 1 if you have the <security/pam_misc.h> header file. */
/* #ifndef OW_HAVE_SECURITY_PAM_MISC_H 
#define OW_HAVE_SECURITY_PAM_MISC_H  1 
#endif */

/* Define to 1 if you have the <slp.h> header file. */
//#ifndef OW_HAVE_SLP_H 
//#define OW_HAVE_SLP_H  1 
//#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef OW_HAVE_STDINT_H 
#define OW_HAVE_STDINT_H  1 
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef OW_HAVE_STDLIB_H 
#define OW_HAVE_STDLIB_H  1 
#endif

/* Define to 1 if you have the <streambuf> header file. */
#ifndef OW_HAVE_STREAMBUF 
#define OW_HAVE_STREAMBUF  1 
#endif

/* Define to 1 if you have the <streambuf.h> header file. */
/* #ifndef OW_HAVE_STREAMBUF_H 
#define OW_HAVE_STREAMBUF_H  1 
#endif */

/* Define to 1 if you have the `strerror' function. */
#ifndef OW_HAVE_STRERROR 
#define OW_HAVE_STRERROR  1 
#endif

/* Define to 1 if you have the <strings.h> header file. */
/* #ifndef OW_HAVE_STRINGS_H 
#define OW_HAVE_STRINGS_H  1 
#endif */

/* Define to 1 if you have the <string.h> header file. */
#ifndef OW_HAVE_STRING_H 
#define OW_HAVE_STRING_H  1 
#endif

/* Define to 1 if you have the `strtoll' function. */
/* #ifndef OW_HAVE_STRTOLL 
#define OW_HAVE_STRTOLL  1 
#endif */

/* Define to 1 if you have the `strtoull' function. */
/* #ifndef OW_HAVE_STRTOULL 
#define OW_HAVE_STRTOULL  1 
#endif */

/* Define to 1 if `st_blksize' is member of `struct stat'. */
/* #ifndef OW_HAVE_STRUCT_STAT_ST_BLKSIZE 
#define OW_HAVE_STRUCT_STAT_ST_BLKSIZE  1 
#endif */

/* Define to 1 if your `struct stat' has `st_blksize'. Deprecated, use
   `HAVE_STRUCT_STAT_ST_BLKSIZE' instead. */
/* #ifndef OW_HAVE_ST_BLKSIZE 
#define OW_HAVE_ST_BLKSIZE  1 
#endif */

/* Define to 1 if you have the <sys/cdefs.h> header file. */
/* #ifndef OW_HAVE_SYS_CDEFS_H 
#define OW_HAVE_SYS_CDEFS_H  1 
#endif */

/* Define to 1 if you have the <sys/file.h> header file. */
/* #ifndef OW_HAVE_SYS_FILE_H 
#define OW_HAVE_SYS_FILE_H  1 
#endif */

/* Define to 1 if you have the <sys/int_types.h> header file. */
/* #undef OW_HAVE_SYS_INT_TYPES_H */

/* Define to 1 if you have the <sys/queue.h> header file. */
/* #ifndef OW_HAVE_SYS_QUEUE_H 
#define OW_HAVE_SYS_QUEUE_H  1 
#endif */

/* Define to 1 if you have the <sys/sockio.h> header file. */
/* #undef OW_HAVE_SYS_SOCKIO_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef OW_HAVE_SYS_STAT_H 
#define OW_HAVE_SYS_STAT_H  1 
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef OW_HAVE_SYS_TYPES_H 
#define OW_HAVE_SYS_TYPES_H  1 
#endif

/* Define to 1 if you have the <unistd.h> header file. */
/* #ifndef OW_HAVE_UNISTD_H 
#define OW_HAVE_UNISTD_H  1 
#endif */

/* Define to 1 if you have the `vprintf' function. */
#ifndef OW_HAVE_VPRINTF 
#define OW_HAVE_VPRINTF  1 
#endif

/* Define to 1 if you have the <zlib.h> header file. */
#ifndef OW_HAVE_ZLIB_H 
#define OW_HAVE_ZLIB_H  1 
#endif

/* Name of package */
#ifndef OW_PACKAGE 
#define OW_PACKAGE  "openwbem" 
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef OW_PACKAGE_BUGREPORT 
#define OW_PACKAGE_BUGREPORT  "" 
#endif

/* Define to the full name of this package. */
#ifndef OW_PACKAGE_NAME 
#define OW_PACKAGE_NAME  "" 
#endif

/* Define to the full name and version of this package. */
#ifndef OW_PACKAGE_STRING 
#define OW_PACKAGE_STRING  "" 
#endif

/* Define to the one symbol short name of this package. */
#ifndef OW_PACKAGE_TARNAME 
#define OW_PACKAGE_TARNAME  "" 
#endif

/* Define to the version of this package. */
#ifndef OW_PACKAGE_VERSION 
#define OW_PACKAGE_VERSION  "" 
#endif

/* The size of a `char', as computed by sizeof. */
#ifndef OW_SIZEOF_CHAR 
#define OW_SIZEOF_CHAR  1 
#endif

/* The size of a `char *', as computed by sizeof. */
#ifndef OW_SIZEOF_CHAR_P 
#define OW_SIZEOF_CHAR_P  4 
#endif

/* The size of a `double', as computed by sizeof. */
#ifndef OW_SIZEOF_DOUBLE 
#define OW_SIZEOF_DOUBLE  8 
#endif

/* The size of a `float', as computed by sizeof. */
#ifndef OW_SIZEOF_FLOAT 
#define OW_SIZEOF_FLOAT  4 
#endif

/* The size of a `int', as computed by sizeof. */
#ifndef OW_SIZEOF_INT 
#define OW_SIZEOF_INT  4 
#endif

/* The size of a `long double', as computed by sizeof. */
#ifndef OW_SIZEOF_LONG_DOUBLE 
#define OW_SIZEOF_LONG_DOUBLE  12 
#endif

/* The size of a `long int', as computed by sizeof. */
#ifndef OW_SIZEOF_LONG_INT 
#define OW_SIZEOF_LONG_INT  4 
#endif

/* The size of a `long long int', as computed by sizeof. */
#ifndef OW_SIZEOF_LONG_LONG_INT 
#define OW_SIZEOF_LONG_LONG_INT  8 
#endif

/* The size of a `short int', as computed by sizeof. */
#ifndef OW_SIZEOF_SHORT_INT 
#define OW_SIZEOF_SHORT_INT  2 
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef OW_STDC_HEADERS 
#define OW_STDC_HEADERS  1 
#endif

/* Define to 1 if `sys_siglist' is declared by <signal.h> or <unistd.h>. */
/* #ifndef OW_SYS_SIGLIST_DECLARED 
#define OW_SYS_SIGLIST_DECLARED  1 
#endif */

/* Version number of package */
#ifndef OW_VERSION 
#define OW_VERSION  "2.9.1" 
#endif

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef OW_WORDS_BIGENDIAN */

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#ifndef OW_YYTEXT_POINTER 
#define OW_YYTEXT_POINTER  1 
#endif

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `uint16_t' if <sys/types.h> does not define. */
/* #undef u_int16_t */

/* Define to `uint32_t' if <sys/types.h> does not define. */
/* #undef u_int32_t */

/* Define to `uint8_t' if <sys/types.h> does not define. */
/* #undef u_int8_t */

// end of autoconf set vars

/**
 * The OW_DEPRECATED macro can be used to trigger compile-time warnings
 * with gcc >= 3.2 when deprecated functions are used.
 *
 * For non-inline functions, the macro is used at the very end of the
 * function declaration, right before the semicolon, unless it's pure
 * virtual:
 *
 * int deprecatedFunc() const OW_DEPRECATED;
 * virtual int deprecatedPureVirtualFunc() const OW_DEPRECATED = 0;
 *
 * Functions which are implemented inline are handled differently:
 * the OW_DEPRECATED macro is used at the front, right before the return
 * type, but after "static" or "virtual":
 *
 * OW_DEPRECATED void deprecatedFuncA() { .. }
 * virtual OW_DEPRECATED int deprecatedFuncB() { .. }
 * static OW_DEPRECATED bool deprecatedFuncC() { .. }
 *
 * You can also mark whole structs or classes as deprecated, by inserting the
 * OW_DEPRECATED macro after the struct/class keyword, but before the
 * name of the struct/class:
 *
 * class OW_DEPRECATED DeprecatedClass { };
 * struct OW_DEPRECATED DeprecatedStruct { };
 *
 */
#if __GNUC__ - 0 > 3 || (__GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 >= 2)
# define OW_DEPRECATED __attribute__ ((deprecated))
#else
# define OW_DEPRECATED
#endif




#ifdef OW_DEBUG_MEMORY
#include "OW_MemTracer.hpp"
#endif

/* For printing function names during debug */
#ifdef OW_PRINT_FUNC_DEBUG
#include "OW_FuncNamePrinter.hpp"

#ifndef OW_PRINT_FUNC_NAME 

#define OW_PRINT_FUNC_NAME  OW_FuncNamePrinter fnp##__LINE__ (__FUNCTION__) 
#endif

#ifndef OW_PRINT_FUNC_NAME_ARGS1 
#define OW_PRINT_FUNC_NAME_ARGS1 (a) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS2 
#define OW_PRINT_FUNC_NAME_ARGS2 (a, b) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS3 
#define OW_PRINT_FUNC_NAME_ARGS3 (a, b, c) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b, c) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS4 
#define OW_PRINT_FUNC_NAME_ARGS4 (a, b, c, d) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b, c, d) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS5 
#define OW_PRINT_FUNC_NAME_ARGS5 (a, b, c, d, e) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b, c, d, e) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS6 
#define OW_PRINT_FUNC_NAME_ARGS6 (a, b, c, d, e, f) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b, c, d, e, f) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS7 
#define OW_PRINT_FUNC_NAME_ARGS7 (a, b, c, d, e, f, g) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b, c, d, e, f, g) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS8 
#define OW_PRINT_FUNC_NAME_ARGS8 (a, b, c, d, e, f, g, h) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b, c, d, e, f, g, h) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS9 
#define OW_PRINT_FUNC_NAME_ARGS9 (a, b, c, d, e, f, g, h, i) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b, c, d, e, f, g, h, i) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS10 
#define OW_PRINT_FUNC_NAME_ARGS10 (a, b, c, d, e, f, g, h, i, j) OW_FuncNamePrinter fnp##__LINE__ ( __FUNCTION__ , a, b, c, d, e, f, g, h, i, j) 
#endif
#else
#ifndef OW_PRINT_FUNC_NAME_ARGS1 
#define OW_PRINT_FUNC_NAME_ARGS1 (a) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS2 
#define OW_PRINT_FUNC_NAME_ARGS2 (a, b) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS3 
#define OW_PRINT_FUNC_NAME_ARGS3 (a, b, c) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS4 
#define OW_PRINT_FUNC_NAME_ARGS4 (a, b, c, d) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS5 
#define OW_PRINT_FUNC_NAME_ARGS5 (a, b, c, d, e) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS6 
#define OW_PRINT_FUNC_NAME_ARGS6 (a, b, c, d, e, f) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS7 
#define OW_PRINT_FUNC_NAME_ARGS7 (a, b, c, d, e, f, g) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS8 
#define OW_PRINT_FUNC_NAME_ARGS8 (a, b, c, d, e, f, g, h) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS9 
#define OW_PRINT_FUNC_NAME_ARGS9 (a, b, c, d, e, f, g, h, i) 
#endif
#ifndef OW_PRINT_FUNC_NAME_ARGS10 
#define OW_PRINT_FUNC_NAME_ARGS10 (a, b, c, d, e, f, g, h, i, j) 
#endif
#ifndef OW_PRINT_FUNC_NAME 
#define OW_PRINT_FUNC_NAME  
#endif
#endif // #ifdef OW_PRINT_FUNC_DEBUG

#ifdef OW_HAVE_SLP_H 
#undef OW_HAVE_SLP_H
#endif

#ifndef OW_DISABLE_DIGEST
#define OW_DISABLE_DIGEST
#endif

// Statements like:
// #pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
// C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles. Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution. You can double
// click on these messages and jump to the line in question.
#define Stringize( L ) #L
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define Reminder __FILE__ "(" $Line ") : Reminder: "

#define _WIN32_WINNT 0x0400
#define OW_USE_DLL 1
#define NO_POSIX_SIGNALS 1
//#define OW_DEBUG_MEMORY 1

#define snprintf _snprintf

// Including winsock2.h should also bring in windows.h
#include <winsock2.h>

#define OW_EXPORT __declspec(dllexport)
#define OW_IMPORT __declspec(dllimport)

#ifdef OW_COMMON_BUILD
#define OW_COMMON_API OW_EXPORT
#else
#define OW_COMMON_API OW_IMPORT
#endif

#ifdef OW_XML_BUILD
#define OW_XML_API OW_EXPORT
#else
#define OW_XML_API OW_IMPORT
#endif

#ifdef OW_MOF_BUILD
#define OW_MOF_API OW_EXPORT
#else
#define OW_MOF_API OW_IMPORT
#endif

#ifdef OW_HTTP_BUILD
#define OW_HTTP_API OW_EXPORT
#else
#define OW_HTTP_API OW_IMPORT
#endif

#ifdef OW_CLIENT_BUILD
#define OW_CLIENT_API OW_EXPORT
#else
#define OW_CLIENT_API OW_IMPORT
#endif

#ifdef OW_LISTENER_BUILD
#define OW_LISTENER_API OW_EXPORT
#else
#define OW_LISTENER_API OW_IMPORT
#endif

 
/* __SRC_COMMON_OW_CONFIG_H */
#endif
