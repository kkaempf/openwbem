/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
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
#include "OW_ProviderMuxLoaderTestCases.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_Array.hpp"
#include "testSharedLibraryLoader.hpp"
#include "OW_ConfigOpts.hpp"
#include <iostream>

using std::cout;
using std::endl;

void OW_ProviderMuxLoaderTestCases::setUp()
{
}

void OW_ProviderMuxLoaderTestCases::tearDown()
{
}

void OW_ProviderMuxLoaderTestCases::testLoadIFCs()
{
	OW_Array<OW_ProviderIFCBaseIFCRef> muxarray;
	OW_ProviderIFCLoaderRef pml = testCreateMuxLoader();
	unitAssertNoThrow( pml->loadIFCs( muxarray ) );
	unitAssert( muxarray.size() == 3 );
	unitAssert( muxarray[0]->getName() == OW_String("lib1") );
}

void OW_ProviderMuxLoaderTestCases::testFailLoadIFCs()
{
	OW_Array<OW_ProviderIFCBaseIFCRef> muxarray;
	testMuxLoaderBad pml( testCreateSharedLibraryLoader() );
	unitAssertNoThrow(pml.loadIFCs( muxarray ));
	unitAssert( muxarray.size() == 0 );
}

void OW_ProviderMuxLoaderTestCases::testLoadCppIFC()
{
	OW_Array<OW_ProviderIFCBaseIFCRef> muxarray;
	g_testEnvironment->setConfigItem(
			OW_ConfigOpts::PROVIDER_IFC_LIBS_opt,
			"../../src/providerifcs/cpp" );
	OW_ProviderIFCLoaderRef pml = OW_ProviderIFCLoader::createProviderIFCLoader(g_testEnvironment);
	unitAssertNoThrow( pml->loadIFCs( muxarray ) );
	cout << "muxarray.size() = " << muxarray.size() << endl;
	unitAssert( muxarray.size() == 1 );
	unitAssert( muxarray[0]->getName() == OW_String("c++") );
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
	// Test doesn't work anymore since the cpp ifc is now a non-loadable ifc.
	//testSuite->addTest (new TestCaller <OW_ProviderMuxLoaderTestCases>
	//		("testLoadCppIFC",
	//		&OW_ProviderMuxLoaderTestCases::testLoadCppIFC));

	return testSuite;
}

