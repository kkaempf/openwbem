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
 * @author Dan Nuffer
 */


#include "OW_config.h"

#ifdef OW_DEBUG_MEMORY
#include "OW_MemTracerTestCases.hpp"
#endif

// test cases includes -- DO NOT EDIT THIS COMMENT
#include "CIMObjectPathTestCases.hpp"
#include "URLTestCases.hpp"
#include "OW_BinarySerializationTestCases.hpp"
#include "OperationContextTestCases.hpp"
#include "MOFCompilerTestCases.hpp"
#include "OW_HTTPUtilsTestCases.hpp"
#include "OW_CIMDateTimeTestCases.hpp"
#include "OW_DateTimeTestCases.hpp"
#include "OW_UTF8UtilsTestCases.hpp"
#include "OW_RandomNumberTestCases.hpp"
#include "OW_UUIDTestCases.hpp"
#include "OW_ThreadPoolTestCases.hpp"
#include "OW_ThreadTestCases.hpp"
#include "OW_ThreadBarrierTestCases.hpp"
#include "OW_ReferenceTestCases.hpp"
#include "OW_SocketUtilsTestCases.hpp"
#include "OW_RWLockerTestCases.hpp"
#include "OW_FileSystemTestCases.hpp"
#include "OW_CIMPropertyTestCases.hpp"
#include "OW_CIMInstanceTestCases.hpp"
#include "OW_StringArrayPropertyTestCases.hpp"
#include "OW_CIMValueTestCases.hpp"
#include "OW_CIMClassTestCases.hpp"
#include "OW_ExceptionTestCases.hpp"
#include "OW_StringStreamTestCases.hpp"
#include "OW_MutexTestCases.hpp"
#include "OW_CIMUrlTestCases.hpp"
#include "OW_ListenerAuthenticatorTestCases.hpp"
#include "OW_InetAddressTestCases.hpp"
#include "OW_Base64TestCases.hpp"
#include "OW_MD5TestCases.hpp"
#include "OW_XMLEscapeTestCases.hpp"
#include "OW_XMLUnescapeTestCases.hpp"
#include "OW_EnumerationTestCases.hpp"
#include "OW_StackTraceTestCases.hpp"
#include "OW_ExecTestCases.hpp"
#include "OW_LinuxSharedLibraryLoaderTestCases.hpp"
#include "OW_ProviderMuxTestCases.hpp"
#include "OW_ProviderMuxLoaderTestCases.hpp"
#include "OW_SharedLibraryLoaderTestCases.hpp"
#include "OW_SharedLibraryTestCases.hpp"
#include "OW_ProviderManagerTestCases.hpp"
#include "OW_HTTPChunkerTestCases.hpp"
#include "OW_LoggerTestCases.hpp"
#include "OW_StringTestCases.hpp"
#include "OW_FormatTestCases.hpp"
#include "OW_ConfigOpts.hpp"

// includes for this file
#include "TestRunner.hpp"
#include "OW_String.hpp"

