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

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_HTTPChunkerTestCases.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_StringStream.hpp"

using namespace OpenWBEM;

void OW_HTTPChunkerTestCases::setUp()
{
}

void OW_HTTPChunkerTestCases::tearDown()
{
}

void OW_HTTPChunkerTestCases::testChunking()
{
	OStringStream chunkedTarget;
	HTTPChunkedOStream ostr(chunkedTarget);
	ostr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::flush;
	unitAssert( chunkedTarget.toString() ==
			"40\r\n"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n"
			);
}

void OW_HTTPChunkerTestCases::testBigChunking()
{
	char* lotsOfAs = new char[10000];
	memset( lotsOfAs, 'A', 9999 );
	lotsOfAs[9999] = 0;
	OStringStream chunkedTarget;
	HTTPChunkedOStream ostr(chunkedTarget);
	ostr << lotsOfAs << std::flush;
	unitAssert( chunkedTarget.toString().length() == 10022 );
	unitAssert( chunkedTarget.toString()[0] == '1' );
	unitAssert( chunkedTarget.toString().substring(4096 + 8, 4) == "1000" );
	delete[] lotsOfAs;
}

void OW_HTTPChunkerTestCases::testUnChunking()
{
	//unitAssert( something( ) );
}

Test* OW_HTTPChunkerTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_HTTPChunker");

	testSuite->addTest (new TestCaller <OW_HTTPChunkerTestCases>
			("testChunking",
			&OW_HTTPChunkerTestCases::testChunking));
	testSuite->addTest (new TestCaller <OW_HTTPChunkerTestCases>
			("testBigChunking",
			&OW_HTTPChunkerTestCases::testBigChunking));
	testSuite->addTest (new TestCaller <OW_HTTPChunkerTestCases>
			("testUnChunking",
			&OW_HTTPChunkerTestCases::testUnChunking));

	return testSuite;
}

