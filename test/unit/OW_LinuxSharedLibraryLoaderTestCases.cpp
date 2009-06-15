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


#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_LinuxSharedLibraryLoaderTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_LinuxSharedLibraryLoaderTestCases,"OW_LinuxSharedLibraryLoader");
#include "blocxx/SharedLibraryLoader.hpp"
#include "UnitTestEnvironment.hpp"

using namespace OpenWBEM;

namespace
{
	const String COMPONENT_NAME("ow.test");
}

void OW_LinuxSharedLibraryLoaderTestCases::setUp()
{
}

void OW_LinuxSharedLibraryLoaderTestCases::tearDown()
{
}

void OW_LinuxSharedLibraryLoaderTestCases::testLoadLibrary()
{
	// TODO: rename this to be generic
	SharedLibraryLoaderRef sll = SharedLibraryLoader::createSharedLibraryLoader();
	SharedLibraryRef lib = sll->loadSharedLibrary(
			"../../src/common/libopenwbem"OW_SHAREDLIB_EXTENSION);
	unitAssert( lib );
}

void OW_LinuxSharedLibraryLoaderTestCases::testGetFunctionPointer()
{
	SharedLibraryLoaderRef sll = SharedLibraryLoader::createSharedLibraryLoader();
	SharedLibraryRef lib = sll->loadSharedLibrary(
			"../../src/cimom/server/libowserver"OW_SHAREDLIB_EXTENSION);
	unitAssert( lib );
/*
	SharedLibraryLoaderRef (*createFunc)();
	unitAssert( SharedLibrary::getFunctionPointer( lib, "createSharedLibraryLoader__22SharedLibraryLoader", createFunc ) );
	unitAssert( createFunc != 0 );
	SharedLibraryLoaderRef sll2 = createFunc();
	unitAssert( !sll2.isNull() );
	*/
}

Test* OW_LinuxSharedLibraryLoaderTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_LinuxSharedLibraryLoader");

#if !defined(OW_STATIC_SERVICES) && !defined(OW_DARWIN)
	// Don't run this test if things are static, as it opens a library that
	// is statically linked to this executable.  On AIX, doing so causes a
	// core dump. 	We can't load this library dynamically on MacOS either
	// (it's not a real bundle).
	testSuite->addTest (new TestCaller <OW_LinuxSharedLibraryLoaderTestCases>
			("testLoadLibrary",
			&OW_LinuxSharedLibraryLoaderTestCases::testLoadLibrary));
	testSuite->addTest (new TestCaller <OW_LinuxSharedLibraryLoaderTestCases>
			("testGetFunctionPointer",
			&OW_LinuxSharedLibraryLoaderTestCases::testGetFunctionPointer));
#endif /* !defined(OW_STATIC_SERVICES) */

	return testSuite;
}