int main( int argc, char *argv[])
{
	TestRunner runner;
	
	// add tests to runner -- DO NOT EDIT THIS COMMENT
	runner.addTest( "CIMObjectPath", CIMObjectPathTestCases::suite());
	runner.addTest( "URL", URLTestCases::suite());
	runner.addTest( "OW_BinarySerialization", OW_BinarySerializationTestCases::suite());
	runner.addTest( "OperationContext", OperationContextTestCases::suite());
	runner.addTest( "MOFCompiler", MOFCompilerTestCases::suite());
	runner.addTest( "OW_HTTPUtils", OW_HTTPUtilsTestCases::suite());
	runner.addTest( "OW_CIMDateTime", OW_CIMDateTimeTestCases::suite());
	runner.addTest( "OW_DateTime", OW_DateTimeTestCases::suite());
	runner.addTest( "OW_UTF8Utils", OW_UTF8UtilsTestCases::suite());
	runner.addTest( "OW_RandomNumber", OW_RandomNumberTestCases::suite());
	runner.addTest( "OW_UUID", OW_UUIDTestCases::suite());
	runner.addTest( "OW_ThreadPool", OW_ThreadPoolTestCases::suite());
	runner.addTest( "OW_Thread", OW_ThreadTestCases::suite());
	runner.addTest( "OW_ThreadBarrier", OW_ThreadBarrierTestCases::suite());
	runner.addTest( "OW_Reference", OW_ReferenceTestCases::suite());
	runner.addTest( "OW_SocketUtils", OW_SocketUtilsTestCases::suite());
	runner.addTest( "OW_RWLocker", OW_RWLockerTestCases::suite());
	runner.addTest( "OW_FileSystem", OW_FileSystemTestCases::suite());
	runner.addTest( "OW_CIMProperty", OW_CIMPropertyTestCases::suite());
	runner.addTest( "OW_CIMInstance", OW_CIMInstanceTestCases::suite());
	runner.addTest( "OW_StringArrayProperty", OW_StringArrayPropertyTestCases::suite());
	runner.addTest( "OW_CIMValue", OW_CIMValueTestCases::suite());
	runner.addTest( "OW_CIMClass", OW_CIMClassTestCases::suite());
	runner.addTest( "OW_Exception", OW_ExceptionTestCases::suite());
	runner.addTest( "OW_StringStream", OW_StringStreamTestCases::suite());
#ifdef OW_DEBUG_MEMORY
	runner.addTest( "OW_MemTracer", OW_MemTracerTestCases::suite());
#endif
	runner.addTest( "OW_Mutex", OW_MutexTestCases::suite());
	runner.addTest( "OW_CIMUrl", OW_CIMUrlTestCases::suite());
	runner.addTest( "OW_ListenerAuthenticator", OW_ListenerAuthenticatorTestCases::suite());
	runner.addTest( "OW_SocketAddress", OW_InetAddressTestCases::suite());
	runner.addTest( "OW_Base64", OW_Base64TestCases::suite());
	runner.addTest( "OW_MD5", OW_MD5TestCases::suite());
	runner.addTest( "OW_XMLEscape", OW_XMLEscapeTestCases::suite());
	runner.addTest( "OW_XMLUnescape", OW_XMLUnescapeTestCases::suite());
	runner.addTest( "OW_Enumeration", OW_EnumerationTestCases::suite());
#ifdef OW_ENABLE_STACK_TRACE_ON_EXCEPTIONS
	runner.addTest( "OW_StackTrace", OW_StackTraceTestCases::suite());
#endif
	runner.addTest( "OW_Exec", OW_ExecTestCases::suite());

#if !defined(OW_STATIC_SERVICES)
	// Don't run this test if things are static, as it opens a library that
	// is statically linked to this executable.  On AIX, doing so causes a
	// core dump. 	
	runner.addTest( "OW_LinuxSharedLibraryLoader", OW_LinuxSharedLibraryLoaderTestCases::suite());
#endif /* !defined(OW_STATIC_SERVICES) */

	runner.addTest( "OW_ProviderMux", OW_ProviderMuxTestCases::suite());
	runner.addTest( "OW_ProviderMuxLoader", OW_ProviderMuxLoaderTestCases::suite());
	runner.addTest( "OW_SharedLibraryLoader", OW_SharedLibraryLoaderTestCases::suite());
	runner.addTest( "OW_SharedLibrary", OW_SharedLibraryTestCases::suite());
	runner.addTest( "OW_ProviderManager", OW_ProviderManagerTestCases::suite());
	runner.addTest( "OW_HTTPChunker", OW_HTTPChunkerTestCases::suite());
	runner.addTest( "OW_Logger", OW_LoggerTestCases::suite());
	runner.addTest( "OW_String", OW_StringTestCases::suite());
	runner.addTest( "OW_Format", OW_FormatTestCases::suite());

	if ( argc < 2 || ( argc == 2 && OpenWBEM::String("all") == argv[1] ) )
	{
		runner.runAll();
	}
	else
	{
		runner.run( argv[1] );
	}
	return 0;
}


