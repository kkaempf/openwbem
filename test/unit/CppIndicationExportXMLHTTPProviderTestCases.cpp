/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "CppIndicationExportXMLHTTPProviderTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(CppIndicationExportXMLHTTPProviderTestCases,"CppIndicationExportXMLHTTPProvider");
#include "OW_CppIndicationExportXMLHTTPProvider.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include <iostream>

using namespace OpenWBEM;
using namespace std;

CppIndicationExportXMLHTTPProviderTestCases::
~CppIndicationExportXMLHTTPProviderTestCases()
{
}

void CppIndicationExportXMLHTTPProviderTestCases::setUp()
{
}

void CppIndicationExportXMLHTTPProviderTestCases::tearDown()
{
}

void CppIndicationExportXMLHTTPProviderTestCases::testList()
{
	char const * msg =
		"***\n"
		"The unit tests relevant to CppIndicationExportXMLHTTPProvider\n"
		"are in these tests:\n"
		"  IndicationBufferingThread\n"
		"  SendIndicationBurstTask\n"
		"  MTQueue\n"
		"  ConfigureCppIndicationExportXMLHTTPProvider\n"
		"***";
	cout << endl << msg << endl;
}

Test* CppIndicationExportXMLHTTPProviderTestCases::suite()
{
	TestSuite *testSuite =
		new TestSuite ("CppIndicationExportXMLHTTPProvider");

	ADD_TEST_TO_SUITE(CppIndicationExportXMLHTTPProviderTestCases, testList);

	return testSuite;
}

