/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_LinuxSharedLibraryLoaderTestCases.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "UnitTestEnvironment.hpp"

void OW_LinuxSharedLibraryLoaderTestCases::setUp()
{
}

void OW_LinuxSharedLibraryLoaderTestCases::tearDown()
{
}

void OW_LinuxSharedLibraryLoaderTestCases::testLoadLibrary()
{
	OW_SharedLibraryLoaderRef sll = OW_SharedLibraryLoader::createSharedLibraryLoader();
	OW_SharedLibraryRef lib = sll->loadSharedLibrary(
			"../../src/common/libopenwbem.so", g_testEnvironment->getLogger());
	unitAssert( !lib.isNull() );
}

void OW_LinuxSharedLibraryLoaderTestCases::testGetFunctionPointer()
{
	OW_SharedLibraryLoaderRef sll = OW_SharedLibraryLoader::createSharedLibraryLoader();
	OW_SharedLibraryRef lib = sll->loadSharedLibrary(
			"../../src/server/libowserver.so", g_testEnvironment->getLogger());
	unitAssert( !lib.isNull() );
/*
	OW_SharedLibraryLoaderRef (*createFunc)();
	unitAssert( OW_SharedLibrary::getFunctionPointer( lib, "createSharedLibraryLoader__22OW_SharedLibraryLoader", createFunc ) );
	unitAssert( createFunc != 0 );
	OW_SharedLibraryLoaderRef sll2 = createFunc();
	unitAssert( !sll2.isNull() );
	*/
}

Test* OW_LinuxSharedLibraryLoaderTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_LinuxSharedLibraryLoader");

	testSuite->addTest (new TestCaller <OW_LinuxSharedLibraryLoaderTestCases>
			("testLoadLibrary",
			&OW_LinuxSharedLibraryLoaderTestCases::testLoadLibrary));
	testSuite->addTest (new TestCaller <OW_LinuxSharedLibraryLoaderTestCases>
			("testGetFunctionPointer",
			&OW_LinuxSharedLibraryLoaderTestCases::testGetFunctionPointer));

	return testSuite;
}

