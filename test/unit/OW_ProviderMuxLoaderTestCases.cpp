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
#include "OW_ProviderMuxLoaderTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_ProviderMuxLoaderTestCases,"OW_ProviderMuxLoader");
#include "OW_ProviderIFCLoader.hpp"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "blocxx/SharedLibraryLoader.hpp"
#include "blocxx/SharedLibrary.hpp"
#include "blocxx/Array.hpp"
#include "testSharedLibraryLoader.hpp"
#include "OW_ConfigOpts.hpp"
#include <iostream>

using std::cout;
using std::endl;

using namespace OpenWBEM;

void OW_ProviderMuxLoaderTestCases::setUp()
{
}

void OW_ProviderMuxLoaderTestCases::tearDown()
{
}

void OW_ProviderMuxLoaderTestCases::testLoadIFCs()
{
	Array<ProviderIFCBaseIFCRef> muxarray;
	ProviderIFCLoaderRef pml = testCreateMuxLoader();
	unitAssertNoThrow( pml->loadIFCs( muxarray ) );
	unitAssert( muxarray.size() == 3 );
	unitAssert( muxarray[0]->getName() == String("lib1") );
}

void OW_ProviderMuxLoaderTestCases::testFailLoadIFCs()
{
	Array<ProviderIFCBaseIFCRef> muxarray;
	testMuxLoaderBad pml( testCreateSharedLibraryLoader() );
	unitAssertNoThrow(pml.loadIFCs( muxarray ));
	unitAssert( muxarray.size() == 0 );
}

Test* OW_ProviderMuxLoaderTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_ProviderIFCLoader");

	testSuite->addTest (new TestCaller <OW_ProviderMuxLoaderTestCases>
			("testLoadIFCs",
			&OW_ProviderMuxLoaderTestCases::testLoadIFCs));
	testSuite->addTest (new TestCaller <OW_ProviderMuxLoaderTestCases>
			("testFailLoadIFCs",
			&OW_ProviderMuxLoaderTestCases::testFailLoadIFCs));

	return testSuite;
}

